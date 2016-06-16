/* linux/arch/arm/mach-exynos4/platsmp.c
 *
 * Copyright (c) 2010-2011 Samsung Electronics Co., Ltd.
 *		http://www.samsung.com
 *
 * Cloned from linux/arch/arm/mach-vexpress/platsmp.c
 *
 *  Copyright (C) 2002 ARM Ltd.
 *  All Rights Reserved
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
*/

#include <linux/init.h>
#include <linux/errno.h>
#include <linux/delay.h>
#include <linux/device.h>
#include <linux/jiffies.h>
#include <linux/smp.h>
#include <linux/io.h>

#include <asm/cacheflush.h>
#include <asm/hardware/gic.h>
#include <asm/smp_plat.h>
#include <asm/smp_scu.h>

#include <mach/hardware.h>
#include <mach/regs-clock.h>
#include <mach/regs-pmu.h>
#include <mach/smc.h>

#include <plat/cpu.h>
#include <plat/regs-watchdog.h>

#ifdef CONFIG_SEC_DEBUG
#include <mach/sec_debug.h>     /* for only sec_debug_watchdog_log() */
#endif

extern void exynos4_secondary_startup(void);

struct _cpu_boot_info {
	void __iomem *boot_base;
	void __iomem *power_base;
};

struct _cpu_boot_info cpu_boot_info[NR_CPUS];

/*
 * control for which core is the next to come out of the secondary
 * boot "holding pen"
 */

volatile int pen_release = -1;

/*
 * Write pen_release in a way that is guaranteed to be visible to all
 * observers, irrespective of whether they're taking part in coherency
 * or not.  This is necessary for the hotplug code to work reliably.
 */
static void write_pen_release(int val)
{
	pen_release = val;
	smp_wmb();
	__cpuc_flush_dcache_area((void *)&pen_release, sizeof(pen_release));
	outer_clean_range(__pa(&pen_release), __pa(&pen_release + 1));
}

static void __iomem *scu_base_addr(void)
{
	return (void __iomem *)(S5P_VA_SCU);
}

static DEFINE_SPINLOCK(boot_lock);

void __cpuinit platform_secondary_init(unsigned int cpu)
{
	/* Enable the full line of zero */
	if (soc_is_exynos4210() || soc_is_exynos4212() ||
	    soc_is_exynos4412() || soc_is_exynos4270())
		enable_cache_foz();

	/*
	 * if any interrupts are already enabled for the primary
	 * core (e.g. timer irq), then they will not have been enabled
	 * for us: do so
	 */
	gic_secondary_init(0);

	/*
	 * let the primary processor know we're out of the
	 * pen, then head off into the C entry point
	 */
	write_pen_release(-1);

	clear_boot_flag(cpu, HOTPLUG);

	/*
	 * Synchronise with the boot thread.
	 */
	spin_lock(&boot_lock);
	spin_unlock(&boot_lock);
}

void change_power_base(unsigned int cpu, void __iomem *base)
{
	cpu_boot_info[cpu].power_base = base;
}

static int exynos_power_up_cpu(unsigned int cpu)
{
	unsigned int timeout;
	unsigned int val;
	unsigned int tmp;
	void __iomem *power_base;

	power_base = cpu_boot_info[cpu].power_base;
	if (power_base == 0)
		return -EPERM;

	val = __raw_readl(power_base + 0x4);
	if (!(val & EXYNOS_CORE_LOCAL_PWR_EN)) {
		tmp = __raw_readl(power_base);
		tmp |= (EXYNOS_CORE_LOCAL_PWR_EN);
		tmp |= (EXYNOS_CORE_AUTOWAKEUP_EN);
		__raw_writel(tmp, power_base);

		/* wait max 10 ms until cpu is on */
		timeout = 10;
		while (timeout) {
			val = __raw_readl(power_base + 0x4);

			if ((val & EXYNOS_CORE_LOCAL_PWR_EN) ==
			     EXYNOS_CORE_LOCAL_PWR_EN)
				break;

			mdelay(1);
			timeout--;
		}

		if (timeout == 0) {
			printk(KERN_ERR "cpu%d power up failed", cpu);
			return -ETIMEDOUT;
		}
	}

#ifdef CONFIG_EXYNOS_DM_CPU_HOTPLUG
	/*
	 * Check Power down cpu wait on WFE, and occur SW reset
	 */
	if (soc_is_exynos3470()) {
		while(!__raw_readl(EXYNOS_PMUREG(0x0908)))
			udelay(10);

		udelay(10);

		tmp = __raw_readl(power_base + 0x4);
		tmp |= (0x3 << 8);
		__raw_writel(tmp, power_base + 0x4);

		/* TODO set COREX's WAKEUP_FROM_LOCAL_CFG register with 0x3 */
		printk("cpu%d: SWRESET\n", cpu);
		__raw_writel(((1 << 4) << cpu), EXYNOS_PMUREG(0x0400));
	}
#endif

	return 0;
}

int __cpuinit boot_secondary(unsigned int cpu, struct task_struct *idle)
{
	unsigned long timeout;
	int ret;
#ifdef CONFIG_SEC_WATCHDOG_RESET
	unsigned int tmp_wtcon;
#endif

	/*
	 * Set synchronisation state between this boot processor
	 * and the secondary one
	 */
	spin_lock(&boot_lock);

#ifdef CONFIG_SEC_WATCHDOG_RESET
	tmp_wtcon = __raw_readl(S3C2410_WTCON);
#elif defined(CONFIG_S3C2410_WATCHDOG)
	if (soc_is_exynos5250())
		watchdog_save();
#endif

	ret = exynos_power_up_cpu(cpu);
	if (ret) {
		spin_unlock(&boot_lock);
		return ret;
	}

	/*
	 * The secondary processor is waiting to be released from
	 * the holding pen - release it, then wait for it to flag
	 * that it has been released by resetting pen_release.
	 *
	 * Note that "pen_release" is the hardware CPU ID, whereas
	 * "cpu" is Linux's internal ID.
	 */
	write_pen_release(cpu_logical_map(cpu));

	/*
	 * Send the secondary CPU a soft interrupt, thereby causing
	 * the boot monitor to read the system wide flags register,
	 * and branch to the address found there.
	 */

	timeout = jiffies + (1 * HZ);
	while (time_before(jiffies, timeout)) {
		smp_rmb();

#ifdef CONFIG_ARM_TRUSTZONE
		if (soc_is_exynos4412() || soc_is_exynos4270())
			exynos_smc(SMC_CMD_CPU1BOOT, cpu, 0, 0);
		else if (soc_is_exynos4210() || soc_is_exynos4212())
			exynos_smc(SMC_CMD_CPU1BOOT, 0, 0, 0);
#endif
		__raw_writel(virt_to_phys(exynos4_secondary_startup),
			cpu_boot_info[cpu].boot_base);

#ifdef CONFIG_SEC_WATCHDOG_RESET
		__raw_writel(tmp_wtcon, S3C2410_WTCON);
#elif defined(CONFIG_S3C2410_WATCHDOG)
		if (soc_is_exynos5250())
			watchdog_restore();
#endif

		if (soc_is_exynos3470())
			dsb_sev();
		else
			arm_send_ping_ipi(cpu);

		if (pen_release == -1)
			break;

		udelay(10);
	}

	/*
	 * now the secondary core is starting up let it run its
	 * calibrations, then wait for it to finish
	 */
	spin_unlock(&boot_lock);

	return pen_release != -1 ? -ENOSYS : 0;
}

/*
 * Initialise the CPU possible map early - this describes the CPUs
 * which may be present or become present in the system.
 */

void __init smp_init_cpus(void)
{
	void __iomem *scu_base = scu_base_addr();
	unsigned int i, ncores;

	if (soc_is_exynos4210() || soc_is_exynos4212() ||
	    soc_is_exynos5250() || soc_is_exynos4270())
		ncores = 2;
	else if (soc_is_exynos4412() || soc_is_exynos3470())
		ncores = 4;
	else
		ncores = scu_base ? scu_get_core_count(scu_base) : 1;

	/* sanity check */
	if (ncores > nr_cpu_ids) {
		pr_warn("SMP: %u cores greater than maximum (%u), clipping\n",
			ncores, nr_cpu_ids);
		ncores = nr_cpu_ids;
	}

	for (i = 0; i < ncores; i++)
		set_cpu_possible(i, true);

	set_smp_cross_call(gic_raise_softirq);
}

void __init platform_smp_prepare_cpus(unsigned int max_cpus)
{
	int i;

	if (soc_is_exynos4210() || soc_is_exynos4212() || soc_is_exynos4412() || soc_is_exynos4270())
		scu_enable(scu_base_addr());

	for (i = 1; i < max_cpus; i++) {
		int pwr_offset = 0;

#ifdef CONFIG_ARM_TRUSTZONE
		cpu_boot_info[i].boot_base = S5P_VA_SYSRAM_NS + 0x1C;
#else

		if (soc_is_exynos4210() &&
			(samsung_rev() >= EXYNOS4210_REV_1_1))
			cpu_boot_info[i].boot_base = EXYNOS_INFORM5;
		else
			cpu_boot_info[i].boot_base = S5P_VA_SYSRAM;
#endif
		if (soc_is_exynos4412() || soc_is_exynos4270())
			cpu_boot_info[i].boot_base += (0x4 * i);

		cpu_boot_info[i].power_base =
			EXYNOS_ARM_CORE_CONFIGURATION(i + pwr_offset);
	}
}
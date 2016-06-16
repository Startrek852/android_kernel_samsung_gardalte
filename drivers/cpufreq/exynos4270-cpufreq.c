/*
 * Copyright (c) 2010-2012 Samsung Electronics Co., Ltd.
 *		http://www.samsung.com
 *
 * EXYNOS4X12 - CPU frequency scaling support
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 */

#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/err.h>
#include <linux/clk.h>
#include <linux/io.h>
#include <linux/slab.h>
#include <linux/cpufreq.h>

#include <mach/asv-exynos.h>
#include <mach/regs-clock.h>
#include <mach/cpufreq.h>

#define CPUFREQ_LEVEL_END	(L13 + 1)

static int max_support_idx;
static int min_support_idx = (CPUFREQ_LEVEL_END - 1);

static struct clk *cpu_clk;
static struct clk *moutcore;
static struct clk *mout_mpll;
static struct clk *mout_apll;

struct cpufreq_clkdiv {
	unsigned int	index;
	unsigned int	clkdiv;
	unsigned int	clkdiv1;
};

static unsigned int exynos4270_volt_table[CPUFREQ_LEVEL_END];

static struct cpufreq_frequency_table exynos4270_freq_table[] = {
	{L0, 1500 * 1000},
	{L1, 1400 * 1000},
	{L2, 1300 * 1000},
	{L3, 1200 * 1000},
	{L4, 1100 * 1000},
	{L5, 1000 * 1000},
	{L6,  900 * 1000},
	{L7,  800 * 1000},
	{L8,  700 * 1000},
	{L9,  600 * 1000},
	{L10, 500 * 1000},
	{L11, 400 * 1000},
	{L12, 300 * 1000},
	{L13, 200 * 1000},
	{0, CPUFREQ_TABLE_END},
};

static struct cpufreq_clkdiv exynos4270_clkdiv_table[CPUFREQ_LEVEL_END];

static unsigned int clkdiv_cpu0_4270[CPUFREQ_LEVEL_END][8] = {
	/*
	 * Clock divider value for following
	 * { DIVCORE, DIVCOREM0, DIVCOREM1,
	 *		DIVATB, DIVPCLK_DBG, DIVAPLL, DIVCORE2 }
	 */
	/* ARM L0: 1500Mhz */
	{ 0, 3, 7, 6, 1, 2, 0 },

	/* ARM L1: 1400Mhz */
	{ 0, 3, 7, 6, 1, 2, 0 },

	/* ARM L2: 1300Mhz */
	{ 0, 3, 7, 5, 1, 2, 0 },

	/* ARM L3: 1200Mhz */
	{ 0, 3, 7, 5, 1, 1, 0 },

	/* ARM L4: 1100Mhz */
	{ 0, 3, 6, 4, 1, 1, 0 },

	/* ARM L5: 1000MHz */
	{ 0, 2, 5, 4, 1, 1, 0 },

	/* ARM L6: 900MHz */
	{ 0, 2, 5, 3, 1, 1, 0 },

	/* ARM L7: 800MHz */
	{ 0, 2, 5, 3, 1, 1, 0 },

	/* ARM L8: 700MHz */
	{ 0, 2, 4, 3, 1, 1, 0 },

	/* ARM L9: 600MHz */
	{ 0, 2, 4, 3, 1, 1, 0 },

	/* ARM L10: 500MHz */
	{ 0, 2, 4, 3, 1, 1, 0 },

	/* ARM L11: 400MHz */
	{ 0, 2, 4, 3, 1, 1, 0 },

	/* ARM L12: 300MHz */
	{ 0, 2, 4, 2, 1, 1, 0 },

	/* ARM L13: 200MHz */
	{ 0, 1, 3, 1, 1, 1, 0 },
};

static unsigned int clkdiv_cpu1_4270[CPUFREQ_LEVEL_END][3] = {
	/*
	 *Clock divider value for following
	 * { DIVCOPY, DIVHPM, DIVCORES }
	 */
	/* ARM L0: 1500MHz */
	{ 6, 0, 7 },

	/* ARM L1: 1400MHz */
	{ 6, 0, 6 },

	/* ARM L2: 1300MHz */
	{ 5, 0, 6 },

	/* ARM L3: 1200MHz */
	{ 5, 0, 5 },

	/* ARM L4: 1100MHz */
	{ 4, 0, 5 },

	/* ARM L5: 1000MHz */
	{ 4, 0, 4 },

	/* ARM L6: 900MHz */
	{ 3, 0, 4 },

	/* ARM L7: 800MHz */
	{ 3, 0, 3 },

	/* ARM L8: 700MHz */
	{ 3, 0, 3 },

	/* ARM L9: 600MHz */
	{ 3, 0, 2 },

	/* ARM L10: 500MHz */
	{ 3, 0, 2 },

	/* ARM L11: 400MHz */
	{ 3, 0, 1 },

	/* ARM L12: 300MHz */
	{ 3, 0, 1 },

	/* ARM L13: 200MHz */
	{ 3, 0, 0 },
};

static unsigned int exynos4270_apll_pms_table[CPUFREQ_LEVEL_END] = {
	/* APLL FOUT L0: 1500MHz */
	((250<<16)|(4<<8)|(0x0)),

	/* APLL FOUT L1: 1400MHz */
	((175<<16)|(3<<8)|(0x0)),

	/* APLL FOUT L2: 1300MHz */
	((325<<16)|(6<<8)|(0x0)),

	/* APLL FOUT L3: 1200MHz */
	((400<<16)|(4<<8)|(0x1)),

	/* APLL FOUT L4: 1100MHz */
	((275<<16)|(3<<8)|(0x1)),

	/* APLL FOUT L5: 1000MHz */
	((250<<16)|(3<<8)|(0x1)),

	/* APLL FOUT L6: 900MHz */
	((300<<16)|(4<<8)|(0x1)),

	/* APLL FOUT L7: 800MHz */
	((200<<16)|(3<<8)|(0x1)),

	/* APLL FOUT L8: 700MHz */
	((175<<16)|(3<<8)|(0x1)),

	/* APLL FOUT L9: 600MHz */
	((400<<16)|(4<<8)|(0x2)),

	/* APLL FOUT L10: 500MHz */
	((250<<16)|(3<<8)|(0x2)),

	/* APLL FOUT L11 400MHz */
	((200<<16)|(3<<8)|(0x2)),

	/* APLL FOUT L12: 300MHz */
	((400<<16)|(4<<8)|(0x3)),

	/* APLL FOUT L13: 200MHz */
	((200<<16)|(3<<8)|(0x3)),

};

static const unsigned int asv_voltage_4270[CPUFREQ_LEVEL_END] = {
	1175000,
	1125000,
	1075000,
	1050000,
	1025000,
	1000000,
	 975000,
	 950000,
	 925000,
	 900000,
	 900000,
	 900000,
	 900000,
	 900000,
};

static void exynos4270_set_clkdiv(unsigned int div_index)
{
	unsigned int tmp;

	/* Change Divider - CPU0 */
	tmp = exynos4270_clkdiv_table[div_index].clkdiv;

	__raw_writel(tmp, EXYNOS4_CLKDIV_CPU);

	while (__raw_readl(EXYNOS4_CLKDIV_STATCPU) & 0x11111111)
		cpu_relax();

	/* Change Divider - CPU1 */
	tmp = exynos4270_clkdiv_table[div_index].clkdiv1;

	__raw_writel(tmp, EXYNOS4_CLKDIV_CPU1);

	while (__raw_readl(EXYNOS4_CLKDIV_STATCPU1) & 0x111)
		cpu_relax();
}

static void exynos4270_set_apll(unsigned int index)
{
	unsigned int tmp, pdiv;

	/* 1. MUX_CORE_SEL = MPLL, ARMCLK uses MPLL for lock time */
	clk_set_parent(moutcore, mout_mpll);

	do {
		cpu_relax();
		tmp = (__raw_readl(EXYNOS4_CLKMUX_STATCPU)
			>> EXYNOS4_CLKSRC_CPU_MUXCORE_SHIFT);
		tmp &= 0x7;
	} while (tmp != 0x2);

	/* 2. Set APLL Lock time */
	pdiv = ((exynos4270_apll_pms_table[index] >> 8) & 0x3f);

	__raw_writel((pdiv * 270), EXYNOS4_APLL_LOCK);

	/* 3. Change PLL PMS values */
	tmp = __raw_readl(EXYNOS4_APLL_CON0);
	tmp &= ~((0x3ff << 16) | (0x3f << 8) | (0x7 << 0));
	tmp |= exynos4270_apll_pms_table[index];
	__raw_writel(tmp, EXYNOS4_APLL_CON0);

	/* 4. wait_lock_time */
	do {
		cpu_relax();
		tmp = __raw_readl(EXYNOS4_APLL_CON0);
	} while (!(tmp & (0x1 << EXYNOS4_APLLCON0_LOCKED_SHIFT)));

	/* 5. MUX_CORE_SEL = APLL */
	clk_set_parent(moutcore, mout_apll);

	do {
		cpu_relax();
		tmp = __raw_readl(EXYNOS4_CLKMUX_STATCPU);
		tmp &= EXYNOS4_CLKMUX_STATCPU_MUXCORE_MASK;
	} while (tmp != (0x1 << EXYNOS4_CLKSRC_CPU_MUXCORE_SHIFT));
}

bool exynos4270_pms_change(unsigned int old_index, unsigned int new_index)
{
	unsigned int old_pm = exynos4270_apll_pms_table[old_index] >> 8;
	unsigned int new_pm = exynos4270_apll_pms_table[new_index] >> 8;

	return (old_pm == new_pm) ? 0 : 1;
}

static void exynos4270_set_frequency(unsigned int old_index,
				  unsigned int new_index)
{
	unsigned int tmp;

	if (old_index > new_index) {
		if (!exynos4270_pms_change(old_index, new_index)) {
			/* 1. Change the system clock divider values */
			exynos4270_set_clkdiv(new_index);
			/* 2. Change just s value in apll m,p,s value */
			tmp = __raw_readl(EXYNOS4_APLL_CON0);
			tmp &= ~(0x7 << 0);
			tmp |= (exynos4270_apll_pms_table[new_index] & 0x7);
			__raw_writel(tmp, EXYNOS4_APLL_CON0);
		} else {
			/* Clock Configuration Procedure */
			/* 1. Change the system clock divider values */
			exynos4270_set_clkdiv(new_index);
			/* 2. Change the apll m,p,s value */
			exynos4270_set_apll(new_index);
		}
	} else if (old_index < new_index) {
		if (!exynos4270_pms_change(old_index, new_index)) {
			/* 1. Change just s value in apll m,p,s value */
			tmp = __raw_readl(EXYNOS4_APLL_CON0);
			tmp &= ~(0x7 << 0);
			tmp |= (exynos4270_apll_pms_table[new_index] & 0x7);
			__raw_writel(tmp, EXYNOS4_APLL_CON0);
			/* 2. Change the system clock divider values */
			exynos4270_set_clkdiv(new_index);
		} else {
			/* Clock Configuration Procedure */
			/* 1. Change the apll m,p,s value */
			exynos4270_set_apll(new_index);
			/* 2. Change the system clock divider values */
			exynos4270_set_clkdiv(new_index);
		}
	}
}

static void __init set_volt_table(void)
{
	unsigned int i;

	for (i = 0; i < CPUFREQ_LEVEL_END; i++) {
		exynos4270_volt_table[i] = asv_voltage_4270[i];
		pr_info("CPUFREQ L%d : %d uV\n", i, exynos4270_volt_table[i]);
	}
}

int exynos4270_cpufreq_init(struct exynos_dvfs_info *info)
{
	int i;
	unsigned int tmp;
	unsigned long rate;

	set_volt_table();

	cpu_clk = clk_get(NULL, "armclk");
	if (IS_ERR(cpu_clk))
		return PTR_ERR(cpu_clk);

	moutcore = clk_get(NULL, "moutcore");
	if (IS_ERR(moutcore))
		goto err_moutcore;

	mout_mpll = clk_get(NULL, "mout_mpll");
	if (IS_ERR(mout_mpll))
		goto err_mout_mpll;

	rate = clk_get_rate(mout_mpll) / 1000;

	mout_apll = clk_get(NULL, "mout_apll");
	if (IS_ERR(mout_apll))
		goto err_mout_apll;

	for (i = L0; i <  CPUFREQ_LEVEL_END; i++) {
		exynos4270_clkdiv_table[i].index = i;

		tmp = __raw_readl(EXYNOS4_CLKDIV_CPU);

		tmp &= ~(EXYNOS4_CLKDIV_CPU0_CORE_MASK |
			EXYNOS4_CLKDIV_CPU0_COREM0_MASK |
			EXYNOS4_CLKDIV_CPU0_COREM1_MASK |
			EXYNOS4_CLKDIV_CPU0_ATB_MASK |
			EXYNOS4_CLKDIV_CPU0_PCLKDBG_MASK |
			EXYNOS4_CLKDIV_CPU0_APLL_MASK |
			EXYNOS4_CLKDIV_CPU0_CORE2_MASK);

		tmp |= ((clkdiv_cpu0_4270[i][0] << EXYNOS4_CLKDIV_CPU0_CORE_SHIFT) |
			(clkdiv_cpu0_4270[i][1] << EXYNOS4_CLKDIV_CPU0_COREM0_SHIFT) |
			(clkdiv_cpu0_4270[i][2] << EXYNOS4_CLKDIV_CPU0_COREM1_SHIFT) |
			(clkdiv_cpu0_4270[i][4] << EXYNOS4_CLKDIV_CPU0_ATB_SHIFT) |
			(clkdiv_cpu0_4270[i][5] << EXYNOS4_CLKDIV_CPU0_PCLKDBG_SHIFT) |
			(clkdiv_cpu0_4270[i][6] << EXYNOS4_CLKDIV_CPU0_APLL_SHIFT) |
			(clkdiv_cpu0_4270[i][7] << EXYNOS4_CLKDIV_CPU0_CORE2_SHIFT));

		exynos4270_clkdiv_table[i].clkdiv = tmp;

		tmp = __raw_readl(EXYNOS4_CLKDIV_CPU1);

		tmp &= ~(EXYNOS4_CLKDIV_CPU1_COPY_MASK |
			EXYNOS4_CLKDIV_CPU1_HPM_MASK |
			EXYNOS4_CLKDIV_CPU1_CORES_MASK);
		tmp |= ((clkdiv_cpu1_4270[i][0] << EXYNOS4_CLKDIV_CPU1_COPY_SHIFT) |
			(clkdiv_cpu1_4270[i][1] << EXYNOS4_CLKDIV_CPU1_HPM_SHIFT) |
			(clkdiv_cpu1_4270[i][2] << EXYNOS4_CLKDIV_CPU1_CORES_SHIFT));
		exynos4270_clkdiv_table[i].clkdiv1 = tmp;
	}

	info->mpll_freq_khz = rate;
	info->pm_lock_idx = L6;
	info->pll_safe_idx = L8;
	info->max_support_idx = max_support_idx;
	info->min_support_idx = min_support_idx;
	info->cpu_clk = cpu_clk;
	info->volt_table = exynos4270_volt_table;
	info->freq_table = exynos4270_freq_table;
	info->set_freq = exynos4270_set_frequency;
	info->need_apll_change = exynos4270_pms_change;
	return 0;

err_mout_apll:
	clk_put(mout_mpll);
err_mout_mpll:
	clk_put(moutcore);
err_moutcore:
	clk_put(cpu_clk);

	pr_debug("%s: failed initialization\n", __func__);
	return -EINVAL;
}
EXPORT_SYMBOL(exynos4270_cpufreq_init);

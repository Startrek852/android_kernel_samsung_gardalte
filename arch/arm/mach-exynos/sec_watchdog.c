/* sec_watchdog.c copied from herring-watchdog.c
 *
 * Copyright (C) 2010 Google, Inc.
 *
 * This software is licensed under the terms of the GNU General Public
 * License version 2, as published by the Free Software Foundation, and
 * may be copied, distributed, and modified under those terms.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 */

#include <plat/regs-watchdog.h>
#include <plat/watchdog.h>
#include <mach/map.h>

#include <linux/module.h>
#include <linux/moduleparam.h>
#include <linux/init.h>
#include <linux/platform_device.h>
#include <linux/pm.h>
#include <linux/cpufreq.h>
#include <linux/err.h>
#include <linux/irq.h>
#include <linux/interrupt.h>
#include <linux/workqueue.h>
#include <linux/io.h>
#include <linux/clk.h>
#include <linux/cpu.h>

#ifdef CONFIG_SEC_DEBUG
#include <mach/sec_debug.h>
#endif

/* PCLK(=PERIR=ACLK_100)/256/128 (~3200:1s) */
#define TPS 3200
#if defined(CONFIG_MACH_P4) && defined(CONFIG_TARGET_LOCALE_USA)
#define PET_BY_WORKQUEUE
#else
#define PET_BY_DIRECT_TIMER
#endif

/* reset timeout in seconds */
static unsigned watchdog_reset = 20;
module_param_named(sec_reset, watchdog_reset, uint, 0644);

/* pet timeout in seconds
 * 0 means off, 5 is proper */
static unsigned watchdog_pet = CONFIG_SEC_WATCHDOG_PET_TIME;
module_param_named(sec_pet, watchdog_pet, uint, 0644);

#if defined(PET_BY_WORKQUEUE)
static struct workqueue_struct *watchdog_wq;
static void watchdog_workfunc(struct work_struct *work);
static DECLARE_DELAYED_WORK(watchdog_work, watchdog_workfunc);
#elif defined(PET_BY_DIRECT_TIMER)
static struct timer_list pet_watchdog_timer;
static void pet_watchdog_timer_fn(unsigned long data);
#else
static struct hrtimer watchdog_timer;
static enum hrtimer_restart watchdog_timerfunc(struct hrtimer *timer);
#endif

static struct clk *wd_clk;
static spinlock_t wdt_lock;

#if defined(PET_BY_WORKQUEUE)
static void watchdog_workfunc(struct work_struct *work)
{
	pr_info("%s kicking...%x\n", __func__, readl(S3C2410_WTCNT));
	writel(watchdog_reset * TPS, S3C2410_WTCNT);
	queue_delayed_work_on(0, watchdog_wq, &watchdog_work,
			      watchdog_pet * HZ);
}
#elif defined(PET_BY_DIRECT_TIMER)
static void pet_watchdog_timer_fn(unsigned long data)
{
	unsigned long freq = clk_get_rate(wd_clk);

	pr_info("%s kicking...CNT: %x, DAT: %x, CON: %x\n",
			__func__, readl(S3C2410_WTCNT),
			readl(S3C2410_WTDAT), readl(S3C2410_WTCON));
	writel(watchdog_reset * TPS, S3C2410_WTCNT);
	mod_timer(&pet_watchdog_timer, jiffies + watchdog_pet * HZ);
	pr_info("%s checking..CNT: %x, DAT: %x, CON: %x\n",
			__func__, readl(S3C2410_WTCNT),
			readl(S3C2410_WTDAT), readl(S3C2410_WTCON));
}
#else
static enum hrtimer_restart watchdog_timerfunc(struct hrtimer *timer)
{
	unsigned long freq = clk_get_rate(wd_clk);

	pr_info("%s kicking...%x, freq: %lu\n", __func__, readl(S3C2410_WTCNT),
			freq);
	writel(watchdog_reset * TPS, S3C2410_WTCNT);
	hrtimer_start(&watchdog_timer,
		      ktime_set(watchdog_pet, 0), HRTIMER_MODE_REL);
	return HRTIMER_NORESTART;
}
#endif

static void watchdog_start(void)
{
	unsigned int val;
	unsigned long flags;
	unsigned long freq = clk_get_rate(wd_clk);

	spin_lock_irqsave(&wdt_lock, flags);

	pr_info("%s: watchdog source freq= %lu\n", __func__, freq);
	/* set to PCLK / 256 / 128 */
	val = S3C2410_WTCON_DIV128;
	val |= S3C2410_WTCON_PRESCALE(255);
	writel(val, S3C2410_WTCON);

	/* program initial count */
	writel(watchdog_reset * TPS, S3C2410_WTCNT);
	writel(watchdog_reset * TPS, S3C2410_WTDAT);

	/* start timer */
	val |= S3C2410_WTCON_RSTEN | S3C2410_WTCON_ENABLE;
	writel(val, S3C2410_WTCON);
	spin_unlock_irqrestore(&wdt_lock, flags);

	/* make sure we're ready to pet the dog */
#if defined(PET_BY_WORKQUEUE)
	queue_delayed_work_on(0, watchdog_wq, &watchdog_work,
			      watchdog_pet * HZ);
#elif defined(PET_BY_DIRECT_TIMER)
	pet_watchdog_timer.expires = jiffies + watchdog_pet * HZ;
	add_timer_on(&pet_watchdog_timer, 0);
#else
	hrtimer_start(&watchdog_timer,
		      ktime_set(watchdog_pet, 0), HRTIMER_MODE_REL);
#endif
	printk(KERN_ERR "watchdog_start() from watchdog_resume\n");
}

static void watchdog_stop(void)
{
	writel(0, S3C2410_WTCON);
#if defined(PET_BY_WORKQUEUE)
	/* do nothing? */
#elif defined(PET_BY_DIRECT_TIMER)
	del_timer(&pet_watchdog_timer);
#else
	hrtimer_cancel(&watchdog_timer);
#endif
	printk(KERN_ERR "watchdog_stop() from watchdog_suspend\n");
}

static int watchdog_probe(struct platform_device *pdev)
{
	struct s3c_watchdog_platdata *pdata;

	printk(KERN_ERR "sec_Watchdog probe start\n");
	wd_clk = clk_get(NULL, "watchdog");
	BUG_ON(!wd_clk);
	clk_enable(wd_clk);

	spin_lock_init(&wdt_lock);

	/* watchdog can be disabled by providing either
	 * "exynos4210_watchdog.sec_pet=0" or
	 * "exynos4210_watchdog.sec_reset=0" to CMDLINE */
	printk(KERN_ERR "sec_watchdog_reset =%d, watchdog_pet=%d\n",
		 watchdog_reset, watchdog_pet);
	if (!watchdog_reset || !watchdog_pet) {
		clk_disable(wd_clk);
		return -ENODEV;
	}

#if defined(PET_BY_WORKQUEUE)
	watchdog_wq = create_singlethread_workqueue("pet_watchdog");
	watchdog_start();
#elif defined(PET_BY_DIRECT_TIMER)
	init_timer(&pet_watchdog_timer);
	pet_watchdog_timer.function = pet_watchdog_timer_fn;
	watchdog_start();
#else
	hrtimer_init(&watchdog_timer, CLOCK_MONOTONIC, HRTIMER_MODE_REL);
	watchdog_timer.function = watchdog_timerfunc;
	watchdog_start();
#endif

	pdata = dev_get_platdata(&pdev->dev);
	/* Enable pmu watchdog reset control */
	if (pdata != NULL && pdata->pmu_wdt_control != NULL) {
		pr_info("%s: pmu_wdt_control() will be run\n", __func__);
		pdata->pmu_wdt_control(1, pdata->pmu_wdt_reset_type);
	}

	printk(KERN_ERR "sec_Watchdog probe complete\n");
	return 0;
}

static int watchdog_suspend(struct device *dev)
{
	struct s3c_watchdog_platdata *pdata;
	struct platform_device *pdev = to_platform_device(dev);
	pdata = dev_get_platdata(&pdev->dev);

	watchdog_stop();

	/* Disable pmu watchdog reset control */
	if (pdata != NULL && pdata->pmu_wdt_control != NULL)
		pdata->pmu_wdt_control(0, pdata->pmu_wdt_reset_type);

	return 0;
}

static int watchdog_resume(struct device *dev)
{
	struct s3c_watchdog_platdata *pdata;
	struct platform_device *pdev = to_platform_device(dev);
	pdata = dev_get_platdata(&pdev->dev);

	/* Enable pmu watchdog reset control */
	if (pdata != NULL && pdata->pmu_wdt_control != NULL)
		pdata->pmu_wdt_control(1, pdata->pmu_wdt_reset_type);

	watchdog_start();
	return 0;
}

static const struct dev_pm_ops watchdog_pm_ops = {
	.suspend_noirq = watchdog_suspend,
	.resume_noirq = watchdog_resume,
};

static struct platform_driver watchdog_driver = {
	.probe = watchdog_probe,
	.driver = {
		   .owner = THIS_MODULE,
		   .name = "watchdog-reset",
		   .pm = &watchdog_pm_ops,
	},
};

static int __init watchdog_init(void)
{
	return platform_driver_register(&watchdog_driver);
}

module_init(watchdog_init);
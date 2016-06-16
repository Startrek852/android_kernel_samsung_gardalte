/* linux/arch/arm/mach-exynos/include/mach/cpufreq.h
 *
 * Copyright (c) 2010 Samsung Electronics Co., Ltd.
 *		http://www.samsung.com
 *
 * EXYNOS - CPUFreq support
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
*/

enum cpufreq_level_index {
	L0, L1, L2, L3, L4,
	L5, L6, L7, L8, L9,
	L10, L11, L12, L13, L14,
	L15, L16, L17, L18, L19,
	L20,
};

struct exynos_dvfs_info {
	unsigned long	mpll_freq_khz;
	unsigned int	pll_safe_idx;
	unsigned int	pm_lock_idx;
	unsigned int	max_support_idx;
	unsigned int	min_support_idx;
	unsigned int	cluster_num;
	unsigned int	boot_freq;
	bool		blocked;
	struct clk	*cpu_clk;
	unsigned int	*volt_table;
	unsigned int	*abb_table;
	const unsigned int	*max_op_freqs;
	struct cpufreq_frequency_table	*freq_table;
	struct regulator *regulator;
	void (*set_freq)(unsigned int, unsigned int);
	bool (*need_apll_change)(unsigned int, unsigned int);
};

#if defined(CONFIG_ARCH_EXYNOS4)
extern int exynos3470_cpufreq_init(struct exynos_dvfs_info *);
extern int exynos4210_cpufreq_init(struct exynos_dvfs_info *);
extern int exynos4x12_cpufreq_init(struct exynos_dvfs_info *);
extern int exynos4270_cpufreq_init(struct exynos_dvfs_info *);
static inline int exynos5250_cpufreq_init(struct exynos_dvfs_info *info)
{
	return 0;
}
extern struct mutex cpufreq_lock;
#elif defined(CONFIG_ARCH_EXYNOS5)
static inline int exynos4210_cpufreq_init(struct exynos_dvfs_info *info)
{
	return 0;
}

static inline int exynos4x12_cpufreq_init(struct exynos_dvfs_info *info)
{
	return 0;
}

static inline int exynos4270_cpufreq_init(struct exynos_dvfs_info *info)
{
	return 0;
}
extern int exynos5250_cpufreq_init(struct exynos_dvfs_info *);
#else
	#warning "Should define CONFIG_ARCH_EXYNOS4(5)\n"
#endif
extern void exynos_thermal_throttle(void);
extern void exynos_thermal_unthrottle(void);

#ifdef CONFIG_EXYNOS_DM_CPU_HOTPLUG
extern void dm_cpu_hotplug_init(void);
#endif

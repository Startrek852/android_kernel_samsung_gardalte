/*
 * Copyright (c) 2011-2012 Samsung Electronics Co., Ltd.
 *		http://www.samsung.com
 *
 * Header file for exynos4 clock support
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
*/

#ifndef __ASM_ARCH_CLOCK_EXYNOS4_H
#define __ASM_ARCH_CLOCK_EXYNOS4_H __FILE__

#include <linux/clk.h>

extern struct clksrc_clk exynos4_clk_aclk_133;
extern struct clksrc_clk exynos4_clk_mout_mpll;

extern struct clksrc_sources exynos4_clkset_mout_corebus;
extern struct clksrc_sources exynos4_clkset_group;

extern struct clk *exynos4_clkset_vpllsrc_list[];
extern struct clk *exynos4_clkset_aclk_top_list[];
extern struct clk *exynos4_clkset_group_list[];
extern struct clksrc_clk exynos4_clk_core2clk;
extern struct clksrc_clk exynos4_clk_mout_corebus;
extern struct clksrc_clk exynos4_clk_coreclk;
extern struct clksrc_clk exynos4_clk_armclk;
extern struct clksrc_clk exynos4_clk_mout_epll;

extern struct clksrc_clk exynos4_clk_aclk_200;
extern struct clksrc_clk exynos4_clk_aclk_100;
#if defined(CONFIG_SOC_EXYNOS4212) || defined(CONFIG_SOC_EXYNOS4412)
extern struct clksrc_clk exynos4212_clk_aclk_400_mcuisp;
extern struct clksrc_clk exynos4212_clk_aclk_266;
#endif

extern int exynos4_clksrc_mask_fsys_ctrl(struct clk *clk, int enable);
extern int exynos4_clk_ip_fsys_ctrl(struct clk *clk, int enable);
extern int exynos4_clk_ip_peril_ctrl(struct clk *clk, int enable);
extern int exynos4_clk_ip_lcd1_ctrl(struct clk *clk, int enable);
extern int exynos4_clk_ip_dmc_ctrl(struct clk *clk, int enable);
extern int exynos4_clk_ip_cam_ctrl(struct clk *clk, int enable);

#if defined(CONFIG_SOC_EXYNOS3470)
extern struct clksrc_clk exynos4_clk_sclk_vpll;
extern struct clksrc_clk exynos3470_clk_mout_mpll_user_top;
extern struct clksrc_clk exynos3470_clk_aclk_400_mcuisp;
extern struct clksrc_clk exynos3470_clk_aclk_266;
#endif

#if defined(CONFIG_SOC_EXYNOS4270)
extern struct clksrc_sources exynos4_clk_src_apll;
extern struct clksrc_sources exynos4_clk_src_bpll;
extern struct clksrc_sources exynos4_clk_src_epll;
extern struct clksrc_sources exynos4_clk_src_vpll;
extern struct clksrc_sources exynos4_clk_src_mpll;
extern struct clksrc_clk exynos4270_clk_aclk_400_mcuisp;
extern struct clksrc_clk exynos4270_clk_aclk_266;
#endif

#endif /* __ASM_ARCH_CLOCK_EXYNOS4_H */

/*
 * Copyright (c) 2011-2012 Samsung Electronics Co., Ltd.
 *              http://www.samsung.com
 *
 * EXYNOS4212 - Clock support
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
*/

#include <linux/kernel.h>
#include <linux/err.h>
#include <linux/clk.h>
#include <linux/io.h>
#include <linux/syscore_ops.h>

#include <plat/cpu-freq.h>
#include <plat/clock.h>
#include <plat/cpu.h>
#include <plat/pll.h>
#include <plat/s5p-clock.h>
#include <plat/clock-clksrc.h>
#include <plat/pm.h>

#include <mach/hardware.h>
#include <mach/map.h>
#include <mach/regs-clock.h>
#include <mach/exynos-clock.h>
#include <mach/exynos-fimc-is.h>
#include <mach/sysmmu.h>

#include "common.h"
#include "clock-exynos4.h"

#ifdef CONFIG_PM_SLEEP
static struct sleep_save exynos3470_clock_save[] = {
	SAVE_ITEM(EXYNOS4270_CLKDIV_DMC1),
	SAVE_ITEM(EXYNOS4270_CLKDIV_ACP0),
	SAVE_ITEM(EXYNOS4270_CLKSRC_ACP),
	SAVE_ITEM(EXYNOS4270_CLKSRC_DMC),
	SAVE_ITEM(EXYNOS4270_CLKGATE_IP_DMC0),
	SAVE_ITEM(EXYNOS4270_CLKGATE_IP_ACP0),
	SAVE_ITEM(EXYNOS4270_CLKSRC_MASK_ACP),
};
#endif

static int exynos3470_clk_ip_acp_ctrl(struct clk *clk, int enable)
{
	return s5p_gatectrl(EXYNOS4270_CLKGATE_IP_ACP0, clk, enable);
}

static int exynos3470_clk_ip_isp0_ctrl(struct clk *clk, int enable)
{
	return s5p_gatectrl(EXYNOS4_CLKGATE_IP_ISP0, clk, enable);
}

static int exynos3470_clk_ip_isp1_ctrl(struct clk *clk, int enable)
{
	return s5p_gatectrl(EXYNOS4_CLKGATE_IP_ISP1, clk, enable);
}

static int exynos3470_clk_ip_isp_ctrl(struct clk *clk, int enable)
{
	return s5p_gatectrl(EXYNOS4_CLKGATE_IP_ISP, clk, enable);
}

static int exynos3470_clk_ip_acp0_ctrl(struct clk *clk, int enable)
{
	return s5p_gatectrl(EXYNOS4270_CLKGATE_IP_ACP0, clk, enable);
}

static int exynos3470_clk_ip_dmc0_ctrl(struct clk *clk, int enable)
{
	return s5p_gatectrl(EXYNOS4270_CLKGATE_IP_DMC0, clk, enable);
}

/* APLL, BPLL, EPLL, VPLL, MPLL source select function */
static struct clk *exynos3470_clk_src_bpll_list[] = {
	[0] = &clk_fin_bpll,
	[1] = &clk_fout_bpll,
};

static struct clksrc_sources exynos3470_clk_src_bpll = {
	.sources	= exynos3470_clk_src_bpll_list,
	.nr_sources	= ARRAY_SIZE(exynos3470_clk_src_bpll_list),
};

static struct clksrc_clk exynos3470_clk_mout_bpll = {
	.clk	= {
		.name		= "mout_bpll",
		.parent		= &clk_fout_bpll,
	},
	.sources = &exynos3470_clk_src_bpll,
	.reg_div = { .reg = EXYNOS4270_CLKSRC_DMC, .shift = 10, .size = 1 },
};

static struct clksrc_clk exynos3470_clk_sclk_mpll = {
	.clk	= {
		.name		= "sclk_mpll",
		.parent		= &exynos4_clk_mout_mpll.clk,
	},
	.reg_div = { .reg = EXYNOS4270_CLKDIV_DMC1, .shift = 8, .size = 2 },
};

static struct clk *clk_src_mpll_user_list[] = {
	[0] = &clk_fin_mpll,
	[1] = &exynos4_clk_mout_mpll.clk,
};

static struct clksrc_sources clk_src_mpll_user = {
	.sources	= clk_src_mpll_user_list,
	.nr_sources	= ARRAY_SIZE(clk_src_mpll_user_list),
};

static struct clksrc_clk clk_mout_mpll_user = {
	.clk	= {
		.name		= "mout_mpll_user",
	},
	.sources	= &clk_src_mpll_user,
	.reg_src	= { .reg = EXYNOS4_CLKSRC_CPU, .shift = 24, .size = 1 },
};

struct clksrc_clk exynos3470_clk_mout_mpll_user_top = {
	.clk	= {
		.name		= "mout_mpll_user_top",
	},
	.sources	= &clk_src_mpll_user,
	.reg_src	= { .reg = EXYNOS4_CLKSRC_TOP1, .shift = 12, .size = 1 },
};

static struct clksrc_clk exynos3470_clk_atclk = {
	.clk	= {
		.name		= "atclk",
		.parent		= &exynos4_clk_armclk.clk,
	},
	.reg_div = { .reg = EXYNOS4_CLKDIV_CPU, .shift = 16, .size = 3 },
};

static struct clksrc_clk exynos3470_clk_pclk_dbg = {
	.clk	= {
		.name		= "pclk_dbg",
		.parent		= &exynos4_clk_armclk.clk,
	},
	.reg_div = { .reg = EXYNOS4_CLKDIV_CPU, .shift = 20, .size = 3 },
};

static struct clksrc_clk exynos3470_clk_sclk_dmc_pre = {
	.clk	= {
		.name		= "sclk_dmc_pre",
		.parent		= &exynos4_clk_mout_corebus.clk,
	},
	.reg_div = { .reg = EXYNOS4270_CLKDIV_DMC1, .shift = 19, .size = 2 },
};

static struct clksrc_clk exynos3470_clk_sclk_dmc = {
	.clk	= {
		.name		= "sclk_dmc",
		.parent		= &exynos3470_clk_sclk_dmc_pre.clk,
	},
	.reg_div = { .reg = EXYNOS4270_CLKDIV_DMC1, .shift = 27, .size = 3 },
};

static struct clksrc_clk exynos3470_clk_aclk_cored = {
	.clk	= {
		.name		= "aclk_cored",
		.parent		= &exynos3470_clk_sclk_dmc.clk,
	},
	.reg_div = { .reg = EXYNOS4270_CLKDIV_DMC1, .shift = 11, .size = 3 },
};

static struct clksrc_clk exynos3470_clk_aclk_corep = {
	.clk	= {
		.name		= "aclk_corep",
		.parent		= &exynos3470_clk_aclk_cored.clk,
	},
	.reg_div = { .reg = EXYNOS4270_CLKDIV_DMC1, .shift = 15, .size = 3 },
};

static struct clk *exynos3470_clkset_sclk_mpll_user_acp_list[] = {
	[0] = &clk_fin_mpll,
	[1] = &exynos4_clk_mout_mpll.clk,
};

static struct clksrc_sources exynos3470_clkset_sclk_mpll_user_acp = {
	.sources	= exynos3470_clkset_sclk_mpll_user_acp_list,
	.nr_sources	= ARRAY_SIZE(exynos3470_clkset_sclk_mpll_user_acp_list),
};

static struct clksrc_clk exynos3470_clk_sclk_mpll_user_acp = {
	.clk	= {
		.name		= "sclk_mpll_user_acp",
	},
	.sources = &exynos3470_clkset_sclk_mpll_user_acp,
	.reg_src = { .reg = EXYNOS4270_CLKSRC_ACP, .shift = 13, .size = 1 },
};

static struct clk *exynos3470_clkset_sclk_bpll_user_acp_list[] = {
	[0] = &clk_fin_bpll,
	[1] = &exynos3470_clk_mout_bpll.clk,
};

static struct clksrc_sources exynos3470_clkset_sclk_bpll_user_acp = {
	.sources	= exynos3470_clkset_sclk_bpll_user_acp_list,
	.nr_sources	= ARRAY_SIZE(exynos3470_clkset_sclk_bpll_user_acp_list),
};

static struct clksrc_clk exynos3470_clk_sclk_bpll_user_acp = {
	.clk	= {
		.name		= "sclk_bpll_user_acp",
	},
	.sources = &exynos3470_clkset_sclk_bpll_user_acp,
	.reg_src = { .reg = EXYNOS4270_CLKSRC_ACP, .shift = 11, .size = 1 },
};

static struct clk *exynos3470_clkset_mout_dmcbus_acp_list[] = {
	[0] = &exynos3470_clk_sclk_mpll_user_acp.clk,
	[1] = &exynos3470_clk_sclk_bpll_user_acp.clk,
};

static struct clksrc_sources exynos3470_clkset_mout_dmcbus_acp = {
	.sources	= exynos3470_clkset_mout_dmcbus_acp_list,
	.nr_sources	= ARRAY_SIZE(exynos3470_clkset_mout_dmcbus_acp_list),
};

static struct clksrc_clk exynos3470_clk_mout_dmcbus_acp = {
	.clk	= {
		.name		= "mout_dmcbus",
	},
	.sources = &exynos3470_clkset_mout_dmcbus_acp,
	.reg_src = { .reg = EXYNOS4270_CLKSRC_ACP, .shift = 4, .size = 1 },
};

static struct clksrc_clk exynos3470_clk_sclk_dmc_acp = {
	.clk	= {
		.name		= "sclk_dmc_acp",
		.parent		= &exynos3470_clk_mout_dmcbus_acp.clk,
	},
	.reg_div = { .reg = EXYNOS4270_CLKDIV_ACP0, .shift = 8, .size = 3 },
};

static struct clksrc_clk exynos3470_clk_sclk_dmcd_acp = {
	.clk	= {
		.name		= "sclk_dmcd_acp",
		.parent		= &exynos3470_clk_sclk_dmc_acp.clk,
	},
	.reg_div = { .reg = EXYNOS4270_CLKDIV_ACP0, .shift = 12, .size = 3 },
};

static struct clksrc_clk exynos3470_clk_sclk_dmcp_acp = {
	.clk	= {
		.name		= "sclk_dmcp_acp",
		.parent		= &exynos3470_clk_sclk_dmcd_acp.clk,
	},
	.reg_div = { .reg = EXYNOS4270_CLKDIV_ACP0, .shift = 16, .size = 3 },
};

static struct clksrc_clk exynos3470_clk_aclk_acp = {
	.clk	= {
		.name		= "aclk_acp",
		.parent		= &exynos3470_clk_mout_dmcbus_acp.clk,
	},
	.reg_div = { .reg = EXYNOS4270_CLKDIV_ACP0, .shift = 0, .size = 3 },
};

static struct clksrc_clk exynos3470_clk_pclk_acp = {
	.clk	= {
		.name		= "pclk_acp",
		.parent		= &exynos3470_clk_aclk_acp.clk,
	},
	.reg_div = { .reg = EXYNOS4270_CLKDIV_ACP0, .shift = 4, .size = 3 },
};

static struct clk *exynos3470_clkset_mout_g2d_acp_0_list[] = {
	[0] = &exynos3470_clk_sclk_mpll_user_acp.clk,
	[1] = &exynos3470_clk_sclk_bpll_user_acp.clk,
};

static struct clksrc_sources exynos3470_clkset_mout_g2d_acp_0 = {
	.sources	= exynos3470_clkset_mout_g2d_acp_0_list,
	.nr_sources	= ARRAY_SIZE(exynos3470_clkset_mout_g2d_acp_0_list),
};

static struct clksrc_clk exynos3470_clk_mout_g2d_acp_0 = {
	.clk	= {
		.name		= "mout_g2d_acp_0",
		.parent		= &exynos3470_clk_sclk_mpll_user_acp.clk,
	},
	.sources = &exynos3470_clkset_mout_g2d_acp_0,
	.reg_src = { .reg = EXYNOS4270_CLKSRC_ACP, .shift = 20, .size = 1 },
};

static struct clk *exynos3470_clkset_mout_g2d_acp_1_list[] = {
	[0] = &exynos4_clk_mout_epll.clk,
	[1] = &exynos3470_clk_sclk_bpll_user_acp.clk,
};

static struct clksrc_sources exynos3470_clkset_mout_g2d_acp_1 = {
	.sources	= exynos3470_clkset_mout_g2d_acp_1_list,
	.nr_sources	= ARRAY_SIZE(exynos3470_clkset_mout_g2d_acp_1_list),
};

static struct clksrc_clk exynos3470_clk_mout_g2d_acp_1 = {
	.clk	= {
		.name		= "mout_g2d_acp_1",
		.parent		= &exynos3470_clk_sclk_bpll_user_acp.clk,
	},
	.sources = &exynos3470_clkset_mout_g2d_acp_1,
	.reg_src = { .reg = EXYNOS4270_CLKSRC_ACP, .shift = 24, .size = 1 },
};

static struct clk *exynos3470_clkset_mout_g2d_acp_list[] = {
	[0] = &exynos3470_clk_mout_g2d_acp_0.clk,
	[1] = &exynos3470_clk_mout_g2d_acp_1.clk,
};

static struct clksrc_sources exynos3470_clkset_mout_g2d_acp = {
	.sources	= exynos3470_clkset_mout_g2d_acp_list,
	.nr_sources	= ARRAY_SIZE(exynos3470_clkset_mout_g2d_acp_list),
};

static struct clksrc_clk exynos3470_clk_mout_g2d_acp = {
	.clk	= {
		.name		= "mout_g2d_acp",
		.parent		= &exynos3470_clk_mout_g2d_acp_0.clk,
	},
	.sources = &exynos3470_clkset_mout_g2d_acp,
	.reg_src = { .reg = EXYNOS4270_CLKSRC_ACP, .shift = 28, .size = 1 },
};

static struct clksrc_clk exynos3470_clk_sclk_g2d_acp = {
	.clk	= {
		.name		= "sclk_fimg2d",
		.parent		= &exynos3470_clk_mout_g2d_acp.clk,
	},
	.reg_div = { .reg = EXYNOS4270_CLKDIV_ACP1, .shift = 0, .size = 4 },
};

static struct clksrc_clk exynos3470_clk_pre_aclk_400_mcuisp = {
	.clk	= {
		.name		= "pre_aclk_400_mcuisp",
	},
	.sources = &exynos4_clkset_aclk,
	.reg_src = { .reg = EXYNOS4_CLKSRC_TOP1, .shift = 8, .size = 1 },
	.reg_div = { .reg = EXYNOS4_CLKDIV_TOP, .shift = 24, .size = 3 },
};

struct clk *exynos3470_clkset_aclk_400_mcuisp_list[] = {
	[0] = &clk_fin_mpll,
	[1] = &exynos3470_clk_pre_aclk_400_mcuisp.clk,
};

struct clksrc_sources exynos3470_clkset_aclk_400_mcuisp = {
	.sources	= exynos3470_clkset_aclk_400_mcuisp_list,
	.nr_sources	= ARRAY_SIZE(exynos3470_clkset_aclk_400_mcuisp_list),
};

struct clksrc_clk exynos3470_clk_aclk_400_mcuisp = {
	.clk	= {
		.name		= "aclk_400_mcuisp",
		.parent		= &exynos3470_clk_pre_aclk_400_mcuisp.clk,
	},
	.sources = &exynos3470_clkset_aclk_400_mcuisp,
	.reg_src = { .reg = EXYNOS4_CLKSRC_TOP1, .shift = 24, .size = 1 },
};

struct clk *exynos3470_clkset_pre_aclk_266_list[] = {
	[0] = &exynos4_clk_mout_mpll.clk,
	[1] = &exynos4_clk_sclk_vpll.clk,
};

struct clksrc_sources exynos3470_clkset_pre_aclk_266 = {
	.sources	= exynos3470_clkset_pre_aclk_266_list,
	.nr_sources	= ARRAY_SIZE(exynos3470_clkset_pre_aclk_266_list),
};

static struct clksrc_clk exynos3470_clk_pre_aclk_266 = {
	.clk	= {
		.name		= "pre_aclk_266",
	},
	.sources = &exynos3470_clkset_pre_aclk_266,
	.reg_src = { .reg = EXYNOS4_CLKSRC_TOP0, .shift = 12, .size = 1 },
	.reg_div = { .reg = EXYNOS4_CLKDIV_TOP, .shift = 0, .size = 3 },
};

struct clk *exynos3470_clkset_aclk_266_list[] = {
	[0] = &clk_fin_mpll,
	[1] = &exynos3470_clk_pre_aclk_266.clk,
};

struct clksrc_sources exynos3470_clkset_aclk_266 = {
	.sources	= exynos3470_clkset_aclk_266_list,
	.nr_sources	= ARRAY_SIZE(exynos3470_clkset_aclk_266_list),
};

struct clksrc_clk exynos3470_clk_aclk_266 = {
	.clk	= {
		.name		= "aclk_266",
		.parent		= &exynos3470_clk_pre_aclk_266.clk,
	},
	.sources = &exynos3470_clkset_aclk_266,
	.reg_src = { .reg = EXYNOS4_CLKSRC_TOP1, .shift = 20, .size = 1 },
};

static struct clksrc_clk exynos3470_clk_aclk_266_div1 = {
	.clk = {
		.name		= "aclk_266_div1",
		.parent		= &exynos3470_clk_aclk_266.clk,
	},
	.reg_div = { .reg = EXYNOS4_CLKDIV_ISP0, .shift = 4, .size = 3 },
};

static struct clk *exynos3470_clkset_mout_jpeg0_list[] = {
	[0] = &exynos3470_clk_mout_mpll_user_top.clk,
};

static struct clksrc_sources exynos3470_clkset_mout_jpeg0 = {
	.sources	= exynos3470_clkset_mout_jpeg0_list,
	.nr_sources	= ARRAY_SIZE(exynos3470_clkset_mout_jpeg0_list),
};

static struct clksrc_clk exynos3470_clk_mout_jpeg0 = {
	.clk	= {
		.name		= "mout_jpeg0",
		.parent		= &exynos3470_clk_mout_mpll_user_top.clk,
	},
	.sources = &exynos3470_clkset_mout_jpeg0,
	.reg_src = { .reg = EXYNOS4_CLKSRC_CAM1, .shift = 0, .size = 1 },
};

static struct clk *exynos3470_clkset_mout_jpeg_list[] = {
	[0] = &exynos3470_clk_mout_jpeg0.clk,
};

static struct clksrc_sources exynos3470_clkset_mout_jpeg = {
	.sources	= exynos3470_clkset_mout_jpeg_list,
	.nr_sources	= ARRAY_SIZE(exynos3470_clkset_mout_jpeg_list),
};

static struct clksrc_clk exynos3470_clk_dout_tsadc = {
	.clk	= {
		.name		= "dout_tsadc",
	},
	.sources = &exynos4_clkset_group,
	.reg_src = { .reg = EXYNOS4_CLKSRC_FSYS, .shift = 28, .size = 4 },
	.reg_div = { .reg = EXYNOS4_CLKDIV_FSYS0, .shift = 0, .size = 4 },
};

static struct clksrc_clk exynos3470_clk_sclk_tsadc = {
	.clk	= {
		.name		= "sclk_tsadc",
		.parent		= &exynos3470_clk_dout_tsadc.clk,
	},
	.reg_div = { .reg = EXYNOS4_CLKDIV_FSYS0, .shift = 8, .size = 8 },
};

static struct clksrc_clk clksrcs[] = {
	{
		.clk	= {
			.name		= "aclk_clk_jpeg",
		},
		.sources = &exynos3470_clkset_mout_jpeg,
		.reg_src = { .reg = EXYNOS4_CLKSRC_CAM1, .shift = 8, .size = 1 },
		.reg_div = { .reg = EXYNOS4_CLKDIV_CAM1, .shift = 0, .size = 4 },
	}, {
		.clk = {
			.name		= "aclk_400_mcuisp_div0",
			.parent		= &exynos3470_clk_aclk_400_mcuisp.clk,
		},
		.reg_div = { .reg = EXYNOS4_CLKDIV_ISP1, .shift = 4, .size = 3 },
	}, {
		.clk = {
			.name		= "aclk_400_mcuisp_div1",
			.parent		= &exynos3470_clk_aclk_400_mcuisp.clk,
		},
		.reg_div = { .reg = EXYNOS4_CLKDIV_ISP1, .shift = 8, .size = 3 },
	}, {
		.clk = {
			.name		= "aclk_266_div0",
			.parent		= &exynos3470_clk_aclk_266.clk,
		},
		.reg_div = { .reg = EXYNOS4_CLKDIV_ISP0, .shift = 0, .size = 3 },
	}, {
		.clk = {
			.name		= "aclk_266_div2",
			.parent		= &exynos3470_clk_aclk_266_div1.clk,
		},
		.reg_div = { .reg = EXYNOS4_CLKDIV_ISP1, .shift = 0, .size = 3 },
	}, {
		.clk = {
			.name		= "sclk_uart_isp",
		},
		.sources = &exynos4_clkset_group,
		.reg_src = { .reg = EXYNOS4_CLKSRC_ISP, .shift = 12, .size = 4 },
		.reg_div = { .reg = EXYNOS4_CLKDIV_ISP, .shift = 28, .size = 4 },
	},
};

static struct clksrc_clk *sysclks[] = {
	&exynos3470_clk_mout_bpll,
	&exynos3470_clk_sclk_mpll,
	&clk_mout_mpll_user,
	&exynos3470_clk_mout_mpll_user_top,
	&exynos3470_clk_atclk,
	&exynos3470_clk_pclk_dbg,
	&exynos3470_clk_sclk_dmc_pre,
	&exynos3470_clk_sclk_dmc,
	&exynos3470_clk_aclk_cored,
	&exynos3470_clk_aclk_corep,
	&exynos3470_clk_sclk_mpll_user_acp,
	&exynos3470_clk_sclk_bpll_user_acp,
	&exynos3470_clk_mout_dmcbus_acp,
	&exynos3470_clk_sclk_dmc_acp,
	&exynos3470_clk_sclk_dmcd_acp,
	&exynos3470_clk_sclk_dmcp_acp,
	&exynos3470_clk_aclk_acp,
	&exynos3470_clk_pclk_acp,
	&exynos3470_clk_mout_g2d_acp_0,
	&exynos3470_clk_mout_g2d_acp_1,
	&exynos3470_clk_mout_g2d_acp,
	&exynos3470_clk_sclk_g2d_acp,
	&exynos3470_clk_pre_aclk_400_mcuisp,
	&exynos3470_clk_aclk_400_mcuisp,
	&exynos3470_clk_pre_aclk_266,
	&exynos3470_clk_aclk_266,
	&exynos3470_clk_aclk_266_div1,
	&exynos3470_clk_mout_jpeg0,
	&exynos3470_clk_dout_tsadc,
	&exynos3470_clk_sclk_tsadc,
};

static struct clk init_clocks_off[] = {
	{
		.name		= SYSMMU_CLOCK_NAME,
		.devname	= SYSMMU_CLOCK_DEVNAME(2d, 15),
		.enable		= exynos3470_clk_ip_acp0_ctrl,
		.ctrlbit	= (1 << 24),
	}, {
		.name		= SYSMMU_CLOCK_NAME,
		.devname	= SYSMMU_CLOCK_DEVNAME(isp0, 9),
		.enable		= exynos3470_clk_ip_isp0_ctrl,
		.ctrlbit	= (7 << 8),
	}, {
		.name		= SYSMMU_CLOCK_NAME,
		.devname	= SYSMMU_CLOCK_DEVNAME(isp1, 16),
		.enable		= exynos3470_clk_ip_isp1_ctrl,
		.ctrlbit	= (3 << 17) | (1 << 4),
	}, {
		.name		= SYSMMU_CLOCK_NAME,
		.devname	= SYSMMU_CLOCK_DEVNAME(camif0, 12),
		.enable		= exynos3470_clk_ip_isp0_ctrl,
		.ctrlbit	= (1 << 11),
	}, {
		.name		= SYSMMU_CLOCK_NAME,
		.devname	= SYSMMU_CLOCK_DEVNAME(camif1, 13),
		.enable		= exynos3470_clk_ip_isp0_ctrl,
		.ctrlbit	= (1 << 12),
	}, {
		.name		= SYSMMU_CLOCK_NAME,
		.devname	= SYSMMU_CLOCK_DEVNAME(camif2, 14),
		.enable		= exynos4_clk_ip_cam_ctrl,
		.ctrlbit	= (1 << 22),
	}, {
		.name		= "hdmiphy",
		.devname	= "exynos4-hdmi",
		.parent		= &exynos4_clk_aclk_100.clk,
		.enable		= exynos4_clk_ip_peril_ctrl,
		.ctrlbit	= (1 << 14),
	}, {
		.name		= "fimg2d",
		.devname	= "s5p-fimg2d",
		.enable		= exynos3470_clk_ip_acp_ctrl,
		.ctrlbit	= (1 << 23) | (1 << 25),
	}, {
		.name		= "secss",
		.parent		= &exynos3470_clk_aclk_acp.clk,
		.enable		= exynos3470_clk_ip_acp0_ctrl,
		.ctrlbit	= (1 << 4),
	}, {
		.name		= "acp_sss",
		.enable		= exynos3470_clk_ip_acp0_ctrl,
		.ctrlbit	= (1 << 12) | (1 << 15),
	}, {
		.name		= "c2c",
		.enable		= exynos3470_clk_ip_dmc0_ctrl,
		.ctrlbit	= (1 << 26) | (1 << 30),
	}, {
		.name		= "gate_isp0",
		.devname	= FIMC_IS_DEV_NAME,
		.enable		= exynos3470_clk_ip_isp0_ctrl,
		.ctrlbit	= ((0x3 << 30) | (0x1 << 28) |
				   (0x3 << 25) | (0x1 << 23) |
				   (0x1 << 7)  | (0x3F << 0))
	}, {
		.name		= "gate_isp1",
		.devname	= FIMC_IS_DEV_NAME,
		.enable		= exynos3470_clk_ip_isp1_ctrl,
		.ctrlbit	= ((0x3 << 15) | (0x1 << 0)),
	}, {
		.name		= "camif",
		.devname	= "exynos-fimc-lite.0",
		.enable		= exynos3470_clk_ip_isp0_ctrl,
		.ctrlbit	= (1 << 3),
	}, {
		.name		= "camif",
		.devname	= "exynos-fimc-lite.1",
		.enable		= exynos3470_clk_ip_isp0_ctrl,
		.ctrlbit	= (1 << 4),
	}, {
		.name		= "camif",
		.devname	= "exynos-fimc-lite.2",
		.enable		= exynos4_clk_ip_cam_ctrl,
		.ctrlbit	= (1 << 20),
	}, {
		.name		= "spi0_isp",
		.enable		= exynos3470_clk_ip_isp_ctrl,
		.ctrlbit	= (1 << 1),
	}, {
		.name		= "spi1_isp",
		.enable		= exynos3470_clk_ip_isp_ctrl,
		.ctrlbit	= (1 << 2),
	}, {
		.name		= "uart_isp",
		.enable		= exynos3470_clk_ip_isp_ctrl,
		.ctrlbit	= (1 << 3),
	}, {
		.name		= "tsadc_isp",
		.enable		= exynos3470_clk_ip_isp_ctrl,
		.ctrlbit	= (1 << 4),
	}, {
		.name		= "spi0_isp1",
		.enable		= exynos3470_clk_ip_isp1_ctrl,
		.ctrlbit	= (1 << 12),
	}, {
		.name		= "spi1_isp1",
		.enable		= exynos3470_clk_ip_isp1_ctrl,
		.ctrlbit	= (1 << 13),
	}, {
		.name		= "mpwm_isp",
		.enable		= exynos3470_clk_ip_isp0_ctrl,
		.ctrlbit	= (1 << 24),
	}, {
		.name		= "adc_isp",
		.enable		= exynos3470_clk_ip_isp0_ctrl,
		.ctrlbit	= (1 << 27),
	}, {
		.name		= "adc",
		.enable		= exynos4_clk_ip_fsys_ctrl ,
		.ctrlbit	= (1 << 20),
	},
};

#ifdef CONFIG_PM_SLEEP
static int exynos3470_clock_suspend(void)
{
	s3c_pm_do_save(exynos3470_clock_save,
				ARRAY_SIZE(exynos3470_clock_save));
	return 0;
}

static void exynos3470_clock_resume(void)
{
	s3c_pm_do_restore_core(exynos3470_clock_save,
				ARRAY_SIZE(exynos3470_clock_save));
}

#else
#define exynos3470_clock_suspend NULL
#define exynos3470_clock_resume NULL
#endif

static struct syscore_ops exynos3470_clock_syscore_ops = {
	.suspend	= exynos3470_clock_suspend,
	.resume		= exynos3470_clock_resume,
};

static struct vpll_div_data exynos3470_vpll_div[] = {
	{100000000, 3, 200, 4, 0, 0, 0, 0},
	{160000000, 2, 107, 3, -21485, 0, 0, 0},
	{266000000, 3, 266, 3, 0, 0, 0, 0},
	{300000000, 2, 100, 2, 0, 0, 0, 0},
	{340000000, 3, 170, 2, 0, 0, 0, 0},
	{350000000, 2, 117, 2, -21485, 0, 0, 0},
	{440000000, 2, 147, 2, -21485, 0, 0, 0},
	{500000000, 2, 167, 2, -21485, 0, 0, 0},
	{533000000, 3, 267, 2, -32768, 0, 0, 0},
	{600000000, 2, 100, 1, 0, 0, 0, 0},
	{900000000, 2, 150, 1, 0, 0, 0, 0},
};

static unsigned long exynos3470_vpll_get_rate(struct clk *clk)
{
	return clk->rate;
}

static int exynos3470_vpll_set_rate(struct clk *clk, unsigned long rate)
{
	unsigned int vpll_con0, vpll_con1;
	unsigned int locktime;
	unsigned int tmp;
	unsigned int i;
	unsigned int k;

	/* Return if nothing changed */
	if (clk->rate == rate)
		return 0;

	vpll_con0 = __raw_readl(EXYNOS4_VPLL_CON0);
	vpll_con0 &= ~(PLL36XX_MDIV_MASK << PLL36XX_MDIV_SHIFT | \
			PLL36XX_PDIV_MASK << PLL36XX_PDIV_SHIFT | \
			PLL36XX_SDIV_MASK << PLL36XX_SDIV_SHIFT);

	vpll_con1 = __raw_readl(EXYNOS4_VPLL_CON1);
	vpll_con1 &= ~(0xffff << 0);

	for (i = 0; i < ARRAY_SIZE(exynos3470_vpll_div); i++) {
		if (exynos3470_vpll_div[i].rate == rate) {
			k = exynos3470_vpll_div[i].k & 0xFFFF;
			vpll_con1 |= k << 0;
			vpll_con0 |= exynos3470_vpll_div[i].pdiv
							<< PLL36XX_PDIV_SHIFT;
			vpll_con0 |= exynos3470_vpll_div[i].mdiv
							<< PLL36XX_MDIV_SHIFT;
			vpll_con0 |= exynos3470_vpll_div[i].sdiv
							<< PLL36XX_SDIV_SHIFT;
			vpll_con0 |= 1 << 31;
			break;
		}
	}

	if (i == ARRAY_SIZE(exynos3470_vpll_div)) {
		pr_err("%s: Invalid Clock VPLL Frequency\n", __func__);
		return -EINVAL;
	}

	/* 3000 max_cycls : specification data */
	locktime = 3000 * exynos3470_vpll_div[i].pdiv;

	__raw_writel(locktime, EXYNOS4_VPLL_LOCK);
	__raw_writel(vpll_con0, EXYNOS4_VPLL_CON0);
	__raw_writel(vpll_con1, EXYNOS4_VPLL_CON1);

	do {
		tmp = __raw_readl(EXYNOS4_VPLL_CON0);
	} while (!(tmp & (0x1 << 29)));

	clk->rate = rate;

	return 0;
}

static struct clk_ops exynos3470_vpll_ops = {
	.get_rate = exynos3470_vpll_get_rate,
	.set_rate = exynos3470_vpll_set_rate,
};

static struct pll_div_data exynos3470_mpll_div[] = {
	{800000000, 3, 200, 1, 0, 0, 0},
	{400000000, 3, 200, 2, 0, 0, 0},
};

static unsigned long exynos3470_mpll_get_rate(struct clk *clk)
{
	return clk->rate;
}

static int exynos3470_mpll_set_rate(struct clk *clk, unsigned long rate)
{
	unsigned int mpll_con0;
	unsigned int locktime;
	unsigned int tmp;
	unsigned int i;

	/* Return if nothing changed */
	if (clk->rate == rate)
		return 0;

	mpll_con0 = __raw_readl(EXYNOS4270_MPLL_CON0);
	mpll_con0 &= ~(PLL35XX_MDIV_MASK << PLL35XX_MDIV_SHIFT |\
			PLL35XX_PDIV_MASK << PLL35XX_PDIV_SHIFT |\
			PLL35XX_SDIV_MASK << PLL35XX_SDIV_SHIFT);

	for (i = 0; i < ARRAY_SIZE(exynos3470_mpll_div); i++) {
		if (exynos3470_mpll_div[i].rate == rate) {
			mpll_con0 |= exynos3470_mpll_div[i].pdiv
							<< PLL35XX_PDIV_SHIFT;
			mpll_con0 |= exynos3470_mpll_div[i].mdiv
							<< PLL35XX_MDIV_SHIFT;
			mpll_con0 |= exynos3470_mpll_div[i].sdiv
							<< PLL35XX_SDIV_SHIFT;
			mpll_con0 |= 1 << 31;
			break;
		}
	}

	if (i == ARRAY_SIZE(exynos3470_mpll_div)) {
		printk(KERN_ERR "%s: Invalid Clock MPLL Frequency\n", __func__);
		return -EINVAL;
	}

	/* 3000 max_cycls : specification data */
	locktime = 3000 * exynos3470_mpll_div[i].pdiv;

	__raw_writel(locktime, EXYNOS4270_MPLL_LOCK);
	__raw_writel(mpll_con0, EXYNOS4270_MPLL_CON0);

	do {
		tmp = __raw_readl(EXYNOS4270_MPLL_CON0);
	} while (!(tmp & (0x1 << 29)));

	clk->rate = rate;

	return 0;
}

static struct clk_ops exynos3470_mpll_ops = {
	.get_rate = exynos3470_mpll_get_rate,
	.set_rate = exynos3470_mpll_set_rate,
};

/* This setup_pll function will set rate and set parent the pll */
static void setup_pll(const char *pll_name,
			struct clk *parent_clk, unsigned long rate)
{
	struct clk *tmp_clk;

	clk_set_rate(parent_clk, rate);
	tmp_clk = clk_get(NULL, pll_name);
	clk_set_parent(tmp_clk, parent_clk);
	clk_put(tmp_clk);
}

void __init exynos3470_register_clocks(void)
{
	int ptr;

	/* aclk_200 for EXYNOS4270 */
	exynos4_clk_aclk_200.reg_src.shift = 24;
	exynos4_clk_aclk_200.reg_div.shift = 12;

	exynos4_clk_mout_corebus.reg_src.reg = EXYNOS4270_CLKSRC_DMC;
	exynos4_clk_mout_corebus.reg_src.shift = 4;
	exynos4_clk_mout_corebus.reg_src.size = 1;

	exynos4_clk_mout_mpll.reg_src.reg = EXYNOS4270_CLKSRC_DMC;
	exynos4_clk_mout_mpll.reg_src.shift = 12;
	exynos4_clk_mout_mpll.reg_src.size = 1;

	exynos4_clkset_aclk_top_list[0] = &exynos3470_clk_mout_mpll_user_top.clk;
	exynos4_clkset_aclk_top_list[1] = NULL;
	exynos4_clkset_group_list[2] = NULL;
	exynos4_clkset_group_list[5] = NULL;
	exynos4_clkset_group_list[6] = &exynos3470_clk_mout_mpll_user_top.clk;
	exynos4_clkset_vpllsrc_list[1] = NULL;

	clk_fout_vpll.ops = &exynos3470_vpll_ops;
	setup_pll("mout_vpll", &clk_fout_vpll, 500000000);

	clk_fout_mpll.ops = &exynos3470_mpll_ops;

	for (ptr = 0; ptr < ARRAY_SIZE(sysclks); ptr++)
		s3c_register_clksrc(sysclks[ptr], 1);

	s3c_register_clksrc(clksrcs, ARRAY_SIZE(clksrcs));

	s3c_register_clocks(init_clocks_off, ARRAY_SIZE(init_clocks_off));
	s3c_disable_clocks(init_clocks_off, ARRAY_SIZE(init_clocks_off));

	register_syscore_ops(&exynos3470_clock_syscore_ops);
}

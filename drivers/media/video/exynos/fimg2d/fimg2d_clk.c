/* linux/drivers/media/video/exynos/fimg2d/fimg2d_clk.c
 *
 * Copyright (c) 2011 Samsung Electronics Co., Ltd.
 *	http://www.samsung.com/
 *
 * Samsung Graphics 2D driver
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
*/

#include <linux/err.h>
#include <linux/clk.h>
#include <linux/atomic.h>
#include <linux/sched.h>
#include <plat/cpu.h>
#include <plat/fimg2d.h>
#include "fimg2d.h"
#include "fimg2d_clk.h"

void fimg2d_clk_on(struct fimg2d_control *ctrl)
{
	clk_enable(ctrl->clock);
	fimg2d_debug("%s : clock enable\n", __func__);
}

void fimg2d_clk_off(struct fimg2d_control *ctrl)
{
	clk_disable(ctrl->clock);
	fimg2d_debug("%s : clock disable\n", __func__);
}

int fimg2d_clk_setup(struct fimg2d_control *ctrl)
{
	struct fimg2d_platdata *pdata;
	struct clk *parent, *sclk;
	int ret = 0;

	sclk = parent = NULL;
	pdata = to_fimg2d_plat(ctrl->dev);

	if (ip_is_g2d_5g() || ip_is_g2d_5a()) {
		fimg2d_info("aclk_acp(%lu) pclk_acp(%lu)\n",
				clk_get_rate(clk_get(NULL, "aclk_acp")),
				clk_get_rate(clk_get(NULL, "pclk_acp")));
	} else {
		sclk = clk_get(ctrl->dev, pdata->clkname);
		if (IS_ERR(sclk)) {
			fimg2d_err("failed to get fimg2d clk\n");
			ret = -ENOENT;
			goto err_clk1;
		}
		fimg2d_info("fimg2d clk name: %s clkrate: %ld\n",
				pdata->clkname, clk_get_rate(sclk));
	}
	/* clock for gating */
	ctrl->clock = clk_get(ctrl->dev, pdata->gate_clkname);
	if (IS_ERR(ctrl->clock)) {
		fimg2d_err("failed to get gate clk\n");
		ret = -ENOENT;
		goto err_clk2;
	}
	fimg2d_info("gate clk: %s\n", pdata->gate_clkname);

	return ret;

err_clk2:
	if (sclk)
		clk_put(sclk);

err_clk1:
	return ret;
}

void fimg2d_clk_release(struct fimg2d_control *ctrl)
{
	clk_put(ctrl->clock);
	if (ip_is_g2d_4p()) {
		struct fimg2d_platdata *pdata;
		pdata = to_fimg2d_plat(ctrl->dev);
		clk_put(clk_get(ctrl->dev, pdata->clkname));
		clk_put(clk_get(ctrl->dev, pdata->parent_clkname));
	}
}

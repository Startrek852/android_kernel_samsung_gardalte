/* linux/arch/arm/mach-exynos/setup-jpeg.c
 *
 * Copyright (c) 2009-2011 Samsung Electronics Co., Ltd.
 *             http://www.samsung.com
 *
 * Base Exynos4 JPEG configuration
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
*/

#include <linux/kernel.h>
#include <linux/types.h>
#include <linux/clk.h>
#include <linux/err.h>
#include <linux/platform_device.h>

#include <plat/gpio-cfg.h>
#include <plat/clock.h>

#include <mach/regs-clock.h>
#include <mach/map.h>

int __init exynos4_jpeg_setup_clock(struct device *dev,
					unsigned long clk_rate)
{
	struct clk *aclk = NULL;
	struct clk *mout_jpeg = NULL;
	struct clk *mout_mpll = NULL;
	int ret;

	/* Set aclk_jpeg */
	aclk = clk_get(dev, "aclk_clk_jpeg");

	if (IS_ERR(aclk)) {
		dev_err(dev, "failed to get aclk for jpeg\n");
		return PTR_ERR(aclk);
	}

	/* Set mout_jpeg0 */
	mout_jpeg = clk_get(dev, "mout_jpeg0");

	if (IS_ERR(mout_jpeg)) {
		dev_err(dev, "failed to get mout_jpeg0 for jpeg\n");
		goto err_clk1;
	}

	ret = clk_set_parent(aclk, mout_jpeg);
	if (ret < 0) {
		dev_err(dev, "failed to clk_set_parent for aclk_clk_jpeg\n");
		goto err_clk2;
	}

	/* Set mout_mpll */
	if (soc_is_exynos4270() || soc_is_exynos3470())
		mout_mpll = clk_get(dev, "mout_mpll_user_top");
	else
		mout_mpll = clk_get(dev, "mout_mpll_user");

	if (IS_ERR(mout_mpll)) {
		dev_err(dev, "failed to get mout_mpll for jpeg\n");
		goto err_clk2;
	}

	ret = clk_set_parent(mout_jpeg, mout_mpll);
	if (ret < 0) {
		dev_err(dev, "failed to clk_set_parent for mout_jpeg0\n");
		goto err_clk3;
	}

	ret = clk_set_rate(aclk, clk_rate);
	if (ret < 0) {
		dev_err(dev, "failed to clk_set_rate of aclk for jpeg\n");
		goto err_clk3;
	}
	pr_debug("jpeg clk : %s, clk rate : %ld\n", aclk->name, clk_get_rate(aclk));

	clk_put(aclk);
	clk_put(mout_jpeg);
	clk_put(mout_mpll);

	return 0;

err_clk3:
	clk_put(mout_mpll);
err_clk2:
	clk_put(mout_jpeg);
err_clk1:
	clk_put(aclk);

	return -EINVAL;
}

int __init exynos5_jpeg_setup_clock(struct device *dev,
					unsigned long clk_rate)
{
	struct clk *sclk;
	struct clk *mout_user = NULL;
	int ret;

	sclk = clk_get(dev, "sclk_jpeg");
	if (IS_ERR(sclk))
		return PTR_ERR(sclk);

	mout_user = clk_get(dev, "mout_mpll_user");
	if (IS_ERR(mout_user)) {
		dev_err(dev, "failed to clk_get mout_user for jpeg\n");
		clk_put(sclk);
		return PTR_ERR(mout_user);
	}
	ret = clk_set_parent(sclk, mout_user);
	if (ret < 0) {
		dev_err(dev, "failed to clk_set_parent for jpeg\n");
		goto err_clk;
	}

	if (!clk_rate)
		clk_rate = 150000000UL;

	if (clk_set_rate(sclk, clk_rate)) {
		dev_err(dev, "failed to clk_set_rate of sclk for jpeg\n");
		goto err_clk;
	}

	clk_put(sclk);
	clk_put(mout_user);

	return 0;
err_clk:
	clk_put(sclk);
	clk_put(mout_user);
	return -EINVAL;
}

int exynos5_jpeg_fimp_setup_clock(struct device *dev,
	unsigned long clk_rate)
{
	struct clk *sclk;

	sclk = clk_get(dev, "sclk_jpeg");
	if (IS_ERR(sclk))
		return PTR_ERR(sclk);

	clk_enable(sclk);

	if (!clk_rate)
		clk_rate = 166500000UL;

	if (clk_set_rate(sclk, clk_rate)) {
		dev_err(dev, "%s rate change failed: %lu\n", sclk->name, clk_rate);
		clk_disable(sclk);
		clk_put(sclk);
		return PTR_ERR(sclk);
	}

	clk_disable(sclk);
	clk_put(sclk);

	return 0;
}

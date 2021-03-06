/*
 * Copyright (c) 2010-2012 Samsung Electronics Co., Ltd.
 *
 * I2C3 GPIO configuration.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
*/

struct platform_device; /* don't need the contents */

#include <linux/gpio.h>
#include <plat/iic.h>
#include <plat/gpio-cfg.h>
#include <plat/cpu.h>

void s3c_i2c3_cfg_gpio(struct platform_device *dev)
{
#if defined(CONFIG_MACH_GARDA) || defined(CONFIG_MACH_WICHITA) ||\
	defined(CONFIG_MACH_KMINI) || defined(CONFIG_MACH_VASTA)
	s3c_gpio_cfgall_range(EXYNOS4_GPA1(2), 2,
	      S3C_GPIO_SFN(3), S3C_GPIO_PULL_NONE);

#else
	if (soc_is_exynos5250())
		s3c_gpio_cfgall_range(EXYNOS5_GPA1(2), 2,
				      S3C_GPIO_SFN(3), S3C_GPIO_PULL_UP);

	else	/* EXYNOS4210, EXYNOS4212, EXYNOS4412 and EXYNOS4270 */
		s3c_gpio_cfgall_range(EXYNOS4_GPA1(2), 2,
				      S3C_GPIO_SFN(3), S3C_GPIO_PULL_UP);
#endif
}

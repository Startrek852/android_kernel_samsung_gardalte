/*
 * Copyright (c) 2013 Samsung Electronics Co., Ltd.
 *		http://www.samsung.com
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 */

#ifndef __ASM_ARCH_GPIO_EXYNOS_H
#define __ASM_ARCH_GPIO_EXYNOS_H __FILE__

#if defined(CONFIG_MACH_UNIVERSAL222AP) || defined(CONFIG_MACH_GARDA) \
	|| defined(CONFIG_MACH_LT02) || defined(CONFIG_MACH_CARMEN) \
	|| defined(CONFIG_MACH_WICHITA) || defined(CONFIG_MACH_KMINI)
#if defined(CONFIG_TARGET_LOCALE_KOR)
#include "gpio-shannon222ap-kor.h"
#else
#include "gpio-shannon222ap.h"
#endif
#endif

#ifdef CONFIG_SEC_PM
/* Exynos gpio configuration function point for sleep */
extern void (*exynos_set_sleep_gpio_table)(void);

/* To register gpio table */
extern int sec_gpio_init(void);

/* Sleep gpio configuration function */
extern void sec_config_sleep_gpio_table(void);

/* Init gpio configuration function */
extern void sec_config_gpio_table(void);
#endif

#endif /* __ASM_ARCH_GPIO_EXYNOS_H */

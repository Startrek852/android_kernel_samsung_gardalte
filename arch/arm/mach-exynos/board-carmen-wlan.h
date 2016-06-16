/*
 * Copyright (c) 2012 Samsung Electronics Co., Ltd.
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

#ifndef __MACH_EXYNOS_BOARD_CARMEN_WLAN_H
#define __MACH_EXYNOS_BOARD_CARMEN_WLAN_H

#define GPIO_SCSC_HOST_WAKE	EXYNOS4_GPX2(0)
#define GPIO_SCSC_POWER_EN	EXYNOS4_GPC0(3)

#define WLAN_SDIO_CLK   EXYNOS4_GPK1(0)
#define WLAN_SDIO_CMD   EXYNOS4_GPK1(1)
#define WLAN_SDIO_DATA0	EXYNOS4_GPK1(3)
#define WLAN_SDIO_DATA1	EXYNOS4_GPK1(4)
#define WLAN_SDIO_DATA2	EXYNOS4_GPK1(5)
#define WLAN_SDIO_DATA3	EXYNOS4_GPK1(6)

extern void exynos4_smdk4270_mmc1_init(void);

#endif /* __MACH_EXYNOS_BOARD_CARMEN_WLAN_H */

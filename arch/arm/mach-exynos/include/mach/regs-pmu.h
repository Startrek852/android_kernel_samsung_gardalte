/*
 * Copyright (c) 2010-2012 Samsung Electronics Co., Ltd.
 *		http://www.samsung.com
 *
 * EXYNOS - Power management unit definition
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
*/

#ifndef __ASM_ARCH_REGS_PMU_H
#define __ASM_ARCH_REGS_PMU_H __FILE__

#include <mach/map.h>

#define EXYNOS_PMUREG(x)			(S5P_VA_PMU + (x))

#define EXYNOS_CENTRAL_SEQ_CONFIGURATION	EXYNOS_PMUREG(0x0200)
#define EXYNOS_CENTRAL_LOWPWR_CFG		(1 << 16)

#define EXYNOS_CENTRAL_SEQ_OPTION		EXYNOS_PMUREG(0x0208)

#define EXYNOS_SEQ_TRANSITION0			EXYNOS_PMUREG(0x0220)
#define EXYNOS_SEQ_TRANSITION1			EXYNOS_PMUREG(0x0224)
#define EXYNOS_SEQ_TRANSITION2			EXYNOS_PMUREG(0x0228)
#define EXYNOS_SEQ_CORE_TRANSITION0		EXYNOS_PMUREG(0x0260)
#define EXYNOS_SEQ_CORE_TRANSITION1		EXYNOS_PMUREG(0x0264)
#define EXYNOS_SEQ_CORE_TRANSITION2		EXYNOS_PMUREG(0x0268)
#define EXYNOS_SEQ_CORE_TRANSITION3		EXYNOS_PMUREG(0x0270)
#define EXYNOS_SEQ_CORE_TRANSITION4		EXYNOS_PMUREG(0x0274)
#define EXYNOS_SEQ_CORE_TRANSITION5		EXYNOS_PMUREG(0x0278)

#define S5P_SWRESET				EXYNOS_PMUREG(0x0400)
#define EXYNOS_SWRESET				EXYNOS_PMUREG(0x0400)

#define EXYNOS_RST_STAT				EXYNOS_PMUREG(0x0404)
#define EXYNOS_AUTOMATIC_WDT_RESET_DISABLE	EXYNOS_PMUREG(0x0408)
#define EXYNOS_MASK_WDT_RESET_REQUEST		EXYNOS_PMUREG(0x040C)
#define EXYNOS_SYS_WDTRESET			(1 << 20)

#define EXYNOS_WAKEUP_STAT			EXYNOS_PMUREG(0x0600)
#define EXYNOS_WAKEUP_STAT2			EXYNOS_PMUREG(0x0610)
#define EXYNOS_EINT_WAKEUP_MASK			EXYNOS_PMUREG(0x0604)
#define EXYNOS_WAKEUP_MASK			EXYNOS_PMUREG(0x0608)
#define EXYNOS_WAKEUP_MASK2			EXYNOS_PMUREG(0x0614)

#define EXYNOS_HDMI_PHY_CONTROL			EXYNOS_PMUREG(0x0700)
#define EXYNOS_HDMI_PHY_ENABLE			(1 << 0)

#define S5P_MIPI_DPHY_CONTROL(n) 		EXYNOS_PMUREG(0x0710 + (n) * 4)
#define S5P_MIPI_DPHY_ENABLE			(1 << 0)
#define S5P_MIPI_DPHY_SRESETN			(1 << 1)
#define S5P_MIPI_DPHY_MRESETN			(1 << 2)

#define ABB_MODE_060V				0
#define ABB_MODE_065V				1
#define ABB_MODE_070V				2
#define ABB_MODE_075V				3
#define ABB_MODE_080V				4
#define ABB_MODE_085V				5
#define ABB_MODE_090V				6
#define ABB_MODE_095V				7
#define ABB_MODE_100V				8
#define ABB_MODE_105V				9
#define ABB_MODE_110V				10
#define ABB_MODE_115V				11
#define ABB_MODE_120V				12
#define ABB_MODE_125V				13
#define ABB_MODE_130V				14
#define ABB_MODE_135V				15
#define ABB_MODE_140V				16
#define ABB_MODE_145V				17
#define ABB_MODE_150V				18
#define ABB_MODE_155V				19
#define ABB_MODE_160V				20
#define ABB_MODE_BYPASS				255
#define EXYNOS_ABB_INIT				(0x80000080)
#define EXYNOS_ABB_INIT_BYPASS			(0x80000000)

#define EXYNOS_INFORM0				EXYNOS_PMUREG(0x0800)
#define EXYNOS_INFORM1				EXYNOS_PMUREG(0x0804)
#define EXYNOS_INFORM2				EXYNOS_PMUREG(0x0808)
#define EXYNOS_INFORM3				EXYNOS_PMUREG(0x080C)
#define EXYNOS_INFORM4				EXYNOS_PMUREG(0x0810)
#define EXYNOS_INFORM5				EXYNOS_PMUREG(0x0814)
#define EXYNOS_INFORM6				EXYNOS_PMUREG(0x0818)
#define EXYNOS_INFORM7				EXYNOS_PMUREG(0x081C)
#define EXYNOS_IROM_DATA0			EXYNOS_PMUREG(0x0980)
#define EXYNOS_IROM_DATA1			EXYNOS_PMUREG(0x0984)
#define EXYNOS_IROM_DATA2			EXYNOS_PMUREG(0x0988)
#define EXYNOS_IROM_DATA3			EXYNOS_PMUREG(0x098C)
#define EXYNOS_CHECK_SLEEP			0x00000BAD
#define EXYNOS_CHECK_DIDLE			0xBAD00000
#define EXYNOS_CHECK_LPA			0xABAD0000

#define EXYNOS_PMU_DEBUG			EXYNOS_PMUREG(0x0A00)

#define EXYNOS_ARM_CORE0_CONFIGURATION		EXYNOS_PMUREG(0x2000)
#define EXYNOS_ARM_CORE0_STATUS			EXYNOS_PMUREG(0x2004)
#define EXYNOS_ARM_CORE0_OPTION			EXYNOS_PMUREG(0x2008)
#define EXYNOS_ARM_CORE1_CONFIGURATION		EXYNOS_PMUREG(0x2080)
#define EXYNOS_ARM_CORE1_STATUS			EXYNOS_PMUREG(0x2084)
#define EXYNOS_ARM_CORE1_OPTION			EXYNOS_PMUREG(0x2088)
#define EXYNOS_ARM_CORE_OPTION(_nr)		(EXYNOS_ARM_CORE0_OPTION \
						+ ((_nr) * 0x80))
#define EXYNOS_USE_DELAYED_RESET_ASSERTION	BIT(12)
#define EXYNOS_ARM_CORE_STATUS(_nr)		(EXYNOS_ARM_CORE0_STATUS \
						+ ((_nr) * 0x80))
#define EXYNOS_ARM_CORE_CONFIGURATION(_nr)	\
			(EXYNOS_ARM_CORE0_CONFIGURATION + ((_nr) * 0x80))
#define EXYNOS_CORE_LOCAL_PWR_EN		0x3
#define EXYNOS_CORE_AUTOWAKEUP_EN		(1 << 31)

#define EXYNOS_ARM_COMMON_CONFIGURATION		EXYNOS_PMUREG(0x2500)
#define EXYNOS_ARM_COMMON_STATUS		EXYNOS_PMUREG(0x2504)
#define EXYNOS_COMMON_CONFIGURATION(_nr)	\
			(EXYNOS_ARM_COMMON_CONFIGURATION + ((_nr) * 0x80))
#define EXYNOS_COMMON_STATUS(_nr)		\
			(EXYNOS_COMMON_CONFIGURATION(_nr) + 0x4)
#define EXYNOS_COMMON_OPTION(_nr)		\
			(EXYNOS_COMMON_CONFIGURATION(_nr) + 0x8)

#define EXYNOS_ARM_L2_CONFIGURATION		EXYNOS_PMUREG(0x2600)
#define EXYNOS_L2_CONFIGURATION(_nr)	\
			(EXYNOS_ARM_L2_CONFIGURATION + ((_nr) * 0x80))
#define EXYNOS_L2_STATUS(_nr)		\
			(EXYNOS_L2_CONFIGURATION(_nr) + 0x4)
#define EXYNOS_L2_OPTION(_nr)		\
			(EXYNOS_L2_CONFIGURATION(_nr) + 0x8)

#define EXYNOS_L2_COMMON_PWR_EN			0x3

#define EXYNOS_PAD_RET_DRAM_OPTION		EXYNOS_PMUREG(0x3008)
#define EXYNOS_PAD_RET_MAUDIO_OPTION		EXYNOS_PMUREG(0x3028)
#define EXYNOS_PAD_RET_JTAG_OPTION		EXYNOS_PMUREG(0x3048)
#define EXYNOS_PAD_RET_GPIO_OPTION		EXYNOS_PMUREG(0x3108)
#define EXYNOS_PAD_RET_UART_OPTION		EXYNOS_PMUREG(0x3128)
#define EXYNOS_PAD_RET_MMCA_OPTION		EXYNOS_PMUREG(0x3148)
#define EXYNOS_PAD_RET_MMCB_OPTION		EXYNOS_PMUREG(0x3168)
#define EXYNOS_PAD_RET_EBIA_OPTION		EXYNOS_PMUREG(0x3188)
#define EXYNOS_PAD_RET_EBIB_OPTION		EXYNOS_PMUREG(0x31A8)

#define EXYNOS_PS_HOLD_CONTROL			EXYNOS_PMUREG(0x330C)
#define EXYNOS_PS_HOLD_EN			(1 << 31)
#define EXYNOS_PS_HOLD_OUTPUT_HIGH		(3 << 8)

/* For XXX_IP_CONFIGURATION(Power Domain) */
#define EXYNOS_INT_LOCAL_PWR_EN			0x7

/* For SYS_PWR_REG */
#define EXYNOS_SYS_PWR_CFG			(1 << 0)

/* Only for EXYNOS4XXX */
#define EXYNOS4_ABB_INT				EXYNOS_PMUREG(0x0780)
#define EXYNOS4_ABB_MIF				EXYNOS_PMUREG(0x0784)
#define EXYNOS4_ABB_G3D				EXYNOS_PMUREG(0x0788)
#define EXYNOS4_ABB_ARM				EXYNOS_PMUREG(0x078C)
#define EXYNOS4_ABB_MEMBER(_nr)			(EXYNOS4_ABB_INT + (_nr * 0x4))

#define EXYNOS4270_ABB_G3D			EXYNOS_PMUREG(0x0780)
#define EXYNOS4270_ABB_MIF			EXYNOS_PMUREG(0x0784)
#define EXYNOS4270_ABB_ARM			EXYNOS_PMUREG(0x0788)
#define EXYNOS4270_ABB_INT			EXYNOS_PMUREG(0x078C)

#define EXYNOS4_ARM_CORE0_LOWPWR		EXYNOS_PMUREG(0x1000)
#define EXYNOS4_DIS_IRQ_CORE0			EXYNOS_PMUREG(0x1004)
#define EXYNOS4_DIS_IRQ_CENTRAL0		EXYNOS_PMUREG(0x1008)
#define EXYNOS4_ARM_CORE1_LOWPWR		EXYNOS_PMUREG(0x1010)
#define EXYNOS4_DIS_IRQ_CORE1			EXYNOS_PMUREG(0x1014)
#define EXYNOS4_DIS_IRQ_CENTRAL1		EXYNOS_PMUREG(0x1018)
#define EXYNOS4_ARM_CORE2_LOWPWR		EXYNOS_PMUREG(0x1020)
#define EXYNOS4_DIS_IRQ_CORE2			EXYNOS_PMUREG(0x1024)
#define EXYNOS4_DIS_IRQ_CENTRAL2		EXYNOS_PMUREG(0x1028)
#define EXYNOS4_ARM_CORE3_LOWPWR		EXYNOS_PMUREG(0x1030)
#define EXYNOS4_DIS_IRQ_CORE3			EXYNOS_PMUREG(0x1034)
#define EXYNOS4_DIS_IRQ_CENTRAL3		EXYNOS_PMUREG(0x1038)
#define EXYNOS4_ARM_COMMON_LOWPWR		EXYNOS_PMUREG(0x1080)
#define EXYNOS4_L2_0_LOWPWR			EXYNOS_PMUREG(0x10C0)
#define EXYNOS4_L2_1_LOWPWR			EXYNOS_PMUREG(0x10C4)
#define EXYNOS4_CMU_ACLKSTOP_LOWPWR		EXYNOS_PMUREG(0x1100)
#define EXYNOS4_CMU_SCLKSTOP_LOWPWR		EXYNOS_PMUREG(0x1104)
#define EXYNOS4_CMU_RESET_LOWPWR		EXYNOS_PMUREG(0x110C)
#define EXYNOS4_APLL_SYSCLK_LOWPWR		EXYNOS_PMUREG(0x1120)
#define EXYNOS4_MPLL_SYSCLK_LOWPWR		EXYNOS_PMUREG(0x1124)
#define EXYNOS4_VPLL_SYSCLK_LOWPWR		EXYNOS_PMUREG(0x1128)
#define EXYNOS4_EPLL_SYSCLK_LOWPWR		EXYNOS_PMUREG(0x112C)
#define EXYNOS4_CMU_CLKSTOP_GPS_ALIVE_LOWPWR	EXYNOS_PMUREG(0x1138)
#define EXYNOS4_CMU_RESET_GPSALIVE_LOWPWR	EXYNOS_PMUREG(0x113C)
#define EXYNOS4_CMU_CLKSTOP_CAM_LOWPWR		EXYNOS_PMUREG(0x1140)
#define EXYNOS4_CMU_CLKSTOP_TV_LOWPWR		EXYNOS_PMUREG(0x1144)
#define EXYNOS4_CMU_CLKSTOP_MFC_LOWPWR		EXYNOS_PMUREG(0x1148)
#define EXYNOS4_CMU_CLKSTOP_G3D_LOWPWR		EXYNOS_PMUREG(0x114C)
#define EXYNOS4_CMU_CLKSTOP_LCD0_LOWPWR		EXYNOS_PMUREG(0x1150)
#define EXYNOS4_CMU_CLKSTOP_MAUDIO_LOWPWR	EXYNOS_PMUREG(0x1158)
#define EXYNOS4_CMU_CLKSTOP_GPS_LOWPWR		EXYNOS_PMUREG(0x115C)
#define EXYNOS4_CMU_RESET_CAM_LOWPWR		EXYNOS_PMUREG(0x1160)
#define EXYNOS4_CMU_RESET_TV_LOWPWR		EXYNOS_PMUREG(0x1164)
#define EXYNOS4_CMU_RESET_MFC_LOWPWR		EXYNOS_PMUREG(0x1168)
#define EXYNOS4_CMU_RESET_G3D_LOWPWR		EXYNOS_PMUREG(0x116C)
#define EXYNOS4_CMU_RESET_LCD0_LOWPWR		EXYNOS_PMUREG(0x1170)
#define EXYNOS4_CMU_RESET_MAUDIO_LOWPWR		EXYNOS_PMUREG(0x1178)
#define EXYNOS4_CMU_RESET_GPS_LOWPWR		EXYNOS_PMUREG(0x117C)
#define EXYNOS4_TOP_BUS_LOWPWR			EXYNOS_PMUREG(0x1180)
#define EXYNOS4_TOP_RETENTION_LOWPWR		EXYNOS_PMUREG(0x1184)
#define EXYNOS4_TOP_PWR_LOWPWR			EXYNOS_PMUREG(0x1188)
#define EXYNOS4_LOGIC_RESET_LOWPWR		EXYNOS_PMUREG(0x11A0)
#define EXYNOS4_ONENAND_MEM_LOWPWR		EXYNOS_PMUREG(0x11C0)
#define EXYNOS4_G2D_ACP_MEM_LOWPWR		EXYNOS_PMUREG(0x11C8)
#define EXYNOS4_USBOTG_MEM_LOWPWR		EXYNOS_PMUREG(0x11CC)
#define EXYNOS4_HSMMC_MEM_LOWPWR		EXYNOS_PMUREG(0x11D0)
#define EXYNOS4_CSSYS_MEM_LOWPWR		EXYNOS_PMUREG(0x11D4)
#define EXYNOS4_SECSS_MEM_LOWPWR		EXYNOS_PMUREG(0x11D8)
#define EXYNOS4_PAD_RETENTION_DRAM_LOWPWR	EXYNOS_PMUREG(0x1200)
#define EXYNOS4_PAD_RETENTION_MAUDIO_LOWPWR	EXYNOS_PMUREG(0x1204)
#define EXYNOS4_PAD_RETENTION_GPIO_LOWPWR	EXYNOS_PMUREG(0x1220)
#define EXYNOS4_PAD_RETENTION_UART_LOWPWR	EXYNOS_PMUREG(0x1224)
#define EXYNOS4_PAD_RETENTION_MMCA_LOWPWR	EXYNOS_PMUREG(0x1228)
#define EXYNOS4_PAD_RETENTION_MMCB_LOWPWR	EXYNOS_PMUREG(0x122C)
#define EXYNOS4_PAD_RETENTION_EBIA_LOWPWR	EXYNOS_PMUREG(0x1230)
#define EXYNOS4_PAD_RETENTION_EBIB_LOWPWR	EXYNOS_PMUREG(0x1234)
#define EXYNOS4_PAD_RETENTION_ISOLATION_LOWPWR	EXYNOS_PMUREG(0x1240)
#define EXYNOS4_PAD_RETENTION_ALV_SEL_LOWPWR	EXYNOS_PMUREG(0x1260)
#define EXYNOS4_XUSBXTI_LOWPWR			EXYNOS_PMUREG(0x1280)
#define EXYNOS4_XXTI_LOWPWR			EXYNOS_PMUREG(0x1284)
#define EXYNOS4_EXT_REGULATOR_LOWPWR		EXYNOS_PMUREG(0x12C0)
#define EXYNOS4_GPIO_MODE_LOWPWR		EXYNOS_PMUREG(0x1300)
#define EXYNOS4_GPIO_MODE_MAUDIO_LOWPWR		EXYNOS_PMUREG(0x1340)
#define EXYNOS4_CAM_LOWPWR			EXYNOS_PMUREG(0x1380)
#define EXYNOS4_TV_LOWPWR			EXYNOS_PMUREG(0x1384)
#define EXYNOS4_MFC_LOWPWR			EXYNOS_PMUREG(0x1388)
#define EXYNOS4_G3D_LOWPWR			EXYNOS_PMUREG(0x138C)
#define EXYNOS4_LCD0_LOWPWR			EXYNOS_PMUREG(0x1390)
#define EXYNOS4_MAUDIO_LOWPWR			EXYNOS_PMUREG(0x1398)
#define EXYNOS4_GPS_LOWPWR			EXYNOS_PMUREG(0x139C)
#define EXYNOS4_GPS_ALIVE_LOWPWR		EXYNOS_PMUREG(0x13A0)

#define EXYNOS4_ARM_COMMON_OPTION		EXYNOS_PMUREG(0x2408)
#define EXYNOS4_TOP_PWR_OPTION			EXYNOS_PMUREG(0x2C48)
#define EXYNOS4_CORE_TOP_PWR_OPTION		EXYNOS_PMUREG(0x2CA8)
#define EXYNOS4_CORE_TOP_PWR_DURATION		EXYNOS_PMUREG(0x2CB0)
#define EXYNOS4_CAM_OPTION			EXYNOS_PMUREG(0x3C08)
#define EXYNOS4_TV_OPTION			EXYNOS_PMUREG(0x3C28)
#define EXYNOS4_MFC_OPTION			EXYNOS_PMUREG(0x3C48)
#define EXYNOS4_G3D_OPTION			EXYNOS_PMUREG(0x3C68)
#define EXYNOS4_LCD0_OPTION			EXYNOS_PMUREG(0x3C88)
#define EXYNOS4_ISP_OPTION			EXYNOS_PMUREG(0X3CA8)
#define EXYNOS4_MAUDIO_OPTION			EXYNOS_PMUREG(0x3CC8)
#define EXYNOS4_GPS_OPTION			EXYNOS_PMUREG(0x3CE8)
#define EXYNOS4_GPS_ALIVE_OPTION		EXYNOS_PMUREG(0x3D08)

#define EXYNOS4_CAM_CONFIGURATION		EXYNOS_PMUREG(0x3C00)
#define EXYNOS4_CAM_STATUS			EXYNOS_PMUREG(0x3C04)
#define EXYNOS4_TV_CONFIGURATION		EXYNOS_PMUREG(0x3C20)
#define EXYNOS4_TV_STATUS			EXYNOS_PMUREG(0x3C24)
#define EXYNOS4_MFC_CONFIGURATION		EXYNOS_PMUREG(0x3C40)
#define EXYNOS4_MFC_STATUS			EXYNOS_PMUREG(0x3C44)
#define EXYNOS4_G3D_CONFIGURATION		EXYNOS_PMUREG(0x3C60)
#define EXYNOS4_G3D_STATUS			EXYNOS_PMUREG(0x3C64)
#define EXYNOS4_LCD0_CONFIGURATION		EXYNOS_PMUREG(0x3C80)
#define EXYNOS4_LCD0_STATUS			EXYNOS_PMUREG(0x3C84)
#define EXYNOS4_ISP_CONFIFURATION		EXYNOS_PMUREG(0X3CA0)
#define EXYNOS4_ISP_STATUS			EXYNOS_PMUREG(0X3CA4)
#define EXYNOS4_GPS_CONFIGURATION		EXYNOS_PMUREG(0x3CE0)
#define EXYNOS4_GPS_STATUS			EXYNOS_PMUREG(0x3CE4)

/* For EXYNOS_CENTRAL_SEQ_OPTION */
#define EXYNOS4_USE_STANDBY_WFI0		(1 << 16)
#define EXYNOS4_USE_STANDBY_WFI1		(1 << 17)
#define EXYNOS4_USE_STANDBY_WFI2		(1 << 19)
#define EXYNOS4_USE_STANDBY_WFI3		(1 << 20)
#define EXYNOS4_USE_STANDBYWFI_ISP_ARM		(1 << 18)
#define EXYNOS4_USE_STANDBY_WFE0		(1 << 24)
#define EXYNOS4_USE_STANDBY_WFE1		(1 << 25)
#define EXYNOS4_USE_STANDBY_WFE2		(1 << 27)
#define EXYNOS4_USE_STANDBY_WFE3		(1 << 28)

#define EXYNOS4_USE_STANDBYWFE_ISP_ARM		(1 << 26)

/* Only for EXYNOS4210 */
#define EXYNOS4210_USBDEV_PHY_CONTROL		EXYNOS_PMUREG(0x0704)
#define EXYNOS4210_USBHOST_PHY_CONTROL		EXYNOS_PMUREG(0x0708)
#define EXYNOS4210_DAC_PHY_CONTROL		EXYNOS_PMUREG(0x070C)
#define EXYNOS4210_DAC_PHY_ENABLE		(1 << 0)

#define EXYNOS4210_PMU_SATA_PHY_CONTROL		EXYNOS_PMUREG(0x0720)
#define EXYNOS4210_PMU_SATA_PHY_CONTROL_EN	(1 << 0)

#define EXYNOS4210_CMU_CLKSTOP_LCD1_LOWPWR	EXYNOS_PMUREG(0x1154)
#define EXYNOS4210_CMU_RESET_LCD1_LOWPWR	EXYNOS_PMUREG(0x1174)
#define EXYNOS4210_MODIMIF_MEM_LOWPWR		EXYNOS_PMUREG(0x11C4)
#define EXYNOS4210_PCIE_MEM_LOWPWR		EXYNOS_PMUREG(0x11E0)
#define EXYNOS4210_SATA_MEM_LOWPWR		EXYNOS_PMUREG(0x11E4)
#define EXYNOS4210_LCD1_LOWPWR			EXYNOS_PMUREG(0x1394)

#define EXYNOS4210_LCD1_CONFIGURATION		EXYNOS_PMUREG(0x3CA0)
#define EXYNOS4210_LCD1_OPTION			EXYNOS_PMUREG(0x3CA8)

/* Only for EXYNOS4212 & EXYNOS4412 */
#define EXYNOS4x12_USB_PHY_CONTROL			EXYNOS_PMUREG(0x0704)
#define EXYNOS4x12_HSIC0_PHY_CONTROL			EXYNOS_PMUREG(0x0708)
#define EXYNOS4x12_HSIC1_PHY_CONTROL			EXYNOS_PMUREG(0x070C)
#define EXYNOS4X12_ISP_ARM_LOWPWR			EXYNOS_PMUREG(0x1050)
#define EXYNOS4X12_DIS_IRQ_ISP_ARM_LOCAL_LOWPWR		EXYNOS_PMUREG(0x1054)
#define EXYNOS4X12_DIS_IRQ_ISP_ARM_CENTRAL_LOWPWR	EXYNOS_PMUREG(0x1058)
#define EXYNOS4X12_CMU_ACLKSTOP_COREBLK_LOWPWR		EXYNOS_PMUREG(0x1110)
#define EXYNOS4X12_CMU_SCLKSTOP_COREBLK_LOWPWR		EXYNOS_PMUREG(0x1114)
#define EXYNOS4X12_CMU_RESET_COREBLK_LOWPWR		EXYNOS_PMUREG(0x111C)
#define EXYNOS4X12_MPLLUSER_SYSCLK_LOWPWR		EXYNOS_PMUREG(0x1130)
#define EXYNOS4X12_CMU_CLKSTOP_ISP_LOWPWR		EXYNOS_PMUREG(0x1154)
#define EXYNOS4X12_CMU_RESET_ISP_LOWPWR			EXYNOS_PMUREG(0x1174)
#define EXYNOS4X12_TOP_BUS_COREBLK_LOWPWR		EXYNOS_PMUREG(0x1190)
#define EXYNOS4X12_TOP_RETENTION_COREBLK_LOWPWR		EXYNOS_PMUREG(0x1194)
#define EXYNOS4X12_TOP_PWR_COREBLK_LOWPWR		EXYNOS_PMUREG(0x1198)
#define EXYNOS4X12_OSCCLK_GATE_LOWPWR			EXYNOS_PMUREG(0x11A4)
#define EXYNOS4X12_LOGIC_RESET_COREBLK_LOWPWR		EXYNOS_PMUREG(0x11B0)
#define EXYNOS4X12_OSCCLK_GATE_COREBLK_LOWPWR		EXYNOS_PMUREG(0x11B4)
#define EXYNOS4X12_HSI_MEM_LOWPWR			EXYNOS_PMUREG(0x11C4)
#define EXYNOS4X12_ROTATOR_MEM_LOWPWR			EXYNOS_PMUREG(0x11DC)
#define EXYNOS4X12_PAD_RETENTION_GPIO_COREBLK_LOWPWR	EXYNOS_PMUREG(0x123C)
#define EXYNOS4X12_PAD_ISOLATION_COREBLK_LOWPWR		EXYNOS_PMUREG(0x1250)
#define EXYNOS4X12_GPIO_MODE_COREBLK_LOWPWR		EXYNOS_PMUREG(0x1320)
#define EXYNOS4X12_TOP_ASB_RESET_LOWPWR			EXYNOS_PMUREG(0x1344)
#define EXYNOS4X12_TOP_ASB_ISOLATION_LOWPWR		EXYNOS_PMUREG(0x1348)
#define EXYNOS4X12_ISP_LOWPWR				EXYNOS_PMUREG(0x1394)
#define EXYNOS4X12_DRAM_FREQ_DOWN_LOWPWR		EXYNOS_PMUREG(0x13B0)
#define EXYNOS4X12_DDRPHY_DLLOFF_LOWPWR			EXYNOS_PMUREG(0x13B4)
#define EXYNOS4X12_CMU_SYSCLK_ISP_LOWPWR		EXYNOS_PMUREG(0x13B8)
#define EXYNOS4X12_CMU_SYSCLK_GPS_LOWPWR		EXYNOS_PMUREG(0x13BC)
#define EXYNOS4X12_LPDDR_PHY_DLL_LOCK_LOWPWR		EXYNOS_PMUREG(0x13C0)

#define EXYNOS4X12_ARM_L2_0_OPTION			EXYNOS_PMUREG(0x2608)
#define EXYNOS4X12_ARM_L2_1_OPTION			EXYNOS_PMUREG(0x2628)
#define EXYNOS4X12_ONENAND_MEM_OPTION			EXYNOS_PMUREG(0x2E08)
#define EXYNOS4X12_HSI_MEM_OPTION			EXYNOS_PMUREG(0x2E28)
#define EXYNOS4X12_G2D_ACP_MEM_OPTION			EXYNOS_PMUREG(0x2E48)
#define EXYNOS4X12_USBOTG_MEM_OPTION			EXYNOS_PMUREG(0x2E68)
#define EXYNOS4X12_HSMMC_MEM_OPTION			EXYNOS_PMUREG(0x2E88)
#define EXYNOS4X12_CSSYS_MEM_OPTION			EXYNOS_PMUREG(0x2EA8)
#define EXYNOS4X12_SECSS_MEM_OPTION			EXYNOS_PMUREG(0x2EC8)
#define EXYNOS4X12_ROTATOR_MEM_OPTION			EXYNOS_PMUREG(0x2F48)

#define EXYNOS4x12_ISP_CONFIGURATION		EXYNOS_PMUREG(0x3CA0)

/* Common in EXYNOS4212 & EXYNOS4412 & EXYNOS4270 */
#define EXYNOS4X12_LPI_MASK0				EXYNOS_PMUREG(0x0004)
#define EXYNOS4X12_LPI_MASK1				EXYNOS_PMUREG(0x0008)
#define EXYNOS4X12_LPI_MASK2				EXYNOS_PMUREG(0x000C)
#define EXYNOS4X12_RTC_CLKO_SEL				EXYNOS_PMUREG(0x0010)
#define EXYNOS4X12_LPI_DENIAL_MASK0			EXYNOS_PMUREG(0x0018)
#define EXYNOS4X12_LPI_DENIAL_MASK1			EXYNOS_PMUREG(0x001C)
#define EXYNOS4X12_LPI_DENIAL_MASK2			EXYNOS_PMUREG(0x0020)
#define EXYNOS4X12_CENTRAL_SEQ_CONFIGURATION_COREBLK	EXYNOS_PMUREG(0x0240)
#define EXYNOS4X12_AUTOMATIC_WDT_RESET_DISABLE		EXYNOS_PMUREG(0x0408)
#define EXYNOS4X12_MASK_WDT_RESET_DISABLE		EXYNOS_PMUREG(0x040C)
#define EXYNOS4X12_WAKEUP_STAT_COREBLK			EXYNOS_PMUREG(0x0620)
#define EXYNOS4X12_WAKEUP_MASK_COREBLK			EXYNOS_PMUREG(0x0628)

/* Only for EXYNOS4270 */
/* for EXYNOS_WAKEUP_MASK */
#define EXYNOS4270_WAKEUP_CP				(BIT(20) | BIT(24) | BIT(25))

#define EXYNOS4270_CP_CTRL				EXYNOS_PMUREG(0x0024)
#define EXYNOS3470_CP_CTRL				EXYNOS_PMUREG(0x0024)

#define ENABLE_CP		BIT(0)
#define CP_PWRON		BIT(1)
#define CP_RESET_SET		BIT(2)
#define CP_START		BIT(3)
#define CP_ACTIVE_EN		BIT(5)
#define CP_ACTIVE_CLR		BIT(6)
#define CP_RESET_REQ_EN		BIT(7)
#define CP_RESET_REQ_CLR	BIT(8)
#define MASK_CP_PWRDN_DONE	BIT(9)
#define RTC_OUT_EN		BIT(10)
#define CLEANY_BYPASS_EN	BIT(16)

#define EXYNOS4270_CP_STAT				EXYNOS_PMUREG(0x0028)
#define CP_PWRDN_DONE		BIT(0)

#define EXYNOS4270_CP_DEBUG				EXYNOS_PMUREG(0x002C)
#define EXYNOS4270_LPI_MASK3				EXYNOS_PMUREG(0x0030)
#define EXYNOS4270_LPI_DENIAL_MASK3			EXYNOS_PMUREG(0x0034)
#define EXYNOS4270_CP_SEMAPHORE				EXYNOS_PMUREG(0x0038)
#define EXYNOS4270_CP_SEMAPHORE_CNT			EXYNOS_PMUREG(0x003C)
#define EXYNOS4270_CP2AP_MEM_CFG			EXYNOS_PMUREG(0x0040)
#define EXYNOS3470_CP2AP_MEM_CFG			EXYNOS_PMUREG(0x0040)
#define CP_MEM_SIZE_MB(n)	(n/4 << 24)

#define EXYNOS4270_CP2AP_PERI_ACC_WIN			EXYNOS_PMUREG(0x0044)
#define EXYNOS3470_CP2AP_PERI_ACC_WIN			EXYNOS_PMUREG(0x0044)
#define EXYNOS4270_CP2AP_PERI_ACC2_WIN			EXYNOS_PMUREG(0x0048)
#define EXYNOS4270_CP_BOOT_TEST_RST_CFG			EXYNOS_PMUREG(0x004C)
#define EXYNOS4270_AP2CP_PERI_ACCESS_WIN		EXYNOS_PMUREG(0x0050)
#define EXYNOS4270_LIMAP_CFG				EXYNOS_PMUREG(0x0054)
#define EXYNOS4270_MIF_TZPC_CONFIG			EXYNOS_PMUREG(0x0058)
#define EXYNOS4270_CENTRAL_SEQ_CONFIGURATION_CPBLK	EXYNOS_PMUREG(0x0280)
#define EXYNOS3470_CENTRAL_SEQ_CONFIGURATION_CPBLK	EXYNOS_PMUREG(0x0280)
#define EXYNOS4270_UFS_MPHY_CONTROL			EXYNOS_PMUREG(0x071C)
#define EXYNOS4270_BODY_BIAS_CON0			EXYNOS_PMUREG(0x0780)
#define EXYNOS4270_BODY_BIAS_CON1			EXYNOS_PMUREG(0x0784)
#define EXYNOS4270_BODY_BIAS_CON2			EXYNOS_PMUREG(0x0788)
#define EXYNOS4270_BODY_BIAS_CON3			EXYNOS_PMUREG(0x078C)
#define EXYNOS4270_SYSIP_DATA0				EXYNOS_PMUREG(0x0810)
#define EXYNOS4270_SYSIP_DATA1				EXYNOS_PMUREG(0x0814)
#define EXYNOS4270_SYSIP_DATA2				EXYNOS_PMUREG(0x0818)
#define EXYNOS4270_SYSIP_DATA3				EXYNOS_PMUREG(0x081C)
#define EXYNOS4270_BPLLUSER_SYSCLK_LOWPWR		EXYNOS_PMUREG(0x1134)
#define EXYNOS4270_LOGIC_RESET_CPBLK_LOWPWR		EXYNOS_PMUREG(0x11B8)
#define EXYNOS3470_LOGIC_RESET_CPBLK_LOWPWR		EXYNOS_PMUREG(0x11B8)
#define EXYNOS4270_IRAM_MEM_LOWPWR			EXYNOS_PMUREG(0x11E0)
#define EXYNOS4270_PAD_RETENTION_SPI_LOWPWR		EXYNOS_PMUREG(0x1208)
#define EXYNOS4270_PAD_RETENTION_MMC2_LOWPWR		EXYNOS_PMUREG(0x1218)
#define EXYNOS4270_PAD_RETENTION_MMC3_LOWPWR		EXYNOS_PMUREG(0x121C)
#define EXYNOS4270_PAD_RETENTION_JTAG_LOWPWR		EXYNOS_PMUREG(0x1238)
#define EXYNOS4270_EXT_REGULATOR_COREBLK_LOWPWR		EXYNOS_PMUREG(0x12C4)
#define EXYNOS3470_EXT_REGULATOR_COREBLK_LOWPWR		EXYNOS_PMUREG(0x12C4)
#define EXYNOS4270_EXT_REGULATOR_CPBLK_LOWPWR		EXYNOS_PMUREG(0x12C8)
#define EXYNOS3470_EXT_REGULATOR_CPBLK_LOWPWR		EXYNOS_PMUREG(0x12C8)
#define EXYNOS4270_TOP_ASB_RESET_COREBLK_LOWPWR		EXYNOS_PMUREG(0x1350)
#define EXYNOS3470_TOP_ASB_RESET_COREBLK_LOWPWR		EXYNOS_PMUREG(0x1350)
#define EXYNOS4270_TOP_ASB_ISOLATION_COREBLK_LOWPWR	EXYNOS_PMUREG(0x1354)
#define EXYNOS3470_TOP_ASB_ISOLATION_COREBLK_LOWPWR	EXYNOS_PMUREG(0x1354)
#define EXYNOS4270_TOP_ASB_RESET_CPBLK_LOWPWR		EXYNOS_PMUREG(0x1360)
#define EXYNOS3470_TOP_ASB_RESET_CPBLK_LOWPWR		EXYNOS_PMUREG(0x1360)
#define EXYNOS4270_TOP_ASB_ISOLATION_CPBLK_LOWPWR	EXYNOS_PMUREG(0x1364)
#define EXYNOS3470_TOP_ASB_ISOLATION_CPBLK_LOWPWR	EXYNOS_PMUREG(0x1364)
#define EXYNOS4270_BPLL_SYSCLK_LOWPWR			EXYNOS_PMUREG(0x13C4)
#define EXYNOS4270_MPHY_PLL_SYSCLK_LOWPWR		EXYNOS_PMUREG(0x13C8)
#define EXYNOS4270_IRAM_MEM_OPTION			EXYNOS_PMUREG(0x2EE8)
#define EXYNOS4270_PAD_RETENTION_MMC2_OPTION		EXYNOS_PMUREG(0x30C8)
#define EXYNOS4270_PAD_RETENTION_MMC3_OPTION		EXYNOS_PMUREG(0x30E8)
#define EXYNOS4270_PAD_RETENTION_MMC0_OPTION		EXYNOS_PMUREG(0x3148)
#define EXYNOS4270_PAD_RETENTION_MMC1_OPTION		EXYNOS_PMUREG(0x3168)
#define EXYNOS4270_PAD_RETENTION_SPI_OPTION		EXYNOS_PMUREG(0x31C8)
#define EXYNOS4270_EXT_REGULATOR_COREBLK_DURATION	EXYNOS_PMUREG(0x363C)
#define EXYNOS4270_EXT_REGULATOR_CPBLK_DURATION		EXYNOS_PMUREG(0x365C)
#define EXYNOS4270_XUSBXTI_DURATION			EXYNOS_PMUREG(0x341C)
#define EXYNOS4270_XXTI_DURATION			EXYNOS_PMUREG(0x343C)
#define EXYNOS4270_EXT_REGULATOR_DURATION		EXYNOS_PMUREG(0x361C)
#define EXYNOS4270_LOGIC_RESET_CPBLK_DURATION		EXYNOS_PMUREG(0x2D5C)
#define EXYNOS4270_REGULATOR_CPBLK_DURATION		EXYNOS_PMUREG(0x365C)

#define EXYNOS4270_MAUDIO_CONFIGURATION			EXYNOS_PMUREG(0x3CC0)
#define EXYNOS4270_AUDOMATIC_WAKEUP_MASK		(0xFFFFFEFF)
#define EXYNOS3470_AUDOMATIC_WAKEUP_MASK		(0xFFFFFEFF)

/* Only for EXYNOS5250 */
/* for EXYNOS_WAKEUP_MASK */
#define EXYNOS5_MASK_RTC_ALARM				BIT(1)
#define EXYNOS5_MASK_RTC_TICK				BIT(2)
#define EXYNOS5_MASK_KEY				BIT(5)
#define EXYNOS5_MASK_HSI				BIT(8)
#define EXYNOS5_MASK_MMC0				BIT(9)
#define EXYNOS5_MASK_MMC1				BIT(10)
#define EXYNOS5_MASK_MMC2				BIT(11)
#define EXYNOS5_MASK_MMC3				BIT(12)
#define EXYNOS5_MASK_I2S				BIT(13)
#define EXYNOS5_MASK_TIMER				BIT(14)
#define EXYNOS5_MASK_CEC				BIT(15)
#define EXYNOS5_MASK_EXT_GIC0_IRQ			BIT(16)
#define EXYNOS5_MASK_EXT_GIC0_FIQ			BIT(17)
#define EXYNOS5_MASK_EXT_GIC1_IRQ			BIT(18)
#define EXYNOS5_MASK_EXT_GIC1_FIQ			BIT(19)
#define EXYNOS5_MASK_C2C_RESET_REQ			BIT(20)
#define EXYNOS5_DEFAULT_WAKEUP_MASK			(EXYNOS5_MASK_EXT_GIC0_IRQ |\
							EXYNOS5_MASK_EXT_GIC0_FIQ |\
							EXYNOS5_MASK_EXT_GIC1_IRQ |\
							EXYNOS5_MASK_EXT_GIC1_FIQ)

/* Only for EXYNOS5 */
#define EXYNOS5250_ADC_PHY_CONTROL			EXYNOS_PMUREG(0x0718)
#define EXYNOS5_ADC_PHY_ENABLE				(1 << 0)

#define EXYNOS5_USBDEV_PHY_CONTROL			EXYNOS_PMUREG(0x0704)
#define EXYNOS5_USBDEV_PHY_ENABLE			(1 << 0)
#define EXYNOS5_USBDEV1_PHY_CONTROL			EXYNOS_PMUREG(0x0708)
#define EXYNOS5_USBDEV1_PHY_ENABLE			(1 << 0)

#define EXYNOS5_USBHOST_PHY_CONTROL			(soc_is_exynos5250() ? \
							EXYNOS_PMUREG(0x0708) : \
							EXYNOS_PMUREG(0x070C))
#define EXYNOS5_USBHOST_PHY_ENABLE			(1 << 0)

#define EXYNOS5_ADC_PHY_CONTROL				EXYNOS_PMUREG(0x0718)
#define EXYNOS5_ADC_PHY_ENABLE				(1 << 0)

#define EXYNOS5250_DPTX_PHY_CONTROL			EXYNOS_PMUREG(0x0720)
#define EXYNOS5250_DPTX_PHY_ENABLE			(1 << 0)

#define EXYNOS5_ABB_INT					EXYNOS_PMUREG(0x0780)
#define EXYNOS5_ABB_ARM					EXYNOS_PMUREG(0x0784)
#define EXYNOS5_ABB_G3D					EXYNOS_PMUREG(0x0788)
#define EXYNOS5_ABB_MIF					EXYNOS_PMUREG(0x078C)
#define EXYNOS5_ABB_MEMBER(_member)			EXYNOS_##_member

#define EXYNOS5_ARM_CORE0_SYS_PWR_REG			EXYNOS_PMUREG(0x1000)
#define EXYNOS5_DIS_IRQ_ARM_CORE0_LOCAL_SYS_PWR_REG	EXYNOS_PMUREG(0x1004)
#define EXYNOS5_DIS_IRQ_ARM_CORE0_CENTRAL_SYS_PWR_REG	EXYNOS_PMUREG(0x1008)
#define EXYNOS5_ARM_CORE1_SYS_PWR_REG			EXYNOS_PMUREG(0x1010)
#define EXYNOS5_DIS_IRQ_ARM_CORE1_LOCAL_SYS_PWR_REG	EXYNOS_PMUREG(0x1014)
#define EXYNOS5_DIS_IRQ_ARM_CORE1_CENTRAL_SYS_PWR_REG	EXYNOS_PMUREG(0x1018)
#define EXYNOS5_FSYS_ARM_SYS_PWR_REG			EXYNOS_PMUREG(0x1040)
#define EXYNOS5_DIS_IRQ_FSYS_ARM_CENTRAL_SYS_PWR_REG	EXYNOS_PMUREG(0x1048)
#define EXYNOS5_ISP_ARM_SYS_PWR_REG			EXYNOS_PMUREG(0x1050)
#define EXYNOS5_DIS_IRQ_ISP_ARM_LOCAL_SYS_PWR_REG	EXYNOS_PMUREG(0x1054)
#define EXYNOS5_DIS_IRQ_ISP_ARM_CENTRAL_SYS_PWR_REG	EXYNOS_PMUREG(0x1058)
#define EXYNOS5_ARM_COMMON_SYS_PWR_REG			EXYNOS_PMUREG(0x1080)
#define EXYNOS5_ARM_L2_SYS_PWR_REG			EXYNOS_PMUREG(0x10C0)
#define EXYNOS5_CMU_ACLKSTOP_SYS_PWR_REG		EXYNOS_PMUREG(0x1100)
#define EXYNOS5_CMU_SCLKSTOP_SYS_PWR_REG		EXYNOS_PMUREG(0x1104)
#define EXYNOS5_CMU_RESET_SYS_PWR_REG			EXYNOS_PMUREG(0x110C)
#define EXYNOS5_CMU_ACLKSTOP_SYSMEM_SYS_PWR_REG		EXYNOS_PMUREG(0x1120)
#define EXYNOS5_CMU_SCLKSTOP_SYSMEM_SYS_PWR_REG		EXYNOS_PMUREG(0x1124)
#define EXYNOS5_CMU_RESET_SYSMEM_SYS_PWR_REG		EXYNOS_PMUREG(0x112C)
#define EXYNOS5_DRAM_FREQ_DOWN_SYS_PWR_REG		EXYNOS_PMUREG(0x1130)
#define EXYNOS5_DDRPHY_DLLOFF_SYS_PWR_REG		EXYNOS_PMUREG(0x1134)
#define EXYNOS5_DDRPHY_DLLLOCK_SYS_PWR_REG		EXYNOS_PMUREG(0x1138)
#define EXYNOS5_APLL_SYSCLK_SYS_PWR_REG			EXYNOS_PMUREG(0x1140)
#define EXYNOS5_MPLL_SYSCLK_SYS_PWR_REG			EXYNOS_PMUREG(0x1144)
#define EXYNOS5_VPLL_SYSCLK_SYS_PWR_REG			EXYNOS_PMUREG(0x1148)
#define EXYNOS5_EPLL_SYSCLK_SYS_PWR_REG			EXYNOS_PMUREG(0x114C)
#define EXYNOS5_BPLL_SYSCLK_SYS_PWR_REG			EXYNOS_PMUREG(0x1150)
#define EXYNOS5_CPLL_SYSCLK_SYS_PWR_REG			EXYNOS_PMUREG(0x1154)
#define EXYNOS5_GPLL_SYSCLK_SYS_PWR_REG			EXYNOS_PMUREG(0x1158)
#define EXYNOS5_MPLLUSER_SYSCLK_SYS_PWR_REG		EXYNOS_PMUREG(0x1164)
#define EXYNOS5_BPLLUSER_SYSCLK_SYS_PWR_REG		EXYNOS_PMUREG(0x1170)
#define EXYNOS5_TOP_BUS_SYS_PWR_REG			EXYNOS_PMUREG(0x1180)
#define EXYNOS5_TOP_RETENTION_SYS_PWR_REG		EXYNOS_PMUREG(0x1184)
#define EXYNOS5_TOP_PWR_SYS_PWR_REG			EXYNOS_PMUREG(0x1188)
#define EXYNOS5_TOP_BUS_SYSMEM_SYS_PWR_REG		EXYNOS_PMUREG(0x1190)
#define EXYNOS5_TOP_RETENTION_SYSMEM_SYS_PWR_REG	EXYNOS_PMUREG(0x1194)
#define EXYNOS5_TOP_PWR_SYSMEM_SYS_PWR_REG		EXYNOS_PMUREG(0x1198)
#define EXYNOS5_LOGIC_RESET_SYS_PWR_REG			EXYNOS_PMUREG(0x11A0)
#define EXYNOS5_OSCCLK_GATE_SYS_PWR_REG			EXYNOS_PMUREG(0x11A4)
#define EXYNOS5_LOGIC_RESET_SYSMEM_SYS_PWR_REG		EXYNOS_PMUREG(0x11B0)
#define EXYNOS5_OSCCLK_GATE_SYSMEM_SYS_PWR_REG		EXYNOS_PMUREG(0x11B4)
#define EXYNOS5_USBOTG_MEM_SYS_PWR_REG			EXYNOS_PMUREG(0x11C0)
#define EXYNOS5_G2D_MEM_SYS_PWR_REG			EXYNOS_PMUREG(0x11C8)
#define EXYNOS5_USBDRD_MEM_SYS_PWR_REG			EXYNOS_PMUREG(0x11CC)
#define EXYNOS5_SDMMC_MEM_SYS_PWR_REG			EXYNOS_PMUREG(0x11D0)
#define EXYNOS5_CSSYS_MEM_SYS_PWR_REG			EXYNOS_PMUREG(0x11D4)
#define EXYNOS5_SECSS_MEM_SYS_PWR_REG			EXYNOS_PMUREG(0x11D8)
#define EXYNOS5_ROTATOR_MEM_SYS_PWR_REG			EXYNOS_PMUREG(0x11DC)
#define EXYNOS5_INTRAM_MEM_SYS_PWR_REG			EXYNOS_PMUREG(0x11E0)
#define EXYNOS5_INTROM_MEM_SYS_PWR_REG			EXYNOS_PMUREG(0x11E4)
#define EXYNOS5_JPEG_MEM_SYS_PWR_REG			EXYNOS_PMUREG(0x11E8)
#define EXYNOS5_HSI_MEM_SYS_PWR_REG			EXYNOS_PMUREG(0x11EC)
#define EXYNOS5_MCUIOP_MEM_SYS_PWR_REG			EXYNOS_PMUREG(0x11F4)
#define EXYNOS5_SATA_MEM_SYS_PWR_REG			EXYNOS_PMUREG(0x11FC)
#define EXYNOS5_PAD_RETENTION_DRAM_SYS_PWR_REG		EXYNOS_PMUREG(0x1200)
#define EXYNOS5_PAD_RETENTION_MAU_SYS_PWR_REG		EXYNOS_PMUREG(0x1204)
#define EXYNOS5_PAD_RETENTION_EFNAND_SYS_PWR_REG	EXYNOS_PMUREG(0x1208)
#define EXYNOS5_PAD_RETENTION_GPIO_SYS_PWR_REG		EXYNOS_PMUREG(0x1220)
#define EXYNOS5_PAD_RETENTION_UART_SYS_PWR_REG		EXYNOS_PMUREG(0x1224)
#define EXYNOS5_PAD_RETENTION_MMCA_SYS_PWR_REG		EXYNOS_PMUREG(0x1228)
#define EXYNOS5_PAD_RETENTION_MMCB_SYS_PWR_REG		EXYNOS_PMUREG(0x122C)
#define EXYNOS5_PAD_RETENTION_EBIA_SYS_PWR_REG		EXYNOS_PMUREG(0x1230)
#define EXYNOS5_PAD_RETENTION_EBIB_SYS_PWR_REG		EXYNOS_PMUREG(0x1234)
#define EXYNOS5_PAD_RETENTION_SPI_SYS_PWR_REG		EXYNOS_PMUREG(0x1238)
#define EXYNOS5_PAD_RETENTION_GPIO_SYSMEM_SYS_PWR_REG	EXYNOS_PMUREG(0x123C)
#define EXYNOS5_PAD_ISOLATION_SYS_PWR_REG		EXYNOS_PMUREG(0x1240)
#define EXYNOS5_PAD_ISOLATION_SYSMEM_SYS_PWR_REG	EXYNOS_PMUREG(0x1250)
#define EXYNOS5_PAD_ALV_SEL_SYS_PWR_REG			EXYNOS_PMUREG(0x1260)
#define EXYNOS5_XUSBXTI_SYS_PWR_REG			EXYNOS_PMUREG(0x1280)
#define EXYNOS5_XXTI_SYS_PWR_REG			EXYNOS_PMUREG(0x1284)
#define EXYNOS5_EXT_REGULATOR_SYS_PWR_REG		EXYNOS_PMUREG(0x12C0)
#define EXYNOS5_GPIO_MODE_SYS_PWR_REG			EXYNOS_PMUREG(0x1300)
#define EXYNOS5_GPIO_MODE_SYSMEM_SYS_PWR_REG		EXYNOS_PMUREG(0x1320)
#define EXYNOS5_GPIO_MODE_MAU_SYS_PWR_REG		EXYNOS_PMUREG(0x1340)
#define EXYNOS5_TOP_ASB_RESET_SYS_PWR_REG		EXYNOS_PMUREG(0x1344)
#define EXYNOS5_TOP_ASB_ISOLATION_SYS_PWR_REG		EXYNOS_PMUREG(0x1348)
#define EXYNOS5_GSCL_SYS_PWR_REG			EXYNOS_PMUREG(0x1400)
#define EXYNOS5_ISP_SYS_PWR_REG				EXYNOS_PMUREG(0x1404)
#define EXYNOS5_MFC_SYS_PWR_REG				EXYNOS_PMUREG(0x1408)
#define EXYNOS5_G3D_SYS_PWR_REG				EXYNOS_PMUREG(0x140C)
#define EXYNOS5_DISP1_SYS_PWR_REG			EXYNOS_PMUREG(0x1414)
#define EXYNOS5_MAU_SYS_PWR_REG				EXYNOS_PMUREG(0x1418)
#define EXYNOS5_CMU_CLKSTOP_GSCL_SYS_PWR_REG		EXYNOS_PMUREG(0x1480)
#define EXYNOS5_CMU_CLKSTOP_ISP_SYS_PWR_REG		EXYNOS_PMUREG(0x1484)
#define EXYNOS5_CMU_CLKSTOP_MFC_SYS_PWR_REG		EXYNOS_PMUREG(0x1488)
#define EXYNOS5_CMU_CLKSTOP_G3D_SYS_PWR_REG		EXYNOS_PMUREG(0x148C)
#define EXYNOS5_CMU_CLKSTOP_DISP1_SYS_PWR_REG		EXYNOS_PMUREG(0x1494)
#define EXYNOS5_CMU_CLKSTOP_MAU_SYS_PWR_REG		EXYNOS_PMUREG(0x1498)
#define EXYNOS5_CMU_SYSCLK_GSCL_SYS_PWR_REG		EXYNOS_PMUREG(0x14C0)
#define EXYNOS5_CMU_SYSCLK_ISP_SYS_PWR_REG		EXYNOS_PMUREG(0x14C4)
#define EXYNOS5_CMU_SYSCLK_MFC_SYS_PWR_REG		EXYNOS_PMUREG(0x14C8)
#define EXYNOS5_CMU_SYSCLK_G3D_SYS_PWR_REG		EXYNOS_PMUREG(0x14CC)
#define EXYNOS5_CMU_SYSCLK_DISP1_SYS_PWR_REG		EXYNOS_PMUREG(0x14D4)
#define EXYNOS5_CMU_SYSCLK_MAU_SYS_PWR_REG		EXYNOS_PMUREG(0x14D8)
#define EXYNOS5_CMU_RESET_GSCL_SYS_PWR_REG		EXYNOS_PMUREG(0x1580)
#define EXYNOS5_CMU_RESET_ISP_SYS_PWR_REG		EXYNOS_PMUREG(0x1584)
#define EXYNOS5_CMU_RESET_MFC_SYS_PWR_REG		EXYNOS_PMUREG(0x1588)
#define EXYNOS5_CMU_RESET_G3D_SYS_PWR_REG		EXYNOS_PMUREG(0x158C)
#define EXYNOS5_CMU_RESET_DISP1_SYS_PWR_REG		EXYNOS_PMUREG(0x1594)
#define EXYNOS5_CMU_RESET_MAU_SYS_PWR_REG		EXYNOS_PMUREG(0x1598)

#define EXYNOS5_ARM_CORE0_OPTION			EXYNOS_PMUREG(0x2008)
#define EXYNOS5_ARM_CORE1_OPTION			EXYNOS_PMUREG(0x2088)
#define EXYNOS5_FSYS_ARM_OPTION				EXYNOS_PMUREG(0x2208)
#define EXYNOS5_ISP_ARM_CONFIGURATION			EXYNOS_PMUREG(0x2280)
#define EXYNOS5_ISP_ARM_OPTION				EXYNOS_PMUREG(0x2288)
#define EXYNOS5_ARM_COMMON_OPTION			EXYNOS_PMUREG(0x2408)
#define EXYNOS5_ARM_L2_OPTION				EXYNOS_PMUREG(0x2608)
#define EXYNOS5_USE_RETENTION				BIT(4)
#define EXYNOS5_TOP_PWR_OPTION				EXYNOS_PMUREG(0x2C48)
#define EXYNOS5_TOP_PWR_SYSMEM_OPTION			EXYNOS_PMUREG(0x2CC8)
#define EXYNOS5_LOGIC_RESET_DURATION3			EXYNOS_PMUREG(0x2D1C)
#define EXYNOS5_DUR_WAIT_RESET_MASK			0xFFFFF
#define EXYNOS5_DUR_WAIT_RESET_MIN			0xF
#define EXYNOS5_JPEG_MEM_OPTION				EXYNOS_PMUREG(0x2F48)
#define EXYNOS5_PAD_RETENTION_SPI_OPTION		EXYNOS_PMUREG(0x31C8)
#define EXYNOS5_PAD_RETENTION_GPIO_SYSMEM_OPTION	EXYNOS_PMUREG(0x31E8)
#define EXYNOS5_GSCL_CONFIGURATION			EXYNOS_PMUREG(0x4000)
#define EXYNOS5_GSCL_STATUS				EXYNOS_PMUREG(0x4004)
#define EXYNOS5_ISP_CONFIGURATION			EXYNOS_PMUREG(0x4020)
#define EXYNOS5_ISP_STATUS				EXYNOS_PMUREG(0x4024)
#define EXYNOS5_GSCL_OPTION				EXYNOS_PMUREG(0x4008)
#define EXYNOS5_ISP_OPTION				EXYNOS_PMUREG(0x4028)
#define EXYNOS5_MFC_CONFIGURATION			EXYNOS_PMUREG(0x4040)
#define EXYNOS5_MFC_OPTION				EXYNOS_PMUREG(0x4048)
#define EXYNOS5_G3D_CONFIGURATION			EXYNOS_PMUREG(0x4060)
#define EXYNOS5_G3D_STATUS				EXYNOS_PMUREG(0x4064)
#define EXYNOS5_G3D_OPTION				EXYNOS_PMUREG(0x4068)
#define EXYNOS5_DISP1_CONFIGURATION			EXYNOS_PMUREG(0x40A0)
#define EXYNOS5_DISP1_STATUS				EXYNOS_PMUREG(0x40A4)
#define EXYNOS5_DISP1_OPTION				EXYNOS_PMUREG(0x40A8)
#define EXYNOS5_MAU_CONFIGURATION			EXYNOS_PMUREG(0x40C0)
#define EXYNOS5_MAU_OPTION				EXYNOS_PMUREG(0x40C8)
#define EXYNOS5_LOCAL_POWER_STATE_SHIFT			(16)
#define EXYNOS5_LOCAL_POWER_STATE_MASK			(0x3f)
#define EXYNOS5_USE_SC_COUNTER				(1 << 0)
#define EXYNOS5_USE_SC_FEEDBACK				(1 << 1)

#define EXYNOS5_MANUAL_L2RSTDISABLE_CONTROL		(1 << 2)
#define EXYNOS5_SKIP_DEACTIVATE_ACEACP_IN_PWDN		(1 << 7)

#define EXYNOS5_OPTION_USE_STANDBYWFI			(1 << 16)
#define EXYNOS5_OPTION_USE_STANDBYWFE			(1 << 24)

#define EXYNOS5_OPTION_USE_RETENTION			(1 << 4)

/* Only for EXYNOS5 */
#define EXYNOS_ARM_CORE2_CONFIGURATION				EXYNOS_PMUREG(0x2100)
#define EXYNOS_ARM_CORE3_CONFIGURATION				EXYNOS_PMUREG(0x2180)

/* For EXYNOS_CENTRAL_SEQ_OPTION */
#define EXYNOS5_USE_STANDBYWFI_ARM_CORE0		(1 << 16)
#define EXYNOS5_USE_STANDBYWFI_ARM_CORE1		(1 << 17)
#define EXYNOS5_USE_STANDBYWFE_ARM_CORE0		(1 << 24)
#define EXYNOS5_USE_STANDBYWFE_ARM_CORE1		(1 << 25)

#define DUR_WAIT_RESET				(0xF)
#endif /* __ASM_ARCH_REGS_PMU_H */
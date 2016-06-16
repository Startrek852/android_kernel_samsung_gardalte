# arch/arm/mach-exynos/Makefile
#
# Copyright (c) 2010-2011 Samsung Electronics Co., Ltd.
#		http://www.samsung.com/
#
# Licensed under GPLv2

obj-y	:=
obj-m	:=
obj-n	:=
obj-	:=

KBUILD_CFLAGS_KERNEL := -Werror -fdiagnostics-show-option \
			-Werror -Wno-error=unused-function \
			-Wno-error=unused-variable \
			-Wno-error=unused-value \
			-Wno-error=unused-label
export KBUILD_CFLAGS_KERNEL

# Core

obj-$(CONFIG_ARCH_EXYNOS)	+= common.o sec_class.o
obj-$(CONFIG_ARCH_EXYNOS4)	+= clock-exynos4.o
obj-$(CONFIG_ARM_TRUSTZONE)	+= irq-sgi.o
obj-$(CONFIG_CPU_EXYNOS4210)	+= clock-exynos4210.o
obj-$(CONFIG_SOC_EXYNOS4212)	+= clock-exynos4212.o
obj-$(CONFIG_SOC_EXYNOS4270)	+= clock-exynos4270.o pmu-cp.o asv.o asv-exynos4270.o
obj-$(CONFIG_SOC_EXYNOS5250)	+= clock-exynos5250.o
obj-$(CONFIG_SOC_EXYNOS3470)	+= clock-exynos3470.o pmu-cp.o asv-exynos.o asv-exynos3470.o
obj-$(CONFIG_EXYNOS4_BTS)	+= bts-exynos3470.o

obj-$(CONFIG_PM)		+= pm.o
obj-$(CONFIG_PM_GENERIC_DOMAINS)	+= pm_domains.o pm_domains4.o

ifeq ($(CONFIG_SOC_EXYNOS5250),y)
obj-$(CONFIG_CPU_IDLE)		+= cpuidle-exynos5250.o
else
obj-$(CONFIG_CPU_IDLE)		+= cpuidle.o
endif

obj-$(CONFIG_SOC_EXYNOS5250)	+= ori-asv-exynos.o ori-abb-exynos.o ori-asv-exynos5250.o

obj-$(CONFIG_ARCH_EXYNOS)	+= pmu.o

obj-$(CONFIG_SMP)		+= platsmp.o headsmp.o

obj-$(CONFIG_EXYNOS4_MCT)	+= mct.o

obj-$(CONFIG_HOTPLUG_CPU)	+= hotplug.o

obj-$(CONFIG_ARCH_EXYNOS)	+= clock-audss.o

obj-$(CONFIG_EXYNOS_FIQ_DEBUGGER)	+= exynos_fiq_debugger.o

obj-$(CONFIG_EXYNOS_BUSFREQ_OPP)	+= ppmu.o busfreq_opp_exynos4.o busfreq_opp_4x12.o

obj-$(CONFIG_EXYNOS5_CORESIGHT)		+= coresight-exynos5.o
obj-$(CONFIG_EXYNOS_PERSISTENT_CLOCK)	+= persistent_clock.o

obj-$(CONFIG_ARM_TRUSTZONE)		+= smc.o
plus_sec := $(call as-instr,.arch_extension sec,+sec)
AFLAGS_smc.o :=-Wa,-march=armv7-a$(plus_sec)

# machine support

obj-$(CONFIG_MACH_SMDKC210)		+= mach-smdkv310.o
obj-$(CONFIG_MACH_SMDKV310)		+= mach-smdkv310.o
obj-$(CONFIG_MACH_ARMLEX4210)		+= mach-armlex4210.o
obj-$(CONFIG_MACH_UNIVERSAL_C210)	+= mach-universal_c210.o
obj-$(CONFIG_MACH_NURI)			+= mach-nuri.o
obj-$(CONFIG_MACH_ORIGEN)		+= mach-origen.o

obj-$(CONFIG_MACH_SMDK4212)		+= mach-smdk4x12.o
obj-$(CONFIG_MACH_SMDK4412)		+= mach-smdk4x12.o
obj-$(CONFIG_MACH_SMDK4412)		+= board-smdk4x12-mmc.o
obj-$(CONFIG_MACH_SMDK4412)		+= board-smdk4x12-audio.o
obj-$(CONFIG_MACH_SMDK4412)		+= board-smdk4x12-display.o
obj-$(CONFIG_MACH_SMDK4412)		+= board-smdk4x12-usb.o
obj-$(CONFIG_MACH_SMDK4412)		+= board-smdk4x12-media.o
obj-$(CONFIG_MACH_SMDK4412)		+= board-smdk4x12-power.o

obj-$(CONFIG_MACH_SMDK4270)		+= mach-smdk4270.o
obj-$(CONFIG_MACH_SMDK4270)		+= board-smdk4270-power.o
obj-$(CONFIG_MACH_SMDK4270)		+= board-smdk4270-mmc.o
obj-$(CONFIG_MACH_SMDK4270)		+= board-smdk4270-usb.o
obj-$(CONFIG_MACH_SMDK4270)		+= board-smdk4270-display.o
obj-$(CONFIG_MACH_SMDK4270)		+= board-smdk4270-input.o
obj-$(CONFIG_MACH_SMDK4270)		+= board-smdk4270-audio.o
obj-$(CONFIG_MACH_SMDK4270)		+= board-smdk4270-media.o
obj-$(CONFIG_MACH_SMDK4270)		+= board-smdk4270-clock.o

obj-$(CONFIG_MACH_SMDK3470)		+= mach-smdk4270.o
obj-$(CONFIG_MACH_SMDK3470)		+= board-smdk4270-clock.o
obj-$(CONFIG_MACH_SMDK3470)		+= board-smdk4270-mmc.o
obj-$(CONFIG_MACH_SMDK3470)		+= board-smdk4270-usb.o
obj-$(CONFIG_MACH_SMDK3470)		+= board-smdk4270-display.o
obj-$(CONFIG_MACH_SMDK3470)		+= board-smdk4270-media.o
obj-$(CONFIG_MACH_SMDK3470)		+= board-smdk4270-input.o
obj-$(CONFIG_MACH_SMDK3470)		+= board-smdk4270-audio.o

obj-$(CONFIG_MACH_UNIVERSAL222AP)	+= mach-universal222ap.o
obj-$(CONFIG_MACH_UNIVERSAL222AP)	+= board-universal222ap-power.o
obj-$(CONFIG_MACH_UNIVERSAL222AP)	+= board-universal222ap-mmc.o
obj-$(CONFIG_MACH_UNIVERSAL222AP)	+= board-universal222ap-wifi.o
obj-$(CONFIG_MACH_UNIVERSAL222AP)	+= board-universal222ap-usb.o
obj-$(CONFIG_MACH_UNIVERSAL222AP)	+= board-universal222ap-display.o
obj-$(CONFIG_MACH_UNIVERSAL222AP)	+= board-universal222ap-input.o
obj-$(CONFIG_MACH_UNIVERSAL222AP)	+= board-universal222ap-media.o
obj-$(CONFIG_MACH_UNIVERSAL222AP)	+= board-universal222ap-fpga.o
obj-$(CONFIG_MACH_UNIVERSAL222AP)	+= board-universal222ap-gpio.o
obj-$(CONFIG_MACH_UNIVERSAL222AP)	+= board-universal222ap-gpio-i2c.o
obj-$(CONFIG_MACH_UNIVERSAL222AP)	+= board-universal222ap-clock.o
obj-$(CONFIG_MACH_UNIVERSAL222AP)	+= board-universal222ap-audio.o
obj-$(CONFIG_MACH_UNIVERSAL222AP)	+= board-universal222ap-camera.o
obj-$(CONFIG_MACH_UNIVERSAL222AP)	+= board-universal222ap-muic.o
ifeq ($(CONFIG_SOC_EXYNOS4270),y)
obj-$(CONFIG_MACH_UNIVERSAL222AP)	+= board-universal222ap-battery.o
endif
ifeq ($(CONFIG_MACH_GARDA),y)
obj-$(CONFIG_MACH_UNIVERSAL222AP)	+= board-universal222ap-gardabattery.o
endif
obj-$(CONFIG_MACH_UNIVERSAL222AP)	+= sec-reboot.o

obj-$(CONFIG_MACH_GARDA)		+= mach-universal222ap.o
obj-$(CONFIG_MACH_GARDA)		+= board-garda-power.o
obj-$(CONFIG_MACH_GARDA)		+= board-garda-mmc.o
obj-$(CONFIG_MACH_GARDA)		+= board-garda-usb.o
obj-$(CONFIG_MACH_GARDA)		+= board-garda-display.o
obj-$(CONFIG_MACH_GARDA)		+= board-garda-input.o
obj-$(CONFIG_MACH_GARDA)		+= board-garda-media.o
obj-$(CONFIG_MACH_GARDA)		+= board-garda-fpga.o
obj-$(CONFIG_MACH_GARDA)		+= board-garda-gpio.o
obj-$(CONFIG_MACH_GARDA)		+= board-garda-gpio-i2c.o
obj-$(CONFIG_MACH_GARDA)		+= board-garda-clock.o
obj-$(CONFIG_MACH_GARDA)		+= board-garda-audio.o
obj-$(CONFIG_MACH_GARDA)		+= board-garda-camera.o
obj-$(CONFIG_MACH_GARDA)		+= board-garda-muic.o
obj-$(CONFIG_MACH_GARDA)		+= board-garda-battery.o

ifeq ($(CONFIG_MACH_GARDA),y)
ifeq ($(CONFIG_TARGET_LOCALE_KOR),y)
obj-$(CONFIG_MACH_GARDA)		+= board-delos-kor-bt.o
obj-$(CONFIG_MACH_GARDA)		+= board-delos-kor-sensor.o
else
obj-$(CONFIG_MACH_GARDA)		+= board-garda-sensor.o
endif
endif

obj-$(CONFIG_MACH_GARDA)		+= sec-reboot.o
obj-$(CONFIG_MACH_GARDA)		+= board-garda-wlan.o
obj-$(CONFIG_MACH_GARDA)		+= board-garda-thermistor.o

obj-$(CONFIG_MACH_CARMEN)		+= mach-universal222ap.o
obj-$(CONFIG_MACH_CARMEN)		+= board-carmen-power.o
obj-$(CONFIG_MACH_CARMEN)		+= board-carmen-mmc.o
obj-$(CONFIG_MACH_CARMEN)		+= board-carmen-usb.o
obj-$(CONFIG_MACH_CARMEN)		+= board-carmen-display.o
obj-$(CONFIG_MACH_CARMEN)		+= board-carmen-input.o
obj-$(CONFIG_MACH_CARMEN)		+= board-carmen-media.o
obj-$(CONFIG_MACH_CARMEN)		+= board-carmen-fpga.o
obj-$(CONFIG_MACH_CARMEN)		+= board-carmen-gpio.o
obj-$(CONFIG_MACH_CARMEN)		+= board-carmen-gpio-i2c.o
obj-$(CONFIG_MACH_CARMEN)		+= board-carmen-clock.o
obj-$(CONFIG_MACH_CARMEN)		+= board-carmen-audio.o
obj-$(CONFIG_MACH_CARMEN)		+= board-carmen-camera.o
obj-$(CONFIG_MACH_CARMEN)		+= board-carmen-muic.o
obj-$(CONFIG_MACH_CARMEN)		+= board-carmen-battery.o
obj-$(CONFIG_MACH_CARMEN)		+= board-carmen-sensor.o
obj-$(CONFIG_MACH_CARMEN)		+= sec-reboot.o
obj-$(CONFIG_MACH_CARMEN)		+= board-carmen-wlan.o
obj-$(CONFIG_MACH_CARMEN)		+= board-carmen-thermistor.o

obj-$(CONFIG_MACH_WICHITA)		+= mach-universal222ap.o
obj-$(CONFIG_MACH_WICHITA)		+= board-wichita-power.o
obj-$(CONFIG_MACH_WICHITA)		+= board-wichita-mmc.o
obj-$(CONFIG_MACH_WICHITA)		+= board-wichita-usb.o
obj-$(CONFIG_MACH_WICHITA)		+= board-wichita-display.o
obj-$(CONFIG_MACH_WICHITA)		+= board-wichita-input.o
obj-$(CONFIG_MACH_WICHITA)		+= board-wichita-media.o
obj-$(CONFIG_MACH_WICHITA)		+= board-wichita-fpga.o
obj-$(CONFIG_MACH_WICHITA)		+= board-wichita-gpio.o
obj-$(CONFIG_MACH_WICHITA)		+= board-wichita-gpio-i2c.o
obj-$(CONFIG_MACH_WICHITA)		+= board-wichita-clock.o
obj-$(CONFIG_MACH_WICHITA)		+= board-wichita-audio.o
obj-$(CONFIG_MACH_WICHITA)		+= board-wichita-camera.o
obj-$(CONFIG_MACH_WICHITA)		+= board-wichita-muic.o
obj-$(CONFIG_MACH_WICHITA)		+= board-wichita-battery.o
obj-$(CONFIG_MACH_WICHITA)		+= board-wichita-sensor.o
obj-$(CONFIG_MACH_WICHITA)		+= sec-reboot.o
obj-$(CONFIG_MACH_WICHITA)		+= board-wichita-wlan.o
obj-$(CONFIG_MACH_WICHITA)		+= board-wichita-thermistor.o

obj-$(CONFIG_MACH_KMINI)		+= mach-universal222ap.o
obj-$(CONFIG_MACH_KMINI)		+= board-kmini-power.o
obj-$(CONFIG_MACH_KMINI)		+= board-kmini-mmc.o
obj-$(CONFIG_MACH_KMINI)		+= board-kmini-usb.o
obj-$(CONFIG_MACH_KMINI)		+= board-kmini-display.o
obj-$(CONFIG_MACH_KMINI)		+= board-kmini-input.o
obj-$(CONFIG_MACH_KMINI)		+= board-kmini-media.o
obj-$(CONFIG_MACH_KMINI)		+= board-kmini-fpga.o
obj-$(CONFIG_MACH_KMINI)		+= board-kmini-gpio.o
obj-$(CONFIG_MACH_KMINI)		+= board-kmini-gpio-i2c.o
obj-$(CONFIG_MACH_KMINI)		+= board-kmini-clock.o
obj-$(CONFIG_MACH_KMINI)		+= board-kmini-audio.o
obj-$(CONFIG_MACH_KMINI)		+= board-kmini-camera.o
obj-$(CONFIG_MACH_KMINI)		+= board-kmini-muic.o
obj-$(CONFIG_MACH_KMINI)		+= board-kmini-battery.o
obj-$(CONFIG_MACH_KMINI)		+= board-kmini-mfd.o
obj-$(CONFIG_MACH_KMINI)		+= board-kmini-sensor.o
obj-$(CONFIG_MACH_KMINI)		+= sec-reboot.o
obj-$(CONFIG_MACH_KMINI)		+= board-kmini-wlan.o
obj-$(CONFIG_MACH_KMINI)		+= board-kmini-thermistor.o
obj-$(CONFIG_MACH_KMINI)		+= board-kmini-led.o

obj-$(CONFIG_MACH_LT02)			+= mach-universal222ap.o
obj-$(CONFIG_MACH_LT02)			+= board-lt02-power.o
obj-$(CONFIG_MACH_LT02)			+= board-lt02-mmc.o
obj-$(CONFIG_MACH_LT02)			+= board-lt02-wifi.o
obj-$(CONFIG_MACH_LT02)			+= board-lt02-usb.o
obj-$(CONFIG_MACH_LT02)			+= board-lt02-display.o
obj-$(CONFIG_MACH_LT02)			+= board-lt02-input.o
obj-$(CONFIG_MACH_LT02)			+= board-lt02-media.o
obj-$(CONFIG_MACH_LT02)			+= board-lt02-fpga.o
obj-$(CONFIG_MACH_LT02)			+= board-lt02-gpio.o
obj-$(CONFIG_MACH_LT02)			+= board-lt02-gpio-i2c.o
obj-$(CONFIG_MACH_LT02)			+= board-lt02-clock.o
obj-$(CONFIG_MACH_LT02)			+= board-lt02-audio.o
obj-$(CONFIG_MACH_LT02)			+= board-lt02-muic.o

obj-$(CONFIG_MACH_EXYNOS4_DT)		+= mach-exynos4-dt.o
obj-$(CONFIG_MACH_EXYNOS5_DT)		+= mach-exynos5-dt.o

obj-$(CONFIG_MACH_SMDK5250)		+= mach-smdk5250.o

obj-$(CONFIG_EXYNOS_SOUND_PLATFORM_DATA) += exynos_sound_platform_data.o

# device support

obj-y					+= dev-uart.o
obj-$(CONFIG_ARCH_EXYNOS)		+= dev-audio.o
obj-$(CONFIG_EXYNOS4_DEV_AHCI)		+= dev-ahci.o
obj-$(CONFIG_EXYNOS_DEV_DWMCI)		+= dev-dwmci.o
obj-$(CONFIG_EXYNOS_DEV_FIMC_IS)	+= dev-fimc-is.o
obj-$(CONFIG_EXYNOS5_DEV_FIMC_IS)	+= dev-fimc-is.o
obj-$(CONFIG_EXYNOS4_DEV_FIMC_LITE)	+= dev-fimc-lite.o
obj-$(CONFIG_EXYNOS5_DEV_GSC)		+= dev-gsc.o
obj-$(CONFIG_EXYNOS5_DEV_SCALER)	+= dev-scaler.o
obj-$(CONFIG_EXYNOS_DEV_ROTATOR)	+= dev-rotator.o
obj-$(CONFIG_EXYNOS_DEV_SYSMMU)		+= dev-sysmmu.o
obj-$(CONFIG_EXYNOS_DEV_DMA)		+= dma.o
obj-$(CONFIG_EXYNOS_DEV_USB_SWITCH)	+= dev-usb-switch.o
obj-$(CONFIG_EXYNOS4_DEV_USB_OHCI)	+= dev-ohci.o
obj-$(CONFIG_EXYNOS5_DEV_USB3_DRD)	+= dev-usb3-drd.o
obj-$(CONFIG_EXYNOS4_DEV_JPEG)		+= dev-jpeg.o
obj-$(CONFIG_EXYNOS5_DEV_JPEG)		+= dev-jpeg.o
obj-$(CONFIG_EXYNOS5_DEV_JPEG_HX)	+= dev-jpeg-hx.o
obj-$(CONFIG_EXYNOS_DEV_TMU)		+= dev-tmu.o
obj-$(CONFIG_EXYNOS5_DEV_BTS)		+= dev-bts.o
obj-$(CONFIG_EXYNOS5_DEV_HSI2C0)	+= dev-hs-i2c0.o
obj-$(CONFIG_EXYNOS5_DEV_HSI2C1)	+= dev-hs-i2c1.o
obj-$(CONFIG_EXYNOS5_DEV_HSI2C2)	+= dev-hs-i2c2.o
obj-$(CONFIG_EXYNOS5_DEV_HSI2C3)	+= dev-hs-i2c3.o

# S.LSI AP/CP one-chip solution with s shared memory interface
obj-$(CONFIG_EXYNOS_DEV_MODEM_SS222)	+= dev-modem-ss222.o
obj-$(CONFIG_EXYNOS_DEV_SHARED_MEMORY)	+= dev-shdmem.o
obj-$(CONFIG_EXYNOS4_DEV_MCU_IPC)	+= dev-mcu_ipc.o

obj-$(CONFIG_ARCH_EXYNOS)		+= setup-i2c0.o
obj-$(CONFIG_EXYNOS4_SETUP_FIMC)	+= setup-fimc.o
obj-$(CONFIG_EXYNOS4_SETUP_CSIS)	+= setup-csis.o
obj-$(CONFIG_EXYNOS5_SETUP_GSC)		+= setup-gsc.o
obj-$(CONFIG_EXYNOS4_SETUP_FIMD0)	+= setup-fimd0.o
obj-$(CONFIG_EXYNOS_SETUP_FIMD1)	+= setup-fimd1.o
obj-$(CONFIG_EXYNOS_SETUP_DP)		+= setup-dp.o
obj-$(CONFIG_FB_MIPI_DSIM)		+= setup-mipidsim.o
obj-$(CONFIG_EXYNOS_CONTENT_PATH_PROTECTION) += secmem.o
obj-$(CONFIG_EXYNOS_SETUP_FIMC_IS)	+= setup-fimc-is.o
obj-$(CONFIG_EXYNOS5_SETUP_FIMC_IS)	+= setup-fimc-is.o
obj-$(CONFIG_VISION_MODE)		+= setup-fimc-is-sensor.o
obj-$(CONFIG_EXYNOS4_SETUP_I2C1)	+= setup-i2c1.o
obj-$(CONFIG_EXYNOS4_SETUP_I2C2)	+= setup-i2c2.o
obj-$(CONFIG_EXYNOS4_SETUP_I2C3)	+= setup-i2c3.o
obj-$(CONFIG_EXYNOS4_SETUP_I2C4)	+= setup-i2c4.o
obj-$(CONFIG_EXYNOS4_SETUP_I2C5)	+= setup-i2c5.o
obj-$(CONFIG_EXYNOS4_SETUP_I2C6)	+= setup-i2c6.o
obj-$(CONFIG_EXYNOS4_SETUP_I2C7)	+= setup-i2c7.o
obj-$(CONFIG_EXYNOS5_SETUP_HSI2C0)	+= setup-hs-i2c0.o
obj-$(CONFIG_EXYNOS5_SETUP_HSI2C1)	+= setup-hs-i2c1.o
obj-$(CONFIG_EXYNOS5_SETUP_HSI2C2)	+= setup-hs-i2c2.o
obj-$(CONFIG_EXYNOS5_SETUP_HSI2C3)	+= setup-hs-i2c3.o
obj-$(CONFIG_EXYNOS4_SETUP_KEYPAD)	+= setup-keypad.o
obj-$(CONFIG_EXYNOS4_SETUP_MFC)		+= setup-mfc.o
obj-$(CONFIG_EXYNOS4_SETUP_SDHCI_GPIO)	+= setup-sdhci-gpio.o
obj-$(CONFIG_EXYNOS4_SETUP_USB_PHY)	+= setup-usb-phy.o
obj-$(CONFIG_EXYNOS_SETUP_SPI)		+= setup-spi.o
obj-$(CONFIG_EXYNOS5_SETUP_TVOUT)	+= setup-tvout.o
obj-$(CONFIG_EXYNOS_SETUP_ADC)		+= setup-adc.o
obj-$(CONFIG_ION_EXYNOS)		+= dev-ion.o
obj-$(CONFIG_CMA)			+= reserve-mem.o
obj-$(CONFIG_EXYNOS4_SETUP_JPEG)	+= setup-jpeg.o
obj-$(CONFIG_EXYNOS5_SETUP_JPEG)	+= setup-jpeg.o
obj-$(CONFIG_EXYNOS5_SETUP_JPEG_HX)	+= setup-jpeg-hx.o
obj-$(CONFIG_ARCH_EXYNOS5)		+= resetreason.o
obj-$(CONFIG_EXYNOS_TMU)		+= tmu-exynos.o
obj-$(CONFIG_SAMSUNG_MUIC)		+= sec-switch.o

obj-$(CONFIG_BT_BCM4330)		+= board-bluetooth-bcm4330.o
obj-$(CONFIG_BT_BCM4334)		+= board-bluetooth-bcm4334.o
obj-$(CONFIG_BT_BCM43241)		+= board-bluetooth-bcm43241.o
obj-$(CONFIG_BT_BCM4335)		+= board-bluetooth-bcm4335.o

# samsung mobile features

obj-$(CONFIG_SEC_DEBUG)			+= sec_debug.o sec_getlog.o sec_gaf.o
obj-$(CONFIG_SEC_WATCHDOG_RESET)	+= sec_watchdog.o
obj-$(CONFIG_SEC_LOG)			+= sec_log.o

ifeq ($(CONFIG_GPS_BCMxxxxx),y)
obj-$(CONFIG_GPS_BCM4752)		+= sec_gps_bcm4752.o
endif

ifeq ($(CONFIG_GPS_CSRxT),y)
obj-$(CONFIG_GPS_GSD5T)		+= sec_gps_gsd5t.o
endif

obj-$(CONFIG_SEC_THERMISTOR)		+= sec_thermistor.o

# Samsung Mobile Modem Interface (configuration set)
obj-$(CONFIG_SEC_MODEM_SMDK4270)	+= board-smdk4270-modems.o
obj-$(CONFIG_SEC_MODEM_SMDK3470)	+= board-smdk4270-modems.o
obj-$(CONFIG_SEC_MODEM_UNIVERSAL222AP)	+= board-universal222ap-modems.o
obj-$(CONFIG_SEC_MODEM_GARDA)		+= board-garda-modems.o
obj-$(CONFIG_SEC_MODEM_CARMEN)		+= board-carmen-modems.o
obj-$(CONFIG_SEC_MODEM_WICHITA)		+= board-wichita-modems.o
obj-$(CONFIG_SEC_MODEM_KMINI)		+= board-kmini-modems.o
obj-$(CONFIG_SEC_MODEM_LT02)		+= board-lt02-modems.o

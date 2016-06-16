/*
 * Copyright (c) 2013 Samsung Electronics Co., Ltd.
 *		http://www.samsung.com
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 */

#include <linux/platform_device.h>
#include <linux/persistent_ram.h>
#include <linux/serial_core.h>
#include <linux/memblock.h>
#include <linux/mmc/host.h>
#include <linux/io.h>
#include <linux/notifier.h>
#include <linux/reboot.h>
#include <linux/gpio.h>
#include <plat/gpio-cfg.h>
#include <linux/delay.h>
#include <linux/regulator/consumer.h>

#include <asm/mach/arch.h>
#include <asm/hardware/gic.h>
#include <asm/mach-types.h>

#include <plat/regs-serial.h>
#include <plat/cpu.h>
#include <plat/clock.h>
#include <plat/devs.h>
#include <plat/iic.h>
#include <plat/adc.h>
#include <plat/watchdog.h>
#include <mach/map.h>
#include <mach/pmu.h>

#include <mach/gpio.h>
#include <mach/regs-pmu.h>

#include <mach/board-bluetooth-bcm.h>

#ifdef CONFIG_EXYNOS_DEV_MODEM_SS222
#include <mach/shdmem.h>
#endif

#include "common.h"
#include "board-universal222ap.h"
#if defined(CONFIG_MACH_GARDA)
#include "board-garda.h"
#include "board-garda-wlan.h"
#elif defined(CONFIG_MACH_CARMEN)
#include "board-carmen.h"
#include "board-carmen-wlan.h"
#elif defined(CONFIG_MACH_WICHITA)
#include "board-wichita.h"
#include "board-wichita-wlan.h"
#elif defined(CONFIG_MACH_KMINI)
#include "board-kmini.h"
#include "board-kmini-wlan.h"
#endif

#ifdef CONFIG_SEC_DEBUG
#include "mach/sec_debug.h"
#endif
#ifdef CONFIG_DC_MOTOR
#include <linux/dc_motor.h>
#endif
#ifdef CONFIG_MOTOR_DRV_DRV2603
#include <linux/drv2603_vibrator.h>
#endif

static struct platform_device ramconsole_device = {
	.name	= "ram_console",
	.id	= -1,
};

static struct platform_device persistent_trace_device = {
	.name	= "persistent_trace",
	.id	= -1,
};

/* Following are default values for UCON, ULCON and UFCON UART registers */
#define SMDK4270_UCON_DEFAULT	(S3C2410_UCON_TXILEVEL |	\
				 S3C2410_UCON_RXILEVEL |	\
				 S3C2410_UCON_TXIRQMODE |	\
				 S3C2410_UCON_RXIRQMODE |	\
				 S3C2410_UCON_RXFIFO_TOI |	\
				 S3C2443_UCON_RXERR_IRQEN)

#define SMDK4270_ULCON_DEFAULT	S3C2410_LCON_CS8

#define SMDK4270_UFCON_DEFAULT	(S3C2410_UFCON_FIFOMODE |	\
				 S5PV210_UFCON_TXTRIG4 |	\
				 S5PV210_UFCON_RXTRIG4)

static struct s3c2410_uartcfg smdk4270_uartcfgs[] __initdata = {
	[0] = {
		.hwport		= 0,
		.flags		= 0,
		.ucon		= SMDK4270_UCON_DEFAULT,
		.ulcon		= SMDK4270_ULCON_DEFAULT,
		.ufcon		= SMDK4270_UFCON_DEFAULT,
#if defined(CONFIG_BT_BCM4334) || defined(CONFIG_BT_BCM4335)
		.wake_peer	= bcm_bt_lpm_exit_lpm_locked,
#endif
	},
	[1] = {
		.hwport		= 1,
		.flags		= 0,
		.ucon		= SMDK4270_UCON_DEFAULT,
		.ulcon		= SMDK4270_ULCON_DEFAULT,
		.ufcon		= SMDK4270_UFCON_DEFAULT,
	},
	[2] = {
		.hwport		= 2,
		.flags		= 0,
		.ucon		= SMDK4270_UCON_DEFAULT,
		.ulcon		= SMDK4270_ULCON_DEFAULT,
		.ufcon		= SMDK4270_UFCON_DEFAULT,
	},
	[3] = {
		.hwport		= 3,
		.flags		= 0,
		.ucon		= SMDK4270_UCON_DEFAULT,
		.ulcon		= SMDK4270_ULCON_DEFAULT,
		.ufcon		= SMDK4270_UFCON_DEFAULT,
	},
};

static int hw_rev;
static struct gpio hw_rev_gpios[] = {
	{ EXYNOS4_GPF3(2), GPIOF_IN, "hw_rev0" },
	{ EXYNOS4_GPF3(3), GPIOF_IN, "hw_rev1" },
	{ EXYNOS4_GPF2(5), GPIOF_IN, "hw_rev2" },
	{ EXYNOS4_GPF2(6), GPIOF_IN, "hw_rev3" },
};

static char board_info_string[255];

static void __init init_hw_rev(void)
{
	int ret;
	int i;

	ret = gpio_request_array(hw_rev_gpios,
			ARRAY_SIZE(hw_rev_gpios));

	for (i = 0; i < ARRAY_SIZE(hw_rev_gpios); i++)
		s3c_gpio_setpull(hw_rev_gpios[i].gpio, S3C_GPIO_PULL_NONE);

	udelay(9);

	BUG_ON(ret);
	for (i = 0; i < ARRAY_SIZE(hw_rev_gpios); i++)
		hw_rev |= gpio_get_value(hw_rev_gpios[i].gpio) << i;

	snprintf(board_info_string, sizeof(board_info_string) - 1,
			"HW revision: %d, CPU EXYNOS3470 Rev%d.%d",
			hw_rev,
			samsung_rev() >> 4,
			samsung_rev() & 0xf);
	pr_info("%s\n", board_info_string);
	mach_panic_string = board_info_string;
}

int exynos3_get_revision(void)
{
#if defined(CONFIG_MACH_DELOSLTE_KOR_SKT) ||\
	defined(CONFIG_MACH_DELOSLTE_KOR_LGT)
	return (hw_rev < 0x8) ? (hw_rev+0x0f) : hw_rev;
#else
	return hw_rev;
#endif
}

/* ADC */
static struct s3c_adc_platdata smdk_adc_data __initdata = {
	.phy_init       = s3c_adc_phy_init,
	.phy_exit       = s3c_adc_phy_exit,
};
/* WDT */
static struct s3c_watchdog_platdata smdk4270_watchdog_platform_data = {
	exynos_pmu_wdt_control,
	PMU_WDT_RESET_TYPE0,
};

#ifdef CONFIG_SEC_WATCHDOG_RESET
static struct platform_device watchdog_reset_device = {
	.name = "watchdog-reset",
	.id = -1,
};
#endif

#ifdef CONFIG_DC_MOTOR
#if defined(CONFIG_MACH_KMINI)
void __init dc_motor_init(void)
{
	int ret;
	int gpio;

	gpio = GPIO_MOTOR_EN;
	
	ret = gpio_request(gpio, "MOTOR_EN");
	if (ret)
		pr_err("failed to request gpio(TSP_INT)(%d)\n", ret);
	gpio_direction_output(GPIO_MOTOR_EN, 0);
}
#endif

int vibrator_on;
void dc_motor_power_regulator(bool on)
{
	struct regulator *regulator;
	regulator = regulator_get(NULL, "vdd_motor_3v0");

	if (on) {
		vibrator_on = 1;
		regulator_enable(regulator);
	} else {
		if (regulator_is_enabled(regulator))
			regulator_disable(regulator);
		else
			regulator_force_disable(regulator);
		if (vibrator_on == 1)
			msleep(120);
		vibrator_on = 0;
	}
	regulator_put(regulator);
}

void dc_motor_power_gpio(bool on)
{
	gpio_direction_output(GPIO_MOTOR_EN, on);
}

static struct dc_motor_platform_data dc_motor_pdata = {
#if defined(CONFIG_MACH_KMINI)
	.power = dc_motor_power_gpio,
#else
	.power = dc_motor_power_regulator,
#endif
	.max_timeout = 10000,
};

static struct platform_device dc_motor_device = {
	.name = "sec-vibrator",
	.id = -1,
	.dev = {
		.platform_data = &dc_motor_pdata,
	},
};
#endif

#ifdef CONFIG_MOTOR_DRV_DRV2603
static void drv2603_motor_init(void)
{
	int err;
	printk(KERN_ERR "[VIB] %s\n",__func__);
	err = gpio_request(GPIO_MOTOR_EN, "VIB_ON");
	if (err)
		printk(KERN_DEBUG "%s gpio_request error!\n", __func__);
	else {
		gpio_direction_output(GPIO_MOTOR_EN, 0);
		gpio_export(GPIO_MOTOR_EN, 0);
	}
	err = gpio_request(GPIO_MOTOR_PWM, "VIB_ON");
	if (err)
		printk(KERN_DEBUG "%s gpio_request error!\n", __func__);
	else {
		s3c_gpio_cfgpin(GPIO_MOTOR_PWM, S3C_GPIO_SFN(0x2));
		s3c_gpio_setpull(GPIO_MOTOR_PWM, S3C_GPIO_PULL_NONE);
	}
}

static int drv2603_motor_en(bool en)
{
	return gpio_direction_output(GPIO_MOTOR_EN, en);
}

static struct drv2603_vibrator_platform_data motor_pdata = {
	.gpio_en = drv2603_motor_en,
	.max_timeout = 10000,
	.pwm_id = 0,
	.pwm_duty = 38000,
	.pwm_period = 38100,
};

static struct platform_device sec_motor = {
	.name	= "drv2603_vibrator",
	.id	= -1,
	.dev = {
		.platform_data = &motor_pdata,
	}
};
#endif
/*rfkill device registeration*/
#ifdef CONFIG_BT_BCM4334
static struct platform_device bcm4334_bluetooth_device = {
	.name = "bcm4334_bluetooth",
	.id = -1,
};
#else
#ifdef CONFIG_BT_BCM4335
static struct platform_device bcm4335_bluetooth_device = {
	.name = "bcm4335_bluetooth",
	.id = -1,
};
#endif
#endif

#ifdef CONFIG_EXYNOS_PERSISTENT_CLOCK
static struct resource persistent_clock_resource[] = {
 	[0] = DEFINE_RES_MEM(S3C_PA_RTC, SZ_256),
};

static struct platform_device persistent_clock = {
	.name           = "persistent_clock",
	.id             = -1,
	.num_resources	= ARRAY_SIZE(persistent_clock_resource),
	.resource	= persistent_clock_resource,
};
#endif /*CONFIG_EXYNOS_PERSISTENT_CLOCK*/

static struct platform_device *smdk4270_devices[] __initdata = {
	&ramconsole_device,
#ifdef CONFIG_SEC_WATCHDOG_RESET
	&watchdog_reset_device,
#endif
#ifdef CONFIG_EXYNOS_PERSISTENT_CLOCK
	&persistent_clock,
#endif
	&s3c_device_rtc,
	&exynos4_device_g3d,
#if !defined(CONFIG_MACH_DELOSLTE_KOR_SKT) && !defined(CONFIG_MACH_DELOSLTE_KOR_KT) \
	&& !defined(CONFIG_MACH_DELOSLTE_KOR_LGT) && !defined(CONFIG_MACH_KMINI)
 	&s3c_device_i2c4,
#endif
#ifndef CONFIG_SEC_WATCHDOG_RESET
	&s3c_device_wdt,
#endif
	&s3c_device_adc,
#ifdef CONFIG_S5P_DEV_ACE
	&s5p_device_ace,
#endif
#ifdef CONFIG_DC_MOTOR
	&dc_motor_device,
#endif
#ifdef CONFIG_MOTOR_DRV_DRV2603
	&sec_motor,
#endif
#ifdef CONFIG_BT_BCM4334
	&bcm4334_bluetooth_device,
#else
#ifdef CONFIG_BT_BCM4335
    &bcm4335_bluetooth_device,
#endif
#endif

};

static int exynos4_notifier_call(struct notifier_block *this,
		unsigned long code, void *_cmd)
{
	int mode = 0;

	if ((code == SYS_RESTART) && _cmd)
		if (!strcmp((char *)_cmd, "recovery"))
			mode = 0xf;

	__raw_writel(mode, EXYNOS_INFORM4);

	return NOTIFY_DONE;
}

static struct notifier_block exynos4_reboot_notifier = {
	.notifier_call = exynos4_notifier_call,
};

#if defined(CONFIG_CMA)
#include "reserve-mem.h"

static struct cma_region regions[] = {
#if defined(CONFIG_ION_EXYNOS_CONTIGHEAP_SIZE) \
	&& CONFIG_ION_EXYNOS_CONTIGHEAP_SIZE
	{
		.name = "ion",
		.size = CONFIG_ION_EXYNOS_CONTIGHEAP_SIZE * SZ_1K,
	},
#endif
#if defined(CONFIG_ION_EXYNOS_MFC_WFD_SIZE) \
	&& CONFIG_ION_EXYNOS_MFC_WFD_SIZE
	{
		.name = "wfd_mfc",
		.size = CONFIG_ION_EXYNOS_MFC_WFD_SIZE * SZ_1K,
	},
#endif
	{
		.size = 0
	},
};

static const char map[] __initconst =
	"ion-exynos/wfd_mfc=wfd_mfc;"
	"ion-exynos=ion;";

static void __init exynos_reserve_mem(void)
{
#ifdef CONFIG_EXYNOS_DEV_MODEM_SS222
	phys_addr_t reserve_base = shm_get_phys_base();
	size_t reserve_size = shm_get_phys_size();

	if (memblock_is_region_reserved(reserve_base, reserve_size) ||
			memblock_reserve(reserve_base, reserve_size))
		pr_err("Failed to reserve %#x@%#x\n", reserve_size,
			reserve_base);

	exynos_cma_region_reserve(regions, NULL, NULL, map);
#endif
}
#else /* !CONFIG_CMA */
static void exynos_reserve_mem(void)
{
}
#endif

static void __init smdk4270_map_io(void)
{
	clk_xusbxti.rate = 24000000;

	exynos_init_io(NULL, 0);
	s3c24xx_init_clocks(clk_xusbxti.rate);
	s3c24xx_init_uarts(smdk4270_uartcfgs, ARRAY_SIZE(smdk4270_uartcfgs));
#ifdef CONFIG_SEC_DEBUG
	sec_debug_init();
#endif
}

static struct persistent_ram_descriptor smdk3470_prd[] __initdata = {
        {
                .name = "ram_console",
                .size = SZ_2M,
        },
#ifdef CONFIG_PERSISTENT_TRACER
        {
                .name = "persistent_trace",
                .size = SZ_1M,
        },
#endif
};

static struct persistent_ram smdk3470_pr __initdata = {
        .descs = smdk3470_prd,
        .num_descs = ARRAY_SIZE(smdk3470_prd),
        .start = PLAT_PHYS_OFFSET + SZ_512M + SZ_256M,
#ifdef CONFIG_PERSISTENT_TRACER
        .size = 3 * SZ_1M,
#else
        .size = SZ_2M,
#endif
};

static void __init exynos3_init_early(void)
{
#ifdef CONFIG_SEC_DEBUG
	sec_debug_magic_init();
#endif
	persistent_ram_early_init(&smdk3470_pr);
}

static void exynos4270_power_off(void)
{
	printk(KERN_EMERG "%s: set PS_HOLD low\n", __func__);

	__raw_writel(__raw_readl(EXYNOS_PS_HOLD_CONTROL)
				& 0xFFFFFEFF, EXYNOS_PS_HOLD_CONTROL);
	printk(KERN_EMERG "%s: Should not reach here\n", __func__);
}

static void __init universal222ap_machine_init(void)
{
	init_hw_rev();

	s3c_watchdog_set_platdata(&smdk4270_watchdog_platform_data);
#ifdef CONFIG_SEC_WATCHDOG_RESET
	s3c_set_platdata(&smdk4270_watchdog_platform_data,
			sizeof(smdk4270_watchdog_platform_data),
			&watchdog_reset_device);
#endif
	s3c_adc_set_platdata(&smdk_adc_data);

	platform_add_devices(smdk4270_devices, ARRAY_SIZE(smdk4270_devices));

	/* pm_power_off = exynos4270_power_off; */

	exynos4_smdk4270_power_init();
	exynos4_smdk4270_mmc0_init();
#if defined(CONFIG_MACH_GARDA) || defined(CONFIG_MACH_WICHITA)
        brcm_wlan_init();
#else
	exynos4_smdk4270_mmc1_init();
#endif
	exynos4_smdk4270_mmc2_init();
	exynos4_smdk4270_usb_init();
	exynos4_smdk4270_media_init();
	exynos4_smdk4270_audio_init();
#if defined(CONFIG_BATTERY_SAMSUNG)
	exynos4_smdk4270_charger_init();
#endif
#if defined(CONFIG_MACH_KMINI)
	exynos4_smdk4270_mfd_init();
#endif 

#if defined(CONFIG_SEC_THERMISTOR)
	board_sec_thermistor_init();
#endif
	board_universal_ss222ap_init_gpio();
#if defined(CONFIG_SAMSUNG_MUIC)
	exynos4_smdk4270_muic_init();
#endif /* CONFIG_SAMSUNG_MUIC */

	exynos4_universal222ap_clock_init();
	exynos4_smdk4270_display_init();
	exynos4_smdk4270_input_init();
#ifndef CONFIG_MACH_LT02
	exynos4_universal222ap_camera_init();
#endif

	board_universal_ss222ap_init_sensor();

#if defined(CONFIG_LEDS_KTD2026)
	exynos4_smdk4270_led_init();
#endif

	board_universal_ss222ap_init_fpga();
	board_universal_ss222ap_init_gpio_i2c();

#ifdef CONFIG_MOTOR_DRV_DRV2603
	drv2603_motor_init();
#endif
#ifdef CONFIG_DC_MOTOR
#if defined(CONFIG_MACH_KMINI)
	dc_motor_init();
#endif
#endif
	register_reboot_notifier(&exynos4_reboot_notifier);
}

MACHINE_START(UNIVERSAL_SS222AP, "UNIVERSAL_SS222AP")
	/* Maintainer: Kukjin Kim <kgene.kim@samsung.com> */
	.atag_offset	= 0x100,
	.init_early	= exynos3_init_early,
#ifdef CONFIG_SOC_EXYNOS3470
	.init_irq	= exynos5_init_irq,
#else
	.init_irq	= exynos4_init_irq,
#endif
	.map_io		= smdk4270_map_io,
	.handle_irq	= gic_handle_irq,
	.init_machine	= universal222ap_machine_init,
	.timer		= &exynos4_timer,
	.restart	= exynos4_restart,
	.reserve	= exynos_reserve_mem,
MACHINE_END

#if defined(CONFIG_MACH_GARDA)
MACHINE_START(UNIVERSAL_GARDA, "UNIVERSAL_GARDA")
	.atag_offset	= 0x100,
	.init_early	= exynos3_init_early,
	.init_irq	= exynos5_init_irq,
	.map_io		= smdk4270_map_io,
	.handle_irq	= gic_handle_irq,
	.init_machine	= universal222ap_machine_init,
	.timer		= &exynos4_timer,
	.restart	= exynos4_restart,
	.reserve	= exynos_reserve_mem,
MACHINE_END
#elif defined(CONFIG_MACH_LT02)
MACHINE_START(UNIVERSAL_LT02, "UNIVERSAL_LT02")
	.atag_offset	= 0x100,
	.init_early	= exynos3_init_early,
	.init_irq	= exynos5_init_irq,
	.map_io		= smdk4270_map_io,
	.handle_irq	= gic_handle_irq,
	.init_machine	= universal222ap_machine_init,
	.timer		= &exynos4_timer,
	.restart	= exynos4_restart,
	.reserve	= exynos_reserve_mem,
MACHINE_END
#elif defined(CONFIG_MACH_CARMEN)
MACHINE_START(UNIVERSAL_CARMEN, "UNIVERSAL_CARMEN")
	.atag_offset	= 0x100,
	.init_early	= exynos3_init_early,
	.init_irq	= exynos5_init_irq,
	.map_io		= smdk4270_map_io,
	.handle_irq	= gic_handle_irq,
	.init_machine	= universal222ap_machine_init,
	.timer		= &exynos4_timer,
	.restart	= exynos4_restart,
	.reserve	= exynos_reserve_mem,
MACHINE_END
#elif defined(CONFIG_MACH_KMINI)
MACHINE_START(UNIVERSAL_KMINI, "UNIVERSAL_KMINI")
	.atag_offset	= 0x100,
	.init_early	= exynos3_init_early,
	.init_irq	= exynos5_init_irq,
	.map_io		= smdk4270_map_io,
	.handle_irq	= gic_handle_irq,
	.init_machine	= universal222ap_machine_init,
	.timer		= &exynos4_timer,
	.restart	= exynos4_restart,
	.reserve	= exynos_reserve_mem,
MACHINE_END
#elif defined(CONFIG_MACH_WICHITA)
MACHINE_START(UNIVERSAL_WICHITA, "UNIVERSAL_WICHITA")
	.atag_offset	= 0x100,
	.init_early	= exynos3_init_early,
	.init_irq	= exynos5_init_irq,
	.map_io		= smdk4270_map_io,
	.handle_irq	= gic_handle_irq,
	.init_machine	= universal222ap_machine_init,
	.timer		= &exynos4_timer,
	.restart	= exynos4_restart,
	.reserve	= exynos_reserve_mem,
MACHINE_END
#endif

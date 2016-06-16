/* linux/drivers/video/backlight/tc358764_lvds_mipi_lcd.c
 *
 * Samsung SoC MIPI LCD driver.
 *
 * Copyright (c) 2013 Samsung Electronics
 *
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 */

#include <linux/delay.h>
#include <linux/gpio.h>
#include <video/mipi_display.h>

#include <plat/dsim.h>

#include <plat/mipi_dsi.h>
#include <plat/gpio-cfg.h>

#define GAMMA_PARAM_SIZE 26
#define MAX_BRIGHTNESS 255
#define MIN_BRIGHTNESS 0
#define DEFAULT_BRIGHTNESS 0

int tc358764_lvds_probe(struct mipi_dsim_device *dsim)
{
	return 1;
}

void init_lcd(struct mipi_dsim_device *dsim)
{
	u8 InitCode_700[] = {0x05, 0x01, 0x40, 0x00, 0x07, 0x40, 0x00, 0x90, 0x6c};
	u8 InitCode_704[] = {0x05, 0x01, 0x40, 0x04, 0x07, 0x0d, 0x04, 0x03, 0x00};
	u8 InitCode_70C[] = {0x05, 0x01, 0x40, 0x0C, 0x07, 0x04, 0x46, 0x00, 0x00};
	u8 InitCode_710[] = {0x05, 0x01, 0x40, 0x10, 0x07, 0x4b, 0x00, 0x4d, 0x05};
	u8 InitCode_714[] = {0x05, 0x01, 0x40, 0x14, 0x07, 0x20, 0x00, 0x00, 0x00};

	u8 InitCode_718[] = {0x05, 0x01, 0x40, 0x18, 0x07, 0x02, 0x01, 0x00, 0x00};
	u8 InitCode_71c[] = {0x05, 0x01, 0x40, 0x1c, 0x07, 0x2f, 0x00, 0xa8, 0x00};
	u8 InitCode_720[] = {0x05, 0x01, 0x40, 0x20, 0x07, 0x00, 0x00, 0x00, 0x00};
	u8 InitCode_154[] = {0x05, 0x01, 0x40, 0x54, 0x01, 0x00, 0x00, 0x00, 0x00};
	u8 InitCode_154_1[] = {0x05, 0x01, 0x40, 0x54, 0x01, 0x00, 0x00, 0x00, 0x80};

	u8 InitCode_700_1[] = {0x05, 0x01, 0x40, 0x00, 0x07, 0x40, 0x08, 0x90, 0x6c};
	u8 InitCode_70C_1[] = {0x05, 0x01, 0x40, 0x0c, 0x07, 0x56, 0x5e, 0x00, 0x00};
	u8 InitCode_718_1[] = {0x05, 0x01, 0x40, 0x18, 0x07, 0x02, 0x02, 0x00, 0x00};

	u8 InitCode_37C[] = {0x05, 0x01, 0x40, 0x7c, 0x03, 0x63, 0x10, 0x00, 0x00};
	u8 InitCode_380[] = {0x05, 0x01, 0x40, 0x80, 0x03, 0x30 , 0x60, 0xa8, 0x82};
	u8 InitCode_384[] = {0x05, 0x01, 0x40, 0x84, 0x03, 0x8b, 0x40, 0x61, 0x28};
	u8 InitCode_388[] = {0x05, 0x01, 0x40, 0x88, 0x03, 0x85, 0x02, 0x13, 0x00};
	u8 InitCode_38C[] = {0x05, 0x01, 0x40, 0x8c, 0x03, 0x09, 0x00, 0x63, 0x10};

	u8 InitCode_394[] = {0x05, 0x01, 0x40, 0x94, 0x03, 0xa8, 0x82, 0x0b, 0x40};
	u8 InitCode_600[] = {0x05, 0x01, 0x40, 0x00, 0x06, 0x8c, 0xc7, 0x6c, 0x01};
	u8 InitCode_604[] = {0x05, 0x01, 0x40, 0x04, 0x06, 0xe0, 0xff, 0xff, 0x3f};
	u8 InitCode_608[] = {0x05, 0x01, 0x40, 0x08, 0x06, 0x8c, 0x0d};
	u8 InitCode_120[] = {0x05, 0x01, 0x40, 0x20, 0x01, 0x05, 0x00, 0x00, 0x00};
	u8 InitCode_124[] = {0x05, 0x01, 0x40, 0x24, 0x01, 0x00, 0xc4, 0xd2, 0x04};
	u8 InitCode_128[] = {0x05, 0x01, 0x40, 0x28, 0x01, 0x10, 0x40, 0x10, 0x00};
	u8 InitCode_12C[] = {0x05, 0x01, 0x40, 0x2c, 0x01, 0x8d, 0x00, 0x00, 0x00};

	u8 InitCode_130[] = {0x05, 0x01, 0x40, 0x30, 0x01, 0x18, 0x3c, 0x00, 0x00};
	u8 InitCode_134[] = {0x05, 0x01, 0x40, 0x34, 0x01, 0x15, 0x00, 0x00, 0x00};
	u8 InitCode_138[] = {0x05, 0x01, 0x40, 0x38, 0x01, 0x00, 0x80, 0xff, 0x00};
	u8 InitCode_13c[] = {0x05, 0x01, 0x40, 0x3c, 0x01, 0x00, 0x00, 0x00, 0x00};
	u8 InitCode_114[] = {0x05, 0x01, 0x40, 0x14, 0x01, 0x02, 0x63, 0x0c, 0x00};

	u8 InitCode_160[] = {0x05, 0x01, 0x40, 0x60, 0x01, 0xff, 0x00, 0x00, 0x00};
	u8 InitCode_164[] = {0x05, 0x01, 0x40, 0x64, 0x01, 0x7f, 0x00, 0x00, 0x00};
	u8 InitCode_138_1[] = {0x05, 0x01, 0x40, 0x38, 0x01, 0x00, 0x00, 0xFF, 0x3F};
	u8 InitCode_15c[] = {0x05, 0x01, 0x40, 0x5c, 0x01, 0x05, 0x00, 0x00, 0x00};

	u8 InitCode_140[] = {0x05, 0x01, 0x40, 0x40, 0x01, 0x00, 0x00, 0x01, 0x00};
	u8 InitCode_174[] = {0x05, 0x01, 0x40, 0x74, 0x01, 0xff, 0x00, 0x00, 0x00};
	u8 InitCode_404[] = {0x05, 0x01, 0x40, 0x04, 0x04, 0x22, 0x08, 0x55, 0x55};
	u8 InitCode_418[] = {0x05, 0x01, 0x40, 0x18, 0x04, 0xff, 0x02, 0x55, 0x55};
	u8 InitCode_410[] = {0x05, 0x01, 0x40, 0x10, 0x04, 0x00, 0x03, 0xe5, 0x05};

	u8 InitCode_20C[] = {0x05, 0x01, 0x40, 0x0c, 0x02, 0x24, 0x01};
	u8 InitCode_21C[] = {0x05, 0x01, 0x40, 0x1c, 0x02, 0x00};
	u8 InitCode_224[] = {0x05, 0x01, 0x40, 0x24, 0x02, 0x07};
	u8 InitCode_228[] = {0x05, 0x01, 0x40, 0x28, 0x02, 0x01, 0x00, 0x05};
	u8 InitCode_22C[] = {0x05, 0x01, 0x40, 0x2c, 0x02, 0x03, 0xff};
	u8 InitCode_230[] = {0x05, 0x01, 0x40, 0x30, 0x02, 0x01};

	u8 InitCode_234[] = {0x05, 0x01, 0x40, 0x34, 0x02, 0x10, 0x3e, 0x03, 0xca};
	u8 InitCode_238[] = {0x05, 0x01, 0x40, 0x38, 0x02, 0x60, 0x00, 0x00, 0x00};
	u8 InitCode_23C[] = {0x05, 0x01, 0x40, 0x3c, 0x02, 0x30, 0x60, 0xe8, 0x82};
	u8 InitCode_244[] = {0x05, 0x01, 0x40, 0x44, 0x02, 0x85, 0x02, 0x1e, 0x00};
	u8 InitCode_258[] = {0x05, 0x01, 0x40, 0x58, 0x02, 0x13, 0x00, 0x03};

	u8 InitCode_400[] = {0x05, 0x01, 0x40, 0x00, 0x04, 0x00};
	u8 InitCode_158[] = {0x05, 0x01, 0x40, 0x58, 0x01, 0x00};
	u8 InitCode_158_1[] = {0x05, 0x01, 0x40, 0x58, 0x01, 0x01};

	msleep(20);

	while (s5p_mipi_dsi_wr_data(dsim, MIPI_DSI_GENERIC_LONG_WRITE,
				(unsigned int)InitCode_700,
				ARRAY_SIZE(InitCode_700)) == -1);
	msleep(1);
	while (s5p_mipi_dsi_wr_data(dsim, MIPI_DSI_GENERIC_LONG_WRITE,
				(unsigned int)InitCode_704,
				ARRAY_SIZE(InitCode_704)) == -1);
	msleep(1);
	while (s5p_mipi_dsi_wr_data(dsim, MIPI_DSI_GENERIC_LONG_WRITE,
				(unsigned int)InitCode_70C,
				ARRAY_SIZE(InitCode_70C)) == -1);
	msleep(1);
	while (s5p_mipi_dsi_wr_data(dsim, MIPI_DSI_GENERIC_LONG_WRITE,
				(unsigned int)InitCode_710,
				ARRAY_SIZE(InitCode_710)) == -1);
	msleep(1);
	while (s5p_mipi_dsi_wr_data(dsim, MIPI_DSI_GENERIC_LONG_WRITE,
				(unsigned int)InitCode_714,
				ARRAY_SIZE(InitCode_714)) == -1);
	msleep(1);
	while (s5p_mipi_dsi_wr_data(dsim, MIPI_DSI_GENERIC_LONG_WRITE,
				(unsigned int)InitCode_718,
				ARRAY_SIZE(InitCode_718)) == -1);
	msleep(1);
	while (s5p_mipi_dsi_wr_data(dsim, MIPI_DSI_GENERIC_LONG_WRITE,
				(unsigned int)InitCode_71c,
				ARRAY_SIZE(InitCode_71c)) == -1);
	msleep(1);
	while (s5p_mipi_dsi_wr_data(dsim, MIPI_DSI_GENERIC_LONG_WRITE,
				(unsigned int)InitCode_720,
				ARRAY_SIZE(InitCode_720)) == -1);
	msleep(1);
	while (s5p_mipi_dsi_wr_data(dsim, MIPI_DSI_GENERIC_LONG_WRITE,
				(unsigned int)InitCode_154,
				ARRAY_SIZE(InitCode_154)) == -1);
	msleep(1);
	/* configure done */
	while (s5p_mipi_dsi_wr_data(dsim, MIPI_DSI_GENERIC_LONG_WRITE,
				(unsigned int)InitCode_154_1,
				ARRAY_SIZE(InitCode_154_1)) == -1);
	msleep(1);

	while (s5p_mipi_dsi_wr_data(dsim, MIPI_DSI_GENERIC_LONG_WRITE,
				(unsigned int)InitCode_700_1,
				ARRAY_SIZE(InitCode_700_1)) == -1);
	msleep(1);
	while (s5p_mipi_dsi_wr_data(dsim, MIPI_DSI_GENERIC_LONG_WRITE,
				(unsigned int)InitCode_70C_1,
				ARRAY_SIZE(InitCode_70C_1)) == -1);
	msleep(1);
	while (s5p_mipi_dsi_wr_data(dsim, MIPI_DSI_GENERIC_LONG_WRITE,
				(unsigned int)InitCode_718_1,
				ARRAY_SIZE(InitCode_718_1)) == -1);
	msleep(1);
	while (s5p_mipi_dsi_wr_data(dsim, MIPI_DSI_GENERIC_LONG_WRITE,
				(unsigned int)InitCode_154,
				ARRAY_SIZE(InitCode_154)) == -1);
	msleep(1);
	/* configure done */
	while (s5p_mipi_dsi_wr_data(dsim, MIPI_DSI_GENERIC_LONG_WRITE,
				(unsigned int)InitCode_154_1,
				ARRAY_SIZE(InitCode_154_1)) == -1);
	msleep(1);

	while (s5p_mipi_dsi_wr_data(dsim, MIPI_DSI_GENERIC_LONG_WRITE,
				(unsigned int)InitCode_37C,
				ARRAY_SIZE(InitCode_37C)) == -1);
	msleep(1);
	while (s5p_mipi_dsi_wr_data(dsim, MIPI_DSI_GENERIC_LONG_WRITE,
				(unsigned int)InitCode_380,
				ARRAY_SIZE(InitCode_380)) == -1);
	msleep(1);
	while (s5p_mipi_dsi_wr_data(dsim, MIPI_DSI_GENERIC_LONG_WRITE,
				(unsigned int)InitCode_384,
				ARRAY_SIZE(InitCode_384)) == -1);
	msleep(1);
	while (s5p_mipi_dsi_wr_data(dsim, MIPI_DSI_GENERIC_LONG_WRITE,
				(unsigned int)InitCode_388,
				ARRAY_SIZE(InitCode_388)) == -1);
	msleep(1);
	while (s5p_mipi_dsi_wr_data(dsim, MIPI_DSI_GENERIC_LONG_WRITE,
				(unsigned int)InitCode_38C,
				ARRAY_SIZE(InitCode_38C)) == -1);
	msleep(1);
	while (s5p_mipi_dsi_wr_data(dsim, MIPI_DSI_GENERIC_LONG_WRITE,
				(unsigned int)InitCode_394,
				ARRAY_SIZE(InitCode_394)) == -1);
	msleep(1);
	while (s5p_mipi_dsi_wr_data(dsim, MIPI_DSI_GENERIC_LONG_WRITE,
				(unsigned int)InitCode_600,
				ARRAY_SIZE(InitCode_600)) == -1);
	msleep(1);
	while (s5p_mipi_dsi_wr_data(dsim, MIPI_DSI_GENERIC_LONG_WRITE,
				(unsigned int)InitCode_604,
				ARRAY_SIZE(InitCode_604)) == -1);
	msleep(1);
	while (s5p_mipi_dsi_wr_data(dsim, MIPI_DSI_GENERIC_LONG_WRITE,
				(unsigned int)InitCode_608,
				ARRAY_SIZE(InitCode_608)) == -1);
	msleep(1);
	while (s5p_mipi_dsi_wr_data(dsim, MIPI_DSI_GENERIC_LONG_WRITE,
				(unsigned int)InitCode_154,
				ARRAY_SIZE(InitCode_154)) == -1);
	msleep(1);
	/* configure done */
	while (s5p_mipi_dsi_wr_data(dsim, MIPI_DSI_GENERIC_LONG_WRITE,
				(unsigned int)InitCode_154_1,
				ARRAY_SIZE(InitCode_154_1)) == -1);
	msleep(1);

	/* move for system reset command (0x158) */
	while (s5p_mipi_dsi_wr_data(dsim, MIPI_DSI_GENERIC_LONG_WRITE,
				(unsigned int)InitCode_120,
				ARRAY_SIZE(InitCode_120)) == -1);
	msleep(1);
	while (s5p_mipi_dsi_wr_data(dsim, MIPI_DSI_GENERIC_LONG_WRITE,
				(unsigned int)InitCode_124,
				ARRAY_SIZE(InitCode_124)) == -1);
	msleep(1);
	while (s5p_mipi_dsi_wr_data(dsim, MIPI_DSI_GENERIC_LONG_WRITE,
				(unsigned int)InitCode_128,
				ARRAY_SIZE(InitCode_128)) == -1);
	msleep(1);
	while (s5p_mipi_dsi_wr_data(dsim, MIPI_DSI_GENERIC_LONG_WRITE,
				(unsigned int)InitCode_12C,
				ARRAY_SIZE(InitCode_12C)) == -1);
	msleep(1);
	while (s5p_mipi_dsi_wr_data(dsim, MIPI_DSI_GENERIC_LONG_WRITE,
				(unsigned int)InitCode_130,
				ARRAY_SIZE(InitCode_130)) == -1);
	msleep(1);
	while (s5p_mipi_dsi_wr_data(dsim, MIPI_DSI_GENERIC_LONG_WRITE,
				(unsigned int)InitCode_134,
				ARRAY_SIZE(InitCode_134)) == -1);
	msleep(1);
	while (s5p_mipi_dsi_wr_data(dsim, MIPI_DSI_GENERIC_LONG_WRITE,
				(unsigned int)InitCode_138,
				ARRAY_SIZE(InitCode_138)) == -1);
	msleep(1);
	while (s5p_mipi_dsi_wr_data(dsim, MIPI_DSI_GENERIC_LONG_WRITE,
				(unsigned int)InitCode_13c,
				ARRAY_SIZE(InitCode_13c)) == -1);
	msleep(1);
	while (s5p_mipi_dsi_wr_data(dsim, MIPI_DSI_GENERIC_LONG_WRITE,
				(unsigned int)InitCode_114,
				ARRAY_SIZE(InitCode_114)) == -1);
	msleep(1);

	/* backlight pwm configure */
	while (s5p_mipi_dsi_wr_data(dsim, MIPI_DSI_GENERIC_LONG_WRITE,
				(unsigned int)InitCode_160,
				ARRAY_SIZE(InitCode_160)) == -1);
	msleep(1);
	while (s5p_mipi_dsi_wr_data(dsim, MIPI_DSI_GENERIC_LONG_WRITE,
				(unsigned int)InitCode_164,
				ARRAY_SIZE(InitCode_164)) == -1);
	msleep(1);
	while (s5p_mipi_dsi_wr_data(dsim, MIPI_DSI_GENERIC_LONG_WRITE,
				(unsigned int)InitCode_138_1,
				ARRAY_SIZE(InitCode_138_1)) == -1);
	msleep(1);
	while (s5p_mipi_dsi_wr_data(dsim, MIPI_DSI_GENERIC_LONG_WRITE,
				(unsigned int)InitCode_15c,
				ARRAY_SIZE(InitCode_15c)) == -1);
	msleep(1);

	while (s5p_mipi_dsi_wr_data(dsim, MIPI_DSI_GENERIC_LONG_WRITE,
				(unsigned int)InitCode_140,
				ARRAY_SIZE(InitCode_140)) == -1);
	msleep(1);
	while (s5p_mipi_dsi_wr_data(dsim, MIPI_DSI_GENERIC_LONG_WRITE,
				(unsigned int)InitCode_174,
				ARRAY_SIZE(InitCode_174)) == -1);
	msleep(1);
	while (s5p_mipi_dsi_wr_data(dsim, MIPI_DSI_GENERIC_LONG_WRITE,
				(unsigned int)InitCode_404,
				ARRAY_SIZE(InitCode_404)) == -1);
	msleep(1);
	while (s5p_mipi_dsi_wr_data(dsim, MIPI_DSI_GENERIC_LONG_WRITE,
				(unsigned int)InitCode_418,
				ARRAY_SIZE(InitCode_418)) == -1);
	msleep(1);
	while (s5p_mipi_dsi_wr_data(dsim, MIPI_DSI_GENERIC_LONG_WRITE,
				(unsigned int)InitCode_410,
				ARRAY_SIZE(InitCode_410)) == -1);
	msleep(1);
	while (s5p_mipi_dsi_wr_data(dsim, MIPI_DSI_GENERIC_LONG_WRITE,
				(unsigned int)InitCode_20C,
				ARRAY_SIZE(InitCode_20C)) == -1);
	msleep(1);
	while (s5p_mipi_dsi_wr_data(dsim, MIPI_DSI_GENERIC_LONG_WRITE,
				(unsigned int)InitCode_21C,
				ARRAY_SIZE(InitCode_21C)) == -1);
	msleep(1);
	while (s5p_mipi_dsi_wr_data(dsim, MIPI_DSI_GENERIC_LONG_WRITE,
				(unsigned int)InitCode_224,
				ARRAY_SIZE(InitCode_224)) == -1);
	msleep(1);
	while (s5p_mipi_dsi_wr_data(dsim, MIPI_DSI_GENERIC_LONG_WRITE,
				(unsigned int)InitCode_228,
				ARRAY_SIZE(InitCode_228)) == -1);
	msleep(1);
	while (s5p_mipi_dsi_wr_data(dsim, MIPI_DSI_GENERIC_LONG_WRITE,
				(unsigned int)InitCode_22C,
				ARRAY_SIZE(InitCode_22C)) == -1);
	msleep(1);
	while (s5p_mipi_dsi_wr_data(dsim, MIPI_DSI_GENERIC_LONG_WRITE,
				(unsigned int)InitCode_230,
				ARRAY_SIZE(InitCode_230)) == -1);
	msleep(1);
	while (s5p_mipi_dsi_wr_data(dsim, MIPI_DSI_GENERIC_LONG_WRITE,
				(unsigned int)InitCode_234,
				ARRAY_SIZE(InitCode_234)) == -1);
	msleep(1);
	while (s5p_mipi_dsi_wr_data(dsim, MIPI_DSI_GENERIC_LONG_WRITE,
				(unsigned int)InitCode_238,
				ARRAY_SIZE(InitCode_238)) == -1);
	msleep(1);
	while (s5p_mipi_dsi_wr_data(dsim, MIPI_DSI_GENERIC_LONG_WRITE,
				(unsigned int)InitCode_23C,
				ARRAY_SIZE(InitCode_23C)) == -1);
	msleep(1);
	while (s5p_mipi_dsi_wr_data(dsim, MIPI_DSI_GENERIC_LONG_WRITE,
				(unsigned int)InitCode_244,
				ARRAY_SIZE(InitCode_244)) == -1);
	msleep(1);
	while (s5p_mipi_dsi_wr_data(dsim, MIPI_DSI_GENERIC_LONG_WRITE,
				(unsigned int)InitCode_258,
				ARRAY_SIZE(InitCode_258)) == -1);
	msleep(1);
	while (s5p_mipi_dsi_wr_data(dsim, MIPI_DSI_GENERIC_LONG_WRITE,
				(unsigned int)InitCode_400,
				ARRAY_SIZE(InitCode_400)) == -1);
	msleep(1);
	while (s5p_mipi_dsi_wr_data(dsim, MIPI_DSI_GENERIC_LONG_WRITE,
				(unsigned int)InitCode_158,
				ARRAY_SIZE(InitCode_158)) == -1);
	msleep(1);
	/* device ip configure done */
	while (s5p_mipi_dsi_wr_data(dsim, MIPI_DSI_GENERIC_LONG_WRITE,
				(unsigned int)InitCode_158_1,
				ARRAY_SIZE(InitCode_158_1)) == -1);
	msleep(1);
}

int tc358764_lvds_displayon(struct mipi_dsim_device *dsim)
{
	init_lcd(dsim);
	return 1;
}

int tc358764_lvds_suspend(struct mipi_dsim_device *dsim)
{
	if (dsim->pd->lcd_reset)
		dsim->pd->lcd_reset();
	if (dsim->pd->backlight_on_off)
		dsim->pd->backlight_on_off(false);

	return 1;
}

int tc358764_lvds_resume(struct mipi_dsim_device *dsim)
{
	if (dsim->pd->lcd_power_on_off)
		dsim->pd->lcd_power_on_off(true);
	if (dsim->pd->lcd_reset)
		dsim->pd->lcd_reset();
	return 1;
}

struct mipi_dsim_lcd_driver tc358764_lvds_mipi_lcd_driver = {
	.probe	= tc358764_lvds_probe,
	.displayon	= tc358764_lvds_displayon,
	.suspend	= tc358764_lvds_suspend,
	.resume		= tc358764_lvds_resume,
};

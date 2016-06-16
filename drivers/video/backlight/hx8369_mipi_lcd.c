/* linux/drivers/video/backlight/hx8369_mipi_lcd.c
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

#define NEW_LCD_ID 0x49
#define OLD_LCD_ID 0x48

/* Enable extention command */
static unsigned char setextc[] = {0xB9, 0xFF, 0x83, 0x69};

/* Interface pixel format */
static unsigned char colmod[] = {0x3A, 0x77};

/* GOA Timing Control */
static unsigned char setgip[] = {0xD5,
		0x00, 0x00, 0x13, 0x03, 0x35, 0x00, 0x01, 0x10, 0x01, 0x00,
		0x00, 0x00, 0x01, 0x7A, 0x16, 0x04, 0x04, 0x13, 0x07, 0x42,
		0x13, 0x00, 0x00, 0x00, 0x20, 0x10, 0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00, 0x00, 0x36, 0x00, 0x00, 0x48, 0x88,
		0x85, 0x42, 0x00, 0x99, 0x99, 0x00, 0x00, 0x18, 0x88, 0x86,
		0x71, 0x35, 0x99, 0x99, 0x00, 0x00, 0x58, 0x88, 0x87, 0x63,
		0x11, 0x99, 0x99, 0x00, 0x00, 0x08, 0x88, 0x84, 0x50, 0x24,
		0x99, 0x99, 0x00, 0x00, 0x00, 0x51, 0x00, 0x00, 0x00, 0x00,
		0x00, 0x0F, 0x00, 0x0F, 0x00, 0x00, 0x0F, 0x00, 0x0F, 0x00,
		0x01, 0x5A};

/* MIPI command */
static unsigned char mipicmd[] = {0xBA,
		0x31, 0x00, 0x16, 0xCA, 0xB1, 0x0A, 0x00, 0x10, 0x28, 0x02,
		0x21, 0x21, 0x9A, 0x1A, 0x8F};

/* Power Control */
static unsigned char setpower[] = {0xB1,
		0x0A, 0x83, 0x77, 0x00, 0x92, 0x12, 0x16, 0x16, 0x0C, 0x02};

/* Set Display */
static unsigned char setdisctrl[] = {0xB2, 0x00, 0x70};

/* Gamma Setting */
static unsigned char setgamma[] = {0xE0,
		0x00, 0x05, 0x0B, 0x2F, 0x2F, 0x30, 0x1B, 0x3D, 0x07, 0x0D,
		0x0E, 0x12, 0x13, 0x12, 0x13, 0x11, 0x1A, 0x00, 0x05, 0x0B,
		0x2F, 0x2F, 0x30, 0x1B, 0x3D, 0x07, 0x0D, 0x0E, 0x12, 0x13,
		0x12, 0x13, 0x11, 0x1A, 0x01};

/* Digital Gamma */
static unsigned char setdgc[] = {0xC1,
		0x03, 0x00, 0x09, 0x11, 0x17, 0x1C, 0x25, 0x2D, 0x34, 0x3D,
		0x45, 0x4C, 0x56, 0x5F, 0x68, 0x6F, 0x77, 0x80, 0x88, 0x90,
		0x98, 0xA0, 0xA8, 0xB0, 0xB8, 0xC0, 0xC8, 0xD0, 0xD8, 0xE0,
		0xE8, 0xF0, 0xF8, 0xFF, 0x00, 0x00, 0xAA, 0x0A, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00, 0x09, 0x11, 0x17, 0x1C, 0x25, 0x2D,
		0x34, 0x3D, 0x45, 0x4C, 0x56, 0x5F, 0x68, 0x6F, 0x77, 0x80,
		0x88, 0x90, 0x98, 0xA0, 0xA8, 0xB0, 0xB8, 0xC0, 0xC8, 0xD0,
		0xD8, 0xE0, 0xE8, 0xF0, 0xF8, 0xFF, 0x00, 0x00, 0xAA, 0x0A,
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x09, 0x11, 0x17, 0x1C,
		0x25, 0x2D, 0x34, 0x3D, 0x45, 0x4C, 0x56, 0x5F, 0x68, 0x6F,
		0x77, 0x80, 0x88, 0x90, 0x98, 0xA0, 0xA8, 0xB0, 0xB8, 0xC0,
		0xC8, 0xD0, 0xD8, 0xE0, 0xE8, 0xF0, 0xF8, 0xFF, 0x00, 0x00,
		0xAA, 0x0A, 0x00, 0x00, 0x00, 0x00, 0x00};

/* RGB Setting */
static unsigned char setrgbif[] = {0xB3,
		0x83, 0x00, 0x31, 0x03, 0x01, 0x15, 0x14};

/* Display Inversion Setting */
static unsigned char setcyc[] = {0xB4, 0x02};

/* BGP voltage */
static unsigned char setbgp[] = {0xB5, 0x0B, 0x0B, 0x24};

/* Display direction */
static unsigned char setpanel[] = {0xCC, 0x02};

/* Internal used */
static unsigned char intused[] = {0xC6, 0x40};

/* Set Source Option */
static unsigned char setstba[] = {0xC0, 0x73, 0x50, 0x00, 0x34, 0xC4, 0x02};

/* Source EQ */
static unsigned char srceq[] = {0xE3, 0x00, 0x00, 0x13, 0x1B};

/* SET CLOCK */
static unsigned char setclk[] = {0xCB, 0x6D};

/* CABC Control */
static unsigned char setcabc[] = {0xEA, 0x62};

/* Memory access control */
static unsigned char madctl[] = {0x36, 0xC8};

/* Sleep Out */
static unsigned char slpout[] = {0x11};

/* Display On */
static unsigned char dispon[] = {0x29};

/* Display off */
static unsigned char dispoff[] = {0x28};

/* Sleep in */
static unsigned char slpin[] = {0x10};

/* Enable extention command */
static unsigned char setextc_old[] = {0xB9, 0xFF, 0x83, 0x69};

/* Interface pixel format */
static unsigned char colmod_old[] = {0x3A, 0x77};

/* GOA Timing Control */
static unsigned char setgip_old[] = {0xD5,
		0x00, 0x00, 0x13, 0x03, 0x35, 0x00, 0x01, 0x10, 0x01, 0x00,
		0x00, 0x00, 0x01, 0x7A, 0x16, 0x04, 0x20, 0x13, 0x11, 0x34,
		0x13, 0x00, 0x00, 0x01, 0x00, 0x10, 0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00, 0x00, 0x36, 0x00, 0x00, 0x40, 0x00,
		0x88, 0x88, 0x54, 0x20, 0x00, 0x00, 0x00, 0x10, 0x00, 0x88,
		0x88, 0x67, 0x13, 0x50, 0x00, 0x00, 0x50, 0x00, 0x88, 0x88,
		0x76, 0x31, 0x10, 0x00, 0x00, 0x00, 0x00, 0x88, 0x88, 0x45,
		0x02, 0x40, 0x00, 0x00, 0x00, 0x51, 0x00, 0x00, 0x00, 0x0A,
		0x00, 0xEF, 0x00, 0xEF, 0x0A, 0x00, 0xEF, 0x00, 0xEF, 0x00,
		0x01, 0x5A};

/* MIPI command */
static unsigned char mipicmd_old[] = {0xBA,
		0x31, 0x00, 0x16, 0xCA, 0xB0, 0x0A, 0x00, 0x10, 0x28, 0x02,
		0x21, 0x21, 0x9A, 0x1A, 0x8F};

/* Power Control */
static unsigned char setpower_old[] = {0xB1,
		0x0A, 0x83, 0x77, 0x00, 0x92, 0x12, 0x16, 0x16, 0x0C, 0x02};

/* Gamma Setting */
static unsigned char setgamma_old[] = {0xE0,
		0x00, 0x05, 0x0B, 0x2F, 0x2F, 0x30, 0x1B, 0x3E, 0x07, 0x0D,
		0x0E, 0x12, 0x13, 0x12, 0x14, 0x13, 0x1A, 0x00, 0x05, 0x0B,
		0x2F, 0x2F, 0x30, 0x1B, 0x3E, 0x07, 0x0D, 0x0E, 0x12, 0x13,
		0x12, 0x14, 0x13, 0x1A, 0x01};

/* Digital Gamma */
static unsigned char setdgc_old[] = {0xC1,
		0x03, 0x00, 0x09, 0x11, 0x17, 0x1C, 0x25, 0x2D, 0x34, 0x3D,
		0x45, 0x4C, 0x56, 0x5F, 0x68, 0x6F, 0x77, 0x80, 0x88, 0x90,
		0x98, 0xA0, 0xA8, 0xB0, 0xB8, 0xC0, 0xC8, 0xD0, 0xD8, 0xE0,
		0xE8, 0xF0, 0xF8, 0xFF, 0x00, 0x00, 0xAA, 0x0A, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00, 0x09, 0x11, 0x17, 0x1C, 0x25, 0x2D,
		0x34, 0x3D, 0x45, 0x4C, 0x56, 0x5F, 0x68, 0x6F, 0x77, 0x80,
		0x88, 0x90, 0x98, 0xA0, 0xA8, 0xB0, 0xB8, 0xC0, 0xC8, 0xD0,
		0xD8, 0xE0, 0xE8, 0xF0, 0xF8, 0xFF, 0x00, 0x00, 0xAA, 0x0A,
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x09, 0x11, 0x17, 0x1C,
		0x25, 0x2D, 0x34, 0x3D, 0x45, 0x4C, 0x56, 0x5F, 0x68, 0x6F,
		0x77, 0x80, 0x88, 0x90, 0x98, 0xA0, 0xA8, 0xB0, 0xB8, 0xC0,
		0xC8, 0xD0, 0xD8, 0xE0, 0xE8, 0xF0, 0xF8, 0xFF, 0x00, 0x00,
		0xAA, 0x0A, 0x00, 0x00, 0x00, 0x00, 0x00};

/* RGB Setting */
static unsigned char setrgbif_old[] = {0xB3, 0x83, 0x00, 0x31, 0x03};

/* Display Inversion Setting */
static unsigned char setcyc_old[] = {0xB4, 0x02};

/* BGP voltage */
static unsigned char setbgp_old[] = {0xB5, 0x0B, 0x0B, 0x24};

/* Display direction */
static unsigned char setpanel_old[] = {0xCC, 0x0E};

/* Internal used */
static unsigned char intused_old[] = {0xC6, 0x40};

/* Set Source Option */
static unsigned char setstba_old[] = {0xC0, 0x73, 0x50, 0x00, 0x34, 0xC4, 0x02};

/* Source EQ */
static unsigned char srceq_old[] = {0xE3, 0x00, 0x00, 0x13, 0x1B};

/* SET CLOCK */
static unsigned char setclk_old[] = {0xCB, 0x6D};

/* CABC Control */
static unsigned char setcabc_old[] = {0xEA, 0x62};

/* Sleep Out */
static unsigned char slpout_old[] = {0x11};

/* Display On */
static unsigned char dispon_old[] = {0x29};

static int hx8369_probe(struct mipi_dsim_device *dsim)
{
	return 1;
}

static void init_lcd(struct mipi_dsim_device *dsim)
{
	unsigned char read_data[10];
	int ret, panel_version;

	ret = s5p_mipi_dsi_rd_data(dsim, MIPI_DSI_DCS_READ,
			0xDB, 1, &read_data[1]);

	if (ret) {
		printk("Error in dsi read with %d\n", ret);
		return;
	}
	panel_version = read_data[1];
	printk("LCD panel version is %#x\n", panel_version);

	if (panel_version == NEW_LCD_ID) {
		while (s5p_mipi_dsi_wr_data(dsim, MIPI_DSI_DCS_LONG_WRITE,
			(unsigned int)setextc,
				ARRAY_SIZE(setextc)) == -1);

		while (s5p_mipi_dsi_wr_data(dsim, MIPI_DSI_DCS_LONG_WRITE,
			(unsigned int)colmod,
				ARRAY_SIZE(colmod)) == -1);

		while (s5p_mipi_dsi_wr_data(dsim, MIPI_DSI_DCS_LONG_WRITE,
			(unsigned int)setgip,
				ARRAY_SIZE(setgip)) == -1);

		while (s5p_mipi_dsi_wr_data(dsim, MIPI_DSI_DCS_LONG_WRITE,
			(unsigned int)mipicmd,
				ARRAY_SIZE(mipicmd)) == -1);

		while (s5p_mipi_dsi_wr_data(dsim, MIPI_DSI_DCS_LONG_WRITE,
			(unsigned int)setpower,
				ARRAY_SIZE(setpower)) == -1);

		while (s5p_mipi_dsi_wr_data(dsim, MIPI_DSI_DCS_LONG_WRITE,
			(unsigned int)setdisctrl,
				ARRAY_SIZE(setdisctrl)) == -1);

		while (s5p_mipi_dsi_wr_data(dsim, MIPI_DSI_DCS_LONG_WRITE,
			(unsigned int)setgamma,
				ARRAY_SIZE(setgamma)) == -1);

		while (s5p_mipi_dsi_wr_data(dsim, MIPI_DSI_DCS_LONG_WRITE,
			(unsigned int)setdgc,
				ARRAY_SIZE(setdgc)) == -1);

		while (s5p_mipi_dsi_wr_data(dsim, MIPI_DSI_DCS_LONG_WRITE,
			(unsigned int)setrgbif,
				ARRAY_SIZE(setrgbif)) == -1);

		while (s5p_mipi_dsi_wr_data(dsim, MIPI_DSI_DCS_LONG_WRITE,
			(unsigned int)setcyc,
				ARRAY_SIZE(setcyc)) == -1);

		while (s5p_mipi_dsi_wr_data(dsim, MIPI_DSI_DCS_LONG_WRITE,
			(unsigned int)setbgp,
				ARRAY_SIZE(setbgp)) == -1);

		while (s5p_mipi_dsi_wr_data(dsim, MIPI_DSI_DCS_LONG_WRITE,
			(unsigned int)setpanel,
				ARRAY_SIZE(setpanel)) == -1);

		while (s5p_mipi_dsi_wr_data(dsim, MIPI_DSI_DCS_LONG_WRITE,
			(unsigned int)intused,
				ARRAY_SIZE(intused)) == -1);

		while (s5p_mipi_dsi_wr_data(dsim, MIPI_DSI_DCS_LONG_WRITE,
			(unsigned int)setstba,
				ARRAY_SIZE(setstba)) == -1);

		while (s5p_mipi_dsi_wr_data(dsim, MIPI_DSI_DCS_LONG_WRITE,
			(unsigned int)srceq,
				ARRAY_SIZE(srceq)) == -1);

		while (s5p_mipi_dsi_wr_data(dsim, MIPI_DSI_DCS_LONG_WRITE,
			(unsigned int)setclk,
				ARRAY_SIZE(setclk)) == -1);

		while (s5p_mipi_dsi_wr_data(dsim, MIPI_DSI_DCS_LONG_WRITE,
			(unsigned int)setcabc,
				ARRAY_SIZE(setcabc)) == -1);

		while (s5p_mipi_dsi_wr_data(dsim, MIPI_DSI_DCS_LONG_WRITE,
			(unsigned int)madctl,
				ARRAY_SIZE(madctl)) == -1);

		msleep(100);

		while (s5p_mipi_dsi_wr_data(dsim, MIPI_DSI_DCS_LONG_WRITE,
			(unsigned int)dispon,
				ARRAY_SIZE(dispon)) == -1);

		usleep_range(20000, 21000);

		while (s5p_mipi_dsi_wr_data(dsim, MIPI_DSI_DCS_LONG_WRITE,
			(unsigned int)slpout,
				ARRAY_SIZE(slpout)) == -1);

		msleep(120);
	} else {
		if (s5p_mipi_dsi_wr_data(dsim, MIPI_DSI_DCS_LONG_WRITE,
			(unsigned int)setextc_old,
				ARRAY_SIZE(setextc_old)) == -1)
			printk("failed to send setextc_old command\n");
		if (s5p_mipi_dsi_wr_data(dsim, MIPI_DSI_DCS_LONG_WRITE,
			(unsigned int)colmod_old,
				ARRAY_SIZE(colmod_old)) == -1)
			printk("failed to send colmod_old command\n");

		if (s5p_mipi_dsi_wr_data(dsim, MIPI_DSI_DCS_LONG_WRITE,
			(unsigned int)setgip_old,
				ARRAY_SIZE(setgip_old)) == -1)
			printk("failed to send setgip_old command\n");

		if (s5p_mipi_dsi_wr_data(dsim, MIPI_DSI_DCS_LONG_WRITE,
			(unsigned int)mipicmd_old,
				ARRAY_SIZE(mipicmd_old)) == -1)
			printk("failed to send mipicmd_old command\n");

		if (s5p_mipi_dsi_wr_data(dsim, MIPI_DSI_DCS_LONG_WRITE,
			(unsigned int)setpower_old,
				ARRAY_SIZE(setpower_old)) == -1)
			printk("failed to send setpower_old command\n");

		if (s5p_mipi_dsi_wr_data(dsim, MIPI_DSI_DCS_LONG_WRITE,
			(unsigned int)setgamma_old,
				ARRAY_SIZE(setgamma_old)) == -1)
			printk("failed to send setgamma_old command\n");

		if (s5p_mipi_dsi_wr_data(dsim, MIPI_DSI_DCS_LONG_WRITE,
			(unsigned int)setdgc_old,
				ARRAY_SIZE(setdgc_old)) == -1)
			printk("failed to send setdgc_old command\n");

		if (s5p_mipi_dsi_wr_data(dsim, MIPI_DSI_DCS_LONG_WRITE,
			(unsigned int)setrgbif_old,
				ARRAY_SIZE(setrgbif_old)) == -1)
			printk("failed to send setrgbif_old command\n");

		if (s5p_mipi_dsi_wr_data(dsim, MIPI_DSI_DCS_LONG_WRITE,
			(unsigned int)setcyc_old,
				ARRAY_SIZE(setcyc_old)) == -1)
			printk("failed to send setcyc_old command\n");

		if (s5p_mipi_dsi_wr_data(dsim, MIPI_DSI_DCS_LONG_WRITE,
			(unsigned int)setbgp_old,
				ARRAY_SIZE(setbgp_old)) == -1)
			printk("failed to send setbgp_old command\n");

		if (s5p_mipi_dsi_wr_data(dsim, MIPI_DSI_DCS_LONG_WRITE,
			(unsigned int)setpanel_old,
				ARRAY_SIZE(setpanel_old)) == -1)
			printk("failed to send setpanel_old command\n");

		if (s5p_mipi_dsi_wr_data(dsim, MIPI_DSI_DCS_LONG_WRITE,
			(unsigned int)intused_old,
				ARRAY_SIZE(intused_old)) == -1)
			printk("failed to send intused_old command\n");

		if (s5p_mipi_dsi_wr_data(dsim, MIPI_DSI_DCS_LONG_WRITE,
			(unsigned int)setstba_old,
				ARRAY_SIZE(setstba_old)) == -1)
			printk("failed to send setstba_old command\n");

		if (s5p_mipi_dsi_wr_data(dsim, MIPI_DSI_DCS_LONG_WRITE,
			(unsigned int)srceq_old,
				ARRAY_SIZE(srceq_old)) == -1)
			printk("failed to send srceq_old command\n");

		if (s5p_mipi_dsi_wr_data(dsim, MIPI_DSI_DCS_LONG_WRITE,
			(unsigned int)setclk_old,
				ARRAY_SIZE(setclk_old)) == -1)
			printk("failed to send setclk_old command\n");

		if (s5p_mipi_dsi_wr_data(dsim, MIPI_DSI_DCS_LONG_WRITE,
			(unsigned int)setcabc_old,
				ARRAY_SIZE(setcabc_old)) == -1)
			printk("failed to send setcabc_old command\n");

		if (s5p_mipi_dsi_wr_data(dsim, MIPI_DSI_DCS_LONG_WRITE,
			(unsigned int)slpout_old,
				ARRAY_SIZE(slpout_old)) == -1)
			printk("failed to send slpout_old command\n");

		msleep(120);

		if (s5p_mipi_dsi_wr_data(dsim, MIPI_DSI_DCS_LONG_WRITE,
			(unsigned int)dispon_old,
				ARRAY_SIZE(dispon_old)) == -1);
			printk("failed to send dispon_old command\n");

		msleep(120);
	}
}

static int hx8369_displayon(struct mipi_dsim_device *dsim)
{
	init_lcd(dsim);
	return 1;
}

static int hx8369_suspend(struct mipi_dsim_device *dsim)
{
	s5p_mipi_dsi_wr_data(dsim, MIPI_DSI_DCS_LONG_WRITE,
		(unsigned int)dispoff, ARRAY_SIZE(dispoff));
	msleep(100);
	s5p_mipi_dsi_wr_data(dsim, MIPI_DSI_DCS_LONG_WRITE,
		(unsigned int)slpin, ARRAY_SIZE(slpin));
	msleep(100);

	return 1;
}

static int hx8369_resume(struct mipi_dsim_device *dsim)
{
	return 1;
}

struct mipi_dsim_lcd_driver hx8369_mipi_lcd_driver = {
	.probe		= hx8369_probe,
	.displayon	= hx8369_displayon,
	.suspend	= hx8369_suspend,
	.resume		= hx8369_resume,
};

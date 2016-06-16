/* linux/drivers/video/backlight/hx8369b_mipi_lcd.c
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
#include <linux/rtc.h>		/* for rtc_time */
#include <video/mipi_display.h>

#include <linux/lcd.h>

#include <plat/dsim.h>
#include <plat/mipi_dsi.h>
#include <plat/gpio-cfg.h>

#include "hx8369b_mipi_lcd.h"

#define GAMMA_PARAM_SIZE 26
#define MAX_BRIGHTNESS 255
#define MIN_BRIGHTNESS 0
#define DEFAULT_BRIGHTNESS 130
#define KTD253_BACKLIGHT_OFF		0
#define KTD253_MIN_CURRENT_RATIO	1
#define KTD253_MAX_CURRENT_RATIO	32
#define T_LOW_NS       (200 + 10)	/* Additional 10 as safety factor */
#define T_HIGH_NS      (200 + 10)	/* Additional 10 as safety factor */
#define T_ON_MS		2	/* Start up time */
#define T_OFF_MS       3

#define POWER_IS_ON(pwr)		((pwr) <= FB_BLANK_NORMAL)

#define LDI_ID_REG			0x04
#define LDI_ID_LEN			3

#define WR_RETRY	(5)
#define RD_RETRY	(5)

#define FEATURE_KTD283
#ifdef GPIO_LCD_ESD_DETECT
#define GPIO_OLED_DET		/* for compatibility to other model(T0,SHV-E250x) */
#define LCD_ESD_LOGFILE
#define LCD_ESD_RETRY_MAX	(10)
#endif

static int cmdline_bootlogo = false;
static int cmdline_tspconnect = false;

struct lcd_info {
#ifdef FEATURE_KTD283
	unsigned int bl;
	unsigned int current_bl;
	struct backlight_device *bd;
#endif
	unsigned int auto_brightness;
	unsigned int ldi_enable;
	unsigned int power;
	struct mutex lock;
	struct mutex bl_lock;

	struct device *dev;
	struct lcd_device *ld;
	struct mipi_dsim_lcd_device *dsim_dev;
	unsigned char id[LDI_ID_LEN];
	unsigned int irq;
	unsigned int gpio;
	unsigned int connected;
	unsigned int use_bootlogo;

#if defined(GPIO_OLED_DET)
	struct delayed_work oled_detection;
	unsigned int oled_detection_count;
	unsigned int oled_detection_length;
#endif

	struct mipi_dsim_device *dsim;
};

static struct lcd_info *g_lcd;

#if defined(GPIO_OLED_DET)
static void oled_detection_work(struct work_struct *work)
{
	struct lcd_info *lcd = container_of(work, struct lcd_info, oled_detection.work);

	struct file *fp;
	char name[128];
	struct timespec ts;
	struct rtc_time tm;

	int oled_det_level = gpio_get_value(GPIO_LCD_ESD_DETECT);

	if (oled_det_level) {	/* check only Low->High case */
		lcd->oled_detection_count++;
		lcd->oled_detection_length++;

		if (lcd->oled_detection_length < 2) {
			dev_err(&lcd->ld->dev, "%s : (mipi) esd gpio=%d, cnt=%d, length=%d\n", __func__,
				oled_det_level, lcd->oled_detection_count, lcd->oled_detection_length);
#ifdef LCD_ESD_LOGFILE
			getnstimeofday(&ts);
			rtc_time_to_tm(ts.tv_sec, &tm);
			sprintf(name, "%s%02d%02d_%02d:%02d:%02d_esd%1d_%02d",
				"/storage/sdcard0/log/", tm.tm_mon + 1, tm.tm_mday, tm.tm_hour, tm.tm_min, tm.tm_sec,
				oled_det_level, lcd->oled_detection_count);
			fp = filp_open(name, O_CREAT, S_IRWXU | S_IRWXG | S_IRWXO);

			if (IS_ERR_OR_NULL(fp))
				dev_info(&lcd->ld->dev, "fail to create vgh detection log file, %s\n", name);
#endif
		}

		if (lcd->oled_detection_length < LCD_ESD_RETRY_MAX) {
			/* continous call until VGH is low */
			schedule_delayed_work(&lcd->oled_detection, msecs_to_jiffies(10));
		} else {
			/* after MAx times, do not reschedule esd-function */

			/* but, in Max times, show Log */
			dev_err(&lcd->ld->dev, "%s : (mipi) esd gpio=%d, cnt=%d, length=%d(MAX)\n", __func__,
				oled_det_level, lcd->oled_detection_count, lcd->oled_detection_length);

			lcd->oled_detection_length = 0;
#if 0
			/* if user make disconnect of lcd-connector, esd irq called continuously. 
			 * but, May phone Hangup??
			 */
			dev_info(&lcd->ld->dev, "VGH IS NOT OK! LCD SMASH!!!\n");
			sprintf(name, "%s%02d-%02d_%02d:%02d:%02d_POWEROFF",
				"/sdcard/", tm.tm_mon + 1, tm.tm_mday, tm.tm_hour, tm.tm_min, tm.tm_sec);
			fp = filp_open(name, O_CREAT, S_IRWXU | S_IRWXG | S_IRWXO);

			if (IS_ERR_OR_NULL(fp))
				dev_info(&lcd->ld->dev, "fail to create vgh detection log file, %s\n", name);

			sys_sync();
			kernel_power_off();
#endif
		}
	} else {
		/* this path call when booting or ESD-refresh success */
		dev_err(&lcd->ld->dev, "%s : (mipi) esd gpio=%d, cnt=%d, length=%d\n", __func__, oled_det_level,
			lcd->oled_detection_count, lcd->oled_detection_length);

		lcd->oled_detection_length = 0;
	}
}

static irqreturn_t oled_detection_int(int irq, void *_lcd)
{
	struct lcd_info *lcd = _lcd;

	dev_info(&lcd->ld->dev, "%s (mipi)\n", __func__);

	schedule_delayed_work(&lcd->oled_detection, msecs_to_jiffies(10));

	return IRQ_HANDLED;
}
#endif

static int hx8369b_mipi_write(struct lcd_info *lcd, const u8 * seq, u32 len)
{
	int ret = 0;
	int cnt, retry;
	u8 cmd;

	char dbgBuffer[80];
	char *pchar;

	if (!lcd->connected) {
		printk(KERN_ERR "%s : !lcd->connected - return\n", __func__);
		return -EINVAL;
	}

	if (len <= 0) {
		printk(KERN_ERR "%s : len < 0 - return\n", __func__);
		return -EINVAL;
	}

	mutex_lock(&lcd->lock);

	switch (len) {
	case 1:
		cmd = MIPI_DSI_DCS_SHORT_WRITE;
		break;
	case 2:
		cmd = MIPI_DSI_DCS_SHORT_WRITE_PARAM;
		break;
	default:
		cmd = MIPI_DSI_DCS_LONG_WRITE;
		break;
	}

	for (retry = 0; retry < WR_RETRY; retry++) {
		ret = s5p_mipi_dsi_wr_data(lcd->dsim, cmd, (u32) seq, len);
		if (ret >= 0)
			break;
	}

	/* make debug str */
	pchar = dbgBuffer;
	pchar += snprintf(pchar, sizeof(dbgBuffer) - (pchar - dbgBuffer), "%02x - ", cmd);
	for (cnt = 0; cnt < len && cnt < 10; cnt++)
		pchar += snprintf(pchar, sizeof(dbgBuffer) - (pchar - dbgBuffer), "%02x ", seq[cnt]);

	if (retry > 0)
		printk(KERN_ERR "%s : %s - failed - cnt %d, return %d\n", __func__, dbgBuffer, retry, ret);
	else
		printk(KERN_INFO "%s : %s - OK \n", __func__, dbgBuffer);

hx8369b_mipi_write_err:
	mutex_unlock(&lcd->lock);
	return ret;
}

static int hx8369b_read(struct lcd_info *lcd, u8 addr, u8 * buf, u32 len)
{
	int ret = 0;
	u8 cmd;
	int cnt;

	char dbgBuffer[256];
	char *pchar;

	if (!lcd->connected)
		return -EINVAL;

	mutex_lock(&lcd->lock);

	if (len <= 0)
		goto hx8369b_read_err;
	switch (len) {
	case 1:
		cmd = MIPI_DSI_GENERIC_READ_REQUEST_1_PARAM;
		break;
	case 2:
		cmd = MIPI_DSI_GENERIC_READ_REQUEST_2_PARAM;
		break;
	default:
		cmd = MIPI_DSI_DCS_READ;
		break;
	}

	for (cnt = 0; cnt < RD_RETRY; cnt++) {
		ret = s5p_mipi_dsi_rd_data(lcd->dsim, cmd, addr, len, buf);
		if (ret >= 0)
			break;
	}

	if (cnt > 0)
		dev_err(&lcd->ld->dev, "%s : (%02x - %02x %02x %02x %02x) failed - cnt %d, return %d\n", __func__, addr, buf[0], buf[1], buf[2], buf[3], cnt, ret);

	if (ret >= 0) {
		pchar = dbgBuffer;
		pchar += snprintf(pchar, sizeof(dbgBuffer) + (pchar - dbgBuffer), "%02x - ", addr);
		for (cnt = 0; cnt < len; cnt++)
			pchar += snprintf(pchar, sizeof(dbgBuffer) + (pchar - dbgBuffer), "%02x ", buf[cnt]);
		dev_dbg(&lcd->ld->dev, "%s : %s - OK\n", __func__, dbgBuffer);
	}

hx8369b_read_err:
	mutex_unlock(&lcd->lock);
	return ret;
}

#ifdef FEATURE_KTD283
static int update_brightness(struct lcd_info *lcd, u8 force)
{
	int step_count = 0;
	unsigned long irqFlags;
	int brightness = lcd->bd->props.brightness;
	mutex_lock(&lcd->bl_lock);

#if 0
	lcd->bl = ((32 * brightness * 210) / MAX_BRIGHTNESS) / MAX_BRIGHTNESS;
#else
	switch (brightness) {
	case 246 ... 255:
		lcd->bl = 32 - 2;
		break;		/* Max */
	case 236 ... 245:
		lcd->bl = 32 - 4;
		break;
	case 226 ... 235:
		lcd->bl = 32 - 6;
		break;
	case 216 ... 225:
		lcd->bl = 32 - 8;
		break;
	case 201 ... 215:
		lcd->bl = 32 - 10;
		break;
	case 186 ... 200:
		lcd->bl = 32 - 12;
		break;
	case 171 ... 185:
		lcd->bl = 32 - 14;
		break;
	case 156 ... 170:
		lcd->bl = 32 - 16;
		break;
	case 141 ... 155:
		lcd->bl = 32 - 17;
		break;
	case 134 ... 140:
		lcd->bl = 32 - 18;
		break;
	case 126 ... 133:
		lcd->bl = 32 - 19;
		break;
	case 111 ... 125:
		lcd->bl = 32 - 20;
		break;		/* default */
	case 96 ... 110:
		lcd->bl = 32 - 22;
		break;
	case 81 ... 95:
		lcd->bl = 32 - 24;
		break;
	case 71 ... 80:
		lcd->bl = 32 - 26;
		break;
	case 61 ... 70:
		lcd->bl = 32 - 27;
		break;
	case 51 ... 60:
		lcd->bl = 32 - 28;
		break;
	case 41 ... 50:
		lcd->bl = 32 - 29;
		break;
	case 31 ... 40:
		lcd->bl = 32 - 30;
		break;
	case 21 ... 30:
		lcd->bl = 32 - 31;
		break;		/* Min */
	case 1 ... 20:
		lcd->bl = 32 - 31;
		break;		/* Dimming */
	case 0:
		lcd->bl = 32 - 32;
		break;		/* Off */
	default:
		lcd->bl = 32;
		dev_err(&lcd->ld->dev, "%s: unknown brightness %d\n", __func__, brightness);
		break;
	};
#endif

/*      dev_info(&lcd->ld->dev, "%s: brightness %d, lcd->bl %d\n", __func__, brightness, lcd->bl ); */

	if ((force) || ((lcd->ldi_enable) && (lcd->current_bl != lcd->bl))) {

		if (!lcd->connected || lcd->bl == KTD253_BACKLIGHT_OFF) {
			/* Switch off backlight.        */
			dev_info(&lcd->ld->dev, "%s: switching backlight off (connect=%d,bl=%d)\n", __func__, lcd->connected, lcd->bl);
			gpio_set_value(EXYNOS4_GPD0(1), 0);
			msleep(T_OFF_MS);
		} else {
			local_irq_save(irqFlags);

			if (lcd->current_bl == KTD253_BACKLIGHT_OFF) {
				/* Switch on backlight. */
				dev_dbg(&lcd->ld->dev, "%s: switching backlight on\n", __func__);
				gpio_set_value(EXYNOS4_GPD0(1), 1);
				mdelay(T_ON_MS);
				/* Backlight is always at full intensity when switched on. */
				lcd->current_bl = KTD253_MAX_CURRENT_RATIO;
			}

			while (lcd->current_bl != lcd->bl) {
				gpio_set_value(EXYNOS4_GPD0(1), 0);
				ndelay(T_LOW_NS);
				gpio_set_value(EXYNOS4_GPD0(1), 1);
				ndelay(T_HIGH_NS);

				if (lcd->current_bl == KTD253_MIN_CURRENT_RATIO)
					lcd->current_bl = KTD253_MAX_CURRENT_RATIO;
				else
					lcd->current_bl--;

				step_count++;
			}

			local_irq_restore(irqFlags);

			dev_info(&lcd->ld->dev, "%s: after_ lcd ->current_bl = %d\n", __func__, lcd->current_bl);

		}

		lcd->current_bl = lcd->bl;

	}

	mutex_unlock(&lcd->bl_lock);

	return 0;
}
#endif

static int hx8369b_ldi_init(struct lcd_info *lcd)
{
	unsigned char read_data[10];
	int ret, panel_version;

	ret = 0;
	printk(KERN_INFO "+ %s(mipi)\n", __func__);
	do {
		ret = hx8369b_mipi_write(lcd, SEQ_ENABLE_EXTCMD, ARRAY_SIZE(SEQ_ENABLE_EXTCMD));
		if (ret < 0) break;
		ret = hx8369b_mipi_write(lcd, SEQ_MIPICMD, ARRAY_SIZE(SEQ_MIPICMD));
		if (ret < 0) break;
		ret = hx8369b_mipi_write(lcd, SEQ_PIXEL_FORMAT, ARRAY_SIZE(SEQ_PIXEL_FORMAT));
		if (ret < 0) break;
		ret = hx8369b_mipi_write(lcd, SEQ_GOA_TIMING, ARRAY_SIZE(SEQ_GOA_TIMING));
		if (ret < 0) break;
		ret = hx8369b_mipi_write(lcd, SEQ_POWER_CTL, ARRAY_SIZE(SEQ_POWER_CTL));
		if (ret < 0) break;
		ret = hx8369b_mipi_write(lcd, SEQ_RGB_SETTING, ARRAY_SIZE(SEQ_RGB_SETTING));
		if (ret < 0) break;
		ret = hx8369b_mipi_write(lcd, SEQ_DISP_INVERSION, ARRAY_SIZE(SEQ_DISP_INVERSION));
		if (ret < 0) break;
		ret = hx8369b_mipi_write(lcd, SEQ_SET_CLOCK, ARRAY_SIZE(SEQ_SET_CLOCK));
		if (ret < 0) break;
		ret = hx8369b_mipi_write(lcd, SEQ_DISP_DIRECTION, ARRAY_SIZE(SEQ_DISP_DIRECTION));
		if (ret < 0) break;
		ret = hx8369b_mipi_write(lcd, SEQ_DISP_DIRECTION, ARRAY_SIZE(SEQ_DISP_DIRECTION));
		if (ret < 0) break;
		ret = hx8369b_mipi_write(lcd, SEQ_DIGITAL_GAMMA, ARRAY_SIZE(SEQ_DIGITAL_GAMMA));
		if (ret < 0) break;
		ret = hx8369b_mipi_write(lcd, SEQ_SOURCE_TIMING, ARRAY_SIZE(SEQ_SOURCE_TIMING));
		if (ret < 0) break;
		ret = hx8369b_mipi_write(lcd, SEQ_CABC_CTL, ARRAY_SIZE(SEQ_CABC_CTL));
		if (ret < 0) break;
		ret = hx8369b_mipi_write(lcd, SEQ_SOURCE_EQ, ARRAY_SIZE(SEQ_SOURCE_EQ));
		if (ret < 0) break;
		ret = hx8369b_mipi_write(lcd, SEQ_SOURCE_OPTION, ARRAY_SIZE(SEQ_SOURCE_OPTION));
		if (ret < 0) break;
		ret = hx8369b_mipi_write(lcd, SEQ_GAMMA_SETTING, ARRAY_SIZE(SEQ_GAMMA_SETTING));
		if (ret < 0) break;
		ret = hx8369b_mipi_write(lcd, SEQ_SLEEP_OUT, ARRAY_SIZE(SEQ_SLEEP_OUT));
		if (ret < 0) break;
		mdelay(200);

		ret = hx8369b_mipi_write(lcd, SEQ_DISPLAY_ON, ARRAY_SIZE(SEQ_DISPLAY_ON));
		if (ret < 0) break;
		msleep(20);
	} while (0);
	printk(KERN_INFO "- %s(mipi)\n", __func__);

	return 0;
}

static int hx8369b_ldi_enable(struct lcd_info *lcd)
{
	int ret = 0;

	dev_info(&lcd->ld->dev, "x %s\n", __func__);

	return ret;
}

static int hx8369b_ldi_disable(struct lcd_info *lcd)
{
	int ret = 0;

	dev_info(&lcd->ld->dev, "+ %s\n", __func__);

	ret = hx8369b_mipi_write(lcd, SEQ_DISPLAY_OFF, ARRAY_SIZE(SEQ_DISPLAY_OFF));
	msleep(20);

	ret = hx8369b_mipi_write(lcd, SEQ_SLEEP_IN, ARRAY_SIZE(SEQ_SLEEP_IN));
	msleep(120);

	dev_info(&lcd->ld->dev, "- %s\n", __func__);

	return ret;
}

static int hx8369b_power_on(struct lcd_info *lcd)
{
	int ret = 0;

	dev_info(&lcd->ld->dev, "+ %s\n", __func__);

	if (!lcd->ldi_enable)
		ret = hx8369b_ldi_init(lcd);
	if (ret) {
		dev_err(&lcd->ld->dev, "failed to initialize ldi.\n");
		goto err;
	}

	ret = hx8369b_ldi_enable(lcd);
	if (ret) {
		dev_err(&lcd->ld->dev, "failed to enable ldi.\n");
		goto err;
	}

	lcd->ldi_enable = 1;

#ifdef FEATURE_KTD283
	update_brightness(lcd, 1);
#endif

	dev_info(&lcd->ld->dev, "- %s\n", __func__);
err:
	return ret;
}

static int hx8369b_power_off(struct lcd_info *lcd)
{
	int ret = 0;

	dev_info(&lcd->ld->dev, "+ %s\n", __func__);

	lcd->ldi_enable = 0;

	ret = hx8369b_ldi_disable(lcd);

	dev_info(&lcd->ld->dev, "- %s\n", __func__);

	return ret;
}

static int hx8369b_power(struct lcd_info *lcd, int power)
{
	int ret = 0;

	lcd->dsim->cmd_transfer = 1;

	if (POWER_IS_ON(power) && !POWER_IS_ON(lcd->power))
		ret = hx8369b_power_on(lcd);
	else if (!POWER_IS_ON(power) && POWER_IS_ON(lcd->power))
		ret = hx8369b_power_off(lcd);

	lcd->power = power;

	if(ret) dev_info(&lcd->ld->dev, "%s : power on/off cmd - failed\n", __func__);

	lcd->dsim->cmd_transfer = 0;

	return ret;
}

static int hx8369b_set_power(struct lcd_device *ld, int power)
{
	struct lcd_info *lcd = lcd_get_data(ld);

	if (power != FB_BLANK_UNBLANK && power != FB_BLANK_POWERDOWN && power != FB_BLANK_NORMAL) {
		dev_err(&lcd->ld->dev, "%s : power = %d, power value should be 0, 1 or 4.\n", __func__, power);
		return -EINVAL;
	}

	return hx8369b_power(lcd, power);
}

static int hx8369b_get_power(struct lcd_device *ld)
{
	struct lcd_info *lcd = lcd_get_data(ld);

	return lcd->power;
}

static int hx8369b_check_fb(struct lcd_device *ld, struct fb_info *fb)
{
	/* struct s3cfb_window *win = fb->par;
	   struct lcd_info *lcd = lcd_get_data(ld);
	   dev_info(&lcd->ld->dev, "x %s\n", __func__);

	   dev_info(&lcd->ld->dev, "%s, fb%d\n", __func__, win->id); */
	return 0;
}

static struct lcd_ops hx8369b_lcd_ops = {
	.set_power = hx8369b_set_power,
	.get_power = hx8369b_get_power,
	.check_fb = hx8369b_check_fb,
};

#ifdef FEATURE_KTD283
static int ktd283_set_brightness(struct backlight_device *bd)
{
	int ret = 0;
	int brightness = bd->props.brightness;
	struct lcd_info *lcd = bl_get_data(bd);

	if (brightness < MIN_BRIGHTNESS || brightness > bd->props.max_brightness) {
		dev_err(&bd->dev, "lcd brightness should be %d to %d. now %d\n", MIN_BRIGHTNESS, MAX_BRIGHTNESS, brightness);
		return -EINVAL;
	}

	if (lcd->ldi_enable) {
		ret = update_brightness(lcd, 0);
		if (ret < 0) {
			dev_err(&lcd->ld->dev, "err in %s\n", __func__);
			return -EINVAL;
		}
	}

	return ret;
}

static int ktd283_get_brightness(struct backlight_device *bd)
{
	struct lcd_info *lcd = bl_get_data(bd);

	return lcd->current_bl;
}

static const struct backlight_ops ktd283_backlight_ops = {
	.get_brightness = ktd283_get_brightness,
	.update_status = ktd283_set_brightness,
};
#endif

static void hx8369b_read_id(struct lcd_info *lcd, u8 * buf)
{
	int ret = 0;

	ret = hx8369b_read(lcd, LDI_ID_REG, buf, LDI_ID_LEN);
	if (ret < 0) {
		lcd->connected = 0;
		dev_info(&lcd->ld->dev, "panel is not connected well\n");
	}
}

static ssize_t lcd_type_show(struct device *dev, struct device_attribute *attr, char *buf)
{
	char temp[] = "INH_HX8369B\n";

	strcat(buf, temp);
	return strlen(buf);
}

static DEVICE_ATTR(lcd_type, 0444, lcd_type_show, NULL);

static ssize_t auto_brightness_show(struct device *dev, struct device_attribute *attr, char *buf)
{
	struct lcd_info *lcd = dev_get_drvdata(dev);
	char temp[3];

	sprintf(temp, "%d\n", lcd->auto_brightness);
	strcpy(buf, temp);

	return strlen(buf);
}

static ssize_t auto_brightness_store(struct device *dev, struct device_attribute *attr, const char *buf, size_t size)
{
	struct lcd_info *lcd = dev_get_drvdata(dev);
	int value;
	int rc;

	rc = kstrtoul(buf, (unsigned int)0, (unsigned long *)&value);
	if (rc < 0)
		return rc;
	else {
		if (lcd->auto_brightness != value) {
			dev_info(dev, "%s - %d, %d\n", __func__, lcd->auto_brightness, value);
			mutex_lock(&lcd->bl_lock);
			lcd->auto_brightness = value;
			mutex_unlock(&lcd->bl_lock);
		}
	}
	return size;
}

static DEVICE_ATTR(auto_brightness, 0644, auto_brightness_show, auto_brightness_store);

static int hx8369b_probe(struct mipi_dsim_device *dsim)
{
	int ret = 0;
	struct lcd_info *lcd;

	lcd = kzalloc(sizeof(struct lcd_info), GFP_KERNEL);
	if (!lcd) {
		pr_err("failed to allocate for lcd\n");
		ret = -ENOMEM;
		goto err_alloc;
	}

	g_lcd = lcd;

	lcd->ld = lcd_device_register("panel", dsim->dev, lcd, &hx8369b_lcd_ops);
	if (IS_ERR(lcd->ld)) {
		pr_err("failed to register lcd device\n");
		ret = PTR_ERR(lcd->ld);
		goto out_free_lcd;
	}
#ifdef FEATURE_KTD283
	lcd->bd = backlight_device_register("panel", dsim->dev, lcd, &ktd283_backlight_ops, NULL);
	if (IS_ERR(lcd->bd)) {
		pr_err("failed to register backlight device\n");
		ret = PTR_ERR(lcd->bd);
		goto out_free_backlight;
	}

	lcd->bd->props.max_brightness = MAX_BRIGHTNESS;
	lcd->bd->props.brightness = DEFAULT_BRIGHTNESS;
	lcd->bl = ( cmdline_bootlogo ? 16 : 0 );
	lcd->current_bl = lcd->bl;
#endif
	lcd->auto_brightness = 0;
	lcd->dev = dsim->dev;
	lcd->dsim = dsim;
	lcd->use_bootlogo = cmdline_bootlogo;
	lcd->power = (lcd->use_bootlogo ? FB_BLANK_UNBLANK : FB_BLANK_POWERDOWN);
	lcd->ldi_enable = 1;
	lcd->connected = cmdline_tspconnect;

	ret = device_create_file(&lcd->ld->dev, &dev_attr_lcd_type);
	if (ret < 0)
		dev_err(&lcd->ld->dev, "failed to add sysfs entries, %d\n", __LINE__);

	ret = device_create_file(&lcd->bd->dev, &dev_attr_auto_brightness);
	if (ret < 0)
		dev_err(&lcd->ld->dev, "failed to add sysfs entries, %d\n", __LINE__);

	mutex_init(&lcd->lock);
	mutex_init(&lcd->bl_lock);

#if 0 /* it make unstable line in booting */
	hx8369b_read_id(lcd, lcd->id);
	dev_info(&lcd->ld->dev, "ID: %x, %x, %x\n", lcd->id[0], lcd->id[1], lcd->id[2]);
#endif

#if defined(GPIO_OLED_DET)
	if (lcd->connected) {
		lcd->oled_detection_count = 0;
		lcd->oled_detection_length = 0;
		INIT_DELAYED_WORK(&lcd->oled_detection, oled_detection_work);

		s3c_gpio_cfgpin(GPIO_LCD_ESD_DETECT, S3C_GPIO_SFN(0xf));
		s3c_gpio_setpull(GPIO_LCD_ESD_DETECT, S3C_GPIO_PULL_NONE);

		if (request_irq(gpio_to_irq(GPIO_LCD_ESD_DETECT), oled_detection_int, IRQF_TRIGGER_RISING, "oled_detection", lcd))
			pr_err("%s : (mipi) failed to reqeust irq. %d\n", __func__, gpio_to_irq(GPIO_LCD_ESD_DETECT));
	}
#endif

	dev_info(&lcd->ld->dev, "%s lcd panel driver has been probed.\n", dev_name(dsim->dev));

	return 0;

#ifdef FEATURE_KTD283
out_free_backlight:
	lcd_device_unregister(lcd->ld);
	kfree(lcd);
	return ret;
#endif

out_free_lcd:
	kfree(lcd);
	return ret;

err_alloc:
	return ret;
}

static int hx8369b_displayon(struct mipi_dsim_device *dsim)
{
	struct lcd_info *lcd = g_lcd;
	dev_info(&lcd->ld->dev, "+ %s\n", __func__);

	hx8369b_power(lcd, FB_BLANK_UNBLANK);

	dev_info(&lcd->ld->dev, "- %s\n", __func__);
	return 1;
}

static int hx8369b_suspend(struct mipi_dsim_device *dsim)
{
	struct lcd_info *lcd = g_lcd;
	dev_info(&lcd->ld->dev, "+ %s\n", __func__);

#if defined(GPIO_OLED_DET)
	if (lcd->connected) {
		disable_irq(gpio_to_irq(GPIO_LCD_ESD_DETECT));
		gpio_request(GPIO_LCD_ESD_DETECT, "OLED_DET");
		s3c_gpio_cfgpin(GPIO_LCD_ESD_DETECT, S3C_GPIO_OUTPUT);
		s3c_gpio_setpull(GPIO_LCD_ESD_DETECT, S3C_GPIO_PULL_NONE);
		gpio_direction_output(GPIO_LCD_ESD_DETECT, GPIO_LEVEL_LOW);
		gpio_free(GPIO_LCD_ESD_DETECT);
	}
#endif

	/* TODO */
	hx8369b_power(lcd, FB_BLANK_POWERDOWN);

	dev_info(&lcd->ld->dev, "- %s\n", __func__);
	return 1;
}

static int hx8369b_resume(struct mipi_dsim_device *dsim)
{
	int ret = 0;
	struct lcd_info *lcd = g_lcd;

	dev_info(&lcd->ld->dev, "+ %s\n", __func__);

	ret = hx8369b_displayon(dsim);
/*      ret = hx8369b_ldi_init(lcd); */

#if defined(GPIO_OLED_DET)
	if (lcd->connected) {
		s3c_gpio_cfgpin(GPIO_LCD_ESD_DETECT, S3C_GPIO_SFN(0xf));
		s3c_gpio_setpull(GPIO_LCD_ESD_DETECT, S3C_GPIO_PULL_NONE);
		enable_irq(gpio_to_irq(GPIO_LCD_ESD_DETECT));
	}
#endif

	dev_info(&lcd->ld->dev, "- %s\n", __func__);
	return 0;
}

struct mipi_dsim_lcd_driver hx8369b_mipi_lcd_driver = {
	.probe = hx8369b_probe,
	.displayon = hx8369b_displayon,
	.suspend = hx8369b_suspend,
	.resume = hx8369b_resume,
};

static int __init setup_early_bootlogo(char *str)
{
	cmdline_bootlogo = true;
	pr_debug("%s(mipi) : hx8369b_mipi_lcd\n", __func__);
	return 0;
}

early_param("bootlogo", setup_early_bootlogo);

static int __init setup_early_bootsleepcheck(char *str)
{
	cmdline_tspconnect = true;
	pr_debug(KERN_ERR "%s(mipi) : hx8369b_mipi_lcd\n", __func__);
	return 0;
}

early_param("tspconnect", setup_early_bootsleepcheck);

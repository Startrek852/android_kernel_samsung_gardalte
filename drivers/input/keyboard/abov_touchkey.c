/* abov_touchkey.h -- Linux driver for abov chip as touchkey 
 *
 * Copyright (C) 2013 Samsung Electronics Co.Ltd
 * Author: Junkyeong Kim <jk0430.kim@samsung.com>
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the
 * Free Software Foundation; either version 2, or (at your option) any
 * later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 */

#include <linux/delay.h>
#include <linux/firmware.h>
#include <linux/gpio.h>
#include <linux/i2c.h>
#include <linux/init.h>
#include <linux/input.h>
#include <linux/input/mt.h>
#include <linux/interrupt.h>
#include <linux/irq.h>
#include <linux/module.h>
#include <linux/mutex.h>
#include <linux/slab.h>
#include <linux/uaccess.h>
#include <linux/i2c/abov_touchkey.h>
#include <linux/io.h>
#include <mach/gpio.h>
#include <asm/unaligned.h>
#ifdef CONFIG_HAS_EARLYSUSPEND
#include <linux/earlysuspend.h>
#endif

/* registers */
#define ABOV_BTNSTATUS		0x00
#define ABOV_FW_VER			0x01
#define ABOV_PCB_VER		0x02
#define ABOV_COMMAND		0x03
#define ABOV_THRESHOLD		0x04
#define ABOV_SENS			0x05
#define ABOV_SETIDAC		0x06
#define ABOV_DIFFDATA		0x0A
#define ABOV_RAWDATA		0x0E
#define ABOV_VENDORID		0x12
#define ABOV_GLOVE			0x13

/* command */
#define CMD_LED_ON			0x10
#define CMD_LED_OFF			0x20
#define CMD_DATA_UPDATE		0x40
#define CMD_LED_CTRL_ON		0x60
#define CMD_LED_CTRL_OFF	0x70
#define CMD_GLOVE_ON		0x10
#define CMD_GLOVE_OFF		0x20

struct device *sec_touchkey;

#define FW_VERSION 0x20
#define TK_FW_FILENAME "abov_fw.img"

#define I2C_M_WR 0		/* for i2c */

extern struct class *sec_class;

static int touchkey_keycode[] = { 0,
	KEY_MENU, KEY_BACK,
};

struct abov_tk_info {
	struct i2c_client *client;
	struct input_dev *input_dev;
	struct abov_touchkey_platform_data *pdata;
#ifdef CONFIG_HAS_EARLYSUSPEND
	struct early_suspend early_suspend;
#endif
	struct mutex lock;
	char phys[32];
	int irq;
	u16 menu_s;
	u16 back_s;
	u16 menu_raw;
	u16 back_raw;
	int (*power) (bool on);
	void (*input_event)(void *data);
	int touchkey_count;
	u8 fw_update_state;
	u8 fw_ver;
	bool enabled;
};


#ifdef CONFIG_HAS_EARLYSUSPEND
static void abov_tk_early_suspend(struct early_suspend *h);
static void abov_tk_late_resume(struct early_suspend *h);
#endif

#ifdef CONFIG_INPUT_ENABLED
static int abov_tk_input_open(struct input_dev *dev);
static void abov_tk_input_close(struct input_dev *dev);
#endif

static int abov_tk_i2c_read(struct i2c_client *client,
		u8 reg, u8 *val, unsigned int len)
{
	struct abov_tk_info *info = i2c_get_clientdata(client);
	struct i2c_msg msg;
	int ret;
	int retry = 3;

	mutex_lock(&info->lock);
	msg.addr = client->addr;
	msg.flags = I2C_M_WR;
	msg.len = 1;
	msg.buf = &reg;
	while (retry--) {
		ret = i2c_transfer(client->adapter, &msg, 1);
		if (ret >= 0)
			break;

		dev_err(&client->dev, "%s fail(address set)(%d)\n",
			__func__, retry);
		msleep(10);
	}
	if (ret < 0) {
		mutex_unlock(&info->lock);
		return ret;
	}
	retry = 3;
	msg.flags = 1;/*I2C_M_RD*/
	msg.len = len;
	msg.buf = val;
	while (retry--) {
		ret = i2c_transfer(client->adapter, &msg, 1);
		if (ret >= 0) {
			mutex_unlock(&info->lock);
			return 0;
		}
		dev_err(&client->dev, "%s fail(data read)(%d)\n",
			__func__, retry);
		msleep(10);
	}
	mutex_unlock(&info->lock);
	return ret;
}

static int abov_tk_i2c_write(struct i2c_client *client,
		u8 reg, u8 *val, unsigned int len)
{
	struct abov_tk_info *info = i2c_get_clientdata(client);
	struct i2c_msg msg[1];
	unsigned char data[2];
	int ret;
	int retry = 3;

	mutex_lock(&info->lock);
	data[0] = reg;
	data[1] = *val;
	msg->addr = client->addr;
	msg->flags = I2C_M_WR;
	msg->len = 2;
	msg->buf = data;

	while (retry--) {
		ret = i2c_transfer(client->adapter, msg, 1);
		if (ret >= 0) {
			mutex_unlock(&info->lock);
			return 0;
		}
		dev_err(&client->dev, "%s fail(%d)\n",
			__func__, retry);
		msleep(10);
	}
	mutex_unlock(&info->lock);
	return ret;
}

static void release_all_fingers(struct abov_tk_info *info)
{
	struct i2c_client *client = info->client;
	int i;

	dev_dbg(&client->dev, "[TK] %s\n", __func__);

	for (i = 1; i < info->touchkey_count; i++) {
		input_report_key(info->input_dev,
			touchkey_keycode[i], 0);
	}
	input_sync(info->input_dev);
}

static int abov_tk_reset_for_bootmode(struct abov_tk_info *info)
{
	struct i2c_client *client = info->client;

	dev_dbg(&client->dev, " %s excute\n", __func__);

#if 0
	info->pdata->power(false);
	msleep(50);
	info->pdata->power(true);
	msleep(50);
#endif
	return 0;
}

static void abov_tk_reset(struct abov_tk_info *info)
{
	struct i2c_client *client = info->client;

	if (info->enabled == false)
		return;

	dev_notice(&client->dev, "%s++\n", __func__);
	disable_irq_nosync(info->irq);

	info->enabled = false;

	release_all_fingers(info);

	abov_tk_reset_for_bootmode(info);

	info->enabled = true;

	enable_irq(info->irq);
	dev_notice(&client->dev, "%s--\n", __func__);
}

static irqreturn_t abov_tk_interrupt(int irq, void *dev_id)
{
	struct abov_tk_info *info = dev_id;
	struct i2c_client *client = info->client;
	int ret, retry;
	u8 buf, button;
	bool press;

	ret = abov_tk_i2c_read(client, ABOV_BTNSTATUS, &buf, 1);
	if (ret < 0) {
		retry = 3;
		while (retry--) {
			dev_err(&client->dev, "%s read fail(%d)\n",
				__func__, retry);
			abov_tk_reset(info);
			ret = abov_tk_i2c_read(client, ABOV_BTNSTATUS, &buf, 1);
			if (ret == 0)
				break;
		}
		if (retry == 0)
			return IRQ_HANDLED;
	}

	button = buf & 0x03;
	press = (buf & 0x8) > 3;

	if (press) {
		input_report_key(info->input_dev,
			touchkey_keycode[button], 0);
#ifdef CONFIG_SAMSUNG_PRODUCT_SHIP
		dev_notice(&client->dev,
			"key R\n");
#else
		dev_notice(&client->dev,
			"key R : %d(%d)\n",
			touchkey_keycode[button], buf);
#endif
	} else {
		input_report_key(info->input_dev,
			touchkey_keycode[button], 1);
#ifdef CONFIG_SAMSUNG_PRODUCT_SHIP
		dev_notice(&client->dev,
			"key P\n");
#else
		dev_notice(&client->dev,
			"key P : %d(%d)\n",
			touchkey_keycode[button], buf);
#endif
	}
	input_sync(info->input_dev);

	return IRQ_HANDLED;
}

static ssize_t touchkey_led_control(struct device *dev,
		 struct device_attribute *attr, const char *buf,
		 size_t count)
{
	struct abov_tk_info *info = dev_get_drvdata(dev);
	struct i2c_client *client = info->client;
	int data;
	int ret;
	u8 cmd;

	ret = sscanf(buf, "%d", &data);
	if (ret != 1) {
		dev_err(&client->dev, "%s: cmd read err\n", __func__);
		return count;
	}

	if (!(data == 0 || data == 1)) {
		dev_err(&client->dev, "%s: wrong command(%d)\n",
			__func__, data);
		return count;
	}

	if (!info->enabled)
		return count;

	if (data == 1) {
		dev_notice(&client->dev, "led on\n");
		cmd = CMD_LED_ON;
	} else {
		dev_notice(&client->dev, "led off\n");
		cmd = CMD_LED_OFF;
	}
	ret = abov_tk_i2c_write(client, ABOV_BTNSTATUS, &cmd, 1);
	if (ret < 0)
		dev_err(&client->dev, "%s fail(%d)\n", __func__, ret);
	return count;
}

static ssize_t touchkey_threshold_show(struct device *dev,
				struct device_attribute *attr, char *buf)
{
	struct abov_tk_info *info = dev_get_drvdata(dev);
	struct i2c_client *client = info->client;
	u8 r_buf;
	int ret;

	ret = abov_tk_i2c_read(client, ABOV_THRESHOLD, &r_buf, 1);
	if (ret < 0) {
		dev_err(&client->dev, "%s fail(%d)\n", __func__, ret);
		r_buf = 0;
	}
	return sprintf(buf, "%d\n", r_buf);
}

static void get_diff_data(struct abov_tk_info *info)
{
	struct i2c_client *client = info->client;
	u8 r_buf[4];
	int ret;

	ret = abov_tk_i2c_read(client, ABOV_DIFFDATA, r_buf, 4);
	if (ret < 0) {
		dev_err(&client->dev, "%s fail(%d)\n", __func__, ret);
		info->menu_s = 0;
		info->back_s = 0;
		return;
	}

	info->menu_s = (r_buf[0] << 8) | r_buf[1];
	info->back_s = (r_buf[2] << 8) | r_buf[3];
}

static ssize_t touchkey_menu_show(struct device *dev,
				struct device_attribute *attr, char *buf)
{
	struct abov_tk_info *info = dev_get_drvdata(dev);

	get_diff_data(info);

	return sprintf(buf, "%d\n", info->menu_s);
}

static ssize_t touchkey_back_show(struct device *dev,
				struct device_attribute *attr, char *buf)
{
	struct abov_tk_info *info = dev_get_drvdata(dev);

	get_diff_data(info);

	return sprintf(buf, "%d\n", info->back_s);
}

static void get_raw_data(struct abov_tk_info *info)
{
	struct i2c_client *client = info->client;
	u8 r_buf[4];
	int ret;

	ret = abov_tk_i2c_read(client, ABOV_RAWDATA, r_buf, 4);
	if (ret < 0) {
		dev_err(&client->dev, "%s fail(%d)\n", __func__, ret);
		info->menu_raw = 0;
		info->back_raw = 0;
		return;
	}

	info->menu_raw = (r_buf[0] << 8) | r_buf[1];
	info->back_raw = (r_buf[2] << 8) | r_buf[3];
}

static ssize_t touchkey_menu_raw_show(struct device *dev,
				struct device_attribute *attr, char *buf)
{
	struct abov_tk_info *info = dev_get_drvdata(dev);

	get_raw_data(info);

	return sprintf(buf, "%d\n", info->menu_raw);
}

static ssize_t touchkey_back_raw_show(struct device *dev,
				struct device_attribute *attr, char *buf)
{
	struct abov_tk_info *info = dev_get_drvdata(dev);

	get_raw_data(info);

	return sprintf(buf, "%d\n", info->back_raw);
}

static DEVICE_ATTR(touchkey_threshold, S_IRUGO, touchkey_threshold_show, NULL);
static DEVICE_ATTR(brightness, S_IRUGO | S_IWUSR | S_IWGRP, NULL,
			touchkey_led_control);
static DEVICE_ATTR(touchkey_menu, S_IRUGO, touchkey_menu_show, NULL);
static DEVICE_ATTR(touchkey_back, S_IRUGO, touchkey_back_show, NULL);
static DEVICE_ATTR(touchkey_raw_data0, S_IRUGO, touchkey_menu_raw_show, NULL);
static DEVICE_ATTR(touchkey_raw_data1, S_IRUGO, touchkey_back_raw_show, NULL);

static struct attribute *sec_touchkey_attributes[] = {
	&dev_attr_touchkey_threshold.attr,
	&dev_attr_brightness.attr,
	&dev_attr_touchkey_menu.attr,
	&dev_attr_touchkey_back.attr,
	&dev_attr_touchkey_raw_data0.attr,
	&dev_attr_touchkey_raw_data1.attr,
	NULL,
};

static struct attribute_group sec_touchkey_attr_group = {
	.attrs = sec_touchkey_attributes,
};

int get_tk_fw_version(struct abov_tk_info *info)
{
	struct i2c_client *client = info->client;
	u8 buf;
	int ret;
	int retry = 3;

	ret = abov_tk_i2c_read(client, ABOV_FW_VER, &buf, 1);
	if (ret < 0) {
		while (retry--) {
			dev_err(&client->dev, "%s read fail(%d)\n",
				__func__, retry);
			abov_tk_reset(info);
			ret = abov_tk_i2c_read(client, ABOV_FW_VER, &buf, 1);
			if (ret == 0)
				break;
		}
		if (retry == 0)
			return -1;
	}

	info->fw_ver = buf;
	dev_info(&client->dev,
		"fw ver : 0x%x\n", buf);

	return 0;
}

static int abov_tk_fw_check(struct abov_tk_info *info)
{
	struct i2c_client *client = info->client;
	int ret, retry;
	char buf[2] = {0,};

	ret = get_tk_fw_version(info);
	if (ret < 0) {
#if 0
		if (info->pdata->lcd_connect) {
			/* tsp connect check */
			dev_err(&client->dev,
				"%s: i2c fail. but lcd connected\n",
				__func__);
			dev_err(&client->dev,
				"excute firm update\n");
			update = true;
		} else {
#endif
		dev_err(&client->dev,
			"%s: i2c fail...[%d], addr[%d]\n",
			__func__, ret, info->client->addr);
		dev_err(&client->dev,
			"%s: touchkey driver unload\n", __func__);
		return ret;
	}

#if 0
	if (update) {
		dev_err(&client->dev, "excute tk firmware update\n");
		retry = 4;
		while (retry--) {
			ret = mms_flash_fw(info, true, BUILT_IN);
			if (ret) {
				dev_err(&client->dev,
					"failed to mms_flash_fw (%d)\n", ret);
				dev_err(&client->dev,
					"retry flash (%d)\n", retry);
			} else {
				dev_info(&client->dev,
					"fw update success\n");
				break;
			}
		}
	}

	if (update && !!ret)
		return -1;
#endif
	return 0;
}

static int __devinit abov_tk_probe(struct i2c_client *client,
				  const struct i2c_device_id *id)
{
	struct abov_tk_info *info;
	struct input_dev *input_dev;
	struct device *touchkey_dev;
	int ret = 0;
	int i;

	if (!i2c_check_functionality(client->adapter, I2C_FUNC_I2C)) {
		dev_err(&client->dev,
			"i2c_check_functionality fail\n");
		return -EIO;
	}

	info = kzalloc(sizeof(struct abov_tk_info), GFP_KERNEL);
	if (!info) {
		dev_err(&client->dev, "Failed to allocate memory\n");
		ret = -ENOMEM;
		goto err_alloc;
	}

	input_dev = input_allocate_device();
	if (!input_dev) {
		dev_err(&client->dev,
			"Failed to allocate memory for input device\n");
		ret = -ENOMEM;
		goto err_input_alloc;
	}

	info->client = client;
	info->input_dev = input_dev;
	info->pdata = client->dev.platform_data;
	if (info->pdata == NULL) {
		pr_err("failed to get platform data\n");
		goto err_config;
	}
	info->irq = -1;
	mutex_init(&info->lock);

	info->input_event = info->pdata->input_event;
	info->touchkey_count = sizeof(touchkey_keycode) / sizeof(int);
	i2c_set_clientdata(client, info);

	ret = abov_tk_fw_check(info);
	if (ret) {
		dev_err(&client->dev,
			"failed to firmware check (%d)\n", ret);
		goto err_reg_input_dev;
	}

	snprintf(info->phys, sizeof(info->phys),
		 "%s/input0", dev_name(&client->dev));
	input_dev->name = "sec_touchkey";
	input_dev->phys = info->phys;
	input_dev->id.bustype = BUS_HOST;
	input_dev->dev.parent = &client->dev;
#ifdef CONFIG_INPUT_ENABLED
	input_dev->open = abov_tk_input_open;
	input_dev->close = abov_tk_input_close;
#endif
	set_bit(EV_KEY, input_dev->evbit);
	set_bit(KEY_MENU, input_dev->keybit);
	set_bit(KEY_BACK, input_dev->keybit);
	set_bit(EV_LED, input_dev->evbit);
	set_bit(LED_MISC, input_dev->ledbit);
	input_set_drvdata(input_dev, info);

	ret = input_register_device(input_dev);
	if (ret) {
		dev_err(&client->dev, "failed to register input dev (%d)\n",
			ret);
		goto err_reg_input_dev;
	}

	info->enabled = true;

	ret = request_threaded_irq(client->irq, NULL, abov_tk_interrupt,
				IRQF_TRIGGER_RISING, ABOV_TK_NAME, info);
	if (ret < 0) {
		dev_err(&client->dev, "Failed to register interrupt\n");
		goto err_req_irq;
	}
	info->irq = client->irq;

#ifdef CONFIG_HAS_EARLYSUSPEND
	info->early_suspend.level = EARLY_SUSPEND_LEVEL_STOP_DRAWING;
	info->early_suspend.suspend = abov_tk_early_suspend;
	info->early_suspend.resume = abov_tk_late_resume;
	register_early_suspend(&info->early_suspend);
#endif

	sec_touchkey = device_create(sec_class,
		NULL, 0, info, "sec_touchkey");
	if (IS_ERR(sec_touchkey))
		dev_err(&client->dev,
		"Failed to create device for the touchkey sysfs\n");

	ret = sysfs_create_group(&sec_touchkey->kobj,
		&sec_touchkey_attr_group);
	if (ret)
		dev_err(&client->dev, "Failed to create sysfs group\n");

	dev_dbg(&client->dev, "%s done\n", __func__);
	return 0;

err_req_irq:
	input_unregister_device(input_dev);
err_reg_input_dev:
	mutex_destroy(&info->lock);
	info->pdata->keyled(0);
err_config:
	input_free_device(input_dev);
err_input_alloc:
	kfree(info);
err_alloc:
	return ret;

}

static int __devexit abov_tk_remove(struct i2c_client *client)
{
	struct abov_tk_info *info = i2c_get_clientdata(client);

/*	if (info->enabled)
		info->pdata->power(0);
*/
	info->pdata->keyled(0);
#ifdef CONFIG_HAS_EARLYSUSPEND
	unregister_early_suspend(&info->early_suspend);
#endif
	if (info->irq >= 0)
		free_irq(info->irq, info);
	input_unregister_device(info->input_dev);
	kfree(info);

	return 0;
}

static void abov_tk_shutdown(struct i2c_client *client)
{
	struct abov_tk_info *info = i2c_get_clientdata(client);
	u8 cmd = CMD_LED_OFF;

	abov_tk_i2c_write(client, ABOV_BTNSTATUS, &cmd, 1);
	info->pdata->keyled(0);
}

#if defined(CONFIG_PM)
static int abov_tk_suspend(struct device *dev)
{
	struct i2c_client *client = to_i2c_client(dev);
	struct abov_tk_info *info = i2c_get_clientdata(client);

	if (!info->enabled)
		return 0;

	dev_notice(&info->client->dev, "%s: users=%d\n", __func__,
		   info->input_dev->users);

	disable_irq(info->irq);
	info->enabled = false;
	release_all_fingers(info);
/*	info->pdata->power(0);*/
	info->pdata->keyled(0);

	return 0;
}

static int abov_tk_resume(struct device *dev)
{
	struct i2c_client *client = to_i2c_client(dev);
	struct abov_tk_info *info = i2c_get_clientdata(client);

	if (info->enabled)
		return 0;

	dev_notice(&info->client->dev, "%s: users=%d\n", __func__,
		   info->input_dev->users);
/*	info->pdata->power(1);*/
	info->enabled = true;
	info->pdata->keyled(1);

	enable_irq(info->irq);

	return 0;
}
#endif

#ifdef CONFIG_HAS_EARLYSUSPEND
static void abov_tk_early_suspend(struct early_suspend *h)
{
	struct abov_tk_info *info;
	info = container_of(h, struct abov_tk_info, early_suspend);
	abov_tk_suspend(&info->client->dev);

}

static void abov_tk_late_resume(struct early_suspend *h)
{
	struct abov_tk_info *info;
	info = container_of(h, struct abov_tk_info, early_suspend);
	abov_tk_resume(&info->client->dev);
}
#endif

#ifdef CONFIG_INPUT_ENABLED
static int abov_tk_input_open(struct input_dev *dev)
{
	struct abov_tk_info *info = input_get_drvdata(dev);

	dev_info(&info->client->dev, "%s: users=%d\n", __func__,
		   info->input_dev->users);

	abov_tk_resume(&info->client->dev);

	return 0;
}
static void abov_tk_input_close(struct input_dev *dev)
{
	struct abov_tk_info *info = input_get_drvdata(dev);
	dev_info(&info->client->dev, "%s: users=%d\n", __func__,
		   info->input_dev->users);

	abov_tk_suspend(&info->client->dev);
}
#endif

#if defined(CONFIG_PM) && !defined(CONFIG_HAS_EARLYSUSPEND) &&\
	!defined(CONFIG_INPUT_ENABLED)
static const struct dev_pm_ops abov_tk_pm_ops = {
	.suspend = abov_tk_suspend,
	.resume = abov_tk_resume,
};
#endif

static const struct i2c_device_id abov_tk_id[] = {
	{ABOV_TK_NAME, 0},
	{}
};

MODULE_DEVICE_TABLE(i2c, abov_tk_id);

static struct i2c_driver abov_tk_driver = {
	.probe = abov_tk_probe,
	.remove = __devexit_p(abov_tk_remove),
	.shutdown = abov_tk_shutdown,
	.driver = {
		   .name = ABOV_TK_NAME,
#if defined(CONFIG_PM) && !defined(CONFIG_HAS_EARLYSUSPEND) &&\
	!defined(CONFIG_INPUT_ENABLED)
		   .pm = &abov_tk_pm_ops,
#endif
	},
	.id_table = abov_tk_id,
};

static int __init touchkey_init(void)
{
	return i2c_add_driver(&abov_tk_driver);
}

static void __exit touchkey_exit(void)
{
	i2c_del_driver(&abov_tk_driver);
}

late_initcall(touchkey_init);
module_exit(touchkey_exit);

/* Module information */
MODULE_AUTHOR("Samsung Electronics");
MODULE_DESCRIPTION("Touchkey driver for Abov MF16xx chip");
MODULE_LICENSE("GPL");

/*
 * driver/misc/fsa9480-muic.c - FSA9480 micro USB switch device driver
 *
 * Copyright (C) 2010 Samsung Electronics
 * Seung-Jin Hahn <sjin.hahn@samsung.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA
 *
 */

#include <linux/gpio.h>
#include <linux/i2c.h>
#include <linux/interrupt.h>
#include <linux/slab.h>
#include <linux/platform_device.h>
#include <linux/module.h>

#include <linux/muic.h>
#include <linux/fsa9480.h>

extern struct sec_switch_data sec_switch_data;

/* don't access this variable directly!! except get_switch_sel_value function.
 * you must get switch_sel value by using get_switch_sel function. */
static int switch_sel;

/* func : set_switch_sel
 * switch_sel value get from bootloader comand line
 * switch_sel data consist 4 bits
 */
static int set_switch_sel(char *str)
{
	get_option(&str, &switch_sel);
	switch_sel = switch_sel & 0x0f;
	pr_info("%s:%s switch_sel: 0x%x\n", MUIC_DEV_NAME, __func__,
			switch_sel);

	return switch_sel;
}
__setup("pmic_info=", set_switch_sel);

static int get_switch_sel(void)
{
	return switch_sel;
}

static ssize_t fsa9480_muic_show_uart_sel(struct device *dev,
					   struct device_attribute *attr,
					   char *buf)
{
	return 0;
}

static ssize_t fsa9480_muic_set_uart_sel(struct device *dev,
					  struct device_attribute *attr,
					  const char *buf, size_t count)
{
	return 0;
}

static ssize_t fsa9480_muic_show_usb_sel(struct device *dev,
					   struct device_attribute *attr,
					   char *buf)
{
	return 0;
}

static ssize_t fsa9480_muic_set_usb_sel(struct device *dev,
					  struct device_attribute *attr,
					  const char *buf, size_t count)
{
	return 0;
}

static ssize_t fsa9480_muic_show_adc(struct device *dev,
				      struct device_attribute *attr, char *buf)
{
	return 0;
}

static ssize_t fsa9480_muic_show_usb_state(struct device *dev,
					    struct device_attribute *attr,
					    char *buf)
{
	return 0;
}

#if defined(CONFIG_USB_HOST_NOTIFY)
static ssize_t fsa9480_muic_show_otg_test(struct device *dev,
					   struct device_attribute *attr,
					   char *buf)
{
	return 0;
}

static ssize_t fsa9480_muic_set_otg_test(struct device *dev,
					  struct device_attribute *attr,
					  const char *buf, size_t count)
{
	return 0;
}
#endif

static ssize_t fsa9480_muic_show_attached_dev(struct device *dev,
					 struct device_attribute *attr,
					 char *buf)
{
	return 0;
}

static ssize_t fsa9480_muic_show_audio_path(struct device *dev,
					     struct device_attribute *attr,
					     char *buf)
{
	return 0;
}

static ssize_t fsa9480_muic_set_audio_path(struct device *dev,
					    struct device_attribute *attr,
					    const char *buf, size_t count)
{
	return 0;
}


static ssize_t fsa9480_muic_show_apo_factory(struct device *dev,
					     struct device_attribute *attr,
					     char *buf)
{
	return 0;
}

static ssize_t fsa9480_muic_set_apo_factory(struct device *dev,
					    struct device_attribute *attr,
					    const char *buf, size_t count)
{
	return 0;
}

static DEVICE_ATTR(uart_sel, 0664, fsa9480_muic_show_uart_sel,
		fsa9480_muic_set_uart_sel);
static DEVICE_ATTR(usb_sel, 0664,
		fsa9480_muic_show_usb_sel, fsa9480_muic_set_usb_sel);
static DEVICE_ATTR(adc, 0664, fsa9480_muic_show_adc, NULL);
static DEVICE_ATTR(usb_state, 0664, fsa9480_muic_show_usb_state, NULL);
#if defined(CONFIG_USB_HOST_NOTIFY)
static DEVICE_ATTR(otg_test, 0664,
		fsa9480_muic_show_otg_test, fsa9480_muic_set_otg_test);
#endif
static DEVICE_ATTR(attached_dev, 0664, fsa9480_muic_show_attached_dev, NULL);
static DEVICE_ATTR(audio_path, 0664,
		fsa9480_muic_show_audio_path, fsa9480_muic_set_audio_path);
static DEVICE_ATTR(apo_factory, 0664,
		fsa9480_muic_show_apo_factory,
		fsa9480_muic_set_apo_factory);

static struct attribute *fsa9480_muic_attributes[] = {
	&dev_attr_uart_sel.attr,
	&dev_attr_usb_sel.attr,
	&dev_attr_adc.attr,
	&dev_attr_usb_state.attr,
#if defined(CONFIG_USB_HOST_NOTIFY)
	&dev_attr_otg_test.attr,
#endif
	&dev_attr_attached_dev.attr,
	&dev_attr_audio_path.attr,
	&dev_attr_apo_factory.attr,
	NULL
};

static const struct attribute_group fsa9480_muic_group = {
	.attrs = fsa9480_muic_attributes,
};

#if 0
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/err.h>
#include <linux/irq.h>
#include <linux/interrupt.h>
#include <linux/platform_device.h>
#include <linux/slab.h>
#include <mach/param.h>

int usb_cable, uart_cable;

struct fsa9480_usbsw {
	struct i2c_client		*client;
	struct muic_platform_data	*pdata;
	int				intr;
	int				dev1;
	int				dev2;
	int				mansw;
	bool				is_usb_ready;
	struct delayed_work		usb_work;
};

static struct fsa9480_usbsw *local_usbsw;

#ifdef CONFIG_USB_ANDROID_SAMSUNG_COMPOSITE
extern u16 askonstatus;
extern u16 inaskonstatus;
#endif

static ssize_t fsa9480_show_control(struct device *dev,
				   struct device_attribute *attr,
				   char *buf)
{
	struct fsa9480_usbsw *usbsw = dev_get_drvdata(dev);
	struct i2c_client *client = usbsw->client;
	int value;

	value = i2c_smbus_read_byte_data(client, FSA9480_REG_CTRL);
	if (value < 0)
		dev_err(&client->dev, "%s: err %d\n", __func__, value);

	return sprintf(buf, "CONTROL: %02x\n", value);
}

static ssize_t fsa9480_show_device_type(struct device *dev,
				   struct device_attribute *attr,
				   char *buf)
{
	struct fsa9480_usbsw *usbsw = dev_get_drvdata(dev);
	struct i2c_client *client = usbsw->client;
	int value;

	value = i2c_smbus_read_byte_data(client, FSA9480_REG_DEV_T1);
	if (value < 0)
		dev_err(&client->dev, "%s: err %d\n", __func__, value);

	return sprintf(buf, "DEVICE_TYPE: %02x\n", value);
}

static ssize_t fsa9480_show_manualsw(struct device *dev,
		struct device_attribute *attr, char *buf)
{
	struct fsa9480_usbsw *usbsw = dev_get_drvdata(dev);
	struct i2c_client *client = usbsw->client;
	s32 value;

	value = i2c_smbus_read_byte_data(client, FSA9480_REG_MANSW1);
	if (value < 0)
		dev_err(&client->dev, "%s: err %d\n", __func__, value);

	if (value == SW_VAUDIO)
		return sprintf(buf, "VAUDIO\n");
	else if (value == SW_UART)
		return sprintf(buf, "UART\n");
	else if (value == SW_AUDIO)
		return sprintf(buf, "AUDIO\n");
	else if (value == SW_DHOST)
		return sprintf(buf, "DHOST\n");
	else if (value == SW_AUTO)
		return sprintf(buf, "AUTO\n");
	else
		return sprintf(buf, "%x", value);
}

static ssize_t fsa9480_set_manualsw(struct device *dev,
				    struct device_attribute *attr,
				    const char *buf, size_t count)
{
	struct fsa9480_usbsw *usbsw = dev_get_drvdata(dev);
	struct i2c_client *client = usbsw->client;
	s32 value;
	unsigned int path = 0;
	unsigned int mansw1_value;
	int ret;

	value = i2c_smbus_read_byte_data(client, FSA9480_REG_CTRL);
	if (value < 0)
		dev_err(&client->dev, "%s: err %d\n", __func__, value);

	if ((value & ~(CON_SWITCH_OPEN | CON_MANUAL_SW)) != (CON_RAW_DATA | CON_WAIT))
		return 0;

	if (!strncmp(buf, "VAUDIO", 6)) {
		path = SW_VAUDIO;
		value &= ~CON_MANUAL_SW;
		value |= CON_SWITCH_OPEN;
	} else if (!strncmp(buf, "UART", 4)) {
		path = SW_UART;
		value &= ~CON_MANUAL_SW;
		value |= CON_SWITCH_OPEN;
	} else if (!strncmp(buf, "AUDIO", 5)) {
		path = SW_AUDIO;
		value &= ~CON_MANUAL_SW;
		value |= CON_SWITCH_OPEN;
	} else if (!strncmp(buf, "DHOST", 5)) {
		path = SW_DHOST;
		value &= ~CON_MANUAL_SW;
		value |= CON_SWITCH_OPEN;
	} else if (!strncmp(buf, "AUTO", 4)) {
		path = SW_AUTO;
		value |= CON_MANUAL_SW;
		value |= CON_SWITCH_OPEN;
	} else if (!strncmp(buf, "OPEN", 4)) {
		path = SW_AUTO;
		value |= CON_MANUAL_SW;
		value &= ~CON_SWITCH_OPEN;
	} else {
		dev_err(dev, "Wrong command\n");
		return 0;
	}

	usbsw->mansw = path;

	mansw1_value = i2c_smbus_read_byte_data(client, FSA9480_REG_MANSW1);
	if (mansw1_value < 0)
		dev_err(&client->dev, "%s: err %d\n", __func__, mansw1_value);

	mansw1_value = mansw1_value & (0x03);	// clear D+ and D- switching
	mansw1_value |= path;

	dev_info(&client->dev, "%s: manual sw1 write 0x%x\n", __func__, mansw1_value);

	ret = i2c_smbus_write_byte_data(client, FSA9480_REG_MANSW1, mansw1_value);
	if (ret < 0)
		dev_err(&client->dev, "%s: err %d\n", __func__, ret);

	ret = i2c_smbus_write_byte_data(client, FSA9480_REG_CTRL, value);
	if (ret < 0)
		dev_err(&client->dev, "%s: err %d\n", __func__, ret);

	usbsw->mansw = mansw1_value;

	return count;
}

static DEVICE_ATTR(control, S_IRUGO, fsa9480_show_control, NULL);
static DEVICE_ATTR(device_type, S_IRUGO, fsa9480_show_device_type, NULL);
static DEVICE_ATTR(switch, S_IRUGO | S_IWUSR,
		fsa9480_show_manualsw, fsa9480_set_manualsw);

static struct attribute *fsa9480_attributes[] = {
	&dev_attr_control.attr,
	&dev_attr_device_type.attr,
	&dev_attr_switch.attr,
	NULL
};

static const struct attribute_group fsa9480_group = {
	.attrs = fsa9480_attributes,
};

void fsa9480_manual_switching(int path)
{
	struct i2c_client *client = local_usbsw->client;
	s32 value;
	unsigned int data = 0;
	unsigned int mansw1_value;
	int ret;

	value = i2c_smbus_read_byte_data(client, FSA9480_REG_CTRL);
	if (value < 0)
		dev_err(&client->dev, "%s: err %d\n", __func__, value);

	if ((value & ~(CON_SWITCH_OPEN | CON_MANUAL_SW)) != (CON_RAW_DATA | CON_WAIT))
		return;

	if (path == SWITCH_PORT_VAUDIO) {
		data = SW_VAUDIO;
		value &= ~CON_MANUAL_SW;
		value |= CON_SWITCH_OPEN;
	} else if (path ==  SWITCH_PORT_UART) {
		data = SW_UART;
		value &= ~CON_MANUAL_SW;
		value |= CON_SWITCH_OPEN;
	} else if (path ==  SWITCH_PORT_AUDIO) {
		data = SW_AUDIO;
		value &= ~CON_MANUAL_SW;
		value |= CON_SWITCH_OPEN;
	} else if (path ==  SWITCH_PORT_USB) {
		data = SW_DHOST;
		value &= ~CON_MANUAL_SW;
		value |= CON_SWITCH_OPEN;
	} else if (path ==  SWITCH_PORT_AUTO) {
		data = SW_AUTO;
		value |= CON_MANUAL_SW;
		value |= CON_SWITCH_OPEN;
	} else if (path == SWITCH_PORT_OPEN) {
		data = SW_AUTO;
		value |= CON_MANUAL_SW;
		value &= ~CON_SWITCH_OPEN;
	} else {
		printk("%s: wrong path (%d)\n", __func__, path);
		return;
	}

	local_usbsw->mansw = data;

	mansw1_value = i2c_smbus_read_byte_data(client, FSA9480_REG_MANSW1);
	if (mansw1_value < 0)
		dev_err(&client->dev, "%s: err %d\n", __func__, mansw1_value);

	mansw1_value = mansw1_value & (0x03);	// clear D+ and D- switching
	mansw1_value |= data;

	dev_info(&client->dev, "%s: manual sw1 write 0x%x\n", __func__, mansw1_value);

	ret = i2c_smbus_write_byte_data(client, FSA9480_REG_MANSW1, mansw1_value);
	if (ret < 0)
		dev_err(&client->dev, "%s: err %d\n", __func__, ret);

	ret = i2c_smbus_write_byte_data(client, FSA9480_REG_CTRL, value);
	if (ret < 0)
		dev_err(&client->dev, "%s: err %d\n", __func__, ret);

	local_usbsw->mansw = mansw1_value;
}
EXPORT_SYMBOL(fsa9480_manual_switching);

#if defined(CONFIG_MACH_STEALTHV) || defined(CONFIG_MACH_AEGIS) || defined(CONFIG_MACH_CHIEF)
extern void sec_usb_switch(void);
extern void sec_uart_switch(void);
#endif

/* check OCP and OVP */
static int fsa9480_check_valid_charger(struct fsa9480_usbsw *usbsw)
{
	//if ((usbsw->intr & INT_OCP_EN) || (usbsw->intr & INT_OVP_EN))
	if (usbsw->intr & INT_OVP_EN)
		return 0;

	return 1;
}
#endif

static void fsa9480_muic_detect_dev(struct fsa9480_muic_data *muic_data)
{
	struct muic_platform_data *pdata = muic_data->pdata;
	struct sec_switch_data *switch_data = muic_data->switch_data;
	struct i2c_client *i2c = muic_data->i2c;
	int device_type, ret;
	unsigned char val1, val2;
	s32 value;

	if(switch_data->filter_vps_cb)
		switch_data->filter_vps_cb(muic_data);

#if 0
	device_type = i2c_smbus_read_word_data(client, FSA9480_REG_DEV_T1);
	if (device_type < 0)
		dev_err(&client->dev, "%s: err %d\n", __func__, device_type);

	val1 = device_type & 0xff;
	val2 = device_type >> 8;

	dev_info(&client->dev, "dev1: 0x%x, dev2: 0x%x\n", val1, val2);

	/* Attached */
	if (val1 || val2) {
		/* USB */
		if (val1 & DEV_T1_USB_MASK || val2 & DEV_T2_USB_MASK) {
			if (pdata->usb_charger_cb && fsa9480_check_valid_charger(usbsw))
				pdata->usb_charger_cb(FSA9480_ATTACHED);
			if (pdata->usb_cb && usbsw->is_usb_ready)
				pdata->usb_cb(FSA9480_ATTACHED);
#if defined(CONFIG_MACH_STEALTHV) || defined(CONFIG_MACH_AEGIS)|| defined(CONFIG_MACH_CHIEF)
			/* For only LTE device */
			sec_usb_switch();
#else
			if (usbsw->mansw) {
				ret = i2c_smbus_write_byte_data(client,
					FSA9480_REG_MANSW1, usbsw->mansw);
				if (ret < 0)
					dev_err(&client->dev,
						"%s: err %d\n", __func__, ret);
			}
#endif
		/* UART */
		} else if (val1 & DEV_T1_UART_MASK || val2 & DEV_T2_UART_MASK) {
			if (pdata->uart_cb)
				pdata->uart_cb(FSA9480_ATTACHED);
#if defined(CONFIG_MACH_STEALTHV) || defined(CONFIG_MACH_AEGIS)|| defined(CONFIG_MACH_CHIEF) 
			/* For only LTE device */
			sec_uart_switch();
#else
			if (usbsw->mansw) {
				ret = i2c_smbus_write_byte_data(client,
					FSA9480_REG_MANSW1, SW_UART);
				if (ret < 0)
					dev_err(&client->dev,
						"%s: err %d\n", __func__, ret);
			}
#endif
			if (val2 & DEV_T2_JIG_MASK) {
				if (pdata->jig_cb)
					pdata->jig_cb(FSA9480_ATTACHED);
			}
		/* CHARGER */
		} else if (val1 & DEV_T1_CHARGER_MASK) {
			if (pdata->charger_cb && fsa9480_check_valid_charger(usbsw))
				pdata->charger_cb(FSA9480_ATTACHED);
		/* Desk Dock (Stealth-V, Aegis) */
		} else if (val1 & DEV_AUDIO_1) {
#if defined (CONFIG_STEALTHV_USA) || defined (CONFIG_MACH_AEGIS) || defined(CONFIG_MACH_CHIEF)
		if (pdata->deskdock_cb)
			pdata->deskdock_cb(FSA9480_ATTACHED);

			/* Set manual switching */
			value = i2c_smbus_read_byte_data(client, FSA9480_REG_MANSW1);
			if (value < 0)
				dev_err(&client->dev, "%s: err %d\n", __func__, value);
			value = value & ~(0x03);	// vbus manual switching
			dev_info(&client->dev, "DEV_AUDIO_1(ATTACH) -> manual sw1 write 0x%x\n", value);
			ret = i2c_smbus_write_byte_data(client, FSA9480_REG_MANSW1, value);
			if (ret < 0)
				dev_err(&client->dev, "%s: err %d\n", __func__, ret);

			ret = i2c_smbus_read_byte_data(client,
					FSA9480_REG_CTRL);
			if (ret < 0)
				dev_err(&client->dev,
					"%s: err %d\n", __func__, ret);

			ret = i2c_smbus_write_byte_data(client,
					FSA9480_REG_CTRL, ret & ~CON_MANUAL_SW);
			if (ret < 0)
				dev_err(&client->dev,
					"%s: err %d\n", __func__, ret);
#endif
		/* JIG */
		} else if (val2 & DEV_T2_JIG_MASK) {
			if (pdata->jig_cb)
				pdata->jig_cb(FSA9480_ATTACHED);
		/* Desk Dock (Stealth-V, Aegis: Car Dock) */
		} else if (val2 & DEV_AV) {
#if defined (CONFIG_STEALTHV_USA) || defined (CONFIG_MACH_AEGIS) || defined(CONFIG_MACH_CHIEF)
			if (pdata->cardock_cb)
				pdata->cardock_cb(FSA9480_ATTACHED);
#else
			if (pdata->deskdock_cb)
				pdata->deskdock_cb(FSA9480_ATTACHED);
#endif
			ret = i2c_smbus_write_byte_data(client,
					FSA9480_REG_MANSW1, SW_VAUDIO);
			if (ret < 0)
				dev_err(&client->dev,
					"%s: err %d\n", __func__, ret);

			ret = i2c_smbus_read_byte_data(client,
					FSA9480_REG_CTRL);
			if (ret < 0)
				dev_err(&client->dev,
					"%s: err %d\n", __func__, ret);

			ret = i2c_smbus_write_byte_data(client,
					FSA9480_REG_CTRL, ret & ~CON_MANUAL_SW);
			if (ret < 0)
				dev_err(&client->dev,
					"%s: err %d\n", __func__, ret);
		}
	/* Detached */
	} else {
		/* USB */
		if (usbsw->dev1 & DEV_T1_USB_MASK ||
				usbsw->dev2 & DEV_T2_USB_MASK) {
			if (pdata->usb_charger_cb)
				pdata->usb_charger_cb(FSA9480_DETACHED);
#ifndef CONFIG_USB_ANDROID_SAMSUNG_COMPOSITE
			if (pdata->usb_cb && usbsw->is_usb_ready)
				pdata->usb_cb(FSA9480_DETACHED);
#else
                        /* Mass storage icon bug fix : ask on mode */
			if (pdata->usb_cb && usbsw->is_usb_ready) {
                            if ( askonstatus == 0xabcd )
                            {
                                askonstatus=0;
                                inaskonstatus=0;
                                pdata->usb_cb(FSA9480_DETACHED);
                                askonstatus = 0xabcd;
                            }
                            else
                            {
                                 askonstatus=0;
                                inaskonstatus=0;
                                pdata->usb_cb(FSA9480_DETACHED);                               
                            }
			}
#endif
		/* UART */
		} else if (usbsw->dev1 & DEV_T1_UART_MASK ||
				usbsw->dev2 & DEV_T2_UART_MASK) {
			if (pdata->uart_cb)
				pdata->uart_cb(FSA9480_DETACHED);
			if (usbsw->dev2 & DEV_T2_JIG_MASK) {
				if (pdata->jig_cb)
					pdata->jig_cb(FSA9480_DETACHED);
			}
		/* CHARGER */
		} else if (usbsw->dev1 & DEV_T1_CHARGER_MASK) {
			if (pdata->charger_cb)
				pdata->charger_cb(FSA9480_DETACHED);
		/* Desk Dock (Stealth-V, Aegis) */
		} else if (usbsw->dev1 & DEV_AUDIO_1) {
#if defined (CONFIG_STEALTHV_USA) || defined (CONFIG_MACH_AEGIS) || defined(CONFIG_MACH_CHIEF)
			if (pdata->deskdock_cb)
				pdata->deskdock_cb(FSA9480_DETACHED);

			ret = i2c_smbus_read_byte_data(client,
					FSA9480_REG_CTRL);
			if (ret < 0)
				dev_err(&client->dev,
					"%s: err %d\n", __func__, ret);

			ret = i2c_smbus_write_byte_data(client,
					FSA9480_REG_CTRL, ret | CON_MANUAL_SW);
			if (ret < 0)
				dev_err(&client->dev,
					"%s: err %d\n", __func__, ret);
#endif
		/* JIG */
		} else if (usbsw->dev2 & DEV_T2_JIG_MASK) {
			if (pdata->jig_cb)
				pdata->jig_cb(FSA9480_DETACHED);
		/* Car Dock (Stealth-V, Aegis) */
		} else if (usbsw->dev2 & DEV_AV) {
#if defined (CONFIG_STEALTHV_USA) || defined (CONFIG_MACH_AEGIS) || defined(CONFIG_MACH_CHIEF)
			if (pdata->cardock_cb)
				pdata->cardock_cb(FSA9480_DETACHED);
#else
			if (pdata->deskdock_cb)
				pdata->deskdock_cb(FSA9480_DETACHED);
#endif
			ret = i2c_smbus_read_byte_data(client,
					FSA9480_REG_CTRL);
			if (ret < 0)
				dev_err(&client->dev,
					"%s: err %d\n", __func__, ret);

			ret = i2c_smbus_write_byte_data(client,
					FSA9480_REG_CTRL, ret | CON_MANUAL_SW);
			if (ret < 0)
				dev_err(&client->dev,
					"%s: err %d\n", __func__, ret);
		}
	}

	usb_cable = val1;
	uart_cable = val2;
	usbsw->dev1 = val1;
	usbsw->dev2 = val2;
#endif
}

#if 0
static void fsa9480_detect_key(struct fsa9480_usbsw *usbsw)
{
	int button2;
	bool is_press = 0;
	bool is_long;
	unsigned char key_mask;
	unsigned int key_index;
	//struct muic_platform_data *pdata = usbsw->pdata;
	struct i2c_client *client = usbsw->client;

	button2 = i2c_smbus_read_word_data(client, FSA9480_REG_BTN2);
	if (button2 < 0)
		dev_err(&client->dev, "%s: err %d\n", __func__, button2);

	if (usbsw->intr & INT_KP) {
		is_long = 0;
	} else {
		is_long = 1;

		if (usbsw->intr & INT_LKP)
			is_press = 1;
		else
			is_press = 0;
	}

	dev_info(&client->dev, "button2 0x%x, is_press %d, is_long %d\n",
			button2, is_press, is_long);

	key_mask = (button2 & BTN2_KEY_MASK) >> BTN2_KEY_SHIFT;
	key_index = 0;

	while (key_mask) {
		if( key_mask & 0x1 ) {
			if (usbsw->pdata->key_cb) {
				if (is_long) {
					usbsw->pdata->key_cb(key_index, is_press);
				}
				else {
					usbsw->pdata->key_cb(key_index, 1);
					usbsw->pdata->key_cb(key_index, 0);
				}
			}
		}

		key_mask >>= 1;
		key_index++;
	}				
}
#endif

#if 0
static void fsa9480_muic_usb_detect(struct work_struct *work)
{
	struct fsa9480_usbsw *usbsw = container_of(work,
			struct fsa9480_usbsw, usb_work.work);

	usbsw->is_usb_ready = true;

	fsa9480_muic_detect_dev(muic_data);
}
#endif

static void fsa9480_muic_init_detect(struct work_struct *work)
{
	struct fsa9480_muic_data *muic_data =
		container_of(work, struct fsa9480_muic_data, init_work.work);

	pr_info("%s:%s\n", MUIC_DEV_NAME, __func__);

	mutex_lock(&muic_data->muic_mutex);
	fsa9480_muic_detect_dev(muic_data);
	mutex_unlock(&muic_data->muic_mutex);
}

static int fsa9480_init_rev_info(struct fsa9480_muic_data *muic_data)
{
	u8 dev_id;
	int ret = 0;

	pr_info("%s:%s\n", MUIC_DEV_NAME, __func__);

	dev_id = i2c_smbus_read_byte_data(muic_data->i2c, FSA9480_MUIC_REG_DEVID);
	if (dev_id < 0) {
		pr_err("%s:%s i2c io error(%d)\n", MUIC_DEV_NAME, __func__,
				ret);
		ret = -ENODEV;
	} else {
		muic_data->muic_vendor = (dev_id & 0x7);
		muic_data->muic_version = ((dev_id & 0xF8) >> 3);
		pr_info("%s:%s device found: vendor=0x%x, ver=0x%x\n",
				MUIC_DEV_NAME, __func__, muic_data->muic_vendor,
				muic_data->muic_version);
	}

	return ret;
}

static int fsa9480_muic_reg_init(struct fsa9480_muic_data *muic_data)
{
	struct i2c_client *client = muic_data->i2c;
	unsigned int ctrl = CTRL_MASK;
	int mansw = -1;
	int ret;

	pr_info("%s:%s\n", MUIC_DEV_NAME, __func__);

	/* mask interrupts (unmask attach/detach only) */
	ret = i2c_smbus_write_word_data(client, FSA9480_MUIC_REG_INTMASK1,
			((REG_INTMASK2_VALUE << 8) | REG_INTMASK1_VALUE));
	if (ret < 0)
		pr_err("%s: err mask interrupts(%d)\n", __func__, ret);

	/* mask all car kit interrupts */
	ret = i2c_smbus_write_word_data(client, FSA9480_MUIC_REG_CK_INTMASK1,
			0x07ff);
	if (ret < 0)
		pr_err("%s: err mask car kit interrupts(%d)\n", __func__, ret);

	/* Timing1 - Keypress, ADC Dectet Time */
	ret = i2c_smbus_write_byte_data(client, FSA9480_MUIC_REG_TIMING1,
			REG_TIMING1_VALUE);
	if (ret < 0)
		pr_err("%s: err set timing1(%d)\n", __func__, ret);

	/* Timing2 - Long Key Press */
	ret = i2c_smbus_write_byte_data(client, FSA9480_MUIC_REG_TIMING2,
			REG_TIMING2_VALUE);
	if (ret < 0)
		pr_err("%s: err set timing2(%d)\n", __func__, ret);

	ret = i2c_smbus_read_byte_data(client, FSA9480_MUIC_REG_MANSW1);
	if (ret < 0)
		pr_err("%s: err read mansw1(%d)\n", __func__, ret);
	else
		mansw = ret;

	if (mansw > 0)
		ctrl &= ~CTRL_MANUAL_SW_MASK;	/* Manual Switching Mode */

	ret = i2c_smbus_write_byte_data(client, FSA9480_MUIC_REG_CTRL, ctrl);
	if (ret < 0)
		dev_err(&client->dev, "%s: err %d\n", __func__, ret);

	return ret;
}

static irqreturn_t fsa9480_muic_irq_thread(int irq, void *data)
{
	struct fsa9480_muic_data *muic_data = data;
	struct i2c_client *client = muic_data->i2c;
	int intr;

	pr_info("%s:%s irq(%d)\n", MUIC_DEV_NAME, __func__, irq);

	mutex_lock(&muic_data->muic_mutex);

	/* read and clear interrupt status bits */
	intr = i2c_smbus_read_word_data(client, FSA9480_MUIC_REG_INT1);
	if (intr < 0) {
		pr_err("%s: err read interrupt status %d\n", __func__, intr);
	} else if (intr == 0) {
#if defined(CONFIG_MUIC_FSA9480_SUPPORT_DESK_DOCK)
		/* Support Desktopdock charging (vbus manual switching) */
		if (muic_data->attached_dev == ATTACHED_DEV_DESKDOCK_MUIC)
			goto skip;
#endif /* CONFIG_MUIC_FSA9480_SUPPORT_DESK_DOCK */

		/* interrupt was fired, but no status bits were set,
		so device was reset. In this case, the registers were
		reset to defaults so they need to be reinitialised. */
		fsa9480_muic_reg_init(muic_data);
	}

skip:
	pr_info("%s:%s intr(0x%x)\n", MUIC_DEV_NAME, __func__, intr);

	/* device detection */
	fsa9480_muic_detect_dev(muic_data);

	mutex_unlock(&muic_data->muic_mutex);

	return IRQ_HANDLED;
}

static int fsa9480_muic_irq_init(struct fsa9480_muic_data *muic_data)
{
	struct i2c_client *client = muic_data->i2c;
	struct muic_platform_data *pdata = muic_data->pdata;
	int ret = 0;

	pr_info("%s:%s\n", MUIC_DEV_NAME, __func__);

	if (!pdata->irq_gpio) {
		pr_warn("%s:%s No interrupt specified\n", MUIC_DEV_NAME,
				__func__);
		return -ENXIO;
	}

	ret = gpio_request(pdata->irq_gpio, "muic_irq");
	if (ret) {
		pr_err("%s:%s failed request gpio(%d)\n", MUIC_DEV_NAME,
				__func__, pdata->irq_gpio);
		return ret;
	}
	gpio_direction_input(pdata->irq_gpio);
	gpio_free(pdata->irq_gpio);

	client->irq = gpio_to_irq(pdata->irq_gpio);

	if (client->irq) {
		ret = request_threaded_irq(client->irq, NULL,
				fsa9480_muic_irq_thread,
				(IRQF_TRIGGER_FALLING | IRQF_NO_SUSPEND),
				"fsa9480-muic", muic_data);
		if (ret < 0) {
			pr_err("%s:%s failed to reqeust IRQ(%d)\n",
					MUIC_DEV_NAME, __func__, client->irq);
			return ret;
		}

		ret = enable_irq_wake(client->irq);
		if (ret < 0)
			pr_err("%s:%s failed to enable wakeup src\n",
					MUIC_DEV_NAME, __func__);
	}

	return ret;
}

static int fsa9480_muic_probe(struct i2c_client *i2c,
				const struct i2c_device_id *id)
{
	struct i2c_adapter *adapter = to_i2c_adapter(i2c->dev.parent);
	struct muic_platform_data *pdata = i2c->dev.platform_data;
	struct fsa9480_muic_data *muic_data;
	int ret = 0;

	pr_info("%s:%s\n", MUIC_DEV_NAME, __func__);

	if (!i2c_check_functionality(adapter, I2C_FUNC_SMBUS_BYTE_DATA)) {
		pr_err("%s: i2c functionality check error\n", __func__);
		ret = -EIO;
		goto err_return;
	}

	muic_data = kzalloc(sizeof(struct fsa9480_muic_data), GFP_KERNEL);
	if (!muic_data) {
		pr_err("%s: failed to allocate driver data\n", __func__);
		ret = -ENOMEM;
		goto err_return;
	}

	if (!pdata) {
		pr_err("%s: failed to get i2c platform data\n", __func__);
		ret = -ENODEV;
		goto err_io;
	}
	muic_data->pdata = pdata;

	muic_data->i2c = i2c;
	i2c_set_clientdata(i2c, muic_data);

	muic_data->switch_data = &sec_switch_data;

	if (pdata->init_gpio_cb)
		pdata->init_gpio_cb(get_switch_sel());

	mutex_init(&muic_data->muic_mutex);

#if 0
	muic_data->is_factory_start = false;

	wake_lock_init(&muic_data->muic_wake_lock, WAKE_LOCK_SUSPEND,
		"muic wake lock");
#endif

	muic_data->attached_dev = ATTACHED_DEV_UNKNOWN_MUIC;

	/* create sysfs group */
	ret = sysfs_create_group(&switch_device->kobj, &fsa9480_muic_group);
	if (ret) {
		pr_err("%s: failed to create fsa9480 muic attribute group\n",
			__func__);
		goto fail;
	}

	ret = fsa9480_init_rev_info(muic_data);
	if (ret) {
		pr_err("%s: failed to init muic rev register(%d)\n", __func__,
			ret);
		goto fail;
	}

	ret = fsa9480_muic_reg_init(muic_data);
	if (ret) {
		pr_err("%s: failed to init muic register(%d)\n", __func__, ret);
		goto fail;
	}

	ret = fsa9480_muic_irq_init(muic_data);
	if (ret) {
		pr_err("%s: failed to init muic irq(%d)\n", __func__, ret);
		goto fail_init_irq;
	}

	/* initial cable detection */
	INIT_DELAYED_WORK(&muic_data->init_work, fsa9480_muic_init_detect);
	schedule_delayed_work(&muic_data->init_work, msecs_to_jiffies(3000));
#if 0
	INIT_DELAYED_WORK(&muic_data->usb_work, fsa9480_muic_usb_detect);
	schedule_delayed_work(&muic_data->usb_work, msecs_to_jiffies(17000));
#endif

	return 0;

fail_init_irq:
	if (i2c->irq)
		free_irq(i2c->irq, muic_data);
fail:
#if 0
	wake_lock_destroy(&info->muic_wake_lock);
#endif
	mutex_destroy(&muic_data->muic_mutex);
	i2c_set_clientdata(i2c, NULL);
err_io:
	kfree(muic_data);
err_return:
	return ret;
}

static int __devexit fsa9480_muic_remove(struct i2c_client *i2c)
{
#if 0
	struct fsa9480_muic_info *info = platform_get_drvdata(pdev);
	sysfs_remove_group(&switch_device->kobj, &fsa9480_muic_group);

	if (info) {
		dev_info(info->dev, "%s:%s\n", MUIC_DEV_NAME, __func__);
		cancel_delayed_work(&info->init_work);
		cancel_delayed_work(&info->usb_work);
		disable_irq_wake(info->irq_muic);
		free_irq(info->irq_muic, info);
		wake_lock_destroy(&info->muic_wake_lock);
		mutex_destroy(&info->mutex);
		platform_set_drvdata(pdev, NULL);
		kfree(info);
	}
#endif
	return 0;
}

static const struct i2c_device_id fsa9480_i2c_id[] = {
	{ MUIC_DEV_NAME, 0 },
	{}
};
MODULE_DEVICE_TABLE(i2c, fsa9480_i2c_id);

static void fsa9480_muic_shutdown(struct i2c_client *i2c)
{
#if 0
	struct fsa9480_muic_info *info = dev_get_drvdata(dev);
	int ret;
	u8 val;

	dev_info(info->dev, "%s:%s\n", MUIC_DEV_NAME, __func__);
	if (!info->muic) {
		dev_err(info->dev, "%s: no muic i2c client\n", __func__);
		return;
	}

	dev_info(info->dev, "%s: JIGSet: auto detection\n", __func__);
	val = (0 << CTRL3_JIGSET_SHIFT) | (0 << CTRL3_BOOTSET_SHIFT);

	ret = fsa9480_update_reg(info->muic, MAX77693_MUIC_REG_CTRL3, val,
			CTRL3_JIGSET_MASK | CTRL3_BOOTSET_MASK);
	if (ret < 0) {
		dev_err(info->dev, "%s: fail to update reg\n", __func__);
		return;
	}
#endif
}

static struct i2c_driver fsa9480_muic_driver = {
	.driver		= {
		.name	= MUIC_DEV_NAME,
		.owner	= THIS_MODULE,
	},
	.probe		= fsa9480_muic_probe,
	.remove		= __devexit_p(fsa9480_muic_remove),
	.shutdown	= fsa9480_muic_shutdown,
	.id_table	= fsa9480_i2c_id,
};

static int __init fsa9480_muic_init(void)
{
	pr_info("%s:%s\n", MUIC_DEV_NAME, __func__);
	return i2c_add_driver(&fsa9480_muic_driver);
}
module_init(fsa9480_muic_init);

static void __exit fsa9480_muic_exit(void)
{
	pr_info("%s:%s\n", MUIC_DEV_NAME, __func__);
	i2c_del_driver(&fsa9480_muic_driver);
}
module_exit(fsa9480_muic_exit);

MODULE_AUTHOR("Seung-Jin Hahn <sjin.hahn@samsung.com>");
MODULE_DESCRIPTION("FSA9480 USB Switch driver");
MODULE_LICENSE("GPL");

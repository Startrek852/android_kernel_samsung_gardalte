/*
 * driver/irda IRDA driver
 *
 * Copyright (C) 2012, 2013 Samsung Electronics
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

#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/err.h>
#include <linux/i2c.h>
#include <linux/irq.h>
#include <linux/interrupt.h>
#include <linux/platform_device.h>
#include <linux/slab.h>
#include <linux/delay.h>
#include <linux/workqueue.h>
#include <linux/device.h>
#include <linux/platform_data/mc96fr116c.h>
#include "mc96fr116c_fw.h"

#define MAX_SIZE		2048
#define MC96FR116C_READ_LENGTH	8
#define DUMMY			0xffff

#define MC96FR116C_RESET_BOOTLOADER_MODE	0
#define MC96FR116C_RESET_USER_IR_MODE		1

#define MC96FR116C_UNKNOWN_MODE		0
#define MC96FR116C_BOOTLOADER_MODE	1
#define MC96FR116C_USER_IR_MODE		2

/* Delays for both mode of them */
#define MC96FR116C_START_FIRMWARE_DELAY_MS		75
#define MC96FR116C_I2C_RECEIVE_DELAY_MS			25

/* Delays for bootloader mode */
#define MC96FR116C_WATCHDOG_RESET_DELAY_MS		50
#define MC96FR116C_CHECKSUM_CALCULATION_DELAY_MS	30
#define MC96FR116C_ERASE_PROGRAM_CODE_DELAY_MS		30
#define MC96FR116C_RETRY_UPDATING_FW_DELAY_MS		100

/* Delays for user IR mode */
#define MC96FR116C_WAKEUP_STABLE_DELAY_MS		21
#define MC96FR116C_IDLE_MODE_WAIT_DELAY_MS		50
#define MC96FR116C_WAKE_ACTIVE_LOW_DELAY_US		100
#define MC96FR116C_RETRY_I2C_WRITE_DELAY_MS		10
#define MC96FR116C_ENTER_IDLE_STATE_DELAY_MS		400

struct mc96fr116c_data {
	struct mutex			signal_mutex;
	struct i2c_client		*client;
	struct mc96fr116c_platform_data	*pdata;
	u8 signal[MAX_SIZE];
	int count;
	int dev_id;
	int ir_freq;
	int ir_sum;
	int on_off;
	int mode;
	bool last_send_ok;
};

static const u8 INITIAL_BOOT_MODE_DATA[MC96FR116C_READ_LENGTH] = {
	0x00, 0x01, 0x3F, 0xFF, 0x10, 0x00, 0x01, 0x4F,
};
static const u8 CHECKSUM_CALCULATION_COMMAND[8] = {
	0x3A, 0x02, 0x10, 0x00, 0xF0, 0x20, 0xFF, 0xDF,
};
static const u8 SW_RESET_COMMAND[8] = {
	0x3A, 0x02, 0x10, 0x00, 0xF3, 0x10, 0x00, 0xEB,
};

static bool mc96fr116c_wakeup_stop_mode(struct mc96fr116c_data *data)
{
	struct i2c_client *client = data->client;
	int retry = 10;

	if (data->mode == MC96FR116C_BOOTLOADER_MODE) {
		dev_warn(&client->dev, "%s: No need wake up in not Bootloader mode\n",
				__func__);
		return true;
	} else if (data->mode == MC96FR116C_UNKNOWN_MODE) {
		dev_warn(&client->dev, "%s: Try to enter idle state\n",
				__func__);
		data->pdata->ir_wake_en(1);
		msleep(MC96FR116C_ENTER_IDLE_STATE_DELAY_MS);
	}

	if (data->pdata->ir_read_ready())
		return true;

	data->pdata->ir_wake_en(0);
	msleep(MC96FR116C_WAKEUP_STABLE_DELAY_MS);
	do {
		udelay(MC96FR116C_WAKE_ACTIVE_LOW_DELAY_US);
		if (data->pdata->ir_read_ready()) {
			data->pdata->ir_wake_en(1);
			return true;
		}
		dev_warn(&client->dev, "%s: retry %d for idle state\n",
			 __func__, retry);
	} while (retry--);

	dev_err(&client->dev, "%s: Can not wake up\n", __func__);

	return false;
}

static bool mc96fr116c_make_idle_state(struct mc96fr116c_data *data)
{
	struct i2c_client *client = data->client;
	int retry = 10;

	data->pdata->ir_wake_en(1);
	do {
		udelay(MC96FR116C_WAKE_ACTIVE_LOW_DELAY_US);

		if (data->pdata->ir_read_ready())
			return true;

		dev_warn(&client->dev, "%s: retry %d for idle state\n",
			 __func__, retry);

		msleep(MC96FR116C_IDLE_MODE_WAIT_DELAY_MS);
	} while (retry--);

	dev_err(&client->dev, "%s: Can not make idle state\n", __func__);

	return false;
}

static int mc96fr116c_send_command(struct mc96fr116c_data *data,
		const u8 *command, int len)
{
	struct i2c_client *client = data->client;
	int ret;

	if (data->mode == MC96FR116C_USER_IR_MODE)
		mc96fr116c_wakeup_stop_mode(data);

	ret = i2c_master_send(client, command, len);

	if (ret == len) {
		ret = 0;
	} else  {
		dev_err(&client->dev, "%s: i2c communication error %d\n",
				__func__, ret);
		print_hex_dump_bytes("command: ", DUMP_PREFIX_NONE,
				command, len);
		ret = -EIO;
	}

	return ret;
}

static bool mc96fr116c_is_valid_code_checksum(struct mc96fr116c_data *data)
{
	struct i2c_client *client = data->client;
	int ret;
	u8 i2c_buf[MC96FR116C_READ_LENGTH];

	if (data->mode != MC96FR116C_BOOTLOADER_MODE) {
		dev_warn(&client->dev, "%s: No bootloader mode\n", __func__);
		return false;
	}

	ret = i2c_master_recv(client, i2c_buf, sizeof(i2c_buf));
	msleep(MC96FR116C_I2C_RECEIVE_DELAY_MS);
	if (ret < 0 || ret != sizeof(i2c_buf)) {
		dev_err(&client->dev, "%s: i2c communication error %d\n",
				__func__, ret);

		goto out;
	}

	if (!memcmp(i2c_buf, VALID_CODE_CHECKSUM, sizeof(VALID_CODE_CHECKSUM)))
		return true;

	print_hex_dump_bytes("Checksum error: ", DUMP_PREFIX_NONE,
			i2c_buf, sizeof(i2c_buf));
out:
	return false;
}

static bool mc96fr116c_ensure_user_ir_mode(struct mc96fr116c_data *data)
{
	struct i2c_client *client = data->client;
	int ret;
	u8 i2c_buf[MC96FR116C_READ_LENGTH];

	ret = i2c_master_recv(client, i2c_buf, sizeof(i2c_buf));
	msleep(MC96FR116C_I2C_RECEIVE_DELAY_MS);
	if (ret < 0 || ret != sizeof(i2c_buf)) {
		dev_err(&client->dev, "%s: i2c communication error %d\n",
				__func__, ret);

		goto out;
	}

	if (!memcmp(i2c_buf, INITIAL_USER_IR_MODE_DATA,
				sizeof(INITIAL_USER_IR_MODE_DATA))) {
		data->dev_id = i2c_buf[4] << BITS_PER_BYTE | i2c_buf[5];
		data->mode = MC96FR116C_USER_IR_MODE;
		mc96fr116c_make_idle_state(data);
		if (!ret)
			dev_warn(&client->dev, "%s: Can not make idle state.\n",
					__func__);
		return true;
	}

	print_hex_dump_bytes("NOT User IR Mode: ", DUMP_PREFIX_NONE,
			i2c_buf, sizeof(i2c_buf));
out:
	data->mode = MC96FR116C_UNKNOWN_MODE;
	return false;
}

static int mc96fr116c_reset_sw(struct mc96fr116c_data *data, int entry_mode)
{
	int ret;

	if (data->mode == MC96FR116C_USER_IR_MODE) {
		dev_err(&data->client->dev, "%s: Not support sw reset in user IR mode.\n",
				__func__);
		return -EINVAL;
	}

	ret = mc96fr116c_send_command(data, SW_RESET_COMMAND,
			sizeof(SW_RESET_COMMAND));
	if (ret < 0)
		return ret;

	data->pdata->ir_wake_en(1);	/* USER IR MODE */
	msleep(MC96FR116C_WATCHDOG_RESET_DELAY_MS);
	msleep(MC96FR116C_START_FIRMWARE_DELAY_MS);

	ret = mc96fr116c_ensure_user_ir_mode(data);
	if (!ret) {
		dev_err(&data->client->dev, "%s: Reset complete. But It was not entered to User IR mode.\n",
				__func__);
		return -EINVAL;
	}

	return 0;
}

static int mc96fr116c_check_need_fw_update(struct mc96fr116c_data *data)
{
	struct i2c_client *client = data->client;
	u8 i2c_buf[MC96FR116C_READ_LENGTH];
	int ret;
	int retry_count;
	bool boot_condition_ok = false;

	mc96fr116c_wakeup_stop_mode(data);
	for (retry_count = 0; retry_count < 5; retry_count++) {
		ret = i2c_master_recv(client, i2c_buf, sizeof(i2c_buf));
		msleep(MC96FR116C_I2C_RECEIVE_DELAY_MS);
		if (ret < 0 || ret != sizeof(i2c_buf)) {
			dev_err(&client->dev, "%s: i2c communication error %d\n",
					__func__, ret);
			continue;
		}

		if (!memcmp(INITIAL_BOOT_MODE_DATA, i2c_buf,
					MC96FR116C_READ_LENGTH)) {
			dev_dbg(&client->dev, "%s: Bootloader mode, Start address is 3FFF\n",
					__func__);
			boot_condition_ok = true;
			data->mode = MC96FR116C_BOOTLOADER_MODE;
			break;
		} else if (!memcmp(INITIAL_USER_IR_MODE_DATA, i2c_buf,
					MC96FR116C_READ_LENGTH)) {
			dev_warn(&client->dev, "%s: User IR mode, F/W version is V%d.%d",
					__func__, i2c_buf[2], i2c_buf[3]);

			data->mode = MC96FR116C_USER_IR_MODE;
			if (mc96fr116c_reset_sw(data,
					MC96FR116C_RESET_BOOTLOADER_MODE)
					== -EINVAL) {
				return 0;
			}
			retry_count--;
			continue;
		}
		dev_warn(&client->dev, "%s: Retry i2c read %d\n", __func__,
				retry_count);
	}

	if (!boot_condition_ok) {
		if (ret == sizeof(i2c_buf)) {
			dev_err(&client->dev, "%s: The start address is NOT 3FFFh\n",
					__func__);
			print_hex_dump_bytes("Received data: ",
					DUMP_PREFIX_NONE,
					i2c_buf, sizeof(i2c_buf));
			ret = -EINVAL;
		}
		goto error;
	}

	for (retry_count = 0; retry_count < 5; retry_count++) {
		ret = mc96fr116c_send_command(data,
				CHECKSUM_CALCULATION_COMMAND,
				sizeof(CHECKSUM_CALCULATION_COMMAND));

		msleep(MC96FR116C_CHECKSUM_CALCULATION_DELAY_MS);
		if (!ret)
			break;

		dev_warn(&client->dev, "%s: Retry %d i2c write, ret: %d\n",
				__func__, retry_count, ret);
	}

	if (ret < 0)
		goto error;

	msleep(MC96FR116C_CHECKSUM_CALCULATION_DELAY_MS);
	ret = mc96fr116c_is_valid_code_checksum(data);
	if (!ret) {
		ret = -EINVAL;
		goto error;
	}

	return 0;
error:
	return ret;
}

static int __mc96fr116c_update_fw(struct mc96fr116c_data *data)
{
	struct i2c_client *client = data->client;
	u8 i2c_buf[8];
	int i;
	int ret;

	for (i = 0; i < FRAME_COUNT - 1; i++) {
		ret = i2c_master_send(client, &IRDA_binary[i * ONE_PACKET_SIZE],
				ONE_PACKET_SIZE);
		if (ret < 0 || ret != ONE_PACKET_SIZE) {
			dev_err(&client->dev, "%s: Error sending packet %d.\n",
					__func__, i);
			return -EIO;
		}

		msleep(MC96FR116C_ERASE_PROGRAM_CODE_DELAY_MS);
	}

	ret = i2c_master_send(client, &IRDA_binary[i * ONE_PACKET_SIZE], 6);
	if (ret < 0 || ret != 6) {
		dev_err(&client->dev, "%s: Error sending packet %d.\n",
				__func__, i);
		return -EIO;
	}

	msleep(MC96FR116C_ERASE_PROGRAM_CODE_DELAY_MS);
	msleep(MC96FR116C_CHECKSUM_CALCULATION_DELAY_MS);

	ret = mc96fr116c_is_valid_code_checksum(data);
	if (!ret) {
		dev_err(&client->dev, "%s: Updating complete, but checksum error.\n",
				__func__);
		return -EIO;
	}

	return 0;
}

static int mc96fr116c_update_fw(struct mc96fr116c_data *data)
{
	struct i2c_client *client = data->client;
	int retry_count;
	int ret;

	for (retry_count = 0; retry_count < 5; retry_count++) {
		ret = __mc96fr116c_update_fw(data);
		if (!ret)
			return 0;

		dev_warn(&client->dev, "%s: Retry %d for updating fw.\n",
				__func__, retry_count);
		msleep(MC96FR116C_RETRY_UPDATING_FW_DELAY_MS);
	}

	dev_err(&client->dev, "%s: Failed to update F/W\n", __func__);

	return ret;
}

static void mc96fr116c_push_checksum(struct mc96fr116c_data *data, int count)
{
	int i;
	u16 checksum = 0;

	data->signal[0] = count >> BITS_PER_BYTE;
	data->signal[1] = count & 0xFF;

	for (i = 0; i < count; i++)
		checksum += data->signal[i];

	dev_dbg(&data->client->dev, "%s: checksum: %04X\n", __func__, checksum);

	data->signal[count]	= checksum >> BITS_PER_BYTE;
	data->signal[count + 1]	= checksum & 0xFF;
}

static int mc96fr116c_read_device_id(struct mc96fr116c_data *data)
{
	struct i2c_client *client = data->client;
	u8 i2c_buf[8];
	int ret;

	if (data->mode != MC96FR116C_USER_IR_MODE) {
		dev_err(&client->dev, "%s: Reading the device id was proceeded in not User IR mode.\n",
				__func__);

		return -EINVAL;
	}

	mc96fr116c_wakeup_stop_mode(data);
	ret = i2c_master_recv(client, i2c_buf, MC96FR116C_READ_LENGTH);
	msleep(MC96FR116C_I2C_RECEIVE_DELAY_MS);
	if (ret < 0 || ret != MC96FR116C_READ_LENGTH) {
		dev_err(&client->dev, "%s: i2c communication error %d\n",
				__func__, ret);
		return ret;
	}

	if (memcmp(i2c_buf, INITIAL_USER_IR_MODE_DATA,
				sizeof(INITIAL_USER_IR_MODE_DATA))) {
		dev_warn(&client->dev, "%s: Need Firmware update.\n", __func__);
		print_hex_dump_bytes("i2c_buf: ", DUMP_PREFIX_NONE, i2c_buf,
				sizeof(i2c_buf));
	}

	data->dev_id = i2c_buf[4] << BITS_PER_BYTE | i2c_buf[5];
	dev_info(&client->dev, "Device ID: 0x%04X\n", data->dev_id);

	ret = mc96fr116c_make_idle_state(data);
	if (!ret)
		dev_warn(&client->dev, "%s: Can not make idle state.\n",
				__func__);

	return 0;
}

static void mc96fr116c_work(struct mc96fr116c_data *data, int count)
{
	struct i2c_client *client = data->client;
	int buf_size = count + 2;
	int retry;
	int ret;
	bool send_checksum_ok = false;
	int sleep_timing;
	int end_data;
	int emission_time;

	dev_dbg(&client->dev, "%s: total buf_size: %d\n", __func__, buf_size);

	data->pdata->ir_irled_onoff(1);
	mc96fr116c_push_checksum(data, count);

	for (retry = 2; retry; retry--) {
		mc96fr116c_wakeup_stop_mode(data);
		ret = i2c_master_send(client, data->signal, buf_size);

		msleep(MC96FR116C_RETRY_I2C_WRITE_DELAY_MS);

		if (ret == buf_size)
			break;

		if (ret < 0)
			dev_err(&client->dev, "%s: i2c send failed. Retry %d\n",
					__func__, retry);
	}

	for (retry = 10; retry; msleep(1), retry--) {
		if (!data->pdata->ir_read_ready()) {
			send_checksum_ok = true;
			break;
		}
		dev_dbg(&client->dev, "%s: Checking checksum. Retry %d\n",
				__func__, retry);
	}

	end_data = data->signal[count - 2] << 8 | data->signal[count - 1];
	emission_time = (1000 * (data->ir_sum - end_data) / (data->ir_freq))
		+ 10;
	sleep_timing = emission_time - 130;
	if (sleep_timing > 0)
		msleep(sleep_timing);
	emission_time = (1000 * (data->ir_sum) / (data->ir_freq)) + 50;
	if (emission_time > 0)
		msleep(emission_time);

	if (data->pdata->ir_read_ready()) {
		if (send_checksum_ok)
			data->last_send_ok = true;
	} else {
		dev_err(&client->dev, "%s: Sending IR NG!\n", __func__);
		data->last_send_ok = false;
	}

	data->pdata->ir_irled_onoff(0);
	data->count = 2;
	data->ir_freq = 0;
	data->ir_sum = 0;
}

static ssize_t remocon_store(struct device *dev, struct device_attribute *attr,
		const char *buf, size_t size)
{
	struct mc96fr116c_data *data = dev_get_drvdata(dev);
	unsigned int value;
	int i;

	if (!mutex_trylock(&data->signal_mutex))
		return -EBUSY;

	for (i = 0; i < MAX_SIZE; i++) {
		if (sscanf(buf++, "%u", &value) == 1) {
			if (value == 0 || buf == '\0')
				break;

			if (data->count == 2) {
				data->ir_freq = value;
				data->signal[2] = value >> 16;
				data->signal[3] = (value >> 8) & 0xFF;
				data->signal[4] = value & 0xFF;
				data->count += 3;
			} else {
				data->ir_sum += value;
				data->signal[data->count] = value >> 8;
				data->signal[data->count + 1] = value & 0xFF;
				data->count += 2;
			}

			while (value > 0) {
				buf++;
				value /= 10;
			}
		} else {
			break;
		}
	}

	mc96fr116c_work(data, data->count);

	mutex_unlock(&data->signal_mutex);

	return size;
}

static ssize_t remocon_show(struct device *dev, struct device_attribute *attr,
		char *buf)
{
	struct mc96fr116c_data *data = dev_get_drvdata(dev);
	int i;
	char *bufp = buf;

	for (i = 5; i < MAX_SIZE - 1; i++) {
		if (data->signal[i] == 0 && data->signal[i + 1] == 0)
			break;
		else
			bufp += sprintf(bufp, "%u,", data->signal[i]);
	}
	return strlen(buf);
}
static DEVICE_ATTR(ir_send, 0664, remocon_show, remocon_store);

static ssize_t remocon_ack(struct device *dev, struct device_attribute *attr,
		char *buf)
{
	struct mc96fr116c_data *data = dev_get_drvdata(dev);

	return sprintf(buf, "%d\n", !!data->last_send_ok);
}
static DEVICE_ATTR(ir_send_result, 0664, remocon_ack, NULL);

static ssize_t device_id_show(struct device *dev, struct device_attribute *attr,
		char *buf)
{
	struct mc96fr116c_data *data = dev_get_drvdata(dev);

	mc96fr116c_read_device_id(data);

	return scnprintf(buf, 5, "%04X\n", data->dev_id);
}
static DEVICE_ATTR(device_id, 0664, device_id_show, NULL);

static ssize_t write_checksum_calculation_command(struct device *dev,
		struct device_attribute *attr, const char *buf, size_t count)
{
	struct mc96fr116c_data *data = dev_get_drvdata(dev);
	int ret;

	if (data->mode != MC96FR116C_BOOTLOADER_MODE) {
		dev_err(dev, "%s: Can not calculate checksum in not bootloader mode\n",
				__func__);
		return -EINVAL;
	}

	ret = mc96fr116c_send_command(data, CHECKSUM_CALCULATION_COMMAND,
			sizeof(CHECKSUM_CALCULATION_COMMAND));

	msleep(MC96FR116C_CHECKSUM_CALCULATION_DELAY_MS);
	if (ret)
		return -EINVAL;

	return count;
}
static DEVICE_ATTR(write_checksum_command, 0220, NULL,
		write_checksum_calculation_command);

static int __devinit mc96fr116c_probe(struct i2c_client *client,
		const struct i2c_device_id *id)
{
	struct i2c_adapter *adapter = to_i2c_adapter(client->dev.parent);
	struct mc96fr116c_data *data;
	struct device *mc96fr116c_dev;
	int i;
	int ret;

	if (!i2c_check_functionality(adapter, I2C_FUNC_I2C))
		return -EIO;

	data = kzalloc(sizeof(struct mc96fr116c_data), GFP_KERNEL);
	if (NULL == data) {
		dev_err(&client->dev, "Failed to data allocate %s\n", __func__);
		ret = -ENOMEM;
		goto err_free_mem;
	}

	data->client = client;
	data->pdata = client->dev.platform_data;

	if (!data->pdata) {
		dev_err(&client->dev, "%s: The platform data for mc96fr116c was not found\n",
				__func__);
		ret = -ENODEV;
		goto err_free_mem;
	}

	mutex_init(&data->signal_mutex);
	data->count = 2;
	data->on_off = 1;
	data->mode = MC96FR116C_UNKNOWN_MODE;

	i2c_set_clientdata(client, data);

	ret = mc96fr116c_check_need_fw_update(data);
	if (ret) {
		pr_info("%s: Need firmware update!", __func__);
		ret = mc96fr116c_update_fw(data);
	}

	if (ret < 0)
		goto err_remove_clientdata;

	if (data->mode == MC96FR116C_BOOTLOADER_MODE) {
		ret = mc96fr116c_reset_sw(data, MC96FR116C_RESET_USER_IR_MODE);

		if (ret < 0) {
			ret = -ENODEV;
			goto err_remove_clientdata;
		}
	}

	mc96fr116c_read_device_id(data);

	if (data->dev_id !=
			(INITIAL_USER_IR_MODE_DATA[4] << BITS_PER_BYTE |
			INITIAL_USER_IR_MODE_DATA[5])) {
		dev_err(&client->dev, "%s: data->dev_id is %04X. It is not matched with %04X.\n",
				__func__, data->dev_id,
				(INITIAL_USER_IR_MODE_DATA[4] << BITS_PER_BYTE |
				INITIAL_USER_IR_MODE_DATA[5]));
		ret = -ENODEV;
		goto err_remove_clientdata;
	}

	pr_info("MC96FR116C IR Blaster, Abov semicondoctor Co., Ltd.\n");

	mc96fr116c_dev = device_create(sec_class, NULL, 0, data, "sec_ir");

	if (IS_ERR(mc96fr116c_dev))
		dev_err(&client->dev, "Failed to create mc96fr116c_dev device\n");

	if (device_create_file(mc96fr116c_dev, &dev_attr_ir_send) < 0)
		dev_err(&client->dev, "Failed to create device file(%s)!\n",
				dev_attr_ir_send.attr.name);

	if (device_create_file(mc96fr116c_dev, &dev_attr_ir_send_result) < 0)
		dev_err(&client->dev, "Failed to create device file(%s)!\n",
				dev_attr_ir_send.attr.name);

	if (device_create_file(mc96fr116c_dev, &dev_attr_device_id) < 0)
		dev_err(&client->dev, "Failed to create device file(%s)!\n",
				dev_attr_device_id.attr.name);

	if (device_create_file(mc96fr116c_dev,
				&dev_attr_write_checksum_command) < 0)
		dev_err(&client->dev, "Failed to create device file(%s)!\n",
				dev_attr_write_checksum_command.attr.name);

	return 0;

err_remove_clientdata:
	i2c_set_clientdata(client, NULL);
	mutex_destroy(&data->signal_mutex);
err_free_mem:
	kfree(data);
	return ret;
}

#if defined(CONFIG_PM)
static int mc96fr116c_suspend(struct device *dev)
{
	struct i2c_client *client = to_i2c_client(dev);
	struct mc96fr116c_data *data = i2c_get_clientdata(client);

	data->pdata->ir_irled_onoff(0);
	data->on_off = 0;

	return 0;
}

static int mc96fr116c_resume(struct device *dev)
{
	return 0;
}

static const struct dev_pm_ops mc96fr116c_pm_ops = {
	.suspend	= mc96fr116c_suspend,
	.resume		= mc96fr116c_resume,
};
#endif

static int __devexit mc96fr116c_remove(struct i2c_client *client)
{
	struct mc96fr116c_data *data = i2c_get_clientdata(client);

	mutex_destroy(&data->signal_mutex);
	i2c_set_clientdata(client, NULL);
	kfree(data);
	return 0;
}

static const struct i2c_device_id mc96fr116c_id[] = {
	{ "mc96fr116c", 0 },
	{ }
};
MODULE_DEVICE_TABLE(i2c, mc96fr116c_id);

static struct i2c_driver mc96_i2c_driver = {
	.driver = {
		.name = "mc96fr116c",
#if defined(CONFIG_PM)
		.pm	= &mc96fr116c_pm_ops,
#endif
	},
	.probe = mc96fr116c_probe,
	.remove = __devexit_p(mc96fr116c_remove),

	.id_table = mc96fr116c_id,
};

static int __init mc96fr116c_init(void)
{
	return i2c_add_driver(&mc96_i2c_driver);
}
module_init(mc96fr116c_init);

static void __exit mc96fr116c_exit(void)
{
	i2c_del_driver(&mc96_i2c_driver);
}
module_exit(mc96fr116c_exit);

MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("MC96FR116C IR remote controller for Samsung mobile");

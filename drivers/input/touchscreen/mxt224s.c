/*
 *  Copyright (C) 2013, Samsung Electronics Co. Ltd. All Rights Reserved.
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 */

#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/input.h>
#include <linux/input/mt.h>
#include <linux/interrupt.h>
#include <linux/i2c.h>
#include <linux/delay.h>
#include <linux/slab.h>
#include <linux/gpio.h>
#include <linux/i2c/mxt224s.h>
#include <asm/unaligned.h>
#include <linux/firmware.h>
#include <linux/string.h>
#ifdef CONFIG_HAS_EARLYSUSPEND
#include <linux/earlysuspend.h>
#endif

/* #define CONFIG_READ_FROM_FILE */
#ifdef CONFIG_READ_FROM_FILE
#define CONFIG_READ_FROM_SDCARD
#define MXT_CFG_MAGIC		"OBP_RAW V1"
#define MXT_BATT_CFG_NAME	"/sdcard/mxt224s_batt_config.raw"
#define MXT_TA_CFG_NAME		"/sdcard/mxt224s_ta_config.raw"

struct mxt_info {
	u8 family_id;
	u8 variant_id;
	u8 version;
	u8 build;
	u8 matrix_xsize;
	u8 matrix_ysize;
	u8 object_num;
};
#endif


#define OBJECT_TABLE_START_ADDRESS	7
#define OBJECT_TABLE_ELEMENT_SIZE	6

#define CMD_RESET_OFFSET		0
#define CMD_BACKUP_OFFSET		1
#define CMD_CALIBRATE_OFFSET    2
#define CMD_REPORTATLL_OFFSET   3
#define CMD_DEBUG_CTRL_OFFSET   4
#define CMD_DIAGNOSTIC_OFFSET   5

#define DETECT_MSG_MASK			0x80
#define PRESS_MSG_MASK			0x40
#define RELEASE_MSG_MASK		0x20
#define MOVE_MSG_MASK			0x10
#define VECTOR_MSG_MASK			0x08
#define AMPLITUDE_MSG_MASK		0x04
#define SUPPRESS_MSG_MASK		0x02

/* Slave addresses */
#define MXT_APP_LOW			0x4a
#define MXT_APP_HIGH		0x4b
#define MXT_BOOT_LOW		0x24
#define MXT_BOOT_HIGH		0x25

/* FIRMWARE NAME */
#define MXT_FW_NAME			"tsp_atmel/mxt224s.fw"
#define MXT_BOOT_VALUE		0xa5
#define MXT_BACKUP_VALUE	0x55

#if CHECK_ANTITOUCH
#define MXT_T61_TIMER_ONESHOT			0
#define MXT_T61_TIMER_REPEAT			1
#define MXT_T61_TIMER_CMD_START		1
#define MXT_T61_TIMER_CMD_STOP		2
#endif

/* Bootloader mode status */
#define MXT_WAITING_BOOTLOAD_CMD	0xc0	/* valid 7 6 bit only */
#define MXT_WAITING_FRAME_DATA		0x80	/* valid 7 6 bit only */
#define MXT_FRAME_CRC_CHECK		0x02
#define MXT_FRAME_CRC_FAIL		0x03
#define MXT_FRAME_CRC_PASS		0x04
#define MXT_APP_CRC_FAIL		0x40	/* valid 7 8 bit only */
#define MXT_BOOT_STATUS_MASK	0x3f

/* Command to unlock bootloader */
#define MXT_UNLOCK_CMD_MSB		0xaa
#define MXT_UNLOCK_CMD_LSB		0xdc

#define ID_BLOCK_SIZE			7

#define MXT_STATE_INACTIVE		-1
#define MXT_STATE_RELEASE		0
#define MXT_STATE_PRESS			1
#define MXT_STATE_MOVE			2

#define MAX_USING_FINGER_NUM 	10

#define MXT_SW_RESET_TIME		300

#define MXT_PAGE_UP			0x01
#define MXT_PAGE_DOWN		0x02
#define MXT_DELTA_MODE		0x10
#define MXT_REFERENCE_MODE	0x11
#define MXT_CTE_MODE		0x31

/* Feature */
/*#######################################*/
#define SYSFS	1
#define FOR_BRINGUP  1
#define UPDATE_ON_PROBE   1
#define READ_FW_FROM_HEADER	1
#define FOR_DEBUGGING_TEST_DOWNLOADFW_BIN 0
#define ITDEV	0
#define SHOW_COORDINATE	1
#define DEBUG_INFO	0
#define TREAT_ERR				0
#define DOWNLOAD_CONFIG				0
/*#######################################*/

/* touch booster */

/* Firmware */
#if READ_FW_FROM_HEADER
static u8 firmware_mXT[] = {
/*	#include "mxt224s_V1.1.AA_.h"*/
};
#endif

#if ITDEV
static int driver_paused;
static int debug_enabled;
#endif

/* add for protection code */
/*#######################################*/
/*#######################################*/

/* variable related protection code */
/*#######################################*/
static int treat_median_error_status;
/*#######################################*/

struct object_t {
	u8 object_type;
	u16 i2c_address;
	u8 size;
	u8 instances;
	u8 num_report_ids;
} __packed;

struct finger_info {
	s16 x;
	s16 y;
	s16 z;
	u16 w;
	s8 state;
	int16_t component;
	u16 mcount;	/*add for debug*/
};

struct report_id_map_t {
	u8 object_type;     /*!< Object type. */
	u8 instance;        /*!< Instance number. */
};

struct tsp_callbacks {
	void (*inform_charger)(struct tsp_callbacks *tsp_cb, bool mode);
};

struct mxt_data {
	struct i2c_client *client;
	struct input_dev *input_dev;
	struct mxt224s_platform_data *pdata;
	struct finger_info fingers[MXT224S_MAX_MT_FINGERS];
	struct mutex i2c_lock;
#ifdef CONFIG_HAS_EARLYSUSPEND
	struct early_suspend early_suspend;
#endif
	struct object_t *objects;
	u8 family_id;
	u32 finger_mask;
	int gpio_read_done;
	bool object_alloc;
	u8 objects_len;
	u8 tsp_version;
	u8 tsp_build;
	u8 tsp_variant;
	u8 finger_type;
	u16 msg_proc;
	u16 cmd_proc;
	u16 msg_object_size;
	u32 x_dropbits:2;
	u32 y_dropbits:2;
	u8 tchthr_batt;
	u8 tchthr_charging;
	u8 calcfg_batt;
	u8 calcfg_charging;
	u8 disable_config_write;
	u16 distance[10];
	int (*power_on)(void);
	int (*power_off)(void);
	void (*register_cb)(void *);
	void (*read_ta_status)(void *);
	struct tsp_callbacks callbacks;
#if ITDEV
	u16 last_read_addr;
	u16 msg_proc_addr;
#endif
#ifdef CONFIG_READ_FROM_FILE
	struct mxt_info info;
#endif
	u8 max_report_id;
	struct report_id_map_t *rid_map;
	bool rid_map_alloc;
	unsigned int  touch_area_cnt;
	int tcount[10];
	char phys[32];
	bool mxt_enabled;
#ifdef SUPPORT_SHAPE_TOUCH
	int16_t sumsize;
#endif
};

static struct mxt_data *copy_data;
#if 0
int touch_is_pressed;
EXPORT_SYMBOL(touch_is_pressed);
#endif

static bool g_debug_switch;
static u8 threshold;
static int firm_status_data;

static u16 pre_x[][4] = {{0}, };
static u16 pre_y[][4] = {{0}, };

static u8 firmware_latest[] = {0x11, 0xaa};	/* version, build_version */

#if DEBUG_INFO
static u8	*object_type_name[63] = {
	[5]	= "GEN_MESSAGEPROCESSOR_T5",
	[6]	= "GEN_COMMANDPROCESSOR_T6",
	[7]	= "GEN_POWERCONFIG_T7",
	[8]	= "GEN_ACQUIRECONFIG_T8",
	[9]	= "TOUCH_MULTITOUCHSCREEN_T9",
	[15]	= "TOUCH_KEYARRAY_T15",
	[18]	= "SPT_COMCONFIG_T18",
	[23]	= "TOUCH_PROXIMITY_T23",
	[25]	= "SPT_SELFTEST_T25",
	[37]	= "DEBUG_DIAGNOSTICS_T37",
	[38]	= "USER_DATA_T38",
	[40]	= "PROCI_GRIPSUPPRESSION_T40",
	[42]	= "PROCI_TOUCHSUPPRESSION_T42",
	[44]	= "MESSAGECOUNT_T44",
	[46]	= "SPT_CTECONFIG_T46",
	[47]	= "PROCI_STYLUS_T47",
	[55]	= "PROCI_ADAPTIVETHRESHOLD_T55",
	[56]	= "PROCI_SHIELDLESS_T56",
	[57]	= "SPT_GENERICDATA_T57",
	[61]	= "SPT_TIMER_T61",
	[62]	= "PROCG_NOISESUPPRESSION_T62",
};
#endif

extern struct class *sec_class;
static int mxt_make_object_table(struct mxt_data *data);
#ifdef CONFIG_HAS_EARLYSUSPEND
static void mxt_early_suspend(struct early_suspend *h);
static void mxt_late_resume(struct early_suspend *h);
#endif
#ifdef CONFIG_INPUT_ENABLED
static int mxt224s_input_open(struct input_dev *dev);
static void mxt224s_input_close(struct input_dev *dev);
#endif


#ifdef CONFIG_READ_FROM_FILE
int mxt_download_config(struct mxt_data *data, const char *fn);
#endif

/* declare function proto type */
/*
static void mxt_ta_probe(int ta_status);
static void report_input_data(struct mxt_data *data);
*/
static int read_mem(struct mxt_data *data, u16 reg, u8 len, u8 *buf)
{
	struct i2c_client *client = data->client;
	struct i2c_msg msg[2];
	int ret;
	int retry = 3;
	u16 le_reg = cpu_to_le16(reg);
#if 0
	struct i2c_msg msg[2] = {
		{
			.addr = data->client->addr,
			.flags = 0,
			.len = 2,
			.buf = (u8 *)&le_reg,
		},
		{
			.addr = data->client->addr,
			.flags = I2C_M_RD,
			.len = len,
			.buf = buf,
		},
	};
#else
	mutex_lock(&data->i2c_lock);
	msg[0].addr = client->addr;
	msg[0].flags = 0;
	msg[0].len = 2;
	msg[0].buf = (u8 *)&le_reg;
	msg[1].addr = client->addr;
	msg[1].flags = I2C_M_RD;
	msg[1].len = len;
	msg[1].buf = buf;
#endif

	while (retry--) {
		ret = i2c_transfer(data->client->adapter, msg, 2);
		if (ret >= 0)
			break;

		dev_err(&client->dev, "%s fail(%d, %d)\n", __func__, ret, retry);
		msleep(1);
	}
	mutex_unlock(&data->i2c_lock);
	
	if (ret == 2)
		return 0;

	return -EIO;
}

static int write_mem(struct mxt_data *data, u16 reg, u8 len, const u8 *buf)
{
	struct i2c_client *client = data->client;
	int ret;
	int retry = 3;
	u8 tmp[len + 2];

	mutex_lock(&data->i2c_lock);
	put_unaligned_le16(cpu_to_le16(reg), tmp);
	memcpy(tmp + 2, buf, len);

	while (retry--) {
		ret = i2c_master_send(data->client, tmp, sizeof(tmp));
		if (ret >= 0)
			break;

		dev_err(&client->dev, "%s fail(%d, %d)\n", __func__, ret, retry);
		msleep(1);
	}
	mutex_unlock(&data->i2c_lock);

	if (ret == sizeof(tmp))
		return 0;

	return -EIO;
}

static int __devinit mxt_reset(struct mxt_data *data)
{
	struct i2c_client *client = data->client;
	int ret;
	u8 buf = 1u;

	ret = write_mem(data, data->cmd_proc + CMD_RESET_OFFSET, 1, &buf);
	if (ret)
		dev_err(&client->dev,"%s fail\n", __func__);

	return ret;
}

static int __devinit mxt_backup(struct mxt_data *data)
{
	struct i2c_client *client = data->client;
	int ret;
	u8 buf = 0x55u;

	ret = write_mem(data, data->cmd_proc + CMD_BACKUP_OFFSET, 1, &buf);
	if (ret)
		dev_err(&client->dev,"%s fail\n", __func__);

	return ret;
}

static int get_object_info(struct mxt_data *data, u8 object_type, u16 *size,
				u16 *address)
{
	int i;

	for (i = 0; i < data->objects_len; i++) {
		if (data->objects[i].object_type == object_type) {
			*size = data->objects[i].size + 1;
			*address = data->objects[i].i2c_address;
			return 0;
		}
	}

	return -ENODEV;
}

#if CHECK_ANTITOUCH
void mxt_t61_timer_set(struct mxt_data *data, u8 mode, u8 cmd, u16 msPeriod)
{
	struct i2c_client *client = data->client;
	int ret = 0;
	u16 size = 0;
	u16 obj_address = 0;
	u8 buf[5] = {3, 0, 0, 0, 0};

	get_object_info(data, SPT_TIMER_T61, &size, &obj_address);

	buf[1] = cmd;
	buf[2] = mode;
	buf[3] = msPeriod & 0xFF;
	buf[4] = (msPeriod >> 8) & 0xFF;

	ret = write_mem(data, obj_address, 5, buf);
	if (ret)
		dev_err(&client->dev,
			"%s write error T%d address[0x%x]\n",
			__func__, SPT_TIMER_T61, obj_address);

	dev_info(&client->dev, "T61 Timer Enabled %d\n", msPeriod);
}

void mxt_t8_cal_set(struct mxt_data *data, u8 mstime)
{
	struct i2c_client *client = data->client;
	int ret = 0;
	u16 size = 0;
	u16 obj_address = 0;

	if (mstime) {
		data->pdata->check_autocal = 1;
		dev_info(&client->dev, "T8 Autocal Enabled %d\n", mstime);
	} else {
		data->pdata->check_autocal = 0;
		dev_info(&client->dev, "T8 Autocal Disabled %d\n", mstime);
	}

	get_object_info(data, GEN_ACQUISITIONCONFIG_T8, &size, &obj_address);

	ret = write_mem(data, obj_address + 4, 1, &mstime);
	if (ret)
		dev_err(&client->dev, "%s write error T%d address[0x%x]\n",
			__func__, SPT_TIMER_T61, obj_address);
}

/*
static void mxt_check_coordinate(struct mxt_data *data,bool detect, u8 id,u16 *px, u16 *py)
{
	if (detect) {
		data->tcount[id] = 0;
		data->distance[id] = 0;
	}
	if (data->tcount[id] >= 1) {
		data->distance[id] = abs(pre_x[id][0] - *px) + abs(pre_y[id][0] - *py);
		pr_info("[TSP]Check Distance ID:%d, %d\n",id, data->distance[id]);
	}
	pre_x[id][0] = *px;
	pre_y[id][0] = *py;

	data->tcount[id]++;
}
*/
#endif	/* CHECK_ANTITOUCH */

static int write_config(struct mxt_data *data, u8 type, const u8 *cfg)
{
	int ret;
	u16 address = 0;
	u16 size = 0;

	ret = get_object_info(data, type, &size, &address);

	if (size == 0 && address == 0)
		return 0;
	else
		return write_mem(data, address, size, cfg);
}

static int check_instance(struct mxt_data *data, u8 object_type)
{
	int i;

	for (i = 0; i < data->objects_len; i++) {
		if (data->objects[i].object_type == object_type)
			return data->objects[i].instances;
	}
	return 0;
}

static int init_write_config(struct mxt_data *data, u8 type, const u8 *cfg)
{
	struct i2c_client *client = data->client;
	int ret;
	u16 address = 0;
	u16 size = 0;
	u8 *temp;

	ret = get_object_info(data, type, &size, &address);

	if ((size == 0) || (address == 0)) {
		dev_err(&client->dev, "%s error object_type(%d)\n", __func__, type);
		return -ENODEV;
	}

	ret = write_mem(data, address, size, cfg);
	if (check_instance(data, type)) {
#if DEBUG_INFO
		dev_info(&client->dev, "exist instance1 object (%d)\n", type);
#endif
		temp = kmalloc(size * sizeof(u8), GFP_KERNEL);
		memset(temp, 0, size);
		ret |= write_mem(data, address+size, size, temp);
		kfree(temp);
	}

	return ret;
}

#if TREAT_ERR
static int change_config(struct mxt_data *data,
			u16 reg, u8 offeset, u8 change_value)
{
	u8 value = 0;

	value = change_value;
	return write_mem(data, reg+offeset, 1, &value);
}
#endif

static u32 __devinit crc24(u32 crc, u8 byte1, u8 byte2)
{
	static const u32 crcpoly = 0x80001B;
	u32 res;
	u16 data_word;

	data_word = (((u16)byte2) << 8) | byte1;
	res = (crc << 1) ^ (u32)data_word;

	if (res & 0x1000000)
		res ^= crcpoly;

	return res;
}

static int __devinit calculate_infoblock_crc(struct mxt_data *data,
							u32 *crc_pointer)
{
	u32 crc = 0;
	u8 mem[7 + data->objects_len * 6];
	int status;
	int i;

	status = read_mem(data, 0, sizeof(mem), mem);

	if (status)
		return status;

	for (i = 0; i < sizeof(mem) - 1; i += 2)
		crc = crc24(crc, mem[i], mem[i + 1]);

	*crc_pointer = crc24(crc, mem[i], 0) & 0x00FFFFFF;

	return 0;
}

static uint8_t calibrate_chip(struct mxt_data *data)
{
	u8 cal_data = 1;
	int ret = 0;

	ret = write_mem(data, data->cmd_proc + CMD_CALIBRATE_OFFSET,
		1, &cal_data);
	if (!ret)
		dev_info(&data->client->dev, "calibration success!!!\n");
	return ret;
}

#if TREAT_ERR
static void treat_error_status(void)
{
	bool ta_status = 0;
	u16 size;
	u16 obj_address = 0;
	int error = 0;
	struct mxt_data *data = copy_data;

	data->read_ta_status(&ta_status);

	if (treat_median_error_status) {
		pr_err("Error status already treated\n");
		return;
	} else
		treat_median_error_status = 1;

	pr_info("Error status TA is[%d]\n", ta_status);

	if (ta_status) {
#if !(FOR_BRINGUP)
		get_object_info(data,
			GEN_POWERCONFIG_T7, &size, &obj_address);
		/* 1:ACTVACQINT */
		error = change_config(data, obj_address, 1, 255);

		get_object_info(data,
			GEN_ACQUISITIONCONFIG_T8, &size, &obj_address);
		/* 0:CHRGTIME */
		error |= change_config(data, obj_address, 0, 64);

		/* 8:ATCHFRCCALTHR*/
		error |= change_config(data, obj_address, 8, 50);
		/* 9:ATCHFRCCALRATIO*/
		error |= change_config(data, obj_address, 9, 0);

		get_object_info(data,
			PROCI_TOUCHSUPPRESSION_T42, &size, &obj_address);
		/* 0:CTRL */
		error |= change_config(data, obj_address, 0, 3);

		get_object_info(data,
			SPT_CTECONFIG_T46, &size, &obj_address);
		/* 2:IDLESYNCSPERX */
		error |= change_config(data, obj_address, 2, 48);
		/* 3:ACTVSYNCSPERX */
		error |= change_config(data, obj_address, 3, 48);

		get_object_info(data,
			PROCG_NOISESUPPRESSION_T48, &size, &obj_address);
		/* 2:CALCFG */
		error |= change_config(data, obj_address, 2, 114);
		/* 3:BASEFREQ */
		error |= change_config(data, obj_address, 3, 15);
		/* 8:MFFREQ[0] */
		error |= change_config(data, obj_address, 8, 3);
		/* 9:MFFREQ[1] */
		error |= change_config(data, obj_address, 9, 5);
		/* 10:NLGAIN*/
		error |= change_config(data, obj_address, 10, 96);
		/* 11:NLTHR*/
		error |= change_config(data, obj_address, 11, 30);
		/* 17:GCMAXADCSPERX */
		error |= change_config(data, obj_address, 17, 100);
		/* 34:BLEN[0] */
		error |= change_config(data, obj_address, 34, 80);
		/* 35:TCHTHR[0] */
		error |= change_config(data, obj_address, 35, 40);
		/* 36:TCHDI[0] */
		error |= change_config(data, obj_address, 36, 2);
		/* 39:MOVFILTER[0] */
		error |= change_config(data, obj_address, 39, 65);
		/* 41:MRGHYST[0] */
		error |= change_config(data, obj_address, 41, 40);
		/* 42:MRGTHR[0] */
		error |= change_config(data, obj_address, 42, 50);
		/* 43:XLOCLIP[0] */
		error |= change_config(data, obj_address, 43, 5);
		/* 44:XHICLIP[0] */
		error |= change_config(data, obj_address, 44, 5);
		/* 51:JUMPLIMIT[0] */
		error |= change_config(data, obj_address, 51, 25);
		/* 52:TCHHYST[0] */
		error |= change_config(data, obj_address, 52, 15);
#endif
		if (error < 0)
			pr_err("failed to write error status\n");
	} else {
#if !(FOR_BRINGUP)
		get_object_info(data,
			GEN_POWERCONFIG_T7, &size, &obj_address);
		/* 1:ACTVACQINT */
		error = change_config(data, obj_address, 1, 255);

		get_object_info(data,
			GEN_ACQUISITIONCONFIG_T8, &size, &obj_address);
		/* 0:CHRGTIME */
		error |= change_config(data, obj_address, 0, 64);

		/* 8:ATCHFRCCALTHR*/
		error |= change_config(data, obj_address, 8, 50);
		/* 9:ATCHFRCCALRATIO*/
		error |= change_config(data, obj_address, 9, 0);

		get_object_info(data,
			TOUCH_MULTITOUCHSCREEN_T9, &size, &obj_address);
		/* 31:TCHHYST */
		error |= change_config(data, obj_address, 31, 15);

		get_object_info(data,
			PROCI_TOUCHSUPPRESSION_T42, &size, &obj_address);
		/* 0:CTRL */
		error |= change_config(data, obj_address, 0, 3);

		get_object_info(data,
			SPT_CTECONFIG_T46, &size, &obj_address);
		/* 2:IDLESYNCSPERX */
		error |= change_config(data, obj_address, 2, 48);
		/* 3:ACTVSYNCSPERX */
		error |= change_config(data, obj_address, 3, 48);

		get_object_info(data,
			PROCG_NOISESUPPRESSION_T48, &size, &obj_address);
		/* 2:CALCFG */
		error |= change_config(data, obj_address, 2, 242);
		/* 3:BASEFREQ */
		error |= change_config(data, obj_address, 3, 15);
		/* 8:MFFREQ[0] */
		error |= change_config(data, obj_address, 8, 3);
		/* 9:MFFREQ[1] */
		error |= change_config(data, obj_address, 9, 5);
		/* 10:NLGAIN*/
		error |= change_config(data, obj_address, 10, 112);
		/* 11:NLTHR*/
		error |= change_config(data, obj_address, 11, 25);
		/* 17:GCMAXADCSPERX */
		error |= change_config(data, obj_address, 17, 100);
		/* 34:BLEN[0] */
		error |= change_config(data, obj_address, 34, 112);
		/* 35:TCHTHR[0] */
		error |= change_config(data, obj_address, 35, 40);
		/* 41:MRGHYST[0] */
		error |= change_config(data, obj_address, 41, 40);
		/* 42:MRGTHR[0] */
		error |= change_config(data, obj_address, 42, 50);
		/* 51:JUMPLIMIT[0] */
		error |= change_config(data, obj_address, 51, 25);
		/* 52:TCHHYST[0] */
		error |= change_config(data, obj_address, 52, 15);
#endif
		if (error < 0)
			pr_err("failed to write error status\n");
	}
}
#endif

static void mxt_ta_probe(struct mxt_data *data, int ta_status)
{
#ifdef CONFIG_READ_FROM_FILE
	if (ta_status)
		mxt_download_config(data, MXT_TA_CFG_NAME);
	else
		mxt_download_config(data, MXT_BATT_CFG_NAME);
#else	
	u16 obj_address = 0;
	u16 size;
	int error = 0;
	u8 value = 0;

	if (!data->mxt_enabled) {
		pr_err("%s mxt_enabled is 0\n", __func__);
		return;
	}

	if (ta_status) {
		write_config(data, data->pdata->t9_config_chrg[0],
			data->pdata->t9_config_chrg + 1);
		write_config(data, data->pdata->t56_config_chrg[0],
			data->pdata->t56_config_chrg + 1);
		write_config(data, data->pdata->t62_config_chrg[0],
			data->pdata->t62_config_chrg + 1);
	} else {
		write_config(data, data->pdata->t9_config_batt[0],
			data->pdata->t9_config_batt + 1);
		write_config(data, data->pdata->t56_config_batt[0],
			data->pdata->t56_config_batt + 1);
		write_config(data, data->pdata->t62_config_batt[0],
			data->pdata->t62_config_batt + 1);
	}
    get_object_info(data, PROCG_NOISESUPPRESSION_T62,
    	&size, &obj_address);

	/* Read CALCFG1 for Setting CHRGON */
	read_mem(data, obj_address + 1, 1, &value);

	value &= 0xFE;
	error = write_mem(data, obj_address + 1, 1, &value);

	value |= 0x01;
	error = write_mem(data, obj_address + 1, 1, &value);
#endif
	pr_info("%s : threshold[%d]\n", __func__, threshold);
};

static void mxt224s_ta_cb(struct tsp_callbacks *cb, bool ta_status)
{
	struct mxt_data *data =
			container_of(cb, struct mxt_data, callbacks);
	struct i2c_client *client = data->client;

	dev_notice(&client->dev, "%s,(%d)\n", __func__, ta_status);

	if (data->mxt_enabled)
		mxt_ta_probe(data, ta_status);
}

static uint8_t reportid_to_type(struct mxt_data *data,
		u8 report_id, u8 *instance)
{
	struct report_id_map_t *report_id_map;
	report_id_map = data->rid_map;

	if (report_id <= data->max_report_id) {
		*instance = report_id_map[report_id].instance;
		return report_id_map[report_id].object_type;
	} else
		return 0;
}

#if CHECK_ANTITOUCH
static u16 mxt_dist_check(struct mxt_data *data,	unsigned char touch_num)
{
	int count;
	u16 dist_sum = 0;

	for (count = 0; count < touch_num; count++) {
		/* if (touch_num==2)  check : data->distance[0],data->distance[1] */
		if (data->distance[count] < 4) {
			dist_sum++;
		} else {
			dist_sum = 0;
		}
	}
	pr_info("dis0 = %d, dis1 = %d, dis2 = %d, dis3 = %d, dis4 = %d, dis5 = %d\n",
		data->distance[0], data->distance[1], data->distance[2],
		data->distance[3], data->distance[4], data->distance[5]);
	return dist_sum;
}
#endif

static void report_input_data(struct mxt_data *data)
{
	struct device *dev = &data->client->dev;
	int i;
	int count = 0;
	int report_count = 0;

	for (i = 0; i < MXT224S_MAX_MT_FINGERS; i++) {
		if (data->fingers[i].state == MXT_STATE_INACTIVE)
			continue;

		if (data->fingers[i].state == MXT_STATE_RELEASE) {
			input_mt_slot(data->input_dev, i);
			input_mt_report_slot_state(data->input_dev,
					MT_TOOL_FINGER, false);
		} else {
			input_mt_slot(data->input_dev, i);
			input_mt_report_slot_state(data->input_dev,
					MT_TOOL_FINGER, true);
			input_report_abs(data->input_dev, ABS_MT_POSITION_X,
					data->fingers[i].x);
			input_report_abs(data->input_dev, ABS_MT_POSITION_Y,
					data->fingers[i].y);
			input_report_abs(data->input_dev, ABS_MT_TOUCH_MAJOR,
					data->fingers[i].z);
			input_report_abs(data->input_dev, ABS_MT_PRESSURE,
					 data->fingers[i].w);
#ifdef SUPPORT_SHAPE_TOUCH
			input_report_abs(data->input_dev, ABS_MT_COMPONENT,
				data->fingers[i].component);
			input_report_abs(data->input_dev, ABS_MT_SUMSIZE,
				 data->sumsize);
#endif
		}
		report_count++;

#if SHOW_COORDINATE
		switch (data->fingers[i].state) {
		case MXT_STATE_PRESS:
			dev_info(dev, "P: id[%d],x=%d,y=%d,w=%d, z=%d\n",
				i, data->fingers[i].x, data->fingers[i].y
				, data->fingers[i].w, data->fingers[i].z);
			break;
/*
		case MXT_STATE_MOVE:
			dev_info(dev, "M: id[%d],x=%d,y=%d,w=%d,mc=%d\n",
				i, data->fingers[i].x, data->fingers[i].y
				, data->fingers[i].w, data->fingers[i].mcount);
			break;
*/
		case MXT_STATE_RELEASE:
			dev_info(dev, "R: id[%d],mc=%d\n",
				i, data->fingers[i].mcount);
			break;
		default:
			break;
		}
#else
		if (data->fingers[i].state == MXT_STATE_PRESS)
			dev_info(dev, "P: id[%d],w=%d\n"
				, i, data->fingers[i].w);
		else if (data->fingers[i].state == MXT_STATE_RELEASE)
			dev_info(dev, "R: id[%d],mc=%d\n"
				, i, data->fingers[i].mcount);
#endif
		if (data->fingers[i].state == MXT_STATE_RELEASE) {
			data->fingers[i].state = MXT_STATE_INACTIVE;
			data->fingers[i].mcount = 0;
		} else {
			data->fingers[i].state = MXT_STATE_MOVE;
			count++;
		}
	}
	if (report_count > 0) {
#if ITDEV
		if (!driver_paused)
#endif
			input_sync(data->input_dev);
	}
#if 0
	if (count)
		touch_is_pressed = 1;
	else {
#if TSP_USE_SHAPETOUCH
		data->sumsize = 0;
#endif
		touch_is_pressed = 0;
	}
#endif
	data->finger_mask = 0;
}

static void mxt_release_all(struct mxt_data *data)
{
	struct i2c_client *client = data->client;
	int i;
	bool report = false;

	for (i = 0; i < MXT224S_MAX_MT_FINGERS; i++) {
		if (data->fingers[i].state == MXT_STATE_INACTIVE)
			continue;
		report = true;
		data->fingers[i].z = 0;
		data->fingers[i].state = MXT_STATE_RELEASE;
	}
	if (report)
		report_input_data(data);
};

static void mxt_hw_reset(struct mxt_data *data)
{
	struct i2c_client *client = data->client;

	dev_info(&client->dev, "%s excute\n", __func__);
	data->power_off();
	msleep(50);
	data->power_on();
}

static void mxt_treat_T6_object(struct mxt_data *data, u8 *msg)
{
	struct device *dev = &data->client->dev;

	if (msg[1] == 0x00) /* normal mode */
		dev_info(dev, "normal mode\n");
	if (msg[1] & 0x04) /* I2C checksum error */
		dev_err(dev, "I2C checksum error\n");
	if (msg[1] & 0x08) /* config error */
		dev_err(dev, "config error\n");
	if (msg[1] & 0x10) { /* calibration */
		dev_info(dev, "calibration is on going\n");
#if CHECK_ANTITOUCH
		/* After Calibration */
		mxt_t8_cal_set(data, 0);
		data->pdata->check_antitouch = 1;
		mxt_t61_timer_set(data,
			MXT_T61_TIMER_ONESHOT,
			MXT_T61_TIMER_CMD_STOP, 0);
		data->pdata->check_timer = 0;
		data->pdata->check_calgood = 0;
#endif
	}
	if (msg[1] & 0x20) /* signal error */
		dev_err(dev, "signal error\n");
	if (msg[1] & 0x40) /* overflow */
		dev_err(dev, "overflow detected\n");
	if (msg[1] & 0x80) /* reset */
		dev_info(dev, "reset is ongoing\n");
}

static void mxt_treat_T9_object(struct mxt_data *data, u8 *msg)
{
	struct device *dev = &data->client->dev;
	int id = msg[0] - data->finger_type;

	/* not touch event */
	if (id < 0 || id >= MXT224S_MAX_MT_FINGERS)
		return;
#if 0
	if (data->finger_mask & (1U << id))
		report_input_data(data);
#endif
	if (msg[1] & RELEASE_MSG_MASK) {
		data->fingers[id].z = 0;
		data->fingers[id].w = msg[5];
		data->finger_mask |= 1U << id;
		data->fingers[id].state = MXT_STATE_RELEASE;
	} else if ((msg[1] & DETECT_MSG_MASK) &&
		(msg[1] & (PRESS_MSG_MASK | MOVE_MSG_MASK | VECTOR_MSG_MASK))) {
		data->fingers[id].z = msg[6];
		data->fingers[id].w = msg[5];
		if (data->fingers[id].w == 0)
			data->fingers[id].w = 1;

		data->fingers[id].x = (((msg[2] << 4) | (msg[4] >> 4))
			>> data->x_dropbits);
		data->fingers[id].y = (((msg[3] << 4) | (msg[4] & 0xF))
			>> data->y_dropbits);

#ifdef SUPPORT_SHAPE_TOUCH
		data->fingers[id].component = msg[7];
#endif

		data->finger_mask |= 1U << id;

		if (msg[1] & PRESS_MSG_MASK) {
			data->fingers[id].state = MXT_STATE_PRESS;
			data->fingers[id].mcount = 0;
#if CHECK_ANTITOUCH
			/* mxt_check_coordinate(data, 1, id,
				&data->fingers[id].x, &data->fingers[id].y);*/
#endif
		} else if (msg[1] & MOVE_MSG_MASK) {
			data->fingers[id].mcount += 1;
#if CHECK_ANTITOUCH
			/* mxt_check_coordinate(data, 0, id,
				&data->fingers[id].x, &data->fingers[id].y);*/
#endif
		}
	} else if ((msg[1] & SUPPRESS_MSG_MASK)
		&& (data->fingers[id].state != MXT_STATE_INACTIVE)) {
		data->fingers[id].z = 0;
		data->fingers[id].w = msg[5];
		data->fingers[id].state = MXT_STATE_RELEASE;
		data->finger_mask |= 1U << id;
	} else {
	/* ignore changed amplitude message */
		if (!((msg[1] & DETECT_MSG_MASK) && (msg[1] & AMPLITUDE_MSG_MASK)))
			dev_err(dev, "Unknown state %#02x %#02x\n",
				msg[0], msg[1]);;
	}
	if (data->finger_mask)
		report_input_data(data);
}

static void mxt_treat_T42_object(struct mxt_data *data, u8 *msg)
{
	struct device *dev = &data->client->dev;

	if (msg[1] & 0x01) {
		/* Palm Press */
		dev_info(dev, "palm touch detected\n");
/*		touch_is_pressed = 0;*/

	} else {
		/* Palm release */
		dev_info(dev, "palm touch released\n");
/*		touch_is_pressed = 1;*/
	}
}

static void mxt_treat_T57_object(struct mxt_data *data, u8 *msg)
{
	struct device *dev = &data->client->dev;
#if CHECK_ANTITOUCH
	u16 tch_area = 0;
	u16 atch_area = 0;

	tch_area = msg[3] | (msg[4] << 8);
	atch_area = msg[5] | (msg[6] << 8);

	if (data->pdata->check_antitouch) {
		if (tch_area) {
#if DEBUG_INFO
			dev_info(dev, "TCHAREA=%d\n", tch_area);
#endif
			/* First Touch After Calibration */
			if (data->pdata->check_timer == 0) {
				mxt_t61_timer_set(data, MXT_T61_TIMER_ONESHOT,
					MXT_T61_TIMER_CMD_START, 3000);
				data->pdata->check_timer = 1;
			}

			if (tch_area <= 2)
				mxt_t8_cal_set(data, 5);
		}

		if (atch_area >= 3) {
#if DEBUG_INFO
			dev_info(dev, "ATCHAREA=%d\n", atch_area);
#endif
			calibrate_chip(data);
		}
	}

	if (data->pdata->check_calgood == 1) {
		if ((atch_area - tch_area) > 8) {
			if (tch_area < 35) {
#if DEBUG_INFO
				dev_info(dev, "Cal Not Good - %d %d\n",
					atch_area, tch_area);
#endif
				calibrate_chip(data);
			}
		}
		if ((tch_area - atch_area) >= 40) {
#if DEBUG_INFO
			dev_info(dev, "Cal Not Good 2 - %d %d\n",
				atch_area, tch_area);
#endif
			calibrate_chip(data);
		}
	}
#endif	/* CHECK_ANTITOUCH */

#ifdef SUPPORT_SHAPE_TOUCH
	data->sumsize = msg[1] + (msg[2] << 8);
#endif
}

static void mxt_treat_T61_object(struct mxt_data *data, u8 *msg)
{
	struct device *dev = &data->client->dev;
#if CHECK_ANTITOUCH
	if ((msg[1] & 0xa0) == 0xa0) {
		if (data->pdata->check_calgood == 1)
			data->pdata->check_calgood = 0;
	
		if (data->pdata->check_antitouch) {
#if DEBUG_INFO
			dev_info(dev, "SPT_TIMER_T61 Stop\n");
#endif
			data->pdata->check_antitouch = 0;
			data->pdata->check_timer = 0;
			mxt_t8_cal_set(data, 0);
			data->pdata->check_calgood = 1;
			mxt_t61_timer_set(data, MXT_T61_TIMER_ONESHOT,
				MXT_T61_TIMER_CMD_START, 10000);
		}
	}
#endif
}

static irqreturn_t mxt_irq_thread(int irq, void *ptr)
{
	struct mxt_data *data = ptr;
	struct i2c_client *client = data->client;
	u8 msg[data->msg_object_size];
	u8 object_type, instance;
	bool ta_status;

#if DEBUG_INFO
	dev_info(&client->dev, "%s\n", __func__);
#endif
	do {
		if (read_mem(data, data->msg_proc, sizeof(msg), msg)) {
			dev_err(&client->dev, "%s message read fail\n", __func__);

			mxt_release_all(data);
			mxt_hw_reset(data);
			if (data->read_ta_status) {
				data->read_ta_status(&ta_status);
				dev_err(&client->dev,"ta_status is %d\n", ta_status);
				mxt_ta_probe(data, ta_status);
			}
			return IRQ_HANDLED;
		}
#if ITDEV
		if (debug_enabled)
			print_hex_dump(KERN_INFO, "MXT MSG:",
			DUMP_PREFIX_NONE, 16, 1, msg, sizeof(msg), false);
#endif
		object_type = reportid_to_type(data, msg[0] , &instance);

		switch (object_type) {
		case GEN_COMMANDPROCESSOR_T6:
			mxt_treat_T6_object(data, msg);
			break;

		case TOUCH_MULTITOUCHSCREEN_T9:
			mxt_treat_T9_object(data, msg);
			break;

		case PROCI_TOUCHSUPPRESSION_T42:
			mxt_treat_T42_object(data, msg);
			break;

		case PROCI_EXTRATOUCHSCREENDATA_T57:
			mxt_treat_T57_object(data, msg);
			break;

		case SPT_TIMER_T61:
			mxt_treat_T61_object(data, msg);
			break;

		default:
			dev_err(&client->dev, "untreat object type[%d]\n", object_type);
			dev_err(&client->dev, "[0x%x 0x%x 0x%x 0x%x 0x%x 0x%x 0x%x]\n",
				msg[0], msg[1], msg[2], msg[3], msg[4], msg[5], msg[6]);
			break;
		}
	} while (!gpio_get_value(data->gpio_read_done));
#if 0
	if (data->finger_mask)
		report_input_data(data);
#endif

	return IRQ_HANDLED;
}

#if SYSFS
static ssize_t mxt_debug_setting(struct device *dev,
					struct device_attribute *attr,
					const char *buf, size_t count)
{
	g_debug_switch = !g_debug_switch;
	return 0;
}

static ssize_t mxt_object_setting(struct device *dev,
					struct device_attribute *attr,
					const char *buf, size_t count)
{
	struct mxt_data *data = dev_get_drvdata(dev);
	unsigned int object_type;
	unsigned int object_register;
	unsigned int register_value;
	u8 value;
	u8 val;
	int ret;
	u16 address;
	u16 size;
	sscanf(buf, "%u%u%u", &object_type, &object_register, &register_value);
	pr_info("object type T%d", object_type);
	pr_info("object register ->Byte%d\n", object_register);
	pr_info("register value %d\n", register_value);
	ret = get_object_info(data, (u8)object_type, &size, &address);
	if (ret) {
		pr_err("fail to get object_info\n");
		return count;
	}

	size = 1;
	value = (u8)register_value;
	write_mem(data, address+(u16)object_register, size, &value);
	read_mem(data, address+(u16)object_register, (u8)size, &val);

	pr_info("T%d Byte%d is %d\n",
		object_type, object_register, val);
	return count;
}

static ssize_t mxt_object_show(struct device *dev,
					struct device_attribute *attr,
					const char *buf, size_t count)
{
	struct mxt_data *data = dev_get_drvdata(dev);
	unsigned int object_type;
	u8 val;
	int ret;
	u16 address;
	u16 size;
	u16 i;
	sscanf(buf, "%u", &object_type);
	pr_info("object type T%d\n", object_type);
	ret = get_object_info(data, (u8)object_type, &size, &address);
	if (ret) {
		pr_err("fail to get object_info\n");
		return count;
	}
	for (i = 0; i < size; i++) {
		read_mem(data, address+i, 1, &val);
		pr_info("Byte %u --> %u\n", i, val);
	}
	return count;
}

static struct device *sec_touchscreen;
static struct device *mxt_noise_test;
/*
	top_left, top_right, center, bottom_left, bottom_right
*/
static unsigned char test_node[5] = {15, 23, 145, 239, 247};
uint16_t mxt_delta_node[384] = { 0 };
uint16_t mxt_refer_node[384] = { 0 };
static int index_reference;
static int index_delta;

static void diagnostic_chip(u8 mode)
{
	int error;
	u16 t6_address = 0;
	u16 size_one;
	int ret;
	u8 value;
	u16 t37_address = 0;

	ret = get_object_info(copy_data,
		GEN_COMMANDPROCESSOR_T6, &size_one, &t6_address);

	size_one = 1;
	error = write_mem(copy_data, t6_address+5, (u8)size_one, &mode);
	/* QT602240_COMMAND_DIAGNOSTIC, mode); */
	if (error < 0) {
		pr_err("error %s: write_object\n", __func__);
	} else {
		get_object_info(copy_data,
			DEBUG_DIAGNOSTIC_T37, &size_one, &t37_address);
		size_one = 1;
		/* pr_info("diagnostic_chip setting success\n"); */
		read_mem(copy_data, t37_address, (u8)size_one, &value);
		/* pr_info("dianostic_chip mode is %d\n",value); */
	}
}

static uint8_t read_uint16_t(struct mxt_data *data, uint16_t address, uint16_t *buf)
{
	uint8_t status;
	uint8_t temp[2];

	status = read_mem(data, address, 2, temp);
	*buf = ((uint16_t)temp[1]<<8) + (uint16_t)temp[0];

	return status;
}

static void read_dbg_data(uint8_t dbg_mode , uint16_t node, uint16_t *dbg_data)
{
	u8 read_page, read_point;
	uint8_t mode, page;
	u16 size;
	u16 diagnostic_addr = 0;

	if (!copy_data->mxt_enabled) {
		pr_err("read_dbg_data. mxt_enabled is 0\n");
		return;
	}

	get_object_info(copy_data,
		DEBUG_DIAGNOSTIC_T37, &size, &diagnostic_addr);

	read_page = node / 64;
	node %= 64;
	read_point = (node * 2) + 2;

	/* Page Num Clear */
	diagnostic_chip(MXT_CTE_MODE);
	msleep(20);

	do {
		if (read_mem(copy_data, diagnostic_addr, 1, &mode)) {
			pr_info("READ_MEM_FAILED\n");
			return;
		}
	} while (mode != MXT_CTE_MODE);

	diagnostic_chip(dbg_mode);
	msleep(20);

	do {
		if (read_mem(copy_data, diagnostic_addr, 1, &mode)) {
			pr_info("READ_MEM_FAILED\n");
			return;
		}
	} while (mode != dbg_mode);

	for (page = 1; page <= read_page; page++) {
		diagnostic_chip(MXT_PAGE_UP);
		msleep(20);
		do {
			if (read_mem(copy_data,
				diagnostic_addr + 1, 1, &mode)) {
				pr_info("READ_MEM_FAILED\n");
				return;
			}
		} while (mode != page);
	}

	if (read_uint16_t(copy_data, diagnostic_addr + read_point, dbg_data)) {
		pr_info("READ_MEM_FAILED\n");
		return;
	}
}

#define MIN_VALUE 19744
#define MAX_VALUE 28884
static u16 max_ref, min_ref;

static int read_all_data(uint16_t dbg_mode)
{
	u8 read_page, read_point;
	u16 max_value = MIN_VALUE, min_value = MAX_VALUE;
	u16 object_address = 0;
	u8 data_buffer[2] = { 0 };
	u8 node = 0;
	int state = 0;
	int num = 0;
	int ret;
	u16 size;
	u8 val;
	bool dual_x_mode = 0;

	ret = get_object_info(copy_data, PROCG_NOISESUPPRESSION_T62, &size, &object_address);
	read_mem(copy_data, object_address + 3, 1, &val);
	if (val & 0x01)
		dual_x_mode = 1;

	/* Page Num Clear */
	diagnostic_chip(MXT_CTE_MODE);
	msleep(30);/* msleep(20);  */

	diagnostic_chip(dbg_mode);
	msleep(30);/* msleep(20);  */

	ret = get_object_info(copy_data,
		DEBUG_DIAGNOSTIC_T37, &size, &object_address);
	/*jerry no need to leave it */
	msleep(50); /* msleep(20);  */

	for (read_page = 0 ; read_page < 4; read_page++) {
		for (node = 0; node < 64; node++) {
			read_point = (node * 2) + 2;
			read_mem(copy_data, object_address + (u16)read_point, 2, data_buffer);
			mxt_refer_node[num] = ((u16)data_buffer[1]<<8)	+ (u16)data_buffer[0];

			if ((mxt_refer_node[num] > MIN_VALUE) || (mxt_refer_node[num] < MAX_VALUE)) {
				state = 1;
				printk(KERN_ERR
					"[TSP] mxt_refer_node[%3d] = %5d\n",
					num, mxt_refer_node[num]);
				}

				if (data_buffer[0] != 0) {
				if (mxt_refer_node[num] != 0) {
					if (mxt_refer_node[num] > max_value)
						max_value = mxt_refer_node[num];
					if (mxt_refer_node[num] < min_value)
						min_value = mxt_refer_node[num];
				}
			}
			num++;
			/* all node => 19 * 11 = 209 => (3page * 64) + 17 */
			if ((read_page == 3) && (node == 16))
				break;
		}
		diagnostic_chip(MXT_PAGE_UP);
		msleep(20);
	}

	if ((max_value - min_value) > 4000) {
		printk(KERN_ERR
			"[TSP] diff = %d, max_value = %d, min_value = %d\n",
			(max_value - min_value), max_value, min_value);
		state = 1;
	}

	max_ref = max_value;
	min_ref = min_value;

	return state;
}

static int read_all_delta_data(uint16_t dbg_mode)
{
	u8 read_page, read_point;
	u16 object_address = 0;
	u8 data_buffer[2] = { 0 };
	u8 node = 0;
	int state = 0;
	int num = 0;
	int ret;
	u16 size;

	if (!copy_data->mxt_enabled) {
		pr_err("%s : mxt_enabled is 0\n", __func__);
		return 1;
	}

	/* Page Num Clear */
	diagnostic_chip(MXT_CTE_MODE);
	msleep(30);/* msleep(20);  */

	diagnostic_chip(dbg_mode);
	msleep(30);/* msleep(20);  */

	ret = get_object_info(copy_data,
		DEBUG_DIAGNOSTIC_T37, &size, &object_address);
/*jerry no need to leave it */
/*
	for (i = 0; i < 5; i++) {
		if (data_buffer[0] == dbg_mode)
			break;

		msleep(20);
	}
*/
	msleep(50); /* msleep(20);  */


	/* 768/64 */
	for (read_page = 0 ; read_page < 6; read_page++) {
		for (node = 0; node < 64; node++) {
			read_point = (node * 2) + 2;
			read_mem(copy_data,
				object_address+(u16)read_point, 2, data_buffer);
				mxt_delta_node[num] =
					((uint16_t)data_buffer[1]<<8)
					+ (uint16_t)data_buffer[0];

					printk(KERN_ERR
						"[TSP] mxt_delta_node[%3d] = %5d\n",
						num, mxt_delta_node[num]);

			num++;

			/* all node => 24 * 32 = 768 => (12page * 64) */
			/*if ((read_page == 11) && (node == 64))
				break;*/
		}
		diagnostic_chip(MXT_PAGE_UP);
		msleep(35);
	}

	return state;
}

int find_channel(uint16_t dbg_mode)
{
	u8 read_page, read_point;
	u16 object_address = 0;
	u8 data_buffer[2] = { 0 };
	u8 node = 0;
	int state = 0;
	int num = 0;
	int ret;
	u16 size;
	u16 delta_val = 0;
	u16 max_val = 0;

	if (!copy_data->mxt_enabled) {
		pr_err("%s : mxt_enabled is 0\n", __func__);
		return 1;
	}

	/* Page Num Clear */
	diagnostic_chip(MXT_CTE_MODE);
	msleep(30);/* msleep(20);  */

	diagnostic_chip(dbg_mode);
	msleep(30);/* msleep(20);  */

	ret = get_object_info(copy_data,
		DEBUG_DIAGNOSTIC_T37, &size, &object_address);
/*jerry no need to leave it */
/*
	for (i = 0; i < 5; i++) {
		if (data_buffer[0] == dbg_mode)
			break;

		msleep(20);
	}
*/
	msleep(50); /* msleep(20);  */


	/* 768/64 */
	for (read_page = 0 ; read_page < 12; read_page++) {
		for (node = 0; node < 64; node++) {
			read_point = (node * 2) + 2;
			read_mem(copy_data,
				object_address+(u16)read_point, 2, data_buffer);
				delta_val = ((uint16_t)data_buffer[1]<<8)
					+ (uint16_t)data_buffer[0];

				if (delta_val > 32767)
					delta_val = 65535 - delta_val;
				if (delta_val > max_val) {
					max_val = delta_val;
					state = (read_point - 2)/2 +
						(read_page * 64);
				}

			num++;

			/* all node => 24 * 32 = 768 => (12page * 64) */
			/*if ((read_page == 11) && (node == 64))
				break;*/
		}
		diagnostic_chip(MXT_PAGE_UP);
		msleep(35);
	}

	return state;
}

static ssize_t find_channel_show(struct device *dev,
					struct device_attribute *attr,
					char *buf)
{
	int status = 0;

	status = find_channel(MXT_DELTA_MODE);

	return sprintf(buf, "%u\n", status);
}
#endif

static int mxt_check_bootloader(struct i2c_client *client,
					unsigned int state)
{
	u8 val;
	u8 temp;

recheck:
	if (i2c_master_recv(client, &val, 1) != 1)
		return -EIO;

	if (val & 0x20)	{
		if (i2c_master_recv(client, &temp, 1) != 1)
			return -EIO;

		if (i2c_master_recv(client, &temp, 1) != 1)
			return -EIO;
		val &= ~0x20;
	}

	if ((val & 0xF0) == MXT_APP_CRC_FAIL) {
		pr_info("MXT_APP_CRC_FAIL\n");
		if (i2c_master_recv(client, &val, 1) != 1)
			return -EIO;

		if (val & 0x20) {
			if (i2c_master_recv(client, &temp, 1) != 1)
				return -EIO;

			if (i2c_master_recv(client, &temp, 1) != 1)
				return -EIO;
			val &= ~0x20;
		}
	}

	switch (state) {
	case MXT_WAITING_BOOTLOAD_CMD:
	case MXT_WAITING_FRAME_DATA:
		val &= ~MXT_BOOT_STATUS_MASK;
		break;
	case MXT_FRAME_CRC_PASS:
		if (val == MXT_FRAME_CRC_CHECK)
			goto recheck;
		break;
	default:
		return -EINVAL;
	}

	if (val != state) {
		pr_err("Unvalid bootloader mode state\n");
		return -EINVAL;
	}

	return 0;
}

static int mxt_unlock_bootloader(struct i2c_client *client)
{
	u8 buf[2];

	buf[0] = MXT_UNLOCK_CMD_LSB;
	buf[1] = MXT_UNLOCK_CMD_MSB;

	if (i2c_master_send(client, buf, 2) != 2) {
		pr_err("%s: i2c send failed\n",
			__func__);

		return -EIO;
	}

	return 0;
}

static int mxt_fw_write(struct i2c_client *client,
				const u8 *data, unsigned int frame_size)
{
	if (i2c_master_send(client, data, frame_size) != frame_size) {
		pr_err("%s: i2c send failed\n", __func__);
		return -EIO;
	}

	return 0;
}

static int mxt_load_fw(struct device *dev, const char *fn)
{
	struct mxt_data *data = copy_data;
	struct i2c_client *client = copy_data->client;
	unsigned int frame_size;
	unsigned int pos = 0;
	int ret;
	u16 obj_address = 0;
	u16 size_one;
	u8 value;
	unsigned int object_register;
	int check_frame_crc_error = 0;
	int check_wating_frame_data_error = 0;

#if READ_FW_FROM_HEADER
	struct firmware *fw = NULL;

	pr_info("mxt_load_fw start from header!!!\n");
	fw = kzalloc(sizeof(struct firmware), GFP_KERNEL);

	fw->data = firmware_mXT;
	fw->size = sizeof(firmware_mXT);
#else
	const struct firmware *fw = NULL;

	pr_info("mxt_load_fw startl!!!\n");
	ret = request_firmware(&fw, fn, &client->dev);
	if (ret) {
		pr_err("Unable to open firmware %s\n", fn);
		return ret;
	}
#endif
	/* Change to the bootloader mode */
	object_register = 0;
	value = (u8)MXT_BOOT_VALUE;
	ret = get_object_info(data,
		GEN_COMMANDPROCESSOR_T6, &size_one, &obj_address);
	if (ret) {
		pr_err("fail to get object_info\n");
		release_firmware(fw);
		return ret;
	}
	size_one = 1;
	write_mem(data, obj_address + (u16)object_register, (u8)size_one, &value);
	msleep(MXT_SW_RESET_TIME);

	/* Change to slave address of bootloader */
	if (client->addr == MXT_APP_LOW)
		client->addr = MXT_BOOT_LOW;
	else
		client->addr = MXT_BOOT_HIGH;

	ret = mxt_check_bootloader(client, MXT_WAITING_BOOTLOAD_CMD);
	if (ret)
		goto out;

	/* Unlock bootloader */
	mxt_unlock_bootloader(client);

	while (pos < fw->size) {
		ret = mxt_check_bootloader(client,
						MXT_WAITING_FRAME_DATA);
		if (ret) {
			check_wating_frame_data_error++;
			if (check_wating_frame_data_error > 10) {
				pr_err("firm update fail. wating_frame_data err\n");
				goto out;
			} else {
				pr_err("check_wating_frame_data_error = %d, retry\n",
					check_wating_frame_data_error);
				continue;
			}
		}

		frame_size = ((*(fw->data + pos) << 8) | *(fw->data + pos + 1));

		/* We should add 2 at frame size as the the firmware data is not
		* included the CRC bytes.
		*/
		frame_size += 2;

		/* Write one frame to device */
		mxt_fw_write(client, fw->data + pos, frame_size);

		ret = mxt_check_bootloader(client,
						MXT_FRAME_CRC_PASS);
		if (ret) {
			check_frame_crc_error++;
			if (check_frame_crc_error > 10) {
				pr_err("firm update fail. frame_crc err\n");
				goto out;
			} else {
				pr_err("check_frame_crc_error = %d, retry\n",
					check_frame_crc_error);
				continue;
			}
		}

		pos += frame_size;

		pr_info("Updated %d bytes / %zd bytes\n",
			pos, fw->size);

		msleep(20);
	}

out:
#if READ_FW_FROM_HEADER
	kfree(fw);
#else
	release_firmware(fw);
#endif
	/* Change to slave address of application */
	if (client->addr == MXT_BOOT_LOW)
		client->addr = MXT_APP_LOW;
	else
		client->addr = MXT_APP_HIGH;
	return ret;
}

static int mxt_load_fw_bootmode(struct device *dev, const char *fn)
{
	struct i2c_client *client = copy_data->client;
	unsigned int frame_size;
	unsigned int pos = 0;
	int ret;

	int check_frame_crc_error = 0;
	int check_wating_frame_data_error = 0;

#if READ_FW_FROM_HEADER
	struct firmware *fw = NULL;
	pr_info("mxt_load_fw start from header!!!\n");
	fw = kzalloc(sizeof(struct firmware), GFP_KERNEL);

	fw->data = firmware_mXT;
	fw->size = sizeof(firmware_mXT);
#else
	const struct firmware *fw = NULL;
	pr_info("mxt_load_fw start!!!\n");

	ret = request_firmware(&fw, fn, &client->dev);
	if (ret) {
		pr_err("Unable to open firmware %s\n", fn);
		return ret;
	}
#endif
	/* Unlock bootloader */
	mxt_unlock_bootloader(client);

	while (pos < fw->size) {
		ret = mxt_check_bootloader(client,
						MXT_WAITING_FRAME_DATA);
		if (ret) {
			check_wating_frame_data_error++;
			if (check_wating_frame_data_error > 10) {
				pr_err("firm update fail. wating_frame_data err\n");
				goto out;
			} else {
				pr_err("check_wating_frame_data_error = %d, retry\n",
					check_wating_frame_data_error);
				continue;
			}
		}

		frame_size = ((*(fw->data + pos) << 8) | *(fw->data + pos + 1));

		/* We should add 2 at frame size as the the firmware data is not
		* included the CRC bytes.
		*/
		frame_size += 2;

		/* Write one frame to device */
		mxt_fw_write(client, fw->data + pos, frame_size);

		ret = mxt_check_bootloader(client,
						MXT_FRAME_CRC_PASS);
		if (ret) {
			check_frame_crc_error++;
			if (check_frame_crc_error > 10) {
				pr_err("firm update fail. frame_crc err\n");
				goto out;
			} else {
				pr_err("check_frame_crc_error = %d, retry\n",
					check_frame_crc_error);
				continue;
			}
		}

		pos += frame_size;

		pr_info("Updated %d bytes / %zd bytes\n",
			pos, fw->size);

		msleep(20);
	}

out:
#if READ_FW_FROM_HEADER
	kfree(fw);
#else
	release_firmware(fw);
#endif
	/* Change to slave address of application */
	if (client->addr == MXT_BOOT_LOW)
		client->addr = MXT_APP_LOW;
	else
		client->addr = MXT_APP_HIGH;
	return ret;
}

#if SYSFS
static ssize_t set_refer0_mode_show(struct device *dev,
	struct device_attribute *attr, char *buf)
{
	uint16_t mxt_reference = 0;
	read_dbg_data(MXT_REFERENCE_MODE, test_node[0], &mxt_reference);
	return sprintf(buf, "%u\n", mxt_reference);
}

static ssize_t set_refer1_mode_show(struct device *dev,
	struct device_attribute *attr, char *buf)
{
	uint16_t mxt_reference = 0;
	read_dbg_data(MXT_REFERENCE_MODE, test_node[1], &mxt_reference);
	return sprintf(buf, "%u\n", mxt_reference);
}

static ssize_t set_refer2_mode_show(struct device *dev,
	struct device_attribute *attr, char *buf)
{
	uint16_t mxt_reference = 0;
	read_dbg_data(MXT_REFERENCE_MODE, test_node[2], &mxt_reference);
	return sprintf(buf, "%u\n", mxt_reference);
}

static ssize_t set_refer3_mode_show(struct device *dev,
	struct device_attribute *attr, char *buf)
{
	uint16_t mxt_reference = 0;
	read_dbg_data(MXT_REFERENCE_MODE, test_node[3], &mxt_reference);
	return sprintf(buf, "%u\n", mxt_reference);
}

static ssize_t set_refer4_mode_show(struct device *dev,
	struct device_attribute *attr, char *buf)
{
	uint16_t mxt_reference = 0;
	read_dbg_data(MXT_REFERENCE_MODE, test_node[4], &mxt_reference);
	return sprintf(buf, "%u\n", mxt_reference);
}

static ssize_t set_delta0_mode_show(struct device *dev,
	struct device_attribute *attr, char *buf)
{
	uint16_t mxt_delta = 0;
	read_dbg_data(MXT_DELTA_MODE, test_node[0], &mxt_delta);
	if (mxt_delta < 32767)
		return sprintf(buf, "%u\n", mxt_delta);
	else
		mxt_delta = 65535 - mxt_delta;

	if (mxt_delta)
		return sprintf(buf, "-%u\n", mxt_delta);
	else
		return sprintf(buf, "%u\n", mxt_delta);
}

static ssize_t set_delta1_mode_show(struct device *dev,
	struct device_attribute *attr, char *buf)
{
	uint16_t mxt_delta = 0;
	read_dbg_data(MXT_DELTA_MODE, test_node[1], &mxt_delta);
	if (mxt_delta < 32767)
		return sprintf(buf, "%u\n", mxt_delta);
	else
		mxt_delta = 65535 - mxt_delta;

	if (mxt_delta)
		return sprintf(buf, "-%u\n", mxt_delta);
	else
		return sprintf(buf, "%u\n", mxt_delta);
}

static ssize_t set_delta2_mode_show(struct device *dev,
	struct device_attribute *attr, char *buf)
{
	uint16_t mxt_delta = 0;
	read_dbg_data(MXT_DELTA_MODE, test_node[2], &mxt_delta);
	if (mxt_delta < 32767)
		return sprintf(buf, "%u\n", mxt_delta);
	else
		mxt_delta = 65535 - mxt_delta;

	if (mxt_delta)
		return sprintf(buf, "-%u\n", mxt_delta);
	else
		return sprintf(buf, "%u\n", mxt_delta);
}

static ssize_t set_delta3_mode_show(struct device *dev,
	struct device_attribute *attr, char *buf)
{
	uint16_t mxt_delta = 0;
	read_dbg_data(MXT_DELTA_MODE, test_node[3], &mxt_delta);
	if (mxt_delta < 32767)
		return sprintf(buf, "%u\n", mxt_delta);
	else
		mxt_delta = 65535 - mxt_delta;

	if (mxt_delta)
		return sprintf(buf, "-%u\n", mxt_delta);
	else
		return sprintf(buf, "%u\n", mxt_delta);
}

static ssize_t set_delta4_mode_show(struct device *dev,
	struct device_attribute *attr, char *buf)
{
	uint16_t mxt_delta = 0;
	read_dbg_data(MXT_DELTA_MODE, test_node[4], &mxt_delta);
	if (mxt_delta < 32767)
		return sprintf(buf, "%u\n", mxt_delta);
	else
		mxt_delta = 65535 - mxt_delta;

	if (mxt_delta)
		return sprintf(buf, "-%u\n", mxt_delta);
	else
		return sprintf(buf, "%u\n", mxt_delta);
}

static ssize_t set_threshold_mode_show(struct device *dev,
	struct device_attribute *attr, char *buf)
{
	return sprintf(buf, "%u\n", threshold);
}

static ssize_t set_all_refer_mode_show(struct device *dev,
	struct device_attribute *attr, char *buf)
{
	int status = 0;

	status = read_all_data(MXT_REFERENCE_MODE);

	return snprintf(buf, 20,
			"%u, %u, %u\n", status, max_ref, min_ref);
}

static int atoi(const char *str)
{
	int result = 0;
	int count = 0;
	if (str == NULL)
		return -1;
	while (str[count] && str[count] >= '0' && str[count] <= '9') {
		result = result * 10 + str[count] - '0';
		++count;
	}
	return result;
}

static ssize_t disp_all_refdata_show(struct device *dev,
	struct device_attribute *attr, char *buf)
{
	return sprintf(buf, "%u\n",  mxt_refer_node[index_reference]);
}

static ssize_t disp_all_refdata_store(struct device *dev,
	struct device_attribute *attr, const char *buf, size_t size)
{
	index_reference = atoi(buf);
	return size;
}

static ssize_t set_all_delta_mode_show(struct device *dev,
	struct device_attribute *attr, char *buf)
{
	int status = 0;

	status = read_all_delta_data(MXT_DELTA_MODE);

	return sprintf(buf, "%u\n", status);
}

static ssize_t disp_all_deltadata_show(struct device *dev,
	struct device_attribute *attr, char *buf)
{
	if (mxt_delta_node[index_delta] < 32767)
		return sprintf(buf, "%u\n", mxt_delta_node[index_delta]);
	else
		mxt_delta_node[index_delta] = 65535 - mxt_delta_node[index_delta];

	return sprintf(buf, "-%u\n", mxt_delta_node[index_delta]);
}

static ssize_t disp_all_deltadata_store(struct device *dev,
	struct device_attribute *attr, const char *buf, size_t size)
{
	index_delta = atoi(buf);
	return size;
}

static ssize_t set_firm_version_show(struct device *dev,
	struct device_attribute *attr, char *buf)
{
	struct mxt_data *data = copy_data;

	u8 id[ID_BLOCK_SIZE];
	u8 value;
	int ret;
	u8 i;

	if (data->mxt_enabled) {
		disable_irq(data->client->irq);
		for (i = 0; i < 4; i++) {
			ret = read_mem(copy_data, 0, sizeof(id), id);
			if (!ret)
				break;
		}
		enable_irq(data->client->irq);
		if (ret < 0) {
			pr_err("TSP read fail : %s", __func__);
			value = 0;
			return sprintf(buf, "%d\n", value);
		} else {
			pr_info("%s : %#02x\n",
				__func__, id[2]);
			return sprintf(buf, "%#02x\n", id[2]);
		}
	} else {
		pr_err("TSP power off : %s", __func__);
		value = 0;
		return sprintf(buf, "%d\n", value);
	}
}

static ssize_t set_module_off_store(struct device *dev,
	struct device_attribute *attr, const char *buf, size_t size)
{
	struct mxt_data *data = copy_data;
	pr_info("%s!!\n", __func__);

	if (*buf != 'S' && *buf != 'F') {
		pr_err("Invalid values\n");
		return -EINVAL;
	}
	if (data->mxt_enabled) {
		data->mxt_enabled = false;
/*		touch_is_pressed = 0;*/
		disable_irq(data->client->irq);
/*		mxt_internal_suspend(data);*/
	}
	return size;
}

static ssize_t set_module_on_store(struct device *dev,
	struct device_attribute *attr, const char *buf, size_t size)
{
	struct mxt_data *data = copy_data;
	bool ta_status = 0;

	pr_info("%s!!\n", __func__);

	if (*buf != 'S' && *buf != 'F') {
		pr_err("Invalid values\n");
		return -EINVAL;
	}

	if (!data->mxt_enabled) {
/*		mxt_internal_resume(data);*/
		enable_irq(data->client->irq);

		data->mxt_enabled = true;

		if (data->read_ta_status) {
			data->read_ta_status(&ta_status);
			pr_info("ta_status is %d\n", ta_status);
			mxt_ta_probe(data, ta_status);
		}
		calibrate_chip(data);
	}

	return size;
}

#if FOR_DEBUGGING_TEST_DOWNLOADFW_BIN
#include <linux/uaccess.h>

#define MXT_FW_BIN_NAME "/sdcard/mxt.bin"

static int mxt_download(const u8 *pBianry, const u32 unLength)
{
	struct mxt_data *data = copy_data;
	struct i2c_client *client = copy_data->client;
	unsigned int frame_size;
	unsigned int pos = 0;
	int ret;
	u16 obj_address = 0;
	u16 size_one;
	u8 value;
	unsigned int object_register;
	int check_frame_crc_error = 0;
	int check_wating_frame_data_error = 0;

	pr_info("mxt_download start!!!\n");

	/* Change to the bootloader mode */
	object_register = 0;
	value = (u8)MXT_BOOT_VALUE;
	ret = get_object_info(data,
		GEN_COMMANDPROCESSOR_T6, &size_one, &obj_address);
	if (ret) {
		pr_err("fail to get object_info\n");
		return ret;
	}
	size_one = 1;

	write_mem(data, obj_address + (u16)object_register, (u8)size_one, &value);
	msleep(MXT_SW_RESET_TIME);

	/* Change to slave address of bootloader */
	if (client->addr == MXT_APP_LOW)
		client->addr = MXT_BOOT_LOW;
	else
		client->addr = MXT_BOOT_HIGH;
	ret = mxt_check_bootloader(client, MXT_WAITING_BOOTLOAD_CMD);

	if (ret)
		goto out;

	/* Unlock bootloader */
	mxt_unlock_bootloader(client);

	while (pos < unLength) {
		ret = mxt_check_bootloader(client,
						MXT_WAITING_FRAME_DATA);
		if (ret) {
			check_wating_frame_data_error++;
			if (check_wating_frame_data_error > 10) {
				pr_err("firm update fail. wating_frame_data err\n");
				goto out;
			} else {
				pr_info("check_wating_frame_data_error=%d, retry\n",
					check_wating_frame_data_error);
				continue;
			}
		}

		frame_size = ((*(pBianry + pos) << 8) | *(pBianry + pos + 1));

		/* We should add 2 at frame size as the the firmware data is not
		* included the CRC bytes.
		*/
		frame_size += 2;

		/* Write one frame to device */
		mxt_fw_write(client, pBianry + pos, frame_size);

		ret = mxt_check_bootloader(client,
						MXT_FRAME_CRC_PASS);
		if (ret) {
			check_frame_crc_error++;
			if (check_frame_crc_error > 10) {
				pr_err("firm update fail. frame_crc err\n");
				goto out;
			} else {
				pr_info("check_frame_crc_error = %d, retry\n",
					check_frame_crc_error);
				continue;
			}
		}

		pos += frame_size;

		pr_info("Updated %d bytes / %zd bytes\n", pos, unLength);

		msleep(20);
	}

out:
	/* Change to slave address of application */
	if (client->addr == MXT_BOOT_LOW)
		client->addr = MXT_APP_LOW;
	else
		client->addr = MXT_APP_HIGH;
	return ret;
}

int mxt_binfile_download(void)
{
	int nRet = 0;
	int retry_cnt = 0;
	long fw1_size = 0;
	unsigned char *fw_data1;
	struct file *filp;
	loff_t	pos;
	int	ret = 0;
	mm_segment_t oldfs;
	spinlock_t	lock;

	oldfs = get_fs();
	set_fs(get_ds());

	filp = filp_open(MXT_FW_BIN_NAME, O_RDONLY, 0);
	if (IS_ERR(filp)) {
		pr_err("file open error:%d\n", (s32)filp);
		return -1;
	}

	fw1_size = filp->f_path.dentry->d_inode->i_size;
	pr_info("Size of the file : %ld(bytes)\n", fw1_size);

	fw_data1 = kmalloc(fw1_size, GFP_KERNEL);
	memset(fw_data1, 0, fw1_size);

	pos = 0;
	memset(fw_data1, 0, fw1_size);
	ret = vfs_read(filp, (char __user *)fw_data1, fw1_size, &pos);

	if (ret != fw1_size) {
		pr_err("Failed to read file %s (ret = %d)\n",
			MXT_FW_BIN_NAME, ret);
		kfree(fw_data1);
		filp_close(filp, current->files);
		return -1;
	}

	filp_close(filp, current->files);

	set_fs(oldfs);

	for (retry_cnt = 0; retry_cnt < 3; retry_cnt++) {
		pr_info("ADB - MASTER CHIP Firmware update! try : %d",
			retry_cnt+1);
		nRet = mxt_download((const u8 *)fw_data1, (const u32)fw1_size);
		if (nRet)
			continue;
		break;
	}

	kfree(fw_data1);
	return nRet;
}
#endif

static ssize_t set_mxt_firm_update_store(struct device *dev,
	struct device_attribute *attr, const char *buf, size_t size)
{
	struct mxt_data *data = dev_get_drvdata(dev);
	u8 **tsp_config = (u8 **)data->pdata->config;
	int i = 0;
	int error = 0;

	pr_info("set_mxt_update_show start!!\n");
	if (*buf != 'S' && *buf != 'F') {
		pr_err("Invalid values\n");
		return -EINVAL;
	}

	disable_irq(data->client->irq);
	firm_status_data = 1;
#if FOR_DEBUGGING_TEST_DOWNLOADFW_BIN
	error = mxt_binfile_download();
#else
	if (*buf != 'F' && data->tsp_version >= firmware_latest[0]
		&& data->tsp_build >= firmware_latest[1]) {
		pr_err("latest firmware\n");
		firm_status_data = 2;
		enable_irq(data->client->irq);
		return size;
	}
	pr_info("fm_update\n");
	error = mxt_load_fw(dev, MXT_FW_NAME);
#endif

	if (error) {
		firm_status_data = 3;
		pr_err("The firmware update failed(%d)\n", error);
		return error;
	} else {
		firm_status_data = 2;
		pr_info("The firmware update succeeded\n");

		/* Wait for reset */
		msleep(MXT_SW_RESET_TIME);

		mxt_make_object_table(data);
		/* mxt224_initialize(data); */
	}

	for (i = 0; tsp_config[i][0] != RESERVED_T255; i++) {
		error = init_write_config(data, tsp_config[i][0],
							tsp_config[i] + 1);
		if (error) {
			pr_err("init_write_config error\n");
			firm_status_data = 3;
			return error;
		}
	}

	error = mxt_backup(data);
	if (error) {
		pr_err("mxt_backup fail!!!\n");
		return error;
	}

	/* reset the touch IC. */
	error = mxt_reset(data);
	if (error) {
		pr_err("mxt_reset fail!!!\n");
		return error;
	}

	msleep(MXT_SW_RESET_TIME);
	enable_irq(data->client->irq);

	return size;
}

static ssize_t set_mxt_firm_status_show(struct device *dev,
	struct device_attribute *attr, char *buf)
{

	int count;
	pr_info("Enter firmware_status_show by Factory command\n");

	if (firm_status_data == 1)
		count = sprintf(buf, "DOWNLOADING\n");
	else if (firm_status_data == 2)
		count = sprintf(buf, "PASS\n");
	else if (firm_status_data == 3)
		count = sprintf(buf, "FAIL\n");
	else
		count = sprintf(buf, "PASS\n");

	return count;
}

static ssize_t key_threshold_show(struct device *dev,
	struct device_attribute *attr, char *buf)
{
	return sprintf(buf, "%u\n", threshold);
}

static ssize_t key_threshold_store(struct device *dev,
	struct device_attribute *attr, const char *buf, size_t size)
{
	/*TO DO IT*/
	unsigned int object_register = 7;
	u8 value;
	u8 val;
	int ret;
	u16 address = 0;
	u16 size_one;
	int num;
	if (sscanf(buf, "%d", &num) == 1) {
		threshold = num;
		pr_info("threshold value %d\n", threshold);
		ret = get_object_info(copy_data,
			TOUCH_MULTITOUCHSCREEN_T9, &size_one, &address);
		size_one = 1;
		value = (u8)threshold;
		write_mem(copy_data,
			address+(u16)object_register, size_one, &value);
		read_mem(copy_data,
			address+(u16)object_register, (u8)size_one, &val);
		pr_err("T9 Byte%d is %d\n", object_register, val);
	}
	return size;
}

static ssize_t set_mxt_firm_version_show(struct device *dev,
	struct device_attribute *attr, char *buf)
{
	pr_info("phone's version : %#02x,%#02x\n"
		, firmware_latest[0], firmware_latest[1]);
	return sprintf(buf, "%#02x\n", firmware_latest[0]);
}

static ssize_t set_mxt_firm_version_read_show(struct device *dev,
	struct device_attribute *attr, char *buf)
{
	struct mxt_data *data = dev_get_drvdata(dev);
	pr_info("phone's version : %#02x,%#02x\n"
		, data->tsp_version, data->tsp_build);
	return sprintf(buf, "%#02x\n", data->tsp_version);
}

static ssize_t set_mxt_config_version_show(struct device *dev,
		struct device_attribute *attr, char *buf)
{
	struct mxt_data *data = dev_get_drvdata(dev);

	return snprintf(buf, 20,
				"%s\n", data->pdata->config_fw_version);
}

static ssize_t mxt_touchtype_show(struct device *dev,
	struct device_attribute *attr, char *buf)
{
	char temp[15];

	sprintf(temp, "ATMEL,MXT224S\n");
	strcat(buf, temp);

	return strlen(buf);
}

static ssize_t x_line_show(struct device *dev,
	struct device_attribute *attr, char *buf)
{
	u8 data = 24;
	return sprintf(buf, "%d\n", data);
}

static ssize_t y_line_show(struct device *dev,
	struct device_attribute *attr, char *buf)
{
	u8 data = 32;
	return sprintf(buf, "%d\n", data);
}

#if ITDEV
/* Functions for mem_access interface */
struct bin_attribute mem_access_attr;

static int mxt_read_block(struct i2c_client *client,
		   u16 addr,
		   u16 length,
		   u8 *value)
{
	struct i2c_adapter *adapter = client->adapter;
	struct i2c_msg msg[2];
	__le16	le_addr;
	struct mxt_data *mxt;

	mxt = i2c_get_clientdata(client);

	if (mxt != NULL) {
		if ((mxt->last_read_addr == addr) &&
			(addr == mxt->msg_proc_addr)) {
			if  (i2c_master_recv(client, value, length) == length) {
#if ITDEV
				if (debug_enabled)
					print_hex_dump(KERN_INFO, "MXT RX:",
						DUMP_PREFIX_NONE, 16, 1,
						value, length, false);
#endif
				return 0;
			} else
				return -EIO;
		} else {
			mxt->last_read_addr = addr;
		}
	}

	le_addr = cpu_to_le16(addr);
	msg[0].addr  = client->addr;
	msg[0].flags = 0x00;
	msg[0].len   = 2;
	msg[0].buf   = (u8 *) &le_addr;

	msg[1].addr  = client->addr;
	msg[1].flags = I2C_M_RD;
	msg[1].len   = length;
	msg[1].buf   = (u8 *) value;
	if (i2c_transfer(adapter, msg, 2) == 2) {
#if ITDEV
		if (debug_enabled) {
			print_hex_dump(KERN_INFO, "MXT TX:", DUMP_PREFIX_NONE,
				16, 1, msg[0].buf, msg[0].len, false);
			print_hex_dump(KERN_INFO, "MXT RX:", DUMP_PREFIX_NONE,
				16, 1, msg[1].buf, msg[1].len, false);
		}
#endif
		return 0;
	} else
		return -EIO;
}

/* Writes a block of bytes (max 256) to given address in mXT chip. */

int mxt_write_block(struct i2c_client *client,
		    u16 addr,
		    u16 length,
		    u8 *value)
{
	int i;
	struct {
		__le16	le_addr;
		u8	data[256];

	} i2c_block_transfer;

	struct mxt_data *mxt;

	if (length > 256)
		return -EINVAL;

	mxt = i2c_get_clientdata(client);
	if (mxt != NULL)
		mxt->last_read_addr = -1;

	for (i = 0; i < length; i++)
		i2c_block_transfer.data[i] = *value++;

	i2c_block_transfer.le_addr = cpu_to_le16(addr);

	i = i2c_master_send(client, (u8 *) &i2c_block_transfer, length + 2);

	if (i == (length + 2)) {
#if ITDEV
		if (debug_enabled)
			print_hex_dump(KERN_INFO, "MXT TX:", DUMP_PREFIX_NONE,
				16, 1, &i2c_block_transfer, length+2, false);
#endif
		return length;
	} else
		return -EIO;
}

static ssize_t mem_access_read(struct file *filp, struct kobject *kobj,
	struct bin_attribute *bin_attr, char *buf, loff_t off, size_t count)
{
	int ret = 0;
	struct i2c_client *client;

	pr_info("mem_access_read p=%p off=%lli c=%zi\n", buf, off, count);

	if (off >= 32768)
		return -EIO;

	if (off + count > 32768)
		count = 32768 - off;

	if (count > 256)
		count = 256;

	if (count > 0)	{
		client = to_i2c_client(container_of(kobj, struct device, kobj));
		ret = mxt_read_block(client, off, count, buf);
	}

	return ret >= 0 ? count : ret;
}

static ssize_t mem_access_write(struct file *filp, struct kobject *kobj,
	struct bin_attribute *bin_attr, char *buf, loff_t off, size_t count)
{
	int ret = 0;
	struct i2c_client *client;

	pr_info("mem_access_write p=%p off=%lli c=%zi\n", buf, off, count);

	if (off >= 32768)
		return -EIO;

	if (off + count > 32768)
		count = 32768 - off;

	if (count > 256)
		count = 256;

	if (count > 0) {
		client = to_i2c_client(container_of(kobj, struct device, kobj));
		ret = mxt_write_block(client, off, count, buf);
	}

	return ret >= 0 ? count : 0;
}

static ssize_t pause_show(struct device *dev,
	struct device_attribute *attr, char *buf)
{
	int count = 0;

	count += sprintf(buf + count, "%d", driver_paused);
	count += sprintf(buf + count, "\n");

	return count;
}

static ssize_t pause_store(struct device *dev,
	struct device_attribute *attr, const char *buf, size_t count)
{
	int i;
	if (sscanf(buf, "%u", &i) == 1 && i < 2) {
		driver_paused = i;

		pr_info("%s\n", i ? "paused" : "unpaused");
	} else {
		pr_info("pause_driver write error\n");
	}

	return count;
}

static ssize_t debug_enable_show(struct device *dev,
	struct device_attribute *attr, char *buf)
{
	int count = 0;

	count += sprintf(buf + count, "%d", debug_enabled);
	count += sprintf(buf + count, "\n");

	return count;
}

static ssize_t debug_enable_store(struct device *dev,
	struct device_attribute *attr, const char *buf, size_t count)
{
	int i;
	if (sscanf(buf, "%u", &i) == 1 && i < 2) {
		debug_enabled = i;

		pr_info("%s\n",
			i ? "debug enabled" : "debug disabled");
	} else {
		pr_info("debug_enabled write error\n");
	}

	return count;
}

static ssize_t command_calibrate_store(struct device *dev,
	struct device_attribute *attr, const char *buf, size_t count)
{
	struct mxt_data *data = copy_data;
	int ret;

	ret = calibrate_chip(data);

	return (ret < 0) ? ret : count;
}

static ssize_t command_reset_store(struct device *dev,
	struct device_attribute *attr, const char *buf, size_t count)
{
	struct i2c_client *client;
	struct mxt_data   *mxt;
	int ret;

	client = to_i2c_client(dev);
	mxt = i2c_get_clientdata(client);

	ret = mxt_reset(mxt);

	return (ret < 0) ? ret : count;
}

static ssize_t command_backup_store(struct device *dev,
	struct device_attribute *attr, const char *buf, size_t count)
{
	struct i2c_client *client;
	struct mxt_data   *mxt;
	int ret;

	client = to_i2c_client(dev);
	mxt = i2c_get_clientdata(client);

	ret = mxt_backup(mxt);

	return (ret < 0) ? ret : count;
}
#endif

static DEVICE_ATTR(set_refer0, S_IRUGO,
	set_refer0_mode_show, NULL);
static DEVICE_ATTR(set_delta0, S_IRUGO,
	set_delta0_mode_show, NULL);
static DEVICE_ATTR(set_refer1, S_IRUGO,
	set_refer1_mode_show, NULL);
static DEVICE_ATTR(set_delta1, S_IRUGO,
	set_delta1_mode_show, NULL);
static DEVICE_ATTR(set_refer2, S_IRUGO,
	set_refer2_mode_show, NULL);
static DEVICE_ATTR(set_delta2, S_IRUGO,
	set_delta2_mode_show, NULL);
static DEVICE_ATTR(set_refer3, S_IRUGO,
	set_refer3_mode_show, NULL);
static DEVICE_ATTR(set_delta3, S_IRUGO,
	set_delta3_mode_show, NULL);
static DEVICE_ATTR(set_refer4, S_IRUGO,
	set_refer4_mode_show, NULL);
static DEVICE_ATTR(set_delta4, S_IRUGO,
	set_delta4_mode_show, NULL);
static DEVICE_ATTR(set_all_refer, S_IRUGO,
	set_all_refer_mode_show, NULL);
static DEVICE_ATTR(disp_all_refdata, S_IRUGO | S_IWUSR | S_IWGRP,
	disp_all_refdata_show, disp_all_refdata_store);
static DEVICE_ATTR(set_all_delta, S_IRUGO,
	set_all_delta_mode_show, NULL);
static DEVICE_ATTR(disp_all_deltadata, S_IRUGO | S_IWUSR | S_IWGRP,
	disp_all_deltadata_show, disp_all_deltadata_store);
static DEVICE_ATTR(set_firm_version, S_IRUGO | S_IWUSR | S_IWGRP,
	set_firm_version_show, NULL);
static DEVICE_ATTR(set_module_off, S_IRUGO | S_IWUSR | S_IWGRP,
	NULL, set_module_off_store);
static DEVICE_ATTR(set_module_on, S_IRUGO | S_IWUSR | S_IWGRP,
	NULL, set_module_on_store);
static DEVICE_ATTR(mxt_touchtype, S_IRUGO | S_IWUSR | S_IWGRP,
	mxt_touchtype_show, NULL);
static DEVICE_ATTR(set_threshold, S_IRUGO,
	set_threshold_mode_show, NULL);
/* firmware update */
static DEVICE_ATTR(tsp_firm_update, S_IWUSR | S_IWGRP,
	NULL, set_mxt_firm_update_store);
/* firmware update status return */
static DEVICE_ATTR(tsp_firm_update_status, S_IRUGO,
	set_mxt_firm_status_show, NULL);
/* touch threshold return, store */
static DEVICE_ATTR(tsp_threshold, S_IRUGO | S_IWUSR | S_IWGRP,
	key_threshold_show, key_threshold_store);
/* PHONE*/	/* firmware version resturn in phone driver version */
static DEVICE_ATTR(tsp_firm_version_phone, S_IRUGO,
	set_mxt_firm_version_show, NULL);
/*PART*/	/* firmware version resturn in TSP panel version */
static DEVICE_ATTR(tsp_firm_version_panel, S_IRUGO,
	set_mxt_firm_version_read_show, NULL);
static DEVICE_ATTR(tsp_firm_version_config, S_IRUGO,
			set_mxt_config_version_show, NULL);
static DEVICE_ATTR(object_show, S_IWUSR | S_IWGRP, NULL,
	mxt_object_show);
static DEVICE_ATTR(object_write, S_IWUSR | S_IWGRP, NULL,
	mxt_object_setting);
static DEVICE_ATTR(dbg_switch, S_IWUSR | S_IWGRP, NULL,
	mxt_debug_setting);
static DEVICE_ATTR(find_delta_channel, S_IRUGO,
	find_channel_show, NULL);
static DEVICE_ATTR(x_line, S_IRUGO,
	x_line_show, NULL);
static DEVICE_ATTR(y_line, S_IRUGO,
	y_line_show, NULL);
static DEVICE_ATTR(set_tsp_name, S_IRUGO,
			mxt_touchtype_show, NULL);

#if ITDEV
/* Sysfs files for libmaxtouch interface */
static DEVICE_ATTR(pause_driver, 0666,
	pause_show, pause_store);
static DEVICE_ATTR(debug_enable, 0666,
	debug_enable_show, debug_enable_store);
static DEVICE_ATTR(command_calibrate, 0666,
	NULL, command_calibrate_store);
static DEVICE_ATTR(command_reset, 0666,
	NULL, command_reset_store);
static DEVICE_ATTR(command_backup, 0666,
	NULL, command_backup_store);

static struct attribute *libmaxtouch_attributes[] = {
	&dev_attr_pause_driver.attr,
	&dev_attr_debug_enable.attr,
	&dev_attr_command_calibrate.attr,
	&dev_attr_command_reset.attr,
	&dev_attr_command_backup.attr,
	NULL,
};

static struct attribute_group libmaxtouch_attr_group = {
	.attrs = libmaxtouch_attributes,
};
#endif

static struct attribute *mxt_attrs[] = {
	&dev_attr_object_show.attr,
	&dev_attr_object_write.attr,
	&dev_attr_dbg_switch.attr,
	NULL
};

static const struct attribute_group mxt_attr_group = {
	.attrs = mxt_attrs,
};

#endif

static int mxt_make_object_table(struct mxt_data *data)
{
	struct i2c_client *client = data->client;
	struct object_t *object_table;
	struct report_id_map_t *report_id_map_t;
	u32 read_crc = 0;
	u32 calc_crc;
	u16 crc_address;
	u16 dummy;
	int ret, i, j;
	int cur_rep_id, start_report_id;
	u8 id[ID_BLOCK_SIZE];
	u8 type_count = 0;
	u8 tmp;

	ret = read_mem(data, 0, sizeof(id), id);
	if (ret)
		return ret;

	dev_info(&client->dev,"family = %#02x, variant = %#02x, version = %#02x,\
		build = %#02x, matrix X,Y size = %d,%d\n",
		id[0], id[1], id[2], id[3], id[4], id[5]);

	data->family_id = id[0];
	data->tsp_variant = id[1];
	data->tsp_version = id[2];
	data->tsp_build = id[3];
	data->objects_len = id[6];

	if (data->object_alloc) {
		data->object_alloc = false;
		kfree(data->objects);
	}
	object_table = kmalloc(data->objects_len * sizeof(*object_table),
			GFP_KERNEL);
	if (!object_table)
		return -ENOMEM;

	ret = read_mem(data, OBJECT_TABLE_START_ADDRESS,
		data->objects_len * sizeof(*object_table), (u8 *)object_table);
	if (ret) {
		kfree(object_table);
		return ret;
	}

	data->max_report_id = 0;
	for (i = 0; i < data->objects_len; i++) {
		object_table[i].i2c_address =
			le16_to_cpu(object_table[i].i2c_address);
		data->max_report_id += object_table[i].num_report_ids *
			(object_table[i].instances + 1);
		tmp = 0;
		if (object_table[i].num_report_ids) {
			tmp = type_count + 1;
			type_count += object_table[i].num_report_ids *
				(object_table[i].instances + 1);
		}
		switch (object_table[i].object_type) {
		case TOUCH_MULTITOUCHSCREEN_T9:
			data->finger_type = tmp;
			dev_info(&client->dev,"Finger type = %d\n",
				data->finger_type);
			break;
		case GEN_MESSAGEPROCESSOR_T5:
#if ITDEV
			data->msg_proc_addr = object_table[i].i2c_address;
#endif
			data->msg_object_size = object_table[i].size + 1;
			break;
		}
	}
	if (data->rid_map_alloc) {
		data->rid_map_alloc = false;
		kfree(data->rid_map);
	}
	data->rid_map = kmalloc((sizeof(report_id_map_t) *
		data->max_report_id + 1), GFP_KERNEL);
	if (!data->rid_map) {
		kfree(object_table);
		return -ENOMEM;
	}

	data->rid_map_alloc = true;
	data->rid_map[0].instance = 0;
	data->rid_map[0].object_type = 0;
	cur_rep_id = 1;
	for (i = 0; i < data->objects_len; i++) {
		if (object_table[i].num_report_ids != 0) {
			for (j = 0; j <= object_table[i].instances; j++) {
				for (start_report_id = cur_rep_id;
					cur_rep_id <
						(start_report_id + object_table[i].num_report_ids);
					cur_rep_id++) {
					data->rid_map[cur_rep_id].instance = j;
					data->rid_map[cur_rep_id].object_type =
						object_table[i].object_type;
				}
			}
		}
	}
	data->objects = object_table;
	data->object_alloc = true;

	/* Verify CRC */
	crc_address = OBJECT_TABLE_START_ADDRESS +
			data->objects_len * OBJECT_TABLE_ELEMENT_SIZE;

#ifdef __BIG_ENDIAN
#error The following code will likely break on a big endian machine
#endif
	ret = read_mem(data, crc_address, 3, (u8 *)&read_crc);
	if (ret)
		goto err;

	read_crc = le32_to_cpu(read_crc);

	ret = calculate_infoblock_crc(data, &calc_crc);
	if (ret)
		goto err;

	if (read_crc != calc_crc) {
		dev_err(&client->dev,"CRC error\n");
		ret = -EFAULT;
		goto err;
	}

	ret = get_object_info(data, GEN_MESSAGEPROCESSOR_T5, &dummy,
		&data->msg_proc);
	if (ret)
		goto err;

	ret = get_object_info(data, GEN_COMMANDPROCESSOR_T6, &dummy,
		&data->cmd_proc);
	if (ret)
		goto err;

#if DEBUG_INFO
	dev_info(&client->dev, "maXTouch: %d Objects\n",
			data->objects_len);

	for (i = 0; i < data->objects_len; i++) {
		dev_info(&client->dev, "Type:\t\t\t[%d]: %s\n",
			object_table[i].object_type,
			object_type_name[object_table[i].object_type]);
		dev_info(&client->dev, "\tAddress:\t0x%04X\n",
			object_table[i].i2c_address);
		dev_info(&client->dev, "\tSize:\t\t%d Bytes\n",
			object_table[i].size);
		dev_info(&client->dev, "\tInstances:\t%d\n",
			object_table[i].instances);
		dev_info(&client->dev, "\tReport Id's:\t%d\n",
			object_table[i].num_report_ids);
	}
#endif

	return 0;

err:
	data->rid_map_alloc = false;
	kfree(data->rid_map);
	data->object_alloc = false;
	kfree(object_table);
	return ret;
}

static int __devinit mxt_init_config(struct mxt_data *data)
{
	struct i2c_client *client = data->client;
	int ret, i;
	u16 size;
	u16 obj_address = 0;
	u8 **tsp_config;
/*
	if (client->addr == MXT_APP_LOW)
		client->addr = MXT_BOOT_LOW;
	else
		client->addr = MXT_BOOT_HIGH;

	ret = mxt_check_bootloader(client, MXT_WAITING_BOOTLOAD_CMD);
	if (ret >= 0) {
		dev_notice(&client->dev, "boot mode. firm update excute\n");
		mxt_load_fw_bootmode(NULL, MXT_FW_NAME);
		msleep(MXT_SW_RESET_TIME);
	} else {
		if (client->addr == MXT_BOOT_LOW)
			client->addr = MXT_APP_LOW;
		else
			client->addr = MXT_APP_HIGH;
	}
*/
	ret = mxt_make_object_table(data);
	if (ret) {
		dev_err(&client->dev, "%s mxt_make_object_table fail\n", __func__);
		return ret;
	}

	/* tsp_family_id - 0x82 : MXT224S series */
	if (data->family_id == 0x82) {
		tsp_config = (u8 **)data->pdata->config;

#if !(FOR_BRINGUP)
		data->t48_config_batt = data->pdata->t48_config_batt;
		data->t48_config_chrg = data->pdata->t48_config_chrg;
		data->tchthr_batt = data->pdata->tchthr_batt;
		data->tchthr_charging = data->pdata->tchthr_charging;
		data->calcfg_batt = data->pdata->calcfg_batt;
		data->calcfg_charging = data->pdata->calcfg_charging;
#endif
#if UPDATE_ON_PROBE
#if !(FOR_DEBUGGING_TEST_DOWNLOADFW_BIN)
		if (data->tsp_version < firmware_latest[0]
			|| (data->tsp_version == firmware_latest[0]
				&& data->tsp_build != firmware_latest[1])) {
			pr_info("force firmware update\n");
			if (mxt_load_fw(NULL, MXT_FW_NAME))
				goto err_config;
			else {
				msleep(MXT_SW_RESET_TIME);
				mxt_make_object_table(data);
			}
		}
#endif
#endif
	} else {
		dev_err(&client->dev, "wrong family_id (0x%x)\n",
			data->family_id);
		goto err_config;
	}
#if 0
	/* Read USER DATA[0] for Enable&Disable to write configuration */
	get_object_info(data, SPT_USERDATA_T38, &size, &obj_address);
	read_mem(data, obj_address + 0, 1, &data->disable_config_write);
#endif

	for (i = 0; tsp_config[i][0] != RESERVED_T255; i++) {
#if FOR_DEBUGGING_TEST_DOWNLOADFW_BIN
	if (data->disable_config_write == 0)
		ret = init_write_config(data, tsp_config[i][0],
							tsp_config[i] + 1);
	else
		ret = 0;
#else
	ret = init_write_config(data, tsp_config[i][0], tsp_config[i] + 1);
#endif
	if (ret) {
		dev_err(&client->dev, "init_write_config fail)\n");
		goto err_config;
	}

	if (tsp_config[i][0] == TOUCH_MULTITOUCHSCREEN_T9) {
		/* Are x and y inverted? */
		if (tsp_config[i][10] & 0x1) {
			data->x_dropbits =
				(!(tsp_config[i][22] & 0xC)) << 1;
			data->y_dropbits =
				(!(tsp_config[i][20] & 0xC)) << 1;
		} else {
			data->x_dropbits =
				(!(tsp_config[i][20] & 0xC)) << 1;
			data->y_dropbits =
				(!(tsp_config[i][22] & 0xC)) << 1;
			}
		}
	}
	ret = mxt_backup(data);
	if (ret)
		goto err_config;

	/* reset the touch IC. */
	ret = mxt_reset(data);
	if (ret)
		goto err_config;

	msleep(MXT_SW_RESET_TIME);

	return 0;

err_config:
	kfree(data->objects);

	return ret;
}

static int __devinit mxt_probe(struct i2c_client *client,
	const struct i2c_device_id *id)
{
	struct mxt224s_platform_data *pdata = client->dev.platform_data;
	struct mxt_data *data;
	struct input_dev *input_dev;
	int ret = 0;
	int i;
	bool ta_status;

	if (!i2c_check_functionality(client->adapter, I2C_FUNC_I2C)) {
		dev_err(&client->dev,
			"i2c_check_functionality fail\n");
		return -EIO;
	}

	dev_info(&client->dev,"%s\n", __func__);

/*	touch_is_pressed = 0;*/
	if (!pdata) {
		dev_err(&client->dev, "missing platform data\n");
		return -ENODEV;
	}

	data = kzalloc(sizeof(struct mxt_data), GFP_KERNEL);
	if (!data) {
		dev_err(&client->dev, "Failed to allocate memory\n");
		return -ENOMEM;
	}

	input_dev = input_allocate_device();
	if (!input_dev) {
		ret = -ENOMEM;
		dev_err(&client->dev, "input device allocation fail\n");
		goto err_alloc_dev;
	}

	data->pdata = pdata;
	data->power_on = pdata->power_on;
	data->power_off = pdata->power_off;
	data->register_cb = pdata->register_cb; 
	data->read_ta_status = pdata->read_ta_status;
	data->client = client;
	data->input_dev = input_dev;
	data->gpio_read_done = pdata->gpio_read_done;
	i2c_set_clientdata(client, data);

	snprintf(data->phys, sizeof(data->phys),
		"%s/input0", dev_name(&client->dev));
	input_dev->name = "sec_touchscreen";
	input_dev->phys = data->phys;
	input_dev->id.bustype = BUS_I2C;
	input_dev->dev.parent = &client->dev;
#ifdef CONFIG_INPUT_ENABLED
	input_dev->open = mxt224s_input_open;
	input_dev->close = mxt224s_input_close;
#endif

	set_bit(EV_SYN, input_dev->evbit);
	set_bit(EV_ABS, input_dev->evbit);
	set_bit(EV_KEY, input_dev->evbit);
	set_bit(MT_TOOL_FINGER, input_dev->keybit);
	set_bit(INPUT_PROP_DIRECT, input_dev->propbit);

	input_mt_init_slots(input_dev, MAX_USING_FINGER_NUM);
	input_set_abs_params(input_dev, ABS_MT_POSITION_X, pdata->min_x,
		pdata->max_x, 0, 0);
	input_set_abs_params(input_dev, ABS_MT_POSITION_Y, pdata->min_y,
		pdata->max_y, 0, 0);
	input_set_abs_params(input_dev, ABS_MT_TOUCH_MAJOR, pdata->min_z,
		pdata->max_z, 0, 0);
	input_set_abs_params(input_dev, ABS_MT_PRESSURE, pdata->min_w,
		pdata->max_w, 0, 0);
#ifdef SUPPORT_SHAPE_TOUCH
	input_set_abs_params(input_dev, ABS_MT_COMPONENT, 0, 255, 0, 0);
	input_set_abs_params(input_dev, ABS_MT_SUMSIZE, 0, 16 * 26, 0, 0);
#endif
	input_set_drvdata(input_dev, data);

	ret = input_register_device(input_dev);
	if (ret) {
		dev_err(&client->dev, "input_register_device fail(%d)\n", ret);
		goto err_reg_dev;
	}

	mutex_init(&data->i2c_lock);

	data->power_on();

	copy_data = data;

	ret = mxt_init_config(data);
	if (ret) {
		dev_err(&client->dev, "chip config initialization fail\n");
		goto err_init_config;
	}

	data->mxt_enabled = true;

	data->callbacks.inform_charger = mxt224s_ta_cb;
	if (data->register_cb)
		data->register_cb(&data->callbacks);

	if (data->read_ta_status) {
		data->read_ta_status(&ta_status);
		dev_info(&client->dev, "ta_status : %d", ta_status);
		mxt_ta_probe(data, ta_status);
	}
	calibrate_chip(data);

	for (i = 0; i < MXT224S_MAX_MT_FINGERS; i++)
		data->fingers[i].state = MXT_STATE_INACTIVE;

	ret = request_threaded_irq(client->irq, NULL, mxt_irq_thread,
		IRQF_TRIGGER_LOW | IRQF_ONESHOT, "mxt_ts", data);
	if (ret < 0) {
		dev_err(&client->dev, "request irq fail\n");
		goto err_reg_irq;
	}

#ifdef CONFIG_HAS_EARLYSUSPEND
	data->early_suspend.level = EARLY_SUSPEND_LEVEL_BLANK_SCREEN + 1;
	data->early_suspend.suspend = mxt_early_suspend;
	data->early_suspend.resume = mxt_late_resume;
	register_early_suspend(&data->early_suspend);
#endif

#if SYSFS
	ret = sysfs_create_group(&client->dev.kobj, &mxt_attr_group);
	if (ret)
		dev_err(&client->dev, "sysfs_create_group()is falled\n");


#if ITDEV
	ret = sysfs_create_group(&client->dev.kobj, &libmaxtouch_attr_group);
	if (ret) {
		dev_err(&client->dev, "Failed to create libmaxtouch sysfs group\n");
	}

	sysfs_bin_attr_init(&mem_access_attr);
	mem_access_attr.attr.name = "mem_access";
	mem_access_attr.attr.mode = S_IRUGO | S_IWUGO;
	mem_access_attr.read = mem_access_read;
	mem_access_attr.write = mem_access_write;
	mem_access_attr.size = 65535;

	if (sysfs_create_bin_file(&client->dev.kobj, &mem_access_attr) < 0) {
		pr_err("Failed to create device file(%s)!\n",
			mem_access_attr.attr.name);
	}
#endif
	pr_info("mxt file sys call!!!\n");

	sec_touchscreen = device_create(sec_class,
		NULL, 0, NULL, "sec_touchscreen");
	dev_set_drvdata(sec_touchscreen, data);
	if (IS_ERR(sec_touchscreen))
		pr_err("Failed to create device(sec_touchscreen)!\n");

	if (device_create_file(sec_touchscreen,
				&dev_attr_tsp_firm_update) < 0)
		pr_err("Failed to create device file(%s)!\n",
			dev_attr_tsp_firm_update.attr.name);

	if (device_create_file(sec_touchscreen,
				&dev_attr_tsp_firm_update_status) < 0)
		pr_err("Failed to create device file(%s)!\n",
			dev_attr_tsp_firm_update_status.attr.name);

	if (device_create_file(sec_touchscreen,
				&dev_attr_tsp_threshold) < 0)
		pr_err("Failed to create device file(%s)!\n",
			dev_attr_tsp_threshold.attr.name);

	if (device_create_file(sec_touchscreen,
				&dev_attr_tsp_firm_version_phone) < 0)
		pr_err("Failed to create device file(%s)!\n",
			dev_attr_tsp_firm_version_phone.attr.name);

	if (device_create_file(sec_touchscreen,
				&dev_attr_tsp_firm_version_panel) < 0)
		pr_err("Failed to create device file(%s)!\n",
			dev_attr_tsp_firm_version_panel.attr.name);

	if (device_create_file(sec_touchscreen,
				&dev_attr_tsp_firm_version_config) < 0)
		pr_err("Failed to create device file(%s)!\n",
			dev_attr_tsp_firm_version_config.attr.name);

	if (device_create_file(sec_touchscreen,
				&dev_attr_mxt_touchtype) < 0)
		pr_err("Failed to create device file(%s)!\n",
			dev_attr_mxt_touchtype.attr.name);

	mxt_noise_test = device_create(sec_class,
		NULL, 0, NULL, "tsp_noise_test");

	if (IS_ERR(mxt_noise_test))
		pr_err("Failed to create device(mxt_noise_test)!\n");

	if (device_create_file(mxt_noise_test, &dev_attr_set_refer0) < 0)
		pr_err("Failed to create device file(%s)!\n",
			dev_attr_set_refer0.attr.name);

	if (device_create_file(mxt_noise_test, &dev_attr_set_delta0) < 0)
		pr_err("Failed to create device file(%s)!\n",
			dev_attr_set_delta0.attr.name);

	if (device_create_file(mxt_noise_test, &dev_attr_set_refer1) < 0)
		pr_err("Failed to create device file(%s)!\n",
			dev_attr_set_refer1.attr.name);

	if (device_create_file(mxt_noise_test, &dev_attr_set_delta1) < 0)
		pr_err("Failed to create device file(%s)!\n",
			dev_attr_set_delta1.attr.name);

	if (device_create_file(mxt_noise_test, &dev_attr_set_refer2) < 0)
		pr_err("Failed to create device file(%s)!\n",
			dev_attr_set_refer2.attr.name);

	if (device_create_file(mxt_noise_test, &dev_attr_set_delta2) < 0)
		pr_err("Failed to create device file(%s)!\n",
			dev_attr_set_delta2.attr.name);

	if (device_create_file(mxt_noise_test, &dev_attr_set_refer3) < 0)
		pr_err("Failed to create device file(%s)!\n",
			dev_attr_set_refer3.attr.name);

	if (device_create_file(mxt_noise_test, &dev_attr_set_delta3) < 0)
		pr_err("Failed to create device file(%s)!\n",
			dev_attr_set_delta3.attr.name);

	if (device_create_file(mxt_noise_test, &dev_attr_set_refer4) < 0)
		pr_err("Failed to create device file(%s)!\n",
			dev_attr_set_refer4.attr.name);

	if (device_create_file(mxt_noise_test, &dev_attr_set_delta4) < 0)
		pr_err("Failed to create device file(%s)!\n",
			dev_attr_set_delta4.attr.name);

	if (device_create_file(mxt_noise_test, &dev_attr_set_all_refer) < 0)
		pr_err("Failed to create device file(%s)!\n",
			dev_attr_set_all_refer.attr.name);

	if (device_create_file(mxt_noise_test, &dev_attr_disp_all_refdata) < 0)
		pr_err("Failed to create device file(%s)!\n",
			dev_attr_disp_all_refdata.attr.name);

	if (device_create_file(mxt_noise_test, &dev_attr_set_all_delta) < 0)
		pr_err("Failed to create device file(%s)!\n",
			dev_attr_set_all_delta.attr.name);

	if (device_create_file(mxt_noise_test,
				&dev_attr_disp_all_deltadata) < 0)
		pr_err("Failed to create device file(%s)!\n",
			dev_attr_disp_all_deltadata.attr.name);

	if (device_create_file(mxt_noise_test, &dev_attr_set_threshold) < 0)
		pr_err("Failed to create device file(%s)!\n",
			dev_attr_set_threshold.attr.name);

	if (device_create_file(mxt_noise_test, &dev_attr_set_firm_version) < 0)
		pr_err("Failed to create device file(%s)!\n",
			dev_attr_set_firm_version.attr.name);

	if (device_create_file(mxt_noise_test, &dev_attr_set_module_off) < 0)
		pr_err("Failed to create device file(%s)!\n",
			dev_attr_set_module_off.attr.name);

	if (device_create_file(mxt_noise_test, &dev_attr_set_module_on) < 0)
		pr_err("Failed to create device file(%s)!\n",
			dev_attr_set_module_on.attr.name);

	if (device_create_file(mxt_noise_test, &dev_attr_x_line) < 0)
		pr_err("Failed to create device file(%s)!\n",
			dev_attr_x_line.attr.name);

	if (device_create_file(mxt_noise_test, &dev_attr_y_line) < 0)
		pr_err("Failed to create device file(%s)!\n",
			dev_attr_y_line.attr.name);

	if (device_create_file(mxt_noise_test,
				&dev_attr_find_delta_channel) < 0)
		pr_err("Failed to create device file(%s)!\n",
			dev_attr_find_delta_channel.attr.name);

	if (device_create_file(mxt_noise_test,
				&dev_attr_set_tsp_name) < 0)
		pr_err("Failed to create device file(%s)!\n",
			dev_attr_set_tsp_name.attr.name);
#endif

	return 0;

err_reg_irq:
err_init_config:
	input_unregister_device(input_dev);
err_reg_dev:
	data->power_off();
	input_free_device(input_dev);
err_alloc_dev:
	kfree(data);
	return ret;
}

#ifdef CONFIG_READ_FROM_FILE
#if DOWNLOAD_CONFIG
static struct object_t *mxt_get_object(struct mxt_data *data, u8 type)
{
	struct object_t *object;
	int i;

	for (i = 0; i < data->objects_len; i++) {
		object = data->objects + i;
		if (object->object_type == type)
			return object;
	}

	dev_err(&data->client->dev, "Invalid object type T%u\n", type);
	return NULL;
}

static int mxt_write_reg(struct i2c_client *client, u16 reg, u8 val)
{
	u8 buf[3];

	buf[0] = reg & 0xff;
	buf[1] = (reg >> 8) & 0xff;
	buf[2] = val;

	printk("[TSP] mxt_write_reg %d %d\n", reg, val);

	if (i2c_master_send(client, buf, 3) != 3) {
		dev_err(&client->dev, "%s: i2c send failed\n", __func__);
		return -EIO;
	}

	return 0;
}

int mxt_download_config(struct mxt_data *data, const char *fn)
{
	struct device *dev = &data->client->dev;
	struct mxt_info cfg_info;
	struct object_t *object;
#ifdef CONFIG_READ_FROM_SDCARD
	struct firmware *cfg = NULL;
#else
	const struct firmware *cfg = NULL;
#endif
	int ret;
	int offset;
	loff_t pos;
	int i;
	unsigned long info_crc, config_crc;
	unsigned int type, instance, size, object_size, instance_size;
	u8 val;
	u16 reg;

#ifdef CONFIG_READ_FROM_SDCARD
	struct file *filp;
	long cfg_size = 0;
	unsigned char *cfg_data;
	mm_segment_t oldfs;
	
	oldfs = get_fs();
	set_fs(get_ds());	
	
	printk("[TSP] mxt_download_config %s\n", fn);
	
	filp = filp_open(fn, O_RDONLY, 0);
	if (IS_ERR(filp)) {
		pr_err("file open error:%d\n", (s32)filp);
		return -1;
	}
	
	cfg_size = filp->f_path.dentry->d_inode->i_size;
	pr_info("Size of the Cfg file : %ld(bytes)\n", cfg_size);

	cfg_data = kmalloc(cfg_size, GFP_KERNEL);
	memset(cfg_data, 0, cfg_size);

	pos = 0;
	ret = vfs_read(filp, (char __user *)cfg_data, cfg_size, &pos);

	if (ret != cfg_size) {
		pr_err("Failed to read Cfg file %s (ret = %d)\n",
			fn, ret);
		kfree(cfg_data);
		filp_close(filp, current->files);
		return -1;
	}

	filp_close(filp, current->files);

	set_fs(oldfs);
	
	/*firmware struct*/
	cfg = kzalloc(sizeof(struct firmware), GFP_KERNEL);
	cfg->data = cfg_data;
	cfg->size = cfg_size;		
#else
	ret = request_firmware(&cfg, fn, dev);
	if (ret < 0) {
		dev_err(dev, "Failure to request config file %s\n", fn);
		return 0;
	}
#endif		

	if (strncmp(cfg->data, MXT_CFG_MAGIC, strlen(MXT_CFG_MAGIC))) {
		dev_err(dev, "Unrecognised config file\n");
		ret = -EINVAL;
		goto release;
	}

	pos = strlen(MXT_CFG_MAGIC);

	/* Load information block and check */
	for (i = 0; i < sizeof(struct mxt_info); i++) {
		ret = sscanf(cfg->data + pos, "%hhx%n",
			     (unsigned char *)&cfg_info + i,
			     &offset);
		if (ret != 1) {
			dev_err(dev, "Bad format\n");
			ret = -EINVAL;
		}

		pos += offset;
	}

	if (cfg_info.family_id != data->family_id) {
		dev_err(dev, "Family ID mismatch! %x %x\n", cfg_info.family_id, data->family_id);
		ret = -EINVAL;
	}

	if (cfg_info.variant_id != data->tsp_variant) {
		dev_err(dev, "Variant ID mismatch! %x %x\n", cfg_info.variant_id, data->tsp_variant);
		ret = -EINVAL;
	}

	if (cfg_info.version != data->tsp_version)
		dev_err(dev, "Warning: version mismatch! %x %x\n", cfg_info.version, data->tsp_version);

	if (cfg_info.build != data->tsp_build)
		dev_err(dev, "Warning: build num mismatch! %x %x\n", cfg_info.build, data->tsp_build);

	ret = sscanf(cfg->data + pos, "%lx%n", &info_crc, &offset);
	if (ret != 1) {
		dev_err(dev, "Bad format\n");
		ret = -EINVAL;
	}
	pos += offset;

	/* Check config CRC */
	ret = sscanf(cfg->data + pos, "%lx%n", &config_crc, &offset);
	if (ret != 1) {
		dev_err(dev, "Bad format\n");
		ret = -EINVAL;
	}
	pos += offset;

	while (pos < cfg->size) {
		/* Read type, instance, length */
		ret = sscanf(cfg->data + pos, "%x %x %x%n",
			     &type, &instance, &size, &offset);
		if (ret == 0) {
			/* EOF */
			ret = 1;
			goto release;
		} else if (ret < 0) {
			dev_err(dev, "Bad format\n");
			ret = -EINVAL;
			goto release;
		}
		pos += offset;

		object = mxt_get_object(data, type);
		if (!object) {
			ret = -EINVAL;
			goto release;
		}

		object_size = object->size+1;
		instance_size = object->instances+1;

		if (size > object_size) {
			dev_err(dev, "Object length exceeded!\n");
			ret = -EINVAL;
			goto release;
		}

		if (instance >= instance_size) {
			dev_err(dev, "Object instances exceeded!\n");
			ret = -EINVAL;
			goto release;
		}

		reg = object->i2c_address + object_size * instance;

		for (i = 0; i < size; i++) {
			ret = sscanf(cfg->data + pos, "%hhx%n",
				     &val,
				     &offset);
			if (ret != 1) {
				dev_err(dev, "Bad format\n");
				ret = -EINVAL;
				goto release;
			}

			ret = mxt_write_reg(data->client, reg + i, val);
			if (ret)
				goto release;

			pos += offset;
		}

		/* If firmware is upgraded, new bytes may be added to end of
		 * objects. It is generally forward compatible to zero these
		 * bytes - previous behaviour will be retained. However
		 * this does invalidate the CRC and will force a config
		 * download every time until the configuration is updated */
		if (size < object_size) {
			dev_info(dev, "Warning: zeroing %d byte(s) in T%d\n",
				 object->size - size, type);

			for (i = size + 1; i < object_size; i++) {
				ret = mxt_write_reg(data->client, reg + i, 0);
				if (ret)
					goto release;
			}
		}
	}

release:
	
#ifdef CONFIG_READ_FROM_SDCARD
	kfree(cfg);	
	kfree(cfg_data);
#else
	release_firmware(cfg);
#endif	
	return ret;
}

#endif
#endif

static int __devexit mxt_remove(struct i2c_client *client)
{
	struct mxt_data *data = i2c_get_clientdata(client);

#ifdef CONFIG_HAS_EARLYSUSPEND
	unregister_early_suspend(&data->early_suspend);
#endif
	free_irq(client->irq, data);
	kfree(data->objects);
	kfree(data->rid_map);
	gpio_free(data->gpio_read_done);
	data->power_off();
	input_unregister_device(data->input_dev);
	kfree(data);

	return 0;
}

static void mxt224s_shutdown(struct i2c_client *client)
{
	struct mxt_data *data = i2c_get_clientdata(client);

	if (data->mxt_enabled) {
		disable_irq(client->irq);
		data->power_off();
		mxt_release_all(data);
	}
}

#ifdef CONFIG_PM
static int mxt224s_suspend(struct device *dev)
{
	struct i2c_client *client = to_i2c_client(dev);
	struct mxt_data *data = i2c_get_clientdata(client);
	int i;
	int count = 0;

	data->mxt_enabled = 0;
/*	touch_is_pressed = 0;*/

	disable_irq(client->irq);
	data->power_off();
	mxt_release_all(data);

	return 0;
}

static int mxt224s_resume(struct device *dev)
{
	int ret = 0;
	bool ta_status = 0;
	struct i2c_client *client = to_i2c_client(dev);
	struct mxt_data *data = i2c_get_clientdata(client);

	ret = data->power_on();

	data->mxt_enabled = 1;

	if (data->read_ta_status) {
		data->read_ta_status(&ta_status);
		dev_info(&client->dev, "ta_status is %d\n", ta_status);
		mxt_ta_probe(data, ta_status);
	}
	calibrate_chip(data);

	enable_irq(client->irq);

	return ret;
}
#endif

#ifdef CONFIG_HAS_EARLYSUSPEND
static void mxt_early_suspend(struct early_suspend *h)
{
	struct mxt_data *data = container_of(h, struct mxt_data,
								early_suspend);

	if (data->mxt_enabled == 1) {
		pr_info("%s\n", __func__);
		data->mxt_enabled = 0;
/*		touch_is_pressed = 0;*/
		mxt224s_suspend(&data->client->dev);
	} else
		pr_err("%s. but touch already off\n", __func__);
}

static void mxt_late_resume(struct early_suspend *h)
{
	struct mxt_data *data = container_of(h, struct mxt_data,
								early_suspend);

	if (data->mxt_enabled == 0) {
		pr_info("%s\n", __func__);
		mxt224s_resume(&data->client->dev);

		data->mxt_enabled = 1;

		treat_median_error_status = 0;
	} else
		pr_err("%s. but touch already on\n", __func__);
}
#endif

#ifdef CONFIG_INPUT_ENABLED
static int mxt224s_input_open(struct input_dev *dev)
{
	struct mxt_data *data = input_get_drvdata(dev);
	int ret;

	ret = mxt224s_resume(&data->client->dev);

	return ret;
}

static void mxt224s_input_close(struct input_dev *dev)
{
	struct mxt_data *data = input_get_drvdata(dev);

	mxt224s_suspend(&data->client->dev);
}
#endif

static struct i2c_device_id mxt_idtable[] = {
	{MXT224S_DEV_NAME, 0},
	{},
};

MODULE_DEVICE_TABLE(i2c, mxt_idtable);

#if defined(CONFIG_PM) && !defined(CONFIG_HAS_EARLYSUSPEND) &&\
	!defined(CONFIG_INPUT_ENABLED)
static const struct dev_pm_ops mxt224s_pm_ops = {
	.suspend = mxt224s_suspend,
	.resume = mxt224s_resume,
};
#endif

static struct i2c_driver mxt224s_i2c_driver = {
	.id_table = mxt_idtable,
	.probe = mxt_probe,
	.remove = __devexit_p(mxt_remove),
	.shutdown = mxt224s_shutdown,
	.driver = {
		.name	= MXT224S_DEV_NAME,
#if defined(CONFIG_PM) && !defined(CONFIG_HAS_EARLYSUSPEND) &&\
	!defined(CONFIG_INPUT_ENABLED)
		.pm	= &mxt224s_pm_ops,
#endif
	},
};

static int __init mxt224s_init(void)
{
	return i2c_add_driver(&mxt224s_i2c_driver);
}

static void __exit mxt224s_exit(void)
{
	i2c_del_driver(&mxt224s_i2c_driver);
}
module_init(mxt224s_init);
module_exit(mxt224s_exit);

MODULE_DESCRIPTION("mxt224s touch driver");
MODULE_AUTHOR("junkyeong.kim<jk0430.kim@samsung.com>");
MODULE_LICENSE("GPL");

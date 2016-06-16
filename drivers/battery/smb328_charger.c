/*
 *  smb328_charger.c
 *  Samsung SMB328 Charger Driver
 *
 *  Copyright (C) 2012 Samsung Electronics
 *
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 */

#include <linux/battery/sec_charger.h>

static int smb328_i2c_write(struct i2c_client *client,
				int reg, u8 *buf)
{
	int ret;
	ret = i2c_smbus_write_i2c_block_data(client, reg, 1, buf);
	if (ret < 0)
		pr_err("%s: Error(%d)\n", __func__, ret);
	return ret;
}

static int smb328_i2c_read(struct i2c_client *client,
				int reg, u8 *buf)
{
	int ret;
	ret = i2c_smbus_read_i2c_block_data(client, reg, 1, buf);
	if (ret < 0)
		pr_err("%s: Error(%d)\n", __func__, ret);
	return ret;
}

static void smb328_i2c_write_array(struct i2c_client *client,
				u8 *buf, int size)
{
	int i;
	for (i = 0; i < size; i += 3)
		smb328_i2c_write(client, (u8) (*(buf + i)), (buf + i) + 1);
}

static void smb328_set_command(struct i2c_client *client,
				int reg, int datum)
{
	int val;
	u8 data = 0;
	val = smb328_i2c_read(client, reg, &data);
	if (val >= 0) {
		pr_debug("%s : reg(0x%02x): 0x%02x", __func__, reg, data);
		if (data != datum) {
			data = datum;
			if (smb328_i2c_write(client, reg, &data) < 0)
				pr_err("%s : error!\n", __func__);
			val = smb328_i2c_read(client, reg, &data);
			if (val >= 0)
				pr_debug(" => 0x%02x\n", data);
		}
	}
}

static void smb328_test_read(struct i2c_client *client)
{
	u8 data = 0;
	u32 addr = 0;
	for (addr = 0; addr < 0x0c; addr++) {
		smb328_i2c_read(client, addr, &data);
		pr_debug("smb328 addr : 0x%02x data : 0x%02x\n", addr, data);
	}
	for (addr = 0x31; addr < 0x3D; addr++) {
		smb328_i2c_read(client, addr, &data);
		pr_debug("smb328 addr : 0x%02x data : 0x%02x\n", addr, data);
	}
}

static void smb328_read_regs(struct i2c_client *client, char *str)
{
	int i;
	u8 data = 0;
	u32 addr = 0;

	for (addr = 0; addr < 0x0c; addr++) {
		smb328_i2c_read(client, addr, &data);
		sprintf(str+strlen(str), "0x%x, ", data);
	}

	/* "#" considered as new line in application */
	sprintf(str+strlen(str), "#");

	for (addr = 0x31; addr < 0x3d; addr++) {
		smb328_i2c_read(client, addr, &data);
		sprintf(str+strlen(str), "0x%x, ", data);
	}
}

static int smb328_get_charging_status(struct i2c_client *client)
{
	int status = POWER_SUPPLY_STATUS_UNKNOWN;
	u8 data_a = 0;
	u8 data_b = 0;
	u8 data_c = 0;
	u8 data_curr = 0;
	smb328_i2c_read(client, SMB328A_BATTERY_CHARGING_STATUS_A, &data_a);
	pr_debug("%s : charging status A(0x%02x)\n", __func__, data_a);
	smb328_i2c_read(client, SMB328A_BATTERY_CHARGING_STATUS_B, &data_b);
	pr_debug("%s : charging status B(0x%02x)\n", __func__, data_b);
	smb328_i2c_read(client, SMB328A_BATTERY_CHARGING_STATUS_C, &data_c);
	pr_debug("%s : charging status C(0x%02x)\n", __func__, data_c);
	smb328_i2c_read(client, SMB328A_CURRENT_TERMINATION, &data_curr);
	pr_debug("%s : 0x01 : charging currnet (0x%02x)\n", __func__, data_curr);
	smb328_i2c_read(client, SMB328A_INPUT_AND_CHARGE_CURRENTS, &data_curr);
	pr_debug("%s : 0x00 : charging currnet (0x%02x)\n", __func__, data_curr);

	/* At least one charge cycle terminated,
	 * Charge current < Termination Current
	 */
	if ((data_c & 0xc0) == 0xc0) {
		/* top-off by full charging */
		status = POWER_SUPPLY_STATUS_FULL;
		goto charging_status_end;
	}

	/* Is enabled ? */
	if (data_c & 0x01) {

		/* check for 0x30 : 'safety timer' (0b01 or 0b10) or
		 * 'waiting to begin charging' (0b11)
		 * check for 0x06 : no charging (0b00)
		 */
		/* not charging */
		if ((data_c & 0x30) || !(data_c & 0x06)) {
			status = POWER_SUPPLY_STATUS_NOT_CHARGING;
			goto charging_status_end;
		} else {
			status = POWER_SUPPLY_STATUS_CHARGING;
			goto charging_status_end;
		}
	} else
		status = POWER_SUPPLY_STATUS_DISCHARGING;
charging_status_end:return (int)status;
}

static int smb328_get_charging_health(struct i2c_client *client)
{
	int health = POWER_SUPPLY_HEALTH_GOOD;
	u8 data_a = 0;
	u8 data_b = 0;
	u8 data_c = 0;
	smb328_i2c_read(client, SMB328A_BATTERY_CHARGING_STATUS_A, &data_a);
	pr_debug("%s : charging status A(0x%02x)\n", __func__, data_a);
	smb328_i2c_read(client, SMB328A_BATTERY_CHARGING_STATUS_B, &data_b);
	pr_debug("%s : charging status B(0x%02x)\n", __func__, data_b);
	smb328_i2c_read(client, SMB328A_BATTERY_CHARGING_STATUS_C, &data_c);
	pr_debug("%s : charging status C(0x%02x)\n", __func__, data_c);

	/* Is enabled ? */
	if (data_c & 0x01) {
		if (!(data_a & 0x02))	/* Input current is NOT OK */
			health = POWER_SUPPLY_HEALTH_OVERVOLTAGE;
	}
	return (int)health;
}

static void smb328a_allow_volatile_writes(struct i2c_client *client)
{
	int val, reg;
	u8 data;
	reg = SMB328A_COMMAND;
	val = smb328_i2c_read(client, reg, &data);
	if ((val >= 0) && !(data & 0x80)) {
		pr_debug("%s : reg(0x%02x): 0x%02x", __func__, reg, data);
		data |= (0x1 << 7);
		if (smb328_i2c_write(client, reg, &data) < 0)
			pr_err("%s : error!\n", __func__);
		val = smb328_i2c_read(client, reg, &data);
		if (val >= 0) {
			data = (u8) data;
			pr_debug(" => 0x%02x\n", data);
		}
	}
}

static void smb328a_charger_function_control(struct i2c_client *client)
{
	struct sec_charger_info *charger = i2c_get_clientdata(client);
	u8 data;

	pr_debug("########### %s ##########\n", __func__);

	if (charger->charging_current < 0) {
		pr_debug("%s : OTG is activated. Ignore command!\n", __func__);
		return;
	}
	smb328a_allow_volatile_writes(client);
	if (charger->cable_type == POWER_SUPPLY_TYPE_BATTERY) {
		smb328_set_command(client, SMB328A_FUNCTION_CONTROL_B, 0x0);

		/* turn off charger */
		smb328_set_command(client, SMB328A_COMMAND, 0x98);
	} else {
		if (charger->cable_type == POWER_SUPPLY_TYPE_MAINS)
			data = 0xb7;	/* fast 1000mA, termination 200mA */
		else if (charger->cable_type == POWER_SUPPLY_TYPE_MISC)
			data = 0x57;	/* fast 700mA, termination 200mA */
		else
			data = 0x17;	/* fast 500mA, termination 200mA */

		/* this can be changed with top-off setting */
		smb328_set_command(client,
				   SMB328A_INPUT_AND_CHARGE_CURRENTS, data);
		if (charger->cable_type == POWER_SUPPLY_TYPE_MAINS)
			data = 0xb0;	/* input 1A, threshold 4.25V */
		else if (charger->cable_type == POWER_SUPPLY_TYPE_MISC)
			data = 0x50;	/* input 700mA, threshold 4.25V */
		else
			data = 0x10;	/* input 450mA, threshold 4.25V */

		/* AICL enable */
		smb328_set_command(client, SMB328A_CURRENT_TERMINATION, data);

		/* 4.2V float voltage */
		smb328_set_command(client, SMB328A_FLOAT_VOLTAGE, 0xca);

		/* 0xda : top-off by ADC */
		/* 0x9a : top-off by charger */
		smb328_set_command(client, SMB328A_FUNCTION_CONTROL_A1, 0xda);
		smb328_set_command(client, SMB328A_FUNCTION_CONTROL_A2, 0x4c);
		smb328_set_command(client, SMB328A_FUNCTION_CONTROL_B, 0x0);
		if (charger->cable_type == POWER_SUPPLY_TYPE_MAINS)
			data = 0xf5;
		else if (charger->cable_type == POWER_SUPPLY_TYPE_MISC)
			data = 0xf5;
		else
			data = 0xd5;
		smb328_set_command(client, SMB328A_OTG_PWR_AND_LDO_CONTROL,
				   data);

		/* this can be changed with top-off setting */
		smb328_set_command(client,
				   SMB328A_VARIOUS_CONTROL_FUNCTION_A, 0xf6);
		smb328_set_command(client, SMB328A_CELL_TEMPERATURE_MONITOR,
				   0x0);
		smb328_set_command(client, SMB328A_INTERRUPT_SIGNAL_SELECTION,
				   0x0);

		/* turn on charger */
		if (charger->cable_type == POWER_SUPPLY_TYPE_MAINS)
			data = 0x8c;
		else if (charger->cable_type == POWER_SUPPLY_TYPE_MISC)
			data = 0x88;
		else
			data = 0x88;	/* USB */
		smb328_set_command(client, SMB328A_COMMAND, data);
	}
}

static void smb328a_charger_otg_control(struct i2c_client *client)
{
	struct sec_charger_info *charger = i2c_get_clientdata(client);

	pr_debug("################## %s ################\n", __func__);

	smb328a_allow_volatile_writes(client);

	if (charger->cable_type == POWER_SUPPLY_TYPE_BATTERY) {
		/* turn off charger */
		smb328_set_command(client, SMB328A_FUNCTION_CONTROL_B, 0x0c);

		/* delay for reset of charger */
		mdelay(150);

		/* turn off OTG */
		smb328_set_command(client, SMB328A_COMMAND, 0x98);
	} else {
		smb328_set_command(client, SMB328A_FUNCTION_CONTROL_B, 0x0);

		/* delay for reset of charger */
		mdelay(150);

		/* turn on OTG */
		smb328_set_command(client, SMB328A_COMMAND, 0x9a);
	}
}

static void smb328_irq_disable(struct i2c_client *client)
{
	int ret;
	u8 data;

	pr_debug("########### %s ##########\n", __func__);

	smb328_i2c_read(client, 0x31, &data);
	data |= 0x80;
	smb328_i2c_write(client, 0x31, &data);

	smb328_i2c_read(client, 0x04, &data);
	data &= 0xFE;
	smb328_i2c_write(client, 0x04, &data);

	smb328_i2c_read(client, 0x31, &data);
	data &= 0x7F;
	smb328_i2c_write(client, 0x31, &data);
}

bool sec_hal_chg_init(struct i2c_client *client)
{
	smb328_irq_disable(client);
	smb328_test_read(client);
	return true;
}

bool sec_hal_chg_suspend(struct i2c_client *client)
{
	return true;
}

bool sec_hal_chg_resume(struct i2c_client *client)
{
	return true;
}

bool sec_hal_chg_get_property(struct i2c_client *client,
			      enum power_supply_property psp,
			      union power_supply_propval *val)
{
	struct sec_charger_info *charger = i2c_get_clientdata(client);
	u8 data;
	switch (psp) {
	case POWER_SUPPLY_PROP_STATUS:
		val->intval = smb328_get_charging_status(client);
		break;
	case POWER_SUPPLY_PROP_CHARGE_TYPE:
		val->intval = charger->cable_type;
		break;
	case POWER_SUPPLY_PROP_HEALTH:
		val->intval = smb328_get_charging_health(client);
		break;
	case POWER_SUPPLY_PROP_CURRENT_NOW:
		smb328_irq_disable(client);
		if (charger->charging_current) {
			smb328_i2c_read(client,
					SMB328A_INPUT_AND_CHARGE_CURRENTS,
					&data);
			switch (data >> 5) {
			case 0:
				val->intval = 450;
				break;
			case 1:
				val->intval = 600;
				break;
			case 2:
				val->intval = 700;
				break;
			case 3:
				val->intval = 800;
				break;
			case 4:
				val->intval = 900;
				break;
			case 5:
				val->intval = 1000;
				break;
			case 6:
				val->intval = 1100;
				break;
			case 7:
				val->intval = 1200;
				break;
			}
		} else
			val->intval = 0;
		break;
	default:
		return false;
	}
	return true;
}

bool sec_hal_chg_set_property(struct i2c_client *client,
			      enum power_supply_property psp,
			      const union power_supply_propval *val)
{
	struct sec_charger_info *charger = i2c_get_clientdata(client);
	int cable_type;

	switch (psp) {
	/* val->intval : type */
	case POWER_SUPPLY_PROP_ONLINE:
	/* val->intval : charging current */
	case POWER_SUPPLY_PROP_CURRENT_NOW:
		if (charger->charging_current < 0)
			smb328a_charger_otg_control(client);
		else if (charger->charging_current > 0)
			smb328a_charger_function_control(client);
		else {
			smb328a_charger_function_control(client);
			smb328a_charger_otg_control(client);
		}
		smb328_test_read(client);
		break;
	default:
		return false;
	}
	return true;
}

ssize_t sec_hal_chg_show_attrs(struct device *dev,
				const ptrdiff_t offset, char *buf)
{
	struct power_supply *psy = dev_get_drvdata(dev);
	struct sec_charger_info *chg =
		container_of(psy, struct sec_charger_info, psy_chg);
	int i = 0;
	char *str = NULL;

	switch (offset) {
/*	case CHG_REG: */
/*		break; */
	case CHG_DATA:
		i += scnprintf(buf + i, PAGE_SIZE - i, "%02x\n",
			chg->reg_data);
		break;
	case CHG_REGS:
		str = kzalloc(sizeof(char)*1024, GFP_KERNEL);
		if (!str)
			return -ENOMEM;

		smb328_read_regs(chg->client, str);
		i += scnprintf(buf + i, PAGE_SIZE - i, "%s\n",
			str);

		kfree(str);
		break;
	default:
		i = -EINVAL;
	}

	return i;
}

ssize_t sec_hal_chg_store_attrs(struct device *dev,
				const ptrdiff_t offset,
				const char *buf, size_t count)
{
	struct power_supply *psy = dev_get_drvdata(dev);
	struct sec_charger_info *chg =
		container_of(psy, struct sec_charger_info, psy_chg);
	int ret = 0;
	int x = 0;
	u8 data = 0;

	switch (offset) {
	case CHG_REG:
		if (sscanf(buf, "%x\n", &x) == 1) {
			chg->reg_addr = x;
			smb328_i2c_read(chg->client,
				chg->reg_addr, &data);
			chg->reg_data = data;
			pr_debug("%s: (read) addr = 0x%x, data = 0x%x\n",
				__func__, chg->reg_addr, chg->reg_data);
		}
		ret = count;
		break;
	case CHG_DATA:
		if (sscanf(buf, "%x\n", &x) == 1) {
			data = (u8)x;
			pr_debug("%s: (write) addr = 0x%x, data = 0x%x\n",
				__func__, chg->reg_addr, data);
			smb328_i2c_write(chg->client,
				chg->reg_addr, &data);
		}
		ret = count;
		break;
	default:
		ret = -EINVAL;
	}

	return ret;
}


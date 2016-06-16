/*
 *  smb358_charger.c
 *  Samsung SMB358 Charger Driver
 *
 *  Copyright (C) 2012 Samsung Electronics
 *
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 */

#include <linux/battery/sec_charger.h>
#include <linux/debugfs.h>
#include <linux/seq_file.h>

static int smb358_i2c_write(struct i2c_client *client,
				int reg, u8 *buf)
{
	int ret;
	ret = i2c_smbus_write_i2c_block_data(client, reg, 1, buf);
	if (ret < 0)
		dev_err(&client->dev, "%s: Error(%d)\n", __func__, ret);
	return ret;
}

static int smb358_i2c_read(struct i2c_client *client,
				int reg, u8 *buf)
{
	int ret;
	ret = i2c_smbus_read_i2c_block_data(client, reg, 1, buf);
	if (ret < 0)
		dev_err(&client->dev, "%s: Error(%d)\n", __func__, ret);
	return ret;
}

static void smb358_i2c_write_array(struct i2c_client *client,
				u8 *buf, int size)
{
	int i;
	for (i = 0; i < size; i += 3)
		smb358_i2c_write(client, (u8) (*(buf + i)), (buf + i) + 1);
}

static void smb358_set_writable(struct i2c_client *client, int writable)
{
        u8 reg_data;

        smb358_i2c_read(client, SMB358_COMMAND_A, &reg_data);

        if (writable)
                reg_data |= CMD_A_ALLOW_WRITE;
        else
                reg_data &= ~CMD_A_ALLOW_WRITE;

        smb358_i2c_write(client, SMB358_COMMAND_A, &reg_data);
}

static void smb358_set_command(struct i2c_client *client,
				int reg, u8 datum)
{
	int val;
	u8 after_data;

	if (smb358_i2c_write(client, reg, &datum) < 0)
		dev_err(&client->dev,
			"%s : error!\n", __func__);

	msleep(20);
	val = smb358_i2c_read(client, reg, &after_data);
	if (val >= 0)
		dev_dbg(&client->dev,
			"%s : reg(0x%02x) 0x%02x => 0x%02x\n",
			__func__, reg, datum, after_data);
	else
		dev_err(&client->dev, "%s : error!\n", __func__);
}

static void smb358_test_read(struct i2c_client *client)
{
	u8 data = 0;
	u32 addr = 0;
	for (addr = 0; addr <= 0x0E; addr++) {
		smb358_i2c_read(client, addr, &data);
		dev_info(&client->dev,
			"%s : smb358 addr : 0x%02x data : 0x%02x\n",
			__func__, addr, data);
	}
	for (addr = 0x30; addr <= 0x3f; addr++) {
		smb358_i2c_read(client, addr, &data);
		dev_info(&client->dev,
			"%s : smb358 addr : 0x%02x data : 0x%02x\n",
			__func__, addr, data);
	}
}

static int smb358_get_battery_present(struct i2c_client *client)
{
	u8 irq_data;

	smb358_set_writable(client, SMB358_ENABLE_WRITE);

	smb358_i2c_read(client, SMB358_INTERRUPT_STATUS_B, &irq_data);
	dev_info(&client->dev, "%s : 0x%x\n", __func__, irq_data);
	irq_data = irq_data & 0x10;

	smb358_set_writable(client, SMB358_DISABLE_WRITE);
	return !irq_data;
}

static void smb358_read_regs(struct i2c_client *client, char *str)
{
	u8 data = 0;
	u32 addr = 0;

	for (addr = 0; addr <= 0x0E; addr++) {
		smb358_i2c_read(client, addr, &data);
		sprintf(str+strlen(str), "0x%x, ", data);
	}

	/* "#" considered as new line in application */
	sprintf(str+strlen(str), "#");

	for (addr = 0x30; addr <= 0x3f; addr++) {
		smb358_i2c_read(client, addr, &data);
		sprintf(str+strlen(str), "0x%x, ", data);
	}
}


static int smb358_get_charging_status(struct i2c_client *client)
{
	int status = POWER_SUPPLY_STATUS_UNKNOWN;
	u8 data_a = 0;
	u8 data_b = 0;
	u8 data_c = 0;
	u8 data_d = 0;
	u8 data_e = 0;
	u8 irq_c = 0;

	smb358_i2c_read(client, SMB358_STATUS_A, &data_a);
	dev_dbg(&client->dev,
		"%s : charger status A(0x%02x)\n", __func__, data_a);
	smb358_i2c_read(client, SMB358_STATUS_B, &data_b);
	dev_dbg(&client->dev,
		"%s : charger status B(0x%02x)\n", __func__, data_b);
	smb358_i2c_read(client, SMB358_STATUS_C, &data_c);
	dev_dbg(&client->dev,
		"%s : charger status C(0x%02x)\n", __func__, data_c);
	smb358_i2c_read(client, SMB358_STATUS_D, &data_d);
	dev_dbg(&client->dev,
		"%s : charger status D(0x%02x)\n", __func__, data_d);
	smb358_i2c_read(client, SMB358_STATUS_E, &data_e);
	dev_dbg(&client->dev,
		"%s : charger status E(0x%02x)\n", __func__, data_e);
	smb358_i2c_read(client, SMB358_INTERRUPT_STATUS_C, &irq_c);
	dev_dbg(&client->dev,
		"%s : charger irq status C(0x%02x)\n", __func__, irq_c);
	/* At least one charge cycle terminated,
	 * Charge current < Termination Current
	 */
	if ((data_c & 0x20) || (irq_c & 0x01)) {
		/* top-off by full charging */
		status = POWER_SUPPLY_STATUS_FULL;
		goto charging_status_end;
	}

	/* Is enabled ? */
	if (data_c & 0x01) {
		/* check for 0x06 : no charging (0b00) */
		/* not charging */
		if (!(data_c & 0x06)) {
			status = POWER_SUPPLY_STATUS_NOT_CHARGING;
			goto charging_status_end;
		} else {
			status = POWER_SUPPLY_STATUS_CHARGING;
			goto charging_status_end;
		}
	} else
		status = POWER_SUPPLY_STATUS_DISCHARGING;

charging_status_end:
	return (int)status;
}

static int smb358_get_charging_health(struct i2c_client *client)
{
	int health = POWER_SUPPLY_HEALTH_GOOD;
	u8 data_a = 0;
	u8 data_b = 0;
	u8 data_c = 0;
	u8 data_d = 0;
	u8 data_e = 0;
	u8 comm_a = 0;

	smb358_i2c_read(client, SMB358_STATUS_A, &data_a);
	smb358_i2c_read(client, SMB358_STATUS_B, &data_b);
	smb358_i2c_read(client, SMB358_STATUS_C, &data_c);
	smb358_i2c_read(client, SMB358_STATUS_D, &data_d);
	smb358_i2c_read(client, SMB358_STATUS_E, &data_e);
	smb358_i2c_read(client, SMB358_COMMAND_A, &comm_a);
	dev_info(&client->dev,
		 "%s : charger status A(0x%02x) B(0x%02x) C(0x%02x) "
		 "D(0x%02x) E(0x%02x)\n",__func__, data_a, data_b,
		 data_c, data_d, data_e);

	/* Is enabled ? */
	if ((data_c & 0x01) || (comm_a & 0x02)) {
		smb358_i2c_read(client, SMB358_INTERRUPT_STATUS_E, &data_e);
		dev_dbg(&client->dev,
			"%s : charger intterupt status E(0x%02x)\n",
			__func__, data_e);

		if (data_e & 0x01)
			health = POWER_SUPPLY_HEALTH_UNDERVOLTAGE;
		else if (data_e & 0x04)
			health = POWER_SUPPLY_HEALTH_OVERVOLTAGE;
	}

	return (int)health;
}

static u8 smb358_set_float_voltage(struct i2c_client *client, int float_voltage)
{
	u8 reg_data, float_data;

	if (float_voltage < 3500)
		float_data = 0x0;
	else if(float_voltage <= 4340)
		float_data = (float_voltage - 3500) / 20;
	else if(float_voltage == 4350)
		float_data = 0x2B; /* (4340 -3500)/20 + 1 */
	else if(float_voltage <= 4500)
		float_data = (float_voltage - 3500) / 20 + 1;
	else
		float_data = 0x33;

	smb358_i2c_read(client, SMB358_FLOAT_VOLTAGE, &reg_data);
	reg_data &= ~CFG_FLOAT_VOLTAGE_MASK;
	reg_data |= float_data;

	smb358_i2c_write(client, SMB358_FLOAT_VOLTAGE, &reg_data);

	return reg_data;
}

static u8 smb358_set_input_current_limit(struct i2c_client *client,
				int input_current)
{
	u8 curr_data, reg_data;

	if (input_current <= 300)
		curr_data = 0x00;
	else if (input_current <= 500)
		curr_data = 0x01;
	else if (input_current <= 700)
		curr_data = 0x02;
	else if (input_current <= 1000)
		curr_data = 0x03;
	else if (input_current <= 1200)
		curr_data = 0x04;
	else if (input_current <= 1500)
		curr_data = 0x05;
	else if (input_current <= 1800)
		curr_data = 0x06;
	else if (input_current <= 2000)
		curr_data = 0x07;
	else
		curr_data = 0x08;	/* No input current limit */

	smb358_i2c_read(client, SMB358_INPUT_CURRENTLIMIT, &reg_data);
	reg_data &= ~CFG_INPUT_CURRENT_MASK;
	reg_data |= curr_data << CFG_INPUT_CURRENT_SHIFT;

	smb358_i2c_write(client, SMB358_INPUT_CURRENTLIMIT, &reg_data);

	pr_info("%s: set current limit : 0x%x\n", __func__, reg_data);

	return reg_data;

}

static u8 smb358_set_termination_current_limit(struct i2c_client *client,
			int termination_current)
{
	u8 reg_data, term_data;

	if (termination_current <= 30)
		term_data = 0x00;
	else if (termination_current <= 40)
		term_data = 0x01;
	else if (termination_current <= 60)
		term_data = 0x02;
	else if (termination_current <= 80)
		term_data = 0x03;
	else if (termination_current <= 100)
		term_data = 0x04;
	else if (termination_current <= 125)
		term_data = 0x05;
	else if (termination_current <= 150)
		term_data = 0x06;
	else if (termination_current <= 200)
		term_data = 0x07;
	else
		term_data = 0x00;

	/* Charge completion termination current */
	smb358_i2c_read(client, SMB358_CHARGE_CURRENT, &reg_data);
	reg_data &= ~CFG_TERMINATION_CURRENT_MASK;
	reg_data |= term_data << CFG_TERMINATION_CURRENT_SHIFT;

	smb358_i2c_write(client, SMB358_CHARGE_CURRENT, &reg_data);

	return reg_data;
}

static u8 smb358_set_fast_charging_current(struct i2c_client *client,
			int fast_charging_current)
{
	u8 reg_data, chg_data;

	if (fast_charging_current <= 200)
		chg_data = 0x00;
	else if (fast_charging_current <= 450)
		chg_data = 0x01;
	else if (fast_charging_current <= 600)
		chg_data = 0x02;
	else if (fast_charging_current <= 900)
		chg_data = 0x03;
	else if (fast_charging_current <= 1300)
		chg_data = 0x04;
	else if (fast_charging_current <= 1500)
		chg_data = 0x05;
	else if (fast_charging_current <= 1800)
		chg_data = 0x06;
	else if (fast_charging_current <= 2000)
		chg_data = 0x07;
	else
		chg_data = 0x00;

	smb358_i2c_read(client, SMB358_CHARGE_CURRENT, &reg_data);
	reg_data &= ~CFG_CHARGE_CURRENT_MASK;
	reg_data |= chg_data << CFG_CHARGE_CURRENT_SHIFT;

	smb358_i2c_write(client, SMB358_CHARGE_CURRENT, &reg_data);

	pr_info("%s: Charge Current : 0x%x\n", __func__, reg_data);

	return reg_data;
}

static u8 smb358_set_charge_enable(struct i2c_client *client, int enable)
{
	u8 chg_en;
	u8 reg_data;

	reg_data = 0x00;

	smb358_i2c_write(client, SMB358_PIN_ENABLE_CONTROL, &reg_data);

	smb358_i2c_read(client, SMB358_COMMAND_A, &chg_en);
	if (enable)
		chg_en |= CMD_CHARGE_EN;
	else
		chg_en &= ~CMD_CHARGE_EN;

	chg_en |= 0x1 << 6;

	smb358_i2c_write(client, SMB358_COMMAND_A, &chg_en);

	return chg_en;
}

static void smb358_charger_function_control(
				struct i2c_client *client)
{
	struct sec_charger_info *charger = i2c_get_clientdata(client);
	u8 reg_data, charge_mode;

	if (charger->charging_current < 0) {
		dev_dbg(&client->dev,
			"%s : OTG is activated. Ignore command!\n", __func__);
		return;
	}

	smb358_set_writable(client, SMB358_ENABLE_WRITE);

	if (charger->cable_type == POWER_SUPPLY_TYPE_BATTERY) {
		/* Charger Disabled */
		reg_data = 0x0;
		smb358_i2c_write(client, SMB358_COMMAND_B, &reg_data);
		smb358_set_charge_enable(client, 0);

	} else {
		pr_info("%s: Input : %d, Charge : %d\n", __func__,
			charger->pdata->charging_current[charger->cable_type].input_current_limit,
			charger->charging_current);

		/* Not allowed to end a charge cycle & auto-recharge disable */
		smb358_i2c_read(client, SMB358_CHARGE_CONTROL, &reg_data);
		reg_data |= (CFG_CURRNT_TERMNATION | CFG_AUTO_RECHARGE_DISABLE);
		/* APSD disable */
		reg_data &= ~CFG_APSD_ENABLE;
		/* Battery missing detection check via THERM I/O */
		reg_data &= ~CFG_BATT_PRESENT_MASK;
		reg_data |= CFG_BATT_PRESENT_ENABLE;
		smb358_i2c_write(client, SMB358_CHARGE_CONTROL, &reg_data);
		dev_info(&client->dev, "%s : reg_data: %d\n", __func__, reg_data);

		/* enable BMD via THERM I/O */
		smb358_i2c_read(client, SMB358_THERM_CONTROL_A, &reg_data);
		reg_data &= ~0x10;
		smb358_i2c_write(client, SMB358_THERM_CONTROL_A, &reg_data);

		/* charging is not suspend when THERM I/O enabled*/
		smb358_i2c_read(client, SMB358_SYSOK_USB30_SELECTION, &reg_data);
		reg_data |= 0x04;
		smb358_i2c_write(client, SMB358_SYSOK_USB30_SELECTION, &reg_data);

		smb358_i2c_read(client, SMB358_STATUS_INTERRUPT, &reg_data);
		reg_data |= CFG_ENABLE_IRQ_STATUS;
		smb358_i2c_write(client, SMB358_STATUS_INTERRUPT, &reg_data);
		dev_info(&client->dev, "%s : irq_data: %d\n", __func__, reg_data);

		/* Low-bat/SYSOK/BATGOOD Voltage threshold : 2.6v*/
		smb358_i2c_read(client, SMB358_OTHER_CONTROL_A, &reg_data);
		reg_data &= ~0x0F;
		reg_data |= 0x1;
		smb358_i2c_write(client, SMB358_OTHER_CONTROL_A, &reg_data);

		/* AICL enable */
		smb358_i2c_read(client, SMB358_VARIOUS_FUNCTIONS, &reg_data);
		reg_data |= CFG_AICL_ENABLE;
		smb358_i2c_write(client, SMB358_VARIOUS_FUNCTIONS, &reg_data);

		/* set float voltage */
		smb358_set_float_voltage(client,
			charger->pdata->chg_float_voltage);

		/* Set termination current */
		smb358_set_termination_current_limit(client,
			charger->pdata->charging_current[charger->
			cable_type].full_check_current_1st);

		/* Set Input current limit */
		smb358_set_input_current_limit(client,
			charger->pdata->charging_current
			[charger->cable_type].input_current_limit);

		/* Set fast charge current */
		smb358_set_fast_charging_current(client,
			charger->charging_current);

		/* SET USB5/1, AC/USB Mode */
		charge_mode = (charger->cable_type == POWER_SUPPLY_TYPE_MAINS) ||
			(charger->cable_type == POWER_SUPPLY_TYPE_UARTOFF) ?
			0x3 : 0x2;

		smb358_i2c_read(client, SMB358_COMMAND_B, &reg_data);
		reg_data &= ~0x03;
		reg_data |= charge_mode;
		smb358_i2c_write(client, SMB358_COMMAND_B, &reg_data);

		smb358_set_charge_enable(client, 1);
	}

	smb358_set_writable(client, SMB358_DISABLE_WRITE);

	smb358_test_read(client);
}

static void smb358_charger_otg_control(struct i2c_client *client, int enable)
{
	u8 otg_reg, reg_data;

	smb358_set_writable(client, SMB358_ENABLE_WRITE);

	smb358_i2c_read(client, SMB358_COMMAND_A, &otg_reg);
	otg_reg = enable ? otg_reg | CFG_OTG_ENABLE :
			otg_reg & (~CFG_OTG_ENABLE);

	smb358_i2c_write(client, SMB358_COMMAND_A, &otg_reg);

	reg_data = 0x0;
	smb358_i2c_write(client, SMB358_COMMAND_B, &reg_data);

	/* Change "OTG output current limit" to 500mA */
	smb358_i2c_read(client, SMB358_OTG_TLIM_THERM_CONTROL, &reg_data);
	reg_data &= ~0x0C;
	reg_data |= 0x1 << 2;
	smb358_i2c_write(client, SMB358_OTG_TLIM_THERM_CONTROL, &reg_data);

	smb358_set_writable(client, SMB358_DISABLE_WRITE);

}
static int smb358_debugfs_show(struct seq_file *s, void *data)
{
	struct sec_charger_info *charger = s->private;
	u8 reg;
	u8 reg_data;

	seq_printf(s, "SMB CHARGER IC :\n");
	seq_printf(s, "==================\n");
	for (reg = 0x00; reg <= 0x0E; reg++) {
		smb358_i2c_read(charger->client, reg, &reg_data);
		seq_printf(s, "0x%02x:\t0x%02x\n", reg, reg_data);
	}

	for (reg = 0x30; reg <= 0x3F; reg++) {
		smb358_i2c_read(charger->client, reg, &reg_data);
		seq_printf(s, "0x%02x:\t0x%02x\n", reg, reg_data);
	}

	seq_printf(s, "\n");
	return 0;
}

static int smb358_debugfs_open(struct inode *inode, struct file *file)
{
	return single_open(file, smb358_debugfs_show, inode->i_private);
}

static const struct file_operations smb358_debugfs_fops = {
	.open           = smb358_debugfs_open,
	.read           = seq_read,
	.llseek         = seq_lseek,
	.release        = single_release,
};

bool sec_hal_chg_init(struct i2c_client *client)
{
	struct sec_charger_info *charger = i2c_get_clientdata(client);

	dev_info(&client->dev,
		"%s: SMB358 Charger init(Start)!!\n", __func__);

	smb358_test_read(client);
	(void) debugfs_create_file("smb358_regs",
		S_IRUGO, NULL, (void *)charger, &smb358_debugfs_fops);

	return true;
}

bool sec_hal_chg_suspend(struct i2c_client *client)
{
	dev_info(&client->dev,
                "%s: CHARGER - SMB358(suspend mode)!!\n", __func__);

	return true;
}

bool sec_hal_chg_resume(struct i2c_client *client)
{
	dev_info(&client->dev,
                "%s: CHARGER - SMB358(resume mode)!!\n", __func__);

	return true;
}

bool sec_hal_chg_shutdown(struct i2c_client *client)
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
		val->intval = smb358_get_charging_status(client);
		break;
	case POWER_SUPPLY_PROP_HEALTH:
		val->intval = smb358_get_charging_health(client);
		break;
	case POWER_SUPPLY_PROP_PRESENT:
		val->intval = smb358_get_battery_present(client); /* temporary vf check*/
		smb358_test_read(client);
		break;
	case POWER_SUPPLY_PROP_CHARGE_TYPE:
		if (!charger->is_charging)
			val->intval = POWER_SUPPLY_CHARGE_TYPE_NONE;
		else
			val->intval = POWER_SUPPLY_CHARGE_TYPE_FAST;
		break;
	case POWER_SUPPLY_PROP_CURRENT_NOW:
		if (charger->charging_current) {
			smb358_i2c_read(client, SMB358_STATUS_B, &data);
			if (data & 0x20)
				switch ((data & 0x18) >> 3) {
				case 0:
					val->intval = 100;
					break;
				case 1:
					val->intval = 150;
					break;
				case 2:
					val->intval = 200;
					break;
				case 3:
					val->intval = 250;
					break;
				}
			else
				switch (data & 0x07) {
				case 0:
					val->intval = 100;
					break;
				case 1:
					val->intval = 200;
					break;
				case 2:
					val->intval = 450;
					break;
				case 3:
					val->intval = 600;
					break;
				case 4:
					val->intval = 900;
					break;
				case 5:
					val->intval = 1300;
					break;
				case 6:
					val->intval = 1500;
					break;
				case 7:
					val->intval = 1800;
					break;
				}
		} else
			val->intval = 0;
		dev_dbg(&client->dev,
			"%s : set-current(%dmA), current now(%dmA)\n",
			__func__, charger->charging_current, val->intval);
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

	switch (psp) {
	/* val->intval : type */
	case POWER_SUPPLY_PROP_ONLINE:
	/* val->intval : charging current */
	case POWER_SUPPLY_PROP_CURRENT_NOW:
		if (charger->charging_current < 0)
			smb358_charger_otg_control(client, 1);
		else if (charger->charging_current > 0)
			smb358_charger_function_control(client);
		else {
			smb358_charger_function_control(client);
			smb358_charger_otg_control(client, 0);
		}
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
	case CHG_DATA:
		i += scnprintf(buf + i, PAGE_SIZE - i, "%x\n",
			chg->reg_data);
		break;
	case CHG_REGS:
		str = kzalloc(sizeof(char)*1024, GFP_KERNEL);
		if (!str)
			return -ENOMEM;

		smb358_read_regs(chg->client, str);
		i += scnprintf(buf + i, PAGE_SIZE - i, "%s\n",
			str);

		kfree(str);
		break;
	default:
		i = -EINVAL;
		break;
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
			smb358_i2c_read(chg->client,
				chg->reg_addr, &data);
			chg->reg_data = data;
			dev_dbg(dev, "%s: (read) addr = 0x%x, data = 0x%x\n",
				__func__, chg->reg_addr, chg->reg_data);
			ret = count;
		}
		break;
	case CHG_DATA:
		if (sscanf(buf, "%x\n", &x) == 1) {
			data = (u8)x;
			dev_dbg(dev, "%s: (write) addr = 0x%x, data = 0x%x\n",
				__func__, chg->reg_addr, data);
			smb358_i2c_write(chg->client,
				chg->reg_addr, &data);
			ret = count;
		}
		break;
	default:
		ret = -EINVAL;
		break;
	}

	return ret;
}

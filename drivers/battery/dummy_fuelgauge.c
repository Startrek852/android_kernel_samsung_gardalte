/*
 *  dummy_fuelgauge.c
 *  Samsung Dummy Fuel Gauge Driver
 *
 *  Copyright (C) 2012 Samsung Electronics
 *
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 */

#include <linux/battery/sec_fuelgauge.h>
static int dummy_i2c_write(struct i2c_client *client, int reg, u8 *buf)
{
	int ret;
	ret = i2c_smbus_write_i2c_block_data(client, reg, 2, buf);
	if (ret < 0)
		dev_err(&client->dev, "%s: Error(%d)\n", __func__, ret);
	return ret;
}

static int dummy_i2c_read(struct i2c_client *client, int reg, u8 *buf)
{
	int ret;
	ret = i2c_smbus_read_i2c_block_data(client, reg, 2, buf);
	if (ret < 0)
		dev_err(&client->dev, "%s: Error(%d)\n", __func__, ret);
	return ret;
}

static void dummy_i2c_write_array(struct i2c_client *client, u8 *buf,
				    int size)
{
	int i;
	for (i = 0; i < size; i += 3)
		dummy_i2c_write(client, (u8) (*(buf + i)), (buf + i) + 1);
}

bool sec_hal_fg_init(struct i2c_client *client)
{
	/* initialize fuel gauge registers */
	return true;
}

bool sec_hal_fg_reset(struct i2c_client *client)
{
	return true;
}

bool sec_hal_fg_suspend(struct i2c_client *client)
{
	return true;
}

bool sec_hal_fg_resume(struct i2c_client *client)
{
	return true;
}

bool sec_hal_fg_fuelalert_init(struct i2c_client *client, int soc)
{
	return true;
}

bool sec_hal_fg_is_fuelalerted(struct i2c_client *client)
{
	return false;
}

bool sec_hal_fg_full_charged(struct i2c_client *client)
{
	return true;
}

bool sec_hal_fg_fuelalert_process(void *irq_data, bool is_fuel_alerted)
{
	return true;
}

bool sec_hal_fg_get_property(struct i2c_client *client,
			       enum power_supply_property psp,
			       union power_supply_propval *val)
{
	switch (psp) {

	/* Cell voltage (VCELL, mV) */
	case POWER_SUPPLY_PROP_VOLTAGE_NOW:
		val->intval = 3900;
		break;

	/* Additional Voltage Information (mV) */
	case POWER_SUPPLY_PROP_VOLTAGE_AVG:
		switch (val->intval) {
		case SEC_BATTEY_VOLTAGE_AVERAGE:
			break;
		}
		break;

	/* Current (mA) */
	case POWER_SUPPLY_PROP_CURRENT_NOW:
		break;

	/* Average Current (mA) */
	case POWER_SUPPLY_PROP_CURRENT_AVG:
		break;

	/* SOC (%) */
	case POWER_SUPPLY_PROP_CAPACITY:
		val->intval = 500;
		break;

	/* Battery Temperature */
	case POWER_SUPPLY_PROP_TEMP:

	/* Target Temperature */
	case POWER_SUPPLY_PROP_TEMP_AMBIENT:
		break;
	default:
		return false;
	}
	return true;
}

bool sec_hal_fg_set_property(struct i2c_client *client,
			       enum power_supply_property psp,
			       const union power_supply_propval *val)
{
	switch (psp) {

	case POWER_SUPPLY_PROP_ONLINE:
	/* Battery Temperature */
	case POWER_SUPPLY_PROP_TEMP:

	/* Target Temperature */
	case POWER_SUPPLY_PROP_TEMP_AMBIENT:
		break;
	default:
		return false;
	}
	return true;
}









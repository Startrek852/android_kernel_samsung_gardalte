/*
 * SAMSUNG NFC Controller
 *
 * Copyright (C) 2013 Samsung Electronics Co.Ltd
 * Author: Woonki Lee <woonki84.lee@samsung.com>
 *
 * This program is free software; you can redistribute it and/or modify it
 * under  the terms of  the GNU General  Public License as published by the
 * Free Software Foundation;  either version 2 of the License, or (at your
 * option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#include <linux/wait.h>
#include <linux/delay.h>

#ifdef CONFIG_SEC_NFC_I2C_SB
#include <linux/interrupt.h>
#include <linux/poll.h>
#include <linux/sched.h>
#include <linux/i2c.h>
#include <linux/regulator/consumer.h>
#endif

#include <linux/kernel.h>
#include <linux/platform_device.h>
#include <linux/mutex.h>
#include <linux/module.h>
#include <linux/miscdevice.h>
#include <linux/gpio.h>
#include <linux/slab.h>
#include <linux/fs.h>
#include <asm/uaccess.h>
#include <linux/nfc/sec_nfc_sb.h>

#ifdef CONFIG_SEC_NFC_I2C_SB
enum sec_nfc_irq {
	SEC_NFC_NONE,
	SEC_NFC_PAYLOAD,
	SEC_NFC_HEADER,
	SEC_NFC_IRQ_MAX,
};
#endif

struct sec_nfc_info {
	struct miscdevice miscdev;
	struct mutex mutex;
	enum sec_nfc_state state;
	struct device *dev;
	struct sec_nfc_platform_data *pdata;

#ifdef CONFIG_SEC_NFC_I2C_SB
	struct i2c_client *i2c_dev;
	struct mutex read_mutex;
	enum sec_nfc_irq read_irq;
	enum sec_nfc_irq read_times;
	wait_queue_head_t read_wait;
	size_t buflen;
	u8 *buf;
#endif
};

#define LOG_OUT 0

#ifdef CONFIG_SEC_NFC_I2C_SB
static irqreturn_t sec_nfc_irq_thread_fn(int irq, void *dev_id)
{
	struct sec_nfc_info *info = dev_id;

	dev_dbg(info->dev, "IRQ\n");
#if LOG_OUT
	pr_info("[NFC] I2C Interrupt is occurred!");
#endif
	mutex_lock(&info->read_mutex);
	info->read_irq = info->read_times;
	mutex_unlock(&info->read_mutex);

	wake_up_interruptible(&info->read_wait);

	return IRQ_HANDLED;
}
#endif

static int sec_nfc_set_state(struct sec_nfc_info *info,
					enum sec_nfc_state state)
{
	struct sec_nfc_platform_data *pdata = info->pdata;

	/* intfo lock is aleady getten before calling this function */
	info->state = state;

	gpio_set_value(pdata->ven, 0);
	gpio_set_value(pdata->firm, 0);
	msleep(SEC_NFC_VEN_WAIT_TIME);

	pr_info("[NFC] MORE SET VEN/FIRM LOW during : %d ms\n",
		SEC_NFC_VEN_WAIT_TIME);

	if (state == SEC_NFC_ST_FIRM) {
		gpio_set_value(pdata->firm, 1);
		gpio_set_value(pdata->ven, 0);
		msleep(SEC_NFC_VEN_WAIT_TIME);
		pr_info("[NFC] SET VEN LOW during : %d ms\n",
			SEC_NFC_VEN_WAIT_TIME);
	}

	if (state != SEC_NFC_ST_OFF)
		gpio_set_value(pdata->ven, 1);

	msleep(SEC_NFC_VEN_WAIT_TIME);
	pr_info("[NFC] SET VEN HIGH during : %d ms\n", SEC_NFC_VEN_WAIT_TIME);

	dev_dbg(info->dev, "Power state is : %d\n", state);

	return 0;
}

#ifdef CONFIG_SEC_NFC_I2C_SB
static int sec_nfc_reset(struct sec_nfc_info *info)
{
	dev_dbg(info->dev, "i2c failed. return resatrt to M/W\n");

	sec_nfc_set_state(info, SEC_NFC_ST_NORM);

	return -ERESTART;
}

static ssize_t sec_nfc_read(struct file *file, char __user *buf,
			   size_t count, loff_t *ppos)
{
	struct sec_nfc_info *info = container_of(file->private_data,
						struct sec_nfc_info, miscdev);
	enum sec_nfc_irq irq;
	int ret = 0;

	/* dev_dbg(info->dev, "%s: info: %p, count: %zu\n",
		__func__, info, count);
	*/
#if LOG_OUT
	pr_info("[NFC] %s: info: %p, count: %zu\n", __func__, info, count);
#endif
	mutex_lock(&info->mutex);

	if (info->state == SEC_NFC_ST_OFF) {
		/* dev_err(info->dev, "sec_nfc is not enabled\n"); */
		pr_err("[NFC] sec_nfc is not enabled\n");
		ret = -ENODEV;
		goto out;
	}

	mutex_lock(&info->read_mutex);
	irq = info->read_irq;
	mutex_unlock(&info->read_mutex);
	if (irq == SEC_NFC_NONE) {
		if (file->f_flags & O_NONBLOCK) {
			/* dev_err(info->dev, "it is nonblock\n"); */
			pr_err("[NFC] it is nonblock\n");
		}
			ret = -EAGAIN;
			goto out;
	}

	/* i2c recv */
	if (count > info->buflen)
		count = info->buflen;

	if (count < SEC_NFC_MSG_MIN_SIZE || count > SEC_NFC_MSG_MAX_SIZE) {
		/* dev_err(info->dev, "user required wrong size\n"); */
		pr_err("[NFC] user required wrong size\n");
		ret = -EINVAL;
		goto out;
	}

	mutex_lock(&info->read_mutex);
	ret = i2c_master_recv(info->i2c_dev, info->buf, count);
	/* dev_dbg(info->dev, "recv size : %d\n", ret); */
#if LOG_OUT
	pr_info("[NFC] recv size : %d\n", ret);
#endif
	if (ret == -EREMOTEIO) {
		ret = sec_nfc_reset(info);
		goto read_error;
	} else if (ret != count) {
		pr_err("[NFC] read failed: return: %d count: %d\n",
			ret, count);
		/* ret = -EREMOTEIO; */
		goto read_error;
	}

	if (info->read_irq > SEC_NFC_NONE)
		info->read_irq--;

	mutex_unlock(&info->read_mutex);

	if (copy_to_user(buf, info->buf, ret)) {
		/* dev_err(info->dev, "copy failed to user\n"); */
		pr_err("[NFC] copy failed to user\n");
		ret = -EFAULT;
	}

	goto out;

read_error:
	info->read_irq = SEC_NFC_NONE;
	mutex_unlock(&info->read_mutex);
out:
	mutex_unlock(&info->mutex);
#if LOG_OUT
	pr_info("[NFC] read function finish\n");
#endif
	return ret;
}

static ssize_t sec_nfc_write(struct file *file, const char __user *buf,
			   size_t count, loff_t *ppos)
{
	struct sec_nfc_info *info = container_of(file->private_data,
						struct sec_nfc_info, miscdev);
	int ret = 0;

	/* dev_dbg(info->dev, "%s: info: %p, count %zu\n",
		__func__, info, count);
	*/
#if LOG_OUT
	pr_info("[NFC] %s: info: %p, count %zu\n", __func__, info, count);
#endif
	mutex_lock(&info->mutex);

	if (info->state == SEC_NFC_ST_OFF) {
		/* dev_err(info->dev, "sec_nfc is not enabled\n"); */
		pr_err("[NFC] sec_nfc is not enabled\n");
		ret = -ENODEV;
		goto out;
	}

	if (count > info->buflen)
		count = info->buflen;

	if (count < SEC_NFC_MSG_MIN_SIZE || count > SEC_NFC_MSG_MAX_SIZE) {
		/* dev_err(info->dev, "user required wrong size\n"); */
		pr_err("[NFC] user required wrong size\n");
		ret = -EINVAL;
		goto out;
	}

	if (copy_from_user(info->buf, buf, count)) {
		/* dev_err(info->dev, "copy failed from user\n"); */
		pr_err("[NFC] copy failed from user\n");
		ret = -EFAULT;
		goto out;
	}

	//usleep_range(6000, 10000);
	ret = i2c_master_send(info->i2c_dev, info->buf, count);

	if (ret == -EREMOTEIO) {
		ret = sec_nfc_reset(info);
		goto out;
	}

	if (ret != count) {
		/* dev_err(info->dev, "send failed: return: %d count: %d\n",
			ret, count);
		*/
		pr_err("[NFC] send failed: return: %d count: %d\n",
				ret, count);
		ret = -EREMOTEIO;
	}

out:
	mutex_unlock(&info->mutex);
#if LOG_OUT
	pr_info("[NFC] Write success!!!");
#endif
	return ret;
}
#endif

#ifdef CONFIG_SEC_NFC_I2C_SB
static unsigned int sec_nfc_poll(struct file *file, poll_table *wait)
{
	struct sec_nfc_info *info = container_of(file->private_data,
						struct sec_nfc_info, miscdev);
	int ret = 0;

	dev_dbg(info->dev, "%s: info: %p\n", __func__, info);

	mutex_lock(&info->mutex);

	if (info->state == SEC_NFC_ST_OFF) {
		dev_err(info->dev, "sec_nfc is not enabled\n");
		ret = -ENODEV;
		goto out;
	}

	poll_wait(file, &info->read_wait, wait);

	mutex_lock(&info->read_mutex);
	if (info->read_irq == info->read_times)
		ret = (POLLIN | POLLRDNORM);
	mutex_unlock(&info->read_mutex);

out:
	mutex_unlock(&info->mutex);

	return ret;
}
#endif

static long sec_nfc_ioctl(struct file *file, unsigned int cmd,
							unsigned long arg)
{
	struct sec_nfc_info *info = container_of(file->private_data,
						struct sec_nfc_info, miscdev);
	unsigned int mode = (unsigned int)arg;
	int ret = 0;

	dev_dbg(info->dev, "%s: info: %p, cmd: 0x%x\n",
			__func__, info, cmd);

	mutex_lock(&info->mutex);

	switch (cmd) {
	case SEC_NFC_SET_MODE:
		dev_dbg(info->dev, "%s: SEC_NFC_SET_MODE\n", __func__);

		if (info->state == mode)
			break;

		if (mode >= SEC_NFC_ST_COUNT) {
			dev_err(info->dev, "wrong state (%d)\n", mode);
			ret = -EFAULT;
			break;
		}

		ret = sec_nfc_set_state(info, mode);
		if (ret < 0) {
			dev_err(info->dev, "enable failed\n");
			break;
		}

		break;

#ifdef	CONFIG_SEC_NFC_I2C_SB
	case SEC_NFC_SET_READTIMES:
		dev_dbg(info->dev, "%s: SEC_NFC_SET_READTIMES\n", __func__);
		if (mode < SEC_NFC_NONE || mode > SEC_NFC_IRQ_MAX) {
			dev_err(info->dev, "unvalid arguments");
			ret = info->read_times;
		}
		else {
			mutex_lock(&info->read_mutex);
			info->read_times = (enum sec_nfc_irq) mode;
			mutex_unlock(&info->read_mutex);
		}
		break;
#endif
	default:
		dev_err(info->dev, "Unknow ioctl 0x%x\n", cmd);
		ret = -ENOIOCTLCMD;
		break;
	}

	mutex_unlock(&info->mutex);

	return ret;
}

static int sec_nfc_open(struct inode *inode, struct file *file)
{
	struct sec_nfc_info *info = container_of(file->private_data,
						struct sec_nfc_info, miscdev);
	int ret = 0;

	dev_dbg(info->dev, "%s: info : %p" , __func__, info);

	mutex_lock(&info->mutex);
	if (info->state != SEC_NFC_ST_OFF) {
		dev_err(info->dev, "sec_nfc is busy\n");
		ret = -EBUSY;
		goto out;
	}

#ifdef CONFIG_SEC_NFC_I2C_SB
	mutex_lock(&info->read_mutex);
	info->read_irq = SEC_NFC_NONE;
	mutex_unlock(&info->read_mutex);
#endif

	ret = sec_nfc_set_state(info, SEC_NFC_ST_NORM);
out:
	mutex_unlock(&info->mutex);
	return ret;
}

static int sec_nfc_close(struct inode *inode, struct file *file)
{
	struct sec_nfc_info *info = container_of(file->private_data,
						struct sec_nfc_info, miscdev);

	dev_dbg(info->dev, "%s: info : %p" , __func__, info);

	mutex_lock(&info->mutex);
	sec_nfc_set_state(info, SEC_NFC_ST_OFF);
	mutex_unlock(&info->mutex);

	return 0;
}

static const struct file_operations sec_nfc_fops = {
	.owner		= THIS_MODULE,
#ifdef CONFIG_SEC_NFC_I2C_SB
	.read		= sec_nfc_read,
	.write		= sec_nfc_write,
	.poll		= sec_nfc_poll,
#endif
	.open		= sec_nfc_open,
	.release	= sec_nfc_close,
	.unlocked_ioctl	= sec_nfc_ioctl,
};

#ifdef CONFIG_PM
static int sec_nfc_suspend(struct device *dev)
{
#ifdef CONFIG_SEC_NFC_I2C_SB
	struct i2c_client *client = to_i2c_client(dev);
	struct sec_nfc_info *info = i2c_get_clientdata(client);
#else
	struct platform_device *pdev = to_platform_device(dev);
	struct sec_nfc_info *info = platform_get_drvdata(pdev);
#endif
	struct sec_nfc_platform_data *pdata = dev->platform_data;

	int ret = 0;

	mutex_lock(&info->mutex);

	if (info->state == SEC_NFC_ST_FIRM)
		ret = -EPERM;

	mutex_unlock(&info->mutex);

	pdata->cfg_gpio();
	return ret;
}

static int sec_nfc_resume(struct device *dev)
{
	struct sec_nfc_platform_data *pdata = dev->platform_data;
	pdata->cfg_gpio();
	return 0;
}

static SIMPLE_DEV_PM_OPS(sec_nfc_pm_ops, sec_nfc_suspend, sec_nfc_resume);
#endif

#ifdef CONFIG_SEC_NFC_I2C_SB
#define REGULATOR_TVDD_MIN	3300000
#define REGULATOR_TVDD_MAX	3300000
static struct regulator *vdd_reg;

static int __devinit sec_nfc_probe(struct i2c_client *client,
		const struct i2c_device_id *id)
{
	struct device *dev = &client->dev;
#else
static int __devinit sec_nfc_probe(struct platform_device *pdev)
{
	struct device *dev = &pdev->dev;
#endif
	struct sec_nfc_info *info;
	struct sec_nfc_platform_data *pdata = dev->platform_data;
	int ret = 0;

	if (!pdata) {
		dev_err(dev, "No platform data\n");
		ret = -ENOMEM;
		goto err_pdata;
	}

	info = kzalloc(sizeof(struct sec_nfc_info), GFP_KERNEL);
	if (!info) {
		dev_err(dev, "failed to allocate memory for sec_nfc_info\n");
		ret = -ENOMEM;
		goto err_info_alloc;
	}
	info->dev = dev;
	info->pdata = pdata;
	info->state = SEC_NFC_ST_OFF;

	mutex_init(&info->mutex);
	dev_set_drvdata(dev, info);

#ifdef CONFIG_SEC_NFC_I2C_SB
	info->buflen = SEC_NFC_MAX_BUFFER_SIZE;
	info->buf = kzalloc(SEC_NFC_MAX_BUFFER_SIZE, GFP_KERNEL);
	if (!info->buf) {
		dev_err(dev,
			"failed to allocate memory for sec_nfc_info->buf\n");
		ret = -ENOMEM;
		goto err_buf_alloc;
	}

	/* pdata->cfg_gpio(); */

	ret = gpio_request(pdata->irq, "nfc_int");
	if (ret) {
		dev_err(dev, "[NFC] GPIO request is failed to register IRQ\n");
		goto err_irq_req;
	}

	info->i2c_dev = client;
	info->read_times = SEC_NFC_IRQ_MAX - 1;
	info->read_irq = SEC_NFC_NONE;
	mutex_init(&info->read_mutex);
	init_waitqueue_head(&info->read_wait);
	i2c_set_clientdata(client, info);

	dev_info(dev, "%s : requesting IRQ %d\n", __func__,
		 client->irq);

//	ret = request_threaded_irq(pdata->irq, NULL, sec_nfc_irq_thread_fn,
	ret = request_threaded_irq(client->irq, NULL, sec_nfc_irq_thread_fn,
			IRQF_TRIGGER_RISING | IRQF_ONESHOT, SEC_NFC_DRIVER_NAME,
			info);
	if (ret < 0) {
		dev_err(dev, "failed to register IRQ handler\n");
		goto err_irq_req;
	}
#endif

	info->miscdev.minor = MISC_DYNAMIC_MINOR;
	info->miscdev.name = SEC_NFC_DRIVER_NAME;
	info->miscdev.fops = &sec_nfc_fops;
	info->miscdev.parent = dev;

	ret = misc_register(&info->miscdev);

	if (ret < 0) {
		dev_err(dev, "failed to register Device\n");
		goto err_dev_reg;
	}

	ret = gpio_request(pdata->ven, "nfc_ven");
	if (ret) {
		dev_err(dev, "failed to get gpio ven\n");
		goto err_gpio_ven;
	}

	ret = gpio_request(pdata->firm, "nfc_firm");
	if (ret) {
		dev_err(dev, "failed to get gpio firm\n");
		goto err_gpio_firm;
	}

	gpio_direction_output(pdata->ven, 0);
	gpio_direction_output(pdata->firm, 0);

	if (info->pdata->power_onoff)
		info->pdata->power_onoff(1);
	else
		pr_info("%s : no power control function\n", __func__);

	dev_dbg(dev, "%s: info: %p, pdata %p\n", __func__, info, pdata);

	return 0;

err_gpio_firm:
	gpio_free(pdata->firm);
err_gpio_ven:
	gpio_free(pdata->ven);
err_dev_reg:
#ifdef CONFIG_SEC_NFC_I2C_SB
err_irq_req:
	gpio_free(pdata->irq);
err_buf_alloc:
#endif
err_info_alloc:
	kfree(info);
err_pdata:
	return ret;
}

#ifdef CONFIG_SEC_NFC_I2C_SB
static int __devexit sec_nfc_remove(struct i2c_client *client)
{
	struct sec_nfc_info *info = i2c_get_clientdata(client);
	struct sec_nfc_platform_data *pdata = client->dev.platform_data;
#else
static int __devexit sec_nfc_remove(struct platform_device *pdev)
{
	struct sec_nfc_info *info = dev_get_drvdata(&pdev->dev);
	struct sec_nfc_platform_data *pdata = pdev->dev.platform_data;
#endif

	dev_dbg(info->dev, "%s\n", __func__);

	misc_deregister(&info->miscdev);

	if (info->state != SEC_NFC_ST_OFF) {
		gpio_set_value(pdata->firm, 0);
		gpio_set_value(pdata->ven, 0);
	}

	gpio_free(pdata->firm);
	gpio_free(pdata->ven);

#ifdef CONFIG_SEC_NFC_I2C_SB
	free_irq(pdata->irq, info);
#endif

	kfree(info);

	return 0;
}

#ifdef CONFIG_SEC_NFC_I2C_SB
static struct i2c_device_id sec_nfc_id_table[] = {
#else	/* CONFIG_SEC_NFC_I2C */
static struct platform_device_id sec_nfc_id_table[] = {
#endif
	{ SEC_NFC_DRIVER_NAME, 0 },
	{ }
};

#ifdef CONFIG_SEC_NFC_I2C_SB
MODULE_DEVICE_TABLE(i2c, sec_nfc_id_table);
static struct i2c_driver sec_nfc_driver = {
#else
MODULE_DEVICE_TABLE(platform, sec_nfc_id_table);
static struct platform_driver sec_nfc_driver = {
#endif
	.probe = sec_nfc_probe,
	.id_table = sec_nfc_id_table,
	.remove = sec_nfc_remove,
	.driver = {
		.name = SEC_NFC_DRIVER_NAME,
#ifdef CONFIG_PM
		.pm = &sec_nfc_pm_ops,
#endif
	},
};

static int __init sec_nfc_init(void)
{
#ifdef CONFIG_SEC_NFC_I2C_SB
	return i2c_add_driver(&sec_nfc_driver);
#else
	return platform_driver_register(&sec_nfc_driver);
#endif
}

static void __exit sec_nfc_exit(void)
{
#ifdef CONFIG_SEC_NFC_I2C_SB
	i2c_del_driver(&sec_nfc_driver);
#else
	platform_driver_unregister(&sec_nfc_driver);
#endif
}

module_init(sec_nfc_init);
module_exit(sec_nfc_exit);

MODULE_DESCRIPTION("Samsung sec_nfc driver");
MODULE_LICENSE("GPL");

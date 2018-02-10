#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/miscdevice.h>
#include <linux/fs.h>

#include "led/led.h"

#define	DRIVER_NAME					"hi3518ev200"

#define	DRV_DEBUG(fmt, args...)		\
			printk("Driver debug: "fmt"(func: %s, line: %d)\n", ##args, __func__, __LINE__);

#define	DRV_ERROR(fmt, args...)		\
			printk("Driver error: "fmt"(func: %s, line: %d)\n", ##args, __func__, __LINE__);


static ssize_t drv_read(struct file *filp, char __user *buf, size_t count, loff_t *offset)
{
	DRV_DEBUG("driver read");


	return 0;
}

static ssize_t drv_write(struct file *filp, const char __user *buf, size_t count, loff_t *offset)
{
	DRV_DEBUG("driver write");


	return 0;
}

static unsigned int drv_poll(struct file *filp, struct poll_table_struct *table)
{
	DRV_DEBUG("driver poll");

	return 0;
}

static long drv_ioctl(struct file *filp, unsigned int cmd, unsigned long args)
{
	int ret = -1;
	DRV_DEBUG("driver ioctl, cmd = %d", cmd);

	if (_IOC_TYPE(cmd) != LED_IOC_MAGIC) {
		DRV_ERROR("ioctl cmd type error, type = %d", _IOC_TYPE(cmd));
		return -EINVAL;
	}

	if (_IOC_NR(cmd) > LED_IOC_MAXNR) {
		DRV_ERROR("ioctl cmd nr error, nr = %d", _IOC_NR(cmd));
		return -EINVAL;
	}

	ret = led_operation(cmd, args);
	if (ret) {
		DRV_ERROR("led operation failed");
		return -EFAULT;
	}

	return 0;
}

static int drv_open(struct inode *inodp, struct file *filp)
{
	DRV_DEBUG("driver open");

	led_init();

	return 0;
}

static int drv_release(struct inode *inodp, struct file *filp)
{
	DRV_DEBUG("driver release");

	led_uninit();

	return 0;
}

static struct file_operations drv_fops = {
	.owner = THIS_MODULE,
	.read = drv_read,
	.write = drv_write,
	.poll = drv_poll,
	.open = drv_open,
	.release = drv_release,
	.unlocked_ioctl = drv_ioctl,
};

static struct miscdevice drv_misc = {
	.fops = &drv_fops,
	.name = DRIVER_NAME,
	.minor = MISC_DYNAMIC_MINOR,
};

static int drv_init(void)
{
	int ret = -1;

	ret = misc_register(&drv_misc);
	if (ret) {
		DRV_ERROR("misc_register failed, ret = %d", ret);
		return ret;
	}

	DRV_DEBUG("driver init OK");

	return 0;
}

static void drv_exit(void)
{
	DRV_DEBUG("driver exit");
	misc_deregister(&drv_misc);
}

module_init(drv_init);
module_exit(drv_exit);

MODULE_LICENSE("GPL");

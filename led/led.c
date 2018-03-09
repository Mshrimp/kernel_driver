#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/fs.h>
#include <linux/device.h>
#include <linux/spinlock.h>
#include <linux/uaccess.h>
#include <linux/mutex.h>
#include <linux/delay.h>

#include "../chip/chip.h"
#include "led.h"

#define	DEV_NAME				"driver_led"

#define	LED_GET_STATUS			2	// Just for module test

#define LED_ON					0
#define LED_OFF					1

#define	led_debug(fmt, args...)			\
			printk("led debug: "fmt"(func: %s, line: %d)\n", ##args, __func__, __LINE__);
#define	led_error(fmt, args...)			\
			printk("led error: "fmt"(func: %s, line: %d)\n", ##args, __func__, __LINE__);

typedef struct {
	gpio_t gpio;
	u8 status;
	u8 normal;
	spinlock_t lock;
	struct mutex mutex;
} led_info_t;

static led_info_t led_info = {
	.gpio = {
		.group = GPIOG,
		.bit = 11,
	},
	.normal = 0,
};

static int led_set_on(void)
{
	int ret = -1;
	led_debug("led_set_on");

	if (LED_ON == led_info.status) {
		led_debug("already on");
	}

	mutex_lock(&led_info.mutex);
	//ret = set_gpio_output_high(&led_info.gpio);
	ret = set_gpio_output_val(&led_info.gpio, !led_info.normal);
	if (ret) {
		led_error("set on failed, ret = %d", ret);
		return -1;
	}

	led_info.status = LED_ON;
	mutex_unlock(&led_info.mutex);

	return 0;
}

static int led_set_off(void)
{
	int ret = -1;
	led_debug("led_set_off");

	if (LED_OFF == led_info.status) {
		led_debug("already off");
	}

	mutex_lock(&led_info.mutex);
	//ret = set_gpio_output_low(&led_info.gpio);
	ret = set_gpio_output_val(&led_info.gpio, led_info.normal);
	if (ret) {
		led_error("set off failed, ret = %d", ret);
		return -1;
	}

	led_info.status = LED_OFF;
	mutex_unlock(&led_info.mutex);

	return 0;
}
// Just for module test
static int led_get_status(unsigned long *status)
{
	int ret = -1;

	led_debug("led_get_status");

	mutex_lock(&led_info.mutex);
	ret = get_gpio_val(&led_info.gpio);
	if (ret < 0) {
		led_error("get failed, ret = %d", ret);
		return -EAGAIN;
	}
	led_debug("ret = %d", ret);
	mutex_unlock(&led_info.mutex);
	*status = led_info.normal ? !ret : ret;
	led_debug("get status: %s", *status ? "On" : "Off");

	return 0;
}

static int led_gpio_init(void)
{
	int ret = -1;
	led_debug("led_gpio_init");

	ret = set_gpio_output(&led_info.gpio);
	if (ret) {
		led_error("gpio config output failed, ret = %d", ret);
		return -1;
	}

	ret = led_set_off();
	if (ret) {
		led_error("set off failed");
		return -1;
	}

	return 0;
}

/* Led module frame */

int led_init(void)
{
	int ret = -1;
	led_debug("led_init");

	spin_lock_init(&led_info.lock);
	mutex_init(&led_info.mutex);

	spin_lock(&led_info.lock);
	led_info.status = LED_OFF;
	spin_unlock(&led_info.lock);

	ret = led_gpio_init();
	if (ret) {
		led_error("gpio init failed, ret = %d", ret);
		return -1;
	}

	return 0;
}

int led_uninit(void)
{
	int ret = -1;
	led_debug("led_uninit");

	ret = led_set_off();
	if (ret) {
		led_error("set off failed");
		return -1;
	}
	spin_lock(&led_info.lock);
	led_info.status = LED_OFF;
	spin_unlock(&led_info.lock);

	return 0;
}

int led_operation(unsigned int cmd, unsigned long args)
{
	int ret = -1;
	unsigned long int status = 0;
	led_debug("led_operation");

	switch (cmd) {
		case LED_IOC_ON:
			ret = led_set_on();
			break;
		case LED_IOC_OFF:
			ret = led_set_off();
			break;
		case LED_IOC_GET_STATUS:
			ret = led_get_status(&status);
			if (!ret) {
				ret = copy_to_user((void __user *)args, &status, sizeof(status));
				if (ret) {
					led_error("copy_to_user failed, ret = %d", ret);
					return -EAGAIN;
				}
			}
			break;
		default:
			led_error("wrong cmd, nv_ir_led_cmd = %d", cmd);
			ret = -EINVAL;
			break;
	}

	if (ret) {
		led_error("operation failed");
	}

	return ret;
}

long driver_led_ioctl (struct file *filp, unsigned int cmd, unsigned long arg)
{
	int ret = -1;
	printk("Driver: led ioctl!\n");

	if (_IOC_TYPE(cmd) != LED_IOC_MAGIC) {
		led_error("ioctl cmd type error, type = %d", _IOC_TYPE(cmd));
		return -EINVAL;
	}

	if (_IOC_NR(cmd) > LED_IOC_MAXNR) {
		led_error("ioctl cmd nr error, nr = %d", _IOC_NR(cmd));
		return -EINVAL;
	}

	ret = led_operation(_IOC_NR(cmd), arg);
	if (ret) {
		led_error("led_operation failed");
		return -EFAULT;
	}

	return ret;
}

ssize_t driver_led_write (struct file *filp, const char __user *buf, size_t size, loff_t *offset)
{
	printk("Driver: led write!\n");
	return size;
}

ssize_t driver_led_read (struct file *filp, char __user *buf, size_t size, loff_t *offset)
{
	printk("Driver: led read!\n");

	return size;
}

int driver_led_open (struct inode *inodep, struct file *filp)
{
	printk("Driver: led open!\n");

	led_init();

	return 0;
}

int driver_led_close (struct inode *inodep, struct file *filp)
{
	printk("Driver: led close!\n");

	led_uninit();

	return 0;
}

/////////////////////////////////////////////////////////////模块
struct file_operations fops = {
	.owner = THIS_MODULE,
	.open = driver_led_open,
	.release = driver_led_close,
	.read = driver_led_read,
	.write = driver_led_write,
	.unlocked_ioctl = driver_led_ioctl,
};

int major = 0;
struct class *driver_class;
struct device *driver_class_device;

static int driver_led_init(void)
{
	printk("Hello, driver chrdev register led module init!\n");

	major = register_chrdev(major, DEV_NAME, &fops);
	if (major < 0) {
		led_error("register_chrdev failed");
		goto ERR_dev_register;
	}
	printk("major = %d\n", major);

	driver_class = class_create(THIS_MODULE, "driver_class");
	if (!driver_class) {
		led_error("class_create failed");
		goto ERR_class_create;
	}

	driver_class_device = device_create(driver_class, NULL, MKDEV(major, 0), NULL, "driver_class_device");
	if (!driver_class_device) {
		led_error("device_create failed");
		goto ERR_class_device_create;
	}

	return 0;
ERR_class_device_create:
	device_destroy(driver_class, MKDEV(major, 0));
ERR_class_create:
	unregister_chrdev(major, DEV_NAME);
ERR_dev_register:
	return -1;
}

static void driver_led_exit(void)
{
	printk("Goodbye, led module exit!\n");
	class_destroy(driver_class);
	device_destroy(driver_class, MKDEV(major, 0));
	unregister_chrdev(major, DEV_NAME);
}


module_init(driver_led_init);
module_exit(driver_led_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Mshrimp");
MODULE_DESCRIPTION("LED driver");
MODULE_VERSION("V0.1");

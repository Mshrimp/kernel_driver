#include <linux/spinlock.h>
#include <linux/uaccess.h>
#include <linux/mutex.h>

#include "led.h"
#include "../chip/chip.h"

#define	LED_GET_STATUS			2	// Just for module test

#define LED_ON				0
#define LED_OFF				1

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
		return 0;
	}

	mutex_lock(&led_info.mutex);
	ret = set_gpio_output_val(&led_info.gpio, !led_info.normal);
	//ret = set_gpio_output_high(&led_info.gpio);
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
		return 0;
	}

	mutex_lock(&led_info.mutex);
	ret = set_gpio_output_val(&led_info.gpio, led_info.normal);
	//ret = set_gpio_output_low(&led_info.gpio);
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

    /*
	 *ret = led_set_off();
	 *if (ret) {
	 *    led_error("set off failed");
	 *    return -1;
	 *}
     */

	return 0;
}

/* Ir led module frame */

int led_init(void)
{
	int ret = -1;
	led_debug("led_init");

	spin_lock_init(&led_info.lock);
	mutex_init(&led_info.mutex);

	spin_lock(&led_info.lock);
    /*
	 *led_info.normal = 0;
	 *led_info.status = LED_OFF;
     */
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


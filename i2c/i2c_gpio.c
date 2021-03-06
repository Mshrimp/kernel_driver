#include <linux/mutex.h>
#include <linux/spinlock.h>
#include <linux/errno.h>
#include <linux/delay.h>

#include "../chip/chip.h"
#include "i2c_gpio.h"

#define	I2C_DELAY			2

#define	GPIO_SCL			0
#define	GPIO_SDA			1

#define	GPIO_HIGH			1
#define	GPIO_LOW			0

#define	SET_SCL_OUT			set_gpio_output(&i2c_info.gpio.scl)
#define	SET_SDA_OUT			set_gpio_output(&i2c_info.gpio.sda)
#define	SET_SDA_IN			set_gpio_input(&i2c_info.gpio.sda)

#define	SET_SCL_HIGH		set_gpio_output_val(&i2c_info.gpio.scl, GPIO_HIGH)
#define	SET_SCL_LOW			set_gpio_output_val(&i2c_info.gpio.scl, GPIO_LOW)

#define	SET_SDA_HIGH		set_gpio_output_val(&i2c_info.gpio.sda, GPIO_HIGH)
#define	SET_SDA_LOW			set_gpio_output_val(&i2c_info.gpio.sda, GPIO_LOW)

#define	GET_SCL_VAL			get_gpio_val(&i2c_info.gpio.scl)
#define	GET_SDA_VAL			get_gpio_val(&i2c_info.gpio.sda)

#define	i2c_debug(fmt, args...)		\
			printk("i2c debug: "fmt"(func: %s, line: %d)\n", ##args, __func__, __LINE__);
#define	i2c_error(fmt, args...)		\
			printk("i2c error: "fmt"(func: %s, line: %d)\n", ##args, __func__, __LINE__);

typedef struct {
	i2c_gpio_t gpio;
	spinlock_t lock;
	struct mutex i2c_mutex;
} i2c_info_t;


static i2c_info_t i2c_info = {
    /*
	 *.gpio = {
	 *    .scl = {
	 *        .group = GPIOG,
	 *        .bit = 12,
	 *    },
	 *    .sda = {
	 *        .group = GPIOA,
	 *        .bit = 7,
	 *    },
	 *},
     */
};

/*
 *static void i2c_scl_set_high(void)
 *{
 *    int i = 0;
 *
 *    SET_SCL_HIGH;
 *    for (i = 0; i < 10; i++) {
 *        if (!GET_SCL_VAL) {
 *            SET_SCL_HIGH;
 *            udelay(I2C_DELAY);
 *        } else {
 *            break;
 *        }
 *    }
 *
 *    if (i == 10) {
 *        i2c_error("scl busy, set scl high failed");
 *    }
 *}
 */

static void i2c_scl_init(void)
{
	i2c_debug("scl init");
	SET_SCL_OUT;
	SET_SCL_HIGH;
}

static void i2c_sda_init(void)
{
	i2c_debug("sda init");
	SET_SDA_OUT;
	SET_SDA_HIGH;
}

static void i2c_gpio_init(void)
{
	i2c_debug("i2c_gpio_init");
	i2c_sda_init();
	i2c_scl_init();
}

int i2c_init(i2c_gpio_t *gpio)
{
	i2c_debug("i2c_init");

	spin_lock_init(&i2c_info.lock);
	mutex_init(&i2c_info.i2c_mutex);

	i2c_info.gpio.scl = gpio->scl;
	i2c_info.gpio.sda = gpio->sda;

	i2c_gpio_init();

	return 0;
}

int i2c_start(void)
{
	//i2c_debug("i2c_start");

	mutex_lock(&i2c_info.i2c_mutex);
	SET_SDA_OUT;
	udelay(I2C_DELAY);

	SET_SDA_HIGH;
	udelay(I2C_DELAY);

	SET_SCL_HIGH;
	udelay(I2C_DELAY);

	SET_SDA_LOW;
	udelay(I2C_DELAY);

	SET_SCL_LOW;
	udelay(I2C_DELAY);
	mutex_unlock(&i2c_info.i2c_mutex);

	return 0;
}

int i2c_stop(void)
{
	//i2c_debug("i2c_stop");

	mutex_lock(&i2c_info.i2c_mutex);
	SET_SDA_OUT;
	udelay(I2C_DELAY);

	SET_SCL_LOW;
	udelay(I2C_DELAY);

	SET_SDA_LOW;
	udelay(I2C_DELAY);

	SET_SCL_HIGH;
	udelay(I2C_DELAY);

	SET_SDA_HIGH;
	udelay(I2C_DELAY);
	mutex_unlock(&i2c_info.i2c_mutex);

	return 0;
}

static int i2c_send_ack(void)
{
	//i2c_debug("i2c_send_ack");

	mutex_lock(&i2c_info.i2c_mutex);
	SET_SDA_OUT;
	udelay(I2C_DELAY);

	SET_SCL_LOW;
	udelay(I2C_DELAY);

	SET_SDA_LOW;
	udelay(I2C_DELAY);

	SET_SCL_HIGH;
	udelay(I2C_DELAY);

	SET_SCL_LOW;
	udelay(I2C_DELAY);
	mutex_unlock(&i2c_info.i2c_mutex);

	return 0;
}

static int i2c_send_noack(void)
{
	//i2c_debug("i2c_send_noack");

	mutex_lock(&i2c_info.i2c_mutex);
	SET_SDA_OUT;
	udelay(I2C_DELAY);

	SET_SCL_LOW;
	udelay(I2C_DELAY);

	SET_SDA_HIGH;
	udelay(I2C_DELAY);

	SET_SCL_HIGH;
	udelay(I2C_DELAY);

	SET_SCL_LOW;
	udelay(I2C_DELAY);
	mutex_unlock(&i2c_info.i2c_mutex);

	return 0;
}

static int i2c_wait_ack(void)
{
	int ack_times = 0;
	int ret = 0;
	//i2c_debug("i2c_wait_ack");

	mutex_lock(&i2c_info.i2c_mutex);
	SET_SDA_OUT;
	udelay(I2C_DELAY);

	SET_SDA_HIGH;
	udelay(I2C_DELAY);
	//i2c_debug("GET_SDA_VAL SDA High: %d", GET_SDA_VAL);

	SET_SDA_IN;
	udelay(I2C_DELAY);
	//i2c_debug("GET_SDA_VAL in: %d", GET_SDA_VAL);

	SET_SCL_LOW;
	udelay(I2C_DELAY);
	//i2c_debug("GET_SDA_VAL scl low: %d", GET_SDA_VAL);

	SET_SCL_HIGH;
	udelay(I2C_DELAY);
	//i2c_debug("GET_SDA_VAL scl high: %d", GET_SDA_VAL);

	ack_times = 0;
	//i2c_debug("GET_SDA_VAL: %d", GET_SDA_VAL);
	while (GET_SDA_VAL) {
		i2c_debug("i2c_wait_ack, ack_times = %d", ack_times);
		ack_times++;
		if (ack_times == 10) {
			ret = 1;
			i2c_error("i2c ack error, no ack");
			break;
		}
	}

	SET_SCL_LOW;
	mutex_unlock(&i2c_info.i2c_mutex);

	return ret;
}

int i2c_ack(u8 ack_status)
{
	int ret = -1;

	switch (ack_status) {
	case I2C_WAIT_ACK:
		ret = i2c_wait_ack();
		break;
	case I2C_ACK:
		ret = i2c_send_ack();
		break;
	case I2C_NOACK:
		ret = i2c_send_noack();
		break;
	default:
		i2c_error("ack status error");
		return -EINVAL;
	}

	if (ret) {
		i2c_error("ack error");
	}

	return ret;
}

int i2c_write_byte(u8 data)
{
	unsigned long flag = 0;
	u8 i = 0;
	//i2c_debug("i2c_write_byte: 0x%X", data);

	local_irq_save(flag);
	preempt_disable();
	mutex_lock(&i2c_info.i2c_mutex);
	SET_SDA_OUT;
	udelay(I2C_DELAY);

	for (i = 0; i < 8; i++) {
		if (data & 0x80) {
			SET_SDA_HIGH;
		} else {
			SET_SDA_LOW;
		}
		udelay(I2C_DELAY);

		SET_SCL_HIGH;
		udelay(I2C_DELAY);

		SET_SCL_LOW;
		udelay(I2C_DELAY);

		data <<= 0x1;
	}
	mutex_unlock(&i2c_info.i2c_mutex);
	preempt_enable();
	local_irq_restore(flag);

	return 0;
}

int i2c_write_byte_with_ack(u8 data)
{
	//i2c_debug("i2c_write_byte_with_ack: 0x%X", data);
	i2c_write_byte(data);
	if (i2c_ack(I2C_WAIT_ACK)) {
		i2c_error("wait ack failed, no ack");
		i2c_stop();
		return -1;
	}

	return 0;
}

int i2c_read_byte(u8 *data)
{
	unsigned long flag = 0;
	u8 ret = 0;
	u8 i = 0;

	local_irq_save(flag);
	preempt_disable();
	mutex_lock(&i2c_info.i2c_mutex);

	SET_SDA_IN;
	udelay(I2C_DELAY);

	for (i = 0; i < 8; i++) {
		SET_SCL_HIGH;
		udelay(I2C_DELAY);

		ret <<= 1;

		if (GET_SDA_VAL) {
			ret |= 0x01;
		}

		SET_SCL_LOW;
		udelay(I2C_DELAY);
	}

	mutex_unlock(&i2c_info.i2c_mutex);
	preempt_enable();
	local_irq_restore(flag);

	*data = ret;

	return 0;
}


#ifndef	__GPIO_H__
#define	__GPIO_H__

#include <linux/mutex.h>
#include <linux/spinlock.h>
#include <linux/errno.h>
#include <linux/delay.h>

#include "allwinner-h3.h"

// GPIO

typedef struct {
	unsigned char group;
	unsigned char bit;
} gpio_t;

typedef struct {
	gpio_t scl;
	gpio_t sda;
} i2c_gpio_t;

int set_gpio_output(gpio_t *gpio);

int set_gpio_input(gpio_t *gpio);

int set_gpio_output_high(gpio_t *gpio);

int set_gpio_output_low(gpio_t *gpio);

int set_gpio_output_val(gpio_t *gpio, unsigned char val);

int set_gpio_input(gpio_t *gpio);

int get_gpio_val(gpio_t *gpio);

// I2C

typedef enum {
	I2C_WAIT_ACK = 0x0,
	I2C_ACK = 0x1,
	I2C_NOACK = 0x2,
} i2c_ack_e;

#endif	/* __GPIO_H__ */

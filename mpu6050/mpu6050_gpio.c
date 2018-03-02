#include <linux/spinlock.h>

#include "../chip/chip.h"
#include "../i2c/i2c_gpio.h"
#include "mpu6050_gpio.h"

#define	MPU6050_CHIP_ADDR		0x68

#define	mpu_debug(fmt, args...)		\
			printk("MPU6050 debug: "fmt"(func: %s, line: %d)\n", ##args, __func__, __LINE__);
#define	mpu_error(fmt, args...)		\
			printk("MPU6050 error: "fmt"(func: %s, line: %d)\n", ##args, __func__, __LINE__);


typedef struct {
	i2c_gpio_t gpio;
	spinlock_t lock;
} mpu_info_t;

static mpu_info_t mpu_info = {
	.gpio = {
		.scl = {
			.group = GPIOG,
			.bit = 11,
		},
		.sda = {
			.group = GPIOA,
			.bit = 6,
		},
	},
};

int mpu6050_write_byte()
{
	mpu_debug("mpu6050_write_byte");

	i2c_start();
	i2c_write_byte_with_ack(MPU6050_CHIP_ADDR);

	return 0;
}

int mpu6050_read_byte()
{

	return 0;
}

int mpu6050_init(void)
{
	int ret = -1;
	mpu_debug("mpu6050_init");

	spin_lock_init(&mpu_info.lock);

	ret = i2c_init(&mpu_info.gpio);
	if (ret) {
		mpu_error("i2c init failed, ret = %d", ret);
		return -EFAULT;
	}

	mpu6050_chip_init();

	return 0;
}

int mpu6050_uninit(void)
{
	mpu_debug("mpu6050_uninit");

	return 0;
}

int mpu6050_operation(unsigned int cmd, unsigned long args)
{
	int ret = -1;

	switch (cmd) {

	default:
		mpu_error("cmd error no = %d", cmd);
		ret = -EINVAL;
		break;
	}

	if (ret) {
		mpu_error("mpu6050 operation failed");
	}

	return ret;
}


#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/fs.h>
#include <linux/device.h>
#include <linux/spinlock.h>
#include <linux/uaccess.h>
#include <linux/vmalloc.h>

#include "../chip/chip.h"
#include "../i2c/i2c_gpio.h"
#include "mpu6050_gpio.h"

#define DEV_NAME		`			"driver_mpu6050_gpio"

#define	MPU6050_CHIP_ADDR			0xD0

#define	MPU6050_SAMPLE_RATE_DIV_REG	0x19
#define	MPU6050_CFG_REG				0x1A
#define	MPU6050_GYRO_CFG_REG		0x1B
#define	MPU6050_ACCEL_CFG_REG		0x1C

#define	MPU6050_FIFO_ENABLE_REG		0x23
#define	MPU6050_INT_ENABLE_REG		0x38

#define	MPU6050_USER_CTRL_REG		0x6A
#define	MPU6050_PWR_MGMT1_REG		0x6B
#define	MPU6050_PWR_MGMT2_REG		0x6C
#define	MPU6050_WHO_AM_I_REG		0x75

typedef enum {
	MPU6050_ACCEL_AFS_SEL_2G =		(0x00 << 3),
	MPU6050_ACCEL_AFS_SEL_4G =		(0x01 << 3),
	MPU6050_ACCEL_AFS_SEL_8G =		(0x02 << 3),
	MPU6050_ACCEL_AFS_SEL_16G =		(0x03 << 3),
} mpu_accel_fs_e;

typedef enum {
	MPU6050_GYRO_FS_SEL_250	=		(0x00 << 3),
	MPU6050_GYRO_FS_SEL_500	=		(0x01 << 3),
	MPU6050_GYRO_FS_SEL_1000 =		(0x02 << 3),
	MPU6050_GYRO_FS_SEL_2000 =		(0x03 << 3),
} mpu_gyro_fs_e;

typedef enum {
	MPU6050_ACCEL_XOUT_H = 0x3B,
	MPU6050_ACCEL_XOUT_L = 0x3C,
	MPU6050_ACCEL_YOUT_H = 0x3D,
	MPU6050_ACCEL_YOUT_L = 0x3E,
	MPU6050_ACCEL_ZOUT_H = 0x3F,
	MPU6050_ACCEL_ZOUT_L = 0x40,
} mpu_accel_out_e;

typedef enum {
	MPU6050_TEMP_OUT_H = 0x41,
	MPU6050_TEMP_OUT_L = 0x42,
} mpu_temp_out_e;

typedef enum {
	MPU6050_GYRO_XOUT_H = 0x43,
	MPU6050_GYRO_XOUT_L = 0x44,
	MPU6050_GYRO_YOUT_H = 0x45,
	MPU6050_GYRO_YOUT_L = 0x46,
	MPU6050_GYRO_ZOUT_H = 0x47,
	MPU6050_GYRO_ZOUT_L = 0x48,
} mpu_gyro_out_e;

#define	mpu_debug(fmt, args...)		\
			printk("MPU6050 debug: "fmt"(func: %s, line: %d)\n", ##args, __func__, __LINE__);
#define	mpu_error(fmt, args...)		\
			printk("MPU6050 error: "fmt"(func: %s, line: %d)\n", ##args, __func__, __LINE__);


typedef struct {
	i2c_gpio_t gpio;
	spinlock_t lock;
	unsigned char slave_addr;
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
	.slave_addr = MPU6050_CHIP_ADDR,
};

static mpu_result_t mpu_result;

static mpu_accel_t mpu_accel = {
	.xout_h = {
		.reg = MPU6050_ACCEL_XOUT_H,
	},
	.xout_l = {
		.reg = MPU6050_ACCEL_XOUT_L,
	},
	.yout_h = {
		.reg = MPU6050_ACCEL_YOUT_H,
	},
	.yout_l = {
		.reg = MPU6050_ACCEL_YOUT_L,
	},
	.zout_h = {
		.reg = MPU6050_ACCEL_ZOUT_H,
	},
	.zout_l = {
		.reg = MPU6050_ACCEL_ZOUT_L,
	},
};

static mpu_temp_t mpu_temp = {
	.temp_h = {
		.reg = MPU6050_TEMP_OUT_H,
	},
	.temp_l = {
		.reg = MPU6050_TEMP_OUT_L,
	},
};

static mpu_gyro_t mpu_gyro = {
	.xout_h = {
		.reg = MPU6050_GYRO_XOUT_H,
	},
	.xout_l = {
		.reg = MPU6050_GYRO_XOUT_L,
	},
	.yout_h = {
		.reg = MPU6050_GYRO_YOUT_H,
	},
	.yout_l = {
		.reg = MPU6050_GYRO_YOUT_L,
	},
	.zout_h = {
		.reg = MPU6050_GYRO_ZOUT_H,
	},
	.zout_l = {
		.reg = MPU6050_GYRO_ZOUT_L,
	},
};

int mpu6050_write_byte(mpu_reg_t *mpu_reg)
{
	mpu_debug("mpu6050_write_byte");

	i2c_start();
	i2c_write_byte_with_ack(MPU6050_CHIP_ADDR);
	i2c_write_byte_with_ack(mpu_reg->reg);
	i2c_write_byte_with_ack(mpu_reg->data);
	i2c_stop();

	return 0;
}

int mpu6050_write_register(unsigned char reg, unsigned char data)
{
	mpu_debug("mpu6050_write_byte");

	i2c_start();
	i2c_write_byte_with_ack(MPU6050_CHIP_ADDR);
	i2c_write_byte_with_ack(reg);
	i2c_write_byte_with_ack(data);
	i2c_stop();

	return 0;
}

int mpu6050_write_bytes(mpu_regs_t *mpu_regs)
{
	int i = 0;
	mpu_debug("mpu6050_write_bytes, len: %d", mpu_regs->len);

	i2c_start();
	i2c_write_byte_with_ack(MPU6050_CHIP_ADDR);
	i2c_write_byte_with_ack(mpu_regs->reg);

	for (i = 0; i < mpu_regs->len; i++) {
		i2c_write_byte_with_ack(mpu_regs->data[i]);
	}
	i2c_stop();

	return i;
}

int mpu6050_write_regs_data(mpu_regs_data_t *mpu_regs_data)
{
	int i = 0;
	int ret = -1;
	mpu_debug("mpu6050_write_regs_data, len: %d", mpu_regs_data->len);

	for (i = 0; i < mpu_regs_data->len; i++) {
		ret = mpu6050_write_byte(&mpu_regs_data->reg_data[i]);
		if (ret) {
			mpu_error("mpu_6050_write_byte failed, ret = %d", ret);
			return -EFAULT;
		}
	}

	return i;
}

int mpu6050_read_byte(mpu_reg_t *mpu_reg)
{
	mpu_debug("mpu6050_read_byte");

	i2c_start();
	i2c_write_byte_with_ack(MPU6050_CHIP_ADDR);
	i2c_write_byte_with_ack(mpu_reg->reg);

	i2c_start();
	i2c_write_byte_with_ack(MPU6050_CHIP_ADDR + 1);
	i2c_read_byte(&mpu_reg->data);
	i2c_ack(I2C_NOACK);
	i2c_stop();

	return mpu_reg->data;
}

unsigned char mpu6050_read_register(unsigned char reg)
{
	unsigned char data = 0;
	mpu_debug("mpu6050_read_byte");

	i2c_start();
	i2c_write_byte_with_ack(MPU6050_CHIP_ADDR);
	i2c_write_byte_with_ack(reg);

	i2c_start();
	i2c_write_byte_with_ack(MPU6050_CHIP_ADDR + 1);
	i2c_read_byte(&data);
	i2c_ack(I2C_NOACK);
	i2c_stop();

	return data;
}

int mpu6050_read_registers_serial(mpu_regs_t *mpu_regs)
{
	int i = 0;
	mpu_debug("mpu6050_read_register_serial");

	if ((!mpu_regs) || (!mpu_regs->len) || (!mpu_regs->data)) {
		mpu_error("mpu6050 registers param error");
		return -EINVAL;
	}

	i2c_start();
	i2c_write_byte_with_ack(MPU6050_CHIP_ADDR);
	i2c_write_byte_with_ack(mpu_regs->reg);

	i2c_start();
	i2c_write_byte_with_ack(MPU6050_CHIP_ADDR + 1);

	for (i = 0; i < mpu_regs->len; i++) {
		i2c_read_byte(&mpu_regs->data[i]);
		if (i < mpu_regs->len - 1) {
			i2c_ack(I2C_ACK);
		} else {
			i2c_ack(I2C_NOACK);
		}
	}
	i2c_stop();

	return i;
}

int mpu6050_read_bytes(mpu_regs_t *mpu_regs)
{
	int i = 0;
	mpu_debug("mpu6050_read_bytes, len: %d", mpu_regs->len);

	i2c_start();
	i2c_write_byte_with_ack(MPU6050_CHIP_ADDR);
	i2c_write_byte_with_ack(mpu_regs->reg);

	i2c_start();
	i2c_write_byte_with_ack(MPU6050_CHIP_ADDR + 1);

	for (i = 0; i < mpu_regs->len; i++) {
		i2c_read_byte(&mpu_regs->data[i]);
		if (i == (mpu_regs->len - 1)) {
			i2c_ack(I2C_NOACK);
		} else {
			i2c_ack(I2C_ACK);
		}
	}
	i2c_stop();
	mpu_regs->len = i;

	return mpu_regs->len;
}

void mpu6050_chip_reset(void)
{
	mpu6050_write_register(MPU6050_PWR_MGMT1_REG, 0x80);
}

void mpu6050_chip_wakeup(void)
{
	mpu6050_write_register(MPU6050_PWR_MGMT1_REG, 0x00);
}

void mpu6050_set_lpf(int lpf)
{
	unsigned char data = 0;

	if (lpf >= 188) {
		data = 1;
	} else if (lpf >= 98) {
		data = 2;
	} else if (lpf >= 42) {
		data = 3;
	} else if (lpf >= 20) {
		data = 4;
	} else if (lpf >= 10) {
		data = 5;
	} else if (lpf >= 5) {
		data = 6;
	}

	mpu6050_write_register(MPU6050_CFG_REG, data);
}

// Fs = 1KHz
// rate: 4 ~ 1000Hz
void mpu6050_set_sample_rate(int rate)
{
	unsigned char data = 0;

	if (rate > 1000) {
		rate = 1000;
	}

	if (rate < 4) {
		rate = 4;
	}

	data = 1000 / rate - 1;

	mpu6050_write_register(MPU6050_SAMPLE_RATE_DIV_REG, data);

	mpu6050_set_lpf(rate / 2);
}

int mpu6050_check_chip_addr(void)
{
	unsigned char chip_addr = 0;

	chip_addr = mpu6050_read_register(MPU6050_WHO_AM_I_REG);
	if (chip_addr != (MPU6050_CHIP_ADDR >> 1)) {
		mpu_error("get chip address error, chip_addr = 0x%X", chip_addr);
		return -EFAULT;
	}

	return 0;
}

int mpu6050_chip_init(void)
{
	int ret = -1;
	mpu_debug("mpu6050_chip_init");

	mpu6050_chip_reset();
	msleep(100);
	mpu6050_chip_wakeup();

	mpu6050_write_register(MPU6050_GYRO_CFG_REG, MPU6050_GYRO_FS_SEL_2000);
	mpu6050_write_register(MPU6050_ACCEL_CFG_REG, MPU6050_ACCEL_AFS_SEL_16G);

	mpu6050_set_sample_rate(50);

	mpu6050_write_register(MPU6050_INT_ENABLE_REG, 0x00);	// Int disable
	mpu6050_write_register(MPU6050_USER_CTRL_REG, 0x00);	// I2C slave
	mpu6050_write_register(MPU6050_FIFO_ENABLE_REG, 0x00);	// FIFO disable

	ret = mpu6050_check_chip_addr();
	if (ret) {
		mpu_error("mpu6050_check_chip_addr failed");
		return -EFAULT;
	}

	return 0;
}

int mpu6050_set_accel_range(unsigned char accel_fs)
{
	unsigned char data = 0;
	mpu_debug("mpu6050_set_accel_range");

	switch (accel_fs) {
	case 0:
	case 1:
	case 2:
	case 3:
		data = accel_fs << 3;
		break;
	default:
		mpu_error("accel_fs error, accel_fs: %d", accel_fs);
		return -EINVAL;
	}

	mpu6050_write_register(MPU6050_ACCEL_CFG_REG, data);

	return 0;
}

int mpu6050_get_accel_range(unsigned char *accel_fs)
{
	unsigned char data = 0;
	mpu_debug("mpu6050_get_accel_range");

	data = mpu6050_read_register(MPU6050_ACCEL_CFG_REG);

	accel_fs = (data >> 3) & ((0x1 << 3) - 1);
	if (accel_fs > 3) {
		mpu_error("get accel_fs error, accel_fs: %d", accel_fs);
		return -EFAULT;
	}

	return 0;
}

int mpu6050_set_gyro_range(unsigned char gyro_fs)
{
	unsigned char data = 0;
	mpu_debug("mpu6050_set_gyro_range");

	switch (gyro_fs) {
	case 0:
	case 1:
	case 2:
	case 3:
		data = gyro_fs << 3;
		break;
	default:
		mpu_error("gyro_fs error, gyro_fs: %d", gyro_fs);
		return -EINVAL;
	}

	mpu6050_write_register(MPU6050_GYRO_CFG_REG, data);

	return 0;
}

int mpu6050_get_gyro_range(unsigned char *gyro_fs)
{
	unsigned char data = 0;
	mpu_debug("mpu6050_get_gyro_range");

	data = mpu6050_read_register(MPU6050_ACCEL_CFG_REG);

	gyro_fs = (data >> 3) & ((0x1 << 3) - 1);
	if (gyro_fs > 3) {
		mpu_error("get gyro_fs error, gyro_fs: %d", gyro_fs);
		return -EFAULT;
	}

	return 0;
}

int mpu6050_set_accel_and_gyro_range(mpu_range_t mpu_range)
{
	int ret = -1;
	mpu_debug("mpu6050_set_accel_and_gyro_range: %d, %d", mpu_range.accel, mpu_range.gyro);

	if ((mpu_range.accel > 3) || (mpu_range.gyro > 3)) {
		mpu_error("error range: %d, %d", mpu_range.accel, mpu_range.gyro);
		return -EINVAL;
	}

	ret = mpu6050_set_accel_range(mpu_range.accel);
	if (ret) {
		mpu_error("mpu6050_set_accel_range failed, ret = %d", ret);
		return -EFAULT;
	}

	ret = mpu6050_set_gyro_range(mpu_range.gyro);
	if (ret) {
		mpu_error("mpu6050_set_gyro_range failed, ret = %d", ret);
		return -EFAULT;
	}

	return 0;
}

int mpu6050_get_accel_and_gyro_range(mpu_range_t *mpu_range)
{
	int ret = -1;
	mpu_debug("mpu6050_get_accel_and_gyro_range");

	ret = mpu6050_get_accel_range(&mpu_range->accel);
	if (ret) {
		mpu_error("mpu6050_get_accel_range failed, ret = %d", ret);
		return -EFAULT;
	}

	ret = mpu6050_get_gyro_range(&mpu_range->gyro);
	if (ret) {
		mpu_error("mpu6050_get_gyro_range failed, ret = %d", ret);
		return -EFAULT;
	}

	if ((mpu_range->accel > 3) || (mpu_range->gyro > 3)) {
		mpu_error("error range: %d, %d", mpu_range->accel, mpu_range->gyro);
		return -EINVAL;
	}

	return 0;
}

short mpu6050_read_accel_xout(void)
{
	char xout_h, xout_l;
	short xout;
	mpu_debug("mpu6050_read_accel_xout");
	xout_h = mpu6050_read_byte(&mpu_accel.xout_h);
	xout_l = mpu6050_read_byte(&mpu_accel.xout_l);

	xout = (short)((xout_h << 8) | xout_l);
	mpu_debug("read accel xout: 0x%X", xout);

	return xout;
}

short mpu6050_read_accel_yout(void)
{
	char yout_h, yout_l;
	short yout;
	mpu_debug("mpu6050_read_accel_yout");
	yout_h = mpu6050_read_byte(&mpu_accel.yout_h);
	yout_l = mpu6050_read_byte(&mpu_accel.yout_l);

	yout = (short)((yout_h << 8) | yout_l);
	mpu_debug("read accel yout: 0x%X", yout);

	return yout;
}

short mpu6050_read_accel_zout(void)
{
	char zout_h, zout_l;
	short zout;
	mpu_debug("mpu6050_read_accel_zout");
	zout_h = mpu6050_read_byte(&mpu_accel.zout_h);
	zout_l = mpu6050_read_byte(&mpu_accel.zout_l);

	zout = (short)((zout_h << 8) | zout_l);
	mpu_debug("read accel zout: 0x%X", zout);

	return zout;
}

int mpu6050_read_accel_serial(mpu_accel_data_t *mpu_accel_data)
{
	unsigned char data[6] = {0};
	mpu_regs_t mpu_regs;
	int ret = -1;
	mpu_debug("mpu6050_read_accel_serial");

	if (!mpu_accel_data) {
		mpu_error("param error");
		return -EINVAL;
	}

	mpu_regs.reg = MPU6050_ACCEL_XOUT_H;
	mpu_regs.len = 6;
	mpu_regs.data = data;

	ret = mpu6050_read_registers_serial(&mpu_regs);
	if (ret) {
		mpu_error("mpu6050_read_registers_serial failed, ret = %d", ret);
		return -EFAULT;
	}

	mpu_accel_data->xout = (short)((data[0] << 8) | data[1]);
	mpu_accel_data->yout = (short)((data[2] << 8) | data[3]);
	mpu_accel_data->zout = (short)((data[4] << 8) | data[5]);

	return 0;
}

int mpu6050_read_accel(mpu_accel_data_t *mpu_accel_data)
{
	mpu_debug("mpu6050_read_accel");

	mpu_accel_data->xout = mpu6050_read_accel_xout();
	mpu_accel_data->yout = mpu6050_read_accel_yout();
	mpu_accel_data->zout = mpu6050_read_accel_zout();

	return 0;
}

short mpu6050_read_temp(mpu_temp_data_t *mpu_temp_data)
{
	char temp_h, temp_l;
	short temp;
	mpu_debug("mpu6050_read_temp_out");
	temp_h = mpu6050_read_byte(&mpu_temp.temp_h);
	temp_l = mpu6050_read_byte(&mpu_temp.temp_l);

	temp = (short)((temp_h << 8) | temp_l);
	mpu_debug("read temp out: 0x%X", temp);

	mpu_temp_data->temp = temp;

	return temp;
}

short mpu6050_read_temp_serial(mpu_temp_data_t *mpu_temp_data)
{
	unsigned char data[2] = {0};
	mpu_regs_t mpu_regs;
	short temp;
	int ret = -1;
	mpu_debug("mpu6050_read_temp_serial");

	if (!mpu_temp_data) {
		mpu_error("param error");
		return -EINVAL;
	}

	mpu_regs.reg = MPU6050_TEMP_OUT_H;
	mpu_regs.len = 2;
	mpu_regs.data = data;

	ret = mpu6050_read_registers_serial(&mpu_regs);
	if (ret) {
		mpu_error("mpu6050_read_registers_serial failed, ret = %d", ret);
		return -EFAULT;
	}

	temp = (short)((data[0] << 8) | data[1]);
	mpu_debug("read temp: 0x%X", temp);

	mpu_temp_data->temp = temp;

	return temp;
}

short mpu6050_read_gyro_xout(void)
{
	char xout_h, xout_l;
	short xout;
	mpu_debug("mpu6050_read_gyro_xout");
	xout_h = mpu6050_read_byte(&mpu_gyro.xout_h);
	xout_l = mpu6050_read_byte(&mpu_gyro.xout_l);

	xout = (short)((xout_h << 8) | xout_l);
	mpu_debug("read gyro xout: 0x%X", xout);

	return xout;
}

short mpu6050_read_gyro_yout(void)
{
	char yout_h, yout_l;
	short yout;
	mpu_debug("mpu6050_read_gyro_yout");
	yout_h = mpu6050_read_byte(&mpu_gyro.yout_h);
	yout_l = mpu6050_read_byte(&mpu_gyro.yout_l);

	yout = (short)((yout_h << 8) | yout_l);
	mpu_debug("read gyro yout: 0x%X", yout);

	return yout;
}

short mpu6050_read_gyro_zout(void)
{
	char zout_h, zout_l;
	short zout;
	mpu_debug("mpu6050_read_gyro_zout");
	zout_h = mpu6050_read_byte(&mpu_gyro.zout_h);
	zout_l = mpu6050_read_byte(&mpu_gyro.zout_l);

	zout = (short)((zout_h << 8) | zout_l);
	mpu_debug("read gyro zout: 0x%X", zout);

	return zout;
}

int mpu6050_read_gyro(mpu_gyro_data_t *mpu_gyro_data)
{
	mpu_debug("mpu6050_read_gyro");
	mpu_gyro_data->xout = mpu6050_read_gyro_xout();
	mpu_gyro_data->yout = mpu6050_read_gyro_yout();
	mpu_gyro_data->zout = mpu6050_read_gyro_zout();

	return 0;
}

int mpu6050_read_gyro_serial(mpu_gyro_data_t *mpu_gyro_data)
{
	unsigned char data[6] = {0};
	mpu_regs_t mpu_regs;
	int ret = -1;
	mpu_debug("mpu6050_read_gyro_serial");

	if (!mpu_gyro_data) {
		mpu_error("param error");
		return -EINVAL;
	}

	mpu_regs.reg = MPU6050_GYRO_XOUT_H;
	mpu_regs.len = 6;
	mpu_regs.data = data;

	ret = mpu6050_read_registers_serial(&mpu_regs);
	if (ret) {
		mpu_error("mpu6050_read_registers_serial failed, ret = %d", ret);
		return -EFAULT;
	}

	mpu_gyro_data->xout = (short)((data[0] << 8) | data[1]);
	mpu_gyro_data->yout = (short)((data[2] << 8) | data[3]);
	mpu_gyro_data->zout = (short)((data[4] << 8) | data[5]);

	return 0;
}

int mpu6050_get_result(mpu_result_t *mpu_result)
{
	mpu_debug("mpu6050_get_result");
	mpu6050_read_accel(&mpu_result->accel);
	mpu6050_read_gyro(&mpu_result->gyro);
	mpu6050_read_temp(&mpu_result->temp);

	return 0;
}

int mpu6050_get_result_serial(mpu_result_t *mpu_result)
{
	unsigned char data[14] = {0};
	mpu_regs_t mpu_regs;
	int ret = -1;
	mpu_debug("mpu6050_read_gyro_serial");

	if (!mpu_result) {
		mpu_error("param error");
		return -EINVAL;
	}

	mpu_regs.reg = MPU6050_GYRO_XOUT_H;
	mpu_regs.len = 14;
	mpu_regs.data = data;

	ret = mpu6050_read_registers_serial(&mpu_regs);
	if (ret) {
		mpu_error("mpu6050_get_result_serial failed, ret = %d", ret);
		return -EFAULT;
	}

	mpu_result->accel.xout = (short)((data[0] << 8) | data[1]);
	mpu_result->accel.yout = (short)((data[2] << 8) | data[3]);
	mpu_result->accel.zout = (short)((data[4] << 8) | data[5]);

	mpu_result->temp.temp = (short)((data[6] << 8) | data[7]);

	mpu_result->gyro.xout = (short)((data[8] << 8) | data[9]);
	mpu_result->gyro.yout = (short)((data[10] << 8) | data[11]);
	mpu_result->gyro.zout = (short)((data[12] << 8) | data[13]);

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
	mpu_reg_t mpu_reg;
	mpu_regs_t mpu_regs;
	mpu_regs_data_t mpu_regs_data;
	mpu_range_t mpu_range;

	switch (cmd) {
	case MPU_INIT:
		ret = mpu6050_chip_init();
		break;
	case MPU_WRITE_REG:
		ret = copy_from_user(&mpu_reg, (u32 __user *)args, sizeof(mpu_reg_t));
		if (ret) {
			mpu_error("MPU_WRITE_REG, copy_from_user failed, ret = %d", ret);
			return -EFAULT;
		}

		ret = mpu6050_write_byte(&mpu_reg);
		break;
	case MPU_WRITE_REGS:
		ret = copy_from_user(&mpu_regs, (u32 __user *)args, sizeof(mpu_regs_t));
		if (ret) {
			mpu_error("MPU_WRITE_REGS, copy_from_user failed, ret = %d", ret);
			return -EFAULT;
		}

		if (mpu_regs.len > 0) {
			mpu_regs.data = vmalloc(mpu_regs.len);
			if (!mpu_regs.data) {
				mpu_error("MPU_WRITE_REGS, vmalloc failed");
				return -EFAULT;
			}

			ret = copy_from_user(mpu_regs.data, (u32 __user *)args, mpu_regs.len);
			if (ret) {
				mpu_error("MPU_WRITE_REGS, copy_from_user data failed, ret = %d", ret);
				return -EFAULT;
			}
		}

		ret = mpu6050_write_bytes(&mpu_regs);
		if (ret != mpu_regs.len) {
			mpu_error("MPU_WRITE_REGS, mpu6050_write_bytes uncomplete, ret = %d", ret);
			return -EFAULT;
		}
		vfree(mpu_regs.data);

		break;
	case MPU_READ_REG:
		ret = copy_from_user(&mpu_reg, (u32 __user *)args, sizeof(mpu_reg_t));
		if (ret) {
			mpu_error("MPU_READ_REG, copy_from_user failed, ret = %d", ret);
			return -EFAULT;
		}

		ret = mpu6050_read_byte(&mpu_reg);

		ret = copy_to_user((u32 __user *)args, &mpu_reg, sizeof(mpu_reg_t));
		if (ret) {
			mpu_error("MPU_READ_REG, copy_to_user failed, ret = %d", ret);
			return -EFAULT;
		}

		break;
	case MPU_READ_REGS:
		ret = copy_from_user(&mpu_regs, (u32 __user *)args, sizeof(mpu_regs_t));
		if (ret) {
			mpu_error("MPU_READ_REGS, copy_from_user failed, ret = %d", ret);
			return -EFAULT;
		}

		if (mpu_regs.len > 0) {
			mpu_regs.data = vmalloc(mpu_regs.len);
			if (!mpu_regs.data) {
				mpu_error("MPU_READ_REGS, vmalloc failed");
				return -EFAULT;
			}
		}

		ret = mpu6050_read_bytes(&mpu_regs);
		if (ret != mpu_regs.len) {
			mpu_error("MPU_READ_REGS, mpu6050_read_bytes uncomplete, ret = %d", ret);
			return -EFAULT;
		}

		ret = copy_to_user((u32 __user *)args, &mpu_regs, sizeof(mpu_regs_t));
		if (ret) {
			mpu_error("MPU_READ_REGS, copy_to_user failed, ret = %d", ret);
			return -EFAULT;
		}

		break;
	case MPU_WRITE_REGS_DATA:
		ret = copy_from_user(&mpu_regs_data, (u32 __user *)args, sizeof(mpu_regs_data_t));
		if (ret) {
			mpu_error("MPU_WRITE_REGS_DATA, copy_from_user failed, ret = %d", ret);
			return -EFAULT;
		}

		if (mpu_regs_data.len > 0) {
			mpu_regs_data.reg_data = vmalloc(mpu_regs_data.len);
			if (!mpu_regs_data.reg_data) {
				mpu_error("MPU_WRITE_REGS_DATA, vmalloc failed");
				return -EFAULT;
			}
			ret = copy_from_user(mpu_regs_data.reg_data, (u32 __user *)args, mpu_regs_data.len);
			if (ret) {
				mpu_error("MPU_WRITE_REGS_DATA, copy_from_user data failed, ret = %d", ret);
				return -EFAULT;
			}
		}

		ret = mpu6050_write_regs_data(&mpu_regs_data);
		if (ret != mpu_regs_data.len) {
			mpu_error("MPU_WRITE_REGS_DATA, mpu6050_write_regs_data uncomplete, ret = %d", ret);
			return -EFAULT;
		}
		vfree(mpu_regs_data.reg_data);

		break;
	case MPU_GET_ACCEL:
		ret = mpu6050_read_accel_serial(&mpu_result.accel);
		ret = copy_to_user((u32 __user *)args, &mpu_result.accel, sizeof(mpu_accel_data_t));
		if (ret) {
			mpu_error("MPU_GET_ACCEL, copy_to_user failed, ret = %d", ret);
			return -EFAULT;
		}
		break;
	case MPU_GET_GYRO:
		ret = mpu6050_read_gyro_serial(&mpu_result.gyro);
		ret = copy_to_user((u32 __user *)args, &mpu_result.gyro, sizeof(mpu_gyro_data_t));
		if (ret) {
			mpu_error("MPU_GET_GYRO, copy_to_user failed, ret = %d", ret);
			return -EFAULT;
		}
		break;
	case MPU_GET_TEMP:
		ret = mpu6050_read_temp_serial(&mpu_result.temp);
		ret = copy_to_user((u32 __user *)args, &mpu_result.temp, sizeof(mpu_temp_data_t));
		if (ret) {
			mpu_error("MPU_GET_TEMP, copy_to_user failed, ret = %d", ret);
			return -EFAULT;
		}
		break;
	case MPU_GET_RESULT:
		ret = mpu6050_get_result_serial(&mpu_result);
		ret = copy_to_user((u32 __user *)args, &mpu_result, sizeof(mpu_result_t));
		if (ret) {
			mpu_error("MPU_GET_RESULT, copy_to_user failed, ret = %d", ret);
			return -EFAULT;
		}
		break;
	case MPU_SET_RANGE:
		ret = copy_from_user(&mpu_range, (u32 __user *)args, sizeof(mpu_range_t));
		if (ret) {
			mpu_error("MPU_SET_RANGE, copy_from_user failed, ret = %d", ret);
			return -EFAULT;
		}

		ret = mpu6050_set_accel_and_gyro_range(mpu_range);
		if (ret) {
			mpu_error("mpu6050_set_accel_and_gyro_range failed, ret = %d", ret);
			return -EFAULT;
		}
		break;
	case MPU_GET_RANGE:
		ret = mpu6050_get_accel_and_gyro_range(&mpu_range);
		if (ret) {
			mpu_error("mpu6050_set_accel_and_gyro_range failed, ret = %d", ret);
			return -EFAULT;
		}

		ret = copy_to_user((u32 __user *)args, &mpu_range, sizeof(mpu_range_t));
		if (ret) {
			mpu_error("MPU_SET_RANGE, copy_to_user failed, ret = %d", ret);
			return -EFAULT;
		}
		break;
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



long driver_mpu6050_ioctl (struct file *filp, unsigned int cmd, unsigned long arg)
{
	int ret = -1;
	printk("Driver: mpu6050 ioctl!\n");

	if (_IOC_TYPE(cmd) != MPU6050_IOC_MAGIC) {
		mpu_error("ioctl cmd type error, type = %d", _IOC_TYPE(cmd));
		return -EINVAL;
	}

	if (_IOC_NR(cmd) > MPU6050_IOC_MAXNR) {
		mpu_error("ioctl cmd nr error, nr = %d", _IOC_NR(cmd));
		return -EINVAL;
	}

	ret = mpu6050_operation(_IOC_NR(cmd), arg);
	if (ret) {
		mpu_error("mpu6050_operation failed");
		return -EFAULT;
	}

	return ret;
}

ssize_t driver_mpu6050_write (struct file *filp, const char __user *buf, size_t size, loff_t *offset)
{
	printk("Driver: mpu6050 write!\n");
	return size;
}

ssize_t driver_mpu6050_read (struct file *filp, char __user *buf, size_t size, loff_t *offset)
{
	printk("Driver: mpu6050 read!\n");

	return size;
}

int driver_mpu6050_open (struct inode *inodep, struct file *filp)
{
	printk("Driver: mpu6050 open!\n");

	mpu6050_init();

	return 0;
}

int driver_mpu6050_close (struct inode *inodep, struct file *filp)
{
	printk("Driver: mpu6050 close!\n");

	mpu6050_uninit();

	return 0;
}

/////////////////////////////////////////////////////////////模块
struct file_operations fops = {
	.owner = THIS_MODULE,
	.open = driver_mpu6050_open,
	.release = driver_mpu6050_close,
	.read = driver_mpu6050_read,
	.write = driver_mpu6050_write,
	.unlocked_ioctl = driver_mpu6050_ioctl,
};

int major = 0;
struct class *driver_class;
struct device *driver_class_device;

static int driver_mpu6050_init(void)
{
	printk("Hello, driver chrdev register mpu6050 module init!\n");

	major = register_chrdev(major, DEV_NAME, &fops);
	if (major < 0) {
		mpu_error("register_chrdev failed");
		goto ERR_dev_register;
	}
	printk("major = %d\n", major);

	driver_class = class_create(THIS_MODULE, "driver_class");
	if (!driver_class) {
		mpu_error("class_create failed");
		goto ERR_class_create;
	}

	driver_class_device = device_create(driver_class, NULL, MKDEV(major, 0), NULL, "driver_class_device");
	if (!driver_class_device) {
		mpu_error("device_create failed");
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

static void driver_mpu6050_exit(void)
{
	printk("Goodbye, mpu6050 module exit!\n");
	class_destroy(driver_class);
	device_destroy(driver_class, MKDEV(major, 0));
	unregister_chrdev(major, DEV_NAME);
}

module_init(driver_mpu6050_init);
module_exit(driver_mpu6050_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Mshrimp");
MODULE_DESCRIPTION("MPU6050 driver");
MODULE_VERSION("V0.1");

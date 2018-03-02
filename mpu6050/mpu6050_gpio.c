#include <linux/uaccess.h>
#include <linux/spinlock.h>
#include <linux/vmalloc.h>

#include "../chip/chip.h"
#include "../i2c/i2c_gpio.h"
#include "mpu6050_gpio.h"

#define	MPU6050_CHIP_ADDR		0xD0

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

int mpu6050_chip_init(void)
{
	mpu_debug("mpu6050_chip_init");

	return 0;
}

short mpu6050_read_accel_xout(void)
{
	char xout_h, xout_l;
	short xout;
	mpu_debug("mpu6050_read_accel_xout");
	xout_h = mpu6050_read_byte(&mpu_accel.xout_h);
	xout_l = mpu6050_read_byte(&mpu_accel.xout_l);

	xout = (xout_h << 4) | xout_l;
	mpu_debug("read accel xout: %d", xout);

	return xout;
}

short mpu6050_read_accel_yout(void)
{
	char yout_h, yout_l;
	short yout;
	mpu_debug("mpu6050_read_accel_yout");
	yout_h = mpu6050_read_byte(&mpu_accel.yout_h);
	yout_l = mpu6050_read_byte(&mpu_accel.yout_l);

	yout = (yout_h << 4) | yout_l;
	mpu_debug("read accel yout: %d", yout);

	return yout;
}

short mpu6050_read_accel_zout(void)
{
	char zout_h, zout_l;
	short zout;
	mpu_debug("mpu6050_read_accel_zout");
	zout_h = mpu6050_read_byte(&mpu_accel.zout_h);
	zout_l = mpu6050_read_byte(&mpu_accel.zout_l);

	zout = (zout_h << 4) | zout_l;
	mpu_debug("read accel zout: %d", zout);

	return zout;
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

	temp = (temp_h << 4) | temp_l;
	mpu_debug("read temp out: %d", temp);

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

	xout = (xout_h << 4) | xout_l;
	mpu_debug("read gyro xout: %d", xout);

	return xout;
}

short mpu6050_read_gyro_yout(void)
{
	char yout_h, yout_l;
	short yout;
	mpu_debug("mpu6050_read_gyro_yout");
	yout_h = mpu6050_read_byte(&mpu_gyro.yout_h);
	yout_l = mpu6050_read_byte(&mpu_gyro.yout_l);

	yout = (yout_h << 4) | yout_l;
	mpu_debug("read gyro yout: %d", yout);

	return yout;
}

short mpu6050_read_gyro_zout(void)
{
	char zout_h, zout_l;
	short zout;
	mpu_debug("mpu6050_read_gyro_zout");
	zout_h = mpu6050_read_byte(&mpu_gyro.zout_h);
	zout_l = mpu6050_read_byte(&mpu_gyro.zout_l);

	zout = (zout_h << 4) | zout_l;
	mpu_debug("read gyro zout: %d", zout);

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

int mpu6050_get_result(mpu_result_t *mpu_result)
{
	mpu_debug("mpu6050_get_result");
	mpu6050_read_accel(&mpu_result->accel);
	mpu6050_read_gyro(&mpu_result->gyro);
	mpu6050_read_temp(&mpu_result->temp);

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
		ret = mpu6050_read_accel(&mpu_result.accel);
		ret = copy_to_user((u32 __user *)args, &mpu_result.accel, sizeof(mpu_accel_data_t));
		if (ret) {
			mpu_error("MPU_GET_ACCEL, copy_to_user failed, ret = %d", ret);
			return -EFAULT;
		}
		break;
	case MPU_GET_GYRO:
		ret = mpu6050_read_gyro(&mpu_result.gyro);
		ret = copy_to_user((u32 __user *)args, &mpu_result.gyro, sizeof(mpu_gyro_data_t));
		if (ret) {
			mpu_error("MPU_GET_GYRO, copy_to_user failed, ret = %d", ret);
			return -EFAULT;
		}
		break;
	case MPU_GET_TEMP:
		ret = mpu6050_read_temp(&mpu_result.temp);
		ret = copy_to_user((u32 __user *)args, &mpu_result.temp, sizeof(mpu_temp_data_t));
		if (ret) {
			mpu_error("MPU_GET_TEMP, copy_to_user failed, ret = %d", ret);
			return -EFAULT;
		}
		break;
	case MPU_GET_RESULT:
		ret = mpu6050_get_result(&mpu_result);
		ret = copy_to_user((u32 __user *)args, &mpu_result, sizeof(mpu_result_t));
		if (ret) {
			mpu_error("MPU_GET_RESULT, copy_to_user failed, ret = %d", ret);
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


#ifndef	__I2C_H__
#define	__I2C_H__

#include "hi3518ev200/hi3518ev200.h"


#define	I2C_NR		0


int i2c_init(void)
{
	I2C_UNLOCK(I2C_NR);
	I2C_NORMAL_SCL_HCNT_SET(I2C_NR);
	I2C_NORMAL_SCL_LCNT_SET(I2C_NR);
	I2C_SDA_HOLD_SET(I2C_NR);
	I2C_MASTER_RESTART_ENABLE(I2C_NR);
	I2C_INTR_SET_MASK(I2C_NR);
	I2C_RX_TL_SET(I2C_NR);
	I2C_TX_TL_SET(I2C_NR);
	I2C_SLAVER_7BITS_ADDR_SET(I2C_NR, 0xC0);
	I2C_ENABLE(I2C_NR);
	I2C_MST_SINGLE_MODEL_ENABLE(I2C_NR);

	return 0;
}

int i2c_checkout_fifo_notfull(void)
{

	return 0;
}

int i2c_start(void)
{

	return 0;
}

int i2c_stop(void)
{


	return 0;
}

int i2c_ack(u8 ack_status)
{

	return 0;
}

int i2c_write_byte(u8 data)
{


	return 0;
}

int i2c_write()
{

	return 0;
}

int i2c_read_byte(u8 *data)
{

	return 0;
}

int i2c_test_scl_gpio(void)
{

	return 0;
}

int i2c_test_sda_gpio(void)
{

	return 0;
}

#endif	/* __I2C_H__ */

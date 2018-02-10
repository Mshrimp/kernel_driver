#ifndef	__I2C_H__
#define	__I2C_H__

int i2c_init(void);

int i2c_start(void);

int i2c_stop(void);

int i2c_ack(u8 ack_status);

int i2c_write_byte(u8 data);

int i2c_read_byte(u8 *data);

int i2c_test_scl_gpio(void);

int i2c_test_sda_gpio(void);

#endif	/* __I2C_H__ */

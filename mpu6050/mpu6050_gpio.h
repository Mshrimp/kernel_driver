#ifndef	__MPU6050_H__
#define	__MPU6050_H__

#include <asm/ioctl.h>

typedef enum {
	MPU_INIT = 0x0,
	MPU_WRITE_REG = 0x1,
	MPU_WRITE_REGS = 0x2,
	MPU_READ_REG = 0x3,
	MPU_READ_REGS = 0x4,
	MPU_WRITE_REGS_DATA = 0x5,
	MPU_GET_ACCEL = 0x6,
	MPU_GET_GYRO = 0x7,
	MPU_GET_TEMP = 0x8,
	MPU_GET_RESULT = 0x9,
} mpu_cmd_e;

#define		MPU6050_IOC_MAGIC			'm'
#define		MPU6050_IOC_MAXNR			9
#define		MPU6050_IOC_INIT			_IO(MPU6050_IOC_MAGIC, MPU_INIT)
#define		MPU6050_IOC_WRITE_REG		_IO(MPU6050_IOC_MAGIC, MPU_WRITE_REG)
#define		MPU6050_IOC_WRITE_REGS		_IO(MPU6050_IOC_MAGIC, MPU_WRITE_REGS)
#define		MPU6050_IOC_READ_REG		_IO(MPU6050_IOC_MAGIC, MPU_READ_REG)
#define		MPU6050_IOC_READ_REGS		_IO(MPU6050_IOC_MAGIC, MPU_READ_REGS)
#define		MPU6050_IOC_GET_ACCEL		_IO(MPU6050_IOC_MAGIC, MPU_GET_ACCEL)
#define		MPU6050_IOC_GET_GYRO		_IO(MPU6050_IOC_MAGIC, MPU_GET_GYRO)
#define		MPU6050_IOC_GET_TEMP		_IO(MPU6050_IOC_MAGIC, MPU_GET_TEMP)
#define		MPU6050_IOC_GET_RESULT		_IO(MPU6050_IOC_MAGIC, MPU_GET_RESULT)



typedef	struct {
	unsigned char reg;
	unsigned char data;
} mpu_reg_t;

typedef struct {
	unsigned char *data;
	unsigned int len;
	unsigned char reg;
} mpu_regs_t;

typedef struct {
	mpu_reg_t *reg_data;
	unsigned int len;
} mpu_regs_data_t;

typedef struct {
	mpu_reg_t xout_h;
	mpu_reg_t xout_l;
	mpu_reg_t yout_h;
	mpu_reg_t yout_l;
	mpu_reg_t zout_h;
	mpu_reg_t zout_l;
} mpu_accel_t;

typedef struct {
	short xout;
	short yout;
	short zout;
} mpu_accel_data_t;

typedef struct {
	mpu_reg_t temp_h;
	mpu_reg_t temp_l;
} mpu_temp_t;

typedef struct {
	short temp;
} mpu_temp_data_t;

typedef struct {
	mpu_reg_t xout_h;
	mpu_reg_t xout_l;
	mpu_reg_t yout_h;
	mpu_reg_t yout_l;
	mpu_reg_t zout_h;
	mpu_reg_t zout_l;
} mpu_gyro_t;

typedef struct {
	short xout;
	short yout;
	short zout;
} mpu_gyro_data_t;

typedef struct {
	mpu_accel_data_t accel;
	mpu_gyro_data_t gyro;
	mpu_temp_data_t temp;
} mpu_result_t;

int mpu6050_init(void);

int mpu6050_uninit(void);

int mpu6050_operation(unsigned int cmd, unsigned long args);


#endif	/* __MPU6050_H__ */


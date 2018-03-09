#ifndef	__APP_MPU6050_H__
#define	__APP_MPU6050_H__


typedef	struct {
	float xout;
	float yout;
	float zout;
} app_mpu_accel_t;

typedef	struct {
	float xout;
	float yout;
	float zout;
} app_mpu_gyro_t;

typedef	struct {
	app_mpu_accel_t accel;
	app_mpu_gyro_t gyro;
} app_mpu_result_t;



#endif	/* __APP_MPU6050_H__ */


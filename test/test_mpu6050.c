#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include "../mpu6050/mpu6050_gpio.h"

#define	DRIVER_NAME			"/dev/driver_mpu6050"


#define	app_debug(fmt, args...)		\
			printf("App mpu6050 debug: "fmt"(func: %s, line: %d)\n", ##args, __func__, __LINE__);
#define	app_error(fmt, args...)		\
			printf("App mpu6050 error: "fmt"(func: %s, line: %d)\n", ##args, __func__, __LINE__);

void show_cmd(void)
{
	printf("*******************************************\n");
	printf("0: MPU6050 init\n");
	printf("1: MPU6050 write reg\n");
	printf("2: MPU6050 write regs\n");
	printf("3: MPU6050 read reg\n");
	printf("4: MPU6050 read regs\n");
	printf("5: MPU6050 write regs and datas\n");
	printf("6: MPU6050 get accel out\n");
	printf("7: MPU6050 get gyro out\n");
	printf("8: MPU6050 get temp out\n");
	printf("9: MPU6050 get result\n");
	printf("10: MPU6050 set range\n");
	printf("11: MPU6050 get range\n");
	printf("99: Quit\n");
	printf("*******************************************\n");
}

void show_accel(mpu_accel_data_t mpu_accel)
{
	printf("mpu6050 accel:\n");
	printf("\txout: %d\n", mpu_accel.xout);
	printf("\tyout: %d\n", mpu_accel.yout);
	printf("\tzout: %d\n", mpu_accel.zout);
}

void show_gyro(mpu_gyro_data_t mpu_gyro)
{
	printf("mpu6050 gyro:\n");
	printf("\txout: %d\n", mpu_gyro.xout);
	printf("\tyout: %d\n", mpu_gyro.yout);
	printf("\tzout: %d\n", mpu_gyro.zout);
}

void show_temp(mpu_temp_data_t mpu_temp)
{
	printf("mpu6050 temp: %d\n", mpu_temp.temp);
}

void show_result(mpu_result_t mpu_result)
{
	printf("mpu6050 result:\n");
	printf("\t\taccel\tgyro\n");
	printf("\txout: \t%d\t%d\n", mpu_result.accel.xout, mpu_result.gyro.xout);
	printf("\tyout: \t%d\t%d\n", mpu_result.accel.yout, mpu_result.gyro.yout);
	printf("\tzout: \t%d\t%d\n", mpu_result.accel.zout, mpu_result.gyro.zout);
	printf("\ttemp: \t%d\n", mpu_result.temp.temp);
}

int main(int argc,char **args)
{
	int fd = -1;
	int ret = -1;
	int input = 0;
	unsigned long status;
	mpu_accel_data_t mpu_accel;
	mpu_gyro_data_t mpu_gyro;
	mpu_temp_data_t mpu_temp;
	mpu_result_t mpu_result;
	mpu_range_t mpu_range;

	fd = open(DRIVER_NAME, O_RDWR | O_NONBLOCK);
	if(fd < 0) {
		app_debug("open %s error, fd = %d", DRIVER_NAME, fd);
		return -1;
	}

	while (1) {
		show_cmd();
		scanf("%d", &input);
		printf("Input: %d\n", input);

		if (99 == input) {
			break;
		}

		switch (input) {
		case MPU_INIT:
			app_debug("mpu6050 init");
			ret = ioctl(fd, MPU6050_IOC_INIT);
			break;
		case MPU_GET_ACCEL:
			ret = ioctl(fd, MPU6050_IOC_GET_ACCEL, &mpu_accel);
			show_accel(mpu_accel);
			break;
		case MPU_GET_GYRO:
			ret = ioctl(fd, MPU6050_IOC_GET_GYRO, &mpu_gyro);
			show_gyro(mpu_gyro);
			break;
		case MPU_GET_TEMP:
			ret = ioctl(fd, MPU6050_IOC_GET_TEMP, &mpu_temp);
			show_temp(mpu_temp);
			break;
		case MPU_GET_RESULT:
			ret = ioctl(fd, MPU6050_IOC_GET_RESULT, &mpu_result);
			show_result(mpu_result);
			break;
		case MPU_SET_RANGE:
			ret = ioctl(fd, MPU6050_IOC_SET_RANGE, &mpu_range);
			break;
		case MPU_GET_RANGE:
			ret = ioctl(fd, MPU6050_IOC_GET_RANGE, &mpu_range);
			break;
		default:
			app_error("wrong cmd");
			break;
		}

		if(ret) {
			app_error("iotrl error, ret = %d", ret);
		}

	}

	close(fd);

	return 0;
}




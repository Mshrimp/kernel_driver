#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

#include "../mpu6050/mpu6050_gpio.h"
#include "app_mpu6050.h"

#define	DRIVER_NAME			"/dev/driver_mpu6050"

#define	DATA_FILE			"mpu6050_data"
#define	BUF_SIZE			128

#define	DATA_CNT			1000

#define	app_debug(fmt, args...)		\
			printf("App mpu6050 debug: "fmt"(func: %s, line: %d)\n", ##args, __func__, __LINE__);
#define	app_error(fmt, args...)		\
			printf("App mpu6050 error: "fmt"(func: %s, line: %d)\n", ##args, __func__, __LINE__);

#define	PI						3.1415926

#define	MPU6050_GYRO_RANGE		2000
#define	MPU6050_ACCEL_RANGE		(16 * 9.8)
#define	MPU6050_GROUND			9.8


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

double rad_2_angle(double rad)
{
	double angle = 0;

	angle = (rad / PI) * 180;

	return angle;
}

double calc_result(short out)
{
	float data = 0;
	double rad;
	
	rad = (double)(out) / (0x7FFF);

	return data;
}

int main(int argc,char **args)
{
	int fd = -1;
	int file_fd = -1;
	FILE *fp = NULL;
	int ret = -1;
	int i = 0;
	unsigned long status;
	mpu_accel_data_t mpu_accel;
	mpu_gyro_data_t mpu_gyro;
	mpu_temp_data_t mpu_temp;
	mpu_result_t mpu_result;
	mpu_range_t mpu_range;
	char buffer[BUF_SIZE] = {0};

	fd = open(DRIVER_NAME, O_RDWR | O_NONBLOCK);
	if(fd < 0) {
		app_error("open %s error, fd = %d", DRIVER_NAME, fd);
		return -1;
	}

    /*
	 *ret = ioctl(fd, MPU6050_IOC_INIT);
	 *if (ret) {
	 *    app_error("ioctl MPU6050_IOC_INIT failed");
	 *    return -1;
	 *}
     */

	mpu_range.accel = 3;
	mpu_range.gyro = 3;
	ret = ioctl(fd, MPU6050_IOC_SET_RANGE, &mpu_range);
	if (ret) {
		app_error("ioctl MPU6050_IOC_SET_RANGE failed");
		return -1;
	}

    /*
	 *file_fd = open(DATA_FILE, O_RDWR | O_CREAT);
	 *if (file_fd < 0) {
	 *    app_error("open %s failed", DATA_FILE);
	 *    return -1;
	 *}
     */

	fp = fopen(DATA_FILE, "w+");
	if (fp == NULL) {
		app_error("fopen %s failed", DATA_FILE);
		return -1;
	}

	i = 0;
	while (i < DATA_CNT) {
		memset(&mpu_result, 0, sizeof(mpu_result_t));
		ret = ioctl(fd, MPU6050_IOC_GET_RESULT, &mpu_result);
		if (ret) {
			app_error("ioctl MPU6050_IOC_GET_RESULT failed");
			return -1;
		}
		snprintf(buffer, BUF_SIZE, "%d\t%d\t%d\t%d\t%d\t%d\t%d\n",
				i, mpu_result.accel.xout, mpu_result.accel.yout, mpu_result.accel.zout,
				mpu_result.gyro.xout, mpu_result.gyro.yout, mpu_result.gyro.zout);

        /*
		 *ret = write(file_fd, buffer, BUF_SIZE);
		 *if (ret < 0)
		 *{
		 *    app_error("write data to file failed, ret = %d", ret);
		 *    return -1;
		 *}
         */

        /*
		 *ret = fwrite(buffer, BUF_SIZE, 1, fp);
		 *if (ret < 0)
		 *{
		 *    app_error("write data to file failed, ret = %d", ret);
		 *    return -1;
		 *}
         */

		ret = fprintf(fp, buffer);
		if (ret < 0)
		{
			app_error("write data to file failed, ret = %d", ret);
			return -1;
		}

		//show_result(mpu_result);
		i++;
		usleep(1000);
	}

	fclose(fp);

	close(fd);

	return 0;
}




#include <stdio.h>
#include <math.h>

#define	PI				3.1415926

#define	ACCEL_RANGE		(16 * 9.8)
#define	GYRO_RANGE		2000
#define	G_NUM			9.8
#define	FULL_RANGE		0x7FFF

double short_2_accel(short out)
{
	double accel;

	if (out > 0) {
		accel = ((double)(out) / FULL_RANGE) * (double)(ACCEL_RANGE);
	} else {
		accel = ((double)(out + 1) / FULL_RANGE) * (double)(ACCEL_RANGE);
	}

	return accel;
}

double short_2_gyro(short out)
{
	double gyro;

	if (out > 0) {
		gyro = ((double)(out) / FULL_RANGE) * (double)(GYRO_RANGE);
	} else {
		gyro = ((double)(out + 1) / FULL_RANGE) * (double)(GYRO_RANGE);
	}

	return gyro;
}

double rad_2_ang(double rad)
{
	double angle = 0;

	angle = (rad / PI) * 180;

	return angle;
}

double calc_result(double out)
{
	double rad = 0;

	rad = (double)(out) / 0x7FFF;

	return rad;
}

int main(void)
{
	int input = 0;
	short out = 0;
	double accel, gyro;
	double rad, angle;

	printf("Input a number as out(0xXXXX):\n");
	scanf("%X", &input);
	printf("input = %d, 0x%X\n", input, input);

	out = (short)input;
	printf("out = %d, 0x%X\n", out, out);
	printf("out = %d, 0x%X\n", out + 1, out + 1);

	accel = short_2_accel(out);
	printf("accel = %f\n", accel);
	gyro = short_2_gyro(out);
	printf("gyro = %f\n", gyro);

	rad = calc_result(out);

	printf("rad = %f\n", rad);

	angle = rad_2_ang(rad);

	printf("angle = %f\n", angle);

	printf("out = 0x%X, rad = %f, angle = %f\n", out, rad, angle);

	return 0;
}


#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include "../fm36/fm36.h"

#define	DRIVER_NAME			"/dev/hi3518ev200"


#define	app_debug(fmt, args...)		\
			printf("App fm36 debug: "fmt"(func: %s, line: %d)\n", ##args, __func__, __LINE__);
#define	app_error(fmt, args...)		\
			printf("App fm36 error: "fmt"(func: %s, line: %d)\n", ##args, __func__, __LINE__);

void show_cmd(void)
{
	printf("*******************************************\n");
	printf("0: FM36 chip reset\n");
	printf("1: FM36 init register\n");
	printf("2: FM36 read data\n");
	printf("3: FM36 write data\n");
	printf("9: Quit\n");
	printf("*******************************************\n");
}


int main(int argc,char **args)
{
	int fd = -1;
	int ret = -1;
	int input = -1;
	fm36_read_reg_t read_data;

	fd = open(DRIVER_NAME, O_RDWR | O_NONBLOCK);
	if(fd < 0) {
		app_debug("open %s error, fd = %d", DRIVER_NAME, fd);
		return -1;
	}

	while (1) {
		show_cmd();
		scanf("%d", &input);
		printf("Input: %d\n", input);

		if (9 == input) {
			break;
		}

		switch (input) {
		case FM36_CHIP_RESET:
			app_debug("fm36 chip reset");
			ret = ioctl(fd, FM36_IOC_CHIP_RESET);
			break;
		case FM36_INIT_REGISTER:
			app_debug("fm36 init register");
			ret = ioctl(fd, FM36_IOC_INIT_REGISTER);
			break;
		case FM36_READ_DATA:
			app_debug("fm36 read register data");
			printf("Input register address:(example: 0x230C) ");
			scanf("%X", &read_data.reg);
			app_debug("read register address: 0x%X", read_data.reg);
			ret = ioctl(fd, FM36_IOC_READ_DATA, &read_data);
			break;
		case FM36_WRITE_DATA:
			app_debug("fm36 write register data");
			ret = ioctl(fd, FM36_IOC_WRITE_DATA);
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




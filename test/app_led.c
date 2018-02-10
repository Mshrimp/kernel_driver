#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include "../led/led.h"

#define	DRIVER_NAME			"/dev/hi3518ev200"


#define	app_debug(fmt, args...)		\
			printf("App led debug: "fmt"(func: %s, line: %d)\n", ##args, __func__, __LINE__);
#define	app_error(fmt, args...)		\
			printf("App led error: "fmt"(func: %s, line: %d)\n", ##args, __func__, __LINE__);

void show_cmd(void)
{
	printf("*******************************************\n");
	printf("0: LED on\n");
	printf("1: LED off\n");
	printf("2: LED get status\n");
	printf("9: Quit\n");
	printf("*******************************************\n");
}


int main(int argc,char **args)
{
	int fd = -1;
	int ret = -1;
	int input = -1;
	unsigned long status;

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
		case LED_ON:
			app_debug("led on");
			ret = ioctl(fd, LED_IOC_ON);
			break;
		case LED_OFF:
			app_debug("led off");
			ret = ioctl(fd, LED_IOC_OFF);
			break;
		case LED_GET_STATUS:
			app_debug("led get status");
			ret = ioctl(fd, LED_IOC_GET_STATUS, &status);
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




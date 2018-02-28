#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include "../oled/oled_gpio.h"

#define	DRIVER_NAME			"/dev/h3"


#define	app_debug(fmt, args...)		\
			printf("App oled debug: "fmt"(func: %s, line: %d)\n", ##args, __func__, __LINE__);
#define	app_error(fmt, args...)		\
			printf("App oled error: "fmt"(func: %s, line: %d)\n", ##args, __func__, __LINE__);

void show_cmd(void)
{
	printf("*******************************************\n");
	printf("0: OLED init\n");
	printf("1: OLED clear\n");
	printf("2: OLED full\n");
	printf("3: OLED test\n");
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
		case OLED_INIT:
			app_debug("oled init");
			ret = ioctl(fd, OLED_IOC_INIT);
			break;
		case OLED_CLEAR:
			app_debug("oled clear");
			ret = ioctl(fd, OLED_IOC_CLEAR);
			break;
		case OLED_FULL:
			app_debug("oled full");
			ret = ioctl(fd, OLED_IOC_FULL);
			break;
		case OLED_TEST:
			app_debug("oled test");
			ret = ioctl(fd, OLED_IOC_TEST);
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




#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <time.h>

#include "../oled/oled_gpio.h"

#define	DRIVER_NAME			"/dev/h3"


#define	app_debug(fmt, args...)		\
			printf("App oled debug: "fmt"(func: %s, line: %d)\n", ##args, __func__, __LINE__);
#define	app_error(fmt, args...)		\
			printf("App oled error: "fmt"(func: %s, line: %d)\n", ##args, __func__, __LINE__);

typedef enum {
	OLED_TIME = 0x5,
} oled_test_cmd_e;

void show_cmd(void)
{
	printf("*******************************************\n");
	printf("0: OLED init\n");
	printf("1: OLED clear\n");
	printf("2: OLED full\n");
	printf("3: OLED show char\n");
	printf("4: OLED show string\n");
	printf("5: OLED show time\n");
	printf("8: OLED test\n");
	printf("9: Quit\n");
	printf("*******************************************\n");
}


int main(int argc,char **args)
{
	int fd = -1;
	int ret = -1;
	int input = -1;
	unsigned long status;
	oled_char_t oled_char;
	oled_str_t oled_str;
	time_t timep;
	struct tm time_now;

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
		case OLED_CHAR:
			app_debug("oled show char");
			printf("Input the position to show in oled:(a, b)\n");
			scanf("%d, %d", &oled_char.row, &oled_char.col);
			getchar();
			printf("Input the char to show:\n");
			scanf("%c", &oled_char.ch);
			printf("Show char %c in oled at positon:(%d, %d)\n",
						oled_char.ch, oled_char.row, oled_char.col);
			ret = ioctl(fd, OLED_IOC_CHAR, &oled_char);
			break;
		case OLED_STR:
			app_debug("oled show string");
			printf("Input the position to show in oled:(a, b)\n");
			scanf("%d, %d", &oled_str.row, &oled_str.col);
			getchar();
			printf("Input the string to show:\n");
			//scanf("%c", &oled_char.ch);
			fgets(oled_str.str, OLED_8_16_BUF_SIZE, stdin);
			if (oled_str.str[strlen(oled_str.str) - 1] == '\n') {
				oled_str.str[strlen(oled_str.str) - 1] = '\0';
			}
			printf("len: %d\n", strlen(oled_str.str));
			printf("Show string %s in oled at positon:(%d, %d)\n",
						oled_str.str, oled_str.row, oled_str.col);
			ret = ioctl(fd, OLED_IOC_STR, &oled_str);
			break;
		case OLED_TIME:
			app_debug("oled show time");
			timep = time(NULL);
			oled_str.row = 2;
			oled_str.col = 0;
			strncpy(oled_str.str, asctime(gmtime(&timep)), OLED_8_16_BUF_SIZE);
			printf("Show string %s in oled at positon:(%d, %d)\n",
						oled_str.str, oled_str.row, oled_str.col);

			ret = ioctl(fd, OLED_IOC_STR, &oled_str);
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




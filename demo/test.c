#include <stdio.h>


typedef struct {
	unsigned char direction;
	unsigned char speed;
	unsigned char col_step;
	unsigned char start_page;
	unsigned char stop_page;
} oled_scroll_t;

int main(void)
{
	oled_scroll_t oled_scroll;

	printf("oled vertical and horizontal scroll\n");
	printf("Input the direction to scroll:(0: left, 1: right)\n");
	scanf("%d", &oled_scroll.direction);
	printf("Input the speed to scroll:(0 ~ 7)\n");
	scanf("%d", &oled_scroll.speed);
	printf("Input the col step to scroll:(0 ~ 7)\n");
	scanf("%d", &oled_scroll.col_step);
	printf("oled_scroll.col_step = %d\n", oled_scroll.col_step);
	printf("Input the page to scroll:(a, b)(0 ~ 7)\n");
	scanf("%d, %d", &oled_scroll.start_page, &oled_scroll.stop_page);
	printf("oled_scroll.col_step = %d\n", oled_scroll.col_step);

	printf("direction: %d, page: (%d, %d), speed: %d, col_step: %d\n",
			oled_scroll.direction, oled_scroll.start_page, oled_scroll.stop_page,
			oled_scroll.speed, oled_scroll.col_step);

	return 0;
}


#include <linux/spinlock.h>

//#include "../common.h"
#include "../chip/chip.h"
#include "../i2c/i2c_gpio.h"
#include "oled_gpio.h"
#include "oled_font.h"

#define	SSD1306_CHIP_ADDR		0x78

#define	OLED_CHIP_ADDR			SSD1306_CHIP_ADDR

#define	OLED_MAX_ROW			8
#define	OLED_MAX_COL			128

#define	oled_debug(fmt, args...)		\
			printk("OLED debug: "fmt"(func: %s, line: %d)\n", ##args, __func__, __LINE__);
#define	oled_error(fmt, args...)		\
			printk("OLED error: "fmt"(func: %s, line: %d)\n", ##args, __func__, __LINE__);

typedef struct {
	i2c_gpio_t gpio;
	spinlock_t lock;
} oled_info_t;

static oled_info_t oled_info = {
	.gpio = {
		.scl = {
			.group = GPIOG,
			.bit = 11,
		},
		.sda = {
			.group = GPIOA,
			.bit = 6,
		},
	},
};

int oled_write_commond(unsigned char cmd)
{
	//oled_debug("oled_write_commond: 0x%X", cmd);
	i2c_start();
	i2c_write_byte_with_ack(OLED_CHIP_ADDR);
	i2c_write_byte_with_ack(0x00);
	i2c_write_byte_with_ack(cmd);
	i2c_stop();

	return 0;
}

int oled_write_data(unsigned char data)
{
	//oled_debug("oled_write_data: 0x%X", data);
	i2c_start();
	i2c_write_byte_with_ack(OLED_CHIP_ADDR);
	i2c_write_byte_with_ack(0x40);
	i2c_write_byte_with_ack(data);
	i2c_stop();

	return 0;
}

int oled_chip_init(void)
{
	oled_write_commond(0xAE);
	oled_write_commond(0x20);
	oled_write_commond(0x10);
	oled_write_commond(0xB0);
	oled_write_commond(0xC8);
	oled_write_commond(0x00);
	oled_write_commond(0x10);
	oled_write_commond(0x40);
	oled_write_commond(0x81);
	oled_write_commond(0x7F);
	oled_write_commond(0xA1);
	oled_write_commond(0xA6);
	oled_write_commond(0xA8);
	oled_write_commond(0x3F);
	oled_write_commond(0xA4);
	oled_write_commond(0xD3);
	oled_write_commond(0x00);
	oled_write_commond(0xD5);
	oled_write_commond(0xF0);
	oled_write_commond(0xD9);
	oled_write_commond(0x22);
	oled_write_commond(0xDA);
	oled_write_commond(0x12);
	oled_write_commond(0xDB);
	oled_write_commond(0x20);
	oled_write_commond(0x8D);
	oled_write_commond(0x14);
	oled_write_commond(0xAF);

	return 0;
}

int oled_show_char(void)
{
	int len = 0;
	unsigned char cnt, col;

	len = sizeof(font) / sizeof(font[1]);

	oled_debug("font size: %d", len);

	oled_write_commond(0xB0);
	oled_write_commond(0x00);
	oled_write_commond(0x10);

	for (cnt = 0; cnt < len; cnt++) {
		for (col = 0; col < 6; col++) {
			oled_write_data(font[cnt][col]);
		}
	}

	return 0;
}

int oled_fill_screen(unsigned char data)
{
	unsigned char row, col;
	oled_debug("oled_fill_screen: 0x%X", data);

	for (row = 0; row < OLED_MAX_ROW; row++) {
		oled_write_commond(0xB0 | row);
		oled_write_commond(0x00);
		oled_write_commond(0x10);

		for (col = 0; col < OLED_MAX_COL; col++) {
			oled_write_data(data);
		}
	}

	return 0;
}

int oled_init(void)
{
	int ret = -1;
	printk("oled_init\n");

	spin_lock_init(&oled_info.lock);

	spin_lock(&oled_info.lock);
	spin_unlock(&oled_info.lock);

	ret = i2c_init(&oled_info.gpio);
	if (ret) {
		oled_error("i2c init failed, ret = %d", ret);
		return -1;
	}

	return 0;

}

int oled_uninit(void)
{
	printk("oled_uninit\n");

	return 0;
}

int oled_operation(unsigned int cmd, unsigned long args)
{
	int ret = -1;

	switch (cmd) {
	case OLED_IOC_INIT:
		ret = oled_chip_init();
		break;
	case OLED_IOC_CLEAR:
		ret = oled_fill_screen(0x00);
		break;
	case OLED_IOC_FULL:
		ret = oled_fill_screen(0xFF);
		break;
	case OLED_IOC_TEST:
		ret = oled_show_char();
		break;
	default:
		oled_error("cmd error no = %d", cmd);
		ret = -EINVAL;
		break;
	}

	if (ret) {
		oled_error("oled operation failed");
	}

	return ret;
}


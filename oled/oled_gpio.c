#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/fs.h>
#include <linux/device.h>
#include <linux/spinlock.h>
#include <linux/uaccess.h>

#include "../chip/chip.h"
#include "../i2c/i2c_gpio.h"
#include "oled_gpio.h"
#include "font.h"
#include "font_ascii.h"

#define DEV_NAME				"driver_oled_gpio"

#define	SSD1306_CHIP_ADDR		0x78

#define	OLED_CHIP_ADDR			SSD1306_CHIP_ADDR

#define	OLED_MAX_ROW			64
#define	OLED_MAX_COL			128
#define	OLED_MAX_PAGE			(OLED_MAX_ROW / 8)

#define	oled_debug(fmt, args...)		\
			printk("OLED debug: "fmt"(func: %s, line: %d)\n", ##args, __func__, __LINE__);
#define	oled_error(fmt, args...)		\
			printk("OLED error: "fmt"(func: %s, line: %d)\n", ##args, __func__, __LINE__);


typedef struct {
	unsigned char row;
	unsigned char col;
} oled_pos_t;

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

static oled_pos_t oled_pos = {
	.row = 0,
	.col = 0,
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

int oled_show_hello(void)
{
	int len = 0;
	unsigned char cnt, col;

	len = sizeof(font) / sizeof(font[0]);

	oled_debug("font size: %d", len);

	oled_write_commond(0xB0);
	oled_write_commond(0x00);
	oled_write_commond(0x20);

	for (cnt = 0; cnt < len; cnt++) {
		for (col = 0; col < 6; col++) {
			oled_write_data(font[cnt][col]);
		}
	}

	return 0;
}

int oled_show_char_8_16(unsigned char row, unsigned char col, unsigned char ch)
{
	int i = 0;
	if ((row > OLED_MAX_PAGE - 2) || (col > OLED_MAX_COL - 8)) {
		oled_error("row col error, row = %d, col = %d", row, col);
		return -EINVAL;
	}

	if ((ch - ' ' < 0) || (ch > 0x7F)) {
		oled_error("ch error, ch = %c", ch);
		return -EINVAL;
	}

	oled_debug("oled_show_char_8_16, char: %c, %d", ch, ch);

	oled_write_commond(0xB0 | row);
	oled_write_commond(0x00 | (col & 0x0F));
	oled_write_commond(0x10 | ((col & 0xF0) >> 4));

	for (i = 0; i < 8; i++) {
		oled_write_data(font_ascii[ch - ' '][i]);
	}

	oled_write_commond(0xB0 | (row + 1));
	oled_write_commond(0x00 | (col & 0x0F));
	oled_write_commond(0x10 | ((col & 0xF0) >> 4));

	for (i = 8; i < 16; i++) {
		oled_write_data(font_ascii[ch - ' '][i]);
	}

	return 0;
}

int oled_show_string_8_16(unsigned char row, unsigned char col, unsigned char *str)
{
	int i = 0;
	unsigned char pos_row, pos_col;

	if ((row > OLED_MAX_PAGE - 2) || (col > OLED_MAX_COL - 8)) {
		oled_error("row col error, row = %d, col = %d", row, col);
		return -EINVAL;
	}

	oled_debug("oled_show_string_8_16, str: %s", str);

	pos_row = row;
	pos_col = col;

	while ((*(str + i) != '\0') && (*(str + i) != '\n')) {
		if (pos_col + 8 - 1 < OLED_MAX_COL) {
			oled_show_char_8_16(pos_row, pos_col, *(str + i));
			pos_col += 8;
			i++;
		} else {
			if (pos_row + 2 < OLED_MAX_PAGE) {
				pos_row += 2;
				pos_col = 0;
			} else {
				oled_error("oled write to the end, pos_row = %d, pos_col = %d", pos_row, pos_col);
				return -EFAULT;
			}
		}
	}

	return 0;
}

int oled_horizontal_scroll(oled_scroll_t *oled_scroll_info)
{
	oled_debug("oled_horizontal_scroll");

	if ((oled_scroll_info->start_page >= OLED_MAX_PAGE)
			|| (oled_scroll_info->stop_page >= OLED_MAX_PAGE)) {
		oled_error("page error, start_page = %d, stop_page = %d",
					oled_scroll_info->start_page, oled_scroll_info->stop_page);
		return -EINVAL;
	}

	if (oled_scroll_info->speed >= 7) {
		oled_error("page error, speed = %d", oled_scroll_info->speed);
		return -EINVAL;
	}

	oled_debug("direction: %d, page: (%d, %d), speed: %d",
				oled_scroll_info->direction, oled_scroll_info->start_page,
				oled_scroll_info->stop_page, oled_scroll_info->speed);

	oled_write_commond(0x2E);

	if (oled_scroll_info->direction > 0) {	// Scroll right
		oled_write_commond(0x26);
	} else {	// Scroll left
		oled_write_commond(0x27);
	}
	oled_write_commond(0x00);
	oled_write_commond(oled_scroll_info->start_page & ((0x1 << 3) - 1));
	oled_write_commond(oled_scroll_info->speed & ((0x1 << 3) - 1));
	oled_write_commond(oled_scroll_info->stop_page & ((0x1 << 3) - 1));
	oled_write_commond(0x00);
	oled_write_commond(0xFF);
	oled_write_commond(0x2F);

	return 0;
}

int oled_vertical_horizontal_scroll(oled_scroll_t *oled_scroll_info)
{
	oled_debug("oled_vertical_horizontal_scroll");

	if ((oled_scroll_info->start_page >= OLED_MAX_PAGE)
			|| (oled_scroll_info->stop_page >= OLED_MAX_PAGE)) {
		oled_error("page error, start_page = %d, stop_page = %d",
					oled_scroll_info->start_page, oled_scroll_info->stop_page);
		return -EINVAL;
	}

	if (oled_scroll_info->speed >= 7) {
		oled_error("page error, speed = %d", oled_scroll_info->speed);
		return -EINVAL;
	}

	if (oled_scroll_info->col_step >= OLED_MAX_PAGE) {
		oled_error("page error, col_step = %d", oled_scroll_info->col_step);
		return -EINVAL;
	}

	oled_debug("direction: %d, page: (%d, %d), speed: %d, col_step: %d",
				oled_scroll_info->direction, oled_scroll_info->start_page,
				oled_scroll_info->stop_page, oled_scroll_info->speed,
				oled_scroll_info->col_step);
	oled_write_commond(0x2E);

	if (oled_scroll_info->direction > 0) {	// Scroll right
		oled_write_commond(0x29);
	} else {	// Scroll left
		oled_write_commond(0x2A);
	}
	oled_write_commond(0x00);
	oled_write_commond(oled_scroll_info->start_page & ((0x1 << 3) - 1));
	oled_write_commond(oled_scroll_info->speed & ((0x1 << 3) - 1));
	oled_write_commond(oled_scroll_info->stop_page & ((0x1 << 3) - 1));
	oled_write_commond(oled_scroll_info->col_step & ((0x1 << 6) - 1));
	oled_write_commond(0x2F);

	return 0;
}

int oled_stop_scroll(void)
{
	oled_write_commond(0x2E);

	return 0;
}

int oled_fill_screen(unsigned char data)
{
	unsigned char row, col;
	oled_debug("oled_fill_screen: 0x%X", data);

	for (row = 0; row < OLED_MAX_PAGE; row++) {
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

	oled_chip_init();

	return 0;

}

int oled_uninit(void)
{
	printk("oled_uninit\n");

	oled_write_commond(0x2E);
	oled_write_commond(0xAE);

	return 0;
}

int oled_operation(unsigned int cmd, unsigned long args)
{
	int ret = -1;
	oled_char_t oled_char;
	oled_str_t oled_str;
	oled_scroll_t oled_scroll;

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
	case OLED_IOC_CHAR:
		ret = copy_from_user(&oled_char, (u32 __user *)args, sizeof(oled_char_t));
		if (ret) {
			oled_error("OLED_IOC_CHAR, copy_form_user failed, ret = %d", ret);
			return -EFAULT;
		}

		ret = oled_show_char_8_16(oled_char.row, oled_char.col, oled_char.ch);
		if (ret) {
			oled_error("oled_show_char_8_16 failed");
			return -EFAULT;
		}
		break;
	case OLED_IOC_STR:
		ret = copy_from_user(&oled_str, (u32 __user *)args, sizeof(oled_str_t));
		if (ret) {
			oled_error("OLED_IOC_STR, copy_form_user failed, ret = %d", ret);
			return -EFAULT;
		}

		ret = oled_show_string_8_16(oled_str.row, oled_str.col, oled_str.str);
		if (ret) {
			oled_error("oled_show_string_8_16 failed");
			return -EFAULT;
		}
		break;
	case OLED_IOC_SCROLL_H:
		ret = copy_from_user(&oled_scroll, (u32 __user *)args, sizeof(oled_scroll_t));
		if (ret) {
			oled_error("OLED_IOC_SCROLL_H, copy_form_user failed, ret = %d", ret);
			return -EFAULT;
		}

		ret = oled_horizontal_scroll(&oled_scroll);
		if (ret) {
			oled_error("oled_horizontal_scroll failed");
			return -EFAULT;
		}
		break;
	case OLED_IOC_SCROLL_H_V:
		ret = copy_from_user(&oled_scroll, (u32 __user *)args, sizeof(oled_scroll_t));
		if (ret) {
			oled_error("OLED_IOC_SCROLL_V, copy_form_user failed, ret = %d", ret);
			return -EFAULT;
		}

		ret = oled_vertical_horizontal_scroll(&oled_scroll);
		if (ret) {
			oled_error("oled_vertical_horizontal_scroll failed");
			return -EFAULT;
		}
		break;
	case OLED_IOC_STOP_SCROLL:
		ret = oled_stop_scroll();
		break;
	case OLED_IOC_TEST:
		//ret = oled_show_string_8_16(2, 0, "Hello world!");
		ret = oled_show_char_8_16(2, 0, 'H');
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

long driver_oled_ioctl (struct file *filp, unsigned int cmd, unsigned long arg)
{
	int ret = -1;
	printk("Driver: oled ioctl!\n");

	if (_IOC_TYPE(cmd) != OLED_IOC_MAGIC) {
		oled_error("ioctl cmd type error, type = %d", _IOC_TYPE(cmd));
		return -EINVAL;
	}

	if (_IOC_NR(cmd) > OLED_IOC_MAXNR) {
		oled_error("ioctl cmd nr error, nr = %d", _IOC_NR(cmd));
		return -EINVAL;
	}

	ret = oled_operation(_IOC_NR(cmd), arg);
	if (ret) {
		oled_error("oled_operation failed");
		return -EFAULT;
	}

	return ret;
}

ssize_t driver_oled_write (struct file *filp, const char __user *buf, size_t size, loff_t *offset)
{
	printk("Driver: oled write!\n");
	return size;
}

ssize_t driver_oled_read (struct file *filp, char __user *buf, size_t size, loff_t *offset)
{
	printk("Driver: oled read!\n");

	return size;
}

int driver_oled_open (struct inode *inodep, struct file *filp)
{
	printk("Driver: oled open!\n");

	oled_init();

	return 0;
}

int driver_oled_close (struct inode *inodep, struct file *filp)
{
	printk("Driver: oled close!\n");

	oled_uninit();

	return 0;
}

/////////////////////////////////////////////////////////////模块
struct file_operations fops = {
	.owner = THIS_MODULE,
	.open = driver_oled_open,
	.release = driver_oled_close,
	.read = driver_oled_read,
	.write = driver_oled_write,
	.unlocked_ioctl = driver_oled_ioctl,
};

int major = 0;
struct class *driver_class;
struct device *driver_class_device;

static int driver_oled_init(void)
{
	printk("Hello, driver chrdev register oled module init!\n");

	major = register_chrdev(major, DEV_NAME, &fops);
	if (major < 0) {
		oled_error("register_chrdev failed");
		goto ERR_dev_register;
	}
	printk("major = %d\n", major);

	driver_class = class_create(THIS_MODULE, "driver_class");
	if (!driver_class) {
		oled_error("class_create failed");
		goto ERR_class_create;
	}

	driver_class_device = device_create(driver_class, NULL, MKDEV(major, 0), NULL, "driver_class_device");
	if (!driver_class_device) {
		oled_error("device_create failed");
		goto ERR_class_device_create;
	}

	return 0;
ERR_class_device_create:
	device_destroy(driver_class, MKDEV(major, 0));
ERR_class_create:
	unregister_chrdev(major, DEV_NAME);
ERR_dev_register:
	return -1;
}

static void driver_oled_exit(void)
{
	printk("Goodbye, oled module exit!\n");
	class_destroy(driver_class);
	device_destroy(driver_class, MKDEV(major, 0));
	unregister_chrdev(major, DEV_NAME);
}


module_init(driver_oled_init);
module_exit(driver_oled_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Mshrimp");
MODULE_DESCRIPTION("OLED driver");
MODULE_VERSION("V0.1");

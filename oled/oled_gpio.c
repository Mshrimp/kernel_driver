#include <linux/spinlock.h>

//#include "../common.h"
#include "../chip/chip.h"
#include "../i2c/i2c_gpio.h"
#include "oled_gpio.h"

#define	SSD1306_CHIP_ADDR		0x78

#define	OLED_CHIP_ADDR			SSD1306_CHIP_ADDR

#define	oled_debug(fmt, args...)		\
			printk("OLED debug: "fmt"(func: %s, line: %d)\n", ##args, __func__, __LINE__);
#define	oled_error(fmt, args...)		\
			printk("OLED error: "fmt"(func: %s, line: %d)\n", ##args, __func__, __LINE__);


typedef struct {
    /*
	 *gpio_t sda;
	 *gpio_t scl;
     */
	spinlock_t lock;
} oled_info_t;

static oled_info_t oled_info = {

};




int oled_init(void)
{
	int ret = -1;
	printk("oled_init\n");

	spin_lock_init(&oled_info.lock);

	spin_lock(&oled_info.lock);
	spin_unlock(&oled_info.lock);

	ret = i2c_init();
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
	}

	return ret;
}


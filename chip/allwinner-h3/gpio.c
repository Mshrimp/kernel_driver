#include "gpio.h"

#define	gpio_debug(fmt, args...)		\
			printk("gpio debug: "fmt"(func: %s, line: %d)\n", ##args, __func__, __LINE__);
#define	gpio_error(fmt, args...)		\
			printk("gpio error: "fmt"(func: %s, line: %d)\n", ##args, __func__, __LINE__);


int set_gpio_output(gpio_t *gpio)
{
	SET_GPIO_DIR_OUTPUT(gpio->group, gpio->bit);

	return 0;
}

int set_gpio_input(gpio_t *gpio)
{
	SET_GPIO_DIR_INPUT(gpio->group, gpio->bit);

	return 0;
}

int set_gpio_output_high(gpio_t *gpio)
{
	SET_GPIO_OUTPUT_HIGH(gpio->group, gpio->bit);

	return 0;
}

int set_gpio_output_low(gpio_t *gpio)
{
	SET_GPIO_OUTPUT_LOW(gpio->group, gpio->bit);

	return 0;
}

int set_gpio_output_val(gpio_t *gpio, unsigned char val)
{
	//SET_GPIO_DIR_OUTPUT(gpio->group, gpio->bit);
	SET_GPIO_OUTPUT_VAL(gpio->group, gpio->bit, val);

	return 0;
}

int get_gpio_val(gpio_t *gpio)
{
	unsigned long ret = 0;

	//SET_GPIO_DIR_INPUT(gpio->group, gpio->bit);
	ret = GET_GPIO_VAL(gpio->group, gpio->bit);

	return ret;
}

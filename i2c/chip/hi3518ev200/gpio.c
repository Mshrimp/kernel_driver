#include "hi3518ev200.h"
#include "gpio.h"

#define	gpio_debug(fmt, args...)		\
			printk("gpio debug: "fmt"(func: %s, line: %d)\n", ##args, __func__, __LINE__);
#define	gpio_error(fmt, args...)		\
			printk("gpio error: "fmt"(func: %s, line: %d)\n", ##args, __func__, __LINE__);


static const u32 gpio_mux_reg[GPIO_GROUP_NUM][GPIO_BIT_NUM] = {
	/*Group\Bit	0	1	2	3	4	5	6	7 */
	/* 0 */	  {	28,	29,	30,	12,	00,	01, 02,	03 },
	/* 1 */	  {	31,	32,	33,	34,	35,	36,	37,	38 },
	/* 2 */	  {	04,	05,	06,	07,	 8,	 9,	10,	11 },
	/* 3 */	  { 13,	14,	15,	16,	17,	18,	19,	20 },
	/* 4 */	  { 21,	22,	23,	24,	25,	26,	27,	39 },
	/* 5 */	  { 40,	41,	42,	43,	44,	45,	46,	47 },
	/* 6 */	  { 48,	49,	50,	51,	52,	53,	54,	55 },
	/* 7 */	  { 56,	57,	58,	59,	60,	61,	62,	63 },
	/* 8 */	  { 64,	65 }
};

static const u32 gpio_mux_conf[GPIO_GROUP_NUM][GPIO_BIT_NUM] = {
	/*Group\Bit	0	1	2	3	4	5	6	7 */
	/* 0 */	  {	0,	0,	0,	0,	0,	1,	0,	0 },
	/* 1 */	  { 0,	0,	0,	0,	0,	0,	0,	0 },
	/* 2 */	  {	0,	0,	0,	0,	0,	0,	0,	0 },
	/* 3 */	  { 0,	0,	0,	0,	0,	0,	0,	0 },
	/* 4 */	  { 0,	0,	0,	0,	0,	0,	0,	0 },
	/* 5 */	  { 0,	0,	0,	0,	2,	0,	0,	0 },
	/* 6 */	  { 0,	0,	0,	0,	0,	0,	0,	0 },
	/* 7 */	  { 0,	0,	1,	1,	1,	1,	1,	1 },
	/* 8 */	  { 1,	1 }
};



int set_gpio_output(gpio_t *gpio)
{
	SET_GPIO_MUX_REG(gpio_mux_reg[gpio->group][gpio->bit], gpio_mux_conf[gpio->group][gpio->bit]);
	SET_GPIO_DIR_OUTPUT(gpio->group, gpio->bit);

	return 0;
}

int set_gpio_input(gpio_t *gpio)
{
	SET_GPIO_MUX_REG(gpio_mux_reg[gpio->group][gpio->bit], gpio_mux_conf[gpio->group][gpio->bit]);
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
	SET_GPIO_OUTPUT_VAL(gpio->group, gpio->bit, val);

	return 0;
}

int get_gpio_val(gpio_t *gpio)
{
	unsigned long ret = 0;
	SET_GPIO_MUX_REG(gpio_mux_reg[gpio->group][gpio->bit], gpio_mux_conf[gpio->group][gpio->bit]);
	ret = GET_GPIO_VAL(gpio->group, gpio->bit);

	return ret;
}

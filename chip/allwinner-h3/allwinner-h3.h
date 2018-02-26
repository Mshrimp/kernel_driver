#ifndef	__ALLWINNER_H3__
#define	__ALLWINNER_H3__

#include <linux/kernel.h>
#include <asm/io.h>
#include <mach/hardware.h>

#define	REG_READ(reg)				readl(IO_ADDRESS(reg))
#define	REG_WRITE(val, reg)			writel(val, IO_ADDRESS(reg))

#define	SET_REG_VAL(val, addr)		REG_WRITE(val, addr)
#define	GET_REG_VAL(addr)			REG_READ(addr)

#define	SET_REG_BIT(addr, bit)		\
	do {							\
		unsigned long int val;		\
		val = GET_REG_VAL(addr);	\
		val |= (0x1 << (bit));		\
		SET_REG_VAL(val, addr);		\
	} while (0)

#define	CLR_REG_BIT(addr, bit)		\
	do {							\
		unsigned long int val;		\
		val = GET_REG_VAL(addr);	\
		val &= ~(0x1 << (bit));		\
		SET_REG_VAL(val, addr);		\
	} while (0)

#define	SET_REG_BIT_VAL(addr, bit, val)		\
	do {							\
		unsigned long int value;		\
		value = GET_REG_VAL(addr);	\
		if (val) {				\
			value |= 0x1 << (bit);	\
		} else {					\
			value &= ~(0x1 << (bit));	\
		}							\
		SET_REG_VAL(value, addr);		\
	} while (0)

#define	GET_REG_BIT(addr, bit)		\
	((GET_REG_VAL(addr) & (0x1 << (bit))) >> (bit))

#define	SET_REG_BITS_VAL(addr, bit, num, data)	\
	do {										\
		unsigned long int val;					\
		val = GET_REG_VAL(addr);				\
		val &= (((0x1 << (num)) - 1) << (bit));	\
		val |= ((data) << (bit));				\
		SET_REG_VAL(val, addr);					\
	} while (0)

#define	GET_REG_BITS_VAL(addr, bit, num)	\
	(GET_REG_VAL(addr) & (((0x1 << (num)) - 1) << (bit)))

// GPIO Muxctrl

#define	GPIO_MUX_BASE_ADDR				0x200F0000
#define	GPIO_MUX_REG(nr)				(GPIO_MUX_BASE_ADDR + ((nr) << 2))

#define	SET_GPIO_MUX_REG(reg_nr, val)	SET_REG_VAL(val, GPIO_MUX_REG(reg_nr))


#define	GPIO_GROUP_NUM					7
#define	GPIO_BIT_NUM					32
#define	GPIOA_BIT_NUM					22
#define	GPIOC_BIT_NUM					19
#define	GPIOD_BIT_NUM					18
#define	GPIOE_BIT_NUM					16
#define	GPIOF_BIT_NUM					7
#define	GPIOG_BIT_NUM					14
#define	GPIOL_BIT_NUM					12

#define	GPIO_BASE_ADDR					0x01C20800
#define	GPIO_CTRL_BASE_ADDR(grp)		(GPIO_BASE_ADDR + (grp) * 0x24)
#define	GPIO_DATA_BASE_ADDR(grp)		(GPIO_BASE_ADDR + (grp) * 0x24 + 0x10)

#define	GPIO_CTRL_ADDR(group, nr)				\
		(GPIO_CTRL_BASE_ADDR(group) + ((nr) / 8) * 0x04)
		//(GPIO_CTRL_BASE_ADDR(group) + (((nr) < 8) ? 0x00 : (((nr) < 16) ? 0x04 : 0x08)))

#define	GPIO_DATA_ADDR(group)			GPIO_DATA_BASE_ADDR(group)

#define	GPIOA							0x0
#define	GPIOC							0x2
#define	GPIOD							0x3
#define	GPIOE							0x4
#define	GPIOF							0x5
#define	GPIOG							0x6
//#define	GPIOL							0x6

#define	GPIO_INPUT						0x0
#define	GPIO_OUTPUT						0x1
#define	GPIO_DISABLE					0x7

#define	SET_GPIO_DIR_OUTPUT(grp, bit)	\
			SET_REG_BITS_VAL(GPIO_CTRL_ADDR(grp, bit), ((bit) % 8) * 4, 3, GPIO_OUTPUT)
#define	SET_GPIO_DIR_INPUT(grp, bit)	\
			SET_REG_BITS_VAL(GPIO_CTRL_ADDR(grp, bit), ((bit) % 8) * 4, 3, GPIO_INPUT)

#define	SET_GPIO_OUTPUT_HIGH(grp, bit)		SET_REG_BIT(GPIO_DATA_ADDR(grp), bit)
#define	SET_GPIO_OUTPUT_LOW(grp, bit)		CLR_REG_BIT(GPIO_DATA_ADDR(grp), bit)
#define	SET_GPIO_OUTPUT_VAL(grp, bit, val)	SET_REG_BIT_VAL(GPIO_DATA_ADDR(grp), bit, val)

#define	GET_GPIO_VAL(grp, bit)				GET_REG_BIT(GPIO_DATA_ADDR(grp), bit)





#endif	/* __ALLWINNER_H3__ */

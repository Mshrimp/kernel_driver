#ifndef	__HI3518EV200_H__
#define	__HI3518EV200_H__

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

// Muxctrl

#define	GPIO_MUX_BASE_ADDR				0x200F0000
#define	GPIO_MUX_REG(nr)				(GPIO_MUX_BASE_ADDR + ((nr) << 2))

#define	SET_GPIO_MUX_REG(reg_nr, val)	SET_REG_VAL(val, GPIO_MUX_REG(reg_nr))

// GPIO

#define	GPIO_GROUP_NUM						9
#define	GPIO_BIT_NUM						8

#define	GPIO_NR_CHECK						\
			do {} while (0)

#define	GPIO_BASE_ADDR						0x20140000
#define	GPIO_GROUP_BASE_ADDR(group)			(GPIO_BASE_ADDR + ((group) << 16))

#define	GPIO_DATA_ADDR(grp, bit)			(GPIO_GROUP_BASE_ADDR(grp) + (0x1 << ((bit) + 2)))
#define	GPIO_DIR_ADDR(grp)					(GPIO_GROUP_BASE_ADDR(grp) + 0x400)
#define	GPIO_IS_ADDR(grp)					(GPIO_GROUP_BASE_ADDR(grp) + 0x404)
#define	GPIO_IBE_ADDR(grp)					(GPIO_GROUP_BASE_ADDR(grp) + 0x408)
#define	GPIO_IEV_ADDR(grp)					(GPIO_GROUP_BASE_ADDR(grp) + 0x40C)
#define	GPIO_IE_ADDR(grp)					(GPIO_GROUP_BASE_ADDR(grp) + 0x410)
#define	GPIO_RIS_ADDR(grp)					(GPIO_GROUP_BASE_ADDR(grp) + 0x414)
#define	GPIO_MIS_ADDR(grp)					(GPIO_GROUP_BASE_ADDR(grp) + 0x418)
#define	GPIO_IC_ADDR(grp)					(GPIO_GROUP_BASE_ADDR(grp) + 0x41C)

#define	SET_GPIO_DIR_OUTPUT(grp, bit)		SET_REG_BIT(GPIO_DIR_ADDR(grp), (bit))
#define	SET_GPIO_DIR_INPUT(grp, bit)		CLR_REG_BIT(GPIO_DIR_ADDR(grp), (bit))

#define	SET_GPIO_OUTPUT_HIGH(grp, bit)		SET_REG_BIT(GPIO_DATA_ADDR(grp, bit), (bit))
#define	SET_GPIO_OUTPUT_LOW(grp, bit)		CLR_REG_BIT(GPIO_DATA_ADDR(grp, bit), (bit))
#define	SET_GPIO_OUTPUT_VAL(grp, bit, val)	SET_REG_BIT_VAL(GPIO_DATA_ADDR(grp, bit), bit, val)

#define	GET_GPIO_VAL(grp, bit)			GET_REG_BIT(GPIO_DATA_ADDR(grp, bit), (bit))

// I2C

#define	I2C_TOTLE			3

#define	I2C0_BASE_ADDR		0x200D0000
#define	I2C1_BASE_ADDR		0x20240000
#define	I2C2_BASE_ADDR		0x20250000

#define	I2C_NR_CHECK												\
			do {													\
				if ((nr) >= I2C_TOTLE) {							\
					printk("I2C: channel error, nr = %d", nr);		\
					return -EINVAL;									\
				}													\
			} while (0)			

#define	I2C_BASE_ADDR(nr)		\
			((nr) == 0 ? I2C0_BASE_ADDR : ((nr) == 1 ? I2C1_BASE_ADDR : I2C2_BASE_ADDR))

#define	I2C_CON_ADDR(nr)				(I2C_BASE_ADDR(nr) + 0x00)
#define	I2C_TAR_ADDR(nr)				(I2C_BASE_ADDR(nr) + 0x04)
#define	I2C_DATA_CMD_ADDR(nr)			(I2C_BASE_ADDR(nr) + 0x10)
#define	I2C_SCL_HCNT_ADDR(nr)			(I2C_BASE_ADDR(nr) + 0x1C)
#define	I2C_SCL_LCNT_ADDR(nr)			(I2C_BASE_ADDR(nr) + 0x20)
#define	I2C_INTR_STAT_ADDR(nr)			(I2C_BASE_ADDR(nr) + 0x2C)
#define	I2C_INTR_MASK_ADDR(nr)			(I2C_BASE_ADDR(nr) + 0x30)
#define	I2C_INTR_RAW_ADDR(nr)			(I2C_BASE_ADDR(nr) + 0x34)
#define	I2C_RX_TL_ADDR(nr)				(I2C_BASE_ADDR(nr) + 0x38)
#define	I2C_TX_TL_ADDR(nr)				(I2C_BASE_ADDR(nr) + 0x3C)
#define	I2C_CLR_INTR_ADDR(nr)			(I2C_BASE_ADDR(nr) + 0x40)
#define	I2C_ENABLE_ADDR(nr)				(I2C_BASE_ADDR(nr) + 0x6C)
#define	I2C_STATUS_ADDR(nr)				(I2C_BASE_ADDR(nr) + 0x70)
#define	I2C_TXFLR_ADDR(nr)				(I2C_BASE_ADDR(nr) + 0x74)
#define	I2C_RXFLR_ADDR(nr)				(I2C_BASE_ADDR(nr) + 0x78)
#define	I2C_SDA_HOLD_ADDR(nr)			(I2C_BASE_ADDR(nr) + 0x7C)
#define	I2C_TX_ABRT_SRC_ADDR(nr)		(I2C_BASE_ADDR(nr) + 0x80)
#define	I2C_DMA_CR_ADDR(nr)				(I2C_BASE_ADDR(nr) + 0x88)
#define	I2C_DMA_TDLR_ADDR(nr)			(I2C_BASE_ADDR(nr) + 0x8C)
#define	I2C_DMA_RDLR_ADDR(nr)			(I2C_BASE_ADDR(nr) + 0x90)
#define	I2C_SCL_SWITCH_ADDR(nr)			(I2C_BASE_ADDR(nr) + 0xA0)
#define	I2C_SCL_SIM_ADDR(nr)			(I2C_BASE_ADDR(nr) + 0xA4)
#define	I2C_LOCK_ADDR(nr)				(I2C_BASE_ADDR(nr) + 0xAC)
#define	I2C_MST_SINGLE_CTRL_ADDR(nr)	(I2C_BASE_ADDR(nr) + 0xB0)
#define I2C_MST_SINGLE_CMD_ADDR(nr)		(I2C_BASE_ADDR(nr) + 0xB4)
#define	I2C_SEQUENCE_CMD0_ADDR(nr)		(I2C_BASE_ADDR(nr) + 0xB8)
#define	I2C_SEQUENCE_CMD1_ADDR(nr)		(I2C_BASE_ADDR(nr) + 0xBC)
#define	I2C_SEQUENCE_CMD2_ADDR(nr)		(I2C_BASE_ADDR(nr) + 0xC0)


#define	I2C_UNLOCK_MASK					0x1ACCE551

#define	I2C_MASTER_RESTART_ENABLE_BIT	(5)
#define	I2C_MASTER_ADDR_10BIT_BIT		(12)
#define	I2C_ENABLE_BIT					(0)

#define	I2C_SLAVE_ADDR_10BIT_MASK		((0x01 << 10) - 1)
#define	I2C_SLAVE_ADDR_7BIT_MASK		((0x01 << 7) - 1)
#define	I2C_DATA_MASK					((0x01 << 8) - 1)

#define	I2C_APB_CLK						50000000	// 50MHz
#define	I2C_NORMAL_CLK					100000		// 100KHz
#define	I2C_FAST_CLK					400000		// 400KHz

#define	I2C_NORMAL_SCL_HCNT				(I2C_APB_CLK / I2C_NORMAL_CLK * 5 /10)
#define	I2C_NORMAL_SCL_LCNT				(I2C_APB_CLK / I2C_NORMAL_CLK * 5 /10)
#define	I2C_FAST_SCL_HCNT				(I2C_APB_CLK / I2C_FAST_CLK * 36 /100)
#define	I2C_FAST_SCL_LCNT				(I2C_APB_CLK / I2C_FAST_CLK * 60 /100)

#define	I2C_SCL_HCNT_BITS				(16)
#define	I2C_SCL_LCNT_BITS				(16)

#define	I2C_SDA_HOLD					(I2C_NORMAL_SCL_LCNT / 2)
#define	I2C_SDA_HOLD_BITS				(16)

#define	I2C_MST_SINGLE_MODEL_ENABLE_BIT	(31)		


// Register function
#define	I2C_UNLOCK(nr)					SET_REG_VAL(I2C_UNLOCK_MASK, I2C_LOCK_ADDR(nr))

#define	I2C_ENABLE(nr)					SET_REG_BIT(I2C_ENABLE_ADDR(nr), I2C_ENABLE_BIT)
#define	I2C_DISABLE(nr)					CLR_REG_BIT(I2C_ENABLE_ADDR(nr), I2C_ENABLE_BIT)

#define	I2C_NORMAL_SCL_HCNT_SET(nr)		\
	SET_REG_BITS_VAL(I2C_SCL_HCNT_ADDR(nr), 0, I2C_SCL_HCNT_BITS, I2C_NORMAL_SCL_HCNT)

#define	I2C_NORMAL_SCL_LCNT_SET(nr)		\
	SET_REG_BITS_VAL(I2C_SCL_LCNT_ADDR(nr), 0, I2C_SCL_LCNT_BITS, I2C_NORMAL_SCL_LCNT)

#define	I2C_SDA_HOLD_SET(nr)			\
	SET_REG_BITS_VAL(I2C_SDA_HOLD_ADDR(nr), 0, I2C_SDA_HOLD_BITS, I2C_SDA_HOLD)

#define	I2C_MASTER_RESTART_ENABLE(nr)	SET_REG_BIT(I2C_CON_ADDR(nr), I2C_MASTER_RESTART_ENABLE_BIT)
#define	I2C_MASTER_RESTART_DISABLE(nr)	CLR_REG_BIT(I2C_CON_ADDR(nr), I2C_MASTER_RESTART_ENABLE_BIT)

#define	I2C_INTR_SET_MASK(nr)			SET_REG_VAL(0xFFFFFFFF, I2C_INTR_MASK_ADDR(nr))
#define	I2C_INTR_SET_UNMASK(nr)			SET_REG_VAL(0x0, I2C_INTR_MASK_ADDR(nr))

#define	I2C_RX_TL_SET(nr)				SET_REG_VAL(0x08, I2C_RX_TL_ADDR(nr))
#define	I2C_TX_TL_SET(nr)				SET_REG_VAL(0x08, I2C_TX_TL_ADDR(nr))

#define	I2C_SLAVER_ADDR_7BITS_SET(nr)	CLR_REG_BIT(I2C_TAR_ADDR(nr), I2C_MASTER_ADDR_10BIT_BIT)
#define	I2C_SLAVER_ADDR_10BITS_SET(nr)	SET_REG_BIT(I2C_TAR_ADDR(nr), I2C_MASTER_ADDR_10BIT_BIT)

#define	I2C_SLAVER_7BITS_ADDR_SET(nr, addr)		\
	SET_REG_BITS_VAL(I2C_TAR_ADDR(nr), 0, 7, addr)
#define	I2C_SLAVER_10BITS_ADDR_SET(nr, addr)		\
	SET_REG_BITS_VAL(I2C_TAR_ADDR(nr), 0, 10, addr)

#define	I2C_MST_SINGLE_MODEL_ENABLE(nr)		\
	SET_REG_BIT(I2C_MST_SINGLE_CTRL_ADDR(nr), I2C_MST_SINGLE_MODEL_ENABLE_BIT)
#define	I2C_MST_SINGLE_MODEL_DISABLE(nr)	\
	CLR_REG_BIT(I2C_MST_SINGLE_CTRL_ADDR(nr), I2C_MST_SINGLE_MODEL_ENABLE_BIT)


#endif	/* __HI3518EV200_H__ */

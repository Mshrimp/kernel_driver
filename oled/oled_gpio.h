#ifndef	__OLED_H__
#define	__OLED_H__

#include <asm/ioctl.h>



typedef enum {
	OLED_INIT = 0x0,
	OLED_CLEAR = 0x1,
	OLED_FULL = 0x2,
	OLED_TEST = 0x3,
} oled_cmd_e;


#define		OLED_IOC_MAGIC		'o'
#define		OLED_IOC_MAXNR		3
#define		OLED_IOC_INIT			_IO(OLED_IOC_MAGIC, OLED_INIT)
#define		OLED_IOC_CLEAR			_IO(OLED_IOC_MAGIC, OLED_CLEAR)
#define		OLED_IOC_FULL			_IO(OLED_IOC_MAGIC, OLED_FULL)
#define		OLED_IOC_TEST			_IO(OLED_IOC_MAGIC, OLED_TEST)

int oled_init(void);

int oled_uninit(void);

int oled_operation(unsigned int cmd, unsigned long args);


#endif	/* __OLED_H__ */

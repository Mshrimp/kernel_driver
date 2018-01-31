#ifndef	__LED_H__
#define	__LED_H__

#include <asm/ioctl.h>


typedef enum {
	LED_ON = 0x0,
	LED_OFF = 0x1,
	LED_GET_STATUS = 0x2,
} led_cmd_e;

#define	LED_IOC_MAGIC			'l'
#define	LED_IOC_MAXNR			2
#define	LED_IOC_ON				_IO(LED_IOC_MAGIC, LED_ON)
#define	LED_IOC_OFF				_IO(LED_IOC_MAGIC, LED_OFF)
#define	LED_IOC_GET_STATUS		_IO(LED_IOC_MAGIC, LED_GET_STATUS)


int led_init (void);

int led_uninit (void);

int led_operation(unsigned int cmd, unsigned long args);

#endif

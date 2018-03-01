#ifndef	__OLED_H__
#define	__OLED_H__

#include <asm/ioctl.h>



typedef enum {
	OLED_INIT = 0x0,
	OLED_CLEAR = 0x1,
	OLED_FULL = 0x2,
	OLED_CHAR = 0x3,
	OLED_STR = 0x4,
	OLED_SCROLL_H = 0x5,
	OLED_SCROLL_H_V = 0x6,
	OLED_STOP_SCROLL = 0x7,
	OLED_TEST = 0x8,
} oled_cmd_e;


#define		OLED_IOC_MAGIC		'o'
#define		OLED_IOC_MAXNR		9
#define		OLED_IOC_INIT			_IO(OLED_IOC_MAGIC, OLED_INIT)
#define		OLED_IOC_CLEAR			_IO(OLED_IOC_MAGIC, OLED_CLEAR)
#define		OLED_IOC_FULL			_IO(OLED_IOC_MAGIC, OLED_FULL)
#define		OLED_IOC_CHAR			_IO(OLED_IOC_MAGIC, OLED_CHAR)
#define		OLED_IOC_STR			_IO(OLED_IOC_MAGIC, OLED_STR)
#define		OLED_IOC_SCROLL_H		_IO(OLED_IOC_MAGIC, OLED_SCROLL_H)
#define		OLED_IOC_SCROLL_H_V		_IO(OLED_IOC_MAGIC, OLED_SCROLL_H_V)
#define		OLED_IOC_STOP_SCROLL	_IO(OLED_IOC_MAGIC, OLED_STOP_SCROLL)
#define		OLED_IOC_TEST			_IO(OLED_IOC_MAGIC, OLED_TEST)

#define	OLED_8_16_MAX_CNT		64

#define	OLED_8_16_BUF_SIZE		(OLED_8_16_MAX_CNT + 1)


typedef struct {
	unsigned char row;
	unsigned char col;
	unsigned char ch;
} oled_char_t;

typedef struct {
	unsigned char row;
	unsigned char col;
	unsigned char str[OLED_8_16_BUF_SIZE];
} oled_str_t;

typedef struct {
	unsigned char direction;
	unsigned char speed;
	unsigned char col_step;
	unsigned char start_page;
	unsigned char stop_page;
} oled_scroll_t;

int oled_init(void);

int oled_uninit(void);

int oled_operation(unsigned int cmd, unsigned long args);


#endif	/* __OLED_H__ */

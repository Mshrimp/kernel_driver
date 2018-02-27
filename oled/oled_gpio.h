#ifndef	__OLED_H__
#define	__OLED_H__







int oled_init(void);

int oled_uninit(void);

int oled_operation(unsigned int cmd, unsigned long args);


#endif	/* __OLED_H__ */

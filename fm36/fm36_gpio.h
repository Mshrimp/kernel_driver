#ifndef	__FM36_H__
#define	__FM36_H__

#include <asm/ioctl.h>


typedef struct {
	unsigned char *data;
	unsigned long len;
} fm36_data_t;

typedef struct {
	unsigned short reg;
	unsigned short data;
} fm36_write_reg_t;

typedef struct {
	unsigned short reg;
	unsigned short data;
} fm36_read_reg_t;


typedef enum {
	FM36_CHIP_RESET = 0x0,
	FM36_INIT_REGISTER = 0x1,
	FM36_READ_DATA = 0x2,
	FM36_WRITE_DATA = 0x3,
} fm36_cmd_e;

#define	FM36_IOC_MAGIC			'm'
#define	FM36_IOC_MAXNR			3
#define	FM36_IOC_CHIP_RESET		_IO(FM36_IOC_MAGIC, FM36_CHIP_RESET)
#define	FM36_IOC_INIT_REGISTER	_IO(FM36_IOC_MAGIC, FM36_INIT_REGISTER)
#define	FM36_IOC_READ_DATA		_IO(FM36_IOC_MAGIC, FM36_READ_DATA)
#define	FM36_IOC_WRITE_DATA		_IO(FM36_IOC_MAGIC, FM36_WRITE_DATA)


int fm36_init (void);

int fm36_uninit (void);

int fm36_gpio_test(void);

int fm36_operation(unsigned int cmd, unsigned long args);

#endif

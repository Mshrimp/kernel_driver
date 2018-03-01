#ifndef	__FONT_H__
#define	__FONT_H__


unsigned char font[][6] = {
	{0x00,0x7C,0x10,0x10,0x7C,0x00},/*"H",0*/
	{0x00,0x70,0x68,0x68,0x50,0x00},/*"e",1*/
	{0x00,0x00,0x7E,0x40,0x00,0x00},/*"l",2*/
	{0x00,0x00,0x7E,0x40,0x00,0x00},/*"l",3*/
	{0x00,0x70,0x48,0x48,0x70,0x00},/*"o",4*/
	{0x00,0x00,0x00,0x00,0x00,0x00},/*" ",5*/
	{0x08,0x38,0x28,0x38,0x38,0x08},/*"w",6*/
	{0x00,0x70,0x48,0x48,0x70,0x00},/*"o",7*/
	{0x08,0x48,0x70,0x08,0x08,0x00},/*"r",8*/
	{0x00,0x00,0x7E,0x40,0x00,0x00},/*"l",9*/
	{0x00,0x70,0x48,0x48,0x7E,0x40},/*"d",10*/
	{0x00,0x00,0x4C,0x40,0x00,0x00},/*"!",11*/
};

unsigned char font_8_16[][16] = {
	{0x08,0xF8,0x08,0x00,0x00,0x08,0xF8,0x08,0x20,0x3F,0x21,0x01,0x01,0x21,0x3F,0x20},/*"H",0*/
	{0x00,0x00,0x80,0x80,0x80,0x80,0x00,0x00,0x00,0x1F,0x24,0x24,0x24,0x24,0x17,0x00},/*"e",1*/
	{0x00,0x10,0x10,0xF8,0x00,0x00,0x00,0x00,0x00,0x20,0x20,0x3F,0x20,0x20,0x00,0x00},/*"l",2*/
	{0x00,0x10,0x10,0xF8,0x00,0x00,0x00,0x00,0x00,0x20,0x20,0x3F,0x20,0x20,0x00,0x00},/*"l",3*/
	{0x00,0x00,0x80,0x80,0x80,0x80,0x00,0x00,0x00,0x1F,0x20,0x20,0x20,0x20,0x1F,0x00},/*"o",4*/
	{0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00},/*" ",5*/
	{0x80,0x80,0x00,0x80,0x80,0x00,0x80,0x80,0x01,0x0E,0x30,0x0C,0x07,0x38,0x06,0x01},/*"w",6*/
	{0x00,0x00,0x80,0x80,0x80,0x80,0x00,0x00,0x00,0x1F,0x20,0x20,0x20,0x20,0x1F,0x00},/*"o",7*/
	{0x80,0x80,0x80,0x00,0x80,0x80,0x80,0x00,0x20,0x20,0x3F,0x21,0x20,0x00,0x01,0x00},/*"r",8*/
	{0x00,0x10,0x10,0xF8,0x00,0x00,0x00,0x00,0x00,0x20,0x20,0x3F,0x20,0x20,0x00,0x00},/*"l",9*/
	{0x00,0x00,0x80,0x80,0x80,0x90,0xF0,0x00,0x00,0x1F,0x20,0x20,0x20,0x10,0x3F,0x20},/*"d",10*/
	{0x00,0x00,0x00,0xF8,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x33,0x00,0x00,0x00,0x00},/*"!",11*/
};

#endif	/* __FONT_H__ */

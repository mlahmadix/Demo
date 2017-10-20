#ifndef __IO_MAPPING_H__
#define __IO_MAPPING_H__

enum {
	enum_Input = 0,
	enum_Output
}EnumInOut;

enum {
	CeInout_LS = 0,
	CeInout_HS
}ceInoutSideness;

typedef struct{
	char   Pin_Type;
	char   Pin_HSLS;
	char   Pin_RegBeg;
	char   Pin_RegLen;
	bool * InOutValue;
}GPIO_MAPPING;

/* Address for this global variables will be respectively
   used as Registers INPUT1 and OUTPUT1 addresses
*/
static unsigned long ulGPIO_INPUT1;
static unsigned long ulGPIO_OUTPUT1;

#endif

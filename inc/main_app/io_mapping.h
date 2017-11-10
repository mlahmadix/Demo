#ifndef __IO_MAPPING_H__
#define __IO_MAPPING_H__

enum {
	enum_Input = 0,
	enum_Output
};

enum {
	CeInout_LS = 0,
	CeInout_HS
};

typedef struct{
	char   Pin_Type;
	char   Pin_HSLS;
	char   Pin_RegBeg;
	char   Pin_RegLen;
	bool * InOutValue;
}GPIO_MAPPING;

#endif

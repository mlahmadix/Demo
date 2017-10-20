#ifndef __LIB_IO_H__
#define __LIB_IO_H__

#include "inout/lib_bits.h"

class gpioHandler
{
	public:
		gpioHandler(unsigned long ulInpOutReg);
		~gpioHandler();
		void SetOutput(unsigned long ulOutputBeg, unsigned long ulOutputLen,  bool bHighLowSide);
		void ResetOutput(unsigned long ulOutputBeg, unsigned long ulOutputLen,  bool bHighLowSide);
		bool GetInputOutputStatus(unsigned long ulInOutBeg, unsigned long ulOutputLen,  bool bHighLowSide);

	private:
		RegBits * mInOutReg;
};


#endif

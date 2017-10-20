#include "inout/lib_io.h"
#include "logger/lib_logger.h"		

#define TAG "LIBIO"

using namespace std; 		

gpioHandler::gpioHandler(unsigned long ulInpOutReg)
{
	ALOGD(TAG, __FUNCTION__, "CTOR");
	mInOutReg = new RegBits(ulInpOutReg);
}

gpioHandler::~gpioHandler()
{
	ALOGD(TAG, __FUNCTION__, "DTOR");
	if( mInOutReg ) {
		delete mInOutReg;
	}
}

void gpioHandler::SetOutput(unsigned long ulOutputBeg, unsigned long ulOutputLen,  bool bHighLowSide)
{
	if(bHighLowSide)
		mInOutReg->SetBitFieldValue (ulOutputBeg, ulOutputLen, static_cast<unsigned long>(1));
	else
		mInOutReg->ResetBitFieldValue (ulOutputBeg, ulOutputLen);
}

void gpioHandler::ResetOutput(unsigned long ulOutputBeg, unsigned long ulOutputLen,  bool bHighLowSide)
{
	if(bHighLowSide)
		mInOutReg->ResetBitFieldValue (ulOutputBeg, ulOutputLen);
	else
		mInOutReg->SetBitFieldValue (ulOutputBeg, ulOutputLen, static_cast<unsigned long>(1));
}
	
bool gpioHandler::GetInputOutputStatus(unsigned long ulInOutBeg, unsigned long ulInOutLen,  bool bHighLowSide)
{
	unsigned long ulVal = mInOutReg->getBitFieldValue(ulInOutBeg, ulInOutLen);
	ALOGD(TAG, __FUNCTION__, "Returned InOut Value= 0x%08X", ulVal);
	if (ulVal) {
		if(bHighLowSide)
			return true;
		else
			return false;
	} else {
		if(bHighLowSide)
			return false;
		else
			return true;
	}	
}

#include "inout/InOutApp.h"
#include "logger/lib_logger.h"		

#define TAG "INOUT_APP"

using namespace std;

InOutApp::InOutApp()
{
	ALOGD(TAG, __FUNCTION__, "CTOR");
	
	mRegOutput = new gpioHandler((unsigned long)&ulGPIO_OUTPUT1);
	mRegInput  = new gpioHandler((unsigned long)&ulGPIO_INPUT1);
}

InOutApp::~InOutApp()
{
	ALOGD(TAG, __FUNCTION__, "DTOR");
	delete mRegInput;
	delete mRegOutput;
}

void InOutApp::UpdateInputs()
{
	for(int i = 0; i < CeEnum_TotInputs; i++) {
		*(GPIO_MAP_INPUTS1[i].InOutValue) = mRegInput->GetInputOutputStatus(GPIO_MAP_INPUTS1[i].Pin_RegBeg,
																		    GPIO_MAP_INPUTS1[i].Pin_RegLen,
																		    GPIO_MAP_INPUTS1[i].Pin_HSLS);
	}
}

void InOutApp::UpdateOutputs()
{
	for(int i = 0; i < CeEnum_TotInputs; i++) {
		*(GPIO_MAP_OUTPUTS1[i].InOutValue) = mRegOutput->GetInputOutputStatus(GPIO_MAP_OUTPUTS1[i].Pin_RegBeg,
																		      GPIO_MAP_OUTPUTS1[i].Pin_RegLen,
																		      GPIO_MAP_OUTPUTS1[i].Pin_HSLS);
	}
}

bool InOutApp::CheckInput(int enumOutputIdx)
{
	return *(GPIO_MAP_OUTPUTS1[enumOutputIdx].InOutValue);
}
bool InOutApp::CheckOutput(int enumOutputIdx)
{
	return *(GPIO_MAP_OUTPUTS1[enumOutputIdx].InOutValue);
}

void InOutApp::SetOutputOn(int enumOutputIdx)
{
		mRegOutput->SetOutput (GPIO_MAP_OUTPUTS1[enumOutputIdx].Pin_RegBeg,
							   GPIO_MAP_OUTPUTS1[enumOutputIdx].Pin_RegLen,
							   GPIO_MAP_OUTPUTS1[enumOutputIdx].Pin_HSLS);
							   
		*(GPIO_MAP_OUTPUTS1[enumOutputIdx].InOutValue) = mRegOutput->GetInputOutputStatus(GPIO_MAP_OUTPUTS1[enumOutputIdx].Pin_RegBeg,
																		                  GPIO_MAP_OUTPUTS1[enumOutputIdx].Pin_RegLen,
																		                  GPIO_MAP_OUTPUTS1[enumOutputIdx].Pin_HSLS);
}
void InOutApp::SetOutputOff(int enumOutputIdx)
{
		mRegOutput->ResetOutput (GPIO_MAP_OUTPUTS1[enumOutputIdx].Pin_RegBeg,
							     GPIO_MAP_OUTPUTS1[enumOutputIdx].Pin_RegLen,
							     GPIO_MAP_OUTPUTS1[enumOutputIdx].Pin_HSLS);
							   
		*(GPIO_MAP_OUTPUTS1[enumOutputIdx].InOutValue) = mRegOutput->GetInputOutputStatus(GPIO_MAP_OUTPUTS1[enumOutputIdx].Pin_RegBeg,
																		                  GPIO_MAP_OUTPUTS1[enumOutputIdx].Pin_RegLen,
																		                  GPIO_MAP_OUTPUTS1[enumOutputIdx].Pin_HSLS);
}

void InOutApp::FlashOutput(int enumOutputIdx, unsigned long ulInterval, unsigned long ulDuration)
{
	
}


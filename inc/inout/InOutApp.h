#ifndef __INOUTAPP_H__
#define __INOUTAPP_H__

#include "inout/lib_io.h"
#include "main_app/io_mapping.h"

class InOutApp
{	
	public:
		InOutApp();
		~InOutApp();
		void UpdateInputs();
		void UpdateOutputs();
		bool CheckInput(int enumOutputIdx);
		bool CheckOutput(int enumOutputIdx);
		void SetOutputOn(int enumOutputIdx);
		void SetOutputOff(int enumOutputIdx);
		void FlashOutput(int enumOutputIdx, unsigned long ulInterval, unsigned long ulDuration);
		enum {
			CeEnum_ParkIn,
			CeEnum_IgnitionIn,
			CeEnum_LeftSignIn,
			CeEnum_RightSignIn,
			CeEnum_TotInputs
		}EnumInputs;

		enum {
			CeEnum_ParkOut,
			CeEnum_IgnitionOut,
			CeEnum_LeftSignOut,
			CeEnum_RightSignOut,
			CeEnum_TotOutputs
		}EnumOutputs;

	private:
		gpioHandler * mRegOutput;
		gpioHandler * mRegInput;
		bool mParkPos;
		bool mIgnition;
		bool mLeftSign;
		bool mRightSign;
		bool mParkPosOut;
		bool mIgnitionOut;
		bool mLeftSignOut;
		bool mRightSignOut;
		/* Address for this global variables will be respectively
			used as Registers INPUT1 and OUTPUT1 addresses
		*/
		unsigned long ulGPIO_INPUT1;
		unsigned long ulGPIO_OUTPUT1;
		GPIO_MAPPING GPIO_MAP_INPUTS1 [CeEnum_TotInputs] = {
			{enum_Input, CeInout_LS, 0, 2, &mParkPos}   , //CeEnum_ParkIn
			{enum_Input, CeInout_LS, 2, 2, &mIgnition}  , //CeEnum_IgnitionIn
			{enum_Input, CeInout_HS, 4, 2, &mLeftSign}  , //CeEnum_LeftSignIn
			{enum_Input, CeInout_HS, 6, 2, &mRightSign} , //CeEnum_RightSignIn
		};

		GPIO_MAPPING GPIO_MAP_OUTPUTS1 [CeEnum_TotOutputs] = {
			{enum_Output, CeInout_HS, 0, 2, &mParkPosOut}   , //CeEnum_ParkOut
			{enum_Output, CeInout_HS, 2, 2, &mIgnitionOut}  , //CeEnum_IgnitionOut
			{enum_Output, CeInout_LS, 4, 2, &mLeftSignOut}  , //CeEnum_LeftSignOut
			{enum_Output, CeInout_LS, 6, 2, &mRightSignOut} , //CeEnum_RightSignOut
		};
};

#endif

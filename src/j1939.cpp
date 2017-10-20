#include <iostream>
#include "main_app/j1939.h"
#include "logger/lib_logger.h"

#define TAG "J1939Layer"

using namespace std;


J1939Layer::J1939Layer(std::string CanFifoName)
{
	ALOGD(TAG, __FUNCTION__, "CTOR");
	CanInfDrv = new CANDrv(CanFifoName);
}


J1939Layer::~J1939Layer()
{
	ALOGD(TAG, __FUNCTION__, "DTOR");
	if(CanInfDrv){
		CanInfDrv->StopCANDriver();
		delete CanInfDrv;
	}
}

bool J1939Layer::SendJ1939Msg(struct can_frame &TxCanMsg)
{
	if(CanInfDrv->getCANStatus() == true)
		return CanInfDrv->CanSendMsg(TxCanMsg);
	else
		return false;
}

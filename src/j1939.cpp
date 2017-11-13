#include <iostream>
#include "main_app/j1939.h"
#include "logger/lib_logger.h"

#define TAG "J1939Layer"

using namespace std;


J1939Layer::J1939Layer(string CanFifoName, string CanInfName):
CANDrv(CanFifoName, CanInfName, static_cast<unsigned long>(J1939_BaudRate))
{
	ALOGD(TAG, __FUNCTION__, "CTOR");
}


J1939Layer::~J1939Layer()
{
	ALOGD(TAG, __FUNCTION__, "DTOR");
}

bool J1939Layer::SendJ1939Msg(struct can_frame &TxCanMsg)
{
	if(getCANStatus() == true)
		return CanSendMsg(TxCanMsg);
	else
		return false;
}

void J1939Layer::ForceStopCAN()
{
	StopCANDriver();
}

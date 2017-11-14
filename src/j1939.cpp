#include <iostream>
#include "main_app/j1939.h"
#include "logger/lib_logger.h"

#define TAG "J1939Layer"

using namespace std;


J1939Layer::J1939Layer(string CanFifoName, string CanInfName, const struct J1939_eRxDataInfo * J1939_RxDataParams, unsigned int size, struct can_filter * J1939Filters):
CANDrv(CanFifoName, CanInfName, static_cast<unsigned long>(J1939_BaudRate), J1939Filters),
mEffectiveRxMsgNum(0)
{
	ALOGD(TAG, __FUNCTION__, "CTOR");
	if(getCANStatus()) {
		ALOGD(TAG, __FUNCTION__, "J1939 Initialized Successfully");
		InstallJ1939_RXParsers(J1939_RxDataParams, size);
		if(setCanFilters(J1939Filters, mEffectiveRxMsgNum)) {
			ALOGD(TAG, __FUNCTION__, "J1939 CAN Filters Set Successfully");
		}
		pthread_create(&J1939Thread, NULL, pvthJ1939ParseFrames_Exe, this);
	}
}


J1939Layer::~J1939Layer()
{
	ALOGD(TAG, __FUNCTION__, "DTOR");
	//join Parsing Thread
	pthread_join(J1939Thread, NULL);
	RemoveJ1939_RXParsers();
}

unsigned long J1939Layer::ulBuildExtCanId(unsigned char ucSA, unsigned short usPGN)
{
	unsigned char ucDefPrio = 6;
	return static_cast<unsigned long>(static_cast<unsigned long>(ucDefPrio << 24) +
									  static_cast<unsigned long>(usPGN << 8) +
									  static_cast<unsigned long>(ucSA));
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


void J1939Layer::InstallJ1939_RXParsers(const J1939_eRxDataInfo * J1939_RxDataParams, unsigned int size)
{
	mEffectiveRxMsgNum = size;
	for(unsigned int i = 0; i < size; i++) {
		mJ1939_RxDataDef[i] = J1939_RxDataParams[i];
	}
}

unsigned short J1939Layer::usGetPGNfromArbID(unsigned long ulArbID)
{
	return static_cast<unsigned short>((ulArbID >> 8) & 0xFFFF);
}

unsigned char J1939Layer::ucGetSAfromArbID(unsigned long ulArbID)
{
	return static_cast<unsigned char>(ulArbID & 0xFF);
}



void J1939Layer::j1939_updateDataParameters(struct can_frame NewRecvMsg)
{
	signed long Tempvalue = 0;
	for(unsigned int i = 0; i < mEffectiveRxMsgNum; i++) {
		if(ucGetSAfromArbID(NewRecvMsg.can_id) == mJ1939_RxDataDef[i].ucSA) {
			if(usGetPGNfromArbID(NewRecvMsg.can_id) == mJ1939_RxDataDef[i].usPGN) {
				memcpy((void*)&Tempvalue,(void*)&NewRecvMsg.data[mJ1939_RxDataDef[i].ucStartByte],mJ1939_RxDataDef[i].ucNumBytes);
				Tempvalue *= mJ1939_RxDataDef[i].ulMulA;
				Tempvalue /= mJ1939_RxDataDef[i].ulDivB;
				Tempvalue += mJ1939_RxDataDef[i].slOffsetC;
				if(mJ1939_RxDataDef[i].slMaxRange <= 0xFF) {
					if(mJ1939_RxDataDef[i].slMinRange < 0){
						Tempvalue = static_cast<signed char>(Tempvalue);
					}else {
						Tempvalue = static_cast<unsigned char>(Tempvalue);
					}
				} else if(mJ1939_RxDataDef[i].slMaxRange <= 0xFFFF) {
					if(mJ1939_RxDataDef[i].slMinRange < 0){
						Tempvalue = static_cast<signed short>(Tempvalue);
					}else {
						Tempvalue = static_cast<unsigned short>(Tempvalue);
					}
				} else {
					if(mJ1939_RxDataDef[i].slMinRange < 0){
						Tempvalue = static_cast<signed long>(Tempvalue);
					}else {
						Tempvalue = static_cast<unsigned long>(Tempvalue);
					}
				}
				
				if(Tempvalue < mJ1939_RxDataDef[i].slMinRange){
					Tempvalue = mJ1939_RxDataDef[i].slMinRange;
				}
				
				if(Tempvalue > mJ1939_RxDataDef[i].slMaxRange){
					Tempvalue = mJ1939_RxDataDef[i].slMaxRange;
				}
				memcpy(mJ1939_RxDataDef[i].pucData,(void*)&Tempvalue, mJ1939_RxDataDef[i].ucNumBytes);
			}
		}
	}
}


void * J1939Layer::pvthJ1939ParseFrames_Exe (void* context)
{
	J1939Layer * J1939LayerInst = static_cast<J1939Layer *>(context);
	struct timespec j1939ParserTimer;
	j1939ParserTimer.tv_sec = 0;
	j1939ParserTimer.tv_nsec = 5000000;
	struct can_frame NewRecvMsg;
	unsigned int uiSize = 0;
	while(J1939LayerInst->getCANStatus()) {
		if(J1939LayerInst->CANFifo->DequeueMessage((void *)&NewRecvMsg, &uiSize)){
			J1939LayerInst->j1939_updateDataParameters(NewRecvMsg);
		}
		nanosleep(&j1939ParserTimer, NULL);
	}
	pthread_exit(NULL);
}


void J1939Layer::RemoveJ1939_RXParsers()
{
	for(unsigned int i = 0; i < mEffectiveRxMsgNum; i++) {
		mJ1939_RxDataDef[i] = {0};
	}
	mEffectiveRxMsgNum = 0;
}

#include <iostream>
#include <chrono> //include timestamp operations
#include "main_app/j1939.h"
#include "logger/lib_logger.h"

#define TAG "J1939Layer"

using namespace std;
using namespace std::chrono;

J1939Layer::J1939Layer(std::string CanFifoName, std::string CanInfName, const J1939_eRxDataInfo * J1939_RxDataParams, int size):
CANDrv(CanFifoName, CanInfName, static_cast<unsigned long>(J1939_BaudRate), NULL),
mEffectiveRxMsgNum(cstJ1939_Num_MsgRX)//Take Default Max value
{
	ALOGD(TAG, __FUNCTION__, "CTOR");
	if(getCANStatus()) {
		ALOGD(TAG, __FUNCTION__, "J1939 Initialized Successfully");
		InstallJ1939_RXParsers(J1939_RxDataParams, size);
		pthread_create(&J1939Thread, NULL, pvthJ1939ParseFrames_Exe, this);
	}
}

J1939Layer::J1939Layer(string CanFifoName, string CanInfName, const J1939_eRxDataInfo * J1939_RxDataParams, int size,
struct can_filter * J1939Filters):
CANDrv(CanFifoName, CanInfName, static_cast<unsigned long>(J1939_BaudRate), J1939Filters),
mEffectiveRxMsgNum(cstJ1939_Num_MsgRX)//Take Default Max value
{
	ALOGD(TAG, __FUNCTION__, "CTOR");
	if(getCANStatus()) {
		ALOGD(TAG, __FUNCTION__, "J1939 Initialized Successfully");
		InstallJ1939_RXParsers(J1939_RxDataParams, size);
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


void J1939Layer::InstallJ1939_RXParsers(const J1939_eRxDataInfo * J1939_RxDataParams, int size)
{
	if(J1939_RxDataParams != NULL) {
		if(size <= cstJ1939_Num_MsgRX)
			mEffectiveRxMsgNum = size;
		else
			mEffectiveRxMsgNum = cstJ1939_Num_MsgRX;

		ALOGD(TAG, __FUNCTION__, "Nber of RX Messages1= %d", size);
		ALOGD(TAG, __FUNCTION__, "Nber of RX Messages2= %d", mEffectiveRxMsgNum);
		
		mJ1939_RxDataDef = new J1939_eRxDataInfo[mEffectiveRxMsgNum];
		mJ1939_RxMsgTimeout = new J1939_eRXMsgTimeout[mEffectiveRxMsgNum];

		for(unsigned int i = 0; i < mEffectiveRxMsgNum; i++) {
			mJ1939_RxDataDef[i] = J1939_RxDataParams[i];
		}
		
		for(unsigned int j = 0; j < mEffectiveRxMsgNum; j++) {
			mJ1939_RxMsgTimeout[j].usPGN = J1939_RxDataParams[j].usPGN;
			mJ1939_RxMsgTimeout[j].ucSA = J1939_RxDataParams[j].ucSA;
			mJ1939_RxMsgTimeout[j].ulTimeout = 0;
			mJ1939_RxMsgTimeout[j].ulPrevTimeout = 0;
			mJ1939_RxMsgTimeout[j].ulMaxTimeout = J1939_RxDataParams[j].Timeout;
		}
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

unsigned long long J1939Layer::getCurrentMsTimestamp()
{
	return duration_cast<std::chrono::milliseconds>(system_clock::now().time_since_epoch()).count();
}

void J1939Layer::j1939_updateDataValidity(struct CanMsgTstamp NewRecvMsg)
{
	for(unsigned int i = 0; i < mEffectiveRxMsgNum; i++) {
		if((usGetPGNfromArbID(NewRecvMsg.RxCanMsg.can_id) == mJ1939_RxMsgTimeout[i].usPGN)&&
		   (ucGetSAfromArbID(NewRecvMsg.RxCanMsg.can_id) == mJ1939_RxMsgTimeout[i].ucSA)) {
			*(mJ1939_RxDataDef[i].pbDataStatus) = true;
			mJ1939_RxMsgTimeout[i].ulTimeout = 0;
			if(NewRecvMsg.ulMsgTstamp == 0){
				NewRecvMsg.ulMsgTstamp = getCurrentMsTimestamp();
			}
			mJ1939_RxMsgTimeout[i].ulPrevTimeout = NewRecvMsg.ulMsgTstamp;
		}else {
			if(NewRecvMsg.ulMsgTstamp == 0){
				NewRecvMsg.ulMsgTstamp = getCurrentMsTimestamp();
			}
			mJ1939_RxMsgTimeout[i].ulTimeout = NewRecvMsg.ulMsgTstamp - mJ1939_RxMsgTimeout[i].ulPrevTimeout;
			if(mJ1939_RxMsgTimeout[i].ulTimeout >= mJ1939_RxMsgTimeout[i].ulMaxTimeout) {
				*(mJ1939_RxDataDef[i].pbDataStatus) = false;
			}
		}
	}
}

void J1939Layer::j1939_updateDataParameters(struct CanMsgTstamp NewRecvMsg)
{
	signed long Tempvalue = 0;
	for(unsigned int i = 0; i < mEffectiveRxMsgNum; i++) {
		if(ucGetSAfromArbID(NewRecvMsg.RxCanMsg.can_id) == mJ1939_RxDataDef[i].ucSA) {
			if(usGetPGNfromArbID(NewRecvMsg.RxCanMsg.can_id) == mJ1939_RxDataDef[i].usPGN) {
				memcpy((void*)&Tempvalue,(void*)&NewRecvMsg.RxCanMsg.data[mJ1939_RxDataDef[i].ucStartByte],mJ1939_RxDataDef[i].ucNumBytes);
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
	j1939ParserTimer.tv_nsec = 1000000;//1ms for test Thread scheduling
	struct CanMsgTstamp NewRecvMsg;
	unsigned int uiSize = 0;
	while(J1939LayerInst->getCANStatus()) {
		if(J1939LayerInst->CANFifo->DequeueMessage((void *)&NewRecvMsg, &uiSize)){
			J1939LayerInst->j1939_updateDataParameters(NewRecvMsg);
		}else {
			//This is Tricky as we are in a Static context
			NewRecvMsg.ulMsgTstamp = 0;
			NewRecvMsg.RxCanMsg.can_id = 0xFFFFFFFF;
			NewRecvMsg.RxCanMsg.can_dlc = 0;
		}
		J1939LayerInst->j1939_updateDataValidity(NewRecvMsg);
		nanosleep(&j1939ParserTimer, NULL);
	}
	pthread_exit(NULL);
}


void J1939Layer::RemoveJ1939_RXParsers()
{
	for(unsigned int i = 0; i < mEffectiveRxMsgNum; i++) {
		mJ1939_RxDataDef[i] = {0};
		mJ1939_RxMsgTimeout[i] = {0};
	}
	delete [] mJ1939_RxDataDef;
	delete [] mJ1939_RxMsgTimeout;
	mEffectiveRxMsgNum = 0;
}

void J1939Layer::UpdateDTCStatuses(char * ucDM_DTC_Data)
{
	if((ucDM_DTC_Data != NULL) && (mEffectiveDtcSuppNum > 0)) {
		memcpy(ucDM_DTC_Data, ucDM_DTC_Status, mEffectiveDtcSuppNum);
	}
}

#include <iostream>
#include <chrono> //include timestamp operations
#include "main_app/j1939.h"
#include "logger/lib_logger.h"

#define TAG "J1939Layer"

using namespace std;
using namespace std::chrono;

J1939Layer::J1939Layer(std::string CanFifoName, std::string CanInfName, const J1939_eRxDataInfo * J1939_RxDataParams, int size,
                       const stDM_iDTCDataStruct* J1939_DtcDiagStruct, int Dtcsize):
CANDrv(CanFifoName, CanInfName, static_cast<unsigned long>(J1939_BaudRate), NULL),
mEffectiveRxMsgNum(cstJ1939_Num_MsgRX)//Take Default Max value
{
	ALOGD(TAG, __FUNCTION__, "CTOR");
	if(getCANStatus()) {
		ALOGD(TAG, __FUNCTION__, "J1939 Initialized Successfully");
		InstallJ1939_RXParsers(J1939_RxDataParams, size);
		J1939_InitDiagMachine(J1939_DtcDiagStruct, Dtcsize);
		pthread_create(&J1939Thread, NULL, pvthJ1939ParseFrames_Exe, this);
	}
}

J1939Layer::J1939Layer(string CanFifoName, string CanInfName, const J1939_eRxDataInfo * J1939_RxDataParams, int size,
                       const stDM_iDTCDataStruct* J1939_DtcDiagStruct, int Dtcsize, struct can_filter * J1939Filters):
CANDrv(CanFifoName, CanInfName, static_cast<unsigned long>(J1939_BaudRate), J1939Filters),
mEffectiveRxMsgNum(cstJ1939_Num_MsgRX)//Take Default Max value
{
	ALOGD(TAG, __FUNCTION__, "CTOR");
	if(getCANStatus()) {
		ALOGD(TAG, __FUNCTION__, "J1939 Initialized Successfully");
		InstallJ1939_RXParsers(J1939_RxDataParams, size);
		J1939_InitDiagMachine(J1939_DtcDiagStruct, Dtcsize);
		pthread_create(&J1939Thread, NULL, pvthJ1939ParseFrames_Exe, this);
	}
}


J1939Layer::~J1939Layer()
{
	ALOGD(TAG, __FUNCTION__, "DTOR");
	//join Parsing Thread
	pthread_join(J1939Thread, NULL);
	RemoveJ1939_RXParsers();
	J1939_ExitDiagMachine();
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


void J1939Layer::InstallJ1939_RXParsers(const J1939_eRxDataInfo *J1939_RxDataParams, int size)
{
	if(J1939_RxDataParams != NULL) {
		mEffectiveRxMsgNum = (size <= cstJ1939_Num_MsgRX) ? size : cstJ1939_Num_MsgRX;
		mJ1939_RxDataDef = new J1939_eRxDataInfo[mEffectiveRxMsgNum];
		mJ1939_RxMsgTimeout = new J1939_eRXMsgTimeout[mEffectiveRxMsgNum];

		if((mJ1939_RxDataDef != NULL) && (mJ1939_RxMsgTimeout != NULL)) {
			memcpy((void*)mJ1939_RxDataDef, (void*)J1939_RxDataParams, mEffectiveRxMsgNum*sizeof(J1939_RxDataParams[0]));
			
			for(unsigned int j = 0; j < mEffectiveRxMsgNum; j++) {
				mJ1939_RxMsgTimeout[j].usPGN = J1939_RxDataParams[j].usPGN;
				mJ1939_RxMsgTimeout[j].ucSA = J1939_RxDataParams[j].ucSA;
				mJ1939_RxMsgTimeout[j].ulTimeout = 0;
				mJ1939_RxMsgTimeout[j].ulPrevTimeout = 0;
				mJ1939_RxMsgTimeout[j].ulMaxTimeout = J1939_RxDataParams[j].Timeout;
			}
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


void J1939Layer::j1939_ParseNewDataMsg(struct CanMsgTstamp NewRecvMsg)
{
	/*the Logic here is to check
	 * One: Simple Diagnostic PGN (DM1, DM2 or DM13) ==> Process with updating Diag Buffer
	 * 		and call j1939_updateDiagParameters
	 * Two: If it's Multipacket Frame
	 * 		Step 1: Process first message and extract Page Number, Number of Total bytes ...
	 * 		Step 2: Process all the following sequences, update Diag buffer and call
	 * 				j1939_updateDiagParameters
	 * Three: Else ==> J1939 Data param messsage ==> Call j1939_updateDataParameters
	 */
	 bool isValidDtcMsg = false;
	 unsigned short usRecvPGN = usGetPGNfromArbID(NewRecvMsg.RxCanMsg.can_id);
	 unsigned char usRecvSA   = ucGetSAfromArbID (NewRecvMsg.RxCanMsg.can_id);
	 if((usRecvPGN == mJ1939_DmPgnList[DM_DM1_PGN]) || (usRecvPGN == mJ1939_DmPgnList[DM_DM2_PGN])
	    || (usRecvPGN == mJ1939_DmPgnList[DM_DM13_PGN])) { //Single-Frame DTC Data
			for(unsigned int i =0; i < mEffectiveDtcSuppNum; i++){
				if(mJ1939_SuppDtcDiag[i].ucSA == usRecvSA) {
					isValidDtcMsg = true;
					break;
				}
			}
			if(isValidDtcMsg) {
				mJ1939_DmDTC_MPTotalBytes = 6; // we don't care about last two bytes
				memset(mJ1939_DmDTC_TotBuffer, 0, mJ1939_DmDTC_TotalBufferSize);
				memcpy((void*)&mJ1939_DmDTC_TotBuffer[0], (void*)&NewRecvMsg.RxCanMsg.data[0], mJ1939_DmDTC_MPTotalBytes);
				j1939_updateDiagParameters(mJ1939_DmDTC_TotBuffer, mJ1939_DmDTC_MPTotalBytes);
			}
	 } else if(usRecvPGN == mJ1939_DmPgnList[DM_TP_CM_BAM_PGN]) { //MultiPacket-Frame DTC Data - First BAM Msg
		for(unsigned int i =0; i < mEffectiveDtcSuppNum; i++){
			if(mJ1939_SuppDtcDiag[i].ucSA == usRecvSA) {
				isValidDtcMsg = true;
				break;
			}
		}
		if(isValidDtcMsg){
			bool DM_MP_SupportedPGN = false;
			//Received MultiPacket PGN can be DM1, DM2 or DM13
			DM_MP_SupportedPGN = DM_MP_SupportedPGN || ((NewRecvMsg.RxCanMsg.data[5] == ((mJ1939_DmPgnList[DM_DM1_PGN] >> 0) & 0xFF))       // DM1 PGN
								   && (NewRecvMsg.RxCanMsg.data[6] == ((mJ1939_DmPgnList[DM_DM1_PGN] >> 8) & 0xFF)));
			DM_MP_SupportedPGN = DM_MP_SupportedPGN ||  ((NewRecvMsg.RxCanMsg.data[5] == ((mJ1939_DmPgnList[DM_DM2_PGN] >> 0) & 0xFF))       // DM2 PGN
								   && (NewRecvMsg.RxCanMsg.data[6] == ((mJ1939_DmPgnList[DM_DM2_PGN] >> 8) & 0xFF)));
			DM_MP_SupportedPGN = DM_MP_SupportedPGN ||  ((NewRecvMsg.RxCanMsg.data[5] == ((mJ1939_DmPgnList[DM_DM13_PGN] >> 0) & 0xFF))       // DM2 PGN
								   && (NewRecvMsg.RxCanMsg.data[6] == ((mJ1939_DmPgnList[DM_DM13_PGN] >> 8) & 0xFF)));					   
			
			if ((NewRecvMsg.RxCanMsg.data[0] == 0x20)         // Control Byte
			     && DM_MP_SupportedPGN                      // Received MultiPacket PGN can be DM1, DM2 or DM13
                 && !(NewRecvMsg.RxCanMsg.data[7] & 1)) {   // For the moment we support only 1-Page 0
				mJ1939_DmDTC_MPTotalBytes   = NewRecvMsg.RxCanMsg.data[2];
				mJ1939_DmDTC_MPTotalBytes <<= 8;
				mJ1939_DmDTC_MPTotalBytes  += NewRecvMsg.RxCanMsg.data[1];
				mJ1939_DmDTC_MPTotSeq       = NewRecvMsg.RxCanMsg.data[3];
				mJ1939_DmDTC_MPSeqNumber    = 0;
				memset(mJ1939_DmDTC_TotBuffer, 0, mJ1939_DmDTC_TotalBufferSize);
				mJ1939_DmDTC_NewMPFrameRecv = true;
			}
		}

	 } else if(usRecvPGN == mJ1939_DmPgnList[DM_TP_DT_PGN]) { //MultiPacket-Frame DTC Data - Consecutive Sequences
		 ;//
	 }
	 else { //J1939 Data Params Frame
		 j1939_updateDataParameters(NewRecvMsg);
	 }
}

void J1939Layer::j1939_updateDiagParameters(char * pucDTCDiagBuffer, unsigned int uiDiagBufferSize)
{

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
			J1939LayerInst->j1939_ParseNewDataMsg(NewRecvMsg);
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
	if((ucDM_DTC_Data != NULL) && (mEffectiveDtcSuppNum > 0) && (ucDM_DTC_Status != NULL)) {
		memcpy(ucDM_DTC_Data, ucDM_DTC_Status, mEffectiveDtcSuppNum);
	}
}

void J1939Layer::J1939_InitDiagMachine(const stDM_iDTCDataStruct* J1939_DtcDiagStruct, int Dtcsize)
{
	if((J1939_DtcDiagStruct != NULL) && (Dtcsize > 0)) {
		unsigned short TempDiagPgnList[DM_TOT_PGN] = {0xEBFF, 0xECFF, 0xFECA, 0xFECB, 0xDF00};
		for(int i = 0; i < DM_TOT_PGN; i++)
			mJ1939_DmPgnList[i] = TempDiagPgnList[i];
		mEffectiveDtcSuppNum = (Dtcsize <= cstDTC_Num_SupportedDTC) ? Dtcsize:cstDTC_Num_SupportedDTC;
		ucDM_DTC_Status = new unsigned char[mEffectiveDtcSuppNum];
		mJ1939_SuppDtcDiag = new stDM_iDTCDataStruct[mEffectiveDtcSuppNum];
		// 4 bytes for each DTC SPN-FMI combination + 2 bytes for Lamps Data
		mJ1939_DmDTC_TotalBufferSize = static_cast<unsigned short> ( (mEffectiveDtcSuppNum * 4)+2);
		mJ1939_DmDTC_TotBuffer = new char[mJ1939_DmDTC_TotalBufferSize];
		if((mJ1939_SuppDtcDiag != NULL) && (ucDM_DTC_Status != NULL)) {
			memcpy((void*)mJ1939_SuppDtcDiag, (void*)J1939_DtcDiagStruct, mEffectiveDtcSuppNum*sizeof(J1939_DtcDiagStruct[0]));
		}
	}
}

void J1939Layer::J1939_ExitDiagMachine()
{
	if((mJ1939_SuppDtcDiag != NULL) && (mEffectiveDtcSuppNum > 0)) {
		delete [] ucDM_DTC_Status;
		delete [] mJ1939_SuppDtcDiag;
		delete [] mJ1939_DmDTC_TotBuffer;
		mJ1939_DmDTC_TotalBufferSize = 0;
		mEffectiveDtcSuppNum = 0;
	}
}

#ifndef __J1939_H__
#define __J1939_H__

#include "can/can_drv.h"

#define J1939_BaudRate 250000
#define cstJ1939_Num_MsgRX 20

struct J1939_eRxDataInfo
{
  unsigned short usPGN; //Parameter Group Number: CCVS, Fluids, Temps ...
  unsigned char  ucSA;  //Source Address: ECU, TCU, BCU ...
  unsigned char  ucStartByte 	 : 4; //Start Byte: 0 to 7
  unsigned char  ucNumBytes  	 : 4; //Number of Bytes containing Parameter Data: 1, 2, 3, 4
  signed long    ulMulA; //Calculation Formula: Value = (Raw_Data* A)/B +C
  signed long    ulDivB;
  signed long    slOffsetC;
  signed long    slMinRange; //Parameter Min Range
  signed long    slMaxRange; //Parameter Max Range
  unsigned long long Timeout; //Data Timeout Value
  void *pucData;
  bool *pbDataStatus;
};

struct J1939_eRXMsgTimeout
{
  unsigned short usPGN; //Parameter Group Number
  unsigned char  ucSA;  //Source Address
  unsigned long long ulTimeout; //Increasing Timeout value in ms
  unsigned long long ulPrevTimeout;//Last valid Received Message Timestamp
  unsigned long long ulMaxTimeout;//Max Tiemout value in ms
};

class J1939Layer: public CANDrv
{
	public:
		J1939Layer(std::string CanFifoName, std::string CanInfName, const struct J1939_eRxDataInfo * J1939_RxDataParams, unsigned int size,
		           struct can_filter * J1939Filters);
		~J1939Layer();
		bool SendJ1939Msg(struct can_frame &TxCanMsg);
		void ForceStopCAN();
	
	private:
		pthread_t J1939Thread;
		static void * pvthJ1939ParseFrames_Exe (void* context);
		struct J1939_eRxDataInfo mJ1939_RxDataDef[cstJ1939_Num_MsgRX];
		struct J1939_eRXMsgTimeout mJ1939_RxMsgTimeout[cstJ1939_Num_MsgRX];
		unsigned short mEffectiveRxMsgNum;
		void RemoveJ1939_RXParsers();
		void InstallJ1939_RXParsers(const struct J1939_eRxDataInfo * J1939_RxDataParams, unsigned int size);
		void j1939_updateDataValidity(struct CanMsgTstamp NewRecvMsg);
		void j1939_updateDataParameters(struct CanMsgTstamp NewRecvMsg);
		unsigned short usGetPGNfromArbID(unsigned long ulArbID);
		unsigned char ucGetSAfromArbID(unsigned long ulArbID);
		unsigned long ulBuildExtCanId(unsigned char ucSA, unsigned short usPGN);
		unsigned long long getCurrentMsTimestamp();
};

#endif

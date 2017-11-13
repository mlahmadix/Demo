#ifndef __J1939_H__
#define __J1939_H__

#include "can/can_drv.h"

#define J1939_BaudRate 250000
#define cstJ1939_Num_MsgRX 20

struct J1939_eRxDataInfo
{
  unsigned short usPGN;
  unsigned char  ucSA;
  unsigned char  ucStartByte 	 : 4;
  unsigned char  ucNumBytes  	 : 4;
  signed long    ulMulA;
  signed long    ulDivB;
  signed long    slOffsetC;
  signed long    slMinRange;
  signed long    slMaxRange;
  void *pucData;
};

class J1939Layer: public CANDrv
{
	public:
		J1939Layer(std::string CanFifoName, std::string CanInfName, const struct J1939_eRxDataInfo * J1939_RxDataParams, unsigned int size);
		~J1939Layer();
		bool SendJ1939Msg(struct can_frame &TxCanMsg);
		void ForceStopCAN();
	
	private:
		pthread_t J1939Thread;
		static void * pvthJ1939ParseFrames_Exe (void* context);
		struct J1939_eRxDataInfo mJ1939_RxDataDef[cstJ1939_Num_MsgRX];
		unsigned short mEffectiveRxMsgNum;
		void RemoveJ1939_RXParsers();
		void InstallJ1939_RXParsers(const struct J1939_eRxDataInfo * J1939_RxDataParams, unsigned int size);
		void j1939_updateDataParameters(struct can_frame NewRecvMsg);
		unsigned short usGetPGNfromArbID(unsigned long ulArbID);
		unsigned char ucGetSAfromArbID(unsigned long ulArbID);
};

#endif

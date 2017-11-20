#ifndef __J1939_H__
#define __J1939_H__

#include "can/can_drv.h"

#define J1939_BaudRate 250000
#define cstJ1939_Num_MsgRX 100
#define cstDTC_Num_SupportedDTC 255
/*
 *  Diagnostic Messages definition
 */
#define DM_TP_DT_PGN          0xEBFF //Presence of Diagnostic - Multipacket Frame
#define DM_TP_CM_BAM_PGN      0xECFF //Sequence of Multipacket Frames
//Page0 - Sequence 1
// ...
//Page0 - Sequence N
//...
//PageN - Sequence N
#define DM_DM1_PGN            0xFECA //Diagnostic Message 1  - Simple Packet
#define DM_DM2_PGN            0xFECB //Diagnostic Message 2  - Simple Packet
#define DM_DM13_PGN           0xDF00 //Diagnostic Message 13 - Simple Packet

/*
 *  Diagnostic Lamps Definition
 */
enum DM_CklampsEnum
{
   DM_ProtectLamp      = 0x01,
   DM_AmberWarningLamp = 0x04,
   DM_RedStopLamp      = 0x10,
   DM_MalfunctionLamp  = 0x40,
   DM_AnyLamps         = (DM_ProtectLamp | DM_AmberWarningLamp | DM_RedStopLamp | DM_MalfunctionLamp)
};

/*
 *  FMI Matching rules (Failure Mode Identifier)
 */
enum DM_FmiMatchRules
{
   DM_DontFMICare = 0,
   DM_MatchFMI1,
   DM_MatchFMI2,
   DM_MatchFMI3,
   DM_MatchFMI1orFMI2,
   DM_MatchFMI1orFMI2orFMI3
};

/*
 *  DTC Structure (Diagnostic Trouble codes)
 */
typedef struct
{
    unsigned char  ucSA;            // Source Address for this DTC
    unsigned long  ulSPN;           // Suspect Parameter Number for this DTC
    unsigned char  ucLamps;         // DTC Associated Lamps
    unsigned char  ucMatchSPN : 1;  // 1 : Must match SPN, 0 : Don't care about SPN
    unsigned char  ucMaskRule : 3;  // 0 : Don't care about FMI 
								    // 1 : Match FMI1
								    // 2 : Match FMI2
								    // 3 : Match FMI3
                                    // 4 : Match FMI1 or FMI2 
                                    // 5 : Match either FMI1, FMI2 or FMI3
    unsigned char ucReserved: 4;    // Not Used
    unsigned char ucFMI1;           // FMI1 value
    unsigned char ucFMI2;           // FMI2 value
    unsigned char ucFMI3;           // FMI3 value
}stDM_iDTCDataStruct;

/*
 *  J1939 Data Structure
 */
typedef struct
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
}J1939_eRxDataInfo;

/*
 *  J1939/DM Reception Timeout Structure
 */
typedef struct 
{
  unsigned short usPGN; //Parameter Group Number
  unsigned char  ucSA;  //Source Address
  unsigned long long ulTimeout; //Increasing Timeout value in ms
  unsigned long long ulPrevTimeout;//Last valid Received Message Timestamp
  unsigned long long ulMaxTimeout;//Max Tiemout value in ms
}J1939_eRXMsgTimeout;

class J1939Layer: public CANDrv
{
	public:
		J1939Layer(std::string CanFifoName, std::string CanInfName, const J1939_eRxDataInfo * J1939_RxDataParams, int size);
		J1939Layer(std::string CanFifoName, std::string CanInfName, const J1939_eRxDataInfo * J1939_RxDataParams, int size,
		           struct can_filter * J1939Filters);
		~J1939Layer();
		bool SendJ1939Msg(struct can_frame &TxCanMsg);
		void ForceStopCAN();
		void UpdateDTCStatuses(char * ucDM_DTC_Data);
	
	private:
		pthread_t J1939Thread;
		static void * pvthJ1939ParseFrames_Exe (void* context);
		J1939_eRxDataInfo * mJ1939_RxDataDef;
		J1939_eRXMsgTimeout * mJ1939_RxMsgTimeout;
		unsigned long mEffectiveRxMsgNum;
		void RemoveJ1939_RXParsers();
		void InstallJ1939_RXParsers(const J1939_eRxDataInfo * J1939_RxDataParams, int size);
		void j1939_updateDataValidity(struct CanMsgTstamp NewRecvMsg);
		void j1939_updateDataParameters(struct CanMsgTstamp NewRecvMsg);
		unsigned short usGetPGNfromArbID(unsigned long ulArbID);
		unsigned char ucGetSAfromArbID(unsigned long ulArbID);
		unsigned long ulBuildExtCanId(unsigned char ucSA, unsigned short usPGN);
		unsigned long long getCurrentMsTimestamp();
		unsigned long mEffectiveDtcSuppNum;
		/*
		 * Status of Different Supported DTC
		 * 0: DTC Not active
		 * 1: DTC Active
		 * 2: DTC was active
		 * 3: N/A
		 */
		unsigned char ucDM_DTC_Status[cstDTC_Num_SupportedDTC];
};

#endif

#ifndef __J1939_H__
#define __J1939_H__

#include "can/can_drv.h"

#define CAN_J1939_PROTO 5

#define J1939_BaudRate 250000
#define cstJ1939_Num_MsgRX 100
#define cstDTC_Num_SupportedDTC 255
#define cstDTC_Def_Comm_Timeout 5000 //5s

#define NO_CAN_ID 0xFFFFFFFFU

/*
 *  Diagnostic Messages definition
 */
enum DM_PgnList
{
	DM_TP_DT_PGN, 	   //Presence of Diagnostic - Multipacket Frame
	DM_TP_CM_BAM_PGN,  //Sequence of Multipacket Frames
									//Page0 - Sequence 1
									// ...
									//Page0 - Sequence N
									//...
									//PageN - Sequence N
	DM_DM1_PGN,       //Diagnostic Message 1  - Simple Packet
	DM_DM2_PGN,       //Diagnostic Message 2  - Simple Packet
	DM_DM13_PGN,      //Diagnostic Message 13 - Simple Packet
	DM_TOT_PGN
};

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
    unsigned char  ucMatchSPN    : 1;  // 1 : Must match SPN, 0 : Don't care about SPN
    unsigned char  ucFMIMaskRule : 3;  // 0 : Don't care about FMI 
								    // 1 : Match FMI1
								    // 2 : Match FMI2
								    // 3 : Match FMI3
                                    // 4 : Match FMI1 or FMI2 
                                    // 5 : Match either FMI1, FMI2 or FMI3
    unsigned char ucFMI1;           // FMI1 value
    unsigned char ucFMI2;           // FMI2 value
    unsigned char ucFMI3;           // FMI3 value
    unsigned char ucLamps;         // DTC Associated Lamps
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
				   const stDM_iDTCDataStruct* J1939_DtcDiagStruct, int Dtcsize);
		~J1939Layer();
		bool SendJ1939Msg(struct can_frame &TxCanMsg);
		void ForceStopCAN();
		void UpdateDTCStatuses(char * ucDM_DTC_Data);
		bool setCanFilters(struct can_filter * AppliedFilters, unsigned int size);
	
	private:
		pthread_t J1939Thread;
		static void * pvthJ1939ParseFrames_Exe (void* context);
		J1939_eRxDataInfo * mJ1939_RxDataDef;
		J1939_eRXMsgTimeout * mJ1939_RxMsgTimeout;
		unsigned long mEffectiveRxMsgNum;
		void RemoveJ1939_RXParsers();
		void InstallJ1939_RXParsers(const J1939_eRxDataInfo * J1939_RxDataParams, int size);
		void j1939_updateDataValidity(struct CanMsgTstamp NewRecvMsg);
		void j1939_ParseNewDataMsg(struct CanMsgTstamp NewRecvMsg);
		void j1939_updateDataParameters(struct CanMsgTstamp NewRecvMsg);
		void j1939_updateDiagParameters(char * DiagTotalBuffer, unsigned long ulTotSize);
		unsigned short usGetPGNfromArbID(unsigned long ulArbID);
		unsigned char ucGetSAfromArbID(unsigned long ulArbID);
		unsigned long ulBuildExtCanId(unsigned char ucSA, unsigned short usPGN);
		unsigned long long getCurrentMsTimestamp();
		void J1939_InitDiagMachine(const stDM_iDTCDataStruct* J1939_DtcDiagStruct, int Dtcsize);
		void J1939_ExitDiagMachine();
		
		//Following definitions are reserved to DTC J1939 messages
		unsigned long mEffectiveDtcSuppNum;
		stDM_iDTCDataStruct* mJ1939_SuppDtcDiag;
		/*
		 * Status of Different Supported DTC
		 * 0: DTC Not active
		 * 1: DTC Active
		 * 2: DTC was active
		 * 3: N/A
		 */
		enum J1939_DmDTC_Status {
			CeJ1939_DmDTC_OFF = 0,
			CeJ1939_DmDTC_ON,
			CeJ1939_DmDTC_INACTIVE,
			CeJ1939_DmDTC_NA,
			CeJ1939_DmDTC_TOTAL
		};
		unsigned char * ucDM_DTC_Status;
		char * mJ1939_DmDTC_TotBuffer;
		unsigned char mJ1939_DmDTC_MPSeqNumber;//DTC Multipacket Sequence Number
		unsigned char mJ1939_DmDTC_MPTotSeq;//DTC Total Number of Sequences
		unsigned short mJ1939_DmDTC_MPTotalBytes;//DTC Total Number of Sequences
		unsigned short mJ1939_DmDTC_MPRemainBytes;//DTC Total Number of Sequences
		unsigned short mJ1939_DmDTC_TotalBufferSize;//DTC Total Number of bytes
		bool mJ1939_DmDTC_NewMPFrameRecv;//New DTC Multi-packet frame received
		unsigned long mJ1939_DmDTC_TimeoutValue; //DM Multipacket Timeout counter value
		bool mJ1939_DmDTC_MPValidData;  //DM Multipacket Sequence Data validity check
		unsigned short mJ1939_DmPgnList[DM_TOT_PGN];//J1939 DTC List supported
		//check DTC Lamp received value
		bool j1939_CheckDTCLamps(unsigned char ucLamps, unsigned char ucLampsDTCMask);
		bool j1939_CheckFMIMatch(char * pucDiagBuffer, unsigned long ulLength, unsigned char ucFMI);
		bool j1939_CheckSPNFMIMatch(char * pucDiagBuffer, unsigned long ulLength, unsigned long ulSPN, unsigned char ucFMI);
};

#endif

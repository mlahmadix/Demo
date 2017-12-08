#ifndef __CAN_DRV_H__
#define __CAN_DRV_H__

#include <linux/can.h>
#include <linux/can/raw.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <net/if.h>
#include <pthread.h>
#include <sys/types.h>
#include <string.h>
#include <unistd.h>
#include "fifo/lib_fifo.h"
#include "filelog/lib_filelog.h"
#include "can/isotp.h"

#define CANDefaultTimeoutBase 5000 //5s for HeatBet messages
#define CAN_J1939_PROTO 5
#define ISOTP_BUFSIZE 5000  //MAX ISOTP Data Size

class CANDrv
{
	public:
		~CANDrv();
	protected:
		CANDrv(std::string FifoName, int proto, std::string CanInterface, unsigned long baudrate, unsigned long ModeFlags);
		CANDrv(std::string FifoName, int proto, std::string CanInterface, unsigned long baudrate, unsigned long ModeFlags, unsigned long ulRxID,
		       unsigned long ulTxID);
		enum {
			CeCanDrv_NotInit = 0,
			CeCanDrv_Init,
		};
		struct CanMsgTstamp {
			struct can_frame RxCanMsg;
			unsigned long long ulMsgTstamp;
		};
		bool mCanDriverStatus;
		unsigned int iCANDrvInit;
		inline void setCANStatus(bool Canstatus) { mCANStatus =  Canstatus; }
		int CanRecvMsg(struct can_frame &RxCanMsg);
		int CanRecvMsg(unsigned char * DataBuff);
		bool CanSendMsg(struct can_frame TxCanMsg);
		bool CanSendMsg(const unsigned char * Buf, unsigned long ulLen);
		virtual bool setCanFilters(struct can_filter * AppliedFilters, unsigned int size) = 0;
		bool getCANStatus();
		void StopCANDriver();
		void printCanFrame(struct can_frame TxCanMsg);
		void printCanFrame(const unsigned char * Buf, unsigned long ulLen);
		Fifo * CANFifo;
		unsigned long long getCANMsgTimestamp();
	private:
	    virtual bool CheckKernelModule();
		bool initCanDevice(std::string CanInfName);
		#define CANFIFODepth 1000 //example of 1CAN message per 1ms = 1000messages/s
		bool mCANStatus;
		int sockCanfd;
		unsigned long mulModeFlags;
		unsigned long mulBaudrate;
		unsigned long uiDefCANRecTimeout;
		unsigned int muiCanInfIndex;
		std::string pucCanInfName;
		pthread_t Can_Thread;
		pthread_mutex_t Can_Mutex;
		int mCanProtocol;
		unsigned long mDiagRXID;
		unsigned long mDiagTXID;
		unsigned short uwGetCanBaudrate() { return mulBaudrate;}
		unsigned long ulGetCanModeFlags() { return mulModeFlags;}
		unsigned long ulGetCanInfIndex() { return muiCanInfIndex;}
		static void * pvthCanReadRoutine_Exe (void* context);
		static void * pvthIsotpReadRoutine_Exe (void* context);
		DataFileLogger * CanDataLogger;
		enum CeCanMsgDir {
			CeCanDir_TX = 0,
			CeCanDir_RX
		}; 
		void LogCanMsgToFile(struct can_frame CanMsg, CeCanMsgDir MsgDir);
		void LogCanMsgToFile(const unsigned char * Msg, unsigned long ulLen, CeCanMsgDir MsgDir);
		

};

#endif

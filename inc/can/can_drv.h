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

#define CANDefaultTimeoutBase 5000 //5s for HeatBet messages

class CANDrv
{
	public:
		~CANDrv();
	protected:
		CANDrv(std::string FifoName, struct can_filter * CANFilters);
		CANDrv(std::string FifoName, std::string CanInterface, struct can_filter * CANFilters);
		CANDrv(std::string FifoName, std::string CanInterface, unsigned long baudrate, struct can_filter * CANFilters);
		CANDrv(std::string FifoName, std::string CanInterface, unsigned long baudrate, unsigned long ModeFlags, struct can_filter * CANFilters);
		enum {
			CeCanDrv_NotInit = 0,
			CeCanDrv_Init,
		};
		struct CanMsgTstamp {
			struct can_frame RxCanMsg;
			unsigned long long ulMsgTstamp;
		};
		unsigned int iCANDrvInit;
		inline void setCANStatus(bool Canstatus) { mCANStatus =  Canstatus; }
		int CanRecvMsg(struct can_frame &RxCanMsg);
		bool CanSendMsg(struct can_frame &TxCanMsg);
		bool setCanFilters(struct can_filter * AppliedFilters, unsigned int size);
		bool getCANStatus();
		void StopCANDriver();
		void printCanFrame(struct can_frame TxCanMsg);
		Fifo * CANFifo;
		unsigned long long getCANMsgTimestamp();
	private:
		bool initCanDevice(std::string CanInfName, struct can_filter * CANFilters);
		#define CANFIFODepth 1000 //example of 1CAN message per 1ms = 1000messages/s
		#define CANFilterSupported 10
		bool mCANStatus;
		int sockCanfd;
		unsigned long mulModeFlags;
		unsigned long mulBaudrate;
		unsigned long uiDefCANRecTimeout;
		unsigned int muiCanInfIndex;
		std::string pucCanInfName;
		struct can_filter mCANfilters[CANFilterSupported];
		pthread_t Can_Thread;
		pthread_mutex_t Can_Mutex;
		unsigned short uwGetCanBaudrate() { return mulBaudrate;}
		unsigned long ulGetCanModeFlags() { return mulModeFlags;}
		unsigned long ulGetCanInfIndex() { return muiCanInfIndex;}
		static void * pvthCanReadRoutine_Exe (void* context);
		DataFileLogger * CanDataLogger;
		enum CeCanMsgDir {
			CeCanDir_TX = 0,
			CeCanDir_RX
		}; 
		void LogCanMsgToFile(struct can_frame CanMsg, CeCanMsgDir MsgDir);
		

};

#endif

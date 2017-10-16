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

class CANDrv
{
	public:
		CANDrv(std::string FifoName);
		CANDrv(std::string FifoName, std::string CanInterface);
		CANDrv(std::string FifoName, std::string CanInterface, unsigned long baudrate);
		CANDrv(std::string FifoName, std::string CanInterface, unsigned long baudrate, unsigned long ModeFlags);
		~CANDrv();
		int CanRecvMsg(struct can_frame &RxCanMsg);
		bool CanSendMsg(struct can_frame &TxCanMsg);
		bool setCanFilters(struct can_filter * AppliedFilters);
		bool getCANStatus();
		void StopCANDriver();
	
	private:
		#define CANFIFODepth 1000 //example of 1CAN message per 1ms = 1000messages/s
		#define CANFilterSupported 10
		Fifo * CANFifo;
		bool mCANStatus;
		int sockCanfd;
		unsigned long mulModeFlags;
		unsigned long mulBaudrate;
		unsigned int muiCanInfIndex;
		std::string pucCanInfName;
		struct can_filter mCANfilters[CANFilterSupported];
		pthread_t Can_Thread;
		pthread_mutex_t Can_Mutex;
		unsigned short uwGetCanBaudrate() { return mulBaudrate;}
		unsigned long ulGetCanModeFlags() { return mulModeFlags;}
		unsigned long ulGetCanInfIndex() { return muiCanInfIndex;}
		static void * pvthCanReadRoutine_Exe (void* context);
		void printCanFrame(struct can_frame TxCanMsg);
		bool initCanDevice(std::string CanInfName);
		inline void setCANStatus(bool Canstatus) { mCANStatus =  Canstatus; }
};

#endif

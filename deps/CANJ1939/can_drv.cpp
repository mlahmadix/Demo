#include <iostream>
#include <fcntl.h>
#include "can/can_drv.h"
#include "logger/lib_logger.h"

using namespace std;
#define TAG "CANDrv"

CANDrv::CANDrv(string FifoName):
mCANStatus(true),
sockCanfd(-1),
mulModeFlags(0),
mulBaudrate(static_cast<unsigned long>(250000))
{
	ALOGD(TAG, __FUNCTION__, "CTOR");
	CANFifo = new Fifo(CANFIFODepth, FifoName.c_str());
	if(!initCanDevice("vcan0")) {
		ALOGE(TAG, __FUNCTION__, "Fail to init CAN Interface");
		setCANStatus(false);
	}else {
		pthread_create(&Can_Thread, NULL, pvthCanReadRoutine_Exe, this);
	}
}

CANDrv::CANDrv(string FifoName, string CanInterface):
mCANStatus(false),
sockCanfd(-1),
mulModeFlags(0),
mulBaudrate(static_cast<unsigned long>(250000))
{
	ALOGD(TAG, __FUNCTION__, "CTOR");
	CANFifo = new Fifo(CANFIFODepth, FifoName.c_str());
	if(!initCanDevice(CanInterface)) {
		ALOGE(TAG, __FUNCTION__, "Fail to init CAN Interface");
		setCANStatus(false);
	}
}

CANDrv::CANDrv(string FifoName, string CanInterface, unsigned long baudrate):
mCANStatus(false),
sockCanfd(-1),
mulModeFlags(0),
mulBaudrate(baudrate)
{
	ALOGD(TAG, __FUNCTION__, "CTOR");
	CANFifo = new Fifo(CANFIFODepth, FifoName.c_str());
	if(!initCanDevice(CanInterface)) {
		ALOGE(TAG, __FUNCTION__, "Fail to init CAN Interface");
		setCANStatus(false);
	}
}

CANDrv::CANDrv(string FifoName, string CanInterface, unsigned long baudrate, unsigned long ModeFlags):
mCANStatus(false),
sockCanfd(-1),
mulModeFlags(ModeFlags),
mulBaudrate(baudrate)
{
	ALOGD(TAG, __FUNCTION__, "CTOR");
	CANFifo = new Fifo(CANFIFODepth, FifoName.c_str());
	if(!initCanDevice(CanInterface)) {
		ALOGE(TAG, __FUNCTION__, "Fail to init CAN Interface");
		setCANStatus(false);
	}
}

CANDrv::~CANDrv()
{
	ALOGD(TAG, __FUNCTION__, "DTOR");
	//shutdown associated CAN socket
	close(sockCanfd);
	sockCanfd = -1;
	//join Reception Thread
	pthread_join(Can_Thread, NULL);
	//delete CAN Reception FIFO
	delete CANFifo;
}

bool CANDrv::initCanDevice(string CanInfName)
{
	struct ifreq ifr;
	struct sockaddr_can addr;
	memset(&ifr, 0x0, sizeof(ifr));
	memset(&addr, 0x0, sizeof(addr));
	/* open CAN_RAW socket */
	sockCanfd = socket(PF_CAN, SOCK_RAW, CAN_RAW);
	/* convert interface string "can0" into interface index */
	strcpy(ifr.ifr_name, CanInfName.c_str());
	ioctl(sockCanfd, SIOCGIFINDEX, &ifr);
	/* setup address for bind */
	addr.can_ifindex = ifr.ifr_ifindex;
	addr.can_family = PF_CAN;
	/* Set CAN Socket to Non-Blocking
	 * So we can interrupt the read routing */
	int flags = fcntl(sockCanfd, F_GETFL, 0);
	flags |= O_NONBLOCK;
	fcntl(sockCanfd, F_SETFL, flags);
	/* bind socket to the can0 interface */
	bind(sockCanfd, (struct sockaddr *)&addr, sizeof(addr));
	//Fix-me Correct status to be returned
	return true;
}

int CANDrv::CanRecvMsg(struct can_frame &RxCanMsg)
{
	if(read(sockCanfd, &RxCanMsg, sizeof(struct can_frame)) < 0) {
		return -1;
	} else{
		return (sizeof(struct can_frame));
	}
}

bool CANDrv::CanSendMsg(struct can_frame &TxCanMsg)
{
	if(write(sockCanfd, &TxCanMsg, sizeof(TxCanMsg)) == sizeof(TxCanMsg)){
		ALOGD(TAG, __FUNCTION__, "CAN Msg sent successfully");
		printCanFrame(TxCanMsg);
		return true;
	}else{
		ALOGE(TAG, __FUNCTION__, "Fail to send CAN Msg");
		return false;
	}
}

void * CANDrv::pvthCanReadRoutine_Exe (void* context)
{
	CANDrv * CanDrvInst = static_cast<CANDrv *>(context);
	struct can_frame RxCanMsg;
	struct timespec CanRecvTimer;
	CanRecvTimer.tv_sec = 0;
	CanRecvTimer.tv_nsec = 3000000;
	while(CanDrvInst->mCANStatus)
	{
		if(CanDrvInst->CanRecvMsg(RxCanMsg) < 0){
			;
		}else {
			//CanDrvInst->printCanFrame(RxCanMsg);
			//Put in appropriate CAN FIFO for later processing
			//in upper layers: J1939, SmartCraft, NMEA2000, KWP2k, DiagOnCan
			//Should be done in Locked Context
			if(CanDrvInst->CANFifo->EnqueueMessage((void *)&RxCanMsg,8+RxCanMsg.can_dlc) == true) {
				ALOGD(TAG, __FUNCTION__, "CAN Message queued successfully");
			} else {
				ALOGE(TAG, __FUNCTION__, "Fail to queue CAN message");
			}
		}
	    //unlock Context
		nanosleep(&CanRecvTimer, NULL);
	}
	pthread_exit(NULL);
}

void CANDrv::printCanFrame(struct can_frame TxCanMsg)
{
	ALOGD(TAG, __FUNCTION__, "ArbId = 0x%8X", TxCanMsg.can_id);
	ALOGD(TAG, __FUNCTION__, "Data Length Code = %d", (int)TxCanMsg.can_dlc);
	for(int j=0; j < TxCanMsg.can_dlc; j++) {
		ALOGD(TAG, __FUNCTION__, "Data index =%d \t Value=0x%02X", j, (int)TxCanMsg.data[j]);
	}
}

bool CANDrv::getCANStatus()
{
	return mCANStatus;
}

void CANDrv::StopCANDriver()
{
	setCANStatus(false);
}

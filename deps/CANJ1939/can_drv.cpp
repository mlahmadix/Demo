#include <iostream>
#include <fcntl.h>
#include "can/can_drv.h"
#include "logger/lib_logger.h"

using namespace std;
#define TAG "CANDrv"

CANDrv::CANDrv(string FifoName):
mCANStatus(false),
sockCanfd(-1),
mulModeFlags(0),
mulBaudrate(static_cast<unsigned long>(250000)),
uiDefCANRecTimeout(10000)
{
	ALOGD(TAG, __FUNCTION__, "CTOR");
	CANFifo = new Fifo(CANFIFODepth, FifoName.c_str());
	if(!initCanDevice("vcan0")) {
		ALOGE(TAG, __FUNCTION__, "Fail to init CAN Interface");
		setCANStatus(false);
	}else {
		iCANDrvInit = CeCanDrv_Init;
		setCANStatus(true);
		pthread_create(&Can_Thread, NULL, pvthCanReadRoutine_Exe, this);
	}
}

CANDrv::CANDrv(string FifoName, string CanInterface):
mCANStatus(false),
sockCanfd(-1),
mulModeFlags(0),
mulBaudrate(static_cast<unsigned long>(250000)),
uiDefCANRecTimeout(10000)
{
	ALOGD(TAG, __FUNCTION__, "CTOR");
	CANFifo = new Fifo(CANFIFODepth, FifoName.c_str());
	if(!initCanDevice(CanInterface)) {
		ALOGE(TAG, __FUNCTION__, "Fail to init CAN Interface");
		setCANStatus(false);
	}else {
		iCANDrvInit = CeCanDrv_Init;
		setCANStatus(true);
		pthread_create(&Can_Thread, NULL, pvthCanReadRoutine_Exe, this);
	}
}

CANDrv::CANDrv(string FifoName, string CanInterface, unsigned long baudrate):
mCANStatus(false),
sockCanfd(-1),
mulModeFlags(0),
mulBaudrate(baudrate),
uiDefCANRecTimeout(10000)
{
	ALOGD(TAG, __FUNCTION__, "CTOR");
	CANFifo = new Fifo(CANFIFODepth, FifoName.c_str());
	if(!initCanDevice(CanInterface)) {
		ALOGE(TAG, __FUNCTION__, "Fail to init CAN Interface");
		setCANStatus(false);
	}else {
		iCANDrvInit = CeCanDrv_Init;
		setCANStatus(true);
		pthread_create(&Can_Thread, NULL, pvthCanReadRoutine_Exe, this);
	}
}

CANDrv::CANDrv(string FifoName, string CanInterface, unsigned long baudrate, unsigned long ModeFlags):
mCANStatus(false),
sockCanfd(-1),
mulModeFlags(ModeFlags),
mulBaudrate(baudrate),
uiDefCANRecTimeout(10000)
{
	ALOGD(TAG, __FUNCTION__, "CTOR");
	CANFifo = new Fifo(CANFIFODepth, FifoName.c_str());
	if(!initCanDevice(CanInterface)) {
		ALOGE(TAG, __FUNCTION__, "Fail to init CAN Interface");
		setCANStatus(false);
	}else {
		iCANDrvInit = CeCanDrv_Init;
		setCANStatus(true);
		pthread_create(&Can_Thread, NULL, pvthCanReadRoutine_Exe, this);
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
	iCANDrvInit = CeCanDrv_NotInit;
}

bool CANDrv::initCanDevice(string CanInfName)
{
	struct ifreq ifr;
	struct sockaddr_can addr;
	memset(&ifr, 0x0, sizeof(ifr));
	memset(&addr, 0x0, sizeof(addr));
	/* open CAN_RAW socket */
	sockCanfd = socket(PF_CAN, SOCK_RAW, CAN_RAW);
	if(sockCanfd <=0) {
		ALOGE(TAG, __FUNCTION__, "Fail to create RAW Socket");
		return false;
	}
	strcpy(ifr.ifr_name, CanInfName.c_str());
	if(ioctl(sockCanfd, SIOCGIFINDEX, &ifr)){
		ALOGE(TAG, __FUNCTION__, "Cannot Retrieve Interface Index");
		return false;
	}
	/* setup address for bind */
	addr.can_ifindex = ifr.ifr_ifindex;
	addr.can_family = PF_CAN;
	/* Set CAN Socket to Non-Blocking
	 * So we can interrupt the read routing */
	int flags = fcntl(sockCanfd, F_GETFL, 0);
	if(flags != -1) {
		flags |= O_NONBLOCK;
		if(fcntl(sockCanfd, F_SETFL, flags) != -1){
			/* bind socket to the can0 interface */
			bind(sockCanfd, (struct sockaddr *)&addr, sizeof(addr));
			//Fix-me Correct status to be returned
		}else {
			ALOGE(TAG, __FUNCTION__, "Cannot Set Non-Block Socket");
			return false;
		}
	}else {
		ALOGE(TAG, __FUNCTION__, "Cannot Get Socket Flags");
		return false;
	}
	return true;
}

int CANDrv::CanRecvMsg(struct can_frame &RxCanMsg, unsigned long timeout)
{
	struct timeval tv;
	if(timeout > 1000) { // There's seconds in timeout
		tv.tv_sec = timeout % 1000;
	} else {
		tv.tv_sec = 0;
	}
	tv.tv_usec = (timeout * 1000) - (tv.tv_sec * 1000000);
	fd_set readSet;
	FD_ZERO(&readSet);
    FD_SET(sockCanfd, &readSet);
    
    int rc = select(sockCanfd + 1, &readSet, NULL, NULL, &tv);
    if (!rc) {
         return -2; //timeout error
    }else {
		if (FD_ISSET(sockCanfd, &readSet)) {
			if(read(sockCanfd, &RxCanMsg, sizeof(struct can_frame)) < 0) {
				return -1;
			} else{
				return (sizeof(struct can_frame));
			}
		}
	}
	return -1;
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
	int iRecError = 0;
	while(CanDrvInst->mCANStatus)
	{
		if((iRecError = CanDrvInst->CanRecvMsg(RxCanMsg, CanDrvInst->uiDefCANRecTimeout)) == -2){
			ALOGW(TAG, __FUNCTION__, "CAN Reception timeout");
		}else if (iRecError == -1){
			ALOGE(TAG, __FUNCTION__, "CAN Reception Error");
		}else {
			//CanDrvInst->printCanFrame(RxCanMsg);
			//Put in appropriate CAN FIFO for later processing
			//in upper layers: J1939, SmartCraft, NMEA2000, KWP2k, DiagOnCan
			//Should be done in Locked Context
			if(CanDrvInst->CANFifo->EnqueueMessage((void *)&RxCanMsg,8+RxCanMsg.can_dlc) == false) {
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
	ALOGD(TAG, __FUNCTION__, "ArbId = 0x%08X", TxCanMsg.can_id);
	ALOGD(TAG, __FUNCTION__, "Data Length Code = %d", (int)TxCanMsg.can_dlc);
	char BufferData[1024]={0};
	char FieldVal[100]={0};
	for(int j=0; j < (TxCanMsg.can_dlc-1); j++) {
		sprintf(FieldVal,"0x%02X-",(int)TxCanMsg.data[j]);
		strcat(BufferData,FieldVal);
	}
	sprintf(FieldVal,"0x%02X",(int)TxCanMsg.data[TxCanMsg.can_dlc-1]);
	strcat(BufferData,FieldVal);
	ALOGD(TAG, __FUNCTION__, "Data Buffer = %s", BufferData);
}

bool CANDrv::getCANStatus()
{
	return mCANStatus;
}

void CANDrv::StopCANDriver()
{
	setCANStatus(false);
}

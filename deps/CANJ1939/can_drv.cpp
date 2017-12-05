#include <iostream>
#include <fcntl.h> //include socket file control operations ioctl, fcntl ...
#include <sstream> //include stringstream
#include <iomanip> //include setw and setfill
#include <chrono> //include timestamp operations
#include "can/can_drv.h"
#include "logger/lib_logger.h"

using namespace std;
using namespace std::chrono;

#define TAG "CANDrv"

CANDrv::CANDrv(string FifoName, int proto):
mCANStatus(false),
sockCanfd(-1),
mulModeFlags(0),
mulBaudrate(static_cast<unsigned long>(250000)),
uiDefCANRecTimeout(CANDefaultTimeoutBase),
mCanProtocol(proto)
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
#ifdef CANDATALOGGER
		CanDataLogger = new DataFileLogger("CanDataLogger", "CanData.log");
#endif
	}
}

CANDrv::CANDrv(string FifoName, int proto, string CanInterface):
mCANStatus(false),
sockCanfd(-1),
mulModeFlags(0),
mulBaudrate(static_cast<unsigned long>(250000)),
uiDefCANRecTimeout(CANDefaultTimeoutBase),
mCanProtocol(proto)
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
#ifdef CANDATALOGGER
		CanDataLogger = new DataFileLogger("CanDataLogger", "CanData.log");
#endif
	}
}

CANDrv::CANDrv(string FifoName, int proto, string CanInterface, unsigned long baudrate):
mCANStatus(false),
sockCanfd(-1),
mulModeFlags(0),
mulBaudrate(baudrate),
uiDefCANRecTimeout(CANDefaultTimeoutBase),
mCanProtocol(proto)
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
#ifdef CANDATALOGGER
		CanDataLogger = new DataFileLogger("CanDataLogger", "CanData.log");
#endif
	}
}

CANDrv::CANDrv(string FifoName, int proto, string CanInterface, unsigned long baudrate, unsigned long ModeFlags):
mCANStatus(false),
sockCanfd(-1),
mulModeFlags(ModeFlags),
mulBaudrate(baudrate),
uiDefCANRecTimeout(CANDefaultTimeoutBase),
mCanProtocol(proto)
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
#ifdef CANDATALOGGER
		CanDataLogger = new DataFileLogger("CanDataLogger", "CanData.log");
#endif
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
#ifdef CANDATALOGGER
	delete CanDataLogger;
#endif
	iCANDrvInit = CeCanDrv_NotInit;
}

bool CANDrv::initCanDevice(string CanInfName)
{
	ALOGD(TAG, __FUNCTION__, "Init Can Interface %s", CanInfName.c_str());
	struct ifreq ifr;
	struct sockaddr_can addr;
	memset(&ifr, 0x0, sizeof(ifr));
	memset(&addr, 0x0, sizeof(addr));
	/* open CAN_RAW socket */
	sockCanfd = socket(PF_CAN, SOCK_DGRAM, mCanProtocol);
	if(sockCanfd <=0) {
		ALOGE(TAG, __FUNCTION__, "Fail to create RAW Socket");
		return false;
	}
	
	strcpy(ifr.ifr_name, CanInfName.c_str());
	if(ioctl(sockCanfd, SIOCGIFINDEX, &ifr) < 0){
		ALOGE(TAG, __FUNCTION__, "Cannot Retrieve Interface Index");
		return false;
	}
	
	/* setup address for bind */
	addr.can_ifindex = ifr.ifr_ifindex;
	addr.can_family  = AF_CAN;
	
	/* Set CAN Socket to Non-Blocking
	 * So we can interrupt the read routing */
	int flags = fcntl(sockCanfd, F_GETFL, 0);
	if(flags != -1) {
		flags |= O_NONBLOCK;
		if(fcntl(sockCanfd, F_SETFL, flags) != -1){
			ALOGD(TAG, __FUNCTION__, "Non-Block Set Successfully");
		}else {
			ALOGE(TAG, __FUNCTION__, "Cannot Set Non-Block Socket");
			return false;
		}
	}else {
		ALOGE(TAG, __FUNCTION__, "Cannot Get Socket Flags");
		return false;
	}
	
	/* bind socket to the can0 interface */
	bind(sockCanfd, (struct sockaddr *)&addr, sizeof(addr));
	
	memset(&ifr, 0, sizeof(ifr));
	strncpy(ifr.ifr_name, CanInfName.c_str(), strlen(CanInfName.c_str()));
	//Get Interface Status
	if (ioctl(sockCanfd, SIOCGIFINDEX, &ifr) < 0) {
		return 0;
	}
	
	//Put CAN Interface UP
	if (!(ifr.ifr_flags & IFF_UP)) {
		ALOGW(TAG, __FUNCTION__, "Interface %s is Down", CanInfName.c_str());
		ALOGW(TAG, __FUNCTION__, "Set Interface %s Up", CanInfName.c_str());
		ifr.ifr_flags |= (IFF_UP | IFF_RUNNING);
		if (ioctl(sockCanfd, SIOCSIFFLAGS, &ifr) < 0) {
			ALOGE(TAG, __FUNCTION__, "Cannot Set Interface Up");
			return false;
		}
	}
	return true;
}

int CANDrv::CanRecvMsg(struct can_frame &RxCanMsg)
{
	if(read(sockCanfd, &RxCanMsg, sizeof(struct can_frame)) < 0) {
		return -1;
	} else{
		return (sizeof(struct can_frame));
	}
	return -1;
}

bool CANDrv::CanSendMsg(struct can_frame &TxCanMsg)
{
#ifdef CANDATALOGGER
	LogCanMsgToFile(TxCanMsg, CeCanDir_TX);
#endif
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
	CanMsgTstamp RxCanTstmpMsg;
	struct timespec CanRecvTimer;
	CanRecvTimer.tv_sec = 0;
	CanRecvTimer.tv_nsec = 1000000;
	while(CanDrvInst->mCANStatus)
	{
		int iRecError = 0;
		if((iRecError = CanDrvInst->CanRecvMsg(RxCanMsg)) < 0){
			//ALOGE(TAG, __FUNCTION__, "Error Receiving CAN messages");
			;
		}else {
			//ALOGD(TAG, __FUNCTION__, "New Message received");
			//CanDrvInst->printCanFrame(RxCanMsg);
			//Put in appropriate CAN FIFO for later processing
			//in upper layers: J1939, SmartCraft, NMEA2000, KWP2k, DiagOnCan
			//Should be done in Locked Context
#ifdef CANDATALOGGER
			CanDrvInst->LogCanMsgToFile(RxCanMsg, CeCanDir_RX);
#endif
			RxCanTstmpMsg.RxCanMsg = RxCanMsg;
			RxCanTstmpMsg.ulMsgTstamp = CanDrvInst->getCANMsgTimestamp();
			if(CanDrvInst->CANFifo->EnqueueMessage((void *)&RxCanTstmpMsg,sizeof(unsigned long long)+8+RxCanMsg.can_dlc) == false) {
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
	char BufferData[1024]={0};
	char FieldVal[100]={0};
	ALOGD(TAG, __FUNCTION__, "ArbId = 0x%08X", TxCanMsg.can_id);
	ALOGD(TAG, __FUNCTION__, "Data Length Code = %d", (int)TxCanMsg.can_dlc);
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

/*bool CANDrv::setCanFilters(struct can_filter * AppliedFilters, unsigned int size)
{
    if(setsockopt(sockCanfd, SOL_CAN_RAW, CAN_RAW_FILTER, &AppliedFilters, sizeof(struct can_filter)*size) == 0){
		return true;
	}else {
		ALOGD(TAG, __FUNCTION__, "Fail To apply CAN Filters");
		return false;
	}
}*/
void CANDrv::LogCanMsgToFile(struct can_frame CanMsg, CeCanMsgDir MsgDir)
{
	string BufferData;
	std::stringstream CanString;
	CanString << getCANMsgTimestamp();
	CanString << "\t";
	if(MsgDir == CeCanDir_TX) {
		CanString << "TX";
	}else {
		CanString << "RX";
	}
	CanString << "\t";
	
	if((CanMsg.can_id & CAN_SFF_MASK)== CanMsg.can_id) { //CAN Standard Frame <= 0x7FF
		CanString << "STD";
	}else if (((CanMsg.can_id & CAN_EFF_MASK) == CanMsg.can_id) &&
			  ((CanMsg.can_id & CAN_SFF_MASK) != CanMsg.can_id)){ //CAN Extended Frame <= 0x1FFFFFFF & > 0x7FF
		CanString << "EXT";
	}else if((CanMsg.can_id & CAN_RTR_FLAG) == CanMsg.can_id) {
		CanString << "RTR"; //CAN Remote Transmission Request
	}else { //CAN Error Frame
		CanString << "ERR";
	}
	CanString << "\t";
	
	
	CanString << "0x" << std::setfill ('0') << std::setw(sizeof(unsigned long))
	          << std::hex << CanMsg.can_id;
	CanString << "\t";
	CanString << (int)CanMsg.can_dlc;
	CanString << "\t";
	for(int j=0; j < (CanMsg.can_dlc-1); j++) {
		CanString << "0x" << std::setfill ('0') << std::setw(sizeof(unsigned char)*2)
		          << std::hex << (int)CanMsg.data[j];
		CanString << " - ";
	}
	CanString << "0x" << std::setfill ('0') << std::setw(sizeof(unsigned char)*2)
		      << std::hex << (int)CanMsg.data[CanMsg.can_dlc-1];
	BufferData = CanString.str();
	CanDataLogger->WriteLogData(BufferData, true);
}

unsigned long long CANDrv::getCANMsgTimestamp()
{
	return duration_cast<std::chrono::milliseconds>(system_clock::now().time_since_epoch()).count();
}


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

CANDrv::CANDrv(string FifoName, int proto, string CanInterface, unsigned long baudrate, unsigned long ModeFlags):
mCanDriverStatus(false),
mCANStatus(false),
sockCanfd(-1),
mulModeFlags(ModeFlags),
mulBaudrate(baudrate),
uiDefCANRecTimeout(CANDefaultTimeoutBase),
mCanProtocol(proto),
mDiagRXID(0),
mDiagTXID(0)
{
	ALOGD(TAG, __FUNCTION__, "CTOR");
	mCanDriverStatus = CheckKernelModule();
	if(mCanDriverStatus) {
		if(!initCanDevice(CanInterface)) {
			ALOGE(TAG, __FUNCTION__, "Fail to init CAN Interface");
			setCANStatus(false);
		}else {
			iCANDrvInit = CeCanDrv_Init;
			setCANStatus(true);
			CANFifo = new Fifo(CANFIFODepth, FifoName.c_str());
			pthread_create(&Can_Thread, NULL, pvthCanReadRoutine_Exe, this);
#ifdef CANDATALOGGER
			CanDataLogger = new DataFileLogger(FifoName, FifoName+".log", false);
#endif
		}
	}
}

CANDrv::CANDrv(std::string FifoName, int proto, std::string CanInterface, unsigned long baudrate, unsigned long ModeFlags, unsigned long ulRxID,
	   unsigned long ulTxID):
mCanDriverStatus(false),
mCANStatus(false),
sockCanfd(-1),
mulModeFlags(ModeFlags),
mulBaudrate(baudrate),
uiDefCANRecTimeout(0),
mCanProtocol(proto),
mDiagRXID(ulRxID),
mDiagTXID(ulTxID)
{
	ALOGD(TAG, __FUNCTION__, "CTOR");
	mCanDriverStatus = CheckKernelModule();
	if(mCanDriverStatus) {
		if(!initCanDevice(CanInterface)) {
			ALOGE(TAG, __FUNCTION__, "Fail to init CAN Interface");
			setCANStatus(false);
		}else {
			iCANDrvInit = CeCanDrv_Init;
			setCANStatus(true);
			CANFifo = new Fifo(CANFIFODepth, FifoName.c_str());
			pthread_create(&Can_Thread, NULL, pvthIsotpReadRoutine_Exe, this);
#ifdef CANDATALOGGER
			CanDataLogger = new DataFileLogger(FifoName, FifoName+".log", false);
#endif
		}
	}
}

CANDrv::~CANDrv()
{
	ALOGD(TAG, __FUNCTION__, "DTOR");
	//shutdown associated CAN socket
	if(mCanDriverStatus && mCANStatus) {
		if(sockCanfd > 0) {
			close(sockCanfd);
			sockCanfd = -1;
		}
		//join Reception Thread
		pthread_join(Can_Thread, NULL);
		//delete CAN Reception FIFO
		delete CANFifo;
#ifdef CANDATALOGGER
		delete CanDataLogger;
#endif
		iCANDrvInit = CeCanDrv_NotInit;
	}
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
		ALOGE(TAG, __FUNCTION__, "Fail to create %d Socket", mCanProtocol);
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
	
	/*If ISOTP Protocol Define RX/TX Ids */
	if(mCanProtocol == CAN_ISOTP) {
		addr.can_addr.tp.tx_id = mDiagRXID;
		addr.can_addr.tp.rx_id = mDiagTXID;
	}
	
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
	
	/* bind socket to the CAN interface */
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

int CANDrv::CanRecvMsg(unsigned char * DataBuff)
{
	return read(sockCanfd, DataBuff, ISOTP_BUFSIZE);
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

bool CANDrv::CanSendMsg(struct can_frame TxCanMsg)
{
#ifdef CANDATALOGGER
	LogCanMsgToFile(TxCanMsg, CeCanDir_TX);
#endif
	if(write(sockCanfd, (void*)&TxCanMsg, sizeof(struct can_frame)) >= 0){
		ALOGD(TAG, __FUNCTION__, "CAN Msg sent successfully");
		return true;
	}else{
		ALOGE(TAG, __FUNCTION__, "Fail to send CAN Msg");
		return false;
	}
}

bool CANDrv::CanSendMsg(const unsigned char * Buf, unsigned long ulLen)
{
	if(ulLen > ISOTP_BUFSIZE)
		ulLen = ISOTP_BUFSIZE;
#ifdef CANDATALOGGER
	LogCanMsgToFile(Buf, ulLen, CeCanDir_TX);
#endif
	if(write(sockCanfd, Buf, ulLen) >= 0){
		ALOGD(TAG, __FUNCTION__, "CAN Msg sent successfully");
		return true;
	}else{
		ALOGE(TAG, __FUNCTION__, "Fail to send CAN Msg");
		return false;
	}
}

//ISOTP RX Handler Routine
void * CANDrv::pvthIsotpReadRoutine_Exe (void* context)
{
	CANDrv * CanDrvInst = static_cast<CANDrv *>(context);
	struct timespec CanRecvTimer;
	CanRecvTimer.tv_sec = 0;
	CanRecvTimer.tv_nsec = 1000000;
	unsigned char PDUData[ISOTP_BUFSIZE]={0};
	while(CanDrvInst->mCANStatus)
	{
		int nRecvBytes = CanDrvInst->CanRecvMsg(PDUData);
		if(nRecvBytes <= 0){
			//ALOGE(TAG, __FUNCTION__, "Error Receiving ISOTP PDU messages");
			;
		}else {
			ALOGD(TAG, __FUNCTION__, "New Message received");
			CanDrvInst->printCanFrame(PDUData, nRecvBytes);
			//Put in appropriate CAN FIFO for later processing
			//in upper layers: J1939, SmartCraft, NMEA2000, KWP2k, DiagOnCan
			//Should be done in Locked Context
#ifdef CANDATALOGGER
			CanDrvInst->LogCanMsgToFile(PDUData, nRecvBytes, CeCanDir_RX);
#endif
			if(CanDrvInst->CANFifo->EnqueueMessage((void *)PDUData, nRecvBytes) == false) {
				ALOGE(TAG, __FUNCTION__, "Fail to queue CAN message");
			}
		}
	    //unlock Context
		nanosleep(&CanRecvTimer, NULL);
	}
	pthread_exit(NULL);
}

//RAW, J1939, NMEA2000, SmCrft Protocols RX Handler Routine
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
		if(CanDrvInst->CanRecvMsg(RxCanMsg) < 0){
			//ALOGE(TAG, __FUNCTION__, "Error Receiving CAN messages");
			;
		}else {
			ALOGD(TAG, __FUNCTION__, "New Message received");
			CanDrvInst->printCanFrame(RxCanMsg);
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
	char FieldVal[10]={0};
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

void CANDrv::printCanFrame(const unsigned char * Buf, unsigned long ulLen)
{
	char BufferData[1024]={0};
	char FieldVal[10]={0};
	ALOGD(TAG, __FUNCTION__, "PDU ID = 0x%08X", mDiagRXID);
	ALOGD(TAG, __FUNCTION__, "PDU Length Code = %d", ulLen);
	for(unsigned int j=0; j < (ulLen-1); j++) {
		sprintf(FieldVal,"0x%02X-",(int)Buf[j]);
		strcat(BufferData,FieldVal);
	}
	sprintf(FieldVal,"0x%02X",(int)Buf[ulLen-1]);
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

void CANDrv::LogCanMsgToFile(const unsigned char * Msg, unsigned long ulLen, CeCanMsgDir MsgDir)
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
	
	CanString << "ISOTP";
	
	CanString << "\t";
	
	
	CanString << "0x" << std::setfill ('0') << std::setw(sizeof(unsigned long))
			  << std::hex << mDiagRXID;
	CanString << "\t";
	CanString << ulLen;
	CanString << "\t";
	for(unsigned int j=0; j < (ulLen-1); j++) {
		CanString << "0x" << std::setfill ('0') << std::setw(sizeof(unsigned char)*2)
				  << std::hex << (int)Msg[j];
		CanString << " - ";
	}
	CanString << "0x" << std::setfill ('0') << std::setw(sizeof(unsigned char)*2)
			  << std::hex << (int)Msg[ulLen-1];
	BufferData = CanString.str();
	CanDataLogger->WriteLogData(BufferData, true);
}

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
	
	if(mCanProtocol == CAN_J1939_PROTO) {
		CanString << "J1939";
	}else if(mCanProtocol == CAN_RAW) {
		CanString << "RAW";
	}else {
		CanString << "Monitor";
	}
	CanString << "\t";

	if((CanMsg.can_id & CAN_SFF_MASK) == CanMsg.can_id) { //CAN Standard Frame <= 0x7FF
		CanString << "STD";
	} else if ((CanMsg.can_id & CAN_EFF_FLAG) == CAN_EFF_FLAG){ //CAN Extended Frame <= 0x1FFFFFFF & > 0x7FF
		CanString << "EXT";
	}
	CanString << "\t";
	if((CanMsg.can_id & CAN_RTR_FLAG) == CAN_RTR_FLAG) {
		CanString << "RTR"; //CAN Remote Transmission Request
	}
	CanString << "\t";
	if ((CanMsg.can_id & CAN_ERR_FLAG) == CAN_ERR_FLAG){ //CAN Error Frame
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

bool CANDrv::CheckKernelModule()
{
	FILE *fd = popen("lsmod |grep -niw can", "r");//w option is for exact word matching
	char buf[60]={0};
	bool status = false;
	if (fread (buf, 1, sizeof (buf), fd) > 0){ //can core kernel module is loaded
		status = true;
	} else {//can core is not loaded or there are issues on loading it
		ALOGE(TAG, __FUNCTION__, "CAN Core Kernel module not loaded");
	}
	fclose(fd);
	return status;
}


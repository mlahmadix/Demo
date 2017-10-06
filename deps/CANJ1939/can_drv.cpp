#include <iostream>
#include "can/can_drv.h"

using namespace std;

CANDrv::CANDrv(std::string FifoName):
StopCanInterf(true),
mulModeFlags(0),
mulBaudrate(250000),
pucCanInfName("vcan0"),
pucCanFifoName((char*)FifoName.c_str())
{
	CANFifo = new Fifo(CANFIFODepth, (const char*)"Fifo-CAN");
	initCanDevice();
	StopCanInterf = false;
	pthread_create(&Can_Thread, NULL, pvthCanReadRoutine_Exe, this);
}

CANDrv::CANDrv(std::string FifoName, std::string CanInterface):
StopCanInterf(true),
mulModeFlags(0),
mulBaudrate(250000),
pucCanInfName((char*)CanInterface.c_str()),
pucCanFifoName((char*)FifoName.c_str())
{
	CANFifo = new Fifo(CANFIFODepth, (const char*)"Fifo-CAN");
	initCanDevice();
}

CANDrv::CANDrv(std::string FifoName, std::string CanInterface, unsigned long baudrate):
StopCanInterf(true),
mulModeFlags(0),
mulBaudrate(baudrate),
pucCanInfName((char*)CanInterface.c_str()),
pucCanFifoName((char*)FifoName.c_str())
{
	CANFifo = new Fifo(CANFIFODepth, (const char*)"Fifo-CAN");
	initCanDevice();
}

CANDrv::CANDrv(std::string FifoName, std::string CanInterface, unsigned long baudrate, unsigned long ModeFlags):
StopCanInterf(true),
mulModeFlags(ModeFlags),
mulBaudrate(baudrate),
pucCanInfName((char*)CanInterface.c_str()),
pucCanFifoName((char*)FifoName.c_str())
{
	CANFifo = new Fifo(CANFIFODepth, (const char*)"Fifo-CAN");
	initCanDevice();
}

CANDrv::~CANDrv()
{
	//shutdown associated CAN socket
	close(sockCanfd);
	//join Reception Thread
	pthread_join(Can_Thread, NULL);
	//delete CAN Reception FIFO
	delete CANFifo;
}

bool CANDrv::initCanDevice()
{
	struct ifreq ifr;
	struct sockaddr_can addr;
	memset(&ifr, 0x0, sizeof(ifr));
	memset(&addr, 0x0, sizeof(addr));
	/* open CAN_RAW socket */
	sockCanfd = socket(PF_CAN, SOCK_RAW, CAN_RAW);
	/* convert interface string "can0" into interface index */
	strcpy(ifr.ifr_name, pucCanInfName);
	ioctl(sockCanfd, SIOCGIFINDEX, &ifr);
	/* setup address for bind */
	addr.can_ifindex = ifr.ifr_ifindex;
	addr.can_family = PF_CAN;
	/* bind socket to the can0 interface */
	bind(sockCanfd, (struct sockaddr *)&addr, sizeof(addr));
}

int CANDrv::CanRecvMsg(struct can_frame &RxCanMsg)
{
	read(sockCanfd, &RxCanMsg, sizeof(struct can_frame));
	return (sizeof(struct can_frame));
}

bool CANDrv::CanSendMsg(struct can_frame TxCanMsg)
{
	if(write(sockCanfd, &TxCanMsg, sizeof(TxCanMsg)) == sizeof(TxCanMsg)){
		cout << "CAN Msg sent successfully" << endl;
		printCanFrame(TxCanMsg);
		return true;
	}else{
		cout << "Fail to send CAN Msg" << endl;
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
	int retval = 0;
	while(!CanDrvInst->StopCanInterf)
	{
		if(CanDrvInst->CanRecvMsg(RxCanMsg) < 0){
			CanDrvInst->StopCanInterf = true;
			retval = 1;
		}else {
			CanDrvInst->printCanFrame(RxCanMsg);
			//Put in appropriate CAN FIFO for later processing
			//in upper layers: J1939, SmartCraft, NMEA2000, KWP2k, DiagOnCan
			//Should be done in Locked Context
			if(CanDrvInst->CANFifo->EnqueueMessage((void *)&RxCanMsg,8+RxCanMsg.can_dlc) == true) {
				cout << "CAN Message queued successfully" << endl;
			} else {
				cout << "Fail to queue CAN message" << endl;
			}
		}
	    //unlock Context
		nanosleep(&CanRecvTimer, NULL);
	}
	pthread_exit(&retval);
}

bool CANDrv::printCanFrame(struct can_frame TxCanMsg)
{
	cout << "ArbId = 0x" << std::hex << TxCanMsg.can_id << endl;
	cout << "Data Length Code = " << (int)TxCanMsg.can_dlc << endl;
	for(int j=0; j < TxCanMsg.can_dlc; j++) {
		cout << "Data index : " << j << " "
			 << "value : 0x" << std::hex <<(int)TxCanMsg.data[j] << endl;
	}
}

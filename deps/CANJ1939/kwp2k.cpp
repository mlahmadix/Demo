#include <iostream>
#include <chrono> //include timestamp operations
#include "can/kwp2k.h"
#include "logger/lib_logger.h"

#define TAG "KWP2k"

kwp2k::kwp2k(std::string CanFifoName, std::string CanInfName):
CANDrv(CanFifoName, CAN_ISOTP, CanInfName, static_cast<unsigned long>(500000), 0,
       static_cast<unsigned long>(ISOTP_ECU_ID), static_cast<unsigned long>(ISOTP_TOOL_ID)),
mkwp2k_Init(false)
{
	ALOGD(TAG, __FUNCTION__, "CTOR");
	if(CheckKernelModule()) {
		if(getCANStatus()) {
			mkwp2k_Init = true;
			pthread_create(&mkwp2kThread, NULL, pvthKwp2kParseFrames_Exe, this);
		}
	}
}
kwp2k::~kwp2k()
{
	ALOGD(TAG, __FUNCTION__, "DTOR");
	if(mkwp2k_Init) {
		pthread_join(mkwp2kThread, NULL);
		mkwp2k_Init = false;
	}
}
bool kwp2k::Sendkwp2kFrame(unsigned char * Data, unsigned long ulLen)
{
	return CanSendMsg((void*)Data, ulLen);
}

void * kwp2k::pvthKwp2kParseFrames_Exe (void* context)
{
	kwp2k * kwp2kInst = static_cast<kwp2k *>(context);
	struct timespec Kwp2kBaseTimer;
	Kwp2kBaseTimer.tv_sec = 1;
	Kwp2kBaseTimer.tv_nsec = 0;//1s for test Thread scheduling
	while(kwp2kInst->getCANStatus()) {
		ALOGE(TAG, __FUNCTION__, "CAN ISOTP Scheduling Thread");
		nanosleep(&Kwp2kBaseTimer, NULL);
	}
	return NULL;
}
bool kwp2k::CheckKernelModule()
{
	FILE *fd = popen("lsmod |grep -niw can_isotp", "r");//w option is for exact word matching
	char buf[60]={0};
	bool status = false;
	if (fread (buf, 1, sizeof (buf), fd) > 0){ //CAN ISOTP kernel module is loaded
		status = true;
	} else {//CAN ISOTP module is not loaded or there are issues on loading it
		ALOGE(TAG, __FUNCTION__, "CAN ISOTP Kernel module not loaded");
	}
	fclose(fd);
	return status;
}

void kwp2k::ForceStopCAN()
{
	if(mkwp2k_Init && getCANStatus())
		StopCANDriver();
}

bool kwp2k::setCanFilters(struct can_filter * AppliedFilters, unsigned int size)
{
	return true;
}

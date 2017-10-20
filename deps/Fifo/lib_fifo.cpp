#include <iostream>
#include <string.h>
#include <stdlib.h>
#include "fifo/lib_fifo.h"
#include "logger/lib_logger.h"

using namespace std;
#define TAG "FiFoDrv"

Fifo::Fifo(unsigned int uiSize, const char * pucName):
miTail(-1),
miHead(-1),
muiSize(uiSize),
muiCount(0),
miIndex(-1),
mFifoName(pucName)
{
	ALOGD(TAG, __FUNCTION__, "FiFo class CTOR");
	ALOGI(TAG, __FUNCTION__, "CFIFO Lib Version: %s", FIFO_VERSION);
	ALOGD(TAG, __FUNCTION__, "FIFO Name = %s", mFifoName);
	if(muiSize > FifoDefaultSize)
		muiSize = FifoDefaultSize;
	pmFifoRAM = (struct FifoBuff*)malloc(muiSize*sizeof(struct FifoBuff));
}

Fifo::~Fifo()
{
	ALOGD(TAG, __FUNCTION__, "FiFo class DTOR");
	for(unsigned int i = 0; i < muiCount; i++){
		free(pmFifoRAM[i].pucBuf);
	}
	muiSize = 0;
	miTail = miHead = -1;
	muiCount = 0;
	free(pmFifoRAM);
}

bool Fifo::EnqueueMessage(void * pucBuff, unsigned int uiSize)
{
	mFifoMutex.lock();
	if(isFifoFull()) {
		ALOGE(TAG, __FUNCTION__, "Fifo is Full");
		mFifoMutex.unlock();
		return false;
	}
	miIndex++;
	pmFifoRAM[miIndex].pucBuf = (void*)malloc(uiSize);
	if(pmFifoRAM[miIndex].pucBuf != NULL) {
		memcpy(pmFifoRAM[miIndex].pucBuf, pucBuff, uiSize);
		pmFifoRAM[miIndex].iBufSize = uiSize;
		muiCount++;
		mFifoMutex.unlock();
		return true;
	} else {
		ALOGE(TAG, __FUNCTION__, "No enough System RAM space");
		miIndex--;
		mFifoMutex.unlock();
		return false;
	}
}

bool Fifo::DequeueMessage(void * pucBuff, unsigned int * uiSize)
{
	mFifoMutex.lock();
	if(isFifoEmpty()) {
		ALOGE(TAG, __FUNCTION__, "Fifo is Empty");
		mFifoMutex.unlock();
		return false;
	}
	memcpy(pucBuff, pmFifoRAM[miIndex].pucBuf, pmFifoRAM[miIndex].iBufSize);
	*uiSize = pmFifoRAM[miIndex].iBufSize;
	free(pmFifoRAM[miIndex].pucBuf);
	miIndex--;
	muiCount--;
	mFifoMutex.unlock();
	return true;
}

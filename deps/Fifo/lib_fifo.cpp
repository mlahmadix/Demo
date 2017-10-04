#include <iostream>
#include <string.h>
#include <stdlib.h>
#include "fifo/lib_fifo.h"

using namespace std;

Fifo::Fifo(unsigned int uiSize, const char * pucName):
miTail(-1),
miHead(-1),
muiSize(uiSize),
muiCount(0),
miIndex(-1)
{
	cout << "Ctor FiFo class " << endl;
	cout << "FIFO Lib Version: " << FIFO_VERSION << endl;
	memset(mFifoName,0, FifoNameSize);
	memcpy(mFifoName, pucName, strlen(pucName));
	cout << "FIFO Name: " << mFifoName << endl;
	if(muiSize > FifoDefaultSize)
		muiSize = FifoDefaultSize;
	pmFifoRAM = (struct FifoBuff*)malloc(muiSize*sizeof(struct FifoBuff));
}

Fifo::~Fifo()
{
	cout << "Dtor FiFo class " << endl;
	for(int i = 0; i < muiCount; i++){
		free(pmFifoRAM[i].pucBuf);
	}
	muiSize = 0;
	miTail = miHead = -1;
	muiCount = 0;
	free(pmFifoRAM);
}

bool Fifo::EnqueueMessage(void * pucBuff, unsigned int uiSize)
{
	cout << __FUNCTION__ << endl;
	if(isFifoFull()) {
		cout << __FUNCTION__ << mFifoName
		<< ": Fifo is Full" << endl;
		return false;
	}
	miIndex++;
	pmFifoRAM[miIndex].pucBuf = (void*)malloc(uiSize);
	if(pmFifoRAM[miIndex].pucBuf != NULL) {
		memcpy(pmFifoRAM[miIndex].pucBuf, pucBuff, uiSize);
		pmFifoRAM[miIndex].iBufSize = uiSize;
		muiCount++;
		return true;
	} else {
		cout << "No enough System RAM space" << endl;
		miIndex--;
		return false;
	}
}

bool Fifo::DequeueMessage(void * pucBuff, unsigned int * uiSize)
{
	cout << __FUNCTION__ << endl;
	if(isFifoEmpty()) {
		cout << __FUNCTION__ << mFifoName
		<< ": Fifo is Empty" << endl;
		return false;
	}
	memcpy(pucBuff, pmFifoRAM[miIndex].pucBuf, pmFifoRAM[miIndex].iBufSize);
	*uiSize = pmFifoRAM[miIndex].iBufSize;
	free(pmFifoRAM[miIndex].pucBuf);
	miIndex--;
	muiCount--;
	return true;
}

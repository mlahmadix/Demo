#ifndef __LIB_FIFO_H__
#define __LIB_FIFO_H__

#include "fifo/LibFifoConfig.h"

class Fifo {
	
	public:
		struct FifoBuff {
			 void * pucBuf;
			 unsigned int iBufSize;
		 };
	     Fifo(unsigned int uiSize, std::string pucName);
	     ~Fifo();
	     bool EnqueueMessage(void * pucBuff, unsigned int uiSize);
	     bool DequeueMessage(void * pucBuff, unsigned int * uiSize);

	private:
		#define FifoDefaultSize 100
		#define FifoNameSize 50
		 struct FifoBuff * pmFifoRAM;
		 int miTail;
		 int miHead;
		 unsigned int muiSize;
		 unsigned int muiCount;
		 int miIndex;
		 std::string mFifoName;
		 bool isFifoEmpty() { return (!muiCount)? true:false; }
	     bool isFifoFull() { return (muiCount == muiSize)? true:false; }
	     int CurrFifoIndex() { return muiCount;}
	     int CurrFifoSize() { return muiSize;}

};

#endif

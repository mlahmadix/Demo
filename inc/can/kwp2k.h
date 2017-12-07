#ifndef __KWP2K_H__
#define __KWP2K_H__

#include "can/can_drv.h"

class kwp2k: public CANDrv
{
	public:
		kwp2k(std::string CanFifoName, std::string CanInfName);
		~kwp2k();
		void ForceStopCAN();
		virtual bool setCanFilters(struct can_filter * AppliedFilters, unsigned int size);
	
	private:
		bool Sendkwp2kFrame(unsigned char * Data, unsigned long ulLen);
		bool mkwp2k_Init;
		pthread_t mkwp2kThread;
		static void * pvthKwp2kParseFrames_Exe (void* context);
		virtual bool CheckKernelModule();
		#define ISOTP_ECU_ID  0x763 //RX ID
		#define ISOTP_TOOL_ID 0x743 //TX ID
};
#endif

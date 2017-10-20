#ifndef __J1939_H__
#define __J1939_H__

#include "can/can_drv.h"

class J1939Layer
{
	public:
		J1939Layer(std::string CanFifoName);
		~J1939Layer();
		bool SendJ1939Msg(struct can_frame &TxCanMsg);
	
	private:
		CANDrv * CanInfDrv;
		pthread_t J1939Thread;
		static void * pvthJ1939ParseFrames_Exe (void* context);
};

#endif

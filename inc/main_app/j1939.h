#ifndef __J1939_H__
#define __J1939_H__

#include "can/can_drv.h"

#define J1939_BaudRate 250000
class J1939Layer: public CANDrv
{
	public:
		J1939Layer(std::string CanFifoName, std::string CanInfName);
		~J1939Layer();
		bool SendJ1939Msg(struct can_frame &TxCanMsg);
		void ForceStopCAN();
	
	private:
		pthread_t J1939Thread;
		static void * pvthJ1939ParseFrames_Exe (void* context);
};

#endif

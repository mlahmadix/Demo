#include <iostream>
#include <memory>
#include <errno.h>
#include "logger/lib_logger.h"
#include "dynamic/lib_dynamic.h"
#include "can/can_drv.h"
#include "sharedMem/lib_sharedMem.h"
#include "signal/lib_signal.h"
#include "inout/lib_bits.h"

using namespace std;
using namespace boost::interprocess;

#define TAG "Main"

#define duiTotalFifoElem 100

struct can_frame TxCanMsg;

struct ProgramPosition{
	char name[100];
	int pid;
	double latitude;
	double longitude;
};

bool bIgnitionSet = false;

void SignalHandler(int signo)
{
	if(signo == SIGINT){
		ALOGI(TAG, __FUNCTION__, "Interrupt Signal catched");
		bIgnitionSet = true;
	}
}

int main(){

      ALOGI(TAG, __FUNCTION__, "Main Program Init");
      int dyn = iLog_eCreateFile_Exe("HelloLog.txt");
      ALOGI(TAG, __FUNCTION__, "Dynamic return %d", dyn);

      //initialize a Signal Catcher for SIGINT
      std::shared_ptr<SignalApi> InterruptSignal(new SignalApi(SIGINT,SignalHandler));
      
      //initialize GPIO1 Register Value
      unsigned long ulGPIO1 = 0x00000000; //MockRegister to be used as Real H/W register
      std::shared_ptr<RegBits> RegGPIO1(new RegBits((unsigned long)&ulGPIO1));

	  RegGPIO1->SetBitFieldValue(4, 5, 0x15); //write 0x00000015 in Register
	  ALOGD(TAG, __FUNCTION__, "RegBitValue = 0x%08X", RegGPIO1->getBitFieldValue (4, 5));
	  RegGPIO1->SetBitFieldValue(10, 5, 0x15); //write 0x00005400
	  ALOGD(TAG, __FUNCTION__, "RegBitValue = 0x%08X", RegGPIO1->getBitFieldValue (10, 5));
	  RegGPIO1->ResetBitFieldValue(4, 5); //write "00000" in bit 4,5,6,7,8
	  ALOGD(TAG, __FUNCTION__, "RegBitValue = 0x%08X", RegGPIO1->dumpRegValue()); //Result should be 0x00005400
	  
      //Initialize CAN Interface
      std::shared_ptr<CANDrv> CanInfDrv(new CANDrv("CANFIFO-VCan0"));
      TxCanMsg.can_id = 0x0CFEF100;
      TxCanMsg.can_dlc = 8;
      strcpy((char*)TxCanMsg.data, "ABCDEFGH");
      CanInfDrv->CanSendMsg(TxCanMsg);
      
	  //Shared memory testing Structure
	  struct ProgramPosition TestPosWr, TestPosRd;
	  memset(TestPosWr.name, 0, 100);
	  strcpy(TestPosWr.name ,"SharedMemoryProgram\0");
	  TestPosWr.pid = getpid();
	  TestPosWr.latitude = 10.11223344;
	  TestPosWr.longitude = 36.11223344;
	  ALOGD(TAG, __FUNCTION__, "Write Struct Name = %s", TestPosWr.name);
	  ALOGD(TAG, __FUNCTION__, "Write Struct Pid = %d", TestPosWr.pid);
	  ALOGD(TAG, __FUNCTION__, "Write Struct Latitude = %.4f", TestPosWr.latitude);
	  ALOGD(TAG, __FUNCTION__, "Write Struct Longitude = %.4f", TestPosWr.longitude);
	  
	  //Create a new Shm
      std::shared_ptr<Shared_Memory> Shm(new Shared_Memory("shmNew1", 1024));
      //write in shm
      Shm->sharedMemoryWrite((void *)&TestPosWr, 0, sizeof(TestPosWr));
      
      //read from shm
      int Rdsize = Shm->sharedMemoryRead(&TestPosRd, 0, sizeof(TestPosRd));
      ALOGD(TAG, __FUNCTION__, "Read SHM Data Amount = %d", Rdsize);
      ALOGD(TAG, __FUNCTION__, "Read Struct Name = %s", TestPosRd.name);
      ALOGD(TAG, __FUNCTION__, "Read Struct Pid = %d", TestPosRd.pid);
      ALOGD(TAG, __FUNCTION__, "Read Struct Latitude = %.4f", TestPosRd.latitude);
      ALOGD(TAG, __FUNCTION__, "Read Struct Longitude = %.4f", TestPosRd.longitude);
      
      while(bIgnitionSet == false);
      CanInfDrv->StopCANDriver();
      sleep(2);
      ALOGI(TAG, __FUNCTION__, "Good Bye");
      return 0;
}

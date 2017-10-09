#include <iostream>
#include "logger/lib_logger.h"
#include "dynamic/lib_dynamic.h"
#include "can/can_drv.h"
#include "sharedMem/lib_sharedMem.h"

using namespace std;
using namespace boost::interprocess;

#define TAG "Main"

#define duiTotalFifoElem 100

struct can_frame TxMsg;
struct can_frame RxMsg;
unsigned int uiRxMsgDlc;

struct ProgramPosition{
	char name[100];
	int pid;
	double latitude;
	double longitude;
};

int main(){

      ALOGI(TAG, "Main Program Init");
      int dyn = iLog_eCreateFile_Exe("HelloLog.txt");
      ALOGI(TAG, "Dynamic return %d", dyn);
      //Initialize CAN Interface
      CANDrv * CanInfDrv = new CANDrv("CANFIFO-VCan0");
      struct can_frame TxCanMsg;
      TxCanMsg.can_id = 0x0CFEF100;
      TxCanMsg.can_dlc = 8;
      strcpy((char*)TxCanMsg.data, "ABCDEFGH");
      CanInfDrv->CanSendMsg(TxCanMsg);
      
	  //Shared memory testing Structure
	  struct ProgramPosition TestPosWr, TestPosRd;
	  memset(TestPosWr.name, 0, 100);
	  strcpy(&TestPosWr.name[0], "SharedMemoryProgram\0");
	  TestPosWr.pid = getpid();
	  TestPosWr.latitude = 10.11223344;
	  TestPosWr.longitude = 36.11223344;
	  ALOGD(TAG, "Write Struct Name = %s", (char*)TestPosWr.name);
	  ALOGD(TAG, "Write Struct Pid = %d", (int)TestPosWr.pid);
	  ALOGD(TAG, "Write Struct Latitude = %.4f", (double)TestPosWr.latitude);
	  ALOGD(TAG, "Write Struct Longitude = %.4f", (double)TestPosWr.longitude);
	  
	  //Create a new Shm
      Shared_Memory * Shm = new Shared_Memory((char*)"shmNew1", 1024);
      
      //write in shm
      Shm->sharedMemoryWrite((void *)&TestPosWr, 0, sizeof(TestPosWr));
      
      //read from shm
      int Rdsize = Shm->sharedMemoryRead(&TestPosRd, 0, sizeof(TestPosRd));
      ALOGD(TAG, "Read SHM Data Amount = %d", Rdsize);
      ALOGD(TAG, "Read Struct Name = %s", TestPosRd.name);
      ALOGD(TAG, "Read Struct Pid = %d", TestPosRd.pid);
      ALOGD(TAG, "Read Struct Latitude = %.4f", TestPosRd.latitude);
      ALOGD(TAG, "Read Struct Longitude = %.4f", TestPosRd.longitude);
      
      
      while(1);
      delete CanInfDrv;
      ALOGI(TAG, "Good Bye");
      return 0;
}

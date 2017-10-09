#include <iostream>
#include "logger/lib_logger.h"
#include "dynamic/lib_dynamic.h"
#include "can/can_drv.h"
#include "sharedMem/lib_sharedMem.h"

using namespace std;
using namespace boost::interprocess;

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

struct ProgramPosition TestPosWr, TestPosRd;

int main(){
      cout << "This is my new Software" << endl;
      int dyn = ALOGX((char*)"MAIN", 3, (char*)"Dynamic ALOGX Function called");
      cout << "Dynamic Return: " << dyn << endl;
      int arc = iLog_eCreateFile_Exe("HelloLog.txt");
      cout << "Archive Return: " << arc << endl;
      //Initialize CAN Interface
      CANDrv * CanInfDrv = new CANDrv("CANFIFO-VCan0");
      struct can_frame TxCanMsg;
      TxCanMsg.can_id = 0x0CFEF100;
      TxCanMsg.can_dlc = 8;
      strcpy((char*)TxCanMsg.data, "ABCDEFGH");
      CanInfDrv->CanSendMsg(TxCanMsg);
      
	  //Shared memory testing Structure
	  memset(TestPosWr.name, 0, 100);
	  strcpy(&TestPosWr.name[0], "SharedMemoryProgram\0");
	  TestPosWr.pid = getpid();
	  TestPosWr.latitude = 10.11223344;
	  TestPosWr.longitude = 36.11223344;
	  
      cout << "Write Struct Name = " << (char*)TestPosWr.name << endl;
      cout << "Write Struct Pid = " << std::dec << (int)TestPosWr.pid << endl;
      cout << "Write Struct Latitude = " << (double)TestPosWr.latitude << endl;
      cout << "Write Struct Longitude = " << (double)TestPosWr.longitude << endl;
	  
	  //Create a new Shm
      Shared_Memory * Shm = new Shared_Memory("shmNew1", 1024);
      
      //write in shm
      Shm->sharedMemoryWrite((void *)&TestPosWr, 0, sizeof(TestPosWr));
      
      //read from shm
      int Rdsize = Shm->sharedMemoryRead(&TestPosRd, 0, sizeof(TestPosRd));
      cout << "Read SHM Data Amount = " << std::dec << Rdsize << endl;
      cout << "Read Struct Name = " << TestPosRd.name << endl;
      cout << "Read Struct Pid = " << TestPosRd.pid << endl;
      cout << "Read Struct Latitude = " << TestPosRd.latitude << endl;
      cout << "Read Struct Longitude = " << TestPosRd.longitude << endl;
      
      
      while(1);
      delete CanInfDrv;
      cout << "Good Bye" << endl;
      return 0;
}

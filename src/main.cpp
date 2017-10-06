#include <iostream>
#include "archive/lib_archive.h"
#include "dynamic/lib_dynamic.h"
#include "can/can_drv.h"
#include "sharedMem/lib_sharedMem.h"

using namespace std;
using namespace boost::interprocess;

#define duiTotalFifoElem 100

struct can_frame TxMsg;
struct can_frame RxMsg;
unsigned int uiRxMsgDlc;

int main(){
      cout << "This is my new Software" << endl;
      int dyn = ALOGX((char*)"MAIN", 3, (char*)"Dynamic ALOGX Function called");
      cout << "Dynamic Return: " << dyn << endl;
      int arc = iArc_eCreateFile_Exe("HelloLog.txt");
      cout << "Archive Return: " << arc << endl;
      //Initialize CAN Interface
      CANDrv * CanInfDrv = new CANDrv("CANFIFO-VCan0");
      struct can_frame TxCanMsg;
      TxCanMsg.can_id = 0x0CFEF100;
      TxCanMsg.can_dlc = 8;
      strcpy((char*)TxCanMsg.data, "ABCDEFGH");
      CanInfDrv->CanSendMsg(TxCanMsg);
      //shared Memory
      Shared_Memory * Shm = new Shared_Memory(open_or_create, "shmName", 1024, "mysegment");
      //write in shm
      Shm->write_in_shared_memory(5);
      //read from shm
      cout << "SharedMemory value:" << Shm->read_from_sharedMemory() << endl;
      while(1);
      delete CanInfDrv;
      cout << "Good Bye" << endl;
      return 0;
}

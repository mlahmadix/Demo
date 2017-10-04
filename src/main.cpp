#include <iostream>
#include "archive/lib_archive.h"
#include "dynamic/lib_dynamic.h"
#include "can/can_drv.h"

using namespace std;

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
      delete CanInfDrv;
      cout << "Good Bye" << endl;
      return 0;
}

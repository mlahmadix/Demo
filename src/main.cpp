#include <iostream>
#include <memory>
#include <errno.h>
#include "logger/lib_logger.h"
#include "sharedMem/lib_sharedMem.h"
#include "signal/lib_signal.h"
#include "inout/InOutApp.h"
#include "can/kwp2k.h"
#include "can/j1939.h"
#include "nmeaparser/NmeaParser.h"
#include "main_app/eeprom.h"

using namespace std;
using namespace boost::interprocess;

#define TAG "Main"

#define duiTotalFifoElem 100

struct ProgramPosition{
	char name[100];
	int pid;
	double latitude;
	double longitude;
};

//****************************************************************************//
//Definitions of Engine Source Addresses
//****************************************************************************//
#define PROP_SA  0x21
#define ENG_SA   0x00
#define TRANS_SA 0x03
#define BRAKE_SA 0x0B

//****************************************************************************//
//Definition of used received PGNs
//****************************************************************************//
#define EEC1_PGN		0xF004 //SA : 0x00 Engine : Motorisation
#define CCVS_PGN		0xFEF1
#define ENG_TEMP1_PGN	0xFEEE
#define ENG_FLD_PGN		0xFEEF
#define ENG_VOL_PGN	    0xFEEA

//****************************************************************************//
//J1939 Parameters
//****************************************************************************//
unsigned short usEngineSpeed = 0; //Engine RPM in Revolution Per Minute
unsigned short usVehicleSpeed = 0;//Vehicle Speed in Km/h
signed char    scCoolTemp = 0;//Engine Coolant Temperature
unsigned short  usOilPres = 0;//Engine Oil Pressure
unsigned short  usVolLvl = 0;//Engine Volume Level

enum J1939DataStatus {
	CeJ1939_EngSpeed_Status = 0,
	CeJ1939_VehSpeed_Status,
	CeJ1939_CoolTemp_Status,
	CeJ1939_OilPress_Status,
	CeJ1939_VolLevel_Status,
	CeJ1939_MaxParams_Status
};
bool J1939DataStats[CeJ1939_MaxParams_Status] = {false};
//****************************************************************************//
//J1939 Received Messages
//****************************************************************************//

/* Simple Testing Method using Linux can-utils command line cansend for each message
 * Engine Speed (2500 RPM)    : cansend vcan0 06F00400#FF.FF.FF.20.4E.FF.FF.FF
 * Vehicle Speed (130 Km/h)   : cansend vcan0 06FEF100#FF.00.82.FF.FF.FF.FF.FF
 * Engine Cool. Temp (-25°C)  : cansend vcan0 06FEEE00#0F.FF.FF.FF.FF.FF.FF.FF
 * Engine Oil Press. (500mbar): cansend vcan0 06FEEF00#FF.FF.FF.7D.FF.FF.FF.FF
 * Volume Level (50%)         : cansend vcan0 06FEEA00#FF.FF.FF.FF.FF.FF.FF.19
 */

static const J1939_eRxDataInfo CstP1939_iRxDataDef[5] =
{
 {EEC1_PGN,		 ENG_SA,  3, 2,  1, 8  ,   0 ,    0,    12000  ,5000,  &usEngineSpeed  , &J1939DataStats[CeJ1939_EngSpeed_Status]},
 {CCVS_PGN,		 ENG_SA,  1, 2,  1, 256,   0 ,    0,    250    ,5000,  &usVehicleSpeed , &J1939DataStats[CeJ1939_VehSpeed_Status]},
 {ENG_TEMP1_PGN, ENG_SA,  0, 1,  1, 1  ,  -40,  -40,    210    ,5000,  &scCoolTemp     , &J1939DataStats[CeJ1939_CoolTemp_Status]},
 {ENG_FLD_PGN,	 ENG_SA,  3, 1,  4, 1  ,   0 ,    0,    1000   ,5000,  &usOilPres      , &J1939DataStats[CeJ1939_OilPress_Status]},
 {ENG_VOL_PGN,	 ENG_SA,  7, 1,  4, 2  ,   0 ,    0,    1000   ,5000,  &usVolLvl       , &J1939DataStats[CeJ1939_VolLevel_Status]},
};

static const stDM_iDTCDataStruct CstP1939_iSuppDtcMsg[5] =
{
    {0x17,      520211,      1,         DM_MatchFMI1orFMI2orFMI3,       5,      6,     15,     DM_MalfunctionLamp},    //WiFi Module Failure  
    {0x17,      520212,      1,         DM_DontFMICare,                 0,      0,      0,     DM_MalfunctionLamp},    //GSM Module Failure 
    {0x17,      520213,      1,         DM_MatchFMI1,                   5,      0,      0,     DM_MalfunctionLamp},    //GPS Module Failure  
    {0x17,      520214,      1,         DM_MatchFMI1orFMI2,             5,      6,      0,     DM_AmberWarningLamp},    //NetWork not available  
    {0x17,      520215,      1,         DM_MatchFMI1orFMI2orFMI3,       5,      6,     15,     DM_RedStopLamp}     ,    //Ignition Set OFF 
};

unsigned long ulBuildCanId(unsigned char ucSA, unsigned short usPGN)
{
	unsigned char ucDefPrio = 6;
	return static_cast<unsigned long>(static_cast<unsigned long>(ucDefPrio << 24) +
									  static_cast<unsigned long>(usPGN << 8) +
									  static_cast<unsigned long>(ucSA));
}


bool bIgnitionSet = false;

void SignalHandler(int signo)
{
	if(signo == SIGINT){
		ALOGI(TAG, __FUNCTION__, "Interrupt Signal catched");
		bIgnitionSet = true;
	}
}

int main(){

	  initTCPLoggerServer();
	  ALOGI(TAG, __FUNCTION__, "Main Program Init");
	  struct timespec MainDataDisplayTimer;
      //initialize a Signal Catcher for SIGINT
      std::shared_ptr<SignalApi> InterruptSignal(new SignalApi(SIGINT,SignalHandler));
      /* To create a Binary File with Fixed size use following command:
       * sudo dd if=/dev/zero of=./MainE2p.bin bs=16 count=64
       * sector size should be a multiply of 16
       * Total size = bs*count
       * /dev/zero is used to set all bytes to 0x00
       */
	  std::shared_ptr<eeprom> E2PConfigData(new eeprom("./MainE2p.bin", 1024)); //1KB-E2PROM Data
	  char * Buffer = new char [1024];
	  for (int i = 0; i < 1024; i++)
		Buffer[i] = (char)((0x30 +i)&0xFF);
	  E2PConfigData->eeprom_write(Buffer, 0, 1024);
	  int iDumpStat = 0;
	  if(!(iDumpStat = E2PConfigData->eeprom_CloneToFile("E2promClone.bin"))) {
		  ALOGD(TAG, __FUNCTION__, "E2P Dump Successfully");
	  } else {
		  ALOGE(TAG, __FUNCTION__, "Fail to Dump E2P = %d", iDumpStat);
	  }
	  
	  delete [] Buffer;
	  /* You can verify manually content of E2PROM dump buffer
	   * using the hexedit utility
       */
      std::shared_ptr<InOutApp> InOutBank1(new InOutApp());

	  InOutBank1->SetOutputOn(InOutBank1->CeEnum_RightSignOut);
	  std::shared_ptr<kwp2k> kwp2kApp(new kwp2k("vcan0")); 
      
      std::shared_ptr<J1939Layer> J1939LayerApp(new J1939Layer("vcan0", CstP1939_iRxDataDef, 
      sizeof(CstP1939_iRxDataDef)/sizeof(CstP1939_iRxDataDef[0])));

      //Example of Extended CAN Message sending
      static struct can_frame TxCanMsg;
      TxCanMsg.can_id = ulBuildCanId(0x00, 0xFEF1);
      TxCanMsg.can_dlc = 8;
      for(int j = 0; j < TxCanMsg.can_dlc; j++)
    	  TxCanMsg.data[j] = 0x30 + j;
      J1939LayerApp->SendJ1939Msg(TxCanMsg);
      
      MainDataDisplayTimer.tv_sec = 0;
	  MainDataDisplayTimer.tv_nsec = 300000000;
	  nanosleep(&MainDataDisplayTimer, NULL);
      
      //Example of Standard CAN Message sending
      TxCanMsg.can_id = 0x743;
      TxCanMsg.can_dlc = 8;
      for(int k = 0; k < TxCanMsg.can_dlc; k++)
    	  TxCanMsg.data[k] = 0x30 + k;
      J1939LayerApp->SendJ1939Msg(TxCanMsg);
      
      std::shared_ptr<NmeaParser> NmeaGpsP(new NmeaParser("/dev/tty0", 115200));
      
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
      
      
      MainDataDisplayTimer.tv_sec = 1;
	  MainDataDisplayTimer.tv_nsec = 300000000;
      while(bIgnitionSet == false){
		  if(J1939DataStats[CeJ1939_EngSpeed_Status] == true){
			ALOGD(TAG, __FUNCTION__, "usEngineSpeed  = %d %s", usEngineSpeed, "RPM");
		  }else {
			  ALOGD(TAG, __FUNCTION__, "usEngineSpeed  = %s %s", "-----", "RPM");
		  }
		  if(J1939DataStats[CeJ1939_VehSpeed_Status] == true){
			ALOGD(TAG, __FUNCTION__, "usVehicleSpeed = %d %s", usVehicleSpeed, "Kmh");
	      }else {
			  ALOGD(TAG, __FUNCTION__, "usVehicleSpeed = %s %s", "---", "Kmh");
		  }
		  if(J1939DataStats[CeJ1939_CoolTemp_Status] == true){
			ALOGD(TAG, __FUNCTION__, "scCoolTemp     = %d %s", scCoolTemp, "°C");
		  }else {
			  ALOGD(TAG, __FUNCTION__, "scCoolTemp     = %s %s", "----", "°C");
		  }
		  if(J1939DataStats[CeJ1939_OilPress_Status] == true){
			ALOGD(TAG, __FUNCTION__, "ucOilPres      = %d %s", usOilPres, "mBar");
		  }else {
			  ALOGD(TAG, __FUNCTION__, "ucOilPres      = %s %s", "----", "mBar");
		  }
		  if(J1939DataStats[CeJ1939_VolLevel_Status] == true){
			ALOGD(TAG, __FUNCTION__, "usVolLvl      = %d %s", usVolLvl, "%");
		  }else {
			  ALOGD(TAG, __FUNCTION__, "usVolLvl      = %s %s", "----", "%");
		  }
		  nanosleep(&MainDataDisplayTimer, NULL);
	  }
      J1939LayerApp->ForceStopCAN();
      kwp2kApp->ForceStopCAN();
      StopTCPLoggerServer();
      //delete [] J1939Filters;
      sleep(2);
      ALOGI(TAG, __FUNCTION__, "Good Bye");
      return 0;
}

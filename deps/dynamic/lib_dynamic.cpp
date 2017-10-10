#include <iostream>
#include <fstream>
#include <stdio.h>
#include "dynamic/lib_dynamic.h"
#include "logger/lib_logger.h"

using namespace std;
#define TAG "MapFile"

int iLog_eCreateFile_Exe(const char * cpucFilePath)
{
   int iRet = CeFile_Idle;
   ALOGI(TAG, __FUNCTION__, "Library Version = %s",  DYNAMIC_VERSION);
   #ifdef FILECREAT
     fstream LoggingFile(cpucFilePath, ios::in | ios::out | ios::app);
     if(LoggingFile.is_open()){ //file is correctly opened
       LoggingFile << "This is an Example of Logging File" << endl;
       LoggingFile.close();
       iRet = CeFile_LoggingSuccess;
     } else {
	   ALOGE(TAG, __FUNCTION__, "Couldn't Create Logging File \n \
	          Please check file path and persmissions",  DYNAMIC_VERSION);
       iRet = CeFile_LoggingFailed;
     }
   #else
		ALOGE(TAG, __FUNCTION__, "File Creation is not supported");
        iRet = CeFile_LoggingDisabled;
   #endif
   return iRet;
	
}

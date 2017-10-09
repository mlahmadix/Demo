#include <iostream>
#include <stdio.h>
#include "dynamic/lib_dynamic.h"

using namespace std;

int iLog_eCreateFile_Exe(const char * cpucFilePath)
{
   int iRet = CeFile_Idle;
   cout << "Library Version: " << DYNAMIC_VERSION << endl;
   #ifdef FILECREAT
     fstream LoggingFile(cpucFilePath, ios::in | ios::out | ios::app);
     if(LoggingFile.is_open()){ //file is correctly opened
       LoggingFile << "This is an Example of Logging File" << endl;
       LoggingFile.close();
       iRet = CeFile_LoggingSuccess;
     } else {
       cout << "Couldn't Create Logging File" << endl;
       cout << "Please check file path and persmissions" << endl;
       iRet = CeFile_LoggingFailed;
     }
   #else
        cout << "File Creation is not supported" << endl;
        iRet = CeFile_LoggingDisabled;
   #endif
   return iRet;
	
}

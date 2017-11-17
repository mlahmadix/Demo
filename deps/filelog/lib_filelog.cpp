#include <iostream>
#include "filelog/lib_filelog.h"
#include "logger/lib_logger.h"

using namespace std;

DataFileLogger::DataFileLogger(string FileLoggerComp, string cpucFilePath):
mFileLoggingStat(CeFile_Idle),
mFileLoggingTag(FileLoggerComp)
{
   ALOGD(mFileLoggingTag, __FUNCTION__, "CTOR");
   ALOGI(mFileLoggingTag, __FUNCTION__, "DataFileLogger Version = %s",  FILE_VERSION);
     mLogFileHndle.open(cpucFilePath.c_str(), ios::out | ios::app);
     if(mLogFileHndle.is_open()){
       mFileLoggingStat = CeFile_LoggingSuccess;
     } else {
	   ALOGE(mFileLoggingTag, __FUNCTION__, "Couldn't Create Logging File \n \
	          Please check file path and persmissions");
       mFileLoggingStat = CeFile_LoggingFailed;
     }
}

DataFileLogger::~DataFileLogger()
{
	ALOGD(mFileLoggingTag, __FUNCTION__, "CTOR");
	if(mFileLoggingStat == CeFile_LoggingSuccess){
		mLogFileHndle.close();
		mFileLoggingStat = CeFile_Idle;
	}
}

int DataFileLogger::WriteLogData(std::string LoggingMessage, bool retLine)
{
	int iRet = 0;
	if(mFileLoggingStat == CeFile_LoggingSuccess) {
		mLogFileHndle << LoggingMessage;
		if(retLine)
			mLogFileHndle << endl;
	}else {
		iRet = -2; //bad file Desc
	}
	return iRet;
}

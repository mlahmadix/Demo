#include <iostream>
#include "filelog/lib_filelog.h"
#include "logger/lib_logger.h"

using namespace std;

DataFileLogger::DataFileLogger(string FileLoggerComp, string cpucFilePath, bool bErase):
mFileLoggingStat(CeFile_Idle),
mFileLoggingTag(FileLoggerComp)
{
	std::ios_base::openmode flags;
	ALOGD(mFileLoggingTag, __FUNCTION__, "CTOR");
	ALOGI(mFileLoggingTag, __FUNCTION__, "DataFileLogger Version = %s",  FILE_VERSION);
	if(FileExists(cpucFilePath)){//Logging File already exists
		if(bErase){//Erase or not File Data
			flags = ios::out | ios::trunc;
		}else {
			flags = ios::out | ios::app;
		}
	}else {
		flags = ios::out | ios::trunc;
	}
	mLogFileHndle.open(cpucFilePath.c_str(), flags);
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

inline bool DataFileLogger::FileExists(const std::string& name)
{
    ifstream f(name.c_str());
    return f.good();
}

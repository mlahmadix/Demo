#ifndef __LIB_FILELOG_H__
#define __LIB_FILELOG_H__

#include "filelog/LibFileConfig.h"
#include <fstream>

enum CeFileLogStatus{
    CeFile_Idle = -1,
    CeFile_LoggingSuccess = 0, 
    CeFile_LoggingDisabled,
    CeFile_LoggingFailed,
    CeFile_UndefinedState
};

class DataFileLogger
{
	public:
		DataFileLogger(std::string FileLoggerComp, std::string cpucFilePath, bool bErase);
		~DataFileLogger();
		inline CeFileLogStatus getFileLoggingStat() { return mFileLoggingStat; }
		int WriteLogData(std::string LoggingMessage, bool retLine);
	private:
		CeFileLogStatus mFileLoggingStat;
		std::string mFileLoggingTag;
		std::ofstream mLogFileHndle;
		inline bool FileExists(const std::string& name);

};
#endif

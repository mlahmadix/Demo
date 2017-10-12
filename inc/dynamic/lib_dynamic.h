#ifndef __LIB_HEADERS_H__
#define __LIB_HEADERS_H__

#include "dynamic/LibDynConfig.h"

enum {
    CeFile_Idle = -1,
    CeFile_LoggingSuccess = 0, 
    CeFile_LoggingDisabled,
    CeFile_LoggingFailed,
    CeFile_UndefinedState
};

int iLog_eCreateFile_Exe(const char * cpucFilePath);

#endif

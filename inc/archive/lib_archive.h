#ifndef __LIB_ARCHIVE_H__
#define __LIB_ARCHIVE_H__

#include "LibArchConfig.h"

int iArc_eCreateFile_Exe(const char * cpucFilePath);

enum {
    CeFile_Idle = -1,
    CeFile_LoggingSuccess = 0, 
    CeFile_LoggingDisabled,
    CeFile_LoggingFailed,
    CeFile_UndefinedState
};

#endif

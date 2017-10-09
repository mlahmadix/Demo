#include <iostream>
#include <cstdio>
#include <stdarg.h>
#include "logger/lib_logger.h"

using namespace std;

enum priority {
	CeLoggerDebug = 0,
	CeLoggerInfo,
	CeLoggerWarn,
	CeLoggerError,
	CeMaxPriorities
};

const char * TabPrio[CeMaxPriorities] ={
	"Dbg",
	"Inf",
	"Wrn",
	"Err"
};

template <priority p>
void ALOGX(char* tag, char* message, va_list vargs)
{
	if ( p <= CeLoggerDebug ){
		#ifndef LOGDEBUG
			cout << "Logging Feature is not enabled" << endl;
			return;
		#endif
	}
	char buffer[1024]={0};
	char BufferCore[1024]={0};
	va_list vlist;
	va_copy(vlist, vargs);
	vsprintf(BufferCore, message, vlist);
    sprintf(buffer,"%s\t%s\t%s",tag, TabPrio[p], BufferCore);
    cout << buffer << endl;
    va_end(vlist);
}

//Generic Logging Function based on Message Priority
void ALOGE(char* tag, char* message, ...)
{
	va_list ap;
    va_start(ap, message);
	ALOGX<CeLoggerError>(tag, message, ap);
	va_end(ap);
}
void ALOGW(char* tag, char* message, ...)
{
	va_list ap;
    va_start(ap, message);
	ALOGX<CeLoggerWarn>(tag, message, ap);
	va_end(ap);
}
void ALOGI(char* tag, char* message, ...)
{
	va_list ap;
    va_start(ap, message);
	ALOGX<CeLoggerInfo>(tag, message, ap);
	va_end(ap);
}
void ALOGD(char* tag, char* message, ...)
{
	va_list ap;
    va_start(ap, message);
	ALOGX<CeLoggerDebug>(tag, message, ap);
	va_end(ap);
}

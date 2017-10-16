#include <iostream>
#include <cstdio>
#include <stdarg.h>
#include <boost/date_time/posix_time/posix_time.hpp>
#include "logger/lib_logger.h"

using namespace std;
namespace pt = boost::posix_time;

#define KNRM  "\x1B[0m"
#define KRED  "\x1B[31m"
#define KGRN  "\x1B[32m"
#define KYEL  "\x1B[33m"
#define KBLU  "\x1B[34m"
#define KMAG  "\x1B[35m"
#define KCYN  "\x1B[36m"
#define KWHT  "\x1B[37m"

enum priority {
	CeLoggerDebug = 0,
	CeLoggerInfo,
	CeLoggerWarn,
	CeLoggerError,
	CeMaxPriorities
};

const char * TabPrio[CeMaxPriorities] ={
	"DBG",
	"INF",
	"WRN",
	"ERR"
};

const char * PrioColours[CeMaxPriorities]={
	KWHT,
	KGRN,
	KYEL,
	KRED
};

void ALOG_GetTimestamp(std::string &timestamp)
{
	pt::ptime now = pt::second_clock::local_time();
	std::stringstream ss;
	ss << static_cast<int>(now.date().month()) << "/" << now.date().day()
	   << "/" << now.date().year() << "-"
	   << now.time_of_day().hours() << ":" << now.time_of_day().minutes()
	   << ":" << now.time_of_day().seconds();
	timestamp = ss.str();
}

template <priority p>
void ALOGX(std::string tag, std::string func, std::string message, va_list vargs)
{
	if ( p <= CeLoggerDebug ){
		#ifndef LOGDEBUG
			return;
		#endif
	}
	char buffer[1024]={0};
	char BufferCore[1024]={0};
	std::string timestamp;
	va_list vlist;
	va_copy(vlist, vargs);
	ALOG_GetTimestamp(timestamp);
	vsprintf(BufferCore, message.c_str(), vlist);
    sprintf(buffer,"%s\t%s\t%s\t%s\t%s",timestamp.c_str(), tag.c_str(), func.c_str(), TabPrio[p], BufferCore);
    cout << PrioColours[p] << buffer << endl;
    va_end(vlist);
}

//Generic Logging Function based on Message Priority
void ALOGE(std::string tag, std::string func, std::string message, ...)
{
	va_list ap;
    va_start(ap, (char*)message.c_str());
	ALOGX<CeLoggerError>(tag, func, message, ap);
	va_end(ap);
}
void ALOGW(std::string tag, std::string func, std::string message, ...)
{
	va_list ap;
    va_start(ap, (char*)message.c_str());
	ALOGX<CeLoggerWarn>(tag, func, message, ap);
	va_end(ap);
}
void ALOGI(std::string tag, std::string func, std::string message, ...)
{
	va_list ap;
    va_start(ap, (char*)message.c_str());
	ALOGX<CeLoggerInfo>(tag, func, message, ap);
	va_end(ap);
}
void ALOGD(std::string tag, std::string func, std::string message, ...)
{
	va_list ap;
    va_start(ap, (char*)message.c_str());
	ALOGX<CeLoggerDebug>(tag, func, message, ap);
	va_end(ap);
}

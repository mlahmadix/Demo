#ifndef __LIB_LOGGER_H__
#define __LIB_LOGGER_H__

#include "LibLoggerConfig.h"

void ALOGE(std::string tag, std::string func, std::string message, ...);
void ALOGW(std::string tag, std::string func, std::string message, ...);
void ALOGI(std::string tag, std::string func, std::string message, ...);
void ALOGD(std::string tag, std::string func, std::string message, ...);

#endif

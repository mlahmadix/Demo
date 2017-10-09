#ifndef __LIB_LOGGER_H__
#define __LIB_LOGGER_H__

#include "LibLoggerConfig.h"

void ALOGE(char* tag, char* message, ...);
void ALOGW(char* tag, char* message, ...);
void ALOGI(char* tag, char* message, ...);
void ALOGD(char* tag, char* message, ...);

#endif

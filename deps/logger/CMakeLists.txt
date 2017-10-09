set (LOGGER_VERSION_MAJOR 0)
set (LOGGER_VERSION_MINOR 0)
set (LOGGER_VERSION_PATCH 777)
set (LOGGER_VERSION "${LOGGER_VERSION_MAJOR}.${LOGGER_VERSION_MINOR}.${LOGGER_VERSION_PATCH}")

configure_file (
  "../inc/logger/LibLoggerConfig.h.in"
  "../inc/logger/LibLoggerConfig.h"
  )

set (LOGGER_SRC logger/lib_logger.cpp)
set (LOGGER_HDR
     ../inc/logger/lib_logger.h
     ../inc/logger/LibLoggerConfig.h)

include_directories("../inc")

add_library(logger STATIC ${LOGGER_SRC} ${LOGGER_HDR})

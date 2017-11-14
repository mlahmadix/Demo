set (FILE_VERSION_MAJOR 0)
set (FILE_VERSION_MINOR 0)
set (FILE_VERSION_PATCH 999)
set (FILE_VERSION "${FILE_VERSION_MAJOR}.${FILE_VERSION_MINOR}.${FILE_VERSION_PATCH}")

#define Sources and Headers
set (FILE_SRC filelog/lib_filelog.cpp)
set (FILE_HDR
     ../inc/filelog/lib_filelog.h
     ../inc/filelog/LibFileConfig.h)

#include directories
include_directories("../inc")

#compile the shared library
add_library(filelog STATIC ${FILE_SRC} ${FILE_HDR})

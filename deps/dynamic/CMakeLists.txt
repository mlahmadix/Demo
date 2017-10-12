set (DYN_VERSION_MAJOR 0)
set (DYN_VERSION_MINOR 0)
set (DYN_VERSION_PATCH 999)
set (DYN_VERSION "${DYN_VERSION_MAJOR}.${DYN_VERSION_MINOR}.${DYN_VERSION_PATCH}")

#define Sources and Headers
set (DYN_SRC dynamic/lib_dynamic.cpp)
set (DYN_HDR
     ../inc/dynamic/lib_dynamic.h
     ../inc/dynamic/LibDynConfig.h)

#include directories
include_directories("../inc")

#compile the shared library
add_library(dynamic SHARED ${DYN_SRC} ${DYN_HDR})

#install the shared library
install(TARGETS dynamic DESTINATION ${CMAKE_BINARY_DIR}/exe)

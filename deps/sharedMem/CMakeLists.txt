set (ARCH_VERSION_MAJOR 0)
set (ARCH_VERSION_MINOR 0)
set (ARCH_VERSION_PATCH 777)
set (ARCH_VERSION "${ARCH_VERSION_MAJOR}.${ARCH_VERSION_MINOR}.${ARCH_VERSION_PATCH}")

configure_file (
  "../inc/archive/LibArchConfig.h.in"
  "../inc/archive/LibArchConfig.h"
  )

set (ARCH_SRC archive/lib_archive.cpp)
set (ARCH_HDR
     ../inc/archive/lib_archive.h
     ../inc/archive/LibArchConfig.h)

include_directories("../inc")

add_library(archive STATIC ${ARCH_SRC} ${ARCH_HDR})

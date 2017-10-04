set (FIFO_VERSION_MAJOR 0)
set (FIFO_VERSION_MINOR 0)
set (FIFO_VERSION_PATCH 999)
set (FIFO_VERSION "${FIFO_VERSION_MAJOR}.${FIFO_VERSION_MINOR}.${FIFO_VERSION_PATCH}")

configure_file (
  "../inc/fifo/LibFifoConfig.h.in"
  "../inc/fifo/LibFifoConfig.h"
  )

set (FIFO_SRC Fifo/lib_fifo.cpp)
set (FIFO_HDR
     ../inc/fifo/lib_fifo.h
     ../inc/fifo/LibFifoConfig.h)

include_directories("../inc")

add_library(fifo SHARED ${FIFO_SRC} ${FIFO_HDR})

#install the shared library
install(TARGETS fifo DESTINATION ${CMAKE_BINARY_DIR}/exe)

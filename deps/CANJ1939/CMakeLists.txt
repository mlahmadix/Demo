#define Sources and Headers
set (CAN_SRC CANJ1939/can_drv.cpp)
set (CAN_HDR
     ../inc/can/can_drv.h)
     
set (FIFO_LIB fifo)

#include directories
include_directories("../inc")

#compile the shared library
add_library(canj1939 SHARED ${CAN_SRC} ${CAN_HDR})

#link with FIFO static library
target_link_libraries(canj1939 ${FIFO_LIB})

#install the shared library
install(TARGETS canj1939 DESTINATION ${CMAKE_BINARY_DIR}/exe)

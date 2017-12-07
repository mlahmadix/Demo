#define Sources and Headers
set (CAN_SRC CANJ1939/can_drv.cpp
             CANJ1939/j1939.cpp
             CANJ1939/kwp2k.cpp)
set (CAN_HDR
     ../inc/can/can_drv.h
     ../inc/can/j1939.h
     ../inc/can/kwp2k.h)
     
set (FIFO_LIB fifo filelog)

#include directories
include_directories("../inc")

#compile the shared library
add_library(canj1939 STATIC ${CAN_SRC} ${CAN_HDR})

#link with FIFO static library
target_link_libraries(canj1939 ${FIFO_LIB} pthread)

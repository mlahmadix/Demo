set (INOUT_SRC inout/lib_bits.cpp
               inout/lib_io.cpp
               inout/InOut_app.cpp)
set (INOUT_HDR ../inc/inout/lib_bits.h
               ../inc/inout/lib_io.h
               ../inc/inout/InOutApp.h)

include_directories("../inc")

#compile the shared library
add_library(inout STATIC ${INOUT_SRC} ${INOUT_HDR})

set (SIG_SRC signal/lib_signal.cpp)
set (SIG_HDR ../inc/signal/lib_signal.h)

include_directories("../inc")

#compile the shared library
add_library(signal STATIC ${SIG_SRC} ${SIG_HDR})

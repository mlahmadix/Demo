set (SHM_VERSION_MAJOR 0)
set (SHM_VERSION_MINOR 0)
set (SHM_VERSION_PATCH 777)
set (SHM_VERSION "${SHM_VERSION_MAJOR}.${SHM_VERSION_MINOR}.${SHM_VERSION_PATCH}")

set (SHM_SRC sharedMem/lib_sharedMem.cpp)
set (SHM_HDR ../inc/sharedMem/lib_sharedMem.h)

include_directories("../inc")

#compile the shared library
add_library(sharedmem SHARED ${SHM_SRC} ${SHM_HDR})

#install the shared library
install(TARGETS sharedmem DESTINATION ${CMAKE_BINARY_DIR}/exe)

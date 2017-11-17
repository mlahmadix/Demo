set (IOUTIL_VERSION_MAJOR 0)
set (IOUTIL_VERSION_MINOR 0)
set (IOUTIL_VERSION_PATCH 1)
set (IOUTIL_VERSION "${IOUTIL_VERSION_MAJOR}.${IOUTIL_VERSION_MINOR}.${IOUTIL_VERSION_PATCH}")

set (IOUTIL_SRC IOUtils/UartUtils.cpp
				IOUtils/FileIo.cpp
				IOUtils/SocketIo.cpp)
set (IOUTIL_HDR ../inc/IOUtils/UartUtils.h
				../inc/IOUtils/FileIo.h
				../inc/IOUtils/SocketIo.h)

include_directories("../inc")

#compile the shared library
add_library(ioutils STATIC ${IOUTIL_SRC} ${IOUTIL_HDR})

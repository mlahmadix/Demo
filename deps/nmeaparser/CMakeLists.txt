set (NMEAP_VERSION_MAJOR 0)
set (NMEAP_VERSION_MINOR 0)
set (NMEAP_VERSION_PATCH 1)
set (NMEAP_VERSION "${NMEAP_VERSION_MAJOR}.${NMEAP_VERSION_MINOR}.${NMEAP_VERSION_PATCH}")

set (NMEAP_SRC nmeaparser/NmeaParser.cpp)
set (NMEAP_HDR ../inc/nmeaparser/NmeaParser.h
			   ../inc/nmeaparser/NmeaParserDefs.h
			   ../inc/IOUtils/UartUtils.h)

include_directories("../inc")

#compile the shared library
add_library(nmeaparser STATIC ${NMEAP_SRC} ${NMEAP_HDR})

#link with FIFO static library
target_link_libraries(nmeaparser ioutils)

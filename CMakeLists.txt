cmake_minimum_required(VERSION 2.6)
project (HelloWorld)

#define Product versions
set (HelloWorld_VERSION_MAJOR 0)
set (HelloWorld_VERSION_MINOR 0)
set (HelloWorld_VERSION_REVISION 1)
set (HelloWorld_VERSION_TAG 1)
set (HelloWorldVersion "${HelloWorld_VERSION_MAJOR}.${HelloWorld_VERSION_MINOR}.${HelloWorld_VERSION_REVISION}.${HelloWorld_VERSION_TAG}")

#define Project Tree directories
set (MAIN_INC
     inc/TutorialConfig.h
     inc/logger/lib_logger.h
     inc/filelog/lib_filelog.h
     inc/sharedMem/lib_sharedMem.h
     inc/signal/lib_signal.h
     inc/nmeaparser/NmeaParser.h
     inc/can/j1939.h
     inc/main_app/io_mapping.h
     inc/inout/InOutApp.h
     inc/main_app/eeprom.h)

#define Sources, dlls and libs
set (STA_LIB logger filelog canj1939 fifo sharedmem signal inout nmeaparser ioutils)
set (MAIN_SRC   src/main.cpp
				src/eeprom.cpp)

if(NMEAPDEBUG)
	#add definitions
	add_definitions (-DNMEAP_DEBUG)
endif()

if(CANDATALOGGER)
	#add definitions
	add_definitions (-DCANDATALOGGER)
endif()

if(LOGDEBUG)
	#add definitions
	add_definitions (-DLOGDEBUG)
endif()

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Werror -Wno-varargs")

if (CMAKE_BUILD_TYPE STREQUAL DEBUG)
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g -O3")
endif()

#Check if Boost is available on system
find_package(Boost 1.40.0 COMPONENTS date_time filesystem system)
if (NOT ${Boost_FOUND})
    message( FATAL_ERROR "libboost-all-dev was not found : ${Boost_NOT_FOUND_MESSAGE}" )
else ()
	include_directories(${Boost_INCLUDE_DIRS})
endif(NOT ${Boost_FOUND})

#add CMake components subdirectories
add_subdirectory(deps)
#include directory
include_directories("inc")

#compiling step
add_executable(HelloWorld ${MAIN_SRC} ${MAIN_INC} ${Boost_INCLUDE_DIRS})

#link step
target_link_libraries(HelloWorld ${STA_LIB} ${Boost_LIBRARIES} -lboost_system -lpthread -lrt)

enable_testing()

# creates the executable
add_executable(UnitTest auto_unit_test/MyTest.cpp)

# declares a test with our executable
add_test(NAME test1 COMMAND UnitTest)

#install step
install (TARGETS HelloWorld  DESTINATION ${CMAKE_BINARY_DIR}/exe)


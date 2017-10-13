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
     inc/dynamic/lib_dynamic.h
     inc/can/can_drv.h
     inc/sharedMem/lib_sharedMem.h)

#define Sources, dlls and libs
set (STA_LIB logger dynamic canj1939 fifo sharedmem)
set (MAIN_SRC
     src/main.cpp)

if(FILECREAT)
#add definitions
add_definitions (-DFILECREAT)
endif()

if(LOGDEBUG)
#add definitions
add_definitions (-DLOGDEBUG)
endif()

set_property(TARGET ${projectname} PROPERTY CXX_STANDARD 14)
set_property(TARGET ${projectname} PROPERTY CXX_EXTENSIONS OFF)
set_property(TARGET ${projectname} PROPERTY CXX_STANDARD_REQUIRED ON)

#Check if Boost is available on system
find_package(Boost 1.36.0)
if (NOT ${Boost_FOUND})
    message( FATAL_ERROR "libboost-dev was not found : ${Boost_NOT_FOUND_MESSAGE}" )
else ()
	include_directories(${Boost_INCLUDE_DIRS})
endif(NOT ${Boost_FOUND})

#add CMake components subdirectories
add_subdirectory(deps)
#include directory
include_directories("inc")

#include auto-generated directory
include_directories(${CMAKE_BUILD_DIRECTOR}/inc)


#compiling step
add_executable(HelloWorld ${MAIN_SRC} ${MAIN_INC} ${Boost_INCLUDE_DIRS})

#link step
target_link_libraries(HelloWorld ${STA_LIB} ${Boost_LIBRARIES} -lpthread -lrt)

enable_testing()

# creates the executable
add_executable(UnitTest auto_unit_test/MyTest.cpp)

# declares a test with our executable
add_test(NAME test1 COMMAND UnitTest)

#install step
install (TARGETS HelloWorld  DESTINATION ${CMAKE_BINARY_DIR}/exe)


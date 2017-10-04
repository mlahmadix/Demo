cmake_minimum_required(VERSION 2.6)
project (HelloWorld)

#define Product versions
set (HelloWorld_VERSION_MAJOR 0)
set (HelloWorld_VERSION_MINOR 0)
set (HelloWorld_VERSION_REVISION 1)
set (HelloWorld_VERSION_TAG 1)
set (HelloWorldVersion "${HelloWorld_VERSION_MAJOR}.${HelloWorld_VERSION_MINOR}.${HelloWorld_VERSION_REVISION}.${HelloWorld_VERSION_TAG}")

#version configuration file
configure_file (
  "inc/TutorialConfig.h.in"
  "inc/TutorialConfig.h")

#define Project Tree directories
set (MAIN_INC
     inc/TutorialConfig.h
     inc/archive/lib_archive.h
     inc/dynamic/lib_dynamic.h
     inc/can/can_drv.h)

#define Sources, dlls and libs
set (DYN_LIB dynamic canj1939 fifo)
set (STA_LIB archive)
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

#add CMake components subdirectories
add_subdirectory(deps)
#include directory
include_directories("inc")

#compiling step
add_executable(HelloWorld ${MAIN_SRC} ${MAIN_INC})

#link step
target_link_libraries(HelloWorld ${DYN_LIB} ${STA_LIB})

#install step
install (TARGETS HelloWorld DESTINATION ${CMAKE_BINARY_DIR}/exe)


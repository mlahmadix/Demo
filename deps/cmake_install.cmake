# Install script for directory: /home/mlahmadx/Desktop/cmake2/deps

# Set the install prefix
if(NOT DEFINED CMAKE_INSTALL_PREFIX)
  set(CMAKE_INSTALL_PREFIX "/usr/local")
endif()
string(REGEX REPLACE "/$" "" CMAKE_INSTALL_PREFIX "${CMAKE_INSTALL_PREFIX}")

# Set the install configuration name.
if(NOT DEFINED CMAKE_INSTALL_CONFIG_NAME)
  if(BUILD_TYPE)
    string(REGEX REPLACE "^[^A-Za-z0-9_]+" ""
           CMAKE_INSTALL_CONFIG_NAME "${BUILD_TYPE}")
  else()
    set(CMAKE_INSTALL_CONFIG_NAME "")
  endif()
  message(STATUS "Install configuration: \"${CMAKE_INSTALL_CONFIG_NAME}\"")
endif()

# Set the component getting installed.
if(NOT CMAKE_INSTALL_COMPONENT)
  if(COMPONENT)
    message(STATUS "Install component: \"${COMPONENT}\"")
    set(CMAKE_INSTALL_COMPONENT "${COMPONENT}")
  else()
    set(CMAKE_INSTALL_COMPONENT)
  endif()
endif()

# Install shared libraries without execute permission?
if(NOT DEFINED CMAKE_INSTALL_SO_NO_EXE)
  set(CMAKE_INSTALL_SO_NO_EXE "1")
endif()

if(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified")
  if(EXISTS "$ENV{DESTDIR}/home/mlahmadx/Desktop/cmake2/exe/libdynamic.so" AND
     NOT IS_SYMLINK "$ENV{DESTDIR}/home/mlahmadx/Desktop/cmake2/exe/libdynamic.so")
    file(RPATH_CHECK
         FILE "$ENV{DESTDIR}/home/mlahmadx/Desktop/cmake2/exe/libdynamic.so"
         RPATH "")
  endif()
  list(APPEND CMAKE_ABSOLUTE_DESTINATION_FILES
   "/home/mlahmadx/Desktop/cmake2/exe/libdynamic.so")
  if(CMAKE_WARN_ON_ABSOLUTE_INSTALL_DESTINATION)
    message(WARNING "ABSOLUTE path INSTALL DESTINATION : ${CMAKE_ABSOLUTE_DESTINATION_FILES}")
  endif()
  if(CMAKE_ERROR_ON_ABSOLUTE_INSTALL_DESTINATION)
    message(FATAL_ERROR "ABSOLUTE path INSTALL DESTINATION forbidden (by caller): ${CMAKE_ABSOLUTE_DESTINATION_FILES}")
  endif()
file(INSTALL DESTINATION "/home/mlahmadx/Desktop/cmake2/exe" TYPE SHARED_LIBRARY FILES "/home/mlahmadx/Desktop/cmake2/deps/libdynamic.so")
  if(EXISTS "$ENV{DESTDIR}/home/mlahmadx/Desktop/cmake2/exe/libdynamic.so" AND
     NOT IS_SYMLINK "$ENV{DESTDIR}/home/mlahmadx/Desktop/cmake2/exe/libdynamic.so")
    if(CMAKE_INSTALL_DO_STRIP)
      execute_process(COMMAND "/usr/bin/strip" "$ENV{DESTDIR}/home/mlahmadx/Desktop/cmake2/exe/libdynamic.so")
    endif()
  endif()
endif()

if(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified")
  if(EXISTS "$ENV{DESTDIR}/home/mlahmadx/Desktop/cmake2/exe/libfifo.so" AND
     NOT IS_SYMLINK "$ENV{DESTDIR}/home/mlahmadx/Desktop/cmake2/exe/libfifo.so")
    file(RPATH_CHECK
         FILE "$ENV{DESTDIR}/home/mlahmadx/Desktop/cmake2/exe/libfifo.so"
         RPATH "")
  endif()
  list(APPEND CMAKE_ABSOLUTE_DESTINATION_FILES
   "/home/mlahmadx/Desktop/cmake2/exe/libfifo.so")
  if(CMAKE_WARN_ON_ABSOLUTE_INSTALL_DESTINATION)
    message(WARNING "ABSOLUTE path INSTALL DESTINATION : ${CMAKE_ABSOLUTE_DESTINATION_FILES}")
  endif()
  if(CMAKE_ERROR_ON_ABSOLUTE_INSTALL_DESTINATION)
    message(FATAL_ERROR "ABSOLUTE path INSTALL DESTINATION forbidden (by caller): ${CMAKE_ABSOLUTE_DESTINATION_FILES}")
  endif()
file(INSTALL DESTINATION "/home/mlahmadx/Desktop/cmake2/exe" TYPE SHARED_LIBRARY FILES "/home/mlahmadx/Desktop/cmake2/deps/libfifo.so")
  if(EXISTS "$ENV{DESTDIR}/home/mlahmadx/Desktop/cmake2/exe/libfifo.so" AND
     NOT IS_SYMLINK "$ENV{DESTDIR}/home/mlahmadx/Desktop/cmake2/exe/libfifo.so")
    if(CMAKE_INSTALL_DO_STRIP)
      execute_process(COMMAND "/usr/bin/strip" "$ENV{DESTDIR}/home/mlahmadx/Desktop/cmake2/exe/libfifo.so")
    endif()
  endif()
endif()

if(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified")
  if(EXISTS "$ENV{DESTDIR}/home/mlahmadx/Desktop/cmake2/exe/libcanj1939.so" AND
     NOT IS_SYMLINK "$ENV{DESTDIR}/home/mlahmadx/Desktop/cmake2/exe/libcanj1939.so")
    file(RPATH_CHECK
         FILE "$ENV{DESTDIR}/home/mlahmadx/Desktop/cmake2/exe/libcanj1939.so"
         RPATH "")
  endif()
  list(APPEND CMAKE_ABSOLUTE_DESTINATION_FILES
   "/home/mlahmadx/Desktop/cmake2/exe/libcanj1939.so")
  if(CMAKE_WARN_ON_ABSOLUTE_INSTALL_DESTINATION)
    message(WARNING "ABSOLUTE path INSTALL DESTINATION : ${CMAKE_ABSOLUTE_DESTINATION_FILES}")
  endif()
  if(CMAKE_ERROR_ON_ABSOLUTE_INSTALL_DESTINATION)
    message(FATAL_ERROR "ABSOLUTE path INSTALL DESTINATION forbidden (by caller): ${CMAKE_ABSOLUTE_DESTINATION_FILES}")
  endif()
file(INSTALL DESTINATION "/home/mlahmadx/Desktop/cmake2/exe" TYPE SHARED_LIBRARY FILES "/home/mlahmadx/Desktop/cmake2/deps/libcanj1939.so")
  if(EXISTS "$ENV{DESTDIR}/home/mlahmadx/Desktop/cmake2/exe/libcanj1939.so" AND
     NOT IS_SYMLINK "$ENV{DESTDIR}/home/mlahmadx/Desktop/cmake2/exe/libcanj1939.so")
    file(RPATH_CHANGE
         FILE "$ENV{DESTDIR}/home/mlahmadx/Desktop/cmake2/exe/libcanj1939.so"
         OLD_RPATH "/home/mlahmadx/Desktop/cmake2/deps:"
         NEW_RPATH "")
    if(CMAKE_INSTALL_DO_STRIP)
      execute_process(COMMAND "/usr/bin/strip" "$ENV{DESTDIR}/home/mlahmadx/Desktop/cmake2/exe/libcanj1939.so")
    endif()
  endif()
endif()


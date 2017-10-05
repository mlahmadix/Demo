#!/bin/sh

echo "0%   ---- project build cleanup"
make clean
echo "20%  ---- cleanup cmake install files"
rm -rf */*/cmake_install.cmake */cmake_install.cmake cmake_install.cmake
rm -rf */*/install_manifest.txt */install_manifest.txt install_manifest.txt
echo "40%  ---- cleanup cmake cache files"
rm -rf */*/CMakeCache.txt */CMakeCache.txt CMakeCache.txt
echo "60%  ---- cmake files cleanup"
rm -rf */*/CMakeFiles */CMakeFiles CMakeFiles
echo "80%  ---- remove project generated files"
rm -rf */*/Makefile */Makefile Makefile
rm -rf */*/*Config*.h */*Config*.h
rm -rf exe
echo "100% ---- All project cleanup done"

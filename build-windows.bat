@echo off
setlocal enabledelayedexpansion

set CC=gcc.exe
set CXX=g++.exe

cmake -DCMAKE_BUILD_TYPE=Release -DCMAKE_C_COMPILER=%CC% -DCMAKE_CXX_COMPILER=%CXX% -S. -Bbuild -G "MinGW Makefiles"
cmake --build build --config Release --target all --
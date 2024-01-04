cmake_minimum_required(VERSION 3.21.1)
set(CMAKE_CXX_STANDARD 14)

project(prj)
add_executable(a src/main.cpp src/PID.cpp src/UAR.cpp)
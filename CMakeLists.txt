cmake_minimum_required(VERSION 3.21)
project(EX4)

set(CMAKE_CXX_STANDARD 11)

add_executable(EX4 SimpleTest.cpp PhysicalMemory.h PhysicalMemory.cpp VirtualMemory.h VirtualMemory.cpp)

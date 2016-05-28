cmake_minimum_required(VERSION 3.4)
project(Fibers)
enable_language(ASM)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
set(CMAKE_ASM_FLAGS "${CFLAGS} -x assembler-with-cpp")

set(SOURCE_FILES main.cpp fiber_switch.S)
add_executable(Fibers ${SOURCE_FILES})
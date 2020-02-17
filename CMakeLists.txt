cmake_minimum_required(VERSION 3.15)
project(passivity_project)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_SYSTEM_NAME Linux)
set(CMAKE_SYSTEM_PROCESSOR arm)

set(CMAKE_C_COMPILER arm-linux-gnueabihf-gcc)
set(CMAKE_CXX_COMPILER arm-linux-gnueabihf-g++)

set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_PACKAGE ONLY)

set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)

add_executable(passivity_project main.cpp CANopenCommunication.cpp CANopenCommunication.h CANopenNode.cpp CANopenNode.h NodePDO.cpp NodePDO.h CANopenTypes.h utils.h PDOpart.cpp PDOpart.h)

target_link_libraries(passivity_project Threads::Threads)

cmake_minimum_required(VERSION 3.15)
project(passivity_project)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -O0")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O0")

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_SYSTEM_NAME Linux)
#set(CMAKE_SYSTEM_PROCESSOR arm)
#
#set(CMAKE_C_COMPILER arm-linux-gnueabihf-gcc)
#set(CMAKE_CXX_COMPILER arm-linux-gnueabihf-g++)

set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_PACKAGE ONLY)

set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)

add_executable(passivity_project
        CANopenCommunication.cpp
        CANopenNode.cpp
        PDOmessage.cpp
        PDOpart.cpp
        EPOSnode.cpp
        main.cpp
        CANopenUtils.cpp)

target_link_libraries(passivity_project Threads::Threads)

install(CODE "execute_process(COMMAND scp ${CMAKE_BINARY_DIR}/passivity_project debian@192.168.7.2:/home/debian/passivity_project)")

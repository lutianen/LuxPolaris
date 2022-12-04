cmake_minimum_required(VERSION 3.24)
project(LuxPolaris C CXX)

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE "Release")
endif()

set(CXX_FLAGS
        -g
        # -DVALGRIND
        -DCHECK_PTHREAD_RETURN_VALUE
        -D_FILE_OFFSET_BITS=64
        -Wall
        -Wextra
        -Werror
        -Wconversion
        -Wno-unused-parameter
        # -Wold-style-cast
        -Woverloaded-virtual
        -Wpointer-arith
        -Wshadow
        -Wwrite-strings
        -march=native
        # -MMD
        # -std=c++14
        -rdynamic)

option(CXX_VERSION "c++ version" 11)
list(APPEND CXX_FLAGS "-std=c++${CXX_VERSION}")

# 32 bit lib
if(CMAKE_BUILD_BITS EQUAL 32)
    list(APPEND CXX_FLAGS "-m32")
endif()

# Clang 
if(CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
    list(APPEND CXX_FLAGS "-Wno-null-dereference")
    list(APPEND CXX_FLAGS "-Wno-sign-conversion")
    list(APPEND CXX_FLAGS "-Wno-unused-local-typedef")
    list(APPEND CXX_FLAGS "-Wthread-safety")
    list(REMOVE_ITEM CXX_FLAGS "-rdynamic")
endif()
string(REPLACE ";" " " CMAKE_CXX_FLAGS "${CXX_FLAGS}")

set(CMAKE_CXX_FLAGS_DEBUG "-O0")
set(CMAKE_CXX_FLAGS_RELEASE "-O2 -DNDEBUG")
set(EXECUTABLE_OUTPUT_PATH ${PROJECT_BINARY_DIR}/bin)
set(LIBRARY_OUTPUT_PATH ${PROJECT_BINARY_DIR}/lib)

include_directories(${PROJECT_SOURCE_DIR})

string(TOUPPER ${CMAKE_BUILD_TYPE} BUILD_TYPE)
message(STATUS "CXX_FLAGS = " ${CMAKE_CXX_FLAGS} " " ${CMAKE_CXX_FLAGS_${BUILD_TYPE}})

# src
set (SRCS
    src/Acceptor.cc
    src/Buffer.cc
    src/Channel.cc
    src/EPollPoller.cc
    src/EventLoop.cc
    src/EventLoopThread.cc
    src/EventLoopThreadPool.cc
    src/InetAddress.cc
    src/Poller.cc
    src/Sockets.cc
    src/TCPConnection.cc
    src/Connector.cc
    src/TCPClient.cc
    src/TCPServer.cc
    src/Timer.cc
    src/TimerQueue.cc
)

include_directories(../thirdparty/LuxLog/src)
include_directories(src/include)

# LuxPolaris
add_library(LuxPolaris ${SRCS})
target_link_libraries(LuxPolaris LuxLog gtest pthread rt)
install(TARGETS LuxPolaris DESTINATION lib)


add_subdirectory(test)

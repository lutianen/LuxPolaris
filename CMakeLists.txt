cmake_minimum_required(VERSION 3.24)
project(LuxPolaris C CXX)

# Logger
add_subdirectory(thirdparty/LuxLog)
include_directories(thirdparty/LuxLog/src)
# GoogleTest
add_subdirectory(thirdparty/googletest)
# Test
add_subdirectory(test)


cmake_minimum_required(VERSION 3.24)
project(LuxPolaris C CXX)

# Logger
set(LuxLog_BUILD_EXAMPLES OFF CACHE INTERNAL "Don't Build examples with LuxLog")
set(LuxLog_BUILD_TEST OFF CACHE INTERNAL "Don't Build tests with LuxLog")
add_subdirectory(thirdparty/LuxLog)
include_directories(thirdparty/LuxLog/src)
# GoogleTest
add_subdirectory(thirdparty/googletest)
# Test
add_subdirectory(test)


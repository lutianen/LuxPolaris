cmake_minimum_required(VERSION 3.24)
project(LuxPolaris C CXX)

# Set c++ version
set(CXX_VERSION 17 CACHE INTERNAL "c++ version")

# LuxPolaris
add_subdirectory(LuxPolaris)

# Logger
set(LuxLog_BUILD_EXAMPLES OFF CACHE INTERNAL "Don't Build examples with LuxLog")
set(LuxLog_BUILD_TEST OFF CACHE INTERNAL "Don't Build tests with LuxLog")
add_subdirectory(thirdparty/LuxLog)


# GoogleTest
if(CXX_VERSION EQUAL 11)
    message("gtest require cxx >= 14")
else()
    message("Add gtest")
    add_subdirectory(thirdparty/googletest)
endif()



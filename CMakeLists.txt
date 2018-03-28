cmake_minimum_required(VERSION 3.1.0)

project(binance-cxx-api)

set(PROJECT_VERSION_MAJOR 0)
set(PROJECT_VERSION_MINOR 1)
set(PROJECT_VERSION_PATCH 0)

set(CMAKE_CXX_STANDARD 11)

# Manual build type selection (for debugging purposes)
#set(CMAKE_BUILD_TYPE Debug)
set(CMAKE_BUILD_TYPE Release)

# Search path for CMake include files.
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_CURRENT_SOURCE_DIR}/cmake)

find_package(MHD REQUIRED)

file(GLOB_RECURSE BINARY_SRC "src/*.cpp" "include/*.h")

add_executable(biserver ${BINARY_SRC})
target_link_libraries(biserver microhttpd)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)

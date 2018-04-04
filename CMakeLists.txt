cmake_minimum_required(VERSION 3.1.0)

project(binance-server-emulator)

set(PROJECT_VERSION_MAJOR 0)
set(PROJECT_VERSION_MINOR 1)
set(PROJECT_VERSION_PATCH 0)

set(CMAKE_CXX_STANDARD 11)

# Manual build type selection (for debugging purposes)
set(CMAKE_BUILD_TYPE Debug)
#set(CMAKE_BUILD_TYPE Release)

# Search path for CMake include files.
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_CURRENT_SOURCE_DIR}/cmake)

find_package(CURL REQUIRED)
find_package(LibWebSockets REQUIRED)
find_package(JsonCpp REQUIRED)
find_package(MHD REQUIRED)
find_package(Threads REQUIRED)
find_package(OpenSSL REQUIRED)

file(GLOB_RECURSE BINARY_SRC "src/*.cpp" "include/*.h")

add_executable(biserver ${BINARY_SRC})
target_link_libraries(biserver microhttpd curl)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)

# Enable ExternalProject CMake module
include(ExternalProject)

# Download and install GoogleTest
ExternalProject_Add(
	gtest
	GIT_REPOSITORY https://github.com/google/googletest.git
	PREFIX ${CMAKE_CURRENT_BINARY_DIR}/gtest
	# Disable install step
	INSTALL_COMMAND ""
)

# Get GTest source and binary directories from CMake project
ExternalProject_Get_Property(gtest source_dir binary_dir)

# Create a libgtest target to be used as a dependency by test programs
add_library(libgtest IMPORTED STATIC GLOBAL)
add_dependencies(libgtest gtest)

# Set libgtest properties
set_target_properties(libgtest PROPERTIES
	"IMPORTED_LOCATION" "${binary_dir}/googlemock/gtest/libgtest.a"
	"IMPORTED_LINK_INTERFACE_LIBRARIES" "${CMAKE_THREAD_LIBS_INIT}")

include_directories("${source_dir}/googletest/include")

add_executable(biserver_test ${BINARY_SRC})
target_compile_definitions(biserver_test PUBLIC TESTING)
target_link_libraries(biserver_test microhttpd libgtest jsoncpp curl websockets pthread ${OPENSSL_CRYPTO_LIBRARY})


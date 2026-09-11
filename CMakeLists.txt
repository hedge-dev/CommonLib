cmake_minimum_required(VERSION 3.20)
set(CMAKE_CXX_STANDARD 20)

project("CommonLib")

message(STATUS "System: ${CMAKE_SYSTEM_NAME}")

include("toolchains/Processor.cmake")
GetTargetProcessorArchitecture(CMNLIB_TARGET_ARCH)

message(STATUS "Target: ${CMNLIB_TARGET_ARCH}")

set(CMNLIB_LIBS)
set(CMNLIB_INCLUDES)
set(CMNLIB_SOURCES)

set(CMNLIB_ROOT "${CMAKE_SOURCE_DIR}")
set(CMNLIB_INCLUDE_ROOT "${CMNLIB_ROOT}/include")
set(CMNLIB_THIRDPARTY_ROOT "${CMNLIB_ROOT}/thirdparty")

add_subdirectory(${CMNLIB_THIRDPARTY_ROOT})
add_subdirectory(${CMNLIB_INCLUDE_ROOT})

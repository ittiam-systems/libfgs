cmake_minimum_required(VERSION 3.24)
set(projName fgs)
project(${projName} C)

if("${CMAKE_CURRENT_SOURCE_DIR}" STREQUAL "${CMAKE_CURRENT_BINARY_DIR}")
  message(
    FATAL_ERROR
      "Building from within the libfgs source tree is not supported.\n")
endif()

if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE "Release")
endif()

option(ENABLE_SIMD "Flag to enable SIMD functions" ON)

set(PROJECT_ROOT ${CMAKE_CURRENT_SOURCE_DIR})

# Adds a target named 'ithread'
add_subdirectory(libthread lib EXCLUDE_FROM_ALL)

include("${CMAKE_CURRENT_SOURCE_DIR}/cmake/utils.cmake")
defineinterfaces()

include("${CMAKE_CURRENT_SOURCE_DIR}/fgs/lib.cmake")
include("${CMAKE_CURRENT_SOURCE_DIR}/test/sample_app/app.cmake")

## Copyright (c) 2017, Visillect Service LLC. All rights reserved.
##

cmake_minimum_required(VERSION 3.5.1)

set(CMAKE_CONFIGURATION_TYPES "Debug;Release;RelWithDebInfo" CACHE STRING "possible build types" FORCE)

# IMPORTANT: this command must be placed after CMAKE_CONFIGURATION_TYPES is changed
project(segmentation)

# CMAKE_BUILD_TYPE must not be used with non make-based generators (we only consider MSVC here)
# FIXME: we need a generic way of checking whether a generator is make-based or not
if(MSVC_IDE)
  if(CMAKE_BUILD_TYPE)
    message(FATAL_ERROR "CMAKE_BUILD_TYPE is not supported by MSVC generator")
  endif()
else()
  if(CMAKE_BUILD_TYPE)
    if(NOT(CMAKE_BUILD_TYPE STREQUAL "Release" OR
           CMAKE_BUILD_TYPE STREQUAL "Debug" OR
           CMAKE_BUILD_TYPE STREQUAL "RelWithDebInfo"))
      message(FATAL_ERROR "Invalid CMAKE_BUILD_TYPE. Possible values: Debug [default], Release, RelWithDebInfo")
    endif()
  else()
    message(STATUS "Using default build type: Debug")
    set(CMAKE_BUILD_TYPE "Debug")
  endif()
endif()

find_package(jsoncpp QUIET)
if (jsoncpp_FOUND)
  message(STATUS "Found jsoncpp: ${jsoncpp_DIR}")
endif()

include(cmake/compiler.cmake)
include(cmake/options.cmake)
include(cmake/arch.cmake)
include(cmake/func_macro.cmake)

# boost setup
if(WIN32)
  set(Boost_USE_STATIC_LIBS ON)
else()
  set(Boost_USE_STATIC_LIBS OFF)
endif()

find_package(Boost REQUIRED COMPONENTS system filesystem)
set(SEGM_BOOST_VERSION "${Boost_MAJOR_VERSION}.${Boost_MINOR_VERSION}")
if(SEGM_BOOST_VERSION VERSION_LESS "1.54")
  message(FATAL_ERROR "Bad boost version.")
endif()
include_directories(${Boost_INCLUDE_DIRS})

find_package(OpenCV REQUIRED)
if (OpenCV_VERSION_MAJOR EQUAL "3")
  add_definitions("-DOPENCV3=1")
else()
  message(FATAL_ERROR "Bad OpenCV version.")
endif()
include_directories(${OpenCV_INCLUDE_DIRS})

set(MINSUBSYSTEM_ENABLE_MINBASE ON CACHE BOOL "enable minbase")
set(MINSUBSYSTEM_ENABLE_MINGEO OFF CACHE BOOL "enable mingeo")
set(MINSUBSYSTEM_ENABLE_MINUTILS ON CACHE BOOL "enable minutils")
set(MINSUBSYSTEM_ENABLE_MINIMGAPI ON CACHE BOOL "enable minimgapi")
set(MINSUBSYSTEM_ENABLE_MINIMGPRC ON CACHE BOOL "enable minimgprc")
set(MINSUBSYSTEM_ENABLE_MINIMGIO ON CACHE BOOL "enable minimgio")
set(MINSUBSYSTEM_ENABLE_MINSTOPWATCH ON CACHE BOOL "enable minstopwatch")

# global include directories
include_directories(thirdparty/zlib/src)
include_directories(${CMAKE_CURRENT_BINARY_DIR}/thirdparty/libtiff)
include_directories(${CMAKE_CURRENT_BINARY_DIR}/thirdparty/libpng)
include_directories(thirdparty)

include_directories(minsubsystem)
include_directories(vi_packages)

# Sort of ad-hoc include paths.

# output directories
set(SEGM_BINARY_PATH ${CMAKE_BINARY_DIR}/segmentation)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG ${SEGM_BINARY_PATH}/bin.${SEGMENTATION_ARCH}.debug)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE ${SEGM_BINARY_PATH}/bin.${SEGMENTATION_ARCH}.release)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELWITHDEBINFO ${SEGM_BINARY_PATH}/bin.${SEGMENTATION_ARCH}.relwithdebinfo)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY_DEBUG ${SEGM_BINARY_PATH}/lib.${SEGMENTATION_ARCH}.debug)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY_RELEASE ${SEGM_BINARY_PATH}/lib.${SEGMENTATION_ARCH}.release)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY_RELWITHDEBINFO ${SEGM_BINARY_PATH}/lib.${SEGMENTATION_ARCH}.relwithdebinfo)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY_DEBUG ${SEGM_BINARY_PATH}/bin.${SEGMENTATION_ARCH}.debug)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY_RELEASE ${SEGM_BINARY_PATH}/bin.${SEGMENTATION_ARCH}.release)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY_RELWITHDEBINFO ${SEGM_BINARY_PATH}/lib.${SEGMENTATION_ARCH}.relwithdebinfo)

###################################################################

add_subdirectory(thirdparty)
add_subdirectory(minsubsystem)
add_subdirectory(vi_packages)

option(BUILD_SANDBOX  "Build sandbox projects" OFF)
if (BUILD_SANDBOX)
  include_directories(sandbox)
  add_subdirectory(sandbox)
endif()

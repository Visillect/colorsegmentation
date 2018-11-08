## Copyright (c) 2017, Visillect Service LLC. All rights reserved.
##

cmake_minimum_required(VERSION 2.8.12)

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
include(cmake/svn_external.cmake)

# global include directories
include_directories(vi_packages)
# include_directories(se_packages)
include_directories(minsubsystem)
include_directories(thirdparty)
# include_directories(tools)
# include_directories(sandbox)

# Sort of ad-hoc include paths.
# include_directories(prj.thirdparty/tbb/include)
# include_directories(prj.thirdparty/protobuf/src)
# include_directories(prj.thirdparty/yajl/src/api)
# include_directories(prj.thirdparty/zlib/src)
# include_directories(prj.thirdparty/gtest)
# include_directories(${CMAKE_CURRENT_BINARY_DIR}/prj.thirdparty/yajl/include)

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

add_subdirectory(vi_packages)
# add_subdirectory(se_packages)
add_subdirectory(minsubsystem)
add_subdirectory(thirdparty)
# add_subdirectory(sandbox)
# add_subdirectory(tools)

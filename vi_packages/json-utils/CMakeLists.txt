cmake_minimum_required(VERSION 3.0.0)
project(json-utils)

add_library(json-utils INTERFACE)
target_include_directories(json-utils
  INTERFACE
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

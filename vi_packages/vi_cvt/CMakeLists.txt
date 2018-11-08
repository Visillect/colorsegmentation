cmake_minimum_required(VERSION 3.0.0)
project(vi_cvt)

add_library(vi_cvt INTERFACE)
target_include_directories(vi_cvt INTERFACE ${CMAKE_CURRENT_SOURCE_DIR})

cmake_minimum_required(VERSION 3.0.0)
project(mximg)

add_library(mximg
  include/mximg/image.h
  src/image.cpp
)
target_link_libraries(mximg
  PUBLIC
    minimgapi
    vi_cvt
  PRIVATE
    minimgio
)
target_include_directories(mximg PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)

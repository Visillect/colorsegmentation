treat_warnings_as_errors()

cmake_minimum_required(VERSION 2.8.12)

project(remseg)

set(HEADERS
  distance_func.h
  edge_heap.h
  image_map.h
  image.h
  segmentator.hpp
  vertex.h
  utils.h
)

set(SOURCES
  src/distance_func.cpp
  src/edge_heap.cpp
  src/image_map.cpp
  src/image.cpp
  src/vertex.cpp
  src/utils.cpp
)

add_library(remseg
  ${HEADERS}
  ${SOURCES}
)

target_link_libraries(remseg
  minbase
  mingeo
  minimgapi
  minimgio
  i8r
  validate_json
  vi_cvt
)

#----------------demo---------------------------------------
add_executable(remseg_go demo/remseg_go.cpp)
target_link_libraries(remseg_go remseg)

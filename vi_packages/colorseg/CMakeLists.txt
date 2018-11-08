treat_warnings_as_errors()

cmake_minimum_required(VERSION 2.8.12)

project(colorseg)

find_package(OpenCV REQUIRED)
include_directories(SYSTEM ${OpenCV_INCLUDE_DIRS})

find_package(Boost REQUIRED COMPONENTS system filesystem)

set(HEADERS
  color_distance_func.h
  color_vertex.h
  colorspace_transform.h
)

set(SOURCES
  src/color_distance_func.cpp
  src/color_vertex.cpp
)

add_library(colorseg
  ${HEADERS}
  ${SOURCES}
)

target_link_libraries(colorseg
  remseg
)

#----------------demo---------------------------------------
add_executable(colorseg_go demo/colorseg_go.cpp)
target_link_libraries(colorseg_go
  ${Boost_LIBRARIES}
  ${OpenCV_LIBS}
  colorseg)

add_executable(obtain_blocklist demo/obtain_blocklist.cpp)
target_link_libraries(obtain_blocklist
  ${Boost_LIBRARIES}
  ${OpenCV_LIBS}
  colorseg)
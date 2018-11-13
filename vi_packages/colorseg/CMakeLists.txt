treat_warnings_as_errors()

cmake_minimum_required(VERSION 3.0.0)

project(colorseg)

find_package(OpenCV REQUIRED)
find_package(Boost REQUIRED COMPONENTS system filesystem)

add_library(colorseg
  src/color_distance_func.cpp
  src/color_vertex.cpp
)

target_include_directories(remseg
  PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include
  PRIVATE
    ${PROJECT_BINARY_DIR}
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
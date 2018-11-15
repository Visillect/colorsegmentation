treat_warnings_as_errors()

cmake_minimum_required(VERSION 3.0.0)

project(remseg)

find_package(OpenCV REQUIRED)

add_library(remseg
  src/distance_func.cpp
  src/edge_heap.cpp
  src/image_map.cpp
  src/vertex.cpp
  src/utils.cpp
)

target_include_directories(remseg
  PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include
  PRIVATE
    ${PROJECT_BINARY_DIR}
)

target_link_libraries(remseg
    minbase
    mingeo
    minimgapi
    minimgio
    mximg
    i8r
    validate_json
    vi_cvt
)

#----------------demo---------------------------------------
add_executable(remseg_go demo/remseg_go.cpp)
target_link_libraries(remseg_go
  remseg
  ${OpenCV_LIBS})

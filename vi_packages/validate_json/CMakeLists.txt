cmake_minimum_required(VERSION 3.0.0)
treat_warnings_as_errors()

project(validate_json)

find_package(Boost REQUIRED COMPONENTS regex)

add_library(validate_json
  src/validate_json.cpp
)

target_include_directories(validate_json
  PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include
  PRIVATE
    ${Boost_INCLUDE_DIRS}
)

target_link_libraries(validate_json
  PUBLIC
    jsoncpp_compat
  PRIVATE
    ${Boost_LIBRARIES}
)

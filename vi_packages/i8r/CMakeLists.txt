cmake_minimum_required(VERSION 3.0.0)
project(i8r)

find_package(OpenCV REQUIRED)

if(WIN32)
  set(Boost_USE_STATIC_LIBS        ON)
endif()
find_package(Boost REQUIRED COMPONENTS system filesystem date_time regex thread)

add_library(i8r SHARED
  include/i8r/i8r.h
  include/i8r/thirdparty_fwd.h
  include/i8r/types.h
  src/disabled_logger.h
  src/derived_logger.h
  src/draw_utils.h
  src/environment.cpp
  src/environment.h
  src/logger_impl.h
  src/logger_impl.cpp
  src/image_converter.h
  src/image_converter.cpp
  src/image_group.h
  src/image_writer.h
  src/disabled_image_writer.h
  src/image_dir_writer.h
  src/video_writer.h
  src/i8r.cpp
  src/utils.cpp
  src/utils.h
)
target_link_libraries(i8r
  PUBLIC
    jsoncpp_compat
  PRIVATE
    vi_cvt
    ${OpenCV_LIBRARIES}
    ${Boost_LIBRARIES}
)

target_include_directories(i8r
  PUBLIC
    ${Boost_INCLUDE_DIRS}
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_compile_definitions(i8r PRIVATE -DI8R_EXPORTS)

add_executable(i8r_demo
  demo/i8r_demo.cpp
)
target_link_libraries(i8r_demo
  i8r
  ${OpenCV_LIBRARIES}
)

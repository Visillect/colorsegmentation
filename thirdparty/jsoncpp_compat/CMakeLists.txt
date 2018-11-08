cmake_minimum_required(VERSION 3.0.0)
project(jsoncpp_compat)

add_library(${PROJECT_NAME} INTERFACE)
target_include_directories(${PROJECT_NAME}
  INTERFACE
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

if (NOT JSONCPP_REAL_LIB_NAME)
  set(JSONCPP_REAL_LIB_NAME jsoncpp_lib)
endif()
target_link_libraries(${PROJECT_NAME}
  INTERFACE
    ${JSONCPP_REAL_LIB_NAME}
)

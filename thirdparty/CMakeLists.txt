include_directories(
  ${CMAKE_CURRENT_BINARY_DIR}/zlib
  ${CMAKE_CURRENT_BINARY_DIR}/libtiff
  ${CMAKE_CURRENT_BINARY_DIR}/libjpeg
  ${CMAKE_CURRENT_BINARY_DIR}/libpng
)

svn_external(DIR zlib URL https://vis.iitp.ru/svn/se/thirdparty/zlib/tags/1_2_5)
set(ZLIB_INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/zlib)
set(ZLIB_LIBRARIES zlib)
set(ZLIB_FOUND TRUE)

svn_external(DIR libjpeg URL https://vis.iitp.ru/svn/se/thirdparty/libjpeg/tags/8c)
set(LIBJPEG_INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/jpeg)
set(LIBJPEG_LIBRARIES jpeg)
set(LIBJPEG_FOUND TRUE)

if (WITH_TIFF)
  svn_external(DIR libtiff URL https://vis.iitp.ru/svn/se/thirdparty/libtiff/tags/3_9_5)
  set(TIFF_INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/tiff)
  set(TIFF_LIBRARIES tiff)
  set(TIFF_FOUND TRUE)
endif (WITH_TIFF)

svn_external(DIR libpng URL https://vis.iitp.ru/svn/se/thirdparty/libpng/tags/1_5_10beta01)
set(PNG_INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/png)
set(PNG_LIBRARIES png)
set(PNG_LIBRARY png)
set(PNG_FOUND TRUE)

if(NOT jsoncpp_FOUND)
  svn_external(DIR json-cpp URL https://vis.iitp.ru/svn/vi/thirdparty/json-cpp/tags/1_7_4)
else()
  message(STATUS "json-cpp already found, will not build")
endif()
svn_external(DIR jsoncpp_compat URL https://vis.iitp.ru/svn/vi/thirdparty/jsoncpp_compat/tags/1_0)
svn_external(DIR valijson URL https://vis.iitp.ru/svn/vi/thirdparty/valijson/tags/a1d330e285.patched DO_NOT_ADD True)

if (WITH_TESTS)
  svn_external(DIR gtest URL https://vis.iitp.ru/svn/se/thirdparty/gtest/tags/r654)
  add_subdirectory(gtest)
endif (WITH_TESTS)

svn_external(DIR Eigen URL https://vis.iitp.ru/svn/se/thirdparty/Eigen/tags/3_2_5 DO_NOT_ADD True)
svn_external(DIR tclap URL https://vis.iitp.ru/svn/vi/thirdparty/tclap/tags/1-2-1-release-final)
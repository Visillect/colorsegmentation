include_directories(${CMAKE_BINARY_DIR}/thirdparty/libtiff)
include_directories(${CMAKE_BINARY_DIR}/thirdparty/libpng)

set(MINIMGPRC_DISABLE_RARELY_USED_FUNCTIONS OFF CACHE BOOL "Don't compile rarely used functions ­-- override" FORCE)

if (NOT MSVC)
  set(WITH_SYSTEM_PNG ON CACHE BOOL "")
endif()

disable_all_warnings()

svn_external(DIR minbase URL https://vis.iitp.ru/svn/se/packages/minbase/trunk@28189)
svn_external(DIR mingeo URL https://vis.iitp.ru/svn/se/packages/mingeo/trunk@28189)
svn_external(DIR minimgapi URL https://vis.iitp.ru/svn/se/packages/minimgapi/trunk@28189)
svn_external(DIR minimgio URL https://vis.iitp.ru/svn/se/packages/minimgio/trunk@30426)
svn_external(DIR minutils URL https://vis.iitp.ru/svn/se/packages/minutils/trunk@28189)

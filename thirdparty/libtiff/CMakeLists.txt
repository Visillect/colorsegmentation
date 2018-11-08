# ----------------------------------------------------------------------------
#  CMake file for libtiff. See root CMakeLists.txt
#
# ----------------------------------------------------------------------------
project(libtiff)

# Options
option(WITH_TIFF_LZW          "Support TIFF LZW format" ON)
option(WITH_TIFF_ZIP          "Support TIFF ZIP format" OFF)
option(WITH_TIFF_JPEG         "Support TIFF JPEG format" OFF)

if(WITH_TIFF_LZW)
  set(LZW_SUPPORT 1)
endif()

if(WITH_TIFF_JPEG)
  set(JPEG_SUPPORT 1)
  include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../libjpeg)
endif()

if(WITH_TIFF_ZIP)
  set(ZIP_SUPPORT 1)
  include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../zlib)
endif()

include(CheckTypeSize)
include(CheckFunctionExists)
include(CheckIncludeFile)
enable_testing()

check_include_file(assert.h    HAVE_ASSERT_H)
check_include_file(fcntl.h     HAVE_FCNTL_H)
check_include_file(string.h    HAVE_STRING_H)
check_include_file(sys/types.h HAVE_SYS_TYPES_H)
check_include_file(io.h        HAVE_IO_H)
check_include_file(search.h    HAVE_SEARCH_H)
check_include_file(unistd.h    HAVE_UNISTD_H)


check_function_exists(jbg_newlen HAVE_JBG_NEWLEN)
check_function_exists(setmode    HAVE_SETMODE)
check_function_exists(lfind      HAVE_LFIND)
check_function_exists(strchr     HAVE_STRCHR)

check_type_size(int SIZEOF_INT)
check_type_size(long SIZEOF_LONG)

configure_file(${CMAKE_CURRENT_SOURCE_DIR}/tif_config.h.cmakein
               ${CMAKE_CURRENT_BINARY_DIR}/tif_config.h @ONLY)

configure_file(${CMAKE_CURRENT_SOURCE_DIR}/tiffconf.h.cmakein
               ${CMAKE_CURRENT_BINARY_DIR}/tiffconf.h @ONLY)

include_directories(${CMAKE_CURRENT_BINARY_DIR})


# The .cpp files:

set(lib_srcs
  t4.h
    tiffio.hxx
    tiffiop.h
    tif_aux.c
    tif_close.c
    tif_codec.c
    tif_color.c
    tif_compress.c
    tif_dir.c
    tif_dir.h
    tif_dirinfo.c
    tif_dirread.c
    tif_dirwrite.c
    tif_dumpmode.c
    tif_error.c
    tif_extension.c
    tif_fax3.c
    tif_fax3.h
    tif_fax3sm.c
    tif_flush.c
    tif_getimage.c
    tif_jbig.c
    tif_jpeg.c
    tif_luv.c
    tif_lzw.c
    tif_next.c
    tif_ojpeg.c
    tif_open.c
    tif_packbits.c
    tif_pixarlog.c
    tif_predict.c
    tif_predict.h
    tif_print.c
    tif_read.c
    tif_stream.cxx
    tif_strip.c
    tif_swab.c
    tif_thunder.c
    tif_tile.c
    tif_version.c
    tif_warning.c
    tif_write.c
    tif_zip.c
    uvcode.h
    )

if(UNIX OR TOOLCHAIN_UNIX OR ANDROID OR WINPHONE)
    set(lib_srcs ${lib_srcs} tif_unix.c)
endif()

if(WIN32 AND NOT WINPHONE)
    set(lib_srcs ${lib_srcs} tif_win32.c)
endif(WIN32 AND NOT WINPHONE)

#if(APPLE)
#    set(lib_srcs ${lib_srcs} tif_apple.c)
#endif(APPLE)

file(GLOB lib_hdrs *.h*)
file(GLOB cfg_hdrs ${CMAKE_CURRENT_BINARY_DIR}/*.h)

set(the_target "tiff")

if(MSVC)
    string(REPLACE "/W4" "/W0" CMAKE_C_FLAGS "${CMAKE_C_FLAGS}")
    string(REPLACE "/W4" "/W0" CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")
endif()

if(UNIX)
    if(CMAKE_COMPILER_IS_GNUCXX OR CV_ICC)
        set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fPIC")
        set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wno-unused-parameter")
        set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wno-pointer-to-int-cast")
        set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wno-implicit-function-declaration")
        set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wno-sign-compare")
        set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wno-int-to-pointer-cast")
    endif()
endif()

add_library(${the_target} STATIC ${lib_srcs} ${lib_hdrs} ${cfg_hdrs})

IF (WITH_TIFF_ZIP)
  TARGET_LINK_LIBRARIES(${the_target} zlib)
ENDIF()

IF (WITH_TIFF_JPEG)
  TARGET_LINK_LIBRARIES(${the_target} jpeg)
ENDIF()


INSTALL(
  TARGETS ${the_target}
  LIBRARY DESTINATION lib
  ARCHIVE DESTINATION lib)
  
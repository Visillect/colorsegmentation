project(minimgio)

option(WITH_DSHOW "Use DirectX Show" OFF)

set(minimgio_public_HEADERS
  define.h
  device.h
  contrib.h
  minimgio.h)

set(minimgio_HEADERS
  ${minimgio_public_HEADERS}
  src/minimgiolst.h
  src/minimgiopng.h
  src/minimgiojpeg.h
  src/minimgiotiff.h
  src/minimgiowebp.h
  src/minimgiodevice.h
  src/subsystem.h
  src/camstream.h
  src/stream.h
  src/utils.h
  src/pack.h)

set(minimgio_SRCS
  src/minimgio.cpp
  src/device.cpp
  src/contrib.cpp
  src/minimgiolst.cpp
  src/minimgiopng.cpp
  src/minimgiojpeg.cpp
  src/minimgiotiff.cpp
  src/minimgiowebp.cpp
  src/minimgiodevice.cpp
  src/subsystem.cpp
  src/camstream.cpp
  src/utils.cpp
  src/pack.cpp)

set(thirdparty_LIBS)

option(WITH_JPEG "Turns on support for JPEG image format." ON)
option(WITH_SYSTEM_JPEG "Use system JPEG library." OFF)
if (WITH_JPEG)
  add_definitions(-DWITH_JPEG)
  if (WITH_SYSTEM_JPEG)
    find_package(JPEG REQUIRED)
    set(thirdparty_LIBS ${thirdparty_LIBS} ${JPEG_LIBRARIES})
  else()
    set(thirdparty_LIBS ${thirdparty_LIBS} jpeg)
  endif()
endif (WITH_JPEG)

option(WITH_TIFF "Turns on support for TIFF image format." ON)
option(WITH_SYSTEM_TIFF "Use system TIFF library." OFF)
if (WITH_TIFF)
  add_definitions(-DWITH_TIFF)
  if (WITH_SYSTEM_TIFF)
    find_package(TIFF REQUIRED)
    include_directories(${TIFF_INCLUDE_DIR})
    set(thirdparty_LIBS ${thirdparty_LIBS} ${TIFF_LIBRARIES})
  else()
    set(thirdparty_LIBS ${thirdparty_LIBS} tiff)
    include_directories(${libtiff_SOURCE_DIR}
                        ${CMAKE_BINARY_DIR}/prj.thirdparty/libtiff)
  endif()
endif (WITH_TIFF)

option(WITH_PNG "Turns on support for PNG image format." ON)
option(WITH_SYSTEM_PNG "Use system PNG library." OFF)
if (WITH_PNG)
  add_definitions(-DWITH_PNG)
  if (WITH_SYSTEM_PNG)
    find_package(PNG REQUIRED)
    include_directories(${PNG_INCLUDE_DIRS})
    set(thirdparty_LIBS ${thirdparty_LIBS} ${PNG_LIBRARIES})
  else()
    include_directories(${CMAKE_BINARY_DIR}/prj.thirdparty/libpng)
    include_directories(${png_SOURCE_DIR})
    set(thirdparty_LIBS ${thirdparty_LIBS} png)
  endif()
endif (WITH_PNG)

if (WITH_TIFF OR WITH_PNG)
  # low quality code below is required because someone removed `zlib` linkage
  # in `libpng` and possibly in `libtiff` library too
  set(thirdparty_LIBS ${thirdparty_LIBS} zlib)
endif()

option(WITH_WEBP "Turns on support for WebP image format." OFF)
option(WITH_SYSTEM_WEBP "Turns on support for WebP image format." OFF)
if (WITH_WEBP)
  add_definitions(-DWITH_WEBP)
  if (WITH_SYSTEM_WEBP)
    # we hope webp is in path - no error is given at configuration time
    set(thirdparty_LIBS ${thirdparty_LIBS} libwebp)
  else()
    set(thirdparty_LIBS ${thirdparty_LIBS} libwebp)
    include_directories(${CMAKE_SOURCE_DIR}/prj.thirdparty/libwebp)
  endif()
endif (WITH_WEBP)

option(WITH_DSHOW "Turns on dshow support for video input." OFF)
option(WITH_V4L2 "Turns on v4l2 support for video input." OFF)
option(WITH_AVFOUNDATION "Turns on avfoundation support for video input." OFF)

if(WITH_DSHOW AND MSVC)

  add_definitions(-DDIRECTX_SHOW)

  set(minimgio_HEADERS
    ${minimgio_HEADERS}
    src/dshowutils.h
    src/dshowdevice.h
    src/dshowsubsystem.h)

  set(minimgio_SRCS
    ${minimgio_SRCS}
    src/dshowutils.cpp
    src/dshowdevice.cpp
    src/dshowsubsystem.cpp)

  set(thirdparty_LIBS
    ${thirdparty_LIBS}
    winmm
    strmiids)

elseif(WITH_V4L2)

  add_definitions(-DV4L2)

  set(v4l2_HEADERS
    src/v4l2device.h
    src/v4l2subsystem.h
    src/v4l2utils.h)
  set(minimgio_HEADERS
    ${minimgio_HEADERS}
    ${v4l2_HEADERS})

  set(v4l2_SRCS
    src/v4l2device.cpp
    src/v4l2subsystem.cpp
    src/v4l2utils.cpp)
  set(minimgio_SRCS
    ${minimgio_SRCS}
    ${v4l2_SRCS})

  set(thirdparty_LIBS
    ${thirdparty_LIBS}
    v4l2)

elseif(WITH_AVFOUNDATION)

  add_definitions(-DAVFOUNDATION)

  set(av_HEADERS
    src/avfoundationdevice.h
    src/avfoundationsubsystem.h
    src/avfoundationutils.h)
  set(minimgio_HEADERS
    ${minimgio_HEADERS}
    ${av_HEADERS})

  set(av_SRCS
    src/avfoundationdevice.mm
    src/avfoundationsubsystem.mm)
  set(minimgio_SRCS
    ${minimgio_SRCS}
    ${av_SRCS})

endif()

if(BUILD_SHARED_LIBS)
  add_definitions(-DMINIMGIO_EXPORTS)
endif()

add_library(minimgio ${minimgio_SRCS} ${minimgio_HEADERS})

# target_link_libraries can't take empty argument - so we check
if(thirdparty_LIBS)
   target_link_libraries(minimgio ${thirdparty_LIBS})
endif()

if(BUILD_GO)
  add_executable(minimgiodev_go src/minimgiodev_go.cpp)
  target_link_libraries(minimgiodev_go minimgio minimgapi)

  if(WITH_AVFOUNDATION)
    macro(ADD_FRAMEWORK appname fwname)
    find_library(FRAMEWORK_${fwname}
        NAMES ${fwname}
        PATHS ${CMAKE_OSX_SYSROOT}/System/Library
        PATH_SUFFIXES Frameworks
        NO_DEFAULT_PATH)
    if( ${FRAMEWORK_${fwname}} STREQUAL FRAMEWORK_${fwname}-NOTFOUND)
        MESSAGE(ERROR ": Framework ${fwname} not found")
    else()
        TARGET_LINK_LIBRARIES(${appname} "${FRAMEWORK_${fwname}}/${fwname}")
        MESSAGE(STATUS "Framework ${fwname} found at ${FRAMEWORK_${fwname}}")
    endif()
    endmacro(ADD_FRAMEWORK)

    # SET (CMAKE_EXE_LINKER_FLAGS
    # "-framework CoreMedia -framework CoreVideo -framework AVFoundation -framework Foundation")

    add_framework(minimgiodev_go CoreMedia)
    add_framework(minimgiodev_go CoreVideo)
    add_framework(minimgiodev_go Foundation)
    add_framework(minimgiodev_go AVFoundation)
   # add_framework(minimgiodev_go CoreGraphics)
   # add_framework(minimgiodev_go MobileCoreServices)
   # add_framework(minimgiodev_go ImageIO)

  endif(WITH_AVFOUNDATION)

endif(BUILD_GO)

if(WITH_TESTS)
  add_executable(test_minimgio_interface test/test_interface.cpp)
  target_link_libraries(test_minimgio_interface minimgio minimgapi gtest)
  add_test(
    NAME test_minimgio_interface
    COMMAND test_minimgio_interface)

  if(WITH_PNG)  # since WITH_PNG=No is tested in test_interface
    add_executable(test_minimgio_png test/test_png.cpp)
    target_link_libraries(test_minimgio_png minimgio minimgapi gtest)
    add_test(
      NAME test_minimgio_png
      COMMAND test_minimgio_png)
  endif(WITH_PNG)
  if(WITH_TIFF)  # since WITH_TIFF=No is tested in test_interface
    add_executable(test_minimgio_tiff test/test_tiff.cpp)
    target_link_libraries(test_minimgio_tiff minimgio minimgapi gtest)
    add_test(
      NAME test_minimgio_tiff
      COMMAND test_minimgio_tiff)
    if (WITH_TIFF_LZW)
      target_compile_definitions(test_minimgio_tiff PRIVATE -DWITH_TIFF_LZW)
    endif()
    if (WITH_TIFF_ZIP)
      target_compile_definitions(test_minimgio_tiff PRIVATE -DWITH_TIFF_ZIP)
    endif()
    if (WITH_TIFF_JPEG)
      target_compile_definitions(test_minimgio_tiff PRIVATE -DWITH_TIFF_JPEG)
    endif()

  endif(WITH_TIFF)
  if(WITH_JPEG)  # since WITH_JPEG=No is tested in test_interface
    add_executable(test_minimgio_jpeg test/test_jpeg.cpp)
    target_link_libraries(test_minimgio_jpeg minimgio minimgapi gtest)
    add_test(
      NAME test_minimgio_jpeg
      COMMAND test_minimgio_jpeg)
  endif(WITH_JPEG)
endif(WITH_TESTS)

install(FILES ${minimgio_public_HEADERS}
  DESTINATION include/minimgio)
install(TARGETS minimgio
  RUNTIME DESTINATION bin
  LIBRARY DESTINATION lib
  ARCHIVE DESTINATION lib)

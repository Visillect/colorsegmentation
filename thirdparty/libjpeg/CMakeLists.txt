# ----------------------------------------------------------------------------
#  CMake file for libjpeg. See root CMakeLists.txt
#
# ----------------------------------------------------------------------------
project(libjpeg)

# List of C++ files:

include_directories(${CMAKE_CURRENT_SOURCE_DIR})

# The .cpp files:
file(GLOB lib_srcs *.c)
file(GLOB lib_hdrs *.h)

# ----------------------------------------------------------------------------------
# 				Define the library target:
# ----------------------------------------------------------------------------------

set(the_target "jpeg")


if(MSVC)
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} /W3")
endif()

if(UNIX)
     if(CMAKE_COMPILER_IS_GNUCXX OR CV_ICC)
        set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fPIC")
        set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wno-main -Wno-unused-parameter")
     endif()
endif()

if(CMAKE_COMPILER_IS_GNUCXX)
    set_source_files_properties(jcdctmgr.c PROPERTIES COMPILE_FLAGS "-O1")
endif()

add_library(${the_target} STATIC ${lib_srcs} ${lib_hdrs})

install (TARGETS ${the_target}
  LIBRARY DESTINATION lib
  ARCHIVE DESTINATION lib)

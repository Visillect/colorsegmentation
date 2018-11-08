# Option independent functions and macros are defined here.


# each argument past the last expected argument is appended to the variable
# listName with the given prefix
# for example:
#   create_prefixed_list(sources src/ a.c b.c d.c)
# will set sources=src/a.c;src/b.c;src/d.c
#
function(create_prefixed_list listName prefix)
  if (${listName})
    message(WARNING "list is not empty and its content will be lost")
  endif()
  unset(lst)
  foreach(item ${ARGN})
    set(item "${prefix}${item}")
    list(APPEND lst ${item})
  endforeach(item)
  set(${listName} ${lst} PARENT_SCOPE)
endfunction(create_prefixed_list)



# prints the specified variable
#
function(print_var varName)
  if (${varName})
    message("${varName}=${${varName}}")
  else()
    message(WARNING "variable ${varName} is not defined")
  endif()
endfunction(print_var)


function(disable_werror_save_flags)
  set(PREV_CMAKE_C_FLAGS ${CMAKE_C_FLAGS} PARENT_SCOPE)
  set(PREV_CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS} PARENT_SCOPE)
  if(CMAKE_COMPILER_IS_GNUCC OR CMAKE_COMPILER_IS_GNUCXX)
    set(COMMON_FLAGS "-Wno-error")
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${COMMON_FLAGS}" PARENT_SCOPE)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${COMMON_FLAGS}" PARENT_SCOPE)
  endif()
endfunction(disable_werror_save_flags)

function(restore_flags)
  set(CMAKE_C_FLAGS "${PREV_CMAKE_C_FLAGS}" PARENT_SCOPE)
  set(CMAKE_CXX_FLAGS "${PREV_CMAKE_CXX_FLAGS}" PARENT_SCOPE)
endfunction(restore_flags)

# Locates boost libraries of the specific version used in our project.
# Argument(s): the components required.
#
# After invocation of this macro the variable ${Boost_LIBRARIES} will
# contain the list of boost libraries you need to link against (for
# usage with target_link_libraries()).
#
# EXAMPLE:
# find_boost_libs(thread)
# target_link_libraries(myapp ${Boost_LIBRARIES})
#
macro(find_boost_libs libs)
  if(WIN32)
    set(Boost_USE_STATIC_LIBS        ON)
  else()
    set(Boost_USE_STATIC_LIBS        OFF)
  endif()
  set(Boost_USE_MULTITHREADED      ON)
  set(Boost_USE_STATIC_RUNTIME    OFF)
  #find_package(Boost ${TTR_BOOST_VERSION} EXACT REQUIRED COMPONENTS ${ARGV})
  find_package(Boost ${TTR_BOOST_VERSION} REQUIRED COMPONENTS ${ARGV})
endmacro(find_boost_libs)



# performs add_subdirectory() on all subdirectories of the current
# directory which contain CMakeLists.txt. Any arguments given to
# this function will be treated as exceptions (i.e. the specified
# directories will not be processed)
#
function(add_all_subdirectories)
  file(GLOB fileList *)
  foreach(item ${fileList})
    get_filename_component(name ${item} NAME)
    list(FIND ARGV ${name} index)
    if (("${index}" STREQUAL "-1") AND IS_DIRECTORY ${item} AND EXISTS ${item}/CMakeLists.txt)
      add_subdirectory(${item})
    endif()
  endforeach(item)
endfunction(add_all_subdirectories)


function(add_vera_test test_name)
  if (WITH_VERA_TESTS)
    file(GLOB_RECURSE SOURCES_FOR_VERA
      *.cpp
      *.h)
    add_test(NAME ${test_name}
      COMMAND vera++ --root "${SWARM_ROOT}/config/vera++" --error ${SOURCES_FOR_VERA})
  endif()
endfunction(add_vera_test)

function(add_kwstyle_test test_name)
  if (WITH_KWSTYLE_TESTS)
    add_test(NAME ${test_name}
      COMMAND KWStyle -xml "${SWARM_ROOT}/config/kwstyle/default.xml" -v -R -d ${CMAKE_CURRENT_SOURCE_DIR})
  endif()
endfunction(add_kwstyle_test)


function(add_style_tests test_name)
  add_vera_test(${test_name}_vera)
  add_kwstyle_test(${test_name}_kwstyle)
endfunction()


macro(treat_warnings_as_errors)
  get_filename_component(dir_name ${CMAKE_CURRENT_SOURCE_DIR} NAME)
  message(STATUS "${dir_name}: Treat warnings as errors")
  if (MSVC)
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} /WX")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /WX")
  else()
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Werror")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Werror")
  endif()
endmacro(treat_warnings_as_errors)

macro(disable_all_warnings)
  get_filename_component(dir_name ${CMAKE_CURRENT_SOURCE_DIR} NAME)
  message(STATUS "${dir_name}: Disable all warnings")
  if (MSVC)
    string(REGEX REPLACE "/W[0-4]" "" CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")
    string(REGEX REPLACE "/W[0-4]" "" CMAKE_C_FLAGS "${CMAKE_C_FLAGS}")
    string(REPLACE "-Wall" "" CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")
    string(REPLACE "-Wall" "" CMAKE_C_FLAGS "${CMAKE_C_FLAGS}")
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} /W0")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /W0")
  else()
    string(REGEX REPLACE "-pedantic" "" CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS})
    string(REGEX REPLACE "-pedantic" "" CMAKE_C_FLAGS ${CMAKE_C_FLAGS})
    string(REGEX REPLACE "-W[^ ]*" "" CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS})
    string(REGEX REPLACE "-W[^ ]*" "" CMAKE_C_FLAGS ${CMAKE_C_FLAGS})
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -w")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -w")
  endif()
endmacro(disable_all_warnings)


#auto-generation of subfolders structure for VisualStudio solution explorer
macro(relative_parent_folder name)
    get_filename_component(PARENT_FOLDER ${CMAKE_CURRENT_SOURCE_DIR} PATH)
    file(RELATIVE_PATH ${name} ${CMAKE_SOURCE_DIR} ${PARENT_FOLDER})
endmacro(relative_parent_folder)

function(add_library name)
  _add_library(${name} ${ARGN})
  if (MSVC)
    if (NOT ${ARGV1} STREQUAL "INTERFACE") #INTERFACE_LIBRARY targets may only have whitelisted properties. The property "FOLDER" is not allowed.
      set(libname ${ARGV0})
      relative_parent_folder(parent_folder)
      set_property(TARGET ${libname} PROPERTY FOLDER ${parent_folder})
    endif()
  endif()
endfunction()

function(add_executable name)
  _add_executable(${name} ${ARGN})
  if (MSVC)
    set(exename ${ARGV0})
    relative_parent_folder(parent_folder)
    set_property(TARGET ${exename} PROPERTY FOLDER ${parent_folder})
  endif()
endfunction()
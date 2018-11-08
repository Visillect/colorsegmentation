# - svn_external
# This module checks out and updates svn externals

include(CMakeParseArguments)

function(svn_external)

set(options DO_NOT_ADD)
set(one_value_args URL DIR INTO)
cmake_parse_arguments(SE "${options}" "${one_value_args}"
                      "" ${ARGN})
if (SE_URL STREQUAL "")
  message(FATAL_ERROR "svn external URL not set")
endif()
set(url "${SE_URL}")

if (SE_DIR STREQUAL "")
  message(FATAL_ERROR "svn external dir name not set")
endif()
set(prjname "${SE_DIR}")
if (SE_INTO)
  set(checkout_dir ${SE_INTO}/${prjname})
else()
  set(checkout_dir ${prjname})
endif()

if(NOT EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/${checkout_dir})
  get_from_svn(${url} ${checkout_dir})
endif()
# set(solution_root ${CMAKE_SOURCE_DIR})
set(solution_root ${PROJECT_SOURCE_DIR})
file(WRITE ${CMAKE_CURRENT_BINARY_DIR}/update_${prjname}.cmake include(${solution_root}/cmake/svn_external.cmake) \nget_from_svn(${url}\t${checkout_dir}))

add_custom_target(update_${prjname}
  COMMAND ${CMAKE_COMMAND} -P ${CMAKE_CURRENT_BINARY_DIR}/update_${prjname}.cmake
  WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
)
if(EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/${checkout_dir}/CMakeLists.txt)
  if (NOT SE_DO_NOT_ADD)
    add_subdirectory(${checkout_dir})
  endif()
endif()
endfunction(svn_external)

function(delete_directory path)
  string(REPLACE "/" "\\" path_backslash ${path})
  if(WIN32)
    execute_process(
      COMMAND cmd /k rmdir /s /q ${path_backslash}
      OUTPUT_QUIET
    )
  else()
    execute_process(
      COMMAND rm -rf ${path}
      OUTPUT_QUIET
    )
  endif()
endfunction(delete_directory)

function(check_svn_access url)
  if(NOT SUBVERSION_FOUND)
    find_package(Subversion)
  endif()
  if(NOT SUBVERSION_FOUND)
    message(FATAL_ERROR "Svn not found")
  endif()
  execute_process(
    COMMAND svn info ${url} --non-interactive
    RESULT_VARIABLE svn_info_result
    TIMEOUT 2
    OUTPUT_QUIET
  )
  if (NOT ${svn_info_result} STREQUAL 0)
    message(FATAL_ERROR "failed to get svn info for ${url}")
  endif()
endfunction(check_svn_access)

macro(get_svn_last_changed_revision url output_result_variable)
  check_svn_access(${url})
  Subversion_WC_INFO(${url} svn_info)
  set(${output_result_variable} ${svn_info_WC_LAST_CHANGED_REV})
endmacro(get_svn_last_changed_revision)

function(checkout_svn url project_name checkout_dir)
  set(PROJECT_DIR ${CMAKE_CURRENT_SOURCE_DIR}/${project_name})
  string(REPLACE "/" "\\" PROJECT_DIR_BACKSLASH ${PROJECT_DIR})
  if(NOT EXISTS ${checkout_dir})
    message("Checking out ${url} to ${checkout_dir}")
    execute_process(
      COMMAND svn checkout ${url} ${checkout_dir} --non-interactive
      OUTPUT_QUIET
    )
    if(NOT EXISTS ${checkout_dir})
      message(FATAL_ERROR "Failed to checkout ${url}, ask for access manually and save password")
    else()
      Subversion_WC_INFO(${checkout_dir} local_svn_info)
        set(ECHO_COMMAND "echo ${local_svn_info_WC_LAST_CHANGED_REV} > ${PROJECT_DIR_BACKSLASH}_revision.txt")
      if(WIN32)
        execute_process(
          COMMAND cmd /k ${ECHO_COMMAND}
          OUTPUT_QUIET
        )
        set(ECHO_COMMAND "echo ${url} >> ${PROJECT_DIR_BACKSLASH}_revision.txt")
        execute_process(
          COMMAND cmd /k ${ECHO_COMMAND}
          OUTPUT_QUIET
        )
      else()
        execute_process(
          COMMAND sh -c "echo ${local_svn_info_WC_LAST_CHANGED_REV} > ${PROJECT_DIR}_revision.txt"
          OUTPUT_QUIET
        )
        execute_process(
          COMMAND sh -c "echo ${url} >> ${PROJECT_DIR}_revision.txt"
          OUTPUT_QUIET
        )
      endif()
      delete_directory(${checkout_dir}/.svn)
    endif()
  else()
    message(FATAL_ERROR "Failed to checkout to ${dest_dir}: the specified directory already exists")
  endif()
endfunction(checkout_svn)

function(get_from_svn url dest_dir)
  check_svn_access(${url})
  if(NOT GIT_FOUND)
    find_package(Git)
  endif()
  if(NOT GIT_FOUND)
    message(FATAL_ERROR "Git not found")
  endif()
  set(PROJECT_DIR ${CMAKE_CURRENT_SOURCE_DIR}/${dest_dir})
  string(REPLACE "/" "\\" PROJECT_DIR_BACKSLASH ${PROJECT_DIR})
  set(DOWNLOAD_DIR ${PROJECT_DIR}_download)
  if(NOT EXISTS ${PROJECT_DIR})
    checkout_svn(${url} ${dest_dir} ${PROJECT_DIR})
    execute_process(
      COMMAND ${GIT_EXECUTABLE} add ${PROJECT_DIR}_revision.txt
      WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
      OUTPUT_QUIET
    )
  else()
    if(EXISTS ${PROJECT_DIR}_revision.txt)
      #check version
      get_svn_last_changed_revision(${url} remote_svn_revision)
      set(text_read "")
      if(WIN32)
        set(TYPECOMMAND ${PROJECT_DIR_BACKSLASH}_revision.txt)
        execute_process(
          COMMAND cmd /c type ${TYPECOMMAND}
          OUTPUT_VARIABLE text_read
        )
      else()
        set(TYPECOMMAND ${PROJECT_DIR}_revision.txt)
        execute_process(
          COMMAND cat ${TYPECOMMAND}
          OUTPUT_VARIABLE text_read
        )
      endif()
      string(FIND ${text_read} "\n" eol_index)
      string(SUBSTRING ${text_read} 0 ${eol_index} local_svn_revision)
      string(STRIP ${local_svn_revision} local_svn_revision)
    endif()
    if((EXISTS ${PROJECT_DIR}_revision.txt) AND (${local_svn_revision} STREQUAL ${remote_svn_revision}))
      message("${dest_dir} is up-to-date")
    else()
      if(EXISTS ${DOWNLOAD_DIR})
        delete_directory(${DOWNLOAD_DIR})
      endif()
      checkout_svn(${url} ${dest_dir} ${DOWNLOAD_DIR})
      if(EXISTS ${PROJECT_DIR})
        delete_directory(${PROJECT_DIR})
      endif()
      if(WIN32)
        string(REPLACE "/" "\\" DOWNLOAD_DIR_BACKSLASH ${DOWNLOAD_DIR})
        set(RENAMECOMMAND ${DOWNLOAD_DIR_BACKSLASH} ${dest_dir})
        execute_process(
          COMMAND cmd /k rename ${RENAMECOMMAND}
          OUTPUT_QUIET
        )
      else()
        set(RENAMECOMMAND ${DOWNLOAD_DIR} ${PROJECT_DIR})
        execute_process(
          COMMAND mv ${RENAMECOMMAND}
          OUTPUT_QUIET
        )
      endif()
      execute_process(
        COMMAND ${GIT_EXECUTABLE} add ${PROJECT_DIR}
        WORKING_DIRECTORY ${PROJECT_DIR}
        OUTPUT_QUIET
      )
    endif()
  endif()
endfunction(get_from_svn)
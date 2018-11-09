if (NOT MSVC)
  set(WITH_SYSTEM_PNG ON CACHE BOOL "")
endif()

disable_all_warnings()

add_subdirectory(minbase)
add_subdirectory(mingeo)
add_subdirectory(minimgapi)
add_subdirectory(minimgio)
add_subdirectory(minutils)
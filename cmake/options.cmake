# Here we define project-wide options which is recognized by at least
# two libraries.

# Enables unit tests.
# This requires gtest library attached to the build.
OPTION(WITH_TESTS "Turns tests on if enabled." OFF)
IF (WITH_TESTS)
  MESSAGE(STATUS "Unit tests are enabled.")
  OPTION(WITH_TIME_CONSUMING_TESTS "Turns time consuming tests on if enabled." OFF)
  IF (WITH_TIME_CONSUMING_TESTS)
    MESSAGE(STATUS "Time consuming unit tests are enabled.")
  ENDIF (WITH_TIME_CONSUMING_TESTS)
ENDIF (WITH_TESTS)

# Enabled MinLog debug output
option(WITH_MIN_LOG "Enable MinLog traces" OFF)
IF(WITH_MIN_LOG)
  ADD_DEFINITIONS(-DWITH_MIN_LOG)
ENDIF()

# Enables usage of Single Instruction Multiple Data instruction sets
# of the processor, if available.
OPTION(WITH_SIMD "Use SIMD intrinsics." ON)
STRING(TOLOWER ${CMAKE_SYSTEM_PROCESSOR} SYSTEM_PROCESSOR)
MESSAGE(STATUS "System processor (tolower) is ${SYSTEM_PROCESSOR}")
IF (WITH_SIMD)
  IF (${SYSTEM_PROCESSOR} MATCHES "^(i.86|x86|x86_64)$" OR
      ${SYSTEM_PROCESSOR} MATCHES "amd.*64")
    MESSAGE(STATUS "Intel SSE intrinsics are enabled.")
    ADD_DEFINITIONS(-DUSE_SSE_SIMD)

    IF (CMAKE_COMPILER_IS_GNUCXX OR
        CMAKE_COMPILER_IS_GNUC OR
        ${CMAKE_C_COMPILER_ID} MATCHES "Clang" OR
        ${CMAKE_CXX_COMPILER_ID} MATCHES "Clang")
      SET(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -msse -msse2")
      SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -msse -msse2")
    ELSEIF(MSVC AND NOT CMAKE_CL_64)
      # /arch:SSE2 is only valid in 32 bit cl compiler from Microsoft
      ADD_DEFINITIONS(/arch:SSE2)
    ENDIF()
  ENDIF()

  IF (${SYSTEM_PROCESSOR} MATCHES "^arm.*$" OR ${SYSTEM_PROCESSOR} MATCHES "^aarch64$")
    MESSAGE(STATUS "ARM NEON intrinsics are enabled.")
    ADD_DEFINITIONS(-DUSE_NEON_SIMD)
    # enabling -mfpu=neon only for arm.* (not for aarch64)
    IF (${SYSTEM_PROCESSOR} MATCHES "^arm.*$")
      IF (CMAKE_COMPILER_IS_GNUCXX OR
          CMAKE_COMPILER_IS_GNUC OR
          ${CMAKE_C_COMPILER_ID} MATCHES "Clang" OR
          ${CMAKE_CXX_COMPILER_ID} MATCHES "Clang")
        SET(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -mfpu=neon")
        SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -mfpu=neon")
      ENDIF()
    ENDIF()
  ENDIF()
ENDIF()


# Enables support of multi-threaded environment.
OPTION(WITH_MTSAFETY "Support concurrency" ON)
IF (WITH_MTSAFETY)
  ADD_DEFINITIONS(-DWITH_MTSAFETY)
  SET (MTSAFETY_TBB_LIBS tbb)

  IF (MSVC)
    SET(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} /SAFESEH:NO")
  ENDIF()
ENDIF()

# Enables usage of minstopwatch library across the code.
OPTION(WITH_TIMING "Collect execution timing with minstopwatch." OFF)
IF (WITH_TIMING)
  MESSAGE(STATUS "Timing code enabled")
  ADD_DEFINITIONS(-DMINSTOPWATCH_ENABLED)
ENDIF(WITH_TIMING)

# Enables libc++ for Apple platforms and Clang
IF (APPLE OR IOS OR IOS_SIMULATOR)
  OPTION(WITH_LIBCPLUSPLUS   "Use libc++ instead of stdlibc++" ON)
  IF (WITH_LIBCPLUSPLUS)
    IF (${CMAKE_CXX_COMPILER_ID} MATCHES "Clang")
      MESSAGE(STATUS "Using libc++ standard library.")
      SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -stdlib=libc++")
    ENDIF()
  ENDIF()
ENDIF()

# Enables unit tests.
# This requires gtest library attached to the build.
OPTION(WITH_CC_X86 "Turns cross compile for linux x86 with linux amd64" OFF)
IF (WITH_CC_X86 AND (CMAKE_COMPILER_IS_GNUCXX OR CMAKE_COMPILER_IS_GNUC))
  MESSAGE(STATUS "Cross compile for linux x86 enabled")
  SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -m32")
  SET(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -m32")
ENDIF ()

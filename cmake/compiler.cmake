# Default compiler flags.
# These flags are set AFTER toolchain file sets other necessary flags.
IF (CMAKE_CROSSCOMPILING AND IPHONE)
  SET(CMAKE_C_FLAGS "${toolchain_c_flags}")
  SET(CMAKE_CXX_FLAGS "${toolchain_cxx_flags}")
ENDIF()

# Flags for MS Visual Studio.
IF (MSVC)
  ADD_DEFINITIONS(-D_CRT_SECURE_NO_WARNINGS -D_CRT_SECURE_NO_DEPRECATE)

  # Classic workaround to prevent windows.h replace std::min/std::max with
  # its macros.
  ADD_DEFINITIONS(-DNOMINMAX)

  # Workaround MSVC bug with template specialization storage type.
  ADD_DEFINITIONS(-DUSE_STATIC_SPECIAL)

  # Disable inline assembler on x64 compiler.
  IF (CMAKE_CL_64)
    ADD_DEFINITIONS(-DCOMFORT_GNU_COMPILER)
  ENDIF()

  # Disable inline assembler on ARM.
  IF (${CMAKE_SYSTEM_PROCESSOR} MATCHES "^arm.*$")
    ADD_DEFINITIONS(-DCOMFORT_GNU_COMPILER)
  ENDIF()

  # Visual Studio 2012 does not support std::tuple yet.
  ADD_DEFINITIONS(-D_VARIADIC_MAX=10)

# We were said it is FlashCC environment.
ELSEIF (FLASCC_BUILD)
  MESSAGE(STATUS "Setting compiler flags for FlasCC compiler.")

  # Disable MS-specific inline assembler.
  ADD_DEFINITIONS(-DCOMFORT_GNU_COMPILER)

  # Make all libc functions re-entrant.
  ADD_DEFINITIONS(-D_REENTRANT)

  # Debug
  SET(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} ${CMAKE_C_FLAGS}")
  SET(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} -g -ggdb -O0 -g3 -fno-inline -fno-omit-frame-pointer")
  # Release
  SET(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE} ${CMAKE_C_FLAGS}")
  SET(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE} -O4")
  SET(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE} -DNDEBUG")
  # Release with debug info (for profile purposes).
  SET(CMAKE_C_FLAGS_RELWITHDEBINFO "${CMAKE_C_FLAGS_RELWITHDEBINFO} ${CMAKE_C_FLAGS_RELEASE}")
  SET(CMAKE_C_FLAGS_RELWITHDEBINFO "${CMAKE_C_FLAGS_RELWITHDEBINFO} -g -fno-omit-frame-pointer")
  SET(CMAKE_C_FLAGS_RELWITHDEBINFO "${CMAKE_C_FLAGS_RELWITHDEBINFO} -DNDEBUG")

  # CXX stuff is same as C stuff, plus something C++ specific.
  SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${CMAKE_C_FLAGS} -fexceptions")
  SET(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} ${CMAKE_C_FLAGS_DEBUG} -fexceptions")
  SET(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} ${CMAKE_C_FLAGS_RELEASE} -fexceptions")
  SET(CMAKE_CXX_FLAGS_RELWITHDEBINFO "${CMAKE_CXX_FLAGS_RELWITHDEBINFO} ${CMAKE_C_FLAGS_RELWITHDEBINFO} -fexceptions")

# It is GCC or Clang.
ELSEIF (CMAKE_COMPILER_IS_GNUCXX OR
        CMAKE_COMPILER_IS_GNUC OR
        ${CMAKE_C_COMPILER_ID} MATCHES "Clang" OR
        ${CMAKE_CXX_COMPILER_ID} MATCHES "Clang")
  
  # Disable MS-specific inline assembler.
  ADD_DEFINITIONS(-DCOMFORT_GNU_COMPILER)

  # Make all libc functions re-entrant.
  ADD_DEFINITIONS(-D_REENTRANT)

  # Workaround for non-standard behaviour.
  EXECUTE_PROCESS(COMMAND ${CMAKE_C_COMPILER} -dumpversion OUTPUT_VARIABLE GCC_VERSION)
  IF (GCC_VERSION VERSION_LESS 4.3)
    # Since 4.3 GCC conform to 7.1.1 of C++ standard disallowing
    # storage class on explicit template specialisations.
    ADD_DEFINITIONS(-DUSE_STATIC_SPECIAL)
  ENDIF()
  
  # Hide emscripten warnings
  IF (EMSCRIPTEN)
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wno-warn-absolute-paths")
  ENDIF()

  set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fPIC")
  set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -Wextra -Wcast-align")
  set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wno-unused -Wno-missing-field-initializers")
  set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fno-strict-aliasing")
  set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wno-cast-align")
  set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wno-attributes")
  IF(CMAKE_COMPILER_IS_GNUCXX OR CMAKE_COMPILER_IS_GNUC)
     # See http://stackoverflow.com/questions/6687630/c-c-gcc-ld-remove-unused-symbols
     set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -ffunction-sections -fdata-sections")
     set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -Wl,--gc-sections")     
     set(CMAKE_MODULE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -Wl,--gc-sections")     
  ELSEIF(${CMAKE_C_COMPILER_ID} MATCHES "Clang" OR ${CMAKE_CXX_COMPILER_ID} MATCHES "Clang")
     # http://stackoverflow.com/questions/17710024/clang-removing-dead-code-during-static-linking-gcc-equivalent-of-wl-gc-sect
     set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -dead_strip")
     set(CMAKE_MODULE_LINKER_FLAGS "${CMAKE_MODULE_LINKER_FLAGS} -dead_strip")
  ENDIF()

  IF (${CMAKE_SYSTEM_PROCESSOR} MATCHES "^arm.*$")
    SET(ARM_HARD_FP ON CACHE BOOL "HardFP enabled for ARM")
    IF (ARM_HARD_FP)
      message("Enabling hard-fp for ARM")
      SET(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -mfloat-abi=hard")
    ELSE (ARM_HARD_FP)
      message("Enabling soft-fp for ARM")
      SET(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -mfloat-abi=softfp")
    ENDIF (ARM_HARD_FP)
  ENDIF()

  # Debug
  SET(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} ${CMAKE_C_FLAGS}")
  SET(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} -g -ggdb -O0 -g3 -fno-inline -fno-omit-frame-pointer")
  # Release
  SET(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE} ${CMAKE_C_FLAGS}")
  SET(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE} -Os")
  SET(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE} -DNDEBUG")
  # Release with debug info (for profile purposes).
  SET(CMAKE_C_FLAGS_RELWITHDEBINFO "${CMAKE_C_FLAGS_RELWITHDEBINFO} ${CMAKE_C_FLAGS_RELEASE}")
  SET(CMAKE_C_FLAGS_RELWITHDEBINFO "${CMAKE_C_FLAGS_RELWITHDEBINFO} -g -fno-omit-frame-pointer")
  SET(CMAKE_C_FLAGS_RELWITHDEBINFO "${CMAKE_C_FLAGS_RELWITHDEBINFO} -DNDEBUG")

  # CXX stuff is same as C stuff, plus something C++ specific.
  SET(CMAKE_CXX_FLAGS "${CMAKE_C_FLAGS} -fexceptions")
  SET(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} -fexceptions")
  SET(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE} -fexceptions")
  SET(CMAKE_CXX_FLAGS_RELWITHDEBINFO "${CMAKE_C_FLAGS_RELWITHDEBINFO} -fexceptions")

  # enable C++11
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

# It is unknown. Do nothing.
ELSE()

  MESSAGE(WARNING "Unknown compiler. Not using any specific flags. The build will probably fail.")

ENDIF()

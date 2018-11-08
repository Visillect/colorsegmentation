project(minimgapi)

FILE(GLOB MINIMGAPI_PUBLIC_HEADERS
  *.h;
  *.hpp)

FILE(GLOB MINIMGAPI_INTERNAL_HEADERS
  src/*.h;
  src/*.hpp)

FILE(GLOB MINIMGAPI_VECTOR_HEADERS
  src/vector/*.h;
  src/vector/*.hpp)

FILE(GLOB MINIMGAPI_VECTOR_NEON_HEADERS
  src/vector/neon/*.h;
  src/vector/neon/*.hpp)

FILE(GLOB MINIMGAPI_VECTOR_SSE_HEADERS
  src/vector/sse/*.h;
  src/vector/sse/*.hpp)

FILE(GLOB MINIMGAPI_SOURCES
  src/*.c;
  src/*.cpp)

if(BUILD_SHARED_LIBS)
  add_definitions(-DMINIMGAPI_EXPORTS)
endif()

add_library(minimgapi ${MINIMGAPI_SOURCES}
                      ${MINIMGAPI_PUBLIC_HEADERS}
                      ${MINIMGAPI_INTERNAL_HEADERS}
                      ${MINIMGAPI_VECTOR_HEADERS}
                      ${MINIMGAPI_VECTOR_NEON_HEADERS}
                      ${MINIMGAPI_VECTOR_SSE_HEADERS})

source_group("Header Files\\vector" FILES ${MINIMGAPI_VECTOR_HEADERS})
source_group("Header Files\\vector\\neon" FILES ${MINIMGAPI_VECTOR_NEON_HEADERS})
source_group("Header Files\\vector\\sse" FILES ${MINIMGAPI_VECTOR_SSE_HEADERS})

target_link_libraries(minimgapi minutils)

if (USE_ELBRUS_SIMD)
  target_link_libraries(minimgapi eml)
endif()

if (WITH_TIMING)
  target_link_libraries(minimgapi minstopwatch)
endif()

install(FILES ${MINIMGAPI_PUBLIC_HEADERS}
  DESTINATION include/minimgapi)
install(TARGETS minimgapi
  RUNTIME DESTINATION bin
  LIBRARY DESTINATION lib
  ARCHIVE DESTINATION lib)

IF (WITH_TESTS AND NOT ANDROID) # clang crash strikes again
  ADD_EXECUTABLE(test_internal_transpose test/test_internal_transpose.cpp)
  TARGET_LINK_LIBRARIES(test_internal_transpose minimgapi gtest)

  ADD_EXECUTABLE(test_minimgapi test/test_minimgapi.cpp)
  TARGET_LINK_LIBRARIES(test_minimgapi minimgapi minutils gtest)
  ADD_TEST(
    NAME test_minimgapi
    COMMAND test_minimgapi)
ENDIF()

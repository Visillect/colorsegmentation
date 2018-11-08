project(minutils)

if(WITH_TESTS)
  add_definitions(-DUNIT_TEST)
endif(WITH_TESTS)

FILE(GLOB MINUTILS_PUBLIC_HEADERS
  *.h;
  *.hpp)

FILE(GLOB MINUTILS_INTERNAL_HEADERS
  src/*.h;
  src/*.hpp)

FILE(GLOB MINUTILS_SOURCES
  src/*.c;
  src/*.cpp)


if(MSVC)
  set(MINUTILS_INTERNAL_HEADERS
      ${MINUTILS_INTERNAL_HEADERS} stdint-vc.h stdint-doc.h)
endif()


add_definitions(-DMINUTILS_EXPORTS)
add_library(minutils STATIC
            ${MINUTILS_SOURCES} ${MINUTILS_INTERNAL_HEADERS} ${MINUTILS_PUBLIC_HEADERS})

# Add logging support on Android
if (ANDROID)
  target_link_libraries(minutils log)
endif()
	  
install(FILES ${MINUTILS_PUBLIC_HEADERS}
  DESTINATION include/minutils)
install(TARGETS minutils
  RUNTIME DESTINATION bin
  LIBRARY DESTINATION lib
  ARCHIVE DESTINATION lib)

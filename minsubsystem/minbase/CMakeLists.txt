project(minbase)

if(WITH_TESTS)
  add_definitions(-DUNIT_TEST)
endif(WITH_TESTS)

set(MINBASE_PUBLIC_HEADERS
  macro_helpers.h
  crossplat.h
  mintyp.h
  minimg.h
  mingeo.h
  minresult.h
  half.hpp)

set(MINBASE_INTERNAL_HEADERS
  src/gcc_warnings.h)

add_library(minbase STATIC
  src/minbase.cpp
  ${MINBASE_INTERNAL_HEADERS}
  ${MINBASE_PUBLIC_HEADERS})

install(FILES ${MINBASE_PUBLIC_HEADERS}
  DESTINATION include/minbase)
install(TARGETS minbase
  RUNTIME DESTINATION bin
  LIBRARY DESTINATION lib
  ARCHIVE DESTINATION lib)

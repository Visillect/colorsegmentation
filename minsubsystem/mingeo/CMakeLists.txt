project(mingeo)

set(MINGEO_PUBLIC_HEADERS
  mingeo.h)

add_library(mingeo STATIC
  src/mingeo.cpp
  ${MINGEO_PUBLIC_HEADERS})

install(FILES ${MINGEO_PUBLIC_HEADERS}
  DESTINATION include/mingeo)
install(TARGETS mingeo
  RUNTIME DESTINATION bin
  LIBRARY DESTINATION lib
  ARCHIVE DESTINATION lib)

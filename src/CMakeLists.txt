project(POGLIB)
set(CMAKE_CXX_STANDARD 17)

include_directories(
  ${BASTLIB_SOURCE_DIR}
  ${tinyxml2_SOURCE_DIR}
)

set(POGLIB_HEADERS pog.h pogXmlWriter.h)
set(POGLIB_SOURCES pog.cpp pogXmlWriter.cpp)

add_library(POGLIB STATIC ${POGLIB_SOURCES} ${POGLIB_HEADERS})

target_link_libraries(POGLIB PRIVATE BAST_LIB)

project(POGLIB)
set(CMAKE_CXX_STANDARD 17)

include_directories(
        ${CMAKE_CURRENT_BINARY_DIR}
        ${BAST_SOURCE_DIR}
)

set(POGLIB_HEADERS pog.h pogXmlWriter.h)

set(POGLIB_SOURCES pog.cpp pogXmlWriter.cpp)

include_directories(
    ${CMAKE_CURRENT_BINARY_DIR}
	${LIBBAST_SOURCE_DIR}
)

add_library(POGLIB STATIC ${POGLIB_SOURCES} ${POGLIB_HEADERS})
include_directories(${BASTLIB_SOURCE_DIR})

target_link_libraries(POGLIB PRIVATE BAST tinyxml2::tinyxml2)
target_link_libraries(BAST PRIVATE)

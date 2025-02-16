project(POGLoader)
set(CMAKE_CXX_STANDARD 17)


set(POG_HEADERS pog.h pogXmlWriter.h)

set(POG_SOURCES pog.cpp pogXmlWriter.cpp)

include_directories(
    ${CMAKE_CURRENT_BINARY_DIR}
	${LIBBAST_SOURCE_DIR}
)

add_library(POGLoader_LIB STATIC ${POG_SOURCES} ${POG_HEADERS})
include_directories(${BASTLIB_SOURCE_DIR})

target_link_libraries(POGLoader_LIB PRIVATE BAST tinyxml2::tinyxml2)

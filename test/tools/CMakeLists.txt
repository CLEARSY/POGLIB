cmake_minimum_required(VERSION 3.15)

project(loadpog_test)

include_directories(
    ${CMAKE_CURRENT_BINARY_DIR}
    ${POGLIB_SOURCE_DIR}
	${BASTLIB_SOURCE_DIR}
)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR})

add_executable(loadpog loadpog.cpp)

get_target_property(LOADPOG_OUTPUT_DIRECTORY loadpog RUNTIME_OUTPUT_DIRECTORY)

target_link_libraries(loadpog PRIVATE POGLIB BAST_LIB)
set(loadpog_EXE ${LOADPOG_OUTPUT_DIRECTORY}/loadpog${CMAKE_EXECUTABLE_SUFFIX} CACHE PATH "loadpog executable")

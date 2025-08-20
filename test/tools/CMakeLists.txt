cmake_minimum_required(VERSION 3.15)

project(loadpog_test)

include_directories(
    ${CMAKE_CURRENT_BINARY_DIR}
    ${POGLIB_SOURCE_DIR}
	${BASTLIB_SOURCE_DIR}
)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})

add_executable(loadpog loadpog.cpp)

get_target_property(LOADPOG_OUTPUT_DIRECTORY loadpog RUNTIME_OUTPUT_DIRECTORY)

target_link_libraries(loadpog PRIVATE POGLIB BAST_LIB tinyxml2::tinyxml2)
set(loadpog_EXE ${CMAKE_CURRENT_BINARY_DIR}/loadpog${CMAKE_EXECUTABLE_SUFFIX} CACHE PATH "loadpog executable")
message(STATUS "loadpog executable: ${loadpog_EXE}")
message(STATUS "LOADPOG_OUTPUT_DIRECTORY: ${LOADPOG_OUTPUT_DIRECTORY}")

cmake_minimum_required(VERSION 3.16)
project(pog-translators)

# require the compiler to use C++17
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# configure compilers
if(CMAKE_CXX_COMPILER_ID STREQUAL GNU OR CMAKE_CXX_COMPILER_ID MATCHES Clang$)
  add_compile_options(-Wall)
  add_compile_options(-fno-threadsafe-statics)
endif()

# compile command database
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# set build type if not set
if(NOT CMAKE_BUILD_TYPE)
  message(STATUS "Setting build type to 'Release' as none was specified.")
  set(CMAKE_BUILD_TYPE Release CACHE STRING "Choose the type of build." FORCE)
endif()

if(${CMAKE_PROJECT_NAME} STREQUAL ${PROJECT_NAME})
  set(PROJECT_IS_TOP_LEVEL TRUE)
else()
  set(PROJECT_IS_TOP_LEVEL FALSE)
endif()

foreach(submodule
"${PROJECT_SOURCE_DIR}/extern/BAST"
"${PROJECT_SOURCE_DIR}/extern/tinyxml2")
if(NOT EXISTS "${submodule}/CMakeLists.txt")
    message(FATAL_ERROR "Submodule ${submodule} is missing: update submodules and try again.")
    message(FATAL_ERROR "Command : git submodule update --init --recursive.")
endif()
endforeach(submodule)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_SOURCE_DIR}/CMakeModules)

add_subdirectory(extern/tinyxml2)
add_subdirectory(extern/BAST)
add_subdirectory(src)

if(PROJECT_IS_TOP_LEVEL)
  include(CTest)
  add_subdirectory(test)
endif()

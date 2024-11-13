cmake_minimum_required(VERSION 3.21)

project(
  subpalindromes
  DESCRIPTION "Find all subpalindromes in the text"
  LANGUAGES CXX
)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(CMAKE_CXX_EXTENSIONS False)

set(CMAKE_CXX_FLAGS "-Wall -Wextra")
set(CMAKE_CXX_FLAGS_DEBUG "-g -O0")
set(CMAKE_CXX_FLAGS_RELEASE "-O2")

if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Release)
  message(STATUS "CMAKE_BUILD_TYPE is not specified, using Release by default")
endif()

set(SRC_DIR src)
set(INC_DIR inc)

file(GLOB SOURCES CONFIGURE_DEPENDS ${SRC_DIR}/*.cpp)

add_executable(manaker ${SOURCES})

add_executable(trivial ${SOURCES})
target_compile_definitions(trivial PRIVATE TRIVIAL=1)

target_include_directories(manaker PRIVATE ${INC_DIR})
target_include_directories(trivial PRIVATE ${INC_DIR})

option(VERBOSE "Additional prints of debug information" OFF)
if (VERBOSE)
  target_compile_definitions(manaker PRIVATE VERBOSE=1)
  target_compile_definitions(trivial VERBOSE=1)
endif()

option(TIME "Additional prints of elapsed time" OFF)
if (TIME)
  target_compile_definitions(manaker PRIVATE TIME=1)
  target_compile_definitions(trivial PRIVATE TIME=1)
endif()

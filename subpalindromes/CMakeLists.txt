set(CMAKE_CXX_COMPILER "/opt/homebrew/opt/llvm/bin/clang++")
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

#----MANAKER----
add_executable(manaker ${SOURCES})

#----TRIVIAL----
add_executable(trivial ${SOURCES})
target_compile_definitions(trivial PRIVATE TRIVIAL=1)

#---PARALLEL----
add_executable(parallel ${SOURCES})
target_compile_options(parallel PUBLIC "-fopenmp")
target_link_options(parallel PUBLIC "-fopenmp")
target_compile_definitions(parallel PRIVATE TRIVIAL=1 PARALLEL=1)

#----COMMON-----
set(target_list manaker trivial parallel)

foreach(TARGET ${target_list})
  target_include_directories(${TARGET} PRIVATE ${INC_DIR})
endforeach(TARGET)

option(VERBOSE "Additional prints of debug information" OFF)
if (VERBOSE)
  foreach(TARGET ${target_list})
    target_compile_definitions(${TARGET} PRIVATE VERBOSE=1)
  endforeach(TARGET)
endif()

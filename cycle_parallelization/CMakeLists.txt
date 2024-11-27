cmake_minimum_required(VERSION 3.21)

project(
  cycle_parallelization
  DESCRIPTION "Cycle parallelization"
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
set(target_list task01 task02 task03)

foreach(TARGET ${target_list})
  add_executable(${TARGET} ${SRC_DIR}/${TARGET}.cpp)
endforeach(TARGET)

option(PARALLEL "Parallel version of the programm" OFF)
if (PARALLEL)
  foreach(TARGET ${target_list})
    target_compile_definitions(${TARGET} PRIVATE PARALLEL=1)
  endforeach(TARGET)
endif()

option(TIMING "Measure execution time" OFF)
if (TIMING)
  foreach(TARGET ${target_list})
    target_compile_definitions(${TARGET} PRIVATE TIMING=1)
  endforeach(TARGET)
endif()

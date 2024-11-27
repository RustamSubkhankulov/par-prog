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
set(INC_DIR inc)

set(target_list task01 task02 task03)

#------COMMON------

foreach(TARGET ${target_list})
  add_executable(${TARGET} ${SRC_DIR}/${TARGET}.cpp)
  target_include_directories(${TARGET} PUBLIC ${INC_DIR})
endforeach(TARGET)

option(PARALLEL "Parallel version of the programm" OFF)
option(TIMING "Measure execution time" OFF)
option(QUIET "Disable result printing" OFF)

set(options_list PARALLEL TIMING QUIET)

foreach(TARGET ${target_list})
  foreach(OPTION ${options_list})
    if (${OPTION})
      target_compile_definitions(${TARGET} PRIVATE ${OPTION}=1)
    endif()
  endforeach(OPTION)
endforeach(TARGET)

#------TASK01------

target_compile_options(task01 PUBLIC "-fopenmp")
target_link_options(task01 PUBLIC "-fopenmp")

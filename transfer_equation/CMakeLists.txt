cmake_minimum_required(VERSION 3.21)

project(
  transfer_eq
  DESCRIPTION "Solving transfer equation"
  LANGUAGES CXX
)

set(CMAKE_CXX_COMPILER "mpic++")

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(CMAKE_CXX_EXTENSIONS False)

if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Release)
  message(STATUS "CMAKE_BUILD_TYPE is not specified, using Release by default")
endif()

set(CMAKE_CXX_FLAGS "-Wall -Wextra")
set(CMAKE_CXX_FLAGS_DEBUG "-g -O0")
set(CMAKE_CXX_FLAGS_RELEASE "-O0")

set(SRC_DIR src)
set(INC_DIR inc)

set(SEQ_SRC ${SRC_DIR}/sequential.cpp)
set(PAR_SRC ${SRC_DIR}/parallel.cpp ${SRC_DIR}/mpi_support.cpp)

add_executable(sequential ${SEQ_SRC})
target_include_directories(sequential PRIVATE ${INC_DIR})

add_executable(parallel ${PAR_SRC})
target_include_directories(parallel PRIVATE ${INC_DIR})

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
set(CMAKE_CXX_FLAGS_RELEASE "-O3")

set(SRC_DIR src)
set(INC_DIR inc)

set(MPI_ADD_SOURCES ${SRC_DIR}/mpi_support.cpp)

add_executable(sequential ${SRC_DIR}/sequential.cpp)
add_executable(parallel ${SRC_DIR}/parallel.cpp ${MPI_ADD_SOURCES})

target_include_directories(sequential PRIVATE ${INC_DIR})
target_include_directories(parallel PRIVATE ${INC_DIR})

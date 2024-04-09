cmake_minimum_required(VERSION 3.21)

project(
        basic
        DESCRIPTION "Basic MPI usage example"
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

file(GLOB SOURCES CONFIGURE_DEPENDS ${SRC_DIR}/*.cpp)

add_executable(${PROJECT_NAME} ${SOURCES})
target_include_directories(${PROJECT_NAME} PRIVATE ${INC_DIR})

set(SRC_DIR src)
set(INC_DIR inc)

set(CMAKE_CXX_COMPILER mpic++)

set(target_list task01)

#------COMMON------

foreach(TARGET ${target_list})
  set (TARGET_NAME ${TARGET}_MPI)
  add_executable(${TARGET_NAME} ${SRC_DIR}/${TARGET}.cpp ${SRC_DIR}/mpi_support.cpp)
  target_include_directories(${TARGET_NAME} PUBLIC ${INC_DIR})
endforeach(TARGET)

option(PARALLEL "Parallel version of the programm" OFF)
option(TIMING "Measure execution time" OFF)
option(QUIET "Disable result printing" OFF)

set(options_list PARALLEL TIMING QUIET)

foreach(TARGET ${target_list})
  foreach(OPTION ${options_list})
    if (${OPTION})
      set (TARGET_NAME ${TARGET}_MPI)
      target_compile_definitions(${TARGET_NAME} PRIVATE ${OPTION}=1)
    endif()
  endforeach(OPTION)
endforeach(TARGET)

set(SRC_DIR src)
set(INC_DIR inc)

set(target_list task01 task02)

#------COMMON------

option(PARALLEL "Parallel version of the programm" OFF)

foreach(TARGET ${target_list})
  set (TARGET_NAME ${TARGET}_OMP)
  add_executable(${TARGET_NAME} ${SRC_DIR}/${TARGET}.cpp)
  target_include_directories(${TARGET_NAME} PUBLIC ${INC_DIR})

  if (PARALLEL)
    target_compile_options(${TARGET_NAME} PUBLIC "-fopenmp")
    target_link_options(${TARGET_NAME} PUBLIC "-fopenmp")
  endif(PARALLEL)
endforeach(TARGET)

option(TIMING "Measure execution time" OFF)
option(QUIET "Disable result printing" OFF)

set(options_list PARALLEL TIMING QUIET)

foreach(TARGET ${target_list})
  foreach(OPTION ${options_list})
    if (${OPTION})
      set (TARGET_NAME ${TARGET}_OMP)
      target_compile_definitions(${TARGET_NAME} PRIVATE ${OPTION}=1)
    endif()
  endforeach(OPTION)
endforeach(TARGET)

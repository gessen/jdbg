if(JDBG_ENABLE_COVERAGE)
  if(NOT CMAKE_BUILD_TYPE STREQUAL "Debug")
    message(WARNING
      "Code coverage results with an optimised build may be misleading"
    )
  endif()
endif()

function(_setup_coverage_html target exec args output_dir)
  set(output_file "${output_dir}/index.html")
  list(APPEND args --output="${output_file}")
  list(APPEND args --html-nested)

  add_custom_target(${target}-html
    COMMAND ${exec} ${args}
    WORKING_DIRECTORY ${PROJECT_BINARY_DIR}
    COMMENT "Running gcovr to produce an HTML report"
  )
  add_custom_command(TARGET ${target}-html POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E cmake_echo_color --yellow
    "Open file://${output_file} to see an HTML coverage report"
  )
endfunction()

function(_setup_coverage_text target exec args output_dir)
  set(output_file "${output_dir}/coverage.txt")
  list(APPEND args --output="${output_file}")
  list(APPEND args --txt)

  add_custom_target(${target}-text
    COMMAND ${exec} ${args}
    WORKING_DIRECTORY ${PROJECT_BINARY_DIR}
    COMMENT "Running gcovr to produce a text report"
  )
  add_custom_command(TARGET ${target}-html POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E cmake_echo_color --yellow
    "Open file://${output_file} to see a text coverage report"
  )
endfunction()

function(setup_coverage)
  if(CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
    find_package(LlvmCov)
    if(LlvmCov_FOUND)
      set(cov_executable "${LLVM_COV_EXECUTABLE} gcov")
    endif()
  else()
    find_package(GCov)
    if(GCov_FOUND)
      set(cov_executable "${GCOV_EXECUTABLE}")
    endif()
  endif()

  if(TARGET coverage)
    set(coverage_target ${PROJECT_NAME}-coverage)
  else()
    set(coverage_target coverage)
  endif()

  find_package(GCovr 4.2)

  if(NOT cov_executable OR NOT GCovr_FOUND)
    message(WARNING "${coverage_target} target will be unavailable")
    return()
  endif()

  set(output_dir "${PROJECT_BINARY_DIR}/coverage")
  file(MAKE_DIRECTORY "${output_dir}")

  set(gcovr_args)
  list(APPEND gcovr_args --gcov-executable=${cov_executable})
  list(APPEND gcovr_args --root=${PROJECT_SOURCE_DIR})
  include(ProcessorCount)
  ProcessorCount(N)
  if(NOT N EQUAL 0)
    list(APPEND gcovr_args -j${N})
  endif()
  list(APPEND gcovr_args ${PROJECT_BINARY_DIR})

  _setup_coverage_html("${coverage_target}" "${GCOVR_EXECUTABLE}" "${gcovr_args}" "${output_dir}")
  _setup_coverage_text("${coverage_target}" "${GCOVR_EXECUTABLE}" "${gcovr_args}" "${output_dir}")

  add_custom_target(${coverage_target}
    DEPENDS ${coverage_target}-html ${coverage_target}-text
  )
endfunction()

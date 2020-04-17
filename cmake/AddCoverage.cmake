if(JDBG_ENABLE_COVERAGE)
  if(NOT CMAKE_BUILD_TYPE STREQUAL "Debug")
    message(WARNING
      "Code coverage results with an optimised build may be misleading"
    )
  endif()
endif()

function(_setup_coverage_lcov coverage_target check_target cov_executable)
  if(coverage_set)
    return()
  endif()

  # Clang does not work well with lcov
  if(CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
    return()
  endif()

  find_program(LCOV_EXECUTABLE NAMES lcov)
  find_program(GENHTML_EXECUTABLE NAMES genhtml)
  include(FindPackageHandleStandardArgs)
  find_package_handle_standard_args(LCov
    REQUIRED_VARS LCOV_EXECUTABLE GENHTML_EXECUTABLE
  )
  mark_as_advanced(LCOV_EXECUTABLE GENHTML_EXECUTABLE)
  if(NOT LCov_FOUND)
    return()
  endif()

  set(options "")
  set(one_value_args "")
  set(multi_value_args FILTERS EXCLUDE)

  cmake_parse_arguments(options
    "${options}" "${one_value_args}" "${multi_value_args}" ${ARGN}
  )

  if(options_FILTERS)
    set(tmp "")
    foreach(filter IN LISTS options_FILTERS)
      list(APPEND tmp ${filter})
    endforeach()
    set(options_FILTERS ${tmp})
  else()
    set(options_FILTERS '*')
  endif()

  if(options_EXCLUDE)
    set(tmp "")
    foreach(exclude IN LISTS options_EXCLUDE)
      list(APPEND tmp --exclude=${exclude})
    endforeach()
    set(options_EXCLUDE ${tmp})
  endif()

  set(output bin/coverage)

  set(lcov_args_capture)
  list(APPEND lcov_args_capture --capture)
  list(APPEND lcov_args_capture --gcov-tool=${GCOV_EXECUTABLE})
  list(APPEND lcov_args_capture --output-file=${output}.info)
  list(APPEND lcov_args_capture --directory=${PROJECT_BINARY_DIR})
  list(APPEND lcov_args_capture --quiet)

  set(lcov_args_extract)
  list(APPEND lcov_args_extract --extract=${output}.info)
  list(APPEND lcov_args_extract ${options_EXCLUDE})
  list(APPEND lcov_args_extract ${options_FILTERS})
  list(APPEND lcov_args_extract --gcov-tool=${GCOV_EXECUTABLE})
  list(APPEND lcov_args_extract --output-file=${output}.info)
  list(APPEND lcov_args_extract --directory=${PROJECT_BINARY_DIR})
  list(APPEND lcov_args_extract --quiet)

  set(lcov_args_zerocounters)
  list(APPEND lcov_args_zerocounters --zerocounters)
  list(APPEND lcov_args_zerocounters --directory=${PROJECT_BINARY_DIR})
  list(APPEND lcov_args_zerocounters --quiet)

  set(genhtml_args)
  list(APPEND genhtml_args ${output}.info)
  list(APPEND genhtml_args --output-directory=${output})
  list(APPEND genhtml_args --quiet)

  add_custom_target(${coverage_target}
    COMMAND ${LCOV_EXECUTABLE} ${lcov_args_capture}
    COMMAND ${LCOV_EXECUTABLE} ${lcov_args_extract}
    COMMAND ${LCOV_EXECUTABLE} ${lcov_args_zerocounters}
    COMMAND ${GENHTML_EXECUTABLE} ${genhtml_args}
    COMMAND ${CMAKE_COMMAND} -E remove -f ${output}.info
    WORKING_DIRECTORY ${PROJECT_BINARY_DIR}
    COMMENT "Running lcov and genhtml to produce a report"
  )
  add_custom_command(TARGET ${check_target} POST_BUILD
    COMMAND ${LCOV_EXECUTABLE} ${lcov_args_capture}
    COMMAND ${LCOV_EXECUTABLE} ${lcov_args_extract}
    COMMAND ${LCOV_EXECUTABLE} ${lcov_args_zerocounters}
    WORKING_DIRECTORY ${PROJECT_BINARY_DIR}
    COMMENT "Gathering code coverage counters"
  )
  add_custom_command(TARGET ${check_target} POST_BUILD
    COMMAND ${GENHTML_EXECUTABLE} ${genhtml_args}
    COMMAND ${CMAKE_COMMAND} -E remove -f ${output}.info
    WORKING_DIRECTORY ${PROJECT_BINARY_DIR}
    COMMENT "Running genhtml to produce a report"
  )
  foreach(target IN ITEMS ${coverage_target} ${check_target})
    add_custom_command(TARGET ${target} POST_BUILD
      COMMAND ${CMAKE_COMMAND} -E cmake_echo_color --yellow
      "Open ${PROJECT_BINARY_DIR}/${output}/index.html to see coverage report"
    )
  endforeach()
  set(coverage_set TRUE PARENT_SCOPE)
endfunction()

function(_setup_coverage_gcovr coverage_target check_target cov_executable)
  if(coverage_set)
    return()
  endif()

  find_program(GCOVR_EXECUTABLE NAMES gcovr)
  include(FindPackageHandleStandardArgs)
  find_package_handle_standard_args(GCovr REQUIRED_VARS GCOVR_EXECUTABLE)
  mark_as_advanced(GCOVR_EXECUTABLE)
  if(NOT GCovr_FOUND)
    return()
  endif()

  set(options XML)
  set(one_value_args "")
  set(multi_value_args FILTERS EXCLUDE)

  cmake_parse_arguments(options
    "${options}" "${one_value_args}" "${multi_value_args}" ${ARGN}
  )

  if(options_FILTERS)
    set(tmp "")
    foreach(filter IN LISTS options_FILTERS)
      list(APPEND tmp --filter=${filter})
    endforeach()
    set(options_FILTERS ${tmp})
  endif()

  if(options_EXCLUDE)
    set(tmp "")
    foreach(exclude IN LISTS options_EXCLUDE)
      list(APPEND tmp --exclude=${exclude})
    endforeach()
    set(options_EXCLUDE ${tmp})
  endif()

  if(options_XML)
    set(output_mode --xml)
    set(output_file bin/coverage.xml)
  else()
    set(output_mode --html-details)
    set(output_file bin/coverage.html)
  endif()

  set(gcovr_args)
  list(APPEND gcovr_args --gcov-executable=${cov_executable})
  list(APPEND gcovr_args --root=${PROJECT_SOURCE_DIR})
  list(APPEND gcovr_args --object-directory=${PROJECT_BINARY_DIR})
  list(APPEND gcovr_args ${options_FILTERS})
  list(APPEND gcovr_args ${options_EXCLUDE})
  list(APPEND gcovr_args ${output_mode})
  list(APPEND gcovr_args --output=${output_file})
  list(APPEND gcovr_args --delete)
  list(APPEND gcovr_args --print-summary)
  list(APPEND gcovr_args --exclude-unreachable-branches)

  add_custom_target(${coverage_target}
    COMMAND ${GCOVR_EXECUTABLE} ${gcovr_args}
    WORKING_DIRECTORY ${PROJECT_BINARY_DIR}
    COMMENT "Running gcovr to produce a report"
  )
  add_custom_command(TARGET ${check_target} POST_BUILD
    COMMAND ${GCOVR_EXECUTABLE} ${gcovr_args}
    WORKING_DIRECTORY ${PROJECT_BINARY_DIR}
    COMMENT "Running gcovr to produce a report"
  )
  foreach(target IN ITEMS ${coverage_target} ${check_target})
    add_custom_command(TARGET ${target} POST_BUILD
      COMMAND ${CMAKE_COMMAND} -E cmake_echo_color --yellow
      "Open ${PROJECT_BINARY_DIR}/${output_file} to see coverage report"
    )
  endforeach()
  set(coverage_set TRUE PARENT_SCOPE)
endfunction()

function(setup_coverage check_target)
  include(FindPackageHandleStandardArgs)
  if(CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
    find_program(LLVM_COV_EXECUTABLE NAMES llvm-cov)
    find_package_handle_standard_args(LLVMCov REQUIRED_VARS LLVM_COV_EXECUTABLE)
    mark_as_advanced(LLVM_COV_EXECUTABLE)
    if(LLVMCov_FOUND)
      set(cov_executable "${LLVM_COV_EXECUTABLE} gcov")
    endif()
  endif()

  if(NOT cov_executable)
    find_program(GCOV_EXECUTABLE NAMES gcov)
    find_package_handle_standard_args(GCov REQUIRED_VARS GCOV_EXECUTABLE)
    mark_as_advanced(GCOV_EXECUTABLE)
    if(GCov_FOUND)
      set(cov_executable "${GCOV_EXECUTABLE}")
    endif()
  endif()

  if(TARGET coverage)
    set(coverage_target ${PROJECT_NAME}-coverage)
  else()
    set(coverage_target coverage)
  endif()

  if(NOT cov_executable)
    message(WARNING "${coverage_target} target will be unavailable")
    return()
  endif()

  set(coverage_set FALSE)
  _setup_coverage_lcov(
    ${coverage_target} ${check_target} ${cov_executable} ${ARGN}
  )
  _setup_coverage_gcovr(
    ${coverage_target} ${check_target} ${cov_executable} ${ARGN}
  )

  if(NOT coverage_set)
    message(WARNING "${coverage_target} target will be unavailable")
    return()
  endif()
endfunction()

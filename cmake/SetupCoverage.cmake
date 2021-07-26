if(JDBG_ENABLE_COVERAGE)
  if(NOT CMAKE_BUILD_TYPE STREQUAL "Debug")
    message(WARNING
      "Code coverage results with an optimised build may be misleading"
    )
  endif()
endif()

function(_normalise_paths_gcovr_realpath paths)
  set(new_paths)
  foreach(path IN LISTS ${paths})
    if(NOT IS_ABSOLUTE "${path}")
      list(APPEND new_paths "${path}")
      continue()
    endif()

    # Get rid of trailing slash with directories
    file(TO_CMAKE_PATH "${path}" path)
    # Resolve symlinks
    get_filename_component(path ${path} REALPATH)

    list(APPEND new_paths "${path}")
  endforeach()
  set(${paths} ${new_paths} PARENT_SCOPE)
endfunction()

function(_normalise_paths_gcovr_regex paths)
  set(new_paths)
  foreach(path IN LISTS ${paths})
    # Escape special chars
    string(REPLACE "+" "\\+" path ${path})
    string(REPLACE "!" "\\!" path ${path})
    string(REPLACE "|" "\\|" path ${path})
    string(REPLACE "," "\\," path ${path})
    string(REPLACE "." "\\." path ${path})
    string(REPLACE "(" "\\(" path ${path})
    string(REPLACE ")" "\\)" path ${path})
    string(REPLACE "[" "\\[" path ${path})
    string(REPLACE "]" "\\]" path ${path})
    string(REPLACE "{" "\\{" path ${path})
    string(REPLACE "}" "\\}" path ${path})
    string(REPLACE "^" "\\^" path ${path})
    string(REPLACE "$" "\\$" path ${path})

    # Transform ? => (.) and * => (.*)
    string(REPLACE "?" "(.)"  path ${path})
    string(REPLACE "*" "(.*)" path ${path})

    list(APPEND new_paths "${path}")
  endforeach()
  set(${paths} ${new_paths} PARENT_SCOPE)
endfunction()

function(_setup_coverage_lcov coverage_target cov_executable)
  if(coverage_set)
    return()
  endif()

  # Clang does not work well with lcov
  if(CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
    return()
  endif()

  find_package(LCov 1.14)
  if(NOT LCov_FOUND)
    return()
  endif()

  set(options "")
  set(one_value_args POST_TARGET)
  set(multi_value_args FILTERS EXCLUDE)

  cmake_parse_arguments(options
    "${options}" "${one_value_args}" "${multi_value_args}" ${ARGN}
  )

  if(options_FILTERS)
    set(tmp "")
    foreach(filter IN LISTS options_FILTERS)
      # If the entry is a directory append '*'. Entries with trailing '*' are
      # not considered directories so they're safe from this operation.
      if(IS_DIRECTORY "${filter}")
        list(APPEND tmp --include=${filter}/*)
      else()
        list(APPEND tmp --include=${filter})
      endif()
    endforeach()
    set(options_FILTERS ${tmp})
  else()
    set(options_FILTERS --include='*')
  endif()

  if(options_EXCLUDE)
    set(tmp "")
    foreach(exclude IN LISTS options_EXCLUDE)
      # If the entry is a directory append '*'. Entries with trailing '*' are
      # not considered directories so they're safe from this operation.
      if(IS_DIRECTORY "${exclude}")
        list(APPEND tmp --exclude=${exclude}/*)
      else()
        list(APPEND tmp --exclude=${exclude})
      endif()
    endforeach()
    set(options_EXCLUDE ${tmp})
  endif()

  if(JDBG_REPORTS_DIR)
    set(output "${JDBG_REPORTS_DIR}/coverage")
  else()
    set(output "${PROJECT_BINARY_DIR}/bin/coverage")
  endif()
  file(MAKE_DIRECTORY "${output}")

  set(lcov_args_capture)
  list(APPEND lcov_args_capture --capture)
  list(APPEND lcov_args_capture --gcov-tool=${cov_executable})
  list(APPEND lcov_args_capture --output-file=${output}.info)
  list(APPEND lcov_args_capture --directory=${PROJECT_BINARY_DIR}/)
  list(APPEND lcov_args_capture ${options_FILTERS})
  list(APPEND lcov_args_capture ${options_EXCLUDE})
  list(APPEND lcov_args_capture --quiet)

  set(lcov_args_zerocounters)
  list(APPEND lcov_args_zerocounters --zerocounters)
  list(APPEND lcov_args_zerocounters --directory=${PROJECT_BINARY_DIR}/)
  list(APPEND lcov_args_zerocounters --quiet)

  set(genhtml_args)
  list(APPEND genhtml_args ${output}.info)
  list(APPEND genhtml_args --output-directory=${output})
  list(APPEND genhtml_args --quiet)

  add_custom_target(${coverage_target}
    COMMAND ${LCOV_EXECUTABLE} ${lcov_args_capture}
    COMMAND ${LCOV_EXECUTABLE} ${lcov_args_zerocounters}
    COMMAND ${GENHTML_EXECUTABLE} ${genhtml_args}
    COMMAND ${CMAKE_COMMAND} -E remove -f ${output}.info
    WORKING_DIRECTORY ${PROJECT_BINARY_DIR}
    COMMENT "Running lcov and genhtml to produce a report"
  )
  add_custom_command(TARGET ${coverage_target} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E cmake_echo_color --yellow
    "Open ${output}/index.html to see coverage report"
  )
  if(TARGET ${options_POST_TARGET})
    add_custom_command(TARGET ${options_POST_TARGET} POST_BUILD
      COMMAND ${LCOV_EXECUTABLE} ${lcov_args_capture}
      COMMAND ${LCOV_EXECUTABLE} ${lcov_args_zerocounters}
      WORKING_DIRECTORY ${PROJECT_BINARY_DIR}
      COMMENT "Gathering code coverage counters"
    )
    add_custom_command(TARGET ${options_POST_TARGET} POST_BUILD
      COMMAND ${GENHTML_EXECUTABLE} ${genhtml_args}
      COMMAND ${CMAKE_COMMAND} -E remove -f ${output}.info
      WORKING_DIRECTORY ${PROJECT_BINARY_DIR}
      COMMENT "Running genhtml to produce a report"
    )
    add_custom_command(TARGET ${options_POST_TARGET} POST_BUILD
      COMMAND ${CMAKE_COMMAND} -E cmake_echo_color --yellow
      "Open ${output}/index.html to see coverage report"
    )
  endif()
  set(coverage_set TRUE PARENT_SCOPE)
endfunction()

function(_setup_coverage_gcovr coverage_target cov_executable)
  if(coverage_set)
    return()
  endif()

  find_package(GCovr 4.2)
  if(NOT GCovr_FOUND)
    return()
  endif()

  set(options "")
  set(one_value_args POST_TARGET)
  set(multi_value_args FILTERS EXCLUDE)

  cmake_parse_arguments(options
    "${options}" "${one_value_args}" "${multi_value_args}" ${ARGN}
  )

  _normalise_paths_gcovr_realpath(options_FILTERS)
  _normalise_paths_gcovr_realpath(options_EXCLUDE)
  _normalise_paths_gcovr_regex(options_FILTERS)
  _normalise_paths_gcovr_regex(options_EXCLUDE)

  if(options_FILTERS)
    set(tmp "")
    foreach(filter IN LISTS options_FILTERS)
      list(APPEND tmp --filter='${filter}')
    endforeach()
    set(options_FILTERS ${tmp})
  endif()

  if(options_EXCLUDE)
    set(tmp "")
    foreach(exclude IN LISTS options_EXCLUDE)
      list(APPEND tmp --exclude='${exclude}')
    endforeach()
    set(options_EXCLUDE ${tmp})
  endif()

  if(JDBG_REPORTS_DIR)
    set(output_dir "${JDBG_REPORTS_DIR}/coverage")
  else()
    set(output_dir "${PROJECT_BINARY_DIR}/bin/coverage")
  endif()
  set(output_file "${output_dir}/index.html")
  file(MAKE_DIRECTORY "${output_dir}")

  set(root_dir "${PROJECT_SOURCE_DIR}")

  set(gcovr_args)
  list(APPEND gcovr_args --gcov-executable=${cov_executable})
  list(APPEND gcovr_args --root=${root_dir})
  list(APPEND gcovr_args ${options_FILTERS})
  list(APPEND gcovr_args ${options_EXCLUDE})
  list(APPEND gcovr_args --html-details)
  list(APPEND gcovr_args --output=${output_file})
  list(APPEND gcovr_args --delete)
  list(APPEND gcovr_args --print-summary)
  list(APPEND gcovr_args --exclude-unreachable-branches)
  list(APPEND gcovr_args --exclude-throw-branches)
  list(APPEND gcovr_args --html-medium-threshold=80.0)
  list(APPEND gcovr_args --html-high-threshold=90.0)

  include(ProcessorCount)
  ProcessorCount(N)
  if(NOT N EQUAL 0)
    list(APPEND gcovr_args -j${N})
  endif()

  list(APPEND gcovr_args ${PROJECT_BINARY_DIR})

  add_custom_target(${coverage_target}
    COMMAND ${GCOVR_EXECUTABLE} ${gcovr_args}
    WORKING_DIRECTORY ${PROJECT_BINARY_DIR}
    COMMENT "Running gcovr to produce a report"
  )
  add_custom_command(TARGET ${coverage_target} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E cmake_echo_color --yellow
    "Open ${output_file} to see coverage report"
  )
  if(TARGET ${options_POST_TARGET})
    add_custom_command(TARGET ${options_POST_TARGET} POST_BUILD
      COMMAND ${GCOVR_EXECUTABLE} ${gcovr_args}
      WORKING_DIRECTORY ${PROJECT_BINARY_DIR}
      COMMENT "Running gcovr to produce a report"
    )
    add_custom_command(TARGET ${options_POST_TARGET} POST_BUILD
      COMMAND ${CMAKE_COMMAND} -E cmake_echo_color --yellow
      "Open ${output_file} to see coverage report"
    )
  endif()
  set(coverage_set TRUE PARENT_SCOPE)
endfunction()

function(setup_coverage)
  if(CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
    find_package(LlvmCov)
    if(LlvmCov_FOUND)
      set(cov_executable "${LLVM_COV_EXECUTABLE} gcov")
    endif()
  endif()

  if(NOT cov_executable)
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

  if(NOT cov_executable)
    message(WARNING "${coverage_target} target will be unavailable")
    return()
  endif()

  set(coverage_set FALSE)
  _setup_coverage_lcov(${coverage_target} ${cov_executable} ${ARGN})
  _setup_coverage_gcovr(${coverage_target} ${cov_executable} ${ARGN})

  if(NOT coverage_set)
    message(WARNING "${coverage_target} target will be unavailable")
    return()
  endif()
endfunction()

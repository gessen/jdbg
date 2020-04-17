function(_set_version)
  if(JDBG_VERSION)
    return()
  endif()

  find_package(Git REQUIRED)
  execute_process(COMMAND ${GIT_EXECUTABLE} describe --tags --dirty --match "v*"
    WORKING_DIRECTORY ${CMAKE_CURRENT_LIST_DIR}
    OUTPUT_VARIABLE describe_ver
    RESULT_VARIABLE describe_error
    OUTPUT_STRIP_TRAILING_WHITESPACE
  )
  if(describe_error)
    message(FATAL_ERROR "Failed to read version from git tag")
  endif()

  set(ver_regex "^v([0-9]*)[.]([0-9]*)[.]([0-9]*)(-?.*)$")
  string(REGEX REPLACE "${ver_regex}" "\\1" ver_major ${describe_ver})
  string(REGEX REPLACE "${ver_regex}" "\\2" ver_minor ${describe_ver})
  string(REGEX REPLACE "${ver_regex}" "\\3" ver_patch ${describe_ver})
  set(JDBG_VERSION ${ver_major}.${ver_minor}.${ver_patch} PARENT_SCOPE)
endfunction()

_set_version()

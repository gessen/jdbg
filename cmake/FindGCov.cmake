find_program(GCOV_EXECUTABLE
  NAMES ${_CMAKE_TOOLCHAIN_PREFIX}gcov${_CMAKE_TOOLCHAIN_SUFFIX}
  HINTS ${_CMAKE_TOOLCHAIN_LOCATION}
)

if(GCOV_EXECUTABLE)
  execute_process(COMMAND ${GCOV_EXECUTABLE} --version
    OUTPUT_VARIABLE version
    ERROR_QUIET
    OUTPUT_STRIP_TRAILING_WHITESPACE
  )

  if (version MATCHES "^.*gcov \\(GCC\\) .*$")
    string(REGEX REPLACE
      "^.*gcov \\(GCC\\) ([.0-9]+).*$" "\\1"
      GCOV_VERSION "${version}"
    )

    string(REPLACE "." ";" version "${GCOV_VERSION}")
    list(LENGTH version count)
    if(count GREATER 0)
      list(GET version 0 GCOV_MAJOR)
    else()
      set(GCOV_MAJOR 0)
    endif()
    if(count GREATER 1)
      list(GET version 1 GCOV_MINOR)
    else()
      set(GCOV_MINOR 0)
    endif()
    if(count GREATER 2)
      list(GET version 2 GCOV_PATCH)
    else()
      set(GCOV_PATCH 0)
    endif()
  endif()
  unset(version)
endif()

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(GCov
  REQUIRED_VARS GCOV_EXECUTABLE
  VERSION_VAR GCOV_VERSION
)

mark_as_advanced(GCOV_EXECUTABLE)

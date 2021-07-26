find_program(LCOV_EXECUTABLE NAMES lcov)
find_program(GENHTML_EXECUTABLE NAMES genhtml)

if(LCOV_EXECUTABLE)
  execute_process(COMMAND ${LCOV_EXECUTABLE} --version
    OUTPUT_VARIABLE version
    ERROR_QUIET
    OUTPUT_STRIP_TRAILING_WHITESPACE
  )

  if (version MATCHES "^.*LCOV version .*$")
    string(REGEX REPLACE
      "^.*LCOV version ([0-9.]+).*$" "\\1"
      LCOV_VERSION "${version}"
    )

    string(REPLACE "." ";" version "${LCOV_VERSION}")
    list(LENGTH version count)
    if(count GREATER 0)
      list(GET version 0 GCOVR_MAJOR)
    else()
      set(GCOVR_MAJOR 0)
    endif()
    if(count GREATER 1)
      list(GET version 1 GCOVR_MINOR)
    else()
      set(GCOVR_MINOR 0)
    endif()
    if(count GREATER 2)
      list(GET version 2 GCOVR_PATCH)
    else()
      set(GCOVR_PATCH 0)
    endif()
  endif()
  unset(version)
endif()

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(LCov
  REQUIRED_VARS LCOV_EXECUTABLE GENHTML_EXECUTABLE
  VERSION_VAR LCOV_VERSION
)

mark_as_advanced(LCOV_EXECUTABLE)

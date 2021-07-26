find_program(GCOVR_EXECUTABLE NAMES gcovr)

if(GCOVR_EXECUTABLE)
  execute_process(COMMAND ${GCOVR_EXECUTABLE} --version
    OUTPUT_VARIABLE version
    ERROR_QUIET
    OUTPUT_STRIP_TRAILING_WHITESPACE
  )

  if (version MATCHES "^.*gcovr .*$")
    string(REGEX REPLACE
      "^.*gcovr ([0-9.]+).*$" "\\1"
      GCOVR_VERSION "${version}"
    )

    string(REPLACE "." ";" version "${GCOVR_VERSION}")
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
find_package_handle_standard_args(GCovr
  REQUIRED_VARS GCOVR_EXECUTABLE
  VERSION_VAR GCOVR_VERSION
)

mark_as_advanced(GCOVR_EXECUTABLE)

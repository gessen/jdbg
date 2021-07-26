find_program(LLVM_COV_EXECUTABLE NAMES llvm-cov)

if(LLVM_COV_EXECUTABLE)
  execute_process(COMMAND ${LLVM_COV_EXECUTABLE} --version
    OUTPUT_VARIABLE version
    ERROR_QUIET
    OUTPUT_STRIP_TRAILING_WHITESPACE
  )

  if (version MATCHES "^.*LLVM version .*$")
    string(REGEX REPLACE
      "^.*LLVM version ([0-9.]+).*$" "\\1"
      LLVM_COV_VERSION "${version}"
    )

    string(REPLACE "." ";" version "${LLVM_COV_VERSION}")
    list(LENGTH version count)
    if(count GREATER 0)
      list(GET version 0 LLVM_COV_MAJOR)
    else()
      set(LLVM_COV_MAJOR 0)
    endif()
    if(count GREATER 1)
      list(GET version 1 LLVM_COV_MINOR)
    else()
      set(LLVM_COV_MINOR 0)
    endif()
    if(count GREATER 2)
      list(GET version 2 LLVM_COV_PATCH)
    else()
      set(LLVM_COV_PATCH 0)
    endif()
  endif()
  unset(version)
endif()

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(LlvmCov
  REQUIRED_VARS LLVM_COV_EXECUTABLE
  VERSION_VAR LLVM_COV_VERSION
)

mark_as_advanced(LLVM_COV_EXECUTABLE)

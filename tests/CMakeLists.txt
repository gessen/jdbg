add_executable(${PROJECT_NAME}-tests)

include(AddCatch)

target_compile_features(${PROJECT_NAME}-tests
  PRIVATE
    cxx_std_17
)

target_compile_options(${PROJECT_NAME}-tests
  PRIVATE
    # Standard set of warnings
    -Wall
    -Wextra
    -Wpedantic
    # Additional warnings not included in -Wall -Wextra -Wpedantic
    -Wformat
    $<$<CXX_COMPILER_ID:Clang>:-Wformat-pedantic>
    -Woverloaded-virtual
    -Wold-style-cast
    # Increased reliability of backtraces
    -fasynchronous-unwind-tables
    # Stack smashing protector
    -fstack-protector-strong
    # Colourise output
    $<$<CXX_COMPILER_ID:GNU>:-fdiagnostics-color=always>
    $<$<CXX_COMPILER_ID:Clang>:-fcolor-diagnostics>
    # Avoid temporary files, speeding up builds
    -pipe
    # Enable coverage
    $<$<BOOL:${JDBG_ENABLE_COVERAGE}>:--coverage>
)

target_link_options(${PROJECT_NAME}-tests
  PRIVATE
    $<$<BOOL:${JDBG_ENABLE_COVERAGE}>:--coverage>
)

target_link_libraries(${PROJECT_NAME}-tests
  PRIVATE
    jdbg::jdbg
    Catch2::Catch2WithMain
)

set_target_properties(${PROJECT_NAME}-tests
  PROPERTIES
    ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/${CMAKE_INSTALL_LIBDIR}
    LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/${CMAKE_INSTALL_LIBDIR}
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/${CMAKE_INSTALL_BINDIR}
    CXX_EXTENSIONS OFF
)

target_sources(${PROJECT_NAME}-tests
  PRIVATE
    ${CMAKE_CURRENT_LIST_DIR}/jdbg_tests.cpp
    ${CMAKE_CURRENT_LIST_DIR}/pretty_print_tests.cpp
    ${CMAKE_CURRENT_LIST_DIR}/type_name_tests.cpp
)

configure_file(
  "${PROJECT_SOURCE_DIR}/cmake/${PROJECT_NAME}_pch.hpp.in"
  "${PROJECT_BINARY_DIR}/${PROJECT_NAME}_pch.hpp"
  @ONLY
)
target_precompile_headers(${PROJECT_NAME}-tests
  PRIVATE
    "${PROJECT_BINARY_DIR}/${PROJECT_NAME}_pch.hpp"
)

include(Catch)
catch_discover_tests(${PROJECT_NAME}-tests)

if(TARGET check)
  set(check_target ${PROJECT_NAME}-check)
else()
  set(check_target check)
endif()

set(check_target_depends ${PROJECT_NAME}-tests)
add_custom_target(${check_target}
  COMMAND ${CMAKE_CTEST_COMMAND}
  WORKING_DIRECTORY ${PROJECT_BINARY_DIR}
  DEPENDS ${check_target_depends}
)

if(JDBG_ENABLE_COVERAGE)
  include(SetupCoverage)
  setup_coverage()
endif()

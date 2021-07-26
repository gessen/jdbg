add_executable(${PROJECT_NAME}-examples)

target_compile_options(${PROJECT_NAME}-examples
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
    # Don't warn about not handled values in a switch
    -Wno-switch
    # Increased reliability of backtraces
    -fasynchronous-unwind-tables
    # Stack smashing protector
    -fstack-protector-strong
    # Colourise output
    $<$<CXX_COMPILER_ID:GNU>:-fdiagnostics-color=always>
    $<$<CXX_COMPILER_ID:Clang>:-fcolor-diagnostics>
    # Avoid temporary files, speeding up builds
    -pipe
)

# Emit header dependencies (Ninja does it automatically)
if(NOT CMAKE_GENERATOR MATCHES Ninja)
  target_compile_options(${PROJECT_NAME}-examples PRIVATE -MMD)
endif()

target_link_libraries(${PROJECT_NAME}-examples
  PRIVATE
    jdbg::jdbg
)

set_target_properties(${PROJECT_NAME}-examples
  PROPERTIES
    ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/${CMAKE_INSTALL_LIBDIR}
    LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/${CMAKE_INSTALL_LIBDIR}
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/${CMAKE_INSTALL_BINDIR}
    CXX_EXTENSIONS OFF
)

target_sources(${PROJECT_NAME}-examples
  PRIVATE
    ${CMAKE_CURRENT_LIST_DIR}/main.cpp
)

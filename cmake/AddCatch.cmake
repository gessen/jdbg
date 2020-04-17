find_package(Catch2 QUIET)
if(${Catch2_FOUND})
  return()
endif()

include(FetchContent)
FetchContent_Declare(Catch2
  GIT_REPOSITORY https://github.com/catchorg/catch2
  GIT_TAG v2.11.0
  GIT_SHALLOW TRUE
)

FetchContent_GetProperties(Catch2)
if(NOT catch2_POPULATED)
  message(STATUS "Downloading Catch2")
  FetchContent_Populate(Catch2)
  message(STATUS "Downloading Catch2 - done")
  add_subdirectory(${catch2_SOURCE_DIR} ${catch2_BINARY_DIR})
  list(APPEND CMAKE_MODULE_PATH "${catch2_SOURCE_DIR}/contrib")
endif()

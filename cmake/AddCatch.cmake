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

FetchContent_MakeAvailable(Catch2)

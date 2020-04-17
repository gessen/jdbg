function(_set_default_build_type)
  # Determine if jdbg is built as a subproject or if it is the master project.
  set(MASTER_PROJECT OFF PARENT_SCOPE)
  if(NOT CMAKE_CURRENT_SOURCE_DIR STREQUAL CMAKE_SOURCE_DIR)
    return()
  endif()
  set(MASTER_PROJECT ON PARENT_SCOPE)
  set(master_build_types "Debug Release RelWithDebInfo MinSizeRel")
  # Set a default build type if none was specified
  if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
    set(CMAKE_BUILD_TYPE Debug CACHE STRING
      "Choose the type of build, options are:
       None(CMAKE_CXX_FLAGS or CMAKE_C_FLAGS used) ${master_build_types}."
    )
    message(STATUS "No build type selected, default to ${CMAKE_BUILD_TYPE}")
  else()
    set(CMAKE_BUILD_TYPE ${CMAKE_BUILD_TYPE} CACHE STRING
      "Choose the type of build, options are:
       None(CMAKE_CXX_FLAGS or CMAKE_C_FLAGS used) ${master_build_types}."
    )
    message(STATUS "Build type: ${CMAKE_BUILD_TYPE}")
  endif()
  string(REPLACE " " ";" master_build_types_available ${master_build_types})
  set_property(CACHE CMAKE_BUILD_TYPE
    PROPERTY STRINGS ${master_build_types_available}
  )
endfunction()

_set_default_build_type()

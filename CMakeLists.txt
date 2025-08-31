cmake_minimum_required(VERSION 3.29)

project(jdbg
  VERSION 0.4.0
  LANGUAGES CXX
  DESCRIPTION "A helpful debugging macro"
)

# Set CMake modules path
list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_LIST_DIR}/cmake")

option(JDBG_BUILD_TESTING "Build jdbg testing tree." ${PROJECT_IS_TOP_LEVEL})
option(JDBG_BUILD_EXAMPLES "Build jdbg examples tree." ${PROJECT_IS_TOP_LEVEL})

option(JDBG_ENABLE_INSTALL "Enable installation." ${PROJECT_IS_TOP_LEVEL})
option(JDBG_ENABLE_COVERAGE "Enable coverage reporting." ${JDBG_BUILD_TESTING})

if(PROJECT_IS_TOP_LEVEL)
  # Generate JSON Compilation Database
  set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
endif()

add_library(${PROJECT_NAME} INTERFACE)
add_library(${PROJECT_NAME}::${PROJECT_NAME} ALIAS ${PROJECT_NAME})

target_sources(${PROJECT_NAME}
  PUBLIC
    FILE_SET HEADERS
    BASE_DIRS ${PROJECT_SOURCE_DIR}/include
    FILES
      include/jdbg/detail/meta.hpp
      include/jdbg/jdbg.hpp
      include/jdbg/pretty_print.hpp
      include/jdbg/type_name.hpp
)

include(GNUInstallDirs)
include(CTest)
if(JDBG_BUILD_TESTING AND BUILD_TESTING)
  add_subdirectory(tests)
endif()

if(JDBG_BUILD_EXAMPLES)
  add_subdirectory(examples)
endif()

if(JDBG_ENABLE_INSTALL)
  set(config_version_file
    "${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}-config-version.cmake"
  )
  set(config_package_file
    "${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}-config.cmake"
  )
  set(package_install_dir "${CMAKE_INSTALL_LIBDIR}/cmake/${PROJECT_NAME}")
  set(pkg_config "${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}.pc")

  set(targets_config "${PROJECT_NAME}-targets.cmake")
  set(targets_export_name "${PROJECT_NAME}-targets")

  # Generate cmake version file
  include(CMakePackageConfigHelpers)
  write_basic_package_version_file(
    "${config_version_file}"
    COMPATIBILITY SameMajorVersion
  )

  # Configure cmake config file
  configure_package_config_file(
    "${PROJECT_SOURCE_DIR}/cmake/${PROJECT_NAME}-config.cmake.in"
    "${config_package_file}"
    INSTALL_DESTINATION "${package_install_dir}"
  )

  # Configure pkg-config file
  configure_file(
    "${PROJECT_SOURCE_DIR}/cmake/${PROJECT_NAME}.pc.in"
    "${pkg_config}"
    @ONLY
  )

  # Install targets
  install(
    TARGETS ${PROJECT_NAME}
    EXPORT ${targets_export_name}
    FILE_SET HEADERS
  )

  # install cmake config and version files
  install(
    FILES
      "${config_package_file}"
      "${config_version_file}"
    DESTINATION "${package_install_dir}"
  )

  # Install targets config file
  install(
    EXPORT ${targets_export_name}
    FILE ${targets_config}
    NAMESPACE ${PROJECT_NAME}::
    DESTINATION "${package_install_dir}"
  )

  # Install pkg-config file
  install(
    FILES "${pkg_config}"
    DESTINATION "${CMAKE_INSTALL_LIBDIR}/pkgconfig"
  )

  # Package library
  include(AddCPack)
endif()

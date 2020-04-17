cmake_minimum_required(VERSION 3.13)

# Set CMake modules path
list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_LIST_DIR}/cmake")

include(HandleBuildType)
include(HandleVersion)

project(jdbg
  VERSION ${JDBG_VERSION}
  LANGUAGES CXX
)

option(JDBG_BUILD_TESTING "Build jdbg testing tree." ${MASTER_PROJECT})
option(JDBG_BUILD_EXAMPLES "Build jdbg examples tree." ${MASTER_PROJECT})

option(JDBG_ENABLE_INSTALL "Enable installation." ${MASTER_PROJECT})
option(JDBG_ENABLE_COVERAGE "Enable coverage reporting." ${JDBG_BUILD_TESTING})

if(MASTER_PROJECT)
  # Generate JSON Compilation Database
  set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
endif()

add_library(${PROJECT_NAME} INTERFACE)
add_library(${PROJECT_NAME}::${PROJECT_NAME} ALIAS ${PROJECT_NAME})

target_include_directories(${PROJECT_NAME}
  INTERFACE
    $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
)

target_sources(${PROJECT_NAME}
  INTERFACE
    $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include/jdbg/jdbg.hpp>
    $<INSTALL_INTERFACE:include/jdbg/jdbg.hpp>
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
  set(version_config "${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}-config-version.cmake")
  set(project_config "${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}-config.cmake")
  set(pkg_config "${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}.pc")

  set(config_install_dir "${CMAKE_INSTALL_LIBDIR}/cmake/${PROJECT_NAME}")

  set(targets_config "${PROJECT_NAME}-targets.cmake")
  set(targets_export_name "${PROJECT_NAME}-targets")
  set(namespace "${PROJECT_NAME}::")

  # Generate cmake version file
  include(CMakePackageConfigHelpers)
  write_basic_package_version_file(
    "${version_config}"
    COMPATIBILITY SameMajorVersion
  )

  # Configure cmake config file
  configure_package_config_file(
    "${PROJECT_SOURCE_DIR}/cmake/${PROJECT_NAME}-config.cmake.in"
    "${project_config}"
    INSTALL_DESTINATION "${config_install_dir}"
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
    LIBRARY DESTINATION "${CMAKE_INSTALL_LIBDIR}"
    ARCHIVE DESTINATION "${CMAKE_INSTALL_LIBDIR}"
    INCLUDES DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}"
  )

  # Install headers
  install(
    DIRECTORY include/${PROJECT_NAME}
    DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}"
  )

  # install cmake config and version files
  install(
    FILES
      "${project_config}"
      "${version_config}"
    DESTINATION "${config_install_dir}"
  )

  # Install targets config file
  install(
    EXPORT ${targets_export_name}
    FILE ${targets_config}
    NAMESPACE ${namespace}
    DESTINATION "${config_install_dir}"
  )

  # Install pkg-config file
  install(
    FILES "${pkg_config}"
    DESTINATION "${CMAKE_INSTALL_LIBDIR}/pkgconfig"
  )

  # Package library
  include(AddCPack)
endif()

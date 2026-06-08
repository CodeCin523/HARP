cmake_minimum_required(VERSION 3.17)

# ------------------------------------------------------------------------------
# Project
# ------------------------------------------------------------------------------

project(
    HARP
    VERSION 1.0.2
    DESCRIPTION
        "HARP is a minimal runtime that orchestrates modular behavior through runtime-loaded packages."
    LANGUAGES C
)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

option(HARP_BUILD_TOOL  "Build HARP command line tool" ON)
option(HARP_BUILD_TESTS "Build HARP tests" OFF)


# ------------------------------------------------------------------------------
# External Dependencies
# ------------------------------------------------------------------------------

add_subdirectory(external)


# ------------------------------------------------------------------------------
# HarpInterface Library
# ------------------------------------------------------------------------------

add_library(HarpInterface INTERFACE)
add_library(HARP::Interface ALIAS HarpInterface)

target_include_directories(
    HarpInterface INTERFACE
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
)

set_target_properties(
    HarpInterface PROPERTIES
    EXPORT_NAME Interface
)

target_compile_features(
    HarpInterface INTERFACE
    c_std_23
)


# ------------------------------------------------------------------------------
# Shared Library
# ------------------------------------------------------------------------------

add_library(Harp SHARED)
add_library(HARP::Harp ALIAS Harp)

# set_target_properties(
#     Harp PROPERTIES
#     VERSION ${PROJECT_VERSION}
#     SOVERSION ${PROJECT_VERSION_MAJOR}
# )

target_sources(
    Harp PRIVATE
    src/impl/harp_core_api.c
    src/runtime/harp_registry.c
    src/runtime/harp_runtime.c
    src/harp_core.c
)

target_include_directories(
    Harp PRIVATE
    src
)

target_link_libraries(
    Harp
    PUBLIC
        HarpInterface
    PRIVATE
        hmem::hmem
)

target_compile_features(
    Harp PUBLIC
    c_std_23
)


# ------------------------------------------------------------------------------
# Tests
# ------------------------------------------------------------------------------

if(HARP_BUILD_TESTS)
    add_subdirectory(tests)
endif()


# ------------------------------------------------------------------------------
# Command Line Tool
# ------------------------------------------------------------------------------

if(HARP_BUILD_TOOL)

    add_executable(harp
        tools/harp/main.c
    )

    target_link_libraries(
        harp PRIVATE
        HARP::Harp
    )

endif()


# ------------------------------------------------------------------------------
# Installation
# ------------------------------------------------------------------------------

include(GNUInstallDirs)

install(
    DIRECTORY include/
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
)

install(
    TARGETS
        HarpInterface
        Harp
    EXPORT HarpTargets

    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}

    INCLUDES DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
)

if(HARP_BUILD_TOOL)

    install(
        TARGETS harp
        RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    )

endif()


# ------------------------------------------------------------------------------
# CMake Package
# ------------------------------------------------------------------------------

include(CMakePackageConfigHelpers)

write_basic_package_version_file(
    "${CMAKE_CURRENT_BINARY_DIR}/HarpConfigVersion.cmake"
    VERSION ${PROJECT_VERSION}
    COMPATIBILITY SameMajorVersion
)

configure_package_config_file(
    "${CMAKE_CURRENT_LIST_DIR}/cmake/HarpConfig.cmake.in"
    "${CMAKE_CURRENT_BINARY_DIR}/HarpConfig.cmake"
    INSTALL_DESTINATION
        ${CMAKE_INSTALL_LIBDIR}/cmake/harp
)

install(
    EXPORT HarpTargets
    FILE HarpTargets.cmake
    NAMESPACE HARP::
    DESTINATION
        ${CMAKE_INSTALL_LIBDIR}/cmake/harp
)

install(
    FILES
        "${CMAKE_CURRENT_BINARY_DIR}/HarpConfig.cmake"
        "${CMAKE_CURRENT_BINARY_DIR}/HarpConfigVersion.cmake"
    DESTINATION
        ${CMAKE_INSTALL_LIBDIR}/cmake/harp
)
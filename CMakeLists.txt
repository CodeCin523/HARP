cmake_minimum_required(VERSION 3.17)

project(
    HARP
    VERSION 1.0.0
    DESCRIPTION
        "HARP is a minimal runtime that orchestrates modular behavior through runtime-loaded plugins."
    LANGUAGES C
)

option(HARP_BUILD_INTERFACE "Build HARP interface" ON)
option(HARP_BUILD_LIBRARY  "Build HARP shared library" ON)
option(HARP_BUILD_TESTS    "Build HARP tests" OFF)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

# ---- plugin interface ----
if(HARP_BUILD_INTERFACE)
    add_library(Interface INTERFACE)
    add_library(HARP::Interface ALIAS Interface)

    target_include_directories(
        Interface INTERFACE
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
    )

    target_compile_features(
        Interface INTERFACE c_std_23
    )
endif()

# ---- harp library ----
if(HARP_BUILD_LIBRARY)
    add_library(Harp SHARED)
    add_library(HARP::Harp ALIAS Harp)

    set_target_properties(
        Harp PROPERTIES
        VERSION ${PROJECT_VERSION}
        SOVERSION ${PROJECT_VERSION_MAJOR}
    )

    target_sources(
        Harp PRIVATE
        src/harp_core.c
        src/impl/harp_registry.c   # 👈 make sure it's here
    )

    target_compile_features(
        Harp PUBLIC c_std_23
    )

    target_include_directories(
        Harp
        PRIVATE src
        PUBLIC
            $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
            $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
    )
endif()

# ---- harp tests ----
if(HARP_BUILD_TESTS)
    add_subdirectory(tests)
endif()

# ---- install the library ----
include(GNUInstallDirs)

if(HARP_BUILD_INTERFACE)
    install(
        TARGETS Interface
        EXPORT HarpTargets
        INCLUDES DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
    )
endif()

if(HARP_BUILD_LIBRARY)
    install(
        TARGETS Harp
        EXPORT HarpTargets
        LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
        ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
        RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
        INCLUDES DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
    )
endif()

install(DIRECTORY include/ DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})

# ---- cmake package ----
include(CMakePackageConfigHelpers)

write_basic_package_version_file(
    "HarpConfigVersion.cmake"
    VERSION ${PROJECT_VERSION}
    COMPATIBILITY SameMajorVersion)

configure_package_config_file(
    "${CMAKE_CURRENT_LIST_DIR}/cmake/HarpConfig.cmake.in"
    "${CMAKE_CURRENT_BINARY_DIR}/HarpConfig.cmake"
    INSTALL_DESTINATION ${CMAKE_INSTALL_DATAROOTDIR}/cmake/harp
)

install(
    EXPORT HarpTargets
    FILE HarpTargets.cmake
    NAMESPACE HARP::
    DESTINATION ${CMAKE_INSTALL_DATAROOTDIR}/cmake/harp
)
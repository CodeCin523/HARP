set(SRC_DIR ${PROJECT_SOURCE_DIR}/src)

# runtime tests
add_executable(test_harp_registry
    runtime/harp_registry.c
)
target_link_libraries(test_harp_registry PRIVATE Harp hmem::hmem)
target_include_directories(test_harp_registry PRIVATE
    ${SRC_DIR}
)

add_executable(test_harp_runtime
    runtime/harp_runtime.c
)
target_link_libraries(test_harp_runtime PRIVATE Harp hmem::hmem)
target_include_directories(test_harp_runtime PRIVATE
    ${SRC_DIR}
)

add_test(NAME test_harp_registry COMMAND test_harp_registry)
add_test(NAME test_harp_runtime COMMAND test_harp_runtime)

# core tests
add_executable(test_harp_core_generic
    harp_core_generic.c
)
target_link_libraries(test_harp_core_generic PRIVATE Harp)

add_executable(test_harp_core_paths
    harp_core_paths.c
)
target_link_libraries(test_harp_core_paths PRIVATE Harp)

add_test(NAME test_harp_core_generic COMMAND test_harp_core_generic)
add_test(NAME test_harp_core_paths COMMAND test_harp_core_paths)

# package interface
add_library(TestPackageInterface INTERFACE)

target_include_directories(
    TestPackageInterface INTERFACE
    ${CMAKE_CURRENT_SOURCE_DIR}/packages/test_package/include
)

target_link_libraries(
    TestPackageInterface INTERFACE
    Interface
)

# package shared library
add_library(test_package SHARED
    packages/test_package/src/test_package.c
)

target_link_libraries(
    test_package PRIVATE
    Interface
    TestPackageInterface
)

set_target_properties(test_package PROPERTIES
    PREFIX ""
    LIBRARY_OUTPUT_DIRECTORY
        ${CMAKE_BINARY_DIR}/bin/packages/test_package
    RUNTIME_OUTPUT_DIRECTORY
        ${CMAKE_BINARY_DIR}/bin/packages/test_package
)

# package runtime tests
add_executable(test_harp_core_packages
    harp_core_packages.c
)

target_link_libraries(
    test_harp_core_packages PRIVATE
    Harp
    TestPackageInterface
)

add_test(
    NAME test_harp_core_packages
    COMMAND test_harp_core_packages
)

set_target_properties(test_harp_core_packages PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY
        ${CMAKE_BINARY_DIR}/bin
)
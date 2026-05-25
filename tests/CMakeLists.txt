set(SRC_DIR ${PROJECT_SOURCE_DIR}/src)

add_executable(test_harp_registry
    runtime/harp_registry.c
)
target_link_libraries(test_harp_registry PRIVATE Harp)
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
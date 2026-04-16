set(SRC_DIR ${PROJECT_SOURCE_DIR}/src)

add_executable(unit_harp_registry
    unit/impl/harp_registry.c
)
target_link_libraries(unit_harp_registry PRIVATE Harp)
target_include_directories(unit_harp_registry PRIVATE
    ${SRC_DIR}
)

add_test(NAME unit_harp_registry COMMAND unit_harp_registry)
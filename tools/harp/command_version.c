#include <stdio.h>

#include <harp/harp_core.h>
#include <harp/utils/harp_version.h>

int command_version(void) {
    HarpVersion version = harp_version();

    printf(
        "HARP Tool v1.0.2\n"
        "HARP Library v%u.%u.%u\n"
        "HARP Core Handler v%u.%u.%u\n",
        HARP_VERSION_MAJOR(version),
        HARP_VERSION_MINOR(version),
        HARP_VERSION_PATCH(version),

        HARP_VERSION_MAJOR(HARP_CORE_HANDLER_VERSION),
        HARP_VERSION_MINOR(HARP_CORE_HANDLER_VERSION),
        HARP_VERSION_PATCH(HARP_CORE_HANDLER_VERSION)
    );

    return 0;
}
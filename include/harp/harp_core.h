#ifndef HARP_CORE_H
#define HARP_CORE_H

#ifdef __cplusplus
extern "C" {
#endif

#include "harp.h"


typedef struct HarpRuntimeCreator HarpRuntimeCreator;

struct HarpRuntimeCreator {
    HarpCreatorBase _base;

    const char *argv0;
};


/* ================================================================================ */
/*  INTERFACE                                                                       */
/* ================================================================================ */

HarpResult harp_initialize(
    const HarpCreatorBase *creator,
    HarpRuntime **out_runtime
);
HarpResult harp_terminate(
    HarpRuntime *runtime
);

HarpResult harp_runtime_load_directory(
    HarpRuntime *runtime,
    const char *path
);

HarpResult harp_runtime_get_api(
    HarpRuntime *runtime,
    const HarpName name,
    HarpApiBase **out_api
);
HarpResult harp_runtime_get_handler(
    HarpRuntime *runtime,
    const HarpName name,
    HarpHandlerBase **out_handler
);

// HarpResult harp_runtime_get_package_count(
//     HarpRuntime *runtime,
//     uint64_t *out_count
// );
// HarpResult harp_runtime_get_package_name(
//     HarpRuntime *runtime,
//     uint64_t index,
//     HarpName *out_name
// );


#ifdef __cplusplus
}
#endif

#endif /* HARP_CORE_H */
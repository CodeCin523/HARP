#ifndef HARP_API_H
#define HARP_API_H

#ifdef __cplusplus
extern "C" {
#endif

#include "harp.h"


typedef struct HarpRuntimeDesc HarpRuntimeDesc;

struct HarpRuntimeDesc {
    const char *executable_path;
};


/* ================================================================================ */
/*  INTERFACE                                                                       */
/* ================================================================================ */

HarpVersion harp_version(void);

HarpResult harp_initialize(
    const HarpRuntimeDesc *desc,
    HarpRuntime **out_runtime
);
HarpResult harp_terminate(
    HarpRuntime *runtime
);

HarpResult harp_runtime_get_handler(
    HarpRuntime *runtime,
    const HarpDependencyDesc *dependency,
    HarpHandlerBase **out_handler
);

HarpResult harp_runtime_load_packages(
    HarpRuntime *runtime
);
HarpResult harp_runtime_load_packages_from(
    HarpRuntime *runtime,
    const char *path
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

#endif /* HARP_API_H */
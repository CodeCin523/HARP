#ifndef HARP_CORE_H
#define HARP_CORE_H

#ifdef __cplusplus
extern "C" {
#endif


#include "harp.h"


/* ================================================================================ */
/*  TYPEDEF                                                                         */
/* ================================================================================ */

typedef struct HarpRuntime HarpRuntime;


/* ================================================================================ */
/*  INTERFACE                                                                       */
/* ================================================================================ */

HarpResult harp_init(
    HarpRuntime **out_runtime
);
HarpResult harp_exit(
    HarpRuntime *runtime
);

HarpResult harp_runtime_load_packages(
    HarpRuntime *runtime,
    const char *path
);

HarpResult harp_runtime_get_api(
    HarpRuntime *runtime,
    const HarpName name,
    HarpApiBase **out_api
);

HarpResult harp_runtime_get_package_count(
    HarpRuntime *runtime,
    uint64_t *out_count
);
HarpResult harp_runtime_get_package_name(
    HarpRuntime *runtime,
    uint64_t index,
    HarpName *out_name
);


#ifdef __cplusplus
}
#endif

#endif /* HARP_CORE_H */
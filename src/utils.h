#ifndef UTILS_H
#define UTILS_H

#include <harp/harp.h>

#include <harp/utils/harp_version.h>


/* ================================================================================ */
/*  CHECKS                                                                          */
/* ================================================================================ */

static inline HarpResult utils_check_dependency(const HarpDependencyDesc *dependency) {
    if(dependency == NULL || dependency->name == NULL)
        return HARP_RESULT_INVALID_ARGUMENTS;

    return HARP_RESULT_OK;
}
static inline HarpResult utils_check_package_desc(const HarpPackageDesc *package) {
    if(package == NULL || package->name == NULL || package->pfn_register == NULL)
        return HARP_RESULT_INVALID_ARGUMENTS;

    return HARP_RESULT_OK;
}
static inline HarpResult utils_check_handler_desc(const HarpHandlerDesc *handler) {
    if(handler == NULL || handler->name == NULL || handler->pfn_init == NULL || handler->pfn_term == NULL)
        return HARP_RESULT_INVALID_ARGUMENTS;

    return HARP_RESULT_OK;
}
static inline HarpResult utils_check_actor_desc(const HarpActorDesc *actor) {
    if(actor == NULL || actor->name == NULL || actor->pfn_create == NULL || actor->pfn_destroy == NULL)
        return HARP_RESULT_INVALID_ARGUMENTS;

    return HARP_RESULT_OK;
}

static inline HarpResult utils_check_core_handler(const HarpCoreHandler *core) {
    if(core == NULL)
        return HARP_RESULT_INVALID_ARGUMENTS;

    return HARP_RESULT_OK;
}

static inline HarpResult utils_check_runtime(const HarpRuntime *runtime) {
    if(runtime == NULL)
        return HARP_RESULT_INVALID_ARGUMENTS;

    return HARP_RESULT_OK;
}


/* ================================================================================ */
/*  MISCELLANEOUS                                                                   */
/* ================================================================================ */

static inline HarpResult utils_dependency_matches(
    const HarpDependencyDesc *dependency,
    HarpVersion version
) {
    // if(HARP_VERSION_MAJOR(version) != HARP_VERSION_MAJOR(dependency->min_version)
    //     || HARP_VERSION_MINOR(version) >= HARP_VERSION_MINOR(dependency->min_version))
    //     return HARP_RESULT_DEPENDENCY_VERSION_MISMATCH;
    // if(HARP_VERSION_MAJOR(version) != HARP_VERSION_MAJOR(dependency->max_version)
    //     || HARP_VERSION_MINOR(version) >= HARP_VERSION_MINOR(dependency->max_version))
    //     return HARP_RESULT_DEPENDENCY_VERSION_MISMATCH;
    if(version < dependency->min_version)
        return HARP_RESULT_DEPENDENCY_VERSION_MISMATCH;

    if(version > dependency->max_version)
        return HARP_RESULT_DEPENDENCY_VERSION_MISMATCH;

    return HARP_RESULT_OK;
}


#endif /* UTILS_H */
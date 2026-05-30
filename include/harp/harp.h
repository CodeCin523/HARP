#ifndef HARP_H
#define HARP_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stddef.h>
#include <stdint.h>

#undef HARP_UTILS_UNDEF
#include "utils/harp_version.h"


/* ================================================================================ */
/*  TYPEDEF                                                                         */
/* ================================================================================ */

typedef uint32_t HarpResult;
enum {
    HARP_RESULT_OK                          = 0,  // Success
    HARP_RESULT_FAILED                      = 1,  // Generic failure
    HARP_RESULT_CRITICAL_FAIL               = 2,
    HARP_RESULT_INVALID_STATE               = 3,
    HARP_RESULT_OUT_OF_MEMORY               = 4,
    HARP_RESULT_INVALID_ARGUMENTS           = 10, // One or more arguments are invalid
    HARP_RESULT_MISSING_OUTPUT              = 11, // Output pointer required, but NULL
    HARP_RESULT_NAME_EXISTS                 = 20, // The given name already as an entry
    HARP_RESULT_NAME_NOT_FOUND              = 21, // Entry with given name not found
    HARP_RESULT_NAME_TYPE_MISMATCH          = 22,
    HARP_RESULT_DEPENDENCY_NOT_FOUND        = 30,
    HARP_RESULT_DEPENDENCY_VERSION_MISMATCH = 31,
    HARP_RESULT_DEPENDENCY_UNINITIALIZED    = 32,
    HARP_RESULT_DEPENDENCY_CYCLE            = 33,
    HARP_RESULT_EXISTENT_DEPENDENTS         = 34,
    HARP_RESULT_EXISTENT_ACTORS             = 35,
};

typedef const char* HarpName;
typedef uint32_t HarpVersion;

typedef uint8_t HarpCreatorKind;
typedef uint32_t HarpCreatorFlags;
enum {
    HARP_CREATOR_FLAG_DEFAULT_CREATOR       = (1 << 0),
    HARP_CREATOR_FLAG_EXPERIMENTAL          = (1 << 1),
    HARP_CREATOR_FLAG_RESOLVE_DEPENDENCIES  = (1 << 2)
};

typedef struct HarpDependencyDesc HarpDependencyDesc;
typedef struct HarpPackageDesc HarpPackageDesc;
typedef struct HarpHandlerDesc HarpHandlerDesc;
typedef struct HarpActorDesc HarpActorDesc;
typedef struct HarpApiDesc HarpApiDesc;

typedef struct HarpApiBase HarpApiBase;
typedef struct HarpHandlerBase HarpHandlerBase;
typedef struct HarpActorBase HarpActorBase;
typedef struct HarpCreatorBase HarpCreatorBase;

typedef struct HarpCoreApi HarpCoreApi;
typedef struct HarpExtendedApi HarpExtendedApi;

typedef struct HarpRuntime HarpRuntime;


/* ================================================================================ */
/*  DESCRIPTORS                                                                     */
/* ================================================================================ */

struct HarpDependencyDesc {
    HarpName name;

    HarpVersion min_version;
    HarpVersion max_version;

    // uint8_t required; // 1 = must exist, 0 = optional
};

struct HarpPackageDesc {
    HarpName name;
    HarpVersion version;

    HarpResult (*pfn_register)(HarpApiBase *);

    HarpDependencyDesc* p_dependencies;
    uint64_t dependency_count;
};

struct HarpHandlerDesc {
    HarpName name;
    HarpVersion version;
    uint64_t instance_size;
    uint64_t instance_alignment;

    HarpResult (*pfn_init)(HarpCoreApi*, HarpHandlerBase*, HarpCreatorBase*);
    HarpResult (*pfn_term)(HarpCoreApi*, HarpHandlerBase*);

    HarpDependencyDesc* p_dependencies;
    uint64_t dependency_count;
};

struct HarpActorDesc {
    HarpName name;
    HarpVersion version;
    uint64_t instance_size;
    uint64_t instance_alignment;

    HarpResult (*pfn_create)(HarpCoreApi*, HarpActorBase*, HarpCreatorBase*);
    HarpResult (*pfn_destroy)(HarpCoreApi*, HarpActorBase*);

    HarpName parent_handler;
};

struct HarpApiDesc {
    HarpName name;
    HarpVersion version;
    uint64_t instance_size;
    uint64_t instance_alignment;
};


/* ================================================================================ */
/*  BASE STRUCTS                                                                    */
/* ================================================================================ */

struct HarpApiBase {
    HarpHandlerBase *p_handler;
    HarpVersion version;
    uint8_t available; // 1 = available, 0 = unavailable
};

struct HarpHandlerBase {
    HarpName name;
};

struct HarpActorBase {
    HarpName name;
    HarpHandlerBase *p_handler;
};

struct HarpCreatorBase {
    HarpCreatorKind kind;
    HarpCreatorFlags flags;
};


/* ================================================================================ */
/*  Application Programming Interface                                               */
/* ================================================================================ */

#define HARP_CORE_API_NAME "HarpCoreApi"
#define HARP_CORE_API_VERSION HARP_MAKE_VERSION(1,0,0)

struct HarpCoreApi {
    HarpApiBase _base;

    /* Registration */
    HarpResult (*register_api)(HarpHandlerBase *handler, const HarpApiDesc* desc, HarpApiBase** out_api);
    HarpResult (*register_handler)(HarpHandlerBase *handler, const HarpHandlerDesc* desc);
    HarpResult (*register_actor)(HarpHandlerBase *handler, const HarpActorDesc* desc);

    /* Retrieval */
    HarpResult (*get_api)(HarpHandlerBase *handler, const HarpName name, HarpApiBase **out_api);
    HarpResult (*get_handler)(HarpHandlerBase *handler, const HarpName name, HarpHandlerBase** out_handler);
    HarpResult (*get_api_desc)(HarpHandlerBase *handler, const HarpName name, HarpApiDesc **out_desc);
    HarpResult (*get_handler_desc)(HarpHandlerBase *handler, const HarpName name, HarpHandlerDesc** out_desc);
    HarpResult (*get_actor_desc)(HarpHandlerBase *handler, const HarpName name, HarpActorDesc** out_desc);

    /* Lifecycle */
    HarpResult (*handler_initialize)(HarpHandlerBase *handler, const HarpName name, const HarpCreatorBase* creator);
    HarpResult (*handler_terminate)(HarpHandlerBase *handler, const HarpName name);

    HarpResult (*actor_create)(HarpHandlerBase *handler, const HarpName name, const HarpCreatorBase* creator, HarpActorBase** out_actor);
    HarpResult (*actor_destroy)(HarpHandlerBase *handler, const HarpName name, HarpActorBase* actor);

    /* Paths */
    HarpResult (*get_executable_directory)(HarpHandlerBase *handler, const char **out_path);
    HarpResult (*get_working_directory)(HarpHandlerBase *handler, const char **out_path);
    HarpResult (*get_package_directory)(HarpHandlerBase *handler, const HarpName name, const char **out_path);
};

#define HARP_EXTENDED_API_NAME "HarpExtendedApi"
#define HARP_EXTENDED_API_VERSION HARP_MAKE_VERSION(0,0,0)

struct HarpExtendedApi {
    HarpApiBase _base;
    // Extendable API for package-specific extensions
};


#define HARP_UTILS_UNDEF
#include "utils/harp_version.h"
#undef HARP_UTILS_UNDEF

#ifdef __cplusplus
}
#endif

#endif /* HARP_H */
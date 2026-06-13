#ifndef HARP_H
#define HARP_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stddef.h>
#include <stdint.h>

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

typedef const char * HarpName;
typedef uint32_t HarpVersion;

typedef struct HarpDependencyDesc HarpDependencyDesc;
typedef struct HarpPackageDesc HarpPackageDesc;
typedef struct HarpHandlerDesc HarpHandlerDesc;
typedef struct HarpActorDesc HarpActorDesc;

typedef uint32_t HarpCreatorKind;
typedef uint32_t HarpCreatorFlags;
enum {
    HARP_CREATOR_FLAG_DEFAULT_CREATOR       = (1 << 0),
    HARP_CREATOR_FLAG_EXPERIMENTAL          = (1 << 1),
    HARP_CREATOR_FLAG_RESOLVE_DEPENDENCIES  = (1 << 2)
};

typedef uint32_t HarpStatusFlags;
enum {
    HARP_STATUS_FLAG_VALID         = 1 << 0, // the handler instance is initialized and its state is coherent
    HARP_STATUS_FLAG_AVAILABLE     = 1 << 1, // calls through this handler are currently permitted

    HARP_STATUS_FLAG_INITIALIZING  = 1 << 2, // currently initializing
    HARP_STATUS_FLAG_TERMINATING   = 1 << 3, // currently terminating

    HARP_STATUS_FLAG_FAILED        = 1 << 4, // somehow failed at runtime, not used for now by harp
};

typedef struct HarpCreatorBase HarpCreatorBase;
typedef struct HarpHandlerBase HarpHandlerBase;
typedef struct HarpActorBase HarpActorBase;

typedef struct HarpCoreHandler HarpCoreHandler;
typedef struct HarpExtendedHandler HarpExtendedHandler;

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

    HarpResult (*pfn_register)(HarpCoreHandler*);

    HarpDependencyDesc* p_dependencies;
    uint64_t dependency_count;
};

struct HarpHandlerDesc {
    HarpName name;
    HarpVersion version;
    uint64_t instance_size;
    uint64_t instance_alignment;

    HarpResult (*pfn_init)(HarpCoreHandler*, HarpHandlerBase*, HarpCreatorBase*);
    HarpResult (*pfn_term)(HarpCoreHandler*, HarpHandlerBase*);

    HarpDependencyDesc* p_dependencies;
    uint64_t dependency_count;
};

struct HarpActorDesc {
    HarpName name;
    HarpVersion version;
    uint64_t instance_size;
    uint64_t instance_alignment;

    HarpResult (*pfn_create)(HarpCoreHandler*, HarpActorBase*, HarpCreatorBase*);
    HarpResult (*pfn_destroy)(HarpCoreHandler*, HarpActorBase*);

    HarpName parent_handler;
};


/* ================================================================================ */
/*  BASE STRUCTS                                                                    */
/* ================================================================================ */

struct HarpCreatorBase {
    HarpCreatorKind kind;
    HarpCreatorFlags flags;
};

struct HarpHandlerBase {
    const HarpName name;
    HarpStatusFlags status;
};

struct HarpActorBase {
    const HarpName name;
    HarpStatusFlags status;
};


/* ================================================================================ */
/*  Application Programming Interface                                               */
/* ================================================================================ */

#define HARP_CORE_HANDLER_NAME "HarpCoreHandler"
#define HARP_CORE_HANDLER_VERSION HARP_MAKE_VERSION(1,0,0)

struct HarpCoreHandler {
    HarpHandlerBase _base;

    /* Registration */
    HarpResult (*register_handler)(const HarpCoreHandler *h, const HarpHandlerDesc *desc);
    HarpResult (*register_actor)(const HarpCoreHandler *h, const HarpActorDesc *desc);

    /* Retrieval */
    HarpResult (*get_handler)(const HarpCoreHandler *h, const HarpDependencyDesc *dependency, HarpHandlerBase **out_handler);

    HarpResult (*get_actor_count)(const HarpCoreHandler *h, const HarpName name, uint64_t *out_count);
    HarpResult (*get_actor_at)(const HarpCoreHandler *h, const HarpName name, uint64_t index, HarpActorBase **out_actor);
    HarpResult (*get_actors)(const HarpCoreHandler *h, const HarpName name, uint64_t *inout_count, HarpActorBase **out_actors);

    HarpResult (*get_handler_desc)(const HarpCoreHandler *h, const HarpName name, HarpHandlerDesc **out_desc);
    HarpResult (*get_actor_desc)(const HarpCoreHandler *h, const HarpName name, HarpActorDesc **out_desc);

    /* Lifecycle */
    HarpResult (*handler_initialize)(const HarpCoreHandler *h, const HarpName name, const HarpCreatorBase *creator);
    HarpResult (*handler_terminate)(const HarpCoreHandler *h, const HarpName name);

    HarpResult (*actor_create)(const HarpCoreHandler *h, const HarpName name, const HarpCreatorBase *creator, HarpActorBase **out_actor);
    HarpResult (*actor_destroy)(const HarpCoreHandler *h, const HarpName name, HarpActorBase *actor);

    /* Paths */
    HarpResult (*get_executable_directory)(const HarpCoreHandler *h, const char **out_path);
    HarpResult (*get_working_directory)(const HarpCoreHandler *h, const char **out_path);
    HarpResult (*get_package_directory)(const HarpCoreHandler *h, const HarpName name, const char **out_path);
};

#define HARP_EXTENDED_HANDLER_NAME "HarpExtendedHandler"
#define HARP_EXTENDED_HANDLER_VERSION HARP_MAKE_VERSION(0,0,0)

struct HarpExtendedHandler {
    HarpHandlerBase _base;
    

};


#ifdef __cplusplus
}
#endif

#endif /* HARP_H */
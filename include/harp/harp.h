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
/*  GENERIC TYPES                                                                   */
/* ================================================================================ */

typedef uint32_t HarpResult;
enum {
    HARP_RESULT_OK                  = 0,  // Success
    HARP_RESULT_FAILED              = 1,  // Generic failure
    HARP_RESULT_INVALID_ARGUMENTS   = 10, // One or more arguments are invalid
    HARP_RESULT_MISSING_OUTPUT      = 11, // Output pointer required, but NULL
    HARP_RESULT_NAME_EXISTS         = 20, // The given name already as an entry
    HARP_RESULT_NAME_NOT_FOUND      = 21, // Entry with given name not found
};

typedef const char* HarpName;
typedef uint32_t HarpVersion;


/* ================================================================================ */
/*  BASE STRUCTS                                                                    */
/* ================================================================================ */

typedef struct HarpApiBase {
    HarpVersion version;
    uint8_t available;
} HarpApiBase;

typedef uint8_t HarpCreatorKind;
typedef uint32_t HarpCreatorFlags;
enum {
    HARP_CREATOR_FLAG_DEFAULT_CREATOR       = (1 << 0),
    HARP_CREATOR_FLAG_EXPERIMENTAL          = (1 << 1),
    HARP_CREATOR_FLAG_RESOLVE_DEPENDENCIES  = (1 << 2)
};
typedef struct HarpCreatorBase {
    HarpCreatorKind kind;
    HarpCreatorFlags flags;
} HarpCreatorBase;

typedef struct HarpHandlerBase {
    HarpName name;
} HarpHandlerBase;

typedef struct HarpActorBase {
    HarpHandlerBase *p_parent;
} HarpActorBase;


/* ================================================================================ */
/*  DESCRIPTORS                                                                     */
/* ================================================================================ */

typedef struct HarpPluginDesc {
    HarpName name;
    HarpVersion version;

    HarpResult (*pfn_register)(HarpApiBase *);
    HarpResult (*pfn_init)(HarpApiBase *);
    HarpResult (*pfn_term)(HarpApiBase *);

    HarpName* p_required_plugins;
    HarpVersion* p_required_versions;
    uint64_t required_count;
} HarpPluginDesc;

typedef struct HarpApiDesc {
    HarpVersion version;
    uint64_t instance_size;
    HarpName name;
} HarpApiDesc;

typedef struct HarpHandlerDesc {
    HarpVersion version;
    uint64_t instance_size;

    HarpResult (*pfn_init)(struct HarpCoreApi*, HarpHandlerBase*, HarpCreatorBase*);
    HarpResult (*pfn_term)(struct HarpCoreApi*, HarpHandlerBase*);

    HarpName name;
    HarpName* p_dep_names;
    HarpVersion* p_dep_versions;
    uint64_t dep_count;
} HarpHandlerDesc;

typedef struct HarpActorDesc {
    HarpVersion version;
    uint64_t instance_size;

    HarpResult (*pfn_create)(struct HarpCoreApi*, HarpActorBase*, HarpCreatorBase*);
    HarpResult (*pfn_destroy)(struct HarpCoreApi*, HarpActorBase*);

    HarpName name;
    HarpName parent;
} HarpActorDesc;


/* ================================================================================ */
/*  CORE API                                                                        */
/* ================================================================================ */

#define HARP_CORE_API_NAME "HarpCoreApi"
#define HARP_CORE_API_VERSION HARP_MAKE_VERSION(1,0,0)

typedef struct HarpCoreApi {
    HarpApiBase base;

    /* Registration */
    HarpResult (*register_api)(const HarpApiDesc* desc, HarpApiBase** out_api);
    HarpResult (*register_handler)(const HarpHandlerDesc* desc);
    HarpResult (*register_actor)(const HarpActorDesc* desc);

    /* Retrieval */
    HarpResult (*get_api)(const HarpName name, HarpApiBase **out_api);
    HarpResult (*get_handler)(const HarpName name, HarpHandlerBase** out_handler);
    HarpResult (*get_api_desc)(const HarpName name, HarpApiDesc **out_desc);
    HarpResult (*get_handler_desc)(const HarpName name, HarpHandlerDesc** out_desc);
    HarpResult (*get_actor_desc)(const HarpName name, HarpActorDesc** out_desc);

    /* Lifecycle */
    HarpResult (*handler_initialize)(const HarpName name, const HarpCreatorBase* creator);
    HarpResult (*handler_terminate)(const HarpName name);

    HarpResult (*actor_create)(const HarpName name, const HarpCreatorBase* creator, HarpActorBase** out_actor);
    HarpResult (*actor_destroy)(const HarpName name, HarpActorBase* actor);
} HarpCoreApi;


/* ================================================================================ */
/*  EXTENDED API                                                                    */
/* ================================================================================ */

#define HARP_EXTENDED_API_NAME "HarpExtendedApi"
#define HARP_EXTENDED_API_VERSION HARP_MAKE_VERSION(0,0,0)

typedef struct HarpExtendedApi {
    HarpApiBase base;
    // Extendable API for plugin-specific extensions
} HarpExtendedApi;


#define HARP_UTILS_UNDEF
#include "utils/harp_version.h"
#undef HARP_UTILS_UNDEF


#ifdef __cplusplus
}
#endif

#endif /* HARP_H */
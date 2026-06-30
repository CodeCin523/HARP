#ifndef UTILS_HARP_HELPERS_H
#define UTILS_HARP_HELPERS_H

#ifdef __cplusplus
extern "C" {
#endif

#include <harp/harp.h>
#include <harp/utils/harp_build.h>


/* ================================================================================ */
/*  GENERIC                                                                         */
/* ================================================================================ */

#define HARP_UNUSED(x) ((void)(x))


/* ================================================================================ */
/*  CHECKS                                                                          */
/* ================================================================================ */

#if HARP_DEBUG
#include <assert.h>
#define HARP_CHECK_CRITICAL(condition) assert(condition)
#else
#define HARP_CHECK_CRITICAL(condition) ((void)0)
#endif

#define HARP_CHECK_ARG(condition, ret)   do { if(!(condition)) return (ret); } while(0)
#define HARP_CHECK_STATE(condition, ret) do { if(!(condition)) return (ret); } while(0)


/* ================================================================================ */
/*  CASTS                                                                           */
/* ================================================================================ */

#define HARP_HANDLER_AS(type, ptr) ((type *)(ptr))
#define HARP_ACTOR_AS(type, ptr)   ((type *)(ptr))


/* ================================================================================ */
/*  STATUS                                                                          */
/* ================================================================================ */

#define HARP_STATUS_IS_VALID(status) \
    (((status) & HARP_STATUS_FLAG_VALID) != 0)

#define HARP_STATUS_IS_SERVING(status) \
    (((status) & HARP_STATUS_FLAG_SERVING) != 0)

#define HARP_STATUS_IS_INITIALIZING(status) \
    (((status) & HARP_STATUS_FLAG_INITIALIZING) != 0)

#define HARP_STATUS_IS_TERMINATING(status) \
    (((status) & HARP_STATUS_FLAG_TERMINATING) != 0)

#define HARP_STATUS_IS_FAILED(status) \
    (((status) & HARP_STATUS_FLAG_FAILED) != 0)

#define HARP_STATUS_IS_HEALTHY(status) \
    (HARP_STATUS_IS_VALID(status) && \
     HARP_STATUS_IS_SERVING(status) && \
     !HARP_STATUS_IS_FAILED(status))


/* ================================================================================ */
/*  HANDLER                                                                         */
/* ================================================================================ */

#define HARP_HANDLER_IS_VALID(obj) \
    ((obj) != NULL && \
     HARP_STATUS_IS_VALID(HARP_HANDLER_AS(HarpHandlerBase, obj)->status))

#define HARP_HANDLER_IS_SERVING(obj) \
    ((obj) != NULL && \
     HARP_STATUS_IS_SERVING(HARP_HANDLER_AS(HarpHandlerBase, obj)->status))

#define HARP_HANDLER_IS_INITIALIZING(obj) \
    ((obj) != NULL && \
     HARP_STATUS_IS_INITIALIZING(HARP_HANDLER_AS(HarpHandlerBase, obj)->status))

#define HARP_HANDLER_IS_TERMINATING(obj) \
    ((obj) != NULL && \
     HARP_STATUS_IS_TERMINATING(HARP_HANDLER_AS(HarpHandlerBase, obj)->status))

#define HARP_HANDLER_IS_FAILED(obj) \
    ((obj) != NULL && \
     HARP_STATUS_IS_FAILED(HARP_HANDLER_AS(HarpHandlerBase, obj)->status))

#define HARP_HANDLER_IS_HEALTHY(obj) \
    ((obj) != NULL && \
     HARP_STATUS_IS_HEALTHY(HARP_HANDLER_AS(HarpHandlerBase, obj)->status))


/* ================================================================================ */
/*  ACTOR                                                                           */
/* ================================================================================ */

#define HARP_ACTOR_IS_VALID(obj) \
    ((obj) != NULL && \
     HARP_STATUS_IS_VALID(HARP_ACTOR_AS(HarpActorBase, obj)->status))

#define HARP_ACTOR_IS_SERVING(obj) \
    ((obj) != NULL && \
     HARP_STATUS_IS_SERVING(HARP_ACTOR_AS(HarpActorBase, obj)->status))

#define HARP_ACTOR_IS_INITIALIZING(obj) \
    ((obj) != NULL && \
     HARP_STATUS_IS_INITIALIZING(HARP_ACTOR_AS(HarpActorBase, obj)->status))

#define HARP_ACTOR_IS_TERMINATING(obj) \
    ((obj) != NULL && \
     HARP_STATUS_IS_TERMINATING(HARP_ACTOR_AS(HarpActorBase, obj)->status))

#define HARP_ACTOR_IS_FAILED(obj) \
    ((obj) != NULL && \
     HARP_STATUS_IS_FAILED(HARP_ACTOR_AS(HarpActorBase, obj)->status))

#define HARP_ACTOR_IS_HEALTHY(obj) \
    ((obj) != NULL && \
     HARP_STATUS_IS_HEALTHY(HARP_ACTOR_AS(HarpActorBase, obj)->status))


/* ================================================================================ */
/*  HELPERS                                                                         */
/* ================================================================================ */

#define HARP_DEPENDENCY(dep_name, min_ver, max_ver) \
    (HarpDependencyDesc){ \
        .name = (dep_name), \
        .min_version = (min_ver), \
        .max_version = (max_ver) \
    }

#define HARP_DEFAULT_CREATOR \
    (HarpCreatorBase){ \
        .kind = 0, \
        .flags = HARP_CREATOR_FLAG_DEFAULT_CREATOR \
    }


#ifdef __cplusplus
}
#endif

#endif /* UTILS_HARP_HELPERS_H */
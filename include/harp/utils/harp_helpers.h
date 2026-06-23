#ifndef UTILS_HARP_HELPERS_H
#define UTILS_HARP_HELPERS_H

#ifdef __cplusplus
extern "C" {
#endif

#include <harp/harp.h>


/* ================================================================================ */
/*  GENERIC                                                                         */
/* ================================================================================ */

#define HARP_SUCCEEDED(res) ((res) == HARP_RESULT_OK)
#define HARP_FAILED(res)    ((res) != HARP_RESULT_OK)

#define HARP_UNUSED(x) ((void)(x))


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

#define HARP_DEPENDENCY(name, min_ver, max_ver) \
    (HarpDependencyDesc){ \
        .name = (name), \
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
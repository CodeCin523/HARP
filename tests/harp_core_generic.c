#include <assert.h>
#include <stdio.h>
#include <string.h>
#include <stdalign.h>

#include <harp/harp_core.h>

/* ================================================================================ */
/*  TEST OBJECTS                                                                    */
/* ================================================================================ */

typedef struct {
    HarpHandlerBase _base;
    uint32_t initialized;
} TestHandler;

typedef struct {
    HarpActorBase _base;
    uint32_t created;
} TestActor;

/* ================================================================================ */
/*  CALLBACKS                                                                       */
/* ================================================================================ */

static HarpResult test_handler_init(
    HarpCoreHandler *core,
    HarpHandlerBase *handler,
    HarpCreatorBase *creator
) {
    (void)core;
    (void)creator;

    TestHandler *test = (TestHandler*)handler;
    test->initialized = 1;

    return HARP_RESULT_OK;
}

static HarpResult test_handler_term(
    HarpCoreHandler *core,
    HarpHandlerBase *handler
) {
    (void)core;

    TestHandler *test = (TestHandler*)handler;
    test->initialized = 0;

    return HARP_RESULT_OK;
}

static HarpResult test_actor_create(
    HarpCoreHandler *core,
    HarpActorBase *actor,
    HarpCreatorBase *creator
) {
    (void)core;
    (void)creator;

    TestActor *test = (TestActor*)actor;
    test->created = 1;

    return HARP_RESULT_OK;
}

static HarpResult test_actor_destroy(
    HarpCoreHandler *core,
    HarpActorBase *actor
) {
    (void)core;

    TestActor *test = (TestActor*)actor;
    test->created = 0;

    return HARP_RESULT_OK;
}

/* ================================================================================ */
/*  MAIN                                                                            */
/* ================================================================================ */

int main(int argc, char **argv) {
    printf("=== HARP CORE TEST ===\n");

    /* ------------------------------------------------------------------------ */
    /* Runtime                                                                   */
    /* ------------------------------------------------------------------------ */

    HarpRuntime *runtime = NULL;

    HarpRuntimeCreator creator = {
        .argv0 = argv[0]
    };

    assert(
        harp_initialize((HarpCreatorBase *)&creator, &runtime) == HARP_RESULT_OK
    );

    assert(runtime != NULL);
    printf("[OK] runtime initialize\n");

    /* ------------------------------------------------------------------------ */
    /* Core Handler                                                             */
    /* ------------------------------------------------------------------------ */

    HarpDependencyDesc dep = {
        .name = HARP_CORE_HANDLER_NAME,
        .min_version = 0,
        .max_version = UINT32_MAX
    };
    
    HarpHandlerBase *base = NULL;
    
    assert(
        harp_runtime_get_handler(runtime, &dep, &base) == HARP_RESULT_OK
    );
    
    HarpCoreHandler *core = (HarpCoreHandler *)base;
    assert(core != NULL);

    printf("[OK] get core handler\n");

    /* ------------------------------------------------------------------------ */
    /* Register Handler                                                          */
    /* ------------------------------------------------------------------------ */

    static HarpHandlerDesc handler_desc = {
        .name = "test_handler",
        .version = HARP_MAKE_VERSION(1, 0, 0),

        .instance_size = sizeof(TestHandler),
        .instance_alignment = alignof(TestHandler),

        .pfn_init = test_handler_init,
        .pfn_term = test_handler_term,

        .dependency_count = 0,
        .p_dependencies = NULL
    };

    assert(
        core->register_handler(core, &handler_desc) == HARP_RESULT_OK
    );

    printf("[OK] register handler\n");

    /* ------------------------------------------------------------------------ */
    /* Get Handler                                                               */
    /* ------------------------------------------------------------------------ */

    HarpDependencyDesc dep_desc = {
        .name = "test_handler",
        .min_version = 0,
        .max_version = UINT32_MAX
    };

    HarpHandlerBase *handler_base = NULL;

    assert(
        core->get_handler(core, &dep_desc, &handler_base) == HARP_RESULT_OK
    );

    assert(handler_base != NULL);

    TestHandler *handler = (TestHandler*)handler_base;

    printf("[OK] get handler\n");

    /* ------------------------------------------------------------------------ */
    /* Get Handler Desc                                                          */
    /* ------------------------------------------------------------------------ */

    HarpHandlerDesc *handler_desc_out = NULL;

    assert(
        core->get_handler_desc(core, "test_handler", &handler_desc_out)
        == HARP_RESULT_OK
    );

    assert(handler_desc_out != NULL);
    assert(strcmp(handler_desc_out->name, "test_handler") == 0);

    printf("[OK] get handler desc\n");

    /* ------------------------------------------------------------------------ */
    /* Initialize Handler                                                        */
    /* ------------------------------------------------------------------------ */

    HarpCreatorBase handler_creator = {0};

    assert(
        core->handler_initialize(core, "test_handler", &handler_creator)
        == HARP_RESULT_OK
    );

    assert(handler->initialized == 1);

    printf("[OK] initialize handler\n");

    /* ------------------------------------------------------------------------ */
    /* Handler Set Serving                                                       */
    /* ------------------------------------------------------------------------ */

    // Fresh from initialize, the handler is already VALID + SERVING.
    // Re-asserting SERVING=1 on an already-serving handler is a no-op
    // transition and must be rejected.
    assert(
        core->handler_set_serving(core, handler_base, 1)
        == HARP_RESULT_OK
    );

    // Pausing a serving handler is a legal transition.
    assert(
        core->handler_set_serving(core, handler_base, 0)
        == HARP_RESULT_OK
    );

    assert((handler_base->status & HARP_STATUS_FLAG_SERVING) == 0);

    // Clearing SERVING again while already cleared must be rejected.
    assert(
        core->handler_set_serving(core, handler_base, 0)
        == HARP_RESULT_INVALID_STATE
    );

    // Resuming a paused handler is a legal transition.
    assert(
        core->handler_set_serving(core, handler_base, 1)
        == HARP_RESULT_OK
    );

    assert((handler_base->status & HARP_STATUS_FLAG_SERVING) != 0);

    printf("[OK] handler set serving\n");

    /* ------------------------------------------------------------------------ */
    /* Handler Set Failed                                                        */
    /* ------------------------------------------------------------------------ */

    assert((handler_base->status & HARP_STATUS_FLAG_FAILED) == 0);

    // Reporting failure on a healthy handler is a legal transition.
    assert(
        core->handler_set_failed(core, handler_base, 1)
        == HARP_RESULT_OK
    );

    assert((handler_base->status & HARP_STATUS_FLAG_FAILED) != 0);

    // Re-asserting FAILED=1 on an already-failed handler must be rejected.
    assert(
        core->handler_set_failed(core, handler_base, 1)
        == HARP_RESULT_INVALID_STATE
    );

    // Clearing FAILED is a legal transition.
    assert(
        core->handler_set_failed(core, handler_base, 0)
        == HARP_RESULT_OK
    );

    assert((handler_base->status & HARP_STATUS_FLAG_FAILED) == 0);

    // Clearing FAILED again while already clear must be rejected.
    assert(
        core->handler_set_failed(core, handler_base, 0)
        == HARP_RESULT_INVALID_STATE
    );

    printf("[OK] handler set failed\n");

    /* ------------------------------------------------------------------------ */
    /* Register Actor                                                            */
    /* ------------------------------------------------------------------------ */

    static HarpActorDesc actor_desc = {
        .name = "test_actor",
        .version = HARP_MAKE_VERSION(1, 0, 0),

        .instance_size = sizeof(TestActor),
        .instance_alignment = alignof(TestActor),

        .parent_handler = "test_handler",

        .pfn_create = test_actor_create,
        .pfn_destroy = test_actor_destroy
    };

    assert(
        core->register_actor(core, &actor_desc) == HARP_RESULT_OK
    );

    printf("[OK] register actor\n");

    /* ------------------------------------------------------------------------ */
    /* Get Actor Desc                                                            */
    /* ------------------------------------------------------------------------ */

    HarpActorDesc *actor_desc_out = NULL;

    assert(
        core->get_actor_desc(core, "test_actor", &actor_desc_out)
        == HARP_RESULT_OK
    );

    assert(actor_desc_out != NULL);
    assert(strcmp(actor_desc_out->name, "test_actor") == 0);

    printf("[OK] get actor desc\n");

    /* ------------------------------------------------------------------------ */
    /* Create Actor                                                              */
    /* ------------------------------------------------------------------------ */

    HarpActorBase *actor_base = NULL;

    HarpCreatorBase actor_creator = {0};

    assert(
        core->actor_create(core, "test_actor", &actor_creator, &actor_base)
        == HARP_RESULT_OK
    );

    assert(actor_base != NULL);

    TestActor *actor = (TestActor*)actor_base;
    assert(actor->created == 1);

    printf("[OK] create actor\n");

    /* ------------------------------------------------------------------------ */
    /* Actor Set Serving                                                        */
    /* ------------------------------------------------------------------------ */

    // Fresh from create, the actor is already VALID + SERVING.
    assert(
        core->actor_set_serving(core, actor_base, 1)
        == HARP_RESULT_OK
    );

    assert(
        core->actor_set_serving(core, actor_base, 0)
        == HARP_RESULT_OK
    );

    assert((actor_base->status & HARP_STATUS_FLAG_SERVING) == 0);

    assert(
        core->actor_set_serving(core, actor_base, 0)
        == HARP_RESULT_INVALID_STATE
    );

    assert(
        core->actor_set_serving(core, actor_base, 1)
        == HARP_RESULT_OK
    );

    assert((actor_base->status & HARP_STATUS_FLAG_SERVING) != 0);

    printf("[OK] actor set serving\n");

    /* ------------------------------------------------------------------------ */
    /* Actor Set Failed                                                         */
    /* ------------------------------------------------------------------------ */

    assert((actor_base->status & HARP_STATUS_FLAG_FAILED) == 0);

    assert(
        core->actor_set_failed(core, actor_base, 1)
        == HARP_RESULT_OK
    );

    assert((actor_base->status & HARP_STATUS_FLAG_FAILED) != 0);

    assert(
        core->actor_set_failed(core, actor_base, 1)
        == HARP_RESULT_INVALID_STATE
    );

    assert(
        core->actor_set_failed(core, actor_base, 0)
        == HARP_RESULT_OK
    );

    assert((actor_base->status & HARP_STATUS_FLAG_FAILED) == 0);

    assert(
        core->actor_set_failed(core, actor_base, 0)
        == HARP_RESULT_INVALID_STATE
    );

    printf("[OK] actor set failed\n");

    /* ------------------------------------------------------------------------ */
    /* Get Actor Count                                                          */
    /* ------------------------------------------------------------------------ */

    uint64_t actor_count = 0;

    assert(
        core->get_actor_count(core, "test_actor", &actor_count)
        == HARP_RESULT_OK
    );

    assert(actor_count == 1);

    printf("[OK] get actor count\n");

    /* ------------------------------------------------------------------------ */
    /* Get Actor At                                                             */
    /* ------------------------------------------------------------------------ */

    HarpActorBase *actor_at = NULL;

    assert(
        core->get_actor_at(core, "test_actor", 0, &actor_at)
        == HARP_RESULT_OK
    );

    assert(actor_at == actor_base);

    printf("[OK] get actor at\n");

    /* ------------------------------------------------------------------------ */
    /* Get Actors                                                               */
    /* ------------------------------------------------------------------------ */

    uint64_t actor_array_count = 0;

    assert(
        core->get_actors(core, "test_actor", &actor_array_count, NULL)
        == HARP_RESULT_OK
    );

    assert(actor_array_count == 1);

    HarpActorBase *actors[1] = {0};

    assert(
        core->get_actors(core, "test_actor", &actor_array_count, actors)
        == HARP_RESULT_OK
    );

    assert(actor_array_count == 1);
    assert(actors[0] == actor_base);

    printf("[OK] get actors\n");

    /* ------------------------------------------------------------------------ */
    /* Destroy Actor                                                             */
    /* ------------------------------------------------------------------------ */

    assert(
        core->actor_destroy(core, "test_actor", actor_base)
        == HARP_RESULT_OK
    );

    printf("[OK] destroy actor\n");

    /* ------------------------------------------------------------------------ */
    /* Terminate Handler                                                         */
    /* ------------------------------------------------------------------------ */

    assert(
        core->handler_terminate(core, "test_handler")
        == HARP_RESULT_OK
    );

    // HarpHandlerBase *handler_base2 = NULL;
    // core->get_handler(core, &dep_desc, &handler_base2);
    // printf("handler ptr original: %p\n", (void*)handler);
    // printf("handler ptr re-fetched: %p\n", (void*)handler_base2);

    assert(handler->initialized == 0);

    printf("[OK] terminate handler\n");

    /* ------------------------------------------------------------------------ */
    /* Terminate Runtime                                                         */
    /* ------------------------------------------------------------------------ */

    assert(
        harp_terminate(runtime) == HARP_RESULT_OK
    );

    printf("[OK] runtime terminate\n");

    printf("\nALL TESTS PASSED\n");

    return 0;
}

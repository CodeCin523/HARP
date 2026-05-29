#include <assert.h>
#include <stdio.h>
#include <string.h>
#include <stdalign.h>

#include <harp/harp_core.h>
#include <harp/utils/harp_api.h>


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
    HarpCoreApi *api,
    HarpHandlerBase *handler,
    HarpCreatorBase *creator
) {
    (void)api;
    (void)creator;

    TestHandler *test = (TestHandler*)handler;
    test->initialized = 1;

    return HARP_RESULT_OK;
}

static HarpResult test_handler_term(
    HarpCoreApi *api,
    HarpHandlerBase *handler
) {
    (void)api;

    TestHandler *test = (TestHandler*)handler;
    test->initialized = 0;

    return HARP_RESULT_OK;
}

static HarpResult test_actor_create(
    HarpCoreApi *api,
    HarpActorBase *actor,
    HarpCreatorBase *creator
) {
    (void)api;
    (void)creator;

    TestActor *test = (TestActor*)actor;
    test->created = 1;

    return HARP_RESULT_OK;
}

static HarpResult test_actor_destroy(
    HarpCoreApi *api,
    HarpActorBase *actor
) {
    (void)api;

    TestActor *test = (TestActor*)actor;
    test->created = 0;

    return HARP_RESULT_OK;
}


/* ================================================================================ */
/*  MAIN                                                                            */
/* ================================================================================ */

int main() {
    printf("=== HARP CORE TEST ===\n");

    /* ------------------------------------------------------------------------ */
    /* Runtime                                                                   */
    /* ------------------------------------------------------------------------ */

    HarpRuntime *runtime = NULL;

    assert(
        harp_initialize(&runtime) == HARP_RESULT_OK
    );

    assert(runtime != NULL);

    printf("[OK] runtime initialize\n");


    /* ------------------------------------------------------------------------ */
    /* Core API                                                                  */
    /* ------------------------------------------------------------------------ */

    HarpApiBase *core_api_base = NULL;

    assert(
        harp_runtime_get_api(
            runtime,
            HARP_CORE_API_NAME,
            &core_api_base
        ) == HARP_RESULT_OK
    );

    assert(core_api_base != NULL);
    assert(core_api_base->available == 1);

    HarpCoreApi *core_api = (HarpCoreApi*)core_api_base;

    printf("[OK] get core api\n");


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
        core_api->register_handler(
            (HarpHandlerBase*)runtime,
            &handler_desc
        ) == HARP_RESULT_OK
    );

    printf("[OK] register handler\n");


    /* ------------------------------------------------------------------------ */
    /* Get Handler                                                               */
    /* ------------------------------------------------------------------------ */

    HarpHandlerBase *handler_base = NULL;

    assert(
        core_api->get_handler(
            (HarpHandlerBase*)runtime,
            "test_handler",
            &handler_base
        ) == HARP_RESULT_OK
    );

    assert(handler_base != NULL);

    TestHandler *handler = (TestHandler*)handler_base;

    printf("[OK] get handler\n");


    /* ------------------------------------------------------------------------ */
    /* Get Handler Desc                                                          */
    /* ------------------------------------------------------------------------ */

    HarpHandlerDesc *handler_desc_out = NULL;

    assert(
        core_api->get_handler_desc(
            (HarpHandlerBase*)runtime,
            "test_handler",
            &handler_desc_out
        ) == HARP_RESULT_OK
    );

    assert(handler_desc_out != NULL);
    assert(strcmp(handler_desc_out->name, "test_handler") == 0);

    printf("[OK] get handler desc\n");


    /* ------------------------------------------------------------------------ */
    /* Initialize Handler                                                        */
    /* ------------------------------------------------------------------------ */

    HarpCreatorBase handler_creator = {0};

    assert(
        core_api->handler_initialize(
            (HarpHandlerBase*)runtime,
            "test_handler",
            &handler_creator
        ) == HARP_RESULT_OK
    );

    assert(handler->initialized == 1);

    printf("[OK] initialize handler\n");


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
        core_api->register_actor(
            (HarpHandlerBase*)runtime,
            &actor_desc
        ) == HARP_RESULT_OK
    );

    printf("[OK] register actor\n");


    /* ------------------------------------------------------------------------ */
    /* Get Actor Desc                                                            */
    /* ------------------------------------------------------------------------ */

    HarpActorDesc *actor_desc_out = NULL;

    assert(
        core_api->get_actor_desc(
            (HarpHandlerBase*)runtime,
            "test_actor",
            &actor_desc_out
        ) == HARP_RESULT_OK
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
        core_api->actor_create(
            (HarpHandlerBase*)runtime,
            "test_actor",
            &actor_creator,
            &actor_base
        ) == HARP_RESULT_OK
    );

    assert(actor_base != NULL);

    TestActor *actor = (TestActor*)actor_base;

    assert(actor->created == 1);

    printf("[OK] create actor\n");


    /* ------------------------------------------------------------------------ */
    /* Destroy Actor                                                             */
    /* ------------------------------------------------------------------------ */

    assert(
        core_api->actor_destroy(
            (HarpHandlerBase*)runtime,
            "test_actor",
            actor_base
        ) == HARP_RESULT_OK
    );

    printf("[OK] destroy actor\n");


    /* ------------------------------------------------------------------------ */
    /* Terminate Handler                                                         */
    /* ------------------------------------------------------------------------ */

    assert(
        core_api->handler_terminate(
            (HarpHandlerBase*)runtime,
            "test_handler"
        ) == HARP_RESULT_OK
    );

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
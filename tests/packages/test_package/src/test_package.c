#include <harp/harp.h>

#include <test_package/test_api.h>

#include <stdalign.h>

/* ========================================================= */
/* Handler instance                                           */
/* ========================================================= */

typedef struct TestHandler {
    HarpHandlerBase _base;
} TestHandler;

/* ========================================================= */
/* Implementation                                             */
/* ========================================================= */

static int test_add(
    TestApi *self,
    int a,
    int b
) {
    (void)self;
    return a + b;
}

/* ========================================================= */
/* Handler lifecycle                                          */
/* ========================================================= */

static HarpResult test_handler_init(
    HarpCoreHandler *core,
    HarpHandlerBase *handler,
    HarpCreatorBase *creator
) {
    (void)core;
    (void)handler;
    (void)creator;

    return HARP_RESULT_OK;
}

static HarpResult test_handler_term(
    HarpCoreHandler *core,
    HarpHandlerBase *handler
) {
    (void)core;
    (void)handler;

    return HARP_RESULT_OK;
}

/* ========================================================= */
/* Package registration                                       */
/* ========================================================= */

static HarpResult test_package_register(
    HarpCoreHandler *core
) {
    /* ----------------------------------------------------- */
    /* Register handler                                      */
    /* ----------------------------------------------------- */

    HarpHandlerDesc handler_desc = {
        .name = TEST_API_NAME,
        .version = TEST_API_VERSION,

        .instance_size = sizeof(TestApi),
        .instance_alignment = alignof(TestApi),

        .pfn_init = test_handler_init,
        .pfn_term = test_handler_term,

        .p_dependencies = NULL,
        .dependency_count = 0
    };

    HarpHandlerBase *base = NULL;

    HarpResult res =
        core->register_handler(
            core,
            &handler_desc,
            &base
        );

    if (res != HARP_RESULT_OK)
        return res;

    if (base == NULL)
        return HARP_RESULT_CRITICAL_FAIL;

    /* ----------------------------------------------------- */
    /* Bind API (handler = API in your new model)            */
    /* ----------------------------------------------------- */

    TestApi *api = (TestApi *)base;

    api->add = test_add;

    return HARP_RESULT_OK;
}

/* ========================================================= */
/* Package entry                                              */
/* ========================================================= */

HarpResult harp_package_query(
    HarpPackageDesc **out_desc
) {
    static HarpPackageDesc desc = {
        .name = "test_package",
        .version = HARP_MAKE_VERSION(1, 0, 0),

        .pfn_register = test_package_register,

        .p_dependencies = NULL,
        .dependency_count = 0
    };

    *out_desc = &desc;
    return HARP_RESULT_OK;
}
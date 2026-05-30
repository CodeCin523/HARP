#include <harp/harp.h>
#include <harp/utils/harp_api.h>

#include <test_package/test_api.h>

#include <stdalign.h>


typedef struct TestHandler {
    HarpHandlerBase _base;
} TestHandler;


static int test_add(
    int a,
    int b
) {
    return a + b;
}


static HarpResult test_handler_init(
    HarpCoreApi *api,
    HarpHandlerBase *handler,
    HarpCreatorBase *creator
) {
    (void)api;
    (void)handler;
    (void)creator;

    return HARP_RESULT_OK;
}


static HarpResult test_handler_term(
    HarpCoreApi *api,
    HarpHandlerBase *handler
) {
    (void)api;
    (void)handler;

    return HARP_RESULT_OK;
}


static HarpResult test_package_register(
    HarpApiBase *api_base
) {
    HarpCoreApi *core =
        HARP_API_AS(HarpCoreApi, api_base);

    /* API */

    HarpApiDesc api_desc = {
        .name = TEST_API_NAME,
        .version = TEST_API_VERSION,

        .instance_size = sizeof(TestApi),
        .instance_alignment = alignof(TestApi)
    };

    HarpApiBase *base = NULL;

    HarpResult res =
        core->register_api(
            api_base->p_handler,
            &api_desc,
            &base
        );

    if(res != HARP_RESULT_OK)
        return res;

    TestApi *api =
        HARP_API_AS(TestApi, base);

    api->add = test_add;

    /* handler */

    HarpHandlerDesc handler_desc = {
        .name = "test_handler",
        .version = HARP_MAKE_VERSION(1, 0, 0),

        .instance_size = sizeof(TestHandler),
        .instance_alignment = alignof(TestHandler),

        .pfn_init = test_handler_init,
        .pfn_term = test_handler_term
    };

    return core->register_handler(
        api_base->p_handler,
        &handler_desc
    );
}


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
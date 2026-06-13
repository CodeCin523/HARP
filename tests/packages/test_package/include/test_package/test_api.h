#ifndef TEST_PACKAGE_TEST_API_H
#define TEST_PACKAGE_TEST_API_H

#include <harp/harp.h>

#ifdef __cplusplus
extern "C" {
#endif

#define TEST_API_NAME "test_api"
#define TEST_API_VERSION HARP_MAKE_VERSION(1, 0, 0)

/*
    This is now a HANDLER, not an API.
    It is just a typed view over HarpHandlerBase.
*/
typedef struct TestApi TestApi;

struct TestApi {
    HarpHandlerBase _base;

    int (*add)(
        TestApi *self,
        int a,
        int b
    );
};

#ifdef __cplusplus
}
#endif

#endif
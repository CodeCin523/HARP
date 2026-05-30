#ifndef TEST_PACKAGE_TEST_API_H
#define TEST_PACKAGE_TEST_API_H

#include <harp/harp.h>

#ifdef __cplusplus
extern "C" {
#endif

#define TEST_API_NAME "test_api"
#define TEST_API_VERSION HARP_MAKE_VERSION(1, 0, 0)

typedef struct TestApi {
    HarpApiBase _base;

    int (*add)(
        int a,
        int b
    );

} TestApi;

#ifdef __cplusplus
}
#endif

#endif
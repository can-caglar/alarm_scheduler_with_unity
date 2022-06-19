#include "Spy_FakeFunctionsToCall.h"
#include <stdio.h>

static void_fn lastFn;

void ff_create(void)
{
    lastFn = 0;
}

void fn1(void)
{
    lastFn = fn1;
}

void fn2(void)
{
    lastFn = fn2;
}

void_fn spyff_getLastFunctionCalled(void)
{
    void_fn ret = lastFn;
    lastFn = 0;
    return ret;
}
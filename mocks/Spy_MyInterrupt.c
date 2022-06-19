#include "MyInterrupt.h"

static void_fn lastFn;
static int lastPeriod;

void my_int_create(void)
{
    lastFn = 0;
    lastPeriod = 0;
}

void my_int_setup(void_fn function, int period)
{
    lastFn = function;
    lastPeriod = period;
}

void_fn spymi_getLastFn(void)
{
    return lastFn;
}

int spymi_getLastPeriod(void)
{
    return lastPeriod;
}

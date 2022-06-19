#include "unity.h"

#include "Spy_MyInterrupt.h"

void setUp(void)
{
}

void tearDown(void)
{
}

void test_MyInterrupt_Returns0AtCreation(void)
{
    TEST_ASSERT_EQUAL((void_fn)0, spymi_getLastFn());
    TEST_ASSERT_EQUAL(0, spymi_getLastPeriod());
}

void test_MyInterrupt_ReturnsLastFunction(void)
{
    my_int_setup((void_fn)0xBEEF, 60);
    TEST_ASSERT_EQUAL(0xBEEF, spymi_getLastFn());
    TEST_ASSERT_EQUAL(60, spymi_getLastPeriod());
}

#include "unity.h"

#include "Spy_FakeFunctionsToCall.h"

void setUp(void)
{
    ff_create();
}

void tearDown(void)
{
}

void test_FakeFunctionAtStart(void)
{
    TEST_ASSERT_EQUAL_INT(0, spyff_getLastFunctionCalled());
}

void test_FakeFunctionReturnsLastFuncCalled(void)
{
    fn1();
    TEST_ASSERT_EQUAL_INT(fn1, spyff_getLastFunctionCalled());

    fn2();
    TEST_ASSERT_EQUAL_INT(fn2, spyff_getLastFunctionCalled());
}

void test_FakeFunctionReturns0AfterRecreation(void)
{
    fn1();
    ff_create();
    TEST_ASSERT_EQUAL_INT(0, spyff_getLastFunctionCalled());
}

void test_FakeFunctionClearsLastFunctionCalledOnceItIsRead(void)
{
    fn1();
    TEST_ASSERT_EQUAL_INT(fn1, spyff_getLastFunctionCalled());
    TEST_ASSERT_EQUAL_INT(0, spyff_getLastFunctionCalled());
}

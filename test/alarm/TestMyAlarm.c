#include <stdio.h>
#include "unity.h"
#include "MyAlarm.h"
#include "Spy_MyInterrupt.h"
#include "Spy_FakeFunctionsToCall.h"

const int alarmPeriod = 100;    // ms
char testDebugString[256] = "";
const int tenSecondsInMs = 10000; // ms

enum
{
    NO_FUNCTION = 0,
    CALL_EVERY_PERIOD = 0,
};

typedef enum
{
    NOT_PERIODIC,
    PERIODIC
} periodic_e;

static void assert_func_calls_over_some_period(int msToPingFor, periodic_e isPeriodic, int expectedCallTime, void(*func)(void));

void setUp(void)
{
    *testDebugString = 0;
    my_alarm_create();
}

void tearDown(void)
{
}

void test_MyAlarm_100ms_ping_registered_at_boot(void)
{
    TEST_ASSERT_EQUAL_INT(alarmPeriod, spymi_getLastPeriod());
    TEST_ASSERT_EQUAL_PTR(my_alarm_ping, spymi_getLastFn());
}

void test_MyAlarm_NoActionRegisteredCallsNothing(void)
{
    TEST_ASSERT_EQUAL_PTR(0, spyff_getLastFunctionCalled());
    assert_func_calls_over_some_period(tenSecondsInMs, PERIODIC, CALL_EVERY_PERIOD, NO_FUNCTION);
}

void test_MyAlarm_RegisteredActionOnlyCalledWhenPeriodIsRight_MultipleOf100()
{
    int period = 300;
    TEST_ASSERT_EQUAL_INT(ALL_OK, my_alarm_register_function(fn1, period, 0));
    assert_func_calls_over_some_period(tenSecondsInMs, PERIODIC, period, fn1);
}

void test_MyAlarm_IfPeriodLessThan100ItIsCalledAlways()
{
    int period = 99;
    TEST_ASSERT_EQUAL_INT(ALL_OK, my_alarm_register_function(fn1, period, 0));
    assert_func_calls_over_some_period(tenSecondsInMs, PERIODIC, CALL_EVERY_PERIOD, fn1);
}

void test_MyAlarm_RegisteredActionPeriodAssumedToBeCeilingIfNotMultipleOf100()
{
    int period = 330;   // expect it to be called every 400 ms (ceiling of 330)
    int periodCeiling = 400;
    TEST_ASSERT_EQUAL_INT(ALL_OK, my_alarm_register_function(fn1, period, 0));
    assert_func_calls_over_some_period(tenSecondsInMs, PERIODIC, periodCeiling, fn1);
}

void test_MyAlarm_FirstFunctionCallDelayedCorrectlyBasedOnArgument_DelayMultiple100()
{
    int delay = 100;
    int period = 200;
    TEST_ASSERT_EQUAL_INT(ALL_OK, my_alarm_register_function(fn1, period, delay));
    assert_func_calls_over_some_period(delay, NOT_PERIODIC, delay, NO_FUNCTION);
    assert_func_calls_over_some_period(tenSecondsInMs, PERIODIC, period, fn1); // starts after
}

void test_MyAlarm_WhenDelayIsLessThanPingTimeDelayIsIgnored()
{
    int delay = 90;
    int period = 200;
    TEST_ASSERT_EQUAL_INT(ALL_OK, my_alarm_register_function(fn1, period, delay));
    assert_func_calls_over_some_period(tenSecondsInMs, PERIODIC, period, fn1);
}

void test_MyAlarm_WhenDelayIsNotMultipleOf100ItsRoundedUp()
{
    int delay = 460;
    int delayRounded = 500;
    int period = 200;
    TEST_ASSERT_EQUAL_INT(ALL_OK, my_alarm_register_function(fn1, period, delay));
    assert_func_calls_over_some_period(delayRounded, NOT_PERIODIC, delayRounded, NO_FUNCTION);
    assert_func_calls_over_some_period(tenSecondsInMs, PERIODIC, period, fn1);
}

void test_BadParametersAddNothingAndReturnError_Function0()
{
    TEST_ASSERT_EQUAL_INT(BAD_PARAM, my_alarm_register_function(0, 0, 0));
    assert_func_calls_over_some_period(tenSecondsInMs, PERIODIC, 0, NO_FUNCTION);
}

void test_BadParametersAddNothingAndReturnError_NegativePeriod()
{
    TEST_ASSERT_EQUAL_INT(BAD_PARAM, my_alarm_register_function(fn1, -1, 0));  // bad period
    assert_func_calls_over_some_period(tenSecondsInMs, PERIODIC, 0, NO_FUNCTION);
}

void test_BadParametersAddNothingAndReturnError_NegativeDelay()  // bad delay
{
    TEST_ASSERT_EQUAL_INT(BAD_PARAM, my_alarm_register_function(fn1, 300, -1));
    assert_func_calls_over_some_period(tenSecondsInMs, PERIODIC, 0, NO_FUNCTION);
}

void test_MultipleActionsCanBeAdded()
{
    TEST_ASSERT_EQUAL_INT(ALL_OK, my_alarm_register_function(fn1, 300, 0));
    TEST_ASSERT_EQUAL_INT(ALL_OK, my_alarm_register_function(fn2, 500, 0));

    assert_func_calls_over_some_period(300, NOT_PERIODIC, 300, fn1); // 300
    assert_func_calls_over_some_period(200, NOT_PERIODIC, 200, fn2); // 500
    assert_func_calls_over_some_period(100, NOT_PERIODIC, 100, fn1); // 600
    assert_func_calls_over_some_period(300, NOT_PERIODIC, 300, fn1); // 900
    assert_func_calls_over_some_period(100, NOT_PERIODIC, 100, fn2); // 10000
}

void test_ActionDeleteDoesNothingIfFunctionNotFound()
{
    TEST_ASSERT_EQUAL_INT(ALL_OK, my_alarm_register_function(fn1, 300, 0));
    TEST_ASSERT_EQUAL_INT(ALL_OK, my_alarm_register_function(fn2, 500, 0));

    // function doesn't match
    TEST_ASSERT_EQUAL_INT(ALARM_NOT_FOUND, my_alarm_remove_function(fn1, 500, 0));

    // check that it's deleted
    assert_func_calls_over_some_period(300, NOT_PERIODIC, 300, fn1); // 300
    assert_func_calls_over_some_period(200, NOT_PERIODIC, 200, fn2); // 500
    assert_func_calls_over_some_period(100, NOT_PERIODIC, 100, fn1); // 600
    assert_func_calls_over_some_period(300, NOT_PERIODIC, 300, fn1); // 900
    assert_func_calls_over_some_period(100, NOT_PERIODIC, 100, fn2); // 10000
}

void test_ActionDeleteDoesNothingIfPeriodDoesntMatch()
{
    TEST_ASSERT_EQUAL_INT(ALL_OK, my_alarm_register_function(fn1, 300, 0));
    TEST_ASSERT_EQUAL_INT(ALL_OK, my_alarm_register_function(fn2, 500, 0));

    // period doesn't match
    TEST_ASSERT_EQUAL_INT(ALARM_NOT_FOUND, my_alarm_remove_function(fn2, 200, 0));

    // check that it's deleted
    assert_func_calls_over_some_period(300, NOT_PERIODIC, 300, fn1); // 300
    assert_func_calls_over_some_period(200, NOT_PERIODIC, 200, fn2); // 500
    assert_func_calls_over_some_period(100, NOT_PERIODIC, 100, fn1); // 600
    assert_func_calls_over_some_period(300, NOT_PERIODIC, 300, fn1); // 900
    assert_func_calls_over_some_period(100, NOT_PERIODIC, 100, fn2); // 10000
}

void test_ActionDeleteDoesNothingIfFirstDelayDoesntMatch()
{
    TEST_ASSERT_EQUAL_INT(ALL_OK, my_alarm_register_function(fn1, 300, 0));
    TEST_ASSERT_EQUAL_INT(ALL_OK, my_alarm_register_function(fn2, 500, 0));

    // first call time doesn't match
    TEST_ASSERT_EQUAL_INT(ALARM_NOT_FOUND, my_alarm_remove_function(fn2, 500, 10));

    // check that it's deleted
    assert_func_calls_over_some_period(300, NOT_PERIODIC, 300, fn1); // 300
    assert_func_calls_over_some_period(200, NOT_PERIODIC, 200, fn2); // 500
    assert_func_calls_over_some_period(100, NOT_PERIODIC, 100, fn1); // 600
    assert_func_calls_over_some_period(300, NOT_PERIODIC, 300, fn1); // 900
    assert_func_calls_over_some_period(100, NOT_PERIODIC, 100, fn2); // 10000
}

void test_ActionDeletesIfAll3ParamsMatch()
{
    TEST_ASSERT_EQUAL_INT(ALL_OK, my_alarm_register_function(fn1, 300, 0));
    TEST_ASSERT_EQUAL_INT(ALL_OK, my_alarm_register_function(fn2, 500, 0));

    // first call time doesn't match
    TEST_ASSERT_EQUAL_INT(ALL_OK, my_alarm_remove_function(fn2, 500, 0));

    // check that it's deleted
    assert_func_calls_over_some_period(300, NOT_PERIODIC, 300, fn1); // 300
    assert_func_calls_over_some_period(200, NOT_PERIODIC, 200, NO_FUNCTION); // 500
    assert_func_calls_over_some_period(100, NOT_PERIODIC, 100, fn1); // 600
    assert_func_calls_over_some_period(300, NOT_PERIODIC, 300, fn1); // 900
    assert_func_calls_over_some_period(100, NOT_PERIODIC, 100, NO_FUNCTION); // 10000
}

void test_AddMoreActionsThanMaxDoesNothing()
{
    int i = 0;
    for (i = 0; i < 10; i++)
    {
        TEST_ASSERT_EQUAL_INT(ALL_OK, my_alarm_register_function(fn1, i * alarmPeriod, 0));
    }
    TEST_ASSERT_EQUAL_INT(ACTION_LIMIT_REACHED, my_alarm_register_function(fn1, i * alarmPeriod, 0));
    TEST_ASSERT_EQUAL_INT(ALARM_NOT_FOUND, my_alarm_remove_function(fn1, i * alarmPeriod, 0));
}

void test_RemovedAlarmReservesSpaceForMore()
{
    int i = 0;
    for (i = 0; i < 9; i++)
    {
        TEST_ASSERT_EQUAL_INT(ALL_OK, my_alarm_register_function(fn1, i * alarmPeriod, 0));
    }
    TEST_ASSERT_EQUAL_INT(ALL_OK, my_alarm_register_function(fn1, i * alarmPeriod, 0));
    TEST_ASSERT_EQUAL_INT(ALL_OK, my_alarm_remove_function(fn1, i * alarmPeriod, 0));
    TEST_ASSERT_EQUAL_INT(ALL_OK, my_alarm_register_function(fn1, i * alarmPeriod, 0));
}

void test_WhenAlarmIsRemovedNewOnesTakeItsPlace()
{
    TEST_ASSERT_EQUAL_INT(ALL_OK, my_alarm_register_function(fn1,   100, 0));
    TEST_ASSERT_EQUAL_INT(ALL_OK, my_alarm_register_function(fn1,   200, 0));
    TEST_ASSERT_EQUAL_INT(ALL_OK, my_alarm_register_function(fn1,   300, 0));
    TEST_ASSERT_EQUAL_INT(ALL_OK, my_alarm_register_function(fn1,   400, 0));
    TEST_ASSERT_EQUAL_INT(ALL_OK, my_alarm_register_function(fn1,   500, 0));
    TEST_ASSERT_EQUAL_INT(ALL_OK, my_alarm_register_function(fn1,   600, 0));
    TEST_ASSERT_EQUAL_INT(ALL_OK, my_alarm_register_function(fn1,   700, 0));
    TEST_ASSERT_EQUAL_INT(ALL_OK, my_alarm_register_function(fn1,   800, 0));
    TEST_ASSERT_EQUAL_INT(ALL_OK, my_alarm_register_function(fn1,   900, 0));
    TEST_ASSERT_EQUAL_INT(ALL_OK, my_alarm_register_function(fn1,   1000, 0));

    // remove 600
    TEST_ASSERT_EQUAL_INT(ALL_OK, my_alarm_remove_function(fn1,   600, 0));
    TEST_ASSERT_EQUAL_INT(ALARM_NOT_FOUND, my_alarm_remove_function(fn1,   600, 0));

    // add new one in its place
    TEST_ASSERT_EQUAL_INT(ALL_OK, my_alarm_register_function(fn1, 777, 0));

    // remove the rest, showing that they exist!
    TEST_ASSERT_EQUAL_INT(ALL_OK, my_alarm_remove_function(fn1,   100, 0));
    TEST_ASSERT_EQUAL_INT(ALL_OK, my_alarm_remove_function(fn1,   200, 0));
    TEST_ASSERT_EQUAL_INT(ALL_OK, my_alarm_remove_function(fn1,   300, 0));
    TEST_ASSERT_EQUAL_INT(ALL_OK, my_alarm_remove_function(fn1,   400, 0));
    TEST_ASSERT_EQUAL_INT(ALL_OK, my_alarm_remove_function(fn1,   500, 0));
    TEST_ASSERT_EQUAL_INT(ALL_OK, my_alarm_remove_function(fn1,   777, 0)); // the replacement
    TEST_ASSERT_EQUAL_INT(ALL_OK, my_alarm_remove_function(fn1,   700, 0));
    TEST_ASSERT_EQUAL_INT(ALL_OK, my_alarm_remove_function(fn1,   800, 0));
    TEST_ASSERT_EQUAL_INT(ALL_OK, my_alarm_remove_function(fn1,   900, 0));
    TEST_ASSERT_EQUAL_INT(ALL_OK, my_alarm_remove_function(fn1,   1000, 0));
}

/**************************** Helper functions *********************/

static void assert_func_calls_over_some_period(int msToPingFor, periodic_e isPeriodic, int expectedCallTime, void(*func)(void))
{
    void (*expectedFunction)(void) = 0;
    for (int i = 100; i <= msToPingFor; i += alarmPeriod)
    {
        my_alarm_ping();
        if (isPeriodic == NOT_PERIODIC)
        {
            expectedFunction = (i == expectedCallTime) ? func : 0;
        }
        else
        {
            if (expectedCallTime == 0)
            {
                expectedFunction = func;
            }
            else
            {
                expectedFunction = (i % expectedCallTime == 0) ? func : 0;
            }
        }
        sprintf(testDebugString, "i = %d, call expected at i = %d\n", i, expectedCallTime);
        TEST_ASSERT_EQUAL_PTR_MESSAGE(expectedFunction, spyff_getLastFunctionCalled(), testDebugString);
    }
}

/*
Test-drive an alarm clock service that keeps a list of time callbacks.
In production, the timer interrupt will ping the time service
every 100 milliseconds. When a scheduled action is ready to run,
call the callback function.

[x] No actions registered to begin with
[x] Alarm clock module has a 100 ms callback registered from irq
[x] Alarm clock callback calls the alarm ping function
[maybe] Time increments by 100 ms every ping
[x] registered action is only called when period is right
[x] action may be called periodically; 
[x] first call to action can be delayed
[x] negative time cannot be passed?
[x] actions can be called once (one shot)
[x] new action can be added
[x] multiple actions may be added
[x] action can be deleted
[x] maximum of 10 actions allowed, adding more does nothing
[x] Deleted action reserves space,
[] new actions are added in the place of deleted action
[x] removing an action that doesn't exist shouldn't do anything
[x] actions are matched by their function, modified period and modified first start time (modified meaning rounded up or down)
[] multiple functions can be called at same time
[] one function can be registered to be called at various times

Plan:

2 modules: alarm and interrupt
Interrupt interfaces directly with hardware, but we want to intercept it.
So we need to stub the interrupt module- perhaps a spy so we know how it gets configured by our alarm, and can check
if its called back


Alarm interface:
- create()
- addAlarm(function_ptr, period (0 if one shot), whenToStart (0 if asap))
- removeAlarm(function_ptr, period)

Interrupt interface:
- createIrq(function_ptr, period)
- spy_interrupt_getLastFptr
- spy_interrupt_getLastPeriod
*/

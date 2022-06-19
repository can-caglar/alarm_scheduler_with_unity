#include "unity.h"
#include "MyAlarm.h"
#include "Spy_MyInterrupt.h"
#include "Spy_FakeFunctionsToCall.h"

void setUp(void)
{
    my_alarm_create();
}

void tearDown(void)
{
}

void test_MyAlarm_100ms_ping_registered_at_boot(void)
{
    TEST_ASSERT_EQUAL_INT(100, spymi_getLastPeriod());
    TEST_ASSERT_EQUAL_PTR(my_alarm_ping, spymi_getLastFn());
}

void test_MyAlarm_NoActionRegistered(void)
{
    my_alarm_ping();
    TEST_ASSERT_EQUAL_PTR(0, spyff_getLastFunctionCalled());
}

void test_MyAlarm_RegisteredActionGetsCalled(void)
{
    TEST_ASSERT_EQUAL_PTR(0, spyff_getLastFunctionCalled());
    my_alarm_register_function(fn1);
    my_alarm_ping();
    TEST_ASSERT_EQUAL_PTR(fn1, spyff_getLastFunctionCalled());
}

/*
Test-drive an alarm clock service that keeps a list of time callbacks.
In production, the timer interrupt will ping the time service
every 100 milliseconds. When a scheduled action is ready to run,
call the callback function.

[x] No actions registered to begin with
[x] Alarm clock module has a 100 ms callback registered from irq
[x] Alarm clock callback calls the alarm ping function
[] Time increments by 100 ms every ping
[] registered action is not called if time is not right
[] action may be called periodically; 
[] actions can be called once
[] new action can be added
[] action can be deleted
[] multiple actions may be added
[] maximum of 10 actions allowed, adding more does nothing
[] removing an action that doesn't exist shouldn't do anything

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

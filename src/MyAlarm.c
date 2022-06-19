#include "MyAlarm.h"
#include "MyInterrupt.h"

static void(*registeredFunction)(void);

void my_alarm_create(void)
{
    my_int_create();
    my_int_setup(my_alarm_ping, 100);
}

void my_alarm_ping(void)
{
    if (registeredFunction)
    {
        registeredFunction();
    }
}

void my_alarm_register_function(void(*function)(void))
{
    registeredFunction = function;
}
#include "MyAlarm.h"
#include "MyInterrupt.h"

static int nextPeriodUp(int period);

#define MAX_ALARMS  10
#define NO_FUNCTION 0

typedef struct Alarm_s Alarm_s;
static struct Alarm_s
{
    void(*func)(void);
    int period;
    int nextCall;
    int initialFirstCall;  // used for identifying alarm
} RegAlarms[MAX_ALARMS];

static int periodNow = 0;
static const int periodToIncrement = 100;
static int numAlarms = 0;

static void processCalls(const int* period, Alarm_s* alarmEntry);

void my_alarm_create(void)
{
    my_int_create();
    my_int_setup(my_alarm_ping, 100);
    periodNow = 0;
    numAlarms = 0;
}

void my_alarm_ping(void)
{
    periodNow += periodToIncrement;
    for (int i = 0; i < MAX_ALARMS; i++)
    {
        processCalls(&periodNow, &RegAlarms[i]);
    }
}   

AlarmError_e my_alarm_register_function(void(*function)(void), int period, int firstCall)
{
    AlarmError_e err = ALL_OK;
    if (function == 0 || period < 0 || firstCall < 0)
    {
        err = BAD_PARAM;
    }
    if (err == ALL_OK)
    {
        if (numAlarms >= MAX_ALARMS)
        {
            err = ACTION_LIMIT_REACHED;
        }
    }
    if (err == ALL_OK)
    {
        RegAlarms[numAlarms].func = function;
        RegAlarms[numAlarms].period = nextPeriodUp(period);
        firstCall = nextPeriodUp(firstCall);
        RegAlarms[numAlarms].initialFirstCall = firstCall;
        RegAlarms[numAlarms].nextCall = firstCall + RegAlarms[numAlarms].period;
        numAlarms++;
    }
    return err;
}

AlarmError_e my_alarm_remove_function(void(*function)(void), int period, int firstCall)
{
    AlarmError_e err = ALARM_NOT_FOUND;
    for (int i = 0; i < MAX_ALARMS; i++)
    {
        if (RegAlarms[i].func == function)
        {
            if (RegAlarms[i].period == period)
            {
                if (RegAlarms[i].initialFirstCall == firstCall)
                {
                    // found a match
                    RegAlarms[i].func = NO_FUNCTION;
                    err = ALL_OK;
                    numAlarms--;
                }
            }
        }
    }
    return err;
}

/****************** Private functions ********************/


static int nextPeriodUp(int period)
{
    int ret = period;
    // period is 0 if less than increment
    // period is as it is if multiple of increment
    // period is sent to ceiling if not multiple and larger than increment
    if (period < periodToIncrement)
    {
        ret = 0;
    }
    else
    {
        int remainder;
        if ((remainder = period % periodToIncrement) != 0)
        {
            // ceiling
            ret -= remainder;
            ret += periodToIncrement;
        }
    }
    return ret;
}

static void processCalls(const int* period, Alarm_s* alarmEntry)
{
    if (*period >= alarmEntry->nextCall)
    {
        alarmEntry->nextCall += alarmEntry->period;
        if (alarmEntry->func)
        {
            alarmEntry->func();
        }
    }
}
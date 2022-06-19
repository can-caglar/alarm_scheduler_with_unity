#ifndef MYALARM_H
#define MYALARM_H

typedef enum
{
    ALL_OK,
    BAD_PARAM,
    ACTION_LIMIT_REACHED,
    ALARM_NOT_FOUND,
} AlarmError_e;

void my_alarm_create(void);
void my_alarm_ping(void);
AlarmError_e my_alarm_register_function(void(*function)(void), int period, int firstCall);
AlarmError_e my_alarm_remove_function(void(*function)(void), int period, int firstCall);

#endif // MYALARM_H

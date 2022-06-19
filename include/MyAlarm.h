#ifndef MYALARM_H
#define MYALARM_H

void my_alarm_create(void);
void my_alarm_ping(void);
void my_alarm_register_function(void(*function)(void));

#endif // MYALARM_H

#ifndef MYINTERRUPT_H
#define MYINTERRUPT_H

typedef void(*void_fn)(void);

void my_int_create(void);
void my_int_setup(void_fn function, int period);

#endif // MYINTERRUPT_H

#ifndef FAKE_FUNCTIONS_TO_CALL
#define FAKE_FUNCTIONS_TO_CALL

typedef void (*void_fn) (void);

void ff_create(void);
void fn1(void);
void fn2(void);

void_fn spyff_getLastFunctionCalled(void);

#endif
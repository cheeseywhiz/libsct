#ifndef FIB_H
#define FIB_H

#include <stdint.h>

int fib_init_table(void);

uint64_t fibonacci(uint64_t n);

void fib_free_table(void);

#endif

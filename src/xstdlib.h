#ifndef XSTDLIB_H
#define XSTDLIB_H

#include <stdlib.h>

void* xmalloc(size_t size);
void* xrealloc(void *ptr, size_t size);

#endif

#include <assert.h>
#include "xstdlib.h"
#include "exception.h"

void* xmalloc(size_t size) {
	void *ptr = malloc(size);

	if (!ptr) {
		ERRNO();
	}

	assert(ptr);
	return ptr;
}

void* xrealloc(void *ptr, size_t size) {
	ptr = realloc(ptr, size);

	if (!ptr) {
		ERRNO();
	}

	return ptr;
}

#include <stdio.h>
#include <execinfo.h>
#include "exception.h"

void print_backtrace(void) {
	/* Implementation from https://www.gnu.org/software/libc/manual/html_node/Backtraces.html */
	size_t array_size = 1 << 4;
	void *array[array_size];
	size_t actual_size = backtrace(array, array_size);
	char **strings = backtrace_symbols(array, actual_size);
	STDERR("backtrace: libsct version %s\n", SCT_VERSION);

	/* Some entries on the ends are redundant */
	for (size_t i = actual_size - 3; i >= 1; i--) {
	// for (size_t i = 0; i < actual_size; i++) {
		STDERR("%s\n", strings[i]);
	}

	free(strings);
}

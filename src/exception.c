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

        /* Index 0 is argv[0], so we can skip */
        for (size_t i = actual_size - 1; i >= 1; i--) {
                STDERR("%s\n", strings[i]);
        }

        free(strings);
}

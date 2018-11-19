#ifndef SLICE_H
#define SLICE_H

#include <stdlib.h>

struct slice_bounds {
        ssize_t start;
        ssize_t end;
};

struct slice_bounds get_slice_bounds(ssize_t length, ssize_t start, ssize_t end, ssize_t step);

#endif

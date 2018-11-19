#include <stdlib.h>
#include "slice.h"

struct slice_bounds get_slice_bounds(ssize_t length, ssize_t start, ssize_t end, ssize_t step) {
        if (step > 0) {
                if (start < -length) {
                        start = 0;
                } else if (start < 0) {
                        start += length;
                }

                if (end < -length) {
                        end = 0;
                } else if (end < 0) {
                        end += length;
                } else if (end >= length) {
                        end = length;
                }
        } else {
                if (end < -length) {
                        end = -1;
                } else if (end < 0) {
                        end += length;
                }

                if (start < -length) {
                        if (end == -1) {
                                /* both start and end are out of bounds */
                                start = -1;
                        } else {
                                start = 0;
                        }
                } else if (start < 0) {
                        start += length;
                } else if (start >= length) {
                        start = length - 1;
                }
        }

        return (struct slice_bounds) {start, end};
}

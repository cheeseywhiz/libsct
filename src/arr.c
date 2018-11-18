#include <stdio.h>
#include <stdlib.h>
#include "exception.h"
#include "arr.h"

#define NULL_GUARD() \
        if (!self) { \
                EXCEPTION("user: array is NULL"); \
                goto exit; \
        }

static int arr_grow(struct array *self) {
        ssize_t new_size = self->size + (self->size >> 1);  /* 1.5x growth factor */
        void **new_array = realloc(self->array, new_size * sizeof(void*));

        if (!new_array) {
                ERRNO();
                return 1;
        }

        self->size = new_size;
        self->array = new_array;
        return 0;
}

int arr_init(struct array *self) {
        int exit_code = 1;
        NULL_GUARD();
        self->length = 0;
        self->size = 2;
        self->array = NULL;
        exit_code = arr_grow(self);

exit:
        return exit_code;
}

void arr_free(struct array *self) {
        NULL_GUARD();
        free(self->array);
        self->array = NULL;  /* Prevent double frees */

exit:
        return;
}

void arr_deep_free(struct array *self, sct_free_func free_item) {
        NULL_GUARD();

        for (ssize_t index = 0; index < self->length; index++) {
                free_item(self->array[index]);
        }

        arr_free(self);

exit:
        return;
}

void arr_free_all(struct array *self) {
        arr_deep_free(self, free);
}

int arr_append(struct array *self, void *ptr) {
        int exit_code = 1;
        NULL_GUARD();

        if ((self->length == self->size) && arr_grow(self)) {
                goto exit;
        }

        self->array[self->length] = ptr;
        self->length++;
        exit_code = 0;

exit:
        return exit_code;
}

void* arr_get_index(struct array *self, ssize_t index) {
        void *item = NULL;
        NULL_GUARD();

        if (index < 0) {
                index += self->length;
        }

        if (!(0 <= index && index < self->length)) {
                EXCEPTION("user: index out of range");
                goto exit;
        }

        item = self->array[index];

exit:
        return item;
}

static int arr_slice_reverse(struct array *self, struct array *slice, ssize_t start, ssize_t end,
                             ssize_t step) {
        /* self && slice && step < 0 */
        int exit_code = 1;

        if (end < -self->length) {
                end = -1;
        } else if (end < 0) {
                end += self->length;
        }

        if (start < -self->length) {
                if (end == -1) {
                        /* both start and end are out of bounds */
                        start = -1;
                } else {
                        start = 0;
                }
        } else if (start < 0) {
                start += self->length;
        } else if (start >= self->length) {
                start = self->length - 1;
        }

        if (arr_init(slice)) {
                goto exit;
        }

        for (ssize_t index = start; index > end; index += step) {
                if (arr_append(slice, self->array[index])) {
                        goto exit;
                }
        }

        exit_code = 0;

exit:
        if (exit_code) {
                arr_free(slice);
        }

        return exit_code;
}

int arr_slice(struct array *self, struct array *slice, ssize_t start, ssize_t end, ssize_t step) {
        int exit_code = 1;
        NULL_GUARD();

        if (!slice) {
                EXCEPTION("user: slice is NULL");
                goto exit;
        }

        if (!step) {
                EXCEPTION("user: cannot have zero step");
                goto exit;
        } else if (step < 0) {
                return arr_slice_reverse(self, slice, start, end, step);
        }

        if (start < -self->length) {
                start = 0;
        } else if (start < 0) {
                start += self->length;
        }

        if (end < -self->length) {
                end = 0;
        } else if (end < 0) {
                end += self->length;
        } else if (end >= self->length) {
                end = self->length;
        }

        if (arr_init(slice)) {
                goto exit;
        }

        for (ssize_t index = start; index < end; index += step) {
                if (arr_append(slice, self->array[index])) {
                        goto exit;
                }
        }

        exit_code = 0;

exit:
        if (exit_code && slice) {
                arr_free(slice);
        }

        return exit_code;
}

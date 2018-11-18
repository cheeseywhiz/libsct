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
        free(self->array);
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

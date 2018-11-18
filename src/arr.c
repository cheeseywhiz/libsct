#include <stdio.h>
#include <stdlib.h>
#include "exception.h"
#include "arr.h"

#define NULL_GUARD() \
        if (!self) { \
                EXCEPTION("user: array is NULL"); \
                return 1; \
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
        NULL_GUARD();
        self->length = 0;
        self->size = 2;
        self->array = NULL;
        return arr_grow(self);
}

void arr_free(struct array *self) {
        free(self->array);
}

int arr_append(struct array *self, void *ptr) {
        NULL_GUARD();

        if ((self->length == self->size) && arr_grow(self)) {
                return 1;
        }

        self->array[self->length] = ptr;
        self->length++;
        return 0;
}

#ifndef ARR_H
#define ARR_H

#include <stddef.h>

struct array {
        void **array;
        ssize_t length;
        ssize_t size;
};

int arr_init(struct array *self);
/* Initialize a new array. Returns zero if successful or nonzero otherwise. self should be passed
   to arr_free later. */

void arr_free(struct array *self);
/* Free the data associated with the array. */

int arr_append(struct array *self, void *ptr);
/* Add the ptr to the end of the array. Returns zero if successful or nonzero otherwise. */

void* arr_get_index(struct array *self, ssize_t index);
/* Return the index-th item in the array. Supports negative indexing. Will print an exception and
   return NULL if the list is empty or the desired index is out of range. */

#endif

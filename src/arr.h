#ifndef ARR_H
#define ARR_H

#include <stddef.h>
#include <stdlib.h>
#include "sct.h"

struct array {
	void **array;
	ssize_t length;
	ssize_t size;
};

int arr_init(struct array *self);
/* Initialize a new array. Returns zero if successful or nonzero otherwise. self should be passed
   to one of the array free functions later. */

void arr_free(struct array *self);
/* Free the data associated with the array. */

void arr_deep_free(struct array *self, sct_free_func free_item);
/* Call the free_item function on each item in the array. */

void arr_free_all(struct array *self);
/* Call free on each item in the array. */

int arr_append(struct array *self, void *ptr);
/* Add the ptr to the end of the array. Returns zero if successful or nonzero otherwise. */

void* arr_get_index(struct array *self, ssize_t index);
/* Return the index-th item in the array. Supports negative indexing. Will print an exception and
   return NULL if the list is empty or the desired index is out of range. */

int arr_set_index(struct array *self, ssize_t index, void *ptr);
/* Set the value of the index-th ptr in the array. Returns zero if successful or nonzero otherwise.
   arr_set_index will not free any existing ptr. The alternative is to get the node from
   arr_get_index, free the existing ptr, then call arr_set_index. */

int arr_slice(struct array *self, struct array *slice, ssize_t start, ssize_t end, ssize_t step);
/* Initialize a new array on slice that is composed of items from the array from start to end with
   step in between. Will print an exception and return NULL if step is 0. slice should be passed to
   arr_free later. Returns zero if successful or nonzero otherwise. */

int arr_equals(struct array *self, struct array *other);
/* Return if all pointers in self are equal to all pointers in other. */

int arr_copy(struct array *self, struct array *copy);
/* Initialize a new array on copy that is a copy of the array. Returns zero if successful or nonzero
   otherwise. copy should be passed to arr_free later. */

ssize_t arr_find(struct array *self, void *item);
/* Return the index of the first occurance of ptr in the array, or -1 if ptr is not in the array. */

void* arr_pop(struct array *self, ssize_t index);
/* Remove the node at index of the array and return the node's ptr. The node will be freed. Will
   print an exception and return NULL if array is empty or if index is out of range. */

#endif

#include <stdio.h>
#include <stdlib.h>
#include "exception.h"
#include "arr.h"
#include "slice.h"

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
	if (!self) {
		return;
	}

	free(self->array);
	self->array = NULL;  /* Prevent double frees */
}

void arr_deep_free(struct array *self, sct_free_func free_item) {
	if (!self) {
		return;
	}

	for (ssize_t index = 0; index < self->length; index++) {
		free_item(self->array[index]);
	}

	arr_free(self);
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

#define CHECK_INDEX() \
	if (index < 0) { \
		index += self->length; \
	} \
	if (!(0 <= index && index < self->length)) { \
		EXCEPTION("user: index out of range"); \
		goto exit; \
	}

void* arr_get_index(struct array *self, ssize_t index) {
	void *ptr = NULL;
	NULL_GUARD();
	CHECK_INDEX();
	ptr = self->array[index];

exit:
	return ptr;
}

int arr_set_index(struct array *self, ssize_t index, void *ptr) {
	int exit_code = 1;
	NULL_GUARD();
	CHECK_INDEX();
	self->array[index] = ptr;
	exit_code = 0;

exit:
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
	}

	struct slice_bounds bounds = get_slice_bounds(self->length, start, end, step);

	if (arr_init(slice)) {
		goto exit;
	}

	for (ssize_t index = bounds.start;
	     (step > 0) ? (index < bounds.end) : (index > bounds.end);
	     index += step) {
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

int arr_equals(struct array *self, struct array *other) {
	int is_equal = 0;
	NULL_GUARD();

	if (!other) {
		EXCEPTION("user: other array is NULL");
		goto exit;
	}

	if (self->length != other->length) {
		goto exit;
	}

	for (ssize_t index = 0; index < self->length; index++) {
		if (self->array[index] != other->array[index]) {
			goto exit;
		}
	}

	is_equal = 1;

exit:
	return is_equal;
}

int arr_copy(struct array *self, struct array *copy) {
	int exit_code = 1;
	NULL_GUARD();
	exit_code = arr_slice(self, copy, 0, self->length, 1);

exit:
	return exit_code;
}

ssize_t arr_find(struct array *self, void *item) {
	ssize_t index = -1;
	NULL_GUARD();

	for (index = 0; index < self->length; index++) {
		if (self->array[index] == item) {
			return index;
		}
	}

	index = -1;

exit:
	return index;
}

void* arr_pop(struct array *self, ssize_t index) {
	void *item = NULL;
	NULL_GUARD();
	CHECK_INDEX();
	item = self->array[index];

	for (index++; 1 <= index && index < self->length; index++) {
		self->array[index - 1] = self->array[index];
	}

	self->length--;

exit:
	return item;
}

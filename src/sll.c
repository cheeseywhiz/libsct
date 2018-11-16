#include <stddef.h>
#include <stdlib.h>
#include "exception.h"
#include "sll.h"

static struct sll_node* sll_new_node(void *ptr) {
        struct sll_node *self = malloc(sizeof(struct sll_node));

        if (!self) {
                ERRNO();
                return NULL;
        }

        self->next = NULL;
        self->ptr = ptr;
        return self;
}

static struct sll_node* sll_last(struct sll_node **self) {
        struct sll_node *last;
        for (last = *self; last && last->next; last = last->next);
        return last;
}

struct sll_node* sll_append(struct sll_node **self, void *ptr) {
        struct sll_node *new_node = sll_new_node(ptr);

        if (!new_node) {
                return NULL;
        }

        if (*self) {
                struct sll_node *last = sll_last(self);
                last->next = new_node;
        } else {
                *self = new_node;
        }

        return new_node;
}

void sll_deep_free(struct sll_node **self, sll_free_func free_ptr) {
        struct sll_node *item;
        struct sll_node *item_alias;

        for (item = *self; item; ) {
                item_alias = item;
                item = item->next;
                free_ptr(item_alias->ptr);
                free(item_alias);
        }
}

void sll_free_all(struct sll_node **self) {
        sll_deep_free(self, free);
}

static void no_op_free(__attribute__((unused)) void *ptr) {};

void sll_shallow_free(struct sll_node **self) {
        sll_deep_free(self, no_op_free);
}

ssize_t sll_length(struct sll_node **self) {
        ssize_t length = 0;

        for (struct sll_node *item = *self; item; item = item->next) {
                length++;
        }

        return length;
}

struct sll_node* sll_get_index(struct sll_node **self, ssize_t index) {
        if (!*self) {
                EXCEPTION("list is empty");
                return NULL;
        }

        ssize_t length = sll_length(self);

        if (index < 0) {
                index += length;
        }

        if (!(0 <= index && index < length)) {
                EXCEPTION("index out of range");
                return NULL;
        }

        struct sll_node *item = *self;

        for (ssize_t i = 0; i < index; i++) {
                if (!item) {
                        EXCEPTION("item %ld is unexpectedly NULL", i);
                        return NULL;
                }

                item = item->next;
        }

        return item;
}

static void** sll_to_array(struct sll_node **self) {
        if (!*self) {
                return NULL;
        }

        ssize_t length = sll_length(self);
        void **array = malloc(length * sizeof(void*));

        if (!array) {
                ERRNO();
                return NULL;
        }

        struct sll_node *item = *self;
        ssize_t index = 0;

        for (; item; ) {
                array[index] = item->ptr;
                item = item->next;
                index++;
        }

        return array;
}

static struct sll_node* sll_slice_reverse(struct sll_node **self, ssize_t start, ssize_t end,
                                          ssize_t step) {
        /* *self != NULL && step < 0 */
        ssize_t length = sll_length(self);

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

        void **array = sll_to_array(self);

        if (!array) {
                return NULL;
        }

        struct sll_node *slice = NULL;

        for (ssize_t index = start; index > end; index += step) {
                if (!sll_append(&slice, array[index])) {
                        sll_shallow_free(&slice);
                        free(array);
                        return NULL;
                }
        }

        free(array);
        return slice;
}

struct sll_node* sll_slice(struct sll_node **self, ssize_t start, ssize_t end, ssize_t step) {
        if (!*self) {
                return NULL;
        } else if (!step) {
                EXCEPTION("cannot have zero step");
                return NULL;
        } else if (step < 0) {
                return sll_slice_reverse(self, start, end, step);
        }

        ssize_t length = sll_length(self);

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

        void **array = sll_to_array(self);

        if (!array) {
                return NULL;
        }

        struct sll_node *slice = NULL;

        for (ssize_t index = start; index < end; index += step) {
                if (!sll_append(&slice, array[index])) {
                        sll_shallow_free(&slice);
                        free(array);
                        return NULL;
                }
        }

        free(array);
        return slice;
}

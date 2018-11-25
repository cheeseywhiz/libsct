#include <stddef.h>
#include <stdlib.h>
#include "exception.h"
#include "sll.h"
#include "slice.h"

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

void sll_deep_free(struct sll_node **self, sct_free_func free_ptr) {
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

#define CHECK_INDEX() \
        ssize_t length = sll_length(self); \
        if (index < 0) { \
                index += length; \
        } \
        if (!(0 <= index && index < length)) { \
                EXCEPTION("user: index out of range"); \
                return NULL; \
        }

struct sll_node* sll_get_index(struct sll_node **self, ssize_t index) {
        if (!*self) {
                EXCEPTION("user: list is empty");
                return NULL;
        }

        CHECK_INDEX();
        struct sll_node *item = *self;

        for (ssize_t i = 0; i < index; i++) {
                if (!item) {
                        EXCEPTION("internal: item %ld is unexpectedly NULL", i);
                        return NULL;
                }

                item = item->next;
        }

        return item;
}

int sll_set_index(struct sll_node **self, ssize_t index, void *ptr) {
        struct sll_node *item = sll_get_index(self, index);

        if (!item) {
                return 1;
        }

        item->ptr = ptr;
        return 0;
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

struct sll_node* sll_slice(struct sll_node **self, ssize_t start, ssize_t end, ssize_t step) {
        if (!*self) {
                return NULL;
        } else if (!step) {
                EXCEPTION("user: cannot have zero step");
                return NULL;
        }

        struct slice_bounds bounds = get_slice_bounds(sll_length(self), start, end, step);
        void **array = sll_to_array(self);

        if (!array) {
                return NULL;
        }

        struct sll_node *slice = NULL;

        for (ssize_t index = bounds.start;
             (step > 0) ? (index < bounds.end) : (index > bounds.end);
             index += step) {
                if (!sll_append(&slice, array[index])) {
                        sll_shallow_free(&slice);
                        free(array);
                        return NULL;
                }
        }

        free(array);
        return slice;
}

#define XOR(p, q) (((p) || (q)) && !((p) && (q)))

int sll_equals(struct sll_node **self, struct sll_node **other) {
        if (XOR(*self, *other)) {
                /* One or the other is NULL */
                return 0;
        } else if (!(*self && *other)) {
                /* Both NULL */
                return 1;
        }  /* else: neither NULL */

        struct sll_node *self_item = *self;
        struct sll_node *other_item = *other;

        for(; self_item && other_item; ) {
                if (self_item->ptr != other_item->ptr) {
                        return 0;
                }

                self_item = self_item->next;
                other_item = other_item->next;
        }

        /* Even if the first few elements are equal, test if they both end at the same place. */
        return !self_item && !other_item;
}

struct sll_node* sll_copy(struct sll_node **self) {
        return sll_slice(self, 0, sll_length(self), 1);
}

ssize_t sll_find(struct sll_node **self, void *ptr) {
        struct sll_node *item = *self;
        ssize_t index = 0;

        for (; item; ) {
                if (item->ptr == ptr) {
                        return index;
                }

                item = item->next;
                index++;
        }

        return -1;
}

void* sll_pop(struct sll_node **self, ssize_t index) {
        if (!*self) {
                EXCEPTION("user: can't pop on empty list");
                return NULL;
        }

        CHECK_INDEX();
        struct sll_node *item = NULL;

        if (!index) {
                item = *self;
                *self = item->next;
        } else {
                struct sll_node *before = sll_get_index(self, index - 1);

                if (!before) {
                        EXCEPTION("internal: before node is NULL");
                        return NULL;
                }

                item = before->next;

                if (!item) {
                        EXCEPTION("internal: node after before is NULL");
                        return NULL;
                }

                before->next = item->next;
        }

        void *ptr;

        if (item) {
                ptr = item->ptr;
                free(item);
        } else {
                ptr = NULL;
        }

        return ptr;
}

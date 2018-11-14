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

static void no_op_free(__attribute__((unused)) void *ptr) {};

void sll_shallow_free(struct sll_node **self) {
        sll_deep_free(self, no_op_free);
}

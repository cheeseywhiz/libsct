#ifndef SLL_H
#define SLL_H

#include <stddef.h>

struct sll_node {
        struct sll_node *next;
        void *ptr;
};

struct sll_node* sll_append(struct sll_node **self, void *ptr);
/* Append a new node that holds the ptr on to the end of the list at *self. If
   *self is NULL, then *self is set to the new node, thereby creating a new
   list.
   Returns the new node. */

typedef void (*sll_free_func)(void *ptr);
/* Example: free */

void sll_deep_free(struct sll_node **self, sll_free_func free_ptr);
/* Call the free_ptr function on every member of the list and free each
   node. */

void sll_shallow_free(struct sll_node **self);
/* Free all of the node structures in the list. */

size_t sll_length(struct sll_node **self);

#endif

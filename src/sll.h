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

void sll_free_all(struct sll_node **self);
/* Call free on every ptr and node in the list. */

void sll_shallow_free(struct sll_node **self);
/* Free all of the node structures in the list. */

ssize_t sll_length(struct sll_node **self);
/* Counts the number of nodes that are linked together.
   Returns the length of the list. */

struct sll_node* sll_get_index(struct sll_node **self, ssize_t index);
/* Return the index-th node in the list. Supports negative indexing. */

struct sll_node* sll_slice(struct sll_node **self, ssize_t start, ssize_t end, ssize_t step);
/* Return pointer to the first node in a new list constructed from indexes from start to end with
   step inbetween. */

int sll_equals(struct sll_node **self, struct sll_node **other);
/* Return if all pointers in self are equal to all pointers in other. */

struct sll_node* sll_copy(struct sll_node **self);
/* Return a new list with all pointers copied over from the list. */

#endif

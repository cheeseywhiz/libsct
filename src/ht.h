#ifndef HT_H
#define HT_H

#include "sct.h"
#include "arr.h"

typedef sct_hash_int (*ht_hash_key)(void *key);
typedef int (*ht_key_equals)(void *key, void *other);

struct ht_key_type {
        ht_hash_key hash_key;
        ht_key_equals key_equals;
};

extern struct ht_key_type ht_string_type;
extern struct ht_key_type ht_int_type;
extern struct ht_key_type ht_uint64_type;

struct ht_entry {
        sct_hash_int hash;
        void *key;
        void *value;
};

struct ht_hash_table {
        struct ht_key_type key_type;
        ssize_t size;
        struct array entries;
        struct array buckets;
        struct array garbage;
};

int ht_init(struct ht_hash_table *self, struct ht_key_type key_type);
/* Initialize a new hash table. Returns zero if successful or nonzero otherwise, in which the hash
   table should be abandoned. self->garbage holds all entries created by the hash table so that its
   keys and values may be freed. self should be passed to ht_free later. */

void ht_free(struct ht_hash_table *self);
/* Free the data associated with the hash table. */

void ht_generic_free(struct ht_hash_table *self, sct_free_func free_key, sct_free_func free_value);
/* Call free_key on on all keys and free_value on all values in the table's garbage, then free the
   data associated with the table. */

void ht_free_all(struct ht_hash_table *self);
/* Call ht_generic_free with free for both free_key and free_value. */

int ht_entries(struct ht_hash_table *self, struct array *entries);
/* Initialize an array of all entries in the table. Retains entry order. Returns zero if successful
   or nonzero otherwise. entries should be passed to arr_free later. */

ssize_t ht_length(struct ht_hash_table *self);
/* Return the length of the key. Returns negative if not successful. */

int ht_set_item(struct ht_hash_table *self, void *key, void *value);
/* Set key in the hash table to value. Returns zero if successful or nonzero otherwise, in which the
   hash table should be abandoned. */

int ht_delete_item(struct ht_hash_table *self, void *key);
/* Delete the key from the table. Returns zero if successful or nonzero otherwise, in which the hash
   table should be abandoned. */

void* ht_get(struct ht_hash_table *self, void *key, void *default_);
/* Return the value of key if key is in the hash table, NULL if an error occurs, or default_
   otherwise. */

void* ht_get_item(struct ht_hash_table *self, void *key);
/* Return the value of the key in the hash table. Will print an exception and return NULL if key is
   not in the table. */

void* ht_pop(struct ht_hash_table *self, void *key, void *default_);
/* Return key and remove key from the hash table if key is in the hash table, or return default_. */

struct ht_entry* ht_popitem(struct ht_hash_table *self);
/* Pop the last item in the hash table and return its ht_entry structure, or NULL if the table is
   empty. */

int ht_contains(struct ht_hash_table *self, void *key);
/* Return if key is in the hash table. */

int ht_clear(struct ht_hash_table *self);
/* Clear all entries in the hash table. Returns zero if successful or nonzero otherwise. */

#endif

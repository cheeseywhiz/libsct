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
   table should be abandoned. */

void ht_free(struct ht_hash_table *self);
/* Free the data associated with the hash table. */

int ht_entries(struct ht_hash_table *self, struct array *entries);
/* Initialize an array of all entries in the table. Retains entry order. Returns zero if successful
   or nonzero otherwise. */

ssize_t ht_length(struct ht_hash_table *self);
/* Return the length of the key. Returns negative if not successful. */

int ht_set_item(struct ht_hash_table *self, void *key, void *value);
/* Set key in the hash table to value. Returns zero if successful or nonzero otherwise, in which the
   hash table should be abandoned. */

int ht_delete_item(struct ht_hash_table *self, void *key);
/* Delete the key from the table. Returns zero if successful or nonzero otherwise, in which the hash
   table should be abandoned. */

void* ht_get_item(struct ht_hash_table *self, void *key);
/* Return the value of the key in the hash table. */

#endif

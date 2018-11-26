#include <string.h>
#include <stdint.h>
#include "exception.h"
#include "hash.h"
#include "arr.h"
#include "ht.h"
#include "sll.h"

#define EXC_KEY_ERROR() EXCEPTION("user: hash table is NULL")

static int init_buckets(struct ht_hash_table *self);

sct_hash_int ht_string_hash(char *key) {
        return fnv_hash((unsigned char*) key, strlen(key));
}

int ht_string_equals(char *key, char *other) {
        return !strcmp(key, other);
}

struct ht_key_type ht_string_type = {
        .hash_key = (ht_hash_key) ht_string_hash,
        .key_equals = (ht_key_equals) ht_string_equals,
};

sct_hash_int ht_int_hash(int *key) {
        return fnv_hash((unsigned char*) key, sizeof(int));
}

int ht_int_equals(int *key, int *other) {
        return *key == *other;
}

struct ht_key_type ht_int_type = {
        .hash_key = (ht_hash_key) ht_int_hash,
        .key_equals = (ht_key_equals) ht_int_equals,
};

sct_hash_int ht_uint64_hash(uint64_t *key) {
        return fnv_hash((unsigned char*) key, sizeof(key));
}

int ht_uint64_equals(uint64_t *key, uint64_t *other) {
        return *key == *other;
}

struct ht_key_type ht_uint64_type = {
        .hash_key = (ht_hash_key) ht_uint64_hash,
        .key_equals = (ht_key_equals) ht_uint64_equals,
};

#define NULL_GUARD() \
        if (!self) { \
                EXC_KEY_ERROR(); \
                goto exit; \
        }

int ht_init(struct ht_hash_table *self, struct ht_key_type key_type) {
        int exit_code = 1;
        NULL_GUARD();
        self->key_type = key_type;
        self->size = 2;

        if (arr_init(&self->entries)) {
                goto exit;
        }

        if (arr_init(&self->garbage)) {
                goto exit;
        }

        self->buckets = (struct array) {0};
        exit_code = init_buckets(self);

exit:
        return exit_code;
}

static void free_collisions(struct sll_node *collisions) {
        sll_free_all(&collisions);
}

static void free_buckets(struct ht_hash_table *self) {
        arr_deep_free(&self->buckets, (sct_free_func) free_collisions);
}

static void free_entries(struct ht_hash_table *self) {
        arr_free(&self->entries);
}

static int init_buckets(struct ht_hash_table *self) {
        int exit_code = 1;
        free_buckets(self);

        if (arr_init(&self->buckets)) {
                goto exit;
        }

        for (ssize_t i = 0; i < self->size; i++) {
                if (arr_append(&self->buckets, NULL)) {
                        break;
                }
        }

        struct array entries;

        if (ht_entries(self, &entries)) {
                goto exit;
        }

        free_entries(self);
        self->entries = entries;
        ssize_t mask = self->size - 1;

        for (ssize_t entry_i = 0; entry_i < self->entries.length; entry_i++) {
                struct ht_entry *entry = arr_get_index(&self->entries, entry_i);

                if (!entry) {
                        continue;
                }

                ssize_t bucket = entry->hash & mask;
                struct sll_node *collisions = arr_get_index(&self->buckets, bucket);
                ssize_t *entry_i_ptr = malloc(sizeof(ssize_t));

                if (!entry_i_ptr) {
                        goto exit;
                }

                *entry_i_ptr = entry_i;

                if (!sll_append(&collisions, entry_i_ptr)) {
                        goto exit;
                }

                if (arr_set_index(&self->buckets, bucket, collisions)) {
                        goto exit;
                }
        }

        exit_code = 0;

exit:
        return exit_code;
}

void ht_free(struct ht_hash_table *self) {
        if (!self) {
                return;
        }

        free_buckets(self);
        free_entries(self);
        arr_free_all(&self->garbage);
}

int ht_entries(struct ht_hash_table *self, struct array *entries) {
        int exit_code = 1;
        NULL_GUARD();

        if (arr_init(entries)) {
                goto exit;
        }

        for (ssize_t entry_i = 0; entry_i < self->entries.length; entry_i++) {
                struct ht_entry *entry = arr_get_index(&self->entries, entry_i);

                if (!entry) {
                        continue;
                }

                if (arr_append(entries, entry)) {
                        goto exit;
                }
        }

        exit_code = 0;

exit:
        if (exit_code) {
                arr_free(entries);
        }

        return exit_code;
}

ssize_t ht_length(struct ht_hash_table *self) {
        ssize_t length = -1;
        NULL_GUARD();
        length = 0;

        for (ssize_t entry_i = 0; entry_i < self->entries.length; entry_i++) {
                struct ht_entry *entry = arr_get_index(&self->entries, entry_i);

                if (!entry) {
                        continue;
                }

                length++;
        }

exit:
        return length;
}

static int do_resize(struct ht_hash_table *self, int grow) {
        if (grow) {
                self->size <<= 1;
        } else {
                self->size >>= 1;
        }

        return init_buckets(self);
}

static int exceeds_load_factor_limit(struct ht_hash_table *self, int grow) {
        if (!grow) {
                if (self->size == 2) {
                        return 0;
                }

                ssize_t future_size = self->size >> 1;
                return self->entries.length * 3 < future_size * 2;
        }

        return self->entries.length * 3 > self->size * 2;
}

static int resize(struct ht_hash_table *self, int grow) {
        int exit_code = 0;

        if (exceeds_load_factor_limit(self, grow)) {
                exit_code = do_resize(self, grow);
        }

        return exit_code;
}

static ssize_t* resolve_collisions(struct ht_hash_table *self, void *key, sct_hash_int hash,
                                  struct sll_node *collisions, int strict) {
        for (struct sll_node *item = collisions; item; item = item->next) {
                ssize_t *entry_i_ptr = item->ptr;
                struct ht_entry *entry = arr_get_index(&self->entries, *entry_i_ptr);

                if (!entry) {
                        continue;
                }

                if (key == entry->key \
                                || hash == entry->hash \
                                || self->key_type.key_equals(key, entry->key)) {
                        return entry_i_ptr;
                }
        }

        if (strict) {
                EXCEPTION("user: key error %p", key);
        }

        return NULL;
}

#define SET_OUT_PTR(name) \
        if (name##_out) { \
                *name##_out = name; \
        }

static void method_init(struct ht_hash_table *self, void *key, sct_hash_int *hash_out,
                        ssize_t *bucket_out, struct sll_node **collisions_out,
                        ssize_t **entry_i_ptr_out, ssize_t *entry_i_out, int strict) {
        sct_hash_int hash = self->key_type.hash_key(key);
        ssize_t mask = self->size - 1;
        ssize_t bucket = hash & mask;
        struct sll_node *collisions = arr_get_index(&self->buckets, bucket);
        ssize_t *entry_i_ptr = resolve_collisions(self, key, hash, collisions, strict);
        ssize_t entry_i = entry_i_ptr ? *entry_i_ptr : -1;
        SET_OUT_PTR(hash);
        SET_OUT_PTR(bucket);
        SET_OUT_PTR(collisions);
        SET_OUT_PTR(entry_i_ptr);
        SET_OUT_PTR(entry_i);
}

int ht_set_item(struct ht_hash_table *self, void *key, void *value) {
        int exit_code = 1;
        NULL_GUARD();
        sct_hash_int hash;
        ssize_t bucket;
        struct sll_node *collisions;
        ssize_t *entry_i_ptr;
        ssize_t entry_i;
        method_init(self, key, &hash, &bucket, &collisions, &entry_i_ptr, &entry_i, 0);
        struct ht_entry *new_entry = malloc(sizeof(struct ht_entry));

        if (!new_entry) {
                goto exit;
        }

        new_entry->hash = hash;
        new_entry->key = key;
        new_entry->value = value;

        if (arr_append(&self->garbage, new_entry)) {
                goto exit;
        }

        if (entry_i >= 0) {
                if (arr_set_index(&self->entries, entry_i, new_entry)) {
                        goto exit;
                }

                exit_code = 0;
                goto exit;
        }

        entry_i_ptr = malloc(sizeof(ssize_t));

        if (!entry_i_ptr) {
                goto exit;
        }

        entry_i = self->entries.length;
        *entry_i_ptr = entry_i;

        if (arr_append(&self->entries, new_entry) \
                        || !sll_append(&collisions, entry_i_ptr) \
                        || arr_set_index(&self->buckets, bucket, collisions)) {
                free(entry_i_ptr);
                goto exit;
        }

        exit_code = resize(self, 1);

exit:
        return exit_code;
}

int ht_delete_item(struct ht_hash_table *self, void *key) {
        int exit_code = 1;
        NULL_GUARD();
        ssize_t bucket;
        struct sll_node *collisions;
        ssize_t *entry_i_ptr;
        ssize_t entry_i;
        method_init(self, key, NULL, &bucket, &collisions, &entry_i_ptr, &entry_i, 1);

        if (entry_i < 0) {
                goto exit;
        }

        if (arr_set_index(&self->entries, entry_i, NULL)) {
                goto exit;
        }

        ssize_t collision_i = sll_find(&collisions, entry_i_ptr);

        if (collision_i < 0) {
                goto exit;
        }

        free(sll_pop(&collisions, collision_i));

        if (arr_set_index(&self->buckets, bucket, collisions)) {
                goto exit;
        }

        exit_code = resize(self, 0);

exit:
        return exit_code;
}

static void* get(struct ht_hash_table *self, void *key, void *default_, int strict) {
        void *value = default_;
        NULL_GUARD();
        ssize_t entry_i;
        method_init(self, key, NULL, NULL, NULL, NULL, &entry_i, strict);

        if (entry_i < 0) {
                goto exit;
        }

        struct ht_entry *entry = arr_get_index(&self->entries, entry_i);

        if (!entry) {
                value = NULL;
                goto exit;
        }

        value = entry->value;

exit:
        return value;
}

void* ht_get(struct ht_hash_table *self, void *key, void *default_) {
        return get(self, key, default_, 0);
}

void* ht_get_item(struct ht_hash_table *self, void *key) {
        return get(self, key, NULL, 1);
}

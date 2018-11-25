#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include "sct.h"
#include "hash.h"

sct_hash_int fnv_hash(unsigned char *data, size_t length) {
        /* https://tools.ietf.org/html/draft-eastlake-fnv-15 */
        sct_hash_int hash = 0xcbf29ce484222325;
        sct_hash_int prime = 0x100000001b3;

        for (size_t i = 0; i < length; i++) {
                unsigned char byte = data[i];
                hash ^= byte;
                hash *= prime;
        }

        return hash;
}

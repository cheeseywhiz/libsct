#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include "hash.h"

uint64_t fnv_hash(unsigned char *data, size_t length) {
        /* https://tools.ietf.org/html/draft-eastlake-fnv-15 */
        uint64_t hash = 0xcbf29ce484222325;
        uint64_t prime = 0x100000001b3;

        for (size_t i = 0; i < length; i++) {
                unsigned char byte = data[i];
                hash ^= byte;
                hash *= prime;
        }

        return hash;
}

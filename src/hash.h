#ifndef HASH_H
#define HASH_H

#include <stddef.h>
#include <stdint.h>

uint64_t fnv_hash(unsigned char *data, size_t length);
/* Return the Fowler–Noll–Vo hash of the data. */

#endif

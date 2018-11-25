#ifndef SCT_H
#define SCT_H

#include <stdint.h>

typedef void (*sct_free_func)(void *ptr);
/* Example: free */

typedef uint64_t sct_hash_int;

#endif

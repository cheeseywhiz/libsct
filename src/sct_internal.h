#ifndef SCT_INTERNAL_H
#define SCT_INTERNAL_H

#include <stdio.h>

#define STDERR(args...) fprintf(stderr, args)
#define ARRAY_LENGTH(arr) (sizeof(arr) / sizeof(arr[0]))

#endif

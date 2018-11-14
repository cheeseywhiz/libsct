#ifndef EXCEPTION_H
#define EXCEPTION_H

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void print_backtrace(void);

#define STDERR(args...) fprintf(stderr, args)
#define FILE_LINE() STDERR("%s (%d): ", __FILE__, __LINE__)

#ifdef _SCT_SUPPRESS_EXCEPTIONS
#define EXCEPTION(args...)
#else
#define EXCEPTION(args...) \
	print_backtrace(); \
	STDERR("exception: "); \
	FILE_LINE(); \
	STDERR(args); \
	STDERR("\n")

#endif  /* ifdef _SCT_SUPRESS_EXCEPTIONS */

#define ERRNO() EXCEPTION("errno: %s", strerror(errno))

#endif  /* ifndef EXCEPTION_H */

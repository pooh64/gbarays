#pragma once

extern "C" {
#include <gba_types.h>
};
#include <config.h>

typedef uint64_t u64;
typedef int64_t s64;

#define ARRAY_SIZE(arr) (sizeof(arr) / sizeof(*(arr)))

void error(char const *msg, const char *file, int line);

#ifndef NDEBUG
#define assert(expr)                                                           \
	do {                                                                   \
		if (!(expr))                                                   \
			error("assert:" #expr, __FILE__, __LINE__);            \
	} while (0)
#else
#define assert(expr)
#endif

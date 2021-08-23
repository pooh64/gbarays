extern "C" {
#include <gba_video.h>
#include <gba_console.h>
#include <gba_interrupt.h>
#include <gba_systemcalls.h>
#include <gba_input.h>
#include <stdio.h>
#include <stdlib.h>
};

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

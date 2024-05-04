#ifndef DEBUG_H
#define DEBUG_H

#include <stdio.h>

#define DEBUG 0

/*
 *
 * Debug print and printf functions that show file name, function name, and line number.
 * I found these online.
 *
*/

#define debug_printf(fmt, ...) \
        do { if (DEBUG) fprintf(stderr, "%s:%d:%s(): " fmt, __FILE__, \
                                __LINE__, __func__, __VA_ARGS__); } while (0)

#define debug_print(fmt) \
        do { if (DEBUG) fprintf(stderr, "%s:%d:%s(): " fmt, __FILE__, \
                                __LINE__, __func__); } while (0)

#endif

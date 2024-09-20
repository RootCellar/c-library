#ifndef DEBUG_H
#define DEBUG_H

#define DEBUG 0
#define DEBUG_STREAM stdout
#define DEBUG_COLOR ANSI_COLOR_GRAY

#include <stdlib.h>
#include <stdio.h>

#include "terminal_color.h"

#define debug_printf(fmt, ...) do { \
  if (DEBUG) fprintf(DEBUG_STREAM, "%s %s:%d:%s(): " fmt "%s\n", \
  DEBUG_COLOR, __FILE__, __LINE__, __func__, __VA_ARGS__, ANSI_COLOR_RESET); \
  } while (0)

#define debug_print(fmt) do { \
  if (DEBUG) fprintf(DEBUG_STREAM, "%s %s:%d:%s(): " fmt "%s\n", \
  DEBUG_COLOR, __FILE__, __LINE__, __func__, ANSI_COLOR_RESET); \
  } while (0)

#define EXIT_FAIL() do { \
  fprintf(stderr, "%s Exiting from %s in %s line %d %s\n", ANSI_COLOR_BRIGHT_RED, __func__, __FILE__, __LINE__, ANSI_COLOR_RESET); \
  exit(EXIT_FAILURE); \
  } while(0)

#endif

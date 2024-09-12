#ifndef DEBUG_H
#define DEBUG_H

#ifdef DEBUG
#include <stdio.h>
#define debug(fmt, ...)                                                 \
  do { if (DEBUG) fprintf(stderr, fmt, __VA_ARGS__); } while (0)
#else
#define debug(fmt, ...)                         \
  do { ; } while(0)
#endif
#endif

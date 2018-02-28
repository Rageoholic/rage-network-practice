#ifndef DEBUG_H
#define DEBUG_H
#include <stdio.h>
#include <assert.h>
/* Functions */
#ifdef NDEBUG
#define ERROR(str)
#else
#define ERROR(str)                                                             \
  do                                                                           \
  {                                                                            \
    fprintf(stderr, "fatal error at %s:%d: %s", __FILE__, __LINE__, str);      \
    exit(-1);                                                                  \
  } while (0);
#endif
#endif

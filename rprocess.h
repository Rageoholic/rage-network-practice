#ifndef RPROCESS_H
#define RPROCESS_H

#include "rdef.h"

typedef enum
{
  SIGNAL_CHILD
} signalType;

typedef int signalFlags;

#define SF_RESTART 0x01

typedef const int pid;

typedef void (*signalHandler)(int);

pid ForkProcess();

int SetSignalHandler(signalType signum, signalHandler Handler, signalFlags saFlags);
#undef _POSIX_C_SOURCE
#endif

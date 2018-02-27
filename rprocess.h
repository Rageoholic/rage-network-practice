#ifndef RPROCESS_H
#define RPROCESS_H

#include "rdef.h"

typedef enum
{
  SIGNAL_CHILD
} SignalType;

typedef int SignalFlags;

#define SF_RESTART 0x01

typedef const int pid;

typedef void (*SignalHandler)(int);

pid ForkProcess();

int SetSignalHandler(SignalType signum, SignalHandler Handler, SignalFlags saFlags);
#endif

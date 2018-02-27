#ifndef RPROCESS_H
#define RPROCESS_H

#include "def.h"

typedef enum
{
  SIGNAL_CHILD
} SignalType;

typedef int SignalFlags;	/* Set of signal flags for
				   program. See defines with
				   SignalFlags */

#define SF_RESTART 0x01		/* SignalFlags: Functions will not
				   fail with EINTR if they would, they
				   will retry */

typedef const int PID;

typedef void (*SignalHandler)(int);

PID ForkProcess();

int SetSignalHandler(SignalType signum, SignalHandler Handler, SignalFlags saFlags);
#endif

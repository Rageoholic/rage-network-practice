#define _XOPEN_SOURCE 1000

#include "process.h"

#include <signal.h>
#include <stddef.h>
#include <unistd.h>

local int SignalTypeToSignal(SignalType sig)
{
  switch(sig)
  {
  case SIGNAL_CHILD:
    return SIGCHLD;
  }
  return -1;
}

local int SignalFlagsToSaFlags(SignalFlags sigType)
{
  int saFlags = 0;
  if(sigType && SF_RESTART)
  {
    saFlags |= SA_RESTART;
  }
  return saFlags;
}

int SetSignalHandler(SignalType signum, SignalHandler handler,
                     SignalFlags saFlags)
{
  struct sigaction sa = {.sa_handler = handler,
			 .sa_flags = SignalFlagsToSaFlags(saFlags)
  };

  return sigaction(SignalTypeToSignal(signum), &sa, NULL);
}

PID ForkProcess()
{
  return fork();
}

#define _XOPEN_SOURCE 1000

#include "rprocess.h"

#include <signal.h>
#include <stddef.h>
#include <unistd.h>

local int SignalTypeToSignal(signalType sig)
{
  switch(sig)
  {
  case SIGNAL_CHILD:
    return SIGCHLD;
  }
  return -1;
}

local int SignalFlagsToSaFlags(signalFlags sigType)
{
  int saFlags = 0;
  if(sigType && SF_RESTART)
  {
    saFlags |= SA_RESTART;
  }
  return saFlags;
}

int SetSignalHandler(signalType signum, signalHandler handler,
                     signalFlags saFlags)
{
  struct sigaction sa = {0};

  sa.sa_handler = handler;
  sa.sa_flags = SignalFlagsToSaFlags(saFlags);

  return sigaction(SignalTypeToSignal(signum), &sa, NULL);
}

pid ForkProcess()
{
  return fork();
}

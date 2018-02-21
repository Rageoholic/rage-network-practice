#define _XOPEN_SOURCE

#include "rprocess.h"

#include <signal.h>
#include <stddef.h>
#include <unistd.h>

int SetSignalHandler(int signum, void (*Handler)(int), int saFlags)
{
  struct sigaction sa = {0};

  sa.sa_handler = Handler;
  sa.sa_flags = saFlags;

  return sigaction(signum, &sa, NULL);
}

pid ForkProcess()
{
  return fork();
}

#ifndef RPROCESS_H
#define RPROCESS_H

typedef const int pid;

pid ForkProcess();

int SetSignalHandler(int signum, void (*Handler)(int), int saFlags);

#endif

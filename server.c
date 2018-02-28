#include "rutils/network.h"
#include "rutils/process.h"
#include "rutils/debug.h"

#include <assert.h>
#include <errno.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>

local void SigchildHandler(int s)
{
  ignore s;
  int saved_errno = errno;
  while (waitpid(-1, NULL, WNOHANG) > 0)
    ;

  errno = saved_errno;
}

int main(int argc, char *argv[])
{
  if (argc != 2)
  {
    fprintf(stderr, "usage: %s hostname\n", argv[0]);
    return 1;
  }

  Socket sockfd = CreateTCPServerSocket(argv[1], 10);
  if(sockfd == -1)
  {
    perror("server: CreateTCPServerSocket");
    return 2;
  }

  if (SetSignalHandler(SIGNAL_CHILD, SigchildHandler, SF_RESTART) == -1)
  {
    perror("SetSignalHandler");
    return 1;
  }

  puts("server listening for connections");

  while (true) {
    SockAddr theirAddr;

    Socket connfd = AcceptConnection(sockfd, &theirAddr);


    if (connfd == -1)
    {
      perror("AcceptConnection");
      ERROR("invalid fd (goddamnit)")
      continue;
    }

    char s[MAX_ADDR_STR_LEN];


    printf("server: connected to %s\n", SockAddrToStr(&theirAddr, s));

    DestroySockAddr(theirAddr);
    PID returnPid = ForkProcess();
    if (returnPid == -1)
    {
      ERROR("fork failed");
    }
    else if(!returnPid)
    {
      DestroySocket(sockfd);	/* Forked process doesn't care about
				   new connections */
      if (TCPSendData(connfd, "Hello, world!", 13) == -1)
      {
        perror("send");
      }
      DestroySocket(connfd);
      exit(0);
    }
    DestroySocket(connfd); 	/* Parent doesn't give two shits about
				   the external connection */
  }
  return 0;
}

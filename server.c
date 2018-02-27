#include "rnetwork.h"
#include "rprocess.h"
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

  AddrInfo *servInfo;

  int rv;
  if ((rv = GetAddrInfo(AI_PASSIVE, SOCK_STREAM, AF_UNSPEC, argv[1], NULL, &servInfo))
      != 0)
  {
    fprintf(stderr, "%s\n", GaiError(rv));
    return 2;
  }

  fd sockfd = BindToAddrInfo(servInfo, 10);
  if(sockfd == -1)
  {
    perror("BindToAddrInfo");
    return 2;
  }
  char s[INET6_ADDRSTRLEN];
  printf("bound to %s\n", GetIpStr(servInfo, s, sizeof(s)));

  if (SetSignalHandler(SIGNAL_CHILD, SigchildHandler, SF_RESTART) == -1)
  {
    perror("SetSignalHandler");
    return 1;
  }

  FreeAddrInfo(servInfo);

  while (true) {
    SockAddrStorage theirAddr = {0};
    socklen_t sinSize = sizeof(theirAddr);

    fd connfd = AcceptConnection(sockfd, (SockAddr *)&theirAddr, sinSize);


    if (connfd == -1)
    {
      perror("AcceptConnection");
      ERROR("invalid fd (goddamnit)")
      continue;
    }

    char s[INET6_ADDRSTRLEN];


    printf("server: connected to %s\n", SockAddrToStr(&theirAddr, s));
    if (!ForkProcess())
    {
      CloseSocket(sockfd);
      if (SendData(connfd, "Hello, world!", 13, 0) == -1)
      {
        perror("send");
      }
      CloseSocket(connfd);
      exit(0);
    }
    CloseSocket(connfd); // parent doesn't need this
  }
  return 0;
}

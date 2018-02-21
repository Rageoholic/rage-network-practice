#include "rnetwork.h"
#include "rprocess.h"

#include <errno.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>

void SigchildHandler(int s)
{
  (void)s;
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
  if ((rv = GetAddrInfo(AI_PASSIVE, SOCK_STREAM, argv[1], NULL,
                        &servInfo))
      != 0)
  {
    fprintf(stderr, "%s\n", GaiError(rv));
    return 2;
  }

  fd sockfd = BindToSocket(servInfo);
  if (SetSignalHandler(SIGCHLD, SigchildHandler, SA_RESTART) == -1)
  {
    perror("SetSignalHandler");
    return 1;
  }
  FreeAddrInfo(servInfo);
  puts("server: Waiting for connections");

  while (true) {
    SockAddrStorage theirAddr = {0};
    socklen_t sinSize = sizeof(theirAddr);
    fd connfd = AcceptConnection(sockfd, (SockAddr *) &theirAddr, sinSize);

    if (connfd == -1)
    {
      perror("AcceptConnection");
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

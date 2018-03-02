#include "rutils/network.h"

#include <stdio.h>

int main(int argc, char **argv)
{
  if (argc != 2)
  {
    fprintf(stderr, "usage: %s port\n", argv[0]);
    return 2;
  }

  Socket lSock = CreateUDPListenerSocket(argv[1]);

  if (!IsValidSocket(lSock))
  {
    fputs("listener: failed to bind", stderr);
    return 2;
  }

  puts("Ready for data");

  char buf[1000];

  SockAddr theirAddr;
  Length l = UDPRecvData(lSock, buf, sizeof(buf) - 1, 0, &theirAddr);

  if (l == -1)
  {
    fputs("listener: UDPRecvData failed", stderr);
    return 2;
  }

  buf[l] = '\0';

  char ipStr[MAX_ADDR_STR_LEN];

  printf("Recieved data from %s\n\t%s\n", SockAddrToStr(&theirAddr, ipStr),
         buf);

  DestroySockAddr(theirAddr);

  DestroySocket(lSock);
}

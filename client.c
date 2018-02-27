#include "rnetwork.h"
#include <stdio.h>

int main(int argc, char *argv[])
{
  if(argc != 3)
  {
    fprintf(stderr, "usage: %s name port\n", argv[0]);
    return 1;
  }
  int rv;
  AddrInfo *servInfo;
  if((rv = GetAddrInfo(CLIENT, TCP,  argv[2], argv[1], &servInfo)) != 0)
  {
    fprintf(stderr, "GetAddrInfo: %s\n", GaiError(rv));
    return 1;
  }

  fd connfd = ConnectToSocket(servInfo);
  if(connfd == -1)
  {
    fputs("client: failed to connect", stderr);
    return 2;
  }

  char ipbuf[MAX_ADDR_STR_LEN];
  printf("client: connecting to %s\n", GetIpStr(servInfo, ipbuf, sizeof(ipbuf)));

  FreeAddrInfo(servInfo);

  char buf[1000];

  int numbytes = ReadData(connfd, buf, sizeof(buf) -1, 0);

  if(numbytes == -1)
  {
    perror("ReadData");
    return 1;
  }

  buf[numbytes] = '\0';

  printf("client: recieved \"%s\"\n", buf);

  CloseSocket(connfd);
}

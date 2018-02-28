#include "rutils/network.h"
#include <stdio.h>

int main(int argc, char *argv[])
{
  if(argc != 3)
  {
    fprintf(stderr, "usage: %s name port\n", argv[0]);
    return 1;
  }

  AddrInfo servInfo;

  Socket connfd = CreateTCPClientSocket(argv[1], argv[2], &servInfo);
  if(connfd == -1)
  {
    fputs("client: failed to connect", stderr);
    return 2;
  }

  char ipbuf[MAX_ADDR_STR_LEN];
  printf("client: connecting to %s\n", GetIpStr(servInfo, ipbuf, sizeof(ipbuf)));

  DestroyAddrInfo(servInfo);

  char buf[1000];

  Length numbytes = TCPRecvData(connfd, buf, sizeof(buf) -1, 0);

  if(numbytes == -1)
  {
    perror("ReadData");
    return 1;
  }

  buf[numbytes] = '\0';

  printf("client: recieved \"%s\"\n", buf);

  DestroySocket(connfd);
}

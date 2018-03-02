#include "rutils/network.h"

#include <stdio.h>
#include <string.h>

int main(int argc, char **argv)
{
  if (argc != 4)
  {
    fprintf(stderr, "usage: %s host port message\n", argv[0]);
    return 2;
  }

  SockAddr theirAddr = {};
  Socket tsock = CreateUDPTalkerSocket(argv[1], argv[2], &theirAddr);

  if (!IsValidSocket(tsock))
  {
    PrintError("talker");
    return 2;
  }
  Length msgLen = strlen(argv[3]);
  Length l = UDPSendData(tsock, argv[3], msgLen, theirAddr);
  if (l == -1)
  {
    PrintError("talker");
    return 2;
  }
  puts("data sent");

  DestroySockAddr(theirAddr);
  return 0;
}

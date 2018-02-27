#include "rnetwork.h"

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>


int main(int argc, char *argv[])
{

  if (argc != 2)
  {
    fprintf(stderr, "usage: %s hostname\n", argv[0]);
    return 1;
  }

  AddrInfo *tAddrInfo;

  int rv;
  if ((rv = GetAddrInfo(0, TCP, argv[1], NULL, &tAddrInfo))
      != 0)
  {
    fprintf(stderr, "%s\n", GaiError(rv));
    return 2;
  }

  printf("IP addresses for %s\n", argv[1]);

  for (AddrInfo *p = tAddrInfo; p != NULL; p = NextAddrInfo(p)) {
    char *ipver;

    switch (GetIpVer(p))
    {
    case IPV6: ipver = "IPv6"; break;
    default: ipver = "IPv4"; break;
    }
    char ipBuffer[MAX_ADDR_STR_LEN];
    printf("\t%s: %s\n", ipver, GetIpStr(p, ipBuffer, sizeof(ipBuffer)));
  }

  DestroyAddrInfo(tAddrInfo);
}

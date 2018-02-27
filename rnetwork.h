#ifndef RNETWORK_H
#define RNETWORK_H

#include "rdef.h"

#define MAX_ADDR_STR_LEN 46

typedef enum
{
  TCP,
  UDP
}ConnType;

typedef enum
{
  SERVER,
  CLIENT
}Role;

typedef int64_t Length;

typedef struct addrinfo AddrInfo;

typedef struct sockaddr_storage SockAddr;

void *GetInAddr(SockAddr *sa);

int GetAddrInfo(Role role, ConnType connType, const char *port,
                const char *name, struct addrinfo **res);

const char *GaiError(int errcode);

fd BindToAddrInfo(AddrInfo *a, int connBacklog);

fd ConnectToSocket(AddrInfo *a);

AddrInfo *NextAddrInfo(AddrInfo *a);

typedef enum ipver { IPV4, IPV6, NO_SPEC } IpVer;

IpVer GetIpVer(AddrInfo *a);

const char *GetIpStr(AddrInfo *a, char *ipstr, size_t ipstrLength);

void FreeAddrInfo(AddrInfo *a);

fd AcceptConnection(fd sockfd, SockAddr **theirAddr);

const char *SockAddrToStr(SockAddr *addr, char *dst);

int CloseSocket(fd sockfd);

Length SendData(fd sockfd, const void *buf, size_t len, int flags);

Length ReadData(fd sockfd, void *buf, size_t len, int flags);
#endif

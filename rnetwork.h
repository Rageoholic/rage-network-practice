#ifndef RNETWORK_H
#define RNETWORK_H

#include "rdef.h"

#define MAX_ADDR_STR_LEN 46

typedef FD Socket;

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

Socket CreateTCPServerSocket(const char *port);

Socket CreateTCPClientSocket(const char *name, const char *port, AddrInfo **givenServInfo);

void *GetInAddr(SockAddr *sa);

int GetAddrInfo(Role role, ConnType connType, const char *port,
                const char *name, struct addrinfo **res);

const char *GaiError(int errcode);

Socket BindToAddrInfo(AddrInfo *a, int connBacklog);

Socket ConnectToSocket(AddrInfo *a);

AddrInfo *NextAddrInfo(AddrInfo *a);

typedef enum ipver { IPV4, IPV6, NO_SPEC } IpVer;

IpVer GetIpVer(AddrInfo *a);

const char *GetIpStr(AddrInfo *a, char *ipstr, size_t ipstrLength);

void FreeAddrInfo(AddrInfo *a);

Socket AcceptConnection(Socket sockfd, SockAddr **theirAddr);

const char *SockAddrToStr(SockAddr *addr, char *dst);

int CloseSocket(Socket sockfd);

Length SendData(Socket sockfd, const void *buf, size_t len, int flags);

Length ReadData(Socket sockfd, void *buf, size_t len, int flags);
#endif

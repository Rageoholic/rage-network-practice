#ifndef RNETWORK_H
#define RNETWORK_H

#include "rdef.h"

#include <netdb.h>
#include <sys/socket.h>
#include <sys/types.h>

typedef const int fd;

typedef struct addrinfo AddrInfo;

typedef struct sockaddr SockAddr;

typedef struct sockaddr_storage SockAddrStorage;

void *GetInAddr(SockAddr *sa);

int GetAddrInfo(int aiFlags, int aiSocktype, const char *port,
                const char *name, struct addrinfo **res);

const char *GaiError(int errcode);

fd BindToAddrInfo(AddrInfo *a);

fd ConnectToSocket(AddrInfo *a);

AddrInfo *NextAddrInfo(AddrInfo *a);

typedef enum ipver { IPV4, IPV6 } IpVer;

IpVer GetIpVer(AddrInfo *a);

const char *GetIpStr(AddrInfo *a, char *ipstr, size_t ipstrLength);

void FreeAddrInfo(AddrInfo *a);

fd AcceptConnection(fd sockfd, SockAddr *theirAddr, socklen_t addrLen);

const char *SockAddrToStr(SockAddrStorage *addr, char *dst);

int CloseSocket(fd sockfd);

ssize_t SendData(fd sockfd, const void *buf, size_t len, int flags);

ssize_t ReadData(fd sockfd, void *buf, size_t len, int flags);
#endif

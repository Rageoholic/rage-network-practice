#define _XOPEN_SOURCE 1000

#include "rnetwork.h"
#include <stdio.h>

#include <arpa/inet.h>
#include <netdb.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

local int RoleToAIFlags(Role role)
{
  if(role == SERVER)
  {
    return AI_PASSIVE;
  }else
  {
    return 0;
  }
}

Socket CreateTCPServerSocket(const char *port)
{
  AddrInfo *servInfo;

  int rv;
  if ((rv = GetAddrInfo(SERVER, TCP,  port, NULL, &servInfo))
      != 0)
  {
    fprintf(stderr, "%s\n", GaiError(rv));
    return -1;
  }

  Socket sockfd = BindToAddrInfo(servInfo, 10);
  FreeAddrInfo(servInfo);
  return sockfd;
}

Socket CreateTCPClientSocket(const char *name, const char *port, AddrInfo **givenServInfo)
{
  AddrInfo *altServInfo = NULL;

  AddrInfo **servInfo;
  if(givenServInfo == NULL)
  {
    servInfo = &altServInfo;
  }
  else
  {
    servInfo = givenServInfo;
  }
  int rv;
  if ((rv = GetAddrInfo(SERVER, TCP,  port, name, servInfo))
      != 0)
  {
    fprintf(stderr, "%s\n", GaiError(rv));
    return -1;
  }

  Socket sockfd = ConnectToSocket(*servInfo);
  if(altServInfo != NULL)
  {
    FreeAddrInfo(altServInfo);
  }
  return sockfd;
}

local int ConnTypeToSockType(ConnType connType)
{
  if(connType == TCP)
  {
    return SOCK_STREAM;
  }else
  {
    return SOCK_DGRAM;
  }
}

void *GetInAddr(SockAddr *sa)
{
  if (((struct sockaddr *)sa)->sa_family == AF_INET)
  {
    return &(((struct sockaddr_in *)sa)->sin_addr);
  }
  else
  {
    return &(((struct sockaddr_in6 *)sa)->sin6_addr);
  }
}

int GetAddrInfo(Role role, ConnType connType, const char *port,
                const char *name, AddrInfo **res)
{
  AddrInfo hints = {.ai_flags = RoleToAIFlags(role),
                    .ai_family = AF_UNSPEC,
                    .ai_socktype = ConnTypeToSockType(connType)};

  return getaddrinfo(name, port, &hints, res);
}

const char *GaiError(int errcode)
{
  return gai_strerror(errcode);
}

AddrInfo *NextAddrInfo(AddrInfo *a)
{
  return a->ai_next;
}

Socket BindToAddrInfo(AddrInfo *a, int connBacklog)
{
  for(AddrInfo *p = a; p != NULL; p = NextAddrInfo(p))
  {
    int sockfd;
    if((sockfd = socket(p->ai_family, p->ai_socktype, p->ai_protocol)) == -1)
    {
      perror("socket");
      continue;
    }
    int yes = 1;
    if(setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int)) == -1)
    {
      perror("setsockopt");
      exit(1);
    }

    if(bind(sockfd, p->ai_addr, p->ai_addrlen) == -1)
    {
      close(sockfd);
      perror("bind");
      continue;
    }

    if(listen(sockfd, connBacklog) == -1)
    {
      perror("listen");
      close(sockfd);
      continue;
    }
    return sockfd;
  }
  /* Error case */
  return -1;
}

Socket ConnectToSocket(AddrInfo *a)
{
  for (AddrInfo *p = a; p != NULL; p = NextAddrInfo(p)) {
    int sockfd;
    if((sockfd = socket(p->ai_family, p->ai_socktype, p->ai_protocol)) == -1)
    {
      perror("ConnectToSocket: socket");
      continue;
    }

    if(connect(sockfd, p->ai_addr, p->ai_addrlen) == -1)
    {
      close(sockfd);
      perror("ConnectToSocket: connect");
      continue;
    }

    return sockfd;
  }
  return -1;
}

IpVer GetIpVer(AddrInfo *a)
{
  return (a->ai_family == AF_INET) ? IPV4 : IPV6;
}

const char *GetIpStr(AddrInfo *a, char *ipstr, size_t ipstrLength)
{
  void *addr;

  if (GetIpVer(a) == IPV4)
  {
    struct sockaddr_in *ipv4 = (struct sockaddr_in *)a->ai_addr;
    addr = &(ipv4->sin_addr);
  }
  else
  {
    struct sockaddr_in6 *ipv6 = (struct sockaddr_in6 *)a->ai_addr;
    addr = &(ipv6->sin6_addr);
  }

  return inet_ntop(a->ai_family, addr, ipstr, ipstrLength);
}

void FreeAddrInfo(AddrInfo *a)
{
  freeaddrinfo(a);
}

Socket AcceptConnection(Socket sockfd, SockAddr **theirAddr)
{
  *theirAddr = calloc(1,sizeof(SockAddr));
  socklen_t addrLen = sizeof(SockAddr);
  return accept(sockfd, (struct sockaddr *)*theirAddr, &addrLen);
}

const char *SockAddrToStr(SockAddr *addr, char *dst)
{
  return inet_ntop(addr->ss_family, GetInAddr((SockAddr *)addr), dst,
                   sizeof(*addr));
}

int CloseSocket(Socket sockfd)
{
  return close(sockfd);
}

Length SendData(Socket sockfd, const void *buf, size_t len, int flags)
{
  return send(sockfd, buf, len, flags);
}

Length ReadData(Socket sockfd, void *buf, size_t len, int flags)
{
  return recv(sockfd, buf, len, flags);
}

#define _XOPEN_SOURCE 1000

#include "rnetwork.h"
#include <stdio.h>

#include <arpa/inet.h>
#include <netdb.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

void *GetInAddr(SockAddr *sa)
{
  if (sa->sa_family == AF_INET)
  {
    return &(((struct sockaddr_in *)sa)->sin_addr);
  }
  else
  {
    return &(((struct sockaddr_in6 *)sa)->sin6_addr);
  }
}

int GetAddrInfo(int aiFlags, int aiSocktype, const char *port,
                const char *name, AddrInfo **res)
{
  AddrInfo hints
      = {.ai_flags = aiFlags, .ai_family = AF_UNSPEC, .ai_socktype = aiSocktype};

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

fd BindToSocket(AddrInfo *a)
{
  for (AddrInfo *p = a; p != NULL; p = NextAddrInfo(p)) {
    int sockfd;
    if ((sockfd = socket(p->ai_family, p->ai_socktype, p->ai_protocol)) == -1)
    {
      perror("socket");
      continue;
    }

    int yes = 1;
    if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int)) == -1)
    {
      perror("setsockopt");
      exit(1);
    }

    if (bind(sockfd, p->ai_addr, p->ai_addrlen) == -1)
    {
      close(sockfd);
      perror("bind");
      continue;
    }
    return sockfd;
  }
  return -1;
}

fd ConnectToSocket(AddrInfo *a)
{
  for (AddrInfo *p = a; p != NULL; p = NextAddrInfo(p)) {
    int sockfd;
    if((sockfd = socket(p->ai_family, p->ai_socktype, p->ai_protocol)) == -1)
    {
      perror("ConnectToSocket");
      continue;
    }

    if(connect(sockfd, p->ai_addr, p->ai_addrlen) == -1)
    {
      close(sockfd);
      perror("ConnectToSocket: connect");
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

fd AcceptConnection(fd sockfd, SockAddr *theirAddr, socklen_t addrLen)
{
  return accept(sockfd, theirAddr, &addrLen);
}

const char *SockAddrToStr(SockAddrStorage *addr, char *dst)
{
  return inet_ntop(addr->ss_family, GetInAddr((SockAddr *)addr), dst,
                   sizeof(*addr));
}

int CloseSocket(fd sockfd)
{
  return close(sockfd);
}

ssize_t SendData(fd sockfd, const void *buf, size_t len, int flags)
{
  return send(sockfd, buf, len, flags);
}

ssize_t ReadData(fd sockfd, void *buf, size_t len, int flags)
{
  return recv(sockfd, buf, len, flags);
}

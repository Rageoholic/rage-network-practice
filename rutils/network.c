#define _XOPEN_SOURCE 1000

#include "network.h"
#include <stdio.h>

#include <arpa/inet.h>
#include <netdb.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#include <string.h>

#include <assert.h>

#define SOCKADDR_LENGTH_DEFAULT sizeof(struct sockaddr_storage)

local int ReadFlagsToRecvFlags(ReadFlags flags)
{
  int recvFlags = 0;
  if(flags & READ_PEEK)
  {
    recvFlags |= MSG_PEEK;
  }
  if(flags & READ_NO_BLOCK)
  {
    recvFlags |= MSG_DONTWAIT;
  }
  return recvFlags;
}

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

local void *GetInAddr(_SockAddr *sa)
{
  assert(sa != NULL);
  if (((struct sockaddr *)sa)->sa_family == AF_INET)
  {
    return &(((struct sockaddr_in *)sa)->sin_addr);
  }
  else
  {
    return &(((struct sockaddr_in6 *)sa)->sin6_addr);
  }
}

typedef struct
{
  Socket s;
  AddrInfo p;
} AITSTuple;

local AITSTuple AddrInfoToSocket(AddrInfo a)
{

  for (AddrInfo p = a; p != NULL; p = NextAddrInfo(p)) {
    int sockfd;
    if((sockfd = socket(p->ai_family, p->ai_socktype, p->ai_protocol)) == -1)
    {

      perror("AddrInfoToSocket: socket");
      continue;

    }
    AITSTuple ret = {sockfd, p};
    return ret;
  }
  AITSTuple ret = {-1, NULL};
  return ret;
}

Socket CreateTCPServerSocket(const char *port, int connBacklog)
{
  AddrInfo servInfo;

  int rv;
  if ((rv = GetAddrInfo(SERVER, TCP,  NULL, port, &servInfo))
      != 0)
  {
    fprintf(stderr, "%s\n", GaiError(rv));
    return -1;
  }

  Socket sockfd = BindToAddrInfo(servInfo);
  if(sockfd == -1)
  {
    return -1;
  }
  DestroyAddrInfo(servInfo);

  if(ListenToTCPSocket(sockfd, connBacklog) == -1)
  {
    perror("CreateTCPServerSocket: ListenToTCPSocket");
    DestroySocket(sockfd);
    return -1;
  }


  return sockfd;
}

Socket CreateTCPClientSocket(const char *name, const char *port, AddrInfo *givenServInfo)
{
  AddrInfo altServInfo = NULL;

  AddrInfo *servInfo;
  if(givenServInfo == NULL)
  {
    servInfo = &altServInfo;
  }
  else
  {
    servInfo = givenServInfo;
  }
  int rv;
  if ((rv = GetAddrInfo(CLIENT, TCP,  name, port, servInfo))
      != 0)
  {
    fprintf(stderr, "%s\n", GaiError(rv));
    return -1;
  }

  Socket sockfd = ConnectToTCPSocket(*servInfo);
  if(altServInfo != NULL)
  {
    DestroyAddrInfo(altServInfo);
  }
  return sockfd;
}

Socket CreateUDPListenerSocket(const char *port)
{
  AddrInfo servInfo;

  int rv;
  if((rv = GetAddrInfo(SERVER, SOCK_DGRAM, NULL, port, &servInfo)) != 0)
  {
    fprintf(stderr, "%s\n", GaiError(rv));
    return -1;
  }

  Socket sockfd = BindToAddrInfo(servInfo);
  DestroyAddrInfo(servInfo);
  return sockfd;
}

Socket CreateUDPTalkerSocket(const char *name, const char *port,
                             SockAddr *theirAddr)
{
  assert(theirAddr != NULL);

  AddrInfo servInfo;
  int rv;
  if((rv = GetAddrInfo(CLIENT, SOCK_DGRAM, name, port, &servInfo)) != 0)
  {
    fprintf(stderr, "%s\n", GaiError(rv));
    return -1;
  }

  AITSTuple udpconn = AddrInfoToSocket(servInfo);

  theirAddr->_len = udpconn.p->ai_addrlen;
  theirAddr->_s = malloc(theirAddr->_len);
  memcpy(theirAddr->_s, udpconn.p->ai_addr, theirAddr->_len);

  DestroyAddrInfo(servInfo);
  return udpconn.s;
}


int GetAddrInfo(Role role, ConnType connType, const char *name,
                const char *port, AddrInfo *res)
{
  _AddrInfo hints = {.ai_flags = RoleToAIFlags(role),
                    .ai_family = AF_UNSPEC,
                    .ai_socktype = ConnTypeToSockType(connType)};

  return getaddrinfo(name, port, &hints, res);
}

const char *GaiError(int errcode)
{
  return gai_strerror(errcode);
}

AddrInfo NextAddrInfo(AddrInfo a)
{
  return a->ai_next;
}

Socket BindToAddrInfo(AddrInfo a)
{
  for(AddrInfo p = a; p != NULL; p = NextAddrInfo(p))
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


    return sockfd;
  }
  /* Error case */
  return -1;
}

int ListenToTCPSocket(Socket sockfd, int connBacklog)
{
  if(listen(sockfd, connBacklog) == -1)
  {
    return -1;
  }else
  {
    return 0;
  }
}

Socket ConnectToTCPSocket(AddrInfo a)
{
  for (AddrInfo p = a; p != NULL; p = NextAddrInfo(p)) {
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



IpVer GetIpVer(AddrInfo a)
{
  return (a->ai_family == AF_INET) ? IPV4 : IPV6;
}

const char *GetIpStr(AddrInfo a, char *ipstr, size_t ipstrLength)
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

void DestroyAddrInfo(AddrInfo a)
{
  freeaddrinfo(a);
}

Socket AcceptConnection(Socket sockfd, SockAddr *theirAddr)
{
  _SockAddr *lTheirAddr = NULL;
  if(theirAddr != NULL)
  {
    theirAddr->_s = malloc(SOCKADDR_LENGTH_DEFAULT);
    lTheirAddr = theirAddr->_s;
  }
  socklen_t addrLen = theirAddr ? SOCKADDR_LENGTH_DEFAULT : 0;
  return accept(sockfd, lTheirAddr, &addrLen);
}

const char *SockAddrToStr(SockAddr *addr, char *dst)
{
  return inet_ntop(((struct sockaddr_storage *)addr->_s)->ss_family, GetInAddr(addr->_s), dst,
                   addr->_len);
}

int DestroySocket(Socket sockfd)
{
  return close(sockfd);
}

Length TCPSendData(Socket sockfd, const void *buf, size_t len)
{
  return send(sockfd, buf, len, 0);
}

Length TCPRecvData(Socket sockfd, void *buf, size_t len, ReadFlags flags)
{
  return recv(sockfd, buf, len, ReadFlagsToRecvFlags(flags));
}

Length UDPRecvData(Socket sockfd, void *buf, size_t len, ReadFlags flags,
                   SockAddr *theirAddr)
{
  SockAddr lTheirAddr;

  lTheirAddr._s = malloc(sizeof (struct sockaddr_storage));

  lTheirAddr._len = sizeof(struct sockaddr_storage);

  if(theirAddr != NULL)
  {
    *theirAddr = lTheirAddr;
  }else
  {
    free(lTheirAddr._s);
  }
  socklen_t socklen = lTheirAddr._len;
  return recvfrom(sockfd, buf, len, flags, lTheirAddr._s, &socklen);
}

Length UDPSendData(Socket sockfd, const void *buf, size_t len, SockAddr *s)
{
  return sendto(sockfd, buf, len, 0, s->_s, s->_len);
}

void PrintError(char *error)
{
  perror(error);
}

void DestroySockAddr(SockAddr s)
{
  free(s._s);
}

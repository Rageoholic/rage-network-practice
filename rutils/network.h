#ifndef RNETWORK_H
#define RNETWORK_H

#include "def.h"

/* Types and constants */

#define MAX_ADDR_STR_LEN 46

typedef FD Socket;

typedef enum
{
  TCP,				/* Safe guaranteed transfer */
  UDP				/* Unsafe non-guaranteed transfer */
} ConnType;

typedef enum
{
  SERVER,
  CLIENT
} Role;

typedef int64_t Length;

typedef struct addrinfo _AddrInfo, *AddrInfo;

typedef struct sockaddr _SockAddr;

typedef struct len_with_sockaddr
{
  _SockAddr *_s;
  size_t _len;
}SockAddr;

typedef enum ipver { IPV4, IPV6, NO_SPEC } IpVer;



/* Functions */

/*
  func: CreateTCPServerSocket

  Creates a TCP socket to which clients can connect

  args

  port: a character representation of the port you wish to bind to
*/

Socket CreateTCPServerSocket(const char *port, int connBacklog);

/*
  func: CreateTCPClientSocket

  Connect to an external server via TCP

  args

  name: The name of the external server you wish to connect to

  port: a character representation of the port you wish to connect to

  givenServInfo: A pointer to a pointer to an AddrInfo. Pass it a
  locally allocated pointer to an AddrInfo and it will change it to
  point to an actual AddrInfo. Pass NULL if you couldn't give a shit.
*/

Socket CreateTCPClientSocket(const char *name, const char *port, AddrInfo *givenServInfo);

/*
  func: CreateUDPListenerSocket

  Create a UDP socket we can listen on for data

  args

  port: a character representation of the port you wish to bind to
*/
Socket CreateUDPListenerSocket(const char *port);

/*
  func: CreateUDPTalkerSocket

  Create a UDP socket we can use to connect externally

  args

  name: host name for the server

  port: string rep of port to connect to

  servInfo: A pointer to fill out with the pointer to our AddrInfos
  WARNING: Cannot be NULL. If its NULL you can't send data
*/
Socket CreateUDPTalkerSocket(const char *name, const char *port,
			     SockAddr *theirAddr);
/*
  func: GetAddrInfo

  Get the AddrInfo of a specified server. If it doesn't return 0 then
  you can use GaiError to get the string explanation of your error

  args

  role: Are you the server or client

  connType: Type of socket to ask for

  name: Name of server to connect to. If you are the server pass NULL

  port: String representation of port to connect to. If you don't care
  pass NULL

  res: Fills out the pointer passed to this with a pointer to the
  addrinfo
*/

int GetAddrInfo(Role role, ConnType connType, const char *name,
                const char *port, AddrInfo *res);

/*
  func: GaiError

  Turn an error from GetAddrInfo into a string representation

  args

  errcode: the return value from GetAddrInfo
*/

const char *GaiError(int errcode);

/*
  func: BindToAddrInfo

  Bind and start listening to a server socket

  args

  a: The AddrInfo to bind to

  connBacklog: The maximum length of the listen queue
*/

Socket BindToAddrInfo(AddrInfo a);

/*
  func: ConnectToSocket

  Connect to a server

  args

  a: AddrInfo to connect to
*/

int ListenToTCPSocket(Socket sockfd, int connBacklog);

Socket ConnectToTCPSocket(AddrInfo a);

/*
  func: AcceptConnection

  Accept an incoming connection on a socket

  args

  sockfd: The socket we want to accept from

  theirAddr: If NULL this is ignored. If not NULL fill in the pointer
  this points with a pointer to their SockAddr
*/

Socket AcceptConnection(Socket sockfd, SockAddr *theirAddr);

/*
  func: SockAddrToStr

  Convert a SockAddr to a string representation of that SockAddr

  args

  addr: pointer to our SockAddr

  dst: Buffer to filled out. A pointer to this buffer is also returned
  WARNING: Not presently NULL safe
*/

const char *SockAddrToStr(SockAddr *addr, char *dst);

/*
  func: TCPSendData

  Send a buffer of data over a TCP socket. Returns the length of the
  data sent or -1 if it fails

  args

  sockfd: Socket to send data over

  buf: data to send

  len: length of buf
*/

Length TCPSendData(Socket sockfd, const void *buf, size_t len);

/*
  func: TCPRecvData

  Read a buffer of data from a TCP socket. Returns the length of the
  data read or -1 if it fails

  args

  sockfd: Socket to send data over

  buf: Buffer to read data into

  len: length of buf

  flags: Flags we want to set. See ReadFlags for docs on what flags mean
*/

Length TCPRecvData(Socket sockfd, void *buf, size_t len, ReadFlags flags);

/*
  func: UDPSendData

  Send a buffer of data over a UDP socket. Returns the length of the
  data sent or -1 if it fails

  args

  sockfd: Socket to send data over

  buf: data to send

  len: length of buf

  theirAddr: An AddrInfo giving the SockAddr of the server to connect to
*/

Length UDPSendData(Socket sockfd, const void *buf, size_t len,
                   SockAddr *theirAddr);

/*
  func: UDPRecvData

  Recieve a buffer of date over a UDP data. Returns the length of the
  data sent or -1 if it fails

  args

  sockfd: socket to recieve data over

  buf: Buffer of data to fill out

  len: length of buf

  flags: set of read flags. See ReedFlags docs

  theirAddr: A pointer to a SockAddr to fill with data
*/

Length UDPRecvData(Socket sockfd, void *buf, size_t len, ReadFlags flags,
                   SockAddr *theirAddr);


/* Accessors */

AddrInfo NextAddrInfo(AddrInfo a);

IpVer GetIpVer(AddrInfo a);

const char *GetIpStr(AddrInfo a, char *ipstr, size_t ipstrLength);

void PrintError(char *errstr);

/* Destructors */
void DestroyAddrInfo(AddrInfo a);

void DestroySockAddr(SockAddr s);

int DestroySocket(Socket sockfd);


#endif

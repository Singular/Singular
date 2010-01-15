/***************************************************************************
 *
 *   HEADER FILE:  MP_TcpTransp.h
 *
 *        Isolate all the includes we need for unix TCP sockets.
 *
 *  Change Log:
 *       September 10, 1995  SG - Updated implementation.  Much cleaning
 *                                to make it presentable.
 *
 ***************************************************************************/
#ifndef __TcpTransp__h
#define __TcpTransp__h

#include <stdio.h>
#include <sys/types.h>
#include <sys/time.h>

#ifdef __WIN32__

#   include <winsock.h>

#   define  ERRORCODE(err)      WSA ## err
#   define  LASTERROR           WSAGetLastError()

#   define  CLOSE_SOCKET(s)             closesocket(s)
#   define  READ_SOCKET(s, buf, len)    recv(s, buf, len, 0)
#   define  WRITE_SOCKET(s, buf, len)   send(s, buf, len, 0)

int WinSockInitialize _ANSI_ARGS_((void));

#else /* not __WIN32__ */

#   include <sys/socket.h>
#   include <netinet/in.h>
#   include <netinet/tcp.h>
#   include <sys/ioctl.h>
#   include <arpa/inet.h>
#   include <netdb.h>

typedef int SOCKET;

#   define  INVALID_SOCKET      -1
#   define  SOCKET_ERROR        -1
#   define  IPPORT_RESERVED     1024
#   define  ERRORCODE(err)      err
#   define  LASTERROR           errno

#   define  CLOSE_SOCKET(s)             close(s)
#   define  READ_SOCKET(s, buf, len)    read(s, buf, len)
#   define  WRITE_SOCKET(s, buf, len)   write(s, buf, len)

#   define  HOSTENT             struct hostent
#   define  SOCKADDR            struct sockaddr
#   define  TIMEVAL             struct timeval
#   define  SOCKADDR_IN         struct sockaddr_in

#endif /* not __WIN32__ */


#define MP_NO_SUCH_TCP_MODE     0
#define MP_CONNECT_MODE         1
#define MP_LISTEN_MODE          2
#define MP_LAUNCH_MODE          3
#define MP_FORK_MODE            4

/* Initial port for listening */
#define MP_INIT_PORT            "1025"

/* The maximum number of attempts made to bind a socket to a port address */
#define MP_MAX_BIND_ATTEMPTS    1000

#define MP_ACCEPT_WAIT_SEC      5
#define MP_ACCEPT_WAIT_USEC     0

MP_Status_t tcp_flush _ANSI_ARGS_((MP_Link_pt link));
long tcp_write _ANSI_ARGS_((MP_Link_pt, char *, long));
long tcp_read _ANSI_ARGS_((MP_Link_pt, char *, long));
MP_Boolean_t tcp_get_status _ANSI_ARGS_((MP_Link_pt, MP_LinkStatus_t));
MP_Status_t tcp_init_transport _ANSI_ARGS_((MP_Link_pt));
MP_Status_t tcp_open_connection _ANSI_ARGS_((MP_Link_pt, int, char**));
MP_Status_t tcp_close_connection _ANSI_ARGS_((MP_Link_pt));
MP_Status_t tcp_kill_connection _ANSI_ARGS_((MP_Link_pt));
MP_Status_t open_tcp_connect_mode _ANSI_ARGS_((MP_Link_pt, int, char**));
MP_Status_t open_tcp_listen_mode _ANSI_ARGS_((MP_Link_pt, int, char**));
MP_Status_t open_tcp_launch_mode _ANSI_ARGS_((MP_Link_pt, int, char**));
MP_Status_t socket_accept_blocking _ANSI_ARGS_((MP_Link_pt, SOCKET*));
MP_Status_t socket_accept_non_blocking _ANSI_ARGS_((MP_Link_pt, SOCKET*));
char* IMP_GetCmdlineArg _ANSI_ARGS_((int, char**, const char*));


typedef struct{
    SOCKET  bindsock;
    SOCKET  sock;
    short   peerport;
    char    *peerhost;
    char    *myhost;
    MP_LinkStatus_t status;
    pid_t   peerpid;
  char *rsh;
  int mode;
} MP_TCP_t;

#endif

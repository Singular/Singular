/*****************************************
*  Computer Algebra System SINGULAR      *
*****************************************/
/*
* ABSTRACT: wrappijng signal-interuptable system calls
* AUTHOR: Alexander Dreyer, The PolyBoRi Team, 2013
*/

#include <signal.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/wait.h>

#include <unistd.h>
#include <sys/uio.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/socket.h>
#include <time.h>

#ifndef SINGULAR_SI_SIGNALS_H
#define SINGULAR_SI_SIGNALS_H

#define SI_EINTR_SAVE_FUNC(return_type, func, decl, args) \
inline return_type si_##func decl        \
{                                        \
  int res = -1;	                         \
  do                                     \
  {                                      \
    res = func args;		         \
  } while((res < 0) && (errno == EINTR));\
  return res;                            \
}


SI_EINTR_SAVE_FUNC(int, select,
                   (int nfds, fd_set *readfds, fd_set *writefds,
                    fd_set *exceptfds, struct timeval *timeout),
                   (nfds,readfds, writefds, exceptfds, timeout)
                   )

SI_EINTR_SAVE_FUNC(int, pselect,
                   (int nfds, fd_set *readfds, fd_set *writefds,
                    fd_set *exceptfds, const struct timespec *timeout,
                    const sigset_t *sigmask),
                   (nfds, readfds, writefds, exceptfds, timeout,sigmask)
                   )

SI_EINTR_SAVE_FUNC(pid_t, wait, (int *status), (status))
SI_EINTR_SAVE_FUNC(pid_t, waitpid, (pid_t pid, int *status, int options),
                   (pid, status, options))

SI_EINTR_SAVE_FUNC(int, waitid, 
                   (idtype_t idtype, id_t id, siginfo_t *infop, int options),
                   (idtype, id, infop, options))

SI_EINTR_SAVE_FUNC(ssize_t,  read, (int fd, void *buf, size_t count), 
                   (fd, buf, count))


SI_EINTR_SAVE_FUNC(ssize_t, readv,
                   (int fd, const struct iovec *iov, int iovcnt),
                   (fd, iov,iovcnt))

SI_EINTR_SAVE_FUNC(ssize_t, write, (int fd, const void *buf, size_t count),
                   (fd, buf, count))

SI_EINTR_SAVE_FUNC(ssize_t, writev, (int fd, const struct iovec *iov, int iovcnt),
                   (fd, iov, iovcnt) )
 
SI_EINTR_SAVE_FUNC(int, open, (const char *pathname, int flags),
                   (pathname, flags))

SI_EINTR_SAVE_FUNC(int, open, (const char *pathname, int flags, mode_t mode),
                   (pathname, flags, mode))
SI_EINTR_SAVE_FUNC(int, close, (int fd), (fd))

SI_EINTR_SAVE_FUNC(int, accept,
                   (int sockfd, struct sockaddr *addr, socklen_t *addrlen),
                   (sockfd, addr, addrlen))

SI_EINTR_SAVE_FUNC(int, connect,
                   (int sockfd, const struct sockaddr *addr, socklen_t addrlen),
                   (sockfd, addr, addrlen))

/// @note: We respect that the user may explictely deactivate the
/// restart feature by setting the second argumetn to NULL.
inline int
si_nanosleep(const struct timespec *req, struct timespec *rem) {

  int res = -1;
  do
  {
    res = nanosleep(req, rem);
  } while((rem != NULL) && (res < 0) && (errno == EINTR));
  return res;   
}

inline unsigned int si_sleep(unsigned int seconds)
{
  do
  {
    seconds = sleep(seconds);
  } while(seconds != 0);
  return 0;
}

// still todo: read,write,open   in ./omalloc/Misc/dlmalloc
/// TODO: wrap and replace the following system calls: from man 7 signal
/// ???fread, fwrite, fopen, fdopen, popen, fclose




#undef SI_EINTR_SAVE_FUNC


#endif /* SINGULAR_SI_SIGNALS_H */

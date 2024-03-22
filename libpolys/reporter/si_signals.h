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
#include <sys/select.h>

#include <unistd.h>
#include <sys/uio.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/socket.h>
#include <time.h>
#include <stdio.h>
#include <semaphore.h>
#include <stdarg.h>
#ifdef HAVE_POLL_H
#include <poll.h>
#endif

#ifndef SINGULAR_SI_SIGNALS_H
#define SINGULAR_SI_SIGNALS_H

#define SI_EINTR_SAVE_FUNC_TEMPLATE(return_type, newfunc, func, decl, args, err_domain) \
static inline return_type newfunc decl   \
{                                        \
  int res = -1;                                 \
  do                                     \
  {                                      \
    res = func args;                         \
  } while((res err_domain) && (errno == EINTR));\
  return res;                            \
}

#define SI_EINTR_SAVE_FUNC(return_type, func, decl, args) \
  SI_EINTR_SAVE_FUNC_TEMPLATE(return_type, si_##func, func, decl, args, < 0)

#define SI_EINTR_SAVE_SCANF(return_type, func, decl, args) \
  SI_EINTR_SAVE_FUNC_TEMPLATE(return_type, si_##func, func, decl, args, == EOF)

SI_EINTR_SAVE_FUNC(int, select,
                   (int nfds, fd_set *readfds, fd_set *writefds,
                    fd_set *exceptfds, struct timeval *timeout),
                   (nfds,readfds, writefds, exceptfds, timeout)
                   )

#ifdef HAVE_POLL
SI_EINTR_SAVE_FUNC(int, poll,
                   (struct pollfd *fds, nfds_t nfds, int timeout),
                   (fds, nfds, timeout)
                   )
#endif

SI_EINTR_SAVE_FUNC(pid_t, wait, (int *status), (status))
SI_EINTR_SAVE_FUNC(pid_t, waitpid, (pid_t pid, int *status, int options),
                   (pid, status, options))

//SI_EINTR_SAVE_FUNC(int, waitid,
//                   (idtype_t idtype, id_t id, siginfo_t *infop, int options),
//                   (idtype, id, infop, options))

SI_EINTR_SAVE_FUNC(ssize_t,  read, (int fd, void *buf, size_t count),
                   (fd, buf, count))


//SI_EINTR_SAVE_FUNC(ssize_t, readv,
//                   (int fd, const struct iovec *iov, int iovcnt),
//                   (fd, iov,iovcnt))

SI_EINTR_SAVE_FUNC(ssize_t, write, (int fd, const void *buf, size_t count),
                   (fd, buf, count))

//SI_EINTR_SAVE_FUNC(ssize_t, writev, (int fd, const struct iovec *iov, int iovcnt),
//                   (fd, iov, iovcnt) )

SI_EINTR_SAVE_FUNC_TEMPLATE(int, si_open1, open, (const char *pathname, int flags),
                            (pathname, flags), < 0)
SI_EINTR_SAVE_FUNC_TEMPLATE(int, si_open2, open,
                            (const char *pathname, int flags, mode_t mode),
                            (pathname, flags, mode), < 0)

/* Enulate overloading usung preprocessor (we need to be C-compatible) */
#define SI_GET_FIFTH(_4,_3, _2, _1, N, ...) N
#define si_open(...) SI_GET_FIFTH(X,##__VA_ARGS__, si_open2, si_open1)(__VA_ARGS__)

//SI_EINTR_SAVE_FUNC(int, creat, (const char *pathname, mode_t mode),
//                   (pathname, mode))


SI_EINTR_SAVE_FUNC(int, close, (int fd), (fd))

SI_EINTR_SAVE_FUNC(int, accept,
                   (int sockfd, struct sockaddr *addr, socklen_t *addrlen),
                   (sockfd, addr, addrlen))

SI_EINTR_SAVE_FUNC(int, connect,
                   (int sockfd, const struct sockaddr *addr, socklen_t addrlen),
                   (sockfd, addr, addrlen))

#if 0
/* @note: We respect that the user may explictely deactivate the
 * restart feature by setting the second argumetn to NULL.
 */
static inline int
si_nanosleep(const struct timespec *req, struct timespec *rem)
{
  int res = -1;
  do
  {
    res = nanosleep(req, rem);
  } while((rem != NULL) && (res < 0) && (errno == EINTR));
  return res;
}
#endif

static inline unsigned int
si_sleep(unsigned int seconds)
{
  do
  {
    seconds = sleep(seconds);
  } while(seconds != 0);
  return 0;
}

//SI_EINTR_SAVE_FUNC(int, dup, (int oldfd), (oldfd))
SI_EINTR_SAVE_FUNC(int, dup2, (int oldfd, int newfd), (oldfd, newfd))
//SI_EINTR_SAVE_FUNC(int, dup3, (int oldfd, int newfd, int flags),
//                   (oldfd, newfd, flags))

SI_EINTR_SAVE_FUNC(int, unlink, (const char *pathname), (pathname))

SI_EINTR_SAVE_SCANF(int, vscanf,
                   (const char *format, va_list ap),
                   (format, ap))

static inline
int si_scanf(const char *format, ...)
{
  va_list argptr;
  va_start(argptr, format);
  int res = si_vscanf(format, argptr);
  va_end(argptr);
  return res;
}

SI_EINTR_SAVE_SCANF(int, vfscanf,
                   (FILE *stream, const char *format, va_list ap),
                   (stream, format, ap))

static inline int
si_fscanf(FILE *stream, const char *format, ...)
{
  va_list argptr;
  va_start(argptr, format);
  int res = si_vfscanf(stream, format, argptr);
  va_end(argptr);
  return res;
}

SI_EINTR_SAVE_SCANF(int, vsscanf,
                   (const char *str, const char *format, va_list ap),
                   (str, format, ap))

static inline int
si_sscanf(const char *str, const char *format, ...)
{
  va_list argptr;
  va_start(argptr, format);
  int res = si_vsscanf(str, format, argptr);
  va_end(argptr);
  return res;
}

SI_EINTR_SAVE_FUNC(int, stat, (const char *path, struct stat *buf),
                   (path, buf))
SI_EINTR_SAVE_FUNC(int, fstat, (int fd, struct stat *buf),
                   (fd, buf))
SI_EINTR_SAVE_FUNC(int, lstat, (const char *path, struct stat *buf),
                   (path, buf))


SI_EINTR_SAVE_FUNC(int, sigaction,
                   (int signum, const struct sigaction *act,
                    struct sigaction *oldact),
                   (signum, act, oldact))


#ifdef HAVE_SIGINTERRUPT
SI_EINTR_SAVE_FUNC(int, siginterrupt, (int sig, int flag),
                   (sig, flag))
#else
#define si_siginterrupt(arg1, arg2)
#endif


SI_EINTR_SAVE_FUNC(int, sem_wait, (sem_t *sem), (sem))
SI_EINTR_SAVE_FUNC(int, sem_trywait, (sem_t *sem), (sem))
//SI_EINTR_SAVE_FUNC(int, sem_timedwait,
//                   (sem_t *sem, const struct timespec *abs_timeout),
//                   (sem, abs_timeout))


#undef SI_EINTR_SAVE_FUNC


#endif /* SINGULAR_SI_SIGNALS_H */

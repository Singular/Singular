/***********************************************************************
 *
 *                    MP version 1.1.2:  Multi Protocol
 *                    Kent State University, Kent, OH
 *                 Authors:  S. Gray, N. Kajler, P. Wang
 *         (C) 1993, 1994, 1995, 1996, 1997 All Rights Reserved
 *
 *                                 NOTICE
 *
 *  Permission to use, copy, modify, and distribute this software and
 *  its documentation for non-commercial purposes and without fee is
 *  hereby granted provided that the above copyright notice appear in all
 *  copies and that both the copyright notice and this permission notice
 *  appear in supporting documentation.
 *
 *  Neither Kent State University nor the Authors make any representations
 *  about the suitability of this software for any purpose.  The MP Library
 *  is distributed in the hope that it will be useful, but is provided  "as
 *  is" and without any warranty of any kind and without even the implied
 *  warranty of merchantability or fitness for a particular purpose.
 *
 *    IMPORTANT ADDITION: as of September 2006, MP is also licenced under GPL
 *
 *   IMPLEMENTATION FILE:  MP_TcpTransp.c
 *
 *      Public interface routines to the "tcp" transport device.
 *      These include read, write, open, close, and init routines.
 *      There are some support routines as well.
 *
 *  Change Log:
 *       08/16/96   hennig     - tcp transport device ported to Windows95/NT
 *       11/25/95   sgray      - added support to negotiate endian word order.
 *       11/20/95   sgray      - added tcp_has_data() routine to determine
 *                               if there is any data on the link to read and
 *                               put it in the transp_op structure
 *       11/15/95   obachman   - use vfork instead of fork
 *                             - major change to open_tcp_launch_mode:
 *                             1.) -MPapplication takes one string: application
 *                                 and its arguments
 *                             2.) MP appends -MPhost -MPport to arguments of
 *                                 application
 *                             3.) rsh is started with -n to disable
 *                                 standard input
 *                             4.) rsh has fixed number of args, namely:
 *                                 -n host application
 *       11/14/95   obachman   - introduced macro for remote shell command
 *       11/10/95   obachman   - cleaned up things around the select call,
 *                               so that it runs under Linux, and uses
 *                               the timestruct to actually come back after
 *                               an unsuccessful select
 *                             - fixed nasty bug on line 348:
 *                      tcp_rec->peerhost = (char *) malloc(sizeof(host) + 1);
 *                      should have been
 *                      tcp_rec->peerhost = (char *) malloc(strlen(host) + 1);
 *                              - similar in tcp_inti_transport
 *                              - one minor cast to (Char **) of dmy_args
 *       September 11, 1995 SG - Reorganization of files.  Cleaned up
 *                               header files and relocated source to
 *       3/3/96    sgray - moved call to tcp_init_transport() inside
 *                         tcp_open_transport() and removed the init
 *                         routine from the tcp_ops struct.
 *       5/16/96   sgray - Changed the tcp_read() routine to loop until it
 *                         has read all the data requested.  This was done
 *                         b/c the higher level routines in the buffering
 *                         layer return failure if the number of bytes read
 *                         is not the number of bytes requested.  We ran into
 *                         the problem that occasionally the application was
 *                         trying to read the data faster than the TCP layer
 *                         could provide it, providing a "false failure".
 *       3/11/97   sgray - Changed all memory management calls that allocate
 *                         or deallocate strings to use the "Raw" calls.
 *
 ***************************************************************************/

#ifndef lint
static char vcid[] = "@(#) $Id: MP_TcpTransp.c,v 1.14 2008-07-17 10:26:55 Singular Exp $";
#endif /* lint */

#include "MP.h"
#include <string.h>
#include <stdlib.h>
#include <signal.h>


#ifdef __WIN32__

#     include <process.h>

#else /* not __WIN32__ */


#include <unistd.h>
#include <netdb.h>

#endif /* not __WIN32__ */



#define log_msg_len 128
#define MAX_ARGS     24

static char log_msg[log_msg_len];  /* for event logging */


static MP_Status_t tcp_negotiate_word_order _ANSI_ARGS_((MP_Link_pt));
static MP_Status_t tcp_negotiate_fp_format _ANSI_ARGS_((MP_Link_pt));
static MP_Status_t tcp_negotiate_bigint_format _ANSI_ARGS_((MP_Link_pt));
static MP_Status_t tcp_exchange_pids _ANSI_ARGS_((MP_Link_pt));
static int get_tcp_mode _ANSI_ARGS_((int, char**));

#ifdef __WIN32__
static int splitargs _ANSI_ARGS_((char*));
#endif


MP_TranspOps_t tcp_ops = {
    tcp_write,
    tcp_read,
    tcp_flush,
    tcp_get_status,
    tcp_open_connection,
    tcp_close_connection,
    tcp_kill_connection
};


/****  Private routines ******/

/***********************************************************************
 * FUNCTION:  get_tcp_mode
 * INPUT:     argc - number of arguments in argv
 *            argv - arguments as strings
 * OUTPUT:    Success:  one of MP_CONNECT_MODE, MP_LISTEN_MODE, MP_LAUNCH_MODE.
 *            Failure:  MP_NO_SUCH_TCP_MODE
 * OPERATION: Get the argument to the "-MPmode" option and see what it is.
 ***********************************************************************/
#ifdef __STDC__
static int get_tcp_mode(int argc, char **argv)
#else
static int get_tcp_mode(argc, argv)
    int    argc;
    char **argv;
#endif
{
    char *cmode;

#ifdef MP_DEBUG
    fprintf(stderr, "get_tcp_mode: entering\n");
    fflush(stderr);
#endif

    if ((cmode = IMP_GetCmdlineArg(argc, argv, "-MPmode")) != NULL)
        if (!strcmp(cmode, "connect"))
            return MP_CONNECT_MODE;
        else if (!strcmp(cmode, "listen"))
            return MP_LISTEN_MODE;
        else if (!strcmp(cmode, "launch"))
            return MP_LAUNCH_MODE;
        else if (!strcmp(cmode, "fork"))
          return MP_FORK_MODE;
    

#ifdef MP_DEBUG
    fprintf(stderr, "get_tcp_mode: exiting\n");
    fflush(stderr);
#endif

    return MP_NO_SUCH_TCP_MODE;
}


/***********************************************************************
 * FUNCTION:  open_tcp_launch_mode
 * INPUT:     link - pointer to link structure for this connection
 *            argc - number of arguments in argv
 *            argv - arguments as strings
 * OUTPUT:    Success:  MP_Success
 *                      socket connected to the specified host, port
 *            Failure:  MP_Failure
 * OPERATION: Launch an application remotely.  rsh is used for this,
 *            which isn't portable beyond Unix.  The application name
 *            must be supplied. First we open a port in listening mode,
 *            then the application is launched with arguments asking it
 *            to establish a connection to this host at the opened port.
 *            Immediately after the launch, we do a blocking accept(),
 *            waiting for the application to make the connection.
 *            Unfortunately there may be problems, so maybe a non-blocking
 *            accept would be better.
 *
 *            Windows95/NT: Launching applications is only supported on
 *            the local machine. This is done using the spawnv function.
 *            If the "-MPhost <host>" option is given on the command line
 *            <host> is first checked to see if it is identical to the
 *            local host.
 ***********************************************************************/

/* Returns a dupliucate of string `str' in which string `replace_this'
   is replaced by string `replace_by_that' provided all strings are !=
   NULL */
#ifdef __STDC__
static char* strdup_replace(const char* str,
                            const char* replace_this,
                            const char* replace_by_that)
#else
static char* strdup_replace(str, replace_this, replace_by_that)
  const char* str;
  const char* replace_this;
  const char* replace_by_that;
#endif
{
  char *sub, *ret;
  
  if (str == NULL) return NULL;
  
  if (replace_this != NULL && 
      (sub = strstr(str, replace_this)) != NULL &&
      replace_by_that != NULL)
  {
    ret = (char*) IMP_RawMemAllocFnc((strlen(str)  
                              + strlen(replace_by_that) - strlen(replace_this)
                              + 1)*sizeof(char));
    strcpy(ret, str);
    sprintf((void*) ret + ((void*) sub - (void*) str), 
            "%s%s", replace_by_that, &sub[strlen(replace_this)]);
  }
  else
  {
    ret = IMP_RawMemAllocFnc((strlen(str) + 1)*sizeof(char));
    strcpy(ret, str);
  }
  return ret;
}

#ifdef __STDC__
static int UsesLongOpt(int argc, char** argv, const char* opt)
#else
static int UsesLongOpt(argc, argv, opt)
  int argc; 
  char** argv; 
  const char* opt;
#endif
{
  int i=0;
  if (opt == NULL) return -1;
  
  while (i < argc && (argv[i][0] != '-' || strstr(argv[i], opt))) i++;
  
  if (i >= argc) return -1;
  if (argv[i][1] == '-') return 1;
  return 0;
}

#ifdef __STDC__
MP_Status_t open_tcp_launch_mode(MP_Link_pt   link,
                                 int          argc,
                                 char       **argv)
#else
MP_Status_t open_tcp_launch_mode(link, argc, argv)
    MP_Link_pt   link;
    int          argc;
    char       **argv;
#endif
{
#ifdef __WIN32__
    char       myhost[64], cport[10];
    char     **dmy_args, **spawn_argv, *rhost, *p, *mpapp;
    int        spawn_pid, i, applen, appopts;
    MP_TCP_t  *tcp_rec;
    HOSTENT   *myhostent, *rhostent;

#ifdef MP_DEBUG
    fprintf(stderr, "open_tcp_launch_mode: entering\n");
    fflush(stderr);
#endif /* MP_DEBUG */

    TEST_LINK_NULL(link);
    tcp_rec = (MP_TCP_t *)link->transp.private1;
    tcp_rec->peerhost = NULL;

    if (gethostname(myhost, 64) == SOCKET_ERROR)
        return MP_SetError(link, MP_Failure);

    /*
     * Let's assume that there is no rsh application available in Windows95/NT.
     * Therefore, all we can do is spawn a local process. So we must check if
     * the -MPhost argument is either equal to "localhost", myhost or not
     * specified at all. If not, we must bail out.
     */
    rhost = IMP_GetCmdlineArg(argc, argv, "-MPhost");
    if (rhost != NULL && strcmp(rhost, "localhost")) {

        /*
         * OK. So the user absolutely had to specify a host name. Then we have to
         * find out if myhost and the remote host are really the same machine.
         *
         * First, get host information for both host names.
         */
        myhostent = gethostbyname(myhost);
        rhostent  = gethostbyname(rhost);

        if (myhostent == NULL) {
            MP_LogEvent(link, MP_ERROR_EVENT,
                        "MP_OpenLink: cannot get local host information.");
            return MP_SetError(link, MP_Failure);
        }

        if (rhostent == NULL) {
            MP_LogEvent(link, MP_ERROR_EVENT,
                        "MP_OpenLink: cannot get remote host information.");
            return MP_SetError(link, MP_Failure);
        }

        /* Now compare both official host names and see if they are equal. */
        if (strcmp(myhostent->h_name, rhostent->h_name)) {
            MP_LogEvent(link, MP_ERROR_EVENT,
                        "MP_OpenLink: launching remote processes is not"
                        "supported (yet).");
            return MP_SetError(link, MP_Failure);
        }
    }

    /* Get the application to be launched. */
    p = IMP_GetCmdlineArg(argc, argv, "-MPapplication");
    if (p == NULL) {
        MP_LogEvent(link, MP_ERROR_EVENT,
                    "MP_OpenLink: bad or missing -MPapplication argument");
        return MP_SetError(link, MP_Failure);
    }

    /*
     * If we could execute an rsh application we could simply pass it the
     * -MPapplication argument string and let the rsh do the parsing. However,
     * as we need to use the spawnv function to launch the child process we must
     * split the application's argument string into the argv's ourselves:
     */

    /* Make a copy of the -MPapplication argument string. */
    applen = strlen(p) + 1;
    mpapp = (char*)IMP_RawMemAllocFnc(applen);
    if (mpapp == NULL) {
        MP_LogEvent(link, MP_ERROR_EVENT,
                    "MP_OpenLink: memory allocation error");
        return MP_SetError(link, MP_MemAlloc);
    }
    strcpy(mpapp, p);

    /* Split the argument string into several Null-terminated strings. */
    appopts = splitargs(mpapp);

    /* Allocate memory for the application's argv array. */
    spawn_argv = (char**)IMP_MemAllocFnc((appopts + 9) * sizeof(char*));
    if (spawn_argv == NULL) {
        MP_LogEvent(link, MP_ERROR_EVENT,
                    "MP_OpenLink: memory allocation error");
        return MP_SetError(link, MP_MemAlloc);
    }

    /* Fill the argv array with pointers to the arguments. */
    p = mpapp;
    for (i = 0; i < appopts; i++) {
        /* Take one argument and put it to the argv's. */
        spawn_argv[i] = p;
        /* Advance the pointer to the next argument. */
        while (*p++);
    }

    /* User shouldn't have specified a -MPport option, but we'll use it if
       it is there. */
    if (IMP_GetCmdlineArg(argc, argv, "-MPport") == NULL) {
        dmy_args = (char**)IMP_MemAllocFnc((argc + 2) * sizeof(char *));
        if (dmy_args == NULL) {
            MP_LogEvent(link, MP_ERROR_EVENT,
                        "MP_OpenLink: memory allocation error");
            return MP_SetError(link, MP_MemAlloc);
        }

        for (i = 0; i < argc; i++)
            dmy_args[i] = argv[i];
        dmy_args[i++] = "-MPport";
        dmy_args[i++] = MP_INIT_PORT;

        if (open_tcp_listen_mode(link, i, dmy_args) != MP_Success) {
            MP_LogEvent(link, MP_ERROR_EVENT,
                        "MP_OpenLink: can't open a listening socket");
            return MP_SetError(link, MP_CantConnect);
        }

        IMP_MemFreeFnc(dmy_args, (argc + 2) * sizeof(char*));
    }

    else  /* User specified port number, so we use original argc/argv pair. */
        if (open_tcp_listen_mode(link, argc, argv) != MP_Success) {
            MP_LogEvent(link, MP_ERROR_EVENT,
                        "MP_OpenLink: can't open a listening socket");
            return MP_SetError(link, MP_CantConnect);
        }

    sprintf(cport, "%hd", tcp_rec->peerport);

    /* Add the remaining MP options to the application's argument list. */
    spawn_argv[appopts  ] = "-MPtransp";
    spawn_argv[appopts+1] = "TCP";
    spawn_argv[appopts+2] = "-MPmode";
    spawn_argv[appopts+3] = "connect";
    spawn_argv[appopts+4] = "-MPhost";
    spawn_argv[appopts+5] = myhost;
    spawn_argv[appopts+6] = "-MPport";
    spawn_argv[appopts+7] = cport;
    spawn_argv[appopts+8] = NULL;

    /* Launch the application. */
    spawn_pid = spawnv(P_NOWAIT, spawn_argv[0], spawn_argv);
    if (spawn_pid == -1) {
        MP_LogEvent(link, MP_ERROR_EVENT,
                    "MP_OpenLink: can't launch application");
        return MP_SetError(link, MP_Failure);
    }

    /* Free the application argument memory. */
    IMP_MemFreeFnc(spawn_argv, (appopts + 9) * sizeof(char*));
    IMP_RawMemFreeFnc(mpapp);

    /* Wait for the child application to connect. */
    socket_accept_blocking(link, &tcp_rec->sock);

#ifdef MP_DEBUG
    fprintf(stderr, "open_tcp_launch_mode: exiting\n");
    fflush(stderr);
#endif

    RETURN_OK(link);

#else /*  not __WIN32__ */

    char *rsh_argv[5], myhost[64], cport[5], **dmy_args, *appstr, *sub;
    int       rsh_pid = -1, i;
    MP_TCP_t *tcp_rec;
    MP_Boolean_t localhost = MP_FALSE;

#if defined(HAVE_GETHOSTBYNAME) && defined(RSH_CAN_LOCALHOST)
    if (gethostbyname("localhost") != NULL) localhost = MP_TRUE;
#endif
    
    
#ifdef MP_DEBUG
    fprintf(stderr, "open_tcp_launch_mode: entering\n");
    fflush(stderr);
#endif

    TEST_LINK_NULL(link);
    tcp_rec = (MP_TCP_t *)link->transp.private1;
    tcp_rec->peerhost = NULL;

    if (gethostname(myhost, 64) == -1)
        return MP_SetError(link, MP_Failure);
    
    /* NOTE: Ultrix doesn't like this order. It expects rsh host [-n]
       command. */
    /* I tried on every platform I know using all the rsh commands
       available, and

       $rsh hostname -n command

       worked everywhere */

    rsh_argv[0] = IMP_GetCmdlineArg(argc, argv, "-MPrsh");
    if (rsh_argv[0] == NULL)
      rsh_argv[0] = MP_RSH_COMMAND;
    tcp_rec->rsh = IMP_StrDup(rsh_argv[0]);

    rsh_argv[1] = IMP_GetCmdlineArg(argc, argv, "-MPhost");
    /* Let's not be too strict, and allow an empty -MPhost argument */
    if (localhost)
    {
      if ((rsh_argv[1] != NULL && (strcmp(rsh_argv[1], myhost) == 0)) ||
          rsh_argv[1] == NULL) 
        rsh_argv[1] = "localhost";
    }
    else
    {
      rsh_argv[1] = myhost;
    }
    rsh_argv[2] = "-n";
    rsh_argv[3] = IMP_GetCmdlineArg(argc, argv, "-MPapplication");
    if (rsh_argv[3] == NULL) {
        MP_LogEvent(link, MP_ERROR_EVENT,
                    "MP_OpenLink: bad or missing -MPapplication argument");
        return MP_SetError(link, MP_Failure);
    }

    /*
     * User shouldn't have specified a -MPport option, but we'll use it if
     * it is there.
     */

    if (IMP_GetCmdlineArg(argc, argv, "-MPport") == NULL) {
        dmy_args = (char**)IMP_MemAllocFnc((argc + 2) * sizeof(char*));
        if (dmy_args == NULL) {
            MP_LogEvent(link, MP_ERROR_EVENT,
                        "MP_OpenLink: memory allocation error");
            return MP_SetError(link, MP_MemAlloc);
        }

        for (i = 0; i < argc; i++)
            dmy_args[i] = argv[i];
        dmy_args[i++] = "-MPport";
        dmy_args[i++] = MP_INIT_PORT;

        if (open_tcp_listen_mode(link, i, dmy_args) != MP_Success) {
            MP_LogEvent(link, MP_ERROR_EVENT,
                        "MP_OpenLink: can't open a listening socket");
            return MP_SetError(link, MP_CantConnect);
        }

        IMP_MemFreeFnc(dmy_args, (argc + 2) * sizeof(char *));
    }

    else
        /* User specified port number, so we use original argc/argv pair. */
        if (open_tcp_listen_mode(link, argc, argv) != MP_Success) {
            MP_LogEvent(link, MP_ERROR_EVENT,
                        "MP_OpenLink: can't open a listening socket");
            return MP_SetError(link, MP_CantConnect);
        }

    sprintf(cport, "%hd", tcp_rec->peerport);
    if (localhost  &&
        (strcmp(rsh_argv[1], myhost) == 0 ||
         strcmp(rsh_argv[1], "localhost") == 0))
      sprintf(myhost, "localhost");
    
    if (strstr(rsh_argv[3], "$MPport") != NULL ||
        strstr(rsh_argv[3], "$MPhost") != NULL)
    {
      sub = strdup_replace(rsh_argv[3], "$MPport", cport);
      appstr = strdup_replace(sub, "$MPhost", myhost);
      IMP_RawMemFreeFnc(sub);
    }
    else
    {
      appstr = (char*)IMP_RawMemAllocFnc(strlen(rsh_argv[3]) + 110);
      strcpy(appstr, rsh_argv[3]);
      
      if (UsesLongOpt(argc, argv, "MPapplication") > 0)
      {
        strcat(appstr, " --MPtransp TCP --MPmode connect --MPhost ");
        strcat(appstr, myhost);
        strcat(appstr, " --MPport ");
        strcat(appstr, cport);
      }
      else
      {
        strcat(appstr, " -MPtransp TCP -MPmode connect -MPhost ");
        strcat(appstr, myhost);
        strcat(appstr, " -MPport ");
        strcat(appstr, cport);
      }
      
    }

    rsh_argv[3] = appstr;
    rsh_argv[4] = NULL;
    if (tcp_rec->peerhost == NULL) 
      tcp_rec->peerhost = IMP_StrDup(rsh_argv[1]);

    if ((rsh_pid = vfork()) == -1) {
        MP_LogEvent(link, MP_ERROR_EVENT,
                    "MP_OpenLink: can't fork to launch application");
        return MP_SetError(link, MP_Failure);
    }

    if (!rsh_pid) {
        execvp(rsh_argv[0], rsh_argv);
        fputs("open_tcp_launch_mode: execvp failed - bailing out\n", stderr);
        fflush(stderr);
        _exit(1);
    }

    socket_accept_blocking(link, &tcp_rec->sock);

    IMP_RawMemFreeFnc(appstr);

#ifdef MP_DEBUG
    fprintf(stderr, "open_tcp_launch_mode: exiting\n");
    fflush(stderr);
#endif

    RETURN_OK(link);
#endif /* not __WIN32__ */
}



/***********************************************************************
 * FUNCTION:  open_tcp_listen_mode
 * INPUT:     link - pointer to link structure for this connection
 *            argc - number of arguments in argv
 *            argv - arguments as strings
 * OUTPUT:    Success:  MP_Success
 *                      socket connected to the specified host, port
 *            Failure:  MP_Failure
 * OPERATION: Open a socket in the AF_INET domain, verify that the port
 *            is in the legal range (don't annoy anyone), bind the socket,
 *            finally, do a listen.  We update the link structure with
 *            the socket and port information.
 ***********************************************************************/
#ifdef __STDC__
MP_Status_t open_tcp_listen_mode(MP_Link_pt   link,
                                 int          argc,
                                 char       **argv)
#else
MP_Status_t open_tcp_listen_mode(link, argc, argv)
    MP_Link_pt   link;
    int          argc;
    char       **argv;
#endif
{
    SOCKADDR_IN  isin;
    SOCKET       sock;
    int          attempts = 0;
    char        *cport;
    short        port;
    MP_TCP_t    *tcp_rec = (MP_TCP_t *)link->transp.private1;

#ifdef MP_DEBUG
    fprintf(stderr, "open_tcp_listen_mode: entering\n");
    fflush(stderr);
#endif /* MP_DEBUG */

    tcp_rec->peerhost = NULL;

    if ((cport = IMP_GetCmdlineArg(argc, argv, "-MPport")) == NULL)
        return MP_SetError(link, MP_Failure);

    port = atoi(cport);
    if (port < IPPORT_RESERVED)
        port = IPPORT_RESERVED;

    sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock == INVALID_SOCKET) {
        sprintf(log_msg,
                "open_tcp_listen_mode: "
                "Cannot open stream socket (socket error %d)",
                LASTERROR);
        MP_LogEvent(link, MP_ERROR_EVENT, log_msg);
        return MP_SetError(link, MP_Failure);
    }

    /* Initialize socket's address structure */
    isin.sin_family      = AF_INET;
    isin.sin_addr.s_addr = htonl(INADDR_ANY);
    isin.sin_port        = htons(port);

    while (bind(sock, (SOCKADDR*)&isin, sizeof(isin)) == SOCKET_ERROR) {
        if (LASTERROR == ERRORCODE(EADDRINUSE)) {
            port++;

            /* since port is an unsigned short, it may wrap around */
            if (port < IPPORT_RESERVED)
                port = IPPORT_RESERVED;

            isin.sin_port = htons(port);
            attempts = 0;
        }

        if (attempts++ > MP_MAX_BIND_ATTEMPTS) {
            MP_LogEvent(link, MP_ERROR_EVENT,
                        "open_tcp_listen_mode: Cannot bind");
            return MP_SetError(link, MP_Failure);
        }
    }

    /* Now listen for the anticipated connect request */
    listen(sock, 1);
    tcp_rec->bindsock = sock;
    tcp_rec->sock     = sock;
    tcp_rec->peerport = port;

#ifndef NO_LOGGING
    if (link->logmask & MP_LOG_INIT_EVENTS) {
        sprintf(log_msg,
                "open_tcp_listen_mode: socket -> %d, port -> %d",
                sock, port);
        MP_LogEvent(link, MP_INIT_EVENT, log_msg);
    }
#endif /* NO_LOGGING */

#ifdef MP_DEBUG
    fprintf(stderr, "\t  tcp_rec->sock = %d\n", tcp_rec->sock);
    fprintf(stderr, "\t  tcp_rec->port = %d\n", tcp_rec->peerport);
    fprintf(stderr,
            "open_tcp_listen_mode: exiting - socket = %d, port = %d\n",
            sock, port);
    fflush(stderr);
#endif

    RETURN_OK(link);
}



/***********************************************************************
 * FUNCTION:  open_tcp_fork_mode
 * INPUT:     link - pointer to link structure for this connection
 * OUTPUT:    Success:  MP_Success
 *                      socket connected to the a forked child
 *            Failure:  MP_Failure - couldn't make the connection.
 * OPERATION: forks a child, establishes a connection to it and uses
 *            the return of the fork to set parent/child attribute
 ***********************************************************************/
#ifdef __STDC__
MP_Status_t open_tcp_fork_mode(MP_Link_pt link, int argc, char** argv)
#else
MP_Status_t open_tcp_fork_mode(link, argc, argv)
    MP_Link_pt   link;
    int          argc;
    char**       argv;
#endif
{
  int pid;
  MP_TCP_t *tcp_rec = (MP_TCP_t *) link->transp.private1;
#ifndef NO_LOGGING
  char log_msg[200];
  char *parent_logfilename = link->env->logfilename;
  FILE *parent_logfd = link->env->logfd;
#endif
  
  /* prepare the socket */
  if (open_tcp_listen_mode(link, argc, argv) != MP_Success)
  {
    MP_LogEvent(link, MP_ERROR_EVENT,
                "MP_OpenLink: can't open a listening socket");
    return MP_SetError(link, MP_CantConnect);
  }

#ifndef NO_LOGGING
  /* Open the log file before the fork, so that the processes do not
     get out of sync */
  sprintf(log_msg,
          "open_tcp_fork: Preparing to create fork link, parent id: %d",
          getpid());
  if (open_logfile(link->env) != MP_Success) return MP_Failure;
  MP_LogEvent(link, MP_INIT_EVENT,
              "open_tcp_fork: MP Log file for a forked (child) link");
  sprintf(log_msg, "open_tcp_fork: Parent link: L%d, logfile: %s, pid: %d",
          link->link_id, parent_logfilename, getpid());
  MP_LogEvent(link, MP_INIT_EVENT, log_msg);
#endif
  
  /* Do the fork */
  if ((pid = fork()) == -1)
  {
#ifndef NO_LOGGING
    link->env->logfilename = parent_logfilename;
    link->env->logfd = parent_logfd;
#endif    
    MP_LogEvent(link, MP_ERROR_EVENT, "MP_OpenLink: can't fork");
    return MP_SetError(link, MP_Failure);
  }

  if (!pid)
  {
    /* Child's business */
    MP_Status_t stat;
    char cport[12];
    char *argv[] = {"MPtransp","TCP","-MPmode","connect","-MPhost",
                    "localhost", "-MPport", ""};
    sprintf(cport,"%d",tcp_rec->peerport);
    argv[7] = cport;
#ifndef NO_LOGGING
    sprintf(log_msg, "open_tcp_fork:  child starti (pid=%d)",getpid());
#endif
    /* establish connection */
    sleep(1); /* wait for the parent to listen */
    stat = open_tcp_connect_mode(link, 8, argv);

#ifndef NO_LOGGING
    IMP_RawMemFreeFnc(parent_logfilename);
    if (stat == MP_Success)
      sprintf(log_msg, "open_tcp_fork: opened forked link, child id: %d",
              getpid());
    else
      sprintf(log_msg,
              "open_tcp_fork: open of forked link failed, child is exited");
    MP_LogEvent(link, MP_ERROR_EVENT, log_msg);
#endif
    if (stat != MP_Success)
      _exit(1);
    else
    {
      tcp_rec->status = MP_LinkIsChild;
      return MP_Success;
    }
  }
  else
  {
    /* parent's business */
#ifndef NO_LOGGING
    /* reset logging stuff */
    link->env->logfilename = parent_logfilename;
    link->env->logfd = parent_logfd;
#endif

    /* establish connection */
    socket_accept_blocking(link, &tcp_rec->sock);
    tcp_rec->status = MP_LinkIsParent;

#ifndef NO_LOGGING
    sprintf(log_msg,"open_tcp_fork: opened fork link, parent id: %d",
            getpid()); 
    MP_LogEvent(link, MP_INIT_EVENT, log_msg);
#endif

    RETURN_OK(link);
  }
}
  
/***********************************************************************
 * FUNCTION:  open_tcp_connect_mode
 * INPUT:     link - pointer to link structure for this connection
 *            argc - number of arguments in argv
 *            argv - arguments as strings
 * OUTPUT:    Success:  MP_Success
 *                      socket connected to the specified host, port
 *            Failure:  MP_Failure - couldn't make the connection.
 *                      Maybe the peer isn't listening.
 * OPERATION: Establish a TCP connection to the peer's port. Update the
 *            private tcp structure accordingly.  We hope that there
 *            is a peer actually listening on the given port.  We make
 *            1000 attempts before giving up.
 ***********************************************************************/
#ifdef __STDC__
MP_Status_t open_tcp_connect_mode(MP_Link_pt   link,
                                  int          argc,
                                  char       **argv)
#else
MP_Status_t open_tcp_connect_mode(link, argc, argv)
    MP_Link_pt   link;
    int          argc;
    char       **argv;
#endif
{
    SOCKET       sock;
    int          attempts = 0;
    HOSTENT     *hp;
    SOCKADDR_IN  isin;
    char        *host, *cport, *ptr;
    short        port;
    MP_TCP_t    *tcp_rec = (MP_TCP_t *)link->transp.private1;

#ifdef MP_DEBUG
    fprintf(stderr, "open_tcp_connect_mode: entering\n");
    fflush(stderr);
#endif /* MP_DEBUG */

    tcp_rec->peerhost = NULL;

    /* get the port number */
    if (
        (cport = IMP_GetCmdlineArg(argc, argv, "-MPport")) == NULL)
        return MP_SetError(link, MP_Failure);

    port = (short) strtol(cport, &ptr, 10);
    if ((host = IMP_GetCmdlineArg(argc, argv, "-MPhost")) == NULL)
      host = link->env->thishost;
    
    if (ptr == cport || errno == ERANGE)
      return MP_SetError(link, MP_Failure);

    if ((hp = gethostbyname(host)) == NULL) {
        MP_LogEvent(link, MP_ERROR_EVENT,
                    "open_tcp_connect_mode - cannot get hostname");
        return MP_SetError(link, MP_Failure);
    }

    sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock == INVALID_SOCKET) {
        sprintf(log_msg,
                "open_tcp_connect_mode - Cannot open socket (socket error %d)",
                LASTERROR);
        MP_LogEvent(link, MP_ERROR_EVENT, log_msg);
        return MP_SetError(link, MP_Failure);
    }

    /* Initialize the socket address to the server's address. */
    memset((char *)&isin, 0, sizeof(isin));
    isin.sin_family = AF_INET;
    memcpy(&isin.sin_addr.s_addr, hp->h_addr, hp->h_length);
    isin.sin_port = htons(port);

    /* Connect to the server. */
    while (connect(sock, (SOCKADDR*)&isin, sizeof(isin)) == SOCKET_ERROR) {
        CLOSE_SOCKET(sock);
        if ((sock = socket(AF_INET, SOCK_STREAM, 0)) == INVALID_SOCKET) {
            sprintf(log_msg,
                    "open_tcp_connect_mode "
                    "- Cannot open socket (socket error %d)", LASTERROR);
            MP_LogEvent(link, MP_ERROR_EVENT, log_msg);
            return MP_SetError(link, MP_Failure);
        }

        if (attempts > MP_MAX_BIND_ATTEMPTS) {
            MP_LogEvent(link, MP_ERROR_EVENT,
                        "open_tcp_connect_mode - Cannot connect");
            return MP_SetError(link, MP_Failure);
        }

        attempts++;
    }

    if (setsockopt(sock, IPPROTO_TCP, TCP_NODELAY, (char*)&sock, sizeof(sock))
        == SOCKET_ERROR) {
        sprintf(log_msg,
                "open_tcp_connect_mode "
                "- cannot set socket option (socket error %d)", LASTERROR);
        MP_LogEvent(link, MP_ERROR_EVENT, log_msg);
        return MP_SetError(link, MP_Failure);
    }

    tcp_rec->bindsock = INVALID_SOCKET;
    tcp_rec->sock     = sock;
    tcp_rec->peerport = port;

    tcp_rec->peerhost = (char*)IMP_RawMemAllocFnc(strlen(host) +1);
    TEST_MEM_ALLOC_ERROR(link, tcp_rec->peerhost);
    strcpy(tcp_rec->peerhost, host);

    tcp_rec->myhost = (char*)IMP_RawMemAllocFnc(MP_HOST_NAME_LEN);
    TEST_MEM_ALLOC_ERROR(link, tcp_rec->myhost);
    if (gethostname(tcp_rec->myhost, MP_HOST_NAME_LEN) == SOCKET_ERROR) {
        IMP_RawMemFreeFnc(tcp_rec->peerhost);
        IMP_RawMemFreeFnc(tcp_rec->myhost);
        tcp_rec->myhost = NULL;
    }

#ifdef MP_DEBUG
    fprintf(stderr, "open_tcp_connect_mode: exiting - socket is %d\n", sock);
    fflush(stderr);
#endif

    RETURN_OK(link);
}



/***********************************************************************
 * FUNCTION:  socket_accept_non_blocking
 * INPUT:     sock   - the socket to do the accept on
 * OUTPUT:    Success: the socket to be used for io
 *                     Failure: MP_Failure
 *                     sock contains the socket number of the accepted
 *                     socket.
 * PURPOSE:   Perform a non-blocking accept.  If we timeout before getting
 *            a connection request, return MP_Failure.
 ***********************************************************************/
#ifdef __STDC__
MP_Status_t socket_accept_non_blocking(MP_Link_pt link, SOCKET *sock)
#else
MP_Status_t socket_accept_non_blocking(link, sock)
    MP_Link_pt  link;
    SOCKET     *sock;
#endif
{
    SOCKADDR addr;
    SOCKET   nsock = 0;
    int      attempts, addrlen = sizeof(addr);
    fd_set   read_template;
    TIMEVAL  timeout;

#ifdef MP_DEBUG
    fprintf(stderr,
            "socket_accept_non_blocking: entering - socket = %d\n", *sock);
    fflush(stderr);
#endif

    timeout.tv_sec  = MP_ACCEPT_WAIT_SEC;
    timeout.tv_usec = MP_ACCEPT_WAIT_USEC;

    for (attempts = 0; attempts < 2; attempts++) {
        FD_ZERO(&read_template);
        FD_SET(*sock, &read_template);

        if (SELECT(FD_SETSIZE, &read_template, NULL, NULL, &timeout)
            != SOCKET_ERROR) {

            if ((nsock = accept(*sock, &addr, &addrlen)) != INVALID_SOCKET)
                break;

            sprintf(log_msg,
                    "socket_accept_non_blocking: failed (socket error %d)",
                    LASTERROR);
            MP_LogEvent(link, MP_ERROR_EVENT, log_msg);
            return MP_Failure;
        }
    }

    *sock = nsock;

#ifdef MP_DEBUG
    fprintf(stderr,
            "socket_accept_non_blocking: returning - new socket =  %d\n",
            *sock);
    fflush(stderr);
#endif

    return MP_Success;
}



/***********************************************************************
 * FUNCTION:  socket_accept_blocking
 * INPUT:     link - link with which sock will be associated, need this
 *                   really only for the error handling.
 *            sock - the socket to do the accept on
 * OUTPUT:    int - Success: the socket to be used for io
 *                  Failure: MP_Failure
 * PURPOSE:   Perform a blocking accept.  We won't return until we get an
 *            connect request or are interrupted.
 ***********************************************************************/
#ifdef __STDC__
MP_Status_t socket_accept_blocking(MP_Link_pt link, SOCKET *sock)
#else
MP_Status_t socket_accept_blocking (link, sock)
    MP_Link_pt  link;
    SOCKET     *sock;
#endif
{
    SOCKADDR addr;
    SOCKET   nsock = 0;
    int      attempts, addrlen = sizeof(addr);
    fd_set   read_template;

#ifdef MP_DEBUG
    fprintf(stderr, "socket_accept_blocking: entering - sock = %d\n", *sock);
    fflush(stderr);
#endif

    for (attempts = 0; attempts < 2; attempts++) {
        FD_ZERO(&read_template);
        FD_SET(*sock, &read_template);

        if (SELECT(FD_SETSIZE, &read_template, NULL, NULL, NULL)
            != SOCKET_ERROR) {
            if ((nsock = accept(*sock, &addr, &addrlen)) != INVALID_SOCKET)
                break;

            sprintf(log_msg,
                    "socket_accept_blocking: failed (socket error %d)",
                    LASTERROR);
            MP_LogEvent(link, MP_ERROR_EVENT, log_msg);
            return MP_Failure;
        }
    }

    *sock = nsock;

#ifdef MP_DEBUG
    fprintf(stderr, "socket_accept_blocking: returning - new socket =  %d\n", *sock);
    fflush(stderr);
#endif

    return MP_Success;
}





/****  TCP "public" routines ******/

#ifdef __STDC__
MP_Boolean_t tcp_get_status(MP_Link_pt link, MP_LinkStatus_t status_to_check)
#else
MP_Boolean_t tcp_get_status(link, status_to_check)
    MP_Link_pt       link;
    MP_LinkStatus_t  status_to_check;
#endif
{
    SOCKET  sock = ((MP_TCP_t *)link->transp.private1)->sock;
    fd_set  mask, fdmask;
    TIMEVAL wt;

#ifdef MP_DEBUG
    printf("tcp_get_status: entering for link %d\n", link->link_id);
#endif
  if (status_to_check == MP_LinkIsParent)
  {
    if (((MP_TCP_t *)link->transp.private1)->status == MP_LinkIsParent)
      return MP_TRUE;
    else
      return MP_FALSE;
  }
  else if (status_to_check == MP_LinkIsChild)
  {
    if (((MP_TCP_t *)link->transp.private1)->status == MP_LinkIsChild)
      return MP_TRUE;
    else
      return MP_FALSE;
  }
  

    FD_ZERO(&mask);
    FD_SET(sock, &mask);

    fdmask = mask;

    /* Don't block. Return socket status immediately. */
    wt.tv_sec  = 0;
    wt.tv_usec = 0;

    switch (status_to_check) {
    case MP_LinkReadyReading:
        switch (SELECT(FD_SETSIZE, &fdmask, NULL, NULL, &wt)) {
        case 0:
            return MP_FALSE;
        case SOCKET_ERROR:
            return MP_FALSE;
        }
        if (FD_ISSET(sock, &fdmask))
            return MP_TRUE;

        return MP_FALSE;

    case MP_LinkReadyWriting:
        switch (SELECT(FD_SETSIZE, NULL, &fdmask, NULL, &wt)) {
        case 0:
            return MP_FALSE;
        case SOCKET_ERROR:
            return MP_FALSE;
        }
        if (FD_ISSET(sock, &fdmask))
            return MP_TRUE;

        return MP_FALSE;

    default:
        sprintf(log_msg,
                "tcp_get_status: illegal option %d", status_to_check);
        MP_LogEvent(link, MP_ERROR_EVENT, log_msg);
    }

#ifdef MP_DEBUG
    printf("tcp_get_status: exiting\n");
#endif

    return MP_FALSE;
}



#ifdef __STDC__
long tcp_read(MP_Link_pt link, char * buf, long len)
#else
long tcp_read(link, buf, len)
    MP_Link_pt   link;
    char *      buf;
    long         len;
#endif
{
    SOCKET   sock = ((MP_TCP_t *)link->transp.private1)->sock;
    fd_set   mask, readfs;
    long     total = 0, n;

#ifdef MP_DEBUG
    fprintf(stderr, "tcp_read: entering - len = %ld\n", len);
    fflush(stderr);
#endif

    while (len > 0) {
        FD_ZERO(&mask);
        FD_SET(sock, &mask);

        while (MP_TRUE) {
            readfs = mask;

            /* Do blocking read. */
            switch (SELECT(FD_SETSIZE, &readfs, NULL, NULL, NULL)) {
            case 0:
                MP_LogEvent(link, MP_ERROR_EVENT, "tcp_read: timed out");
                return -1;

            case SOCKET_ERROR:
                if (LASTERROR == ERRORCODE(EINTR))
                    continue;
                return -1;
            }

            if (FD_ISSET(sock, &readfs))
                break;
        }

        if ((n = READ_SOCKET(sock, buf, len)) == SOCKET_ERROR) {
            sprintf(log_msg, "tcp_read: can't do read (socket error %d)",
                    LASTERROR);
            MP_LogEvent(link, MP_ERROR_EVENT, log_msg);
            return MP_SetError(link, MP_CantReadLink);
        }

        total += n;
        len   -= n;
        buf   += n;
    }

#ifndef NO_LOGGING
    if (link->logmask & MP_LOG_READ_EVENTS) {
        sprintf(log_msg, "tcp_read: read %ld bytes", total);
        MP_LogEvent(link, MP_READ_EVENT, log_msg);
    }
#endif /* NO_LOGGING */

#ifdef MP_DEBUG
    fprintf(stderr, "tcp_read: exiting - read %ld bytes \n", total);
    fflush(stderr);
#endif /* MP_DEBUG */

    return total;
}


MP_Status_t 
#ifdef __STDC__
tcp_flush(MP_Link_pt link)
#else
tcp_flush(link)
    MP_Link_pt  link;
#endif
{

#ifdef MP_DEBUG
    fprintf(stderr, "tcp_flush: entering\n");
    fflush(stderr);
#endif
#ifdef MP_DEBUG
    fprintf(stderr, "tcp_flush: exiting  \n");
    fflush(stderr);
#endif
  return MP_ClearError(link);
}


#ifdef __STDC__
long tcp_write(MP_Link_pt link,
               char *    buf,
               long       len)
#else
long tcp_write(link, buf, len)
    MP_Link_pt  link;
    char *     buf;
    long        len;
#endif
{
    long   i, cnt;
    SOCKET sock = ((MP_TCP_t *)link->transp.private1)->sock;
    fd_set mask, writefs;

#ifdef MP_DEBUG
    fprintf(stderr, "tcp_write: entering - len = %ld\n", len);
    fflush(stderr);
#endif

    FD_ZERO(&mask);
    FD_SET(sock, &mask);

    while (MP_TRUE) {
        writefs = mask;
        switch (SELECT(FD_SETSIZE, NULL, &writefs, NULL, NULL)) {
        case 0:
            MP_LogEvent(link, MP_ERROR_EVENT, "tcp_write: timed out");
            return -1;

        case SOCKET_ERROR:
            if (LASTERROR == ERRORCODE(EINTR))
                continue;
            MP_LogEvent(link, MP_ERROR_EVENT,
                        "tcp_write: errno isn't EINTR - this is bad");
            return -1;
        }

        if (FD_ISSET(sock, &writefs))
            break;
    }


    for (cnt = len; cnt > 0; cnt -= i, buf += i) {
        if ((i = WRITE_SOCKET(sock, buf, cnt)) == SOCKET_ERROR) {
            sprintf(log_msg, "tcp_write: can't do write (socket error %d)",
                    LASTERROR);
            MP_LogEvent(link, MP_ERROR_EVENT, log_msg);
            return MP_SetError(link, MP_CantWriteLink);
        }

#ifndef NO_LOGGING
        if (link->logmask & MP_LOG_WRITE_EVENTS) {
            sprintf(log_msg, "tcp_write: wrote %ld bytes", i);
            MP_LogEvent(link, MP_WRITE_EVENT, log_msg);
        }
#endif /* NO_LOGGING */
    }

#ifdef MP_DEBUG
    fprintf(stderr, "tcp_write: exiting - wrote %ld bytes \n", i);
    fflush(stderr);
#endif

    return len;
}



/***********************************************************************
 * FUNCTION:  tcp_init_transport
 * INPUT:     link - pointer to the link structure for this connection
 * OUTPUT:    Success:  MP_Success and link structure initialized for the
 *                      tcp connection.
 *            Failure:  MP_Failure
 * OPERATION: If there are no problems, allocate a tcp structure and
 *            attach it to the private pointer inside link.
 ***********************************************************************/
#ifdef __STDC__
MP_Status_t tcp_init_transport(MP_Link_pt link)
#else
MP_Status_t tcp_init_transport(link)
    MP_Link_pt link;
#endif
{
    MP_TCP_t *tcp_rec;

#ifdef MP_DEBUG
    fprintf(stderr, "tcp_init_transport: entering\n"); fflush(stderr);
#endif

    TEST_LINK_NULL(link);
    link->transp.transp_dev =  MP_TcpTransportDev;
    link->transp.transp_ops = &tcp_ops;

    tcp_rec = (MP_TCP_t*)IMP_MemAllocFnc(sizeof(MP_TCP_t));
    TEST_MEM_ALLOC_ERROR(link, tcp_rec);

    /* Initialize TCP record. */
    tcp_rec->bindsock = INVALID_SOCKET;
    tcp_rec->sock     = INVALID_SOCKET;
    tcp_rec->peerport = 0;
    tcp_rec->myhost   = NULL;
    tcp_rec->peerhost = NULL;
    tcp_rec->status   = MP_UnknownStatus;
    tcp_rec->rsh      = NULL;

    link->transp.private1 = (char *)tcp_rec;

#ifdef MP_DEBUG
    fprintf(stderr, "tcp_init_transport: exiting\n");
    fflush(stderr);
#endif

    RETURN_OK(link);
}

/***********************************************************************
 * FUNCTION:  tcp_close_connection
 * INPUT:     link  - pointer to the link structure for this connection
 * OUTPUT:    Success:  MP_Success
 *                      Release the tcp structure pointed to by private1.
 *            Failure:  MP_Failure
 * OPERATION:
 ***********************************************************************/
#ifdef __STDC__
MP_Status_t tcp_close_connection(MP_Link_pt link)
#else
MP_Status_t tcp_close_connection(link)
    MP_Link_pt link;
#endif
{
    MP_TCP_t *tcp_rec;

#ifdef MP_DEBUG
    fprintf(stderr, "tcp_close_connection: entering\n");
    fflush(stderr);
#endif /* MP_DEBUG */

    TEST_LINK_NULL(link);
    if (link->transp.private1 == NULL)
        return MP_SetError(link, MP_NullTransport);
    tcp_rec = (MP_TCP_t*)link->transp.private1;

    CLOSE_SOCKET(tcp_rec->sock);
    if (tcp_rec->bindsock != INVALID_SOCKET)
        CLOSE_SOCKET(tcp_rec->bindsock);

    if (tcp_rec->myhost != NULL)
        IMP_RawMemFreeFnc(tcp_rec->myhost);
    if (tcp_rec->peerhost != NULL)
        IMP_RawMemFreeFnc(tcp_rec->peerhost);
    if (tcp_rec->rsh != NULL)
        IMP_RawMemFreeFnc(tcp_rec->rsh);
    IMP_MemFreeFnc(tcp_rec, sizeof(MP_TCP_t));
    link->transp.private1 = NULL;

#ifdef MP_DEBUG
    fprintf(stderr, "tcp_close_connection: exiting\n");
    fflush(stderr);
#endif /* MP_DEBUG */

    RETURN_OK(link);
}

#ifdef __STDC__
MP_Status_t tcp_kill_connection(MP_Link_pt link)
#else
MP_Status_t tcp_kill_connection(link)
    MP_Link_pt link;
#endif
{
    MP_TCP_t *tcp_rec;
    int fork_pid = -1;
    
#ifdef MP_DEBUG
    fprintf(stderr, "tcp_kill_connection: entering\n");
    fflush(stderr);
#endif /* MP_DEBUG */

    TEST_LINK_NULL(link);
    if (link->transp.private1 == NULL)
        return MP_SetError(link, MP_NullTransport);
    tcp_rec = (MP_TCP_t*)link->transp.private1;
    
    if (tcp_rec->mode == MP_LAUNCH_MODE)
    {
      char  *rsh_argv[5];
      char  rsh_kill[20];
      rsh_argv[0] = tcp_rec->rsh;
      rsh_argv[1] = tcp_rec->peerhost;
      rsh_argv[2] = "-n";
      sprintf(rsh_kill, "kill -9 %d", tcp_rec->peerpid);
      rsh_argv[3] = rsh_kill;
      rsh_argv[4] = NULL;

      if ((fork_pid = vfork()) == -1)
      {
        MP_LogEvent(link, MP_ERROR_EVENT,
                    "MP_OpenLink: can't fork to kill application");
        return MP_SetError(link, MP_Failure);
      }

      if (! fork_pid)
      {
        execvp(rsh_argv[0], rsh_argv);
        fputs("tcp_kill_connection: execvp failed - bailing out\n", stderr);
        fflush(stderr);
        _exit(1);
      }
    }
    else if (tcp_rec->mode == MP_FORK_MODE)
    {
      kill(tcp_rec->peerpid, SIGKILL);
    }
    
    return tcp_close_connection(link);
}

/***********************************************************************
 * FUNCTION:  tcp_open_connection
 * INPUT:     link - pointer to link structure for this connection
 *            argc - number of arguments in argv
 *            argv - arguments as strings
 * OUTPUT:    Success:  MP_Success
 *                      link established as a tcp connection
 *            Failure:  MP_Failure
 * OPERATION: Determine the tcp mode we are using and attempt to
 *            establish a connection accordingly.  We could fail
 *            for any of a number of reasons.  Bad hostname, bad
 *            port number, missing arguments....
 ***********************************************************************/
#ifdef __STDC__
MP_Status_t tcp_open_connection(MP_Link_pt   link,
                                int          argc,
                                char       **argv)
#else
MP_Status_t tcp_open_connection(link, argc, argv)
    MP_Link_pt   link;
    int          argc;
    char       **argv;
#endif
{
    MP_Status_t status;
    struct protoent *protostruct = NULL;
    int sockopt;  

#ifdef MP_DEBUG
    fprintf(stderr, "tcp_open_connection: entering\n");
    fflush(stderr);
#endif

    TEST_LINK_NULL(link);
    if (tcp_init_transport(link) != MP_Success)
        return MP_Failure;

    ((MP_TCP_t *)link->transp.private1)->mode = get_tcp_mode(argc, argv);
    switch (((MP_TCP_t *)link->transp.private1)->mode)
    {
    case MP_LISTEN_MODE:
        status = open_tcp_listen_mode(link, argc, argv);
        if (status != MP_Success)
            break;
        status = socket_accept_blocking(link,
                        &((MP_TCP_t *)link->transp.private1)->sock);
        break;

    case MP_CONNECT_MODE:
        status = open_tcp_connect_mode(link, argc, argv);
        break;

    case MP_LAUNCH_MODE:
        status = open_tcp_launch_mode(link, argc, argv);
        break;

        case MP_FORK_MODE:
          status = open_tcp_fork_mode(link, argc, argv);
          break;
          
    default:
        MP_LogEvent(link, MP_ERROR_EVENT,
                    "Can't open connection, unknown -MPmode argument");
        return MP_SetError(link, MP_Failure);
    }

    if (status != MP_Success)
        return MP_Failure;

    /* get the protocol number for TCP from /etc/protocols */
    if ((protostruct = getprotobyname("tcp")) == NULL) {
      MP_LogEvent(link, MP_ERROR_EVENT,
                    "Can't open connection: can't get TCP protocol info!!");
      return MP_SetError(link, MP_Failure);
      }

    /* set socket to not coalesce small amounts of data in a single packet */
    if (setsockopt(((MP_TCP_t *)link->transp.private1)->sock, 
		   protostruct->p_proto, TCP_NODELAY, &sockopt, sizeof(int)) == -1) {
      MP_LogEvent(link, MP_ERROR_EVENT,
                 "Can't open connection: can't set NODELAY option on socket"); 
      return MP_SetError(link, MP_Failure);
      }

    ERR_CHK(tcp_negotiate_word_order(link));
    ERR_CHK(tcp_negotiate_fp_format(link));
    ERR_CHK(tcp_negotiate_bigint_format(link));
    ERR_CHK(tcp_exchange_pids(link));

#ifdef MP_DEBUG
    fprintf(stderr, "tcp_open_connection: exiting\n");
    fflush(stderr);
#endif

    RETURN_OK(link);
}


#ifdef __STDC__
static MP_Status_t tcp_exchange_pids(MP_Link_pt link)
#else
static MP_Status_t tcp_exchange_pids(link)
    MP_Link_pt link;
#endif
{
  MP_Uint32_t peerpid;
  MP_NumAnnot_t na;

  /* Hmm... should communicate pid_t -- could be 64 bit !! */
  ERR_CHK(MP_PutUint32Packet(link, (MP_Uint32_t) getpid(), 0));
  MP_EndMsgReset(link);
  
  ERR_CHK(MP_SkipMsg(link));
  ERR_CHK(MP_GetUint32Packet(link, &peerpid, &na));
  ((MP_TCP_t *)link->transp.private1)->peerpid = peerpid;

  return MP_Success;
}

/*
 * Currently this is a canned routine.  There is little parsing.  It
 * assumes that the first thing the two partners will do is to negotiate
 * the word order.  If that is a correct assumption, this should work
 * like a champ, otherwise we are in trouble.
 */
#ifdef __STDC__
static MP_Status_t tcp_negotiate_word_order(MP_Link_pt link)
#else
static MP_Status_t tcp_negotiate_word_order(link)
    MP_Link_pt link;
#endif
{
   MP_DictTag_t   dtag;
   MP_NumAnnot_t  na;
   MP_NumChild_t  nc;
   unsigned long  req_word_order;
   MP_Common_t    mp_op;

   /*
    * Step 1:  We send our word order preference to our partner
    */
   ERR_CHK(MP_PutCommonOperatorPacket(link, MP_MpDict, 
              MP_CopMpByteOrderRequest, 0, 1)); 
   ERR_CHK(IMP_PutUint32(link, (unsigned long)link->env->native_word_order));
   MP_EndMsgReset(link);

   /*
    * Step 2: Get a reply.  It should be MP_CopMpByteOrderRequest.
    * Probably should peek at the data stream here to be sure we are
    * getting the right thing.
    */
   ERR_CHK(MP_SkipMsg(link));
   ERR_CHK(MP_GetCommonOperatorPacket(link, &dtag, &mp_op, &na, &nc));
   ERR_CHK(IMP_GetUint32(link, &req_word_order));

   if (mp_op != MP_CopMpByteOrderRequest || dtag != MP_MpDict) {
      MP_LogEvent(link, MP_ERROR_EVENT, 
          "Problem exchanging initialization information: byte order");
      return MP_Failure;
   }


   /*
    * We only have to do something if the word order has changed.
    * This will only happen if both parties are Little Endian
    */
   if (req_word_order == (unsigned long)link->env->native_word_order
       && link->env->native_word_order != link->link_word_order) {
       link->link_word_order = link->env->native_word_order;
       MP_LogEvent(link, MP_INIT_EVENT,
                   "Link word order changed to Little Endian");
   }

   return MP_Success;
}



#ifdef __STDC__
static MP_Status_t tcp_negotiate_fp_format(MP_Link_pt link)
#else
static MP_Status_t tcp_negotiate_fp_format(link)
    MP_Link_pt link;
#endif
{
    MP_DictTag_t   dtag;
    MP_NumAnnot_t  na;
    MP_NumChild_t  nc;
    unsigned long  req_fp_format;
    MP_Common_t    mp_op;


#ifdef MP_DEBUG
    printf("tcp_negotiate_fp_format: entering\n");
#endif

    /*
     * Step 1:  We send our fp format preference to our partner
     */
    ERR_CHK(MP_PutCommonOperatorPacket(link, MP_MpDict, 
                                       MP_CopMpFpFormatRequest, 0, 1));
    ERR_CHK(IMP_PutUint32(link, (unsigned long)link->env->native_fp_format));
    MP_EndMsgReset(link);

    /*
     * Step 2: Get a reply.  It should be "MP_FpFormatRequest"
     * probably should peek at the data stream here to be sure we are
     *  getting the right thing
     */
    ERR_CHK(MP_SkipMsg(link));
    ERR_CHK(MP_GetCommonOperatorPacket(link, &dtag, &mp_op, &na, &nc));
    ERR_CHK(IMP_GetUint32(link, &req_fp_format));

   if (mp_op != MP_CopMpFpFormatRequest || dtag != MP_MpDict) {
      MP_LogEvent(link, MP_ERROR_EVENT, 
          "Problem exchanging initialization information: fp format");
      return MP_Failure;
   }

    /*
     * We only have to do something if the fp format has changed.
     * This will only happen if both parties are something other than IEEE.
     */
    if (req_fp_format == (unsigned long)link->env->native_fp_format
        && link->env->native_fp_format != link->link_fp_format) {
        link->link_fp_format = link->env->native_fp_format;
        MP_LogEvent(link, MP_INIT_EVENT,
                    "Link floating point format changed to VAX");
    }

#ifdef MP_DEBUG
    printf("tcp_negotiate_fp_format: exiting\n");
#endif

    return MP_Success;
}



#ifdef __STDC__
static MP_Status_t tcp_negotiate_bigint_format(MP_Link_pt link)
#else
static MP_Status_t tcp_negotiate_bigint_format(link)
    MP_Link_pt link;
#endif
{
    MP_DictTag_t   dtag;
    MP_NumAnnot_t  na;
    MP_NumChild_t  nc;
    unsigned long  req_bigint_format;
    MP_Common_t    mp_op;

#ifdef MP_DEBUG
    printf("tcp_negotiate_bigint_format: entering\n");
#endif

    /*
     * Step 1:  We send our bigint format preference to our partner
     */
    ERR_CHK(MP_PutCommonOperatorPacket(link, MP_MpDict, 
                                       MP_CopMpBigIntFormatRequest, 0, 1));
    ERR_CHK(IMP_PutUint32(link, (unsigned long)link->link_bigint_format));
    MP_EndMsgReset(link);

    /*
     * Step 2: Get a reply.  It should be "MP_BigIntFormatRequest"
     * probably should peek at the data stream here to be sure we are
     * getting the right thing
     */
    ERR_CHK(MP_SkipMsg(link));
    ERR_CHK(MP_GetCommonOperatorPacket(link, &dtag, &mp_op, &na, &nc));
    ERR_CHK(IMP_GetUint32(link, &req_bigint_format));

    if (mp_op != MP_CopMpBigIntFormatRequest || dtag != MP_MpDict) {
      MP_LogEvent(link, MP_ERROR_EVENT, 
          "Problem exchanging initialization information: big int format");
      return MP_Failure;
    }

    /*
     * We only have to do something if both ends are not already the same
     */
    if (req_bigint_format != (unsigned long)link->link_bigint_format) {
        link->link_bigint_format = MP_GMP;
        MP_LogEvent(link, MP_INIT_EVENT,
                    "Link multiple precision integer format set to GMP");
    }

#ifdef MP_DEBUG
    printf("tcp_negotiate_bigint_format: exiting\n");
#endif

    return MP_Success;
}



#ifdef __STDC__
MP_Status_t tcp_get_port(SOCKET *sock, short *port)
#else
MP_Status_t tcp_get_port(sock, port)
    SOCKET *s;
    short  *port;
#endif
{
    SOCKADDR_IN isin;
    int         attempts = 0;

    if (*port < IPPORT_RESERVED)
        *port = IPPORT_RESERVED;

    if ((*sock = socket(AF_INET, SOCK_STREAM, 0)) == INVALID_SOCKET)
        return MP_Failure;

    /*
     * Initialize socket's address structure
     */
    isin.sin_family      = AF_INET;
    isin.sin_addr.s_addr = htonl(INADDR_ANY);
    isin.sin_port        = htons(*port);

    while (bind(*sock, (SOCKADDR *)&isin, sizeof(isin)) == SOCKET_ERROR) {
        if (LASTERROR == ERRORCODE(EADDRINUSE)) {
            (*port)++;
            /* since port is an unsigned short, it may wrap around */
            if (*port < IPPORT_RESERVED)
                *port = IPPORT_RESERVED;
            isin.sin_port = htons(*port);
            attempts = 0;
        }

        if (attempts++ > MP_MAX_BIND_ATTEMPTS)
            return MP_Failure;
    }

    return MP_Success;
}



#ifdef __WIN32__
/*
 * WinSockInitialize calls the WinSock API initialization function WSAStartup
 * and returns a value != 0 if something went wrong. Extended error information
 * can be retrieved later by calling WSAGetLastError();
 */
#ifdef __STDC__
int WinSockInitialize(void)
#else /* __STDC__ */
int WinSockInitialize()
#endif /* __STDC__ */
{
   WORD        wVersionRequired;
   WSADATA     wsaData;

   wVersionRequired = MAKEWORD(1, 1);
   return WSAStartup(wVersionRequired, &wsaData);
}



/*
 * splitargs splits a string of words separated by white-space characters into
 * multiple strings by inserting Null characters after the ends of the words.
 * The function returns the number of words found in the string.
 */
#ifdef __STDC__
static int splitargs(char *argstr)
#else /* __STDC__ */
static int splitargs(argstr)
   char *argstr;
#endif /* __STDC__ */
{
    char *p, *q;
    int   argcnt, wordflag, quoted;

    p = q = argstr;
    argcnt = wordflag = quoted = 0;

    /* Step through all characters in the string. */
    while (*p)
        switch (*p) {
            /* Is *p a space or a tab character? */
        case ' ':
        case '\t':
            /* Simply pass it on if it is part of a `quoted` string. */
            if (quoted) {
                *q++ = *p++;
                break;
            }

            /*
             * If the character marks the end of a word then terminate that
             * word with a Null character, mark the word as read and increase
             * the argument counter.
             */
            if (wordflag) {
                *q++ = '\0';
                wordflag = 0;
                ++argcnt;
            }
            p++;
            break;

            /* Is *p a backquote character? */
        case '`':

            /* If the next character is another backquote then regard the
             * combination of both as a single literal backquote and pass it
             * on to the result string. If not, set the quote mode, i.e. pass
             * on all following characters until the next backquote is found or
             * the end of the string reached.
             */
            if (*++p == '`') {
                *q++ = *p++;
                wordflag = 1;
            } else
                quoted = !quoted;
            break;

            /* Put all other characters into the result string. */
        default:
            wordflag = 1;
            *q++ = *p++;
            break;
        }

    /* Terminate the last word with a Null character. */
    if (wordflag) {
        ++argcnt;
        *q = '\0';
    }

    return argcnt;
}

#endif /* __WIN32__ */



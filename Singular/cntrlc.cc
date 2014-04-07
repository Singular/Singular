/****************************************
*  Computer Algebra System SINGULAR     *
****************************************/
/*
* ABSTRACT - interupt handling
*/
#include <kernel/mod2.h>

/* includes */
#ifdef DecAlpha_OSF1
#define _XOPEN_SOURCE_EXTENDED
#endif /* MP3-Y2 0.022UF */

#include <omalloc/omalloc.h>

#include <reporter/si_signals.h>
#include <kernel/febase.h>

#include <Singular/tok.h>
#include <Singular/ipshell.h>
void sig_chld_hdl(int sig); /*#include <Singular/links/ssiLink.h>*/
#include <Singular/cntrlc.h>
#include <Singular/feOpt.h>
#include <Singular/misc_ip.h>
#include <Singular/links/silink.h>
#include <Singular/links/ssiLink.h>

#include <stdio.h>
#include <stddef.h>
#include <stdlib.h>
#include <strings.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/wait.h>

/* undef, if you don't want GDB to come up on error */

#define CALL_GDB

#if defined(__OPTIMIZE__) && defined(CALL_GDB)
#undef CALL_GDB
#endif

#if defined(unix)
 #include <unistd.h>
 #include <sys/types.h>

 #ifdef TIME_WITH_SYS_TIME
   #include <time.h>
   #ifdef HAVE_SYS_TIME_H
     #include <sys/time.h>
   #endif
 #else
   #ifdef HAVE_SYS_TIME_H
     #include <sys/time.h>
   #else
     #include <time.h>
   #endif
 #endif
 #ifdef HAVE_SYS_TIMES_H
   #include <sys/times.h>
 #endif

 #define INTERACTIVE 0
 #define STACK_TRACE 1

 #ifdef CALL_GDB
   static void debug (int);
   static void debug_stop (char *const*args);
 #endif
 #ifndef __OPTIMIZE__
   static void stack_trace_sigchld (int);
   static void stack_trace (char *const*args);
 #endif
#endif

si_link pipeLastLink=NULL;
BOOLEAN singular_in_batchmode=FALSE;

void sig_pipe_hdl(int /*sig*/)
{
 if (pipeLastLink!=NULL)
 {
   slClose(pipeLastLink);
   pipeLastLink=NULL;
   WerrorS("pipe failed");
 }
}

volatile BOOLEAN do_shutdown = FALSE;
volatile int defer_shutdown = 0;

void sig_term_hdl(int /*sig*/)
{
  do_shutdown = TRUE;
  if (!defer_shutdown)
  {
    m2_end(1);
  }
}

/*---------------------------------------------------------------------*
 * File scope Variables (Variables share by several functions in
 *                       the same file )
 *
 *---------------------------------------------------------------------*/
/* data */
jmp_buf si_start_jmpbuf;
int siRandomStart;
short si_restart=0;
BOOLEAN siCntrlc = FALSE;

typedef void (*si_hdl_typ)(int);


/*0 implementation*/
/*---------------------------------------------------------------------*
 * Functions declarations
 *
 *---------------------------------------------------------------------*/
void sigint_handler(int /*sig*/);

si_hdl_typ si_set_signal ( int sig, si_hdl_typ signal_handler);

/*---------------------------------------------------------------------*/
/**
 * @brief meta function for binding a signal to an handler

 @param[in] sig             Signal number
 @param[in] signal_handler  Pointer to signal handler

 @return value of signal()
**/
/*---------------------------------------------------------------------*/
si_hdl_typ si_set_signal ( int sig, si_hdl_typ signal_handler)
{
#if 0
  si_hdl_typ retval=signal (sig, (si_hdl_typ)signal_handler);
  if (retval == SIG_ERR)
  {
     fprintf(stderr, "Unable to init signal %d ... exiting...\n", sig);
  }
  si_siginterrupt(sig, 0);
  /*system calls will be restarted if interrupted by  the  specified
   * signal sig.  This is the default behavior in Linux.
   */
#else
  struct sigaction new_action,old_action;
  memset(&new_action, 0, sizeof(struct sigaction));

  /* Set up the structure to specify the new action. */
  new_action.sa_handler = signal_handler;
  if (sig==SIGINT)
    sigemptyset (&new_action.sa_mask);
  else
    new_action.sa_flags = SA_RESTART;

  int r=si_sigaction (sig, &new_action, &old_action);
  si_hdl_typ retval=(si_hdl_typ)old_action.sa_handler;
  if (r == -1)
  {
     fprintf(stderr, "Unable to init signal %d ... exiting...\n", sig);
     retval=SIG_ERR;
  }
#endif
  return retval;
}                               /* si_set_signal */


/*---------------------------------------------------------------------*/
#if defined(__linux__) && defined(__i386)
  #if !defined(HAVE_SIGCONTEXT) && !defined(HAVE_ASM_SIGCONTEXT_H)
// we need the following structure sigcontext_struct.
// if configure finds asm/singcontext.h we assume
// that this file contains the structure and is included
// via signal.h
struct sigcontext_struct {
        unsigned short gs, __gsh;
        unsigned short fs, __fsh;
        unsigned short es, __esh;
        unsigned short ds, __dsh;
        unsigned long edi;
        unsigned long esi;
        unsigned long ebp;
        unsigned long esp;
        unsigned long ebx;
        unsigned long edx;
        unsigned long ecx;
        unsigned long eax;
        unsigned long trapno;
        unsigned long err;
        unsigned long eip;
        unsigned short cs, __csh;
        unsigned long eflags;
        unsigned long esp_at_signal;
        unsigned short ss, __ssh;
        unsigned long i387;
        unsigned long oldmask;
        unsigned long cr2;
};
#endif
#define HAVE_SIGSTRUCT
typedef struct sigcontext_struct sigcontext;
#endif

#if defined(__linux__) && defined(__amd64)
#define HAVE_SIGSTRUCT
#endif


#if defined(HAVE_SIGSTRUCT)
/*2---------------------------------------------------------------------*/
/**
 * @brief signal handler for run time errors, linux/i386, x86_64 version

 @param[in] sig
 @param[in] s
**/
/*---------------------------------------------------------------------*/
void sigsegv_handler(int sig, sigcontext s)
{
  fprintf(stderr,"Singular : signal %d (v: %d):\n",sig,SINGULAR_VERSION);
  if (sig!=SIGINT)
  {
    fprintf(stderr,"current line:>>%s<<\n",my_yylinebuf);
    fprintf(stderr,"Segment fault/Bus error occurred at %lx because of %lx (r:%d)\n"
                   "please inform the authors\n",
                   #ifdef __i386__
                   (long)s.eip,
                   #else /* x86_64*/
                   (long)s.rip,
                   #endif
                   (long)s.cr2,siRandomStart);
  }
#ifdef __OPTIMIZE__
  if(si_restart<3)
  {
    si_restart++;
    fprintf(stderr,"trying to restart...\n");
    init_signals();
    longjmp(si_start_jmpbuf,1);
  }
#endif /* __OPTIMIZE__ */
#ifdef CALL_GDB
  if (sig!=SIGINT)
  {
    if (singular_in_batchmode) debug(STACK_TRACE);
    else                       debug(INTERACTIVE);
  }
#endif /* CALL_GDB */
  exit(0);
}

/*---------------------------------------------------------------------*/
#elif defined(SunOS) /*SPARC_SUNOS*/
/*2
* signal handler for run time errors, sparc sunos 4 version
*/
void sigsegv_handler(int sig, int code, struct sigcontext *scp, char *addr)
{
  fprintf(stderr,"Singular : signal %d, code %d (v: %d):\n",
    sig,code,SINGULAR_VERSION);
  if ((sig!=SIGINT)&&(sig!=SIGABRT))
  {
    fprintf(stderr,"current line:>>%s<<\n",my_yylinebuf);
    fprintf(stderr,"Segment fault/Bus error occurred at %x (r:%d)\n"
                   "please inform the authors\n",
                   (int)addr,siRandomStart);
  }
#ifdef __OPTIMIZE__
  if(si_restart<3)
  {
    si_restart++;
    fprintf(stderr,"trying to restart...\n");
    init_signals();
    longjmp(si_start_jmpbuf,1);
  }
#endif /* __OPTIMIZE__ */
#ifdef CALL_GDB
  if (sig!=SIGINT) debug(STACK_TRACE);
#endif /* CALL_GDB */
  exit(0);
}

#else

/*---------------------------------------------------------------------*/
/*2
* signal handler for run time errors, general version
*/
void sigsegv_handler(int sig)
{
  fprintf(stderr,"Singular : signal %d (v: %d):\n",
    sig,SINGULAR_VERSION);
  if (sig!=SIGINT)
  {
    fprintf(stderr,"current line:>>%s<<\n",my_yylinebuf);
    fprintf(stderr,"Segment fault/Bus error occurred (r:%d)\n"
                   "please inform the authors\n",
                   siRandomStart);
  }
  #ifdef __OPTIMIZE__
  if(si_restart<3)
  {
    si_restart++;
    fprintf(stderr,"trying to restart...\n");
    init_signals();
    longjmp(si_start_jmpbuf,1);
  }
  #endif /* __OPTIMIZE__ */
  #if defined(unix)
  #ifdef CALL_GDB
  if (sig!=SIGINT) debug(STACK_TRACE);
  #endif /* CALL_GDB */
  #endif /* unix */
  exit(0);
}
#endif


/*2
* signal handler for SIGINT
*/
int sigint_handler_cnt=0;
void sigint_handler(int /*sig*/)
{
  mflush();
  #ifdef HAVE_FEREAD
  if (fe_is_raw_tty) fe_temp_reset();
  #endif /* HAVE_FEREAD */
  char default_opt=' ';
  if ((feOptSpec[FE_OPT_CNTRLC].value!=NULL)
      && ((char*)(feOptSpec[FE_OPT_CNTRLC].value))[0])
  { default_opt=((char*)(feOptSpec[FE_OPT_CNTRLC].value))[0]; }
  loop
  {
    int cnt=0;
    int c;

    if (singular_in_batchmode)
    {
      c = 'q';
    }
    else if (default_opt!=' ')
    {
      c = default_opt;
    }
    else
    {
      fprintf(stderr,"// ** Interrupt at cmd:`%s` in line:'%s'\n",
        Tok2Cmdname(iiOp),my_yylinebuf);
      if (feOptValue(FE_OPT_EMACS) == NULL)
      {
        fputs("abort after this command(a), abort immediately(r), print backtrace(b), continue(c) or quit Singular(q) ?",stderr);fflush(stderr);
        c = fgetc(stdin);
      }
      else
      {
        c = 'a';
      }
    }

    switch(c)
    {
      case 'q': case EOF:
                m2_end(2);
      case 'r':
                if (sigint_handler_cnt<3)
                {
                  sigint_handler_cnt++;
                  fputs("** Warning: Singular should be restarted as soon as possible **\n",stderr);
                  fflush(stderr);
                  longjmp(si_start_jmpbuf,1);
                }
                else
                {
                  fputs("** tried too often, try another possibility **\n",stderr);
                  fflush(stderr);
                }
                break;
      case 'b':
                VoiceBackTrack();
                break;
      case 'a':
                siCntrlc++;
      case 'c':
                if ((feOptValue(FE_OPT_EMACS) == NULL) && (default_opt!=' '))
                {
                  /* Read until a newline or EOF */
                  while (c != EOF && c != '\n') c = fgetc(stdin);
                }
                si_set_signal(SIGINT ,(si_hdl_typ)sigint_handler);
                return;
                //siCntrlc ++;
                //if (siCntrlc>2) si_set_signal(SIGINT,(si_hdl_typ) sigsegv_handler);
                //else            si_set_signal(SIGINT,(si_hdl_typ) sigint_handler);
    }
    cnt++;
    if(cnt>5) m2_end(2);
  }
}

//void test_int()
//{
//  if (siCntrlc!=0)
//  {
//    int saveecho = si_echo;
//    siCntrlc = FALSE;
//    si_set_signal(SIGINT ,sigint_handler);
//    iiDebug();
//    si_echo = saveecho;
//  }
//}

#ifdef unix
#  ifndef __OPTIMIZE__
volatile int si_stop_stack_trace_x;
#    ifdef CALL_GDB
static void debug (int method)
{
  if (feOptValue(FE_OPT_NO_TTY))
  {
    dReportError("Caught Signal 11");
    return;
  }
  int pid;
  char buf[16];
  char * args[4] = { (char*)"gdb", (char*)"Singular", NULL, NULL };

  #ifdef HAVE_FEREAD
  if (fe_is_raw_tty) fe_temp_reset();
  #endif /* HAVE_FEREAD */

  sprintf (buf, "%d", getpid ());

  args[2] = buf;

  pid = fork ();
  if (pid == 0)
  {
    switch (method)
    {
      case INTERACTIVE:
        fprintf (stderr, "\n\nquit with \"p si_stop_stack_trace_x=0\"\n\n\n");
        debug_stop (args);
        break;
      case STACK_TRACE:
        fprintf (stderr, "stack_trace\n");
        stack_trace (args);
        break;
      default:
        // should not be reached:
        exit(1);
    }
  }
  else if (pid == -1)
  {
    perror ("could not fork");
    return;
  }

  si_stop_stack_trace_x = 1;
  while (si_stop_stack_trace_x) ;
}

static void debug_stop (char *const*args)
{
  execvp (args[0], args);
  perror ("exec failed");
  _exit (0);
}
#    endif /* CALL_GDB */

static void stack_trace (char *const*args)
{
  int pid;
  int in_fd[2];
  int out_fd[2];
  fd_set fdset;
  fd_set readset;
  struct timeval tv;
  int sel, index, state;
  char buffer[256];
  char c;

  if ((pipe (in_fd) == -1) || (pipe (out_fd) == -1))
  {
    perror ("could open pipe");
    m2_end(999);
  }

  pid = fork ();
  if (pid == 0)
  {
    si_close (0); si_dup2 (in_fd[0],0);   /* set the stdin to the in pipe */
    si_close (1); si_dup2 (out_fd[1],1);  /* set the stdout to the out pipe */
    si_close (2); si_dup2 (out_fd[1],2);  /* set the stderr to the out pipe */

    execvp (args[0], args);      /* exec gdb */
    perror ("exec failed");
    m2_end(999);
  }
  else if (pid == -1)
  {
    perror ("could not fork");
    m2_end(999);
  }

  FD_ZERO (&fdset);
  FD_SET (out_fd[0], &fdset);

  si_write (in_fd[1], "backtrace\n", 10);
  si_write (in_fd[1], "p si_stop_stack_trace_x = 0\n", 28);
  si_write (in_fd[1], "quit\n", 5);

  index = 0;
  state = 0;

  loop
  {
    readset = fdset;
    tv.tv_sec = 1;
    tv.tv_usec = 0;

    sel = si_select (FD_SETSIZE, &readset, NULL, NULL, &tv);
    if (sel == -1)
      break;

    if ((sel > 0) && (FD_ISSET (out_fd[0], &readset)))
    {
      if (si_read (out_fd[0], &c, 1))
      {
        switch (state)
        {
          case 0:
            if (c == '#')
            {
              state = 1;
              index = 0;
              buffer[index++] = c;
            }
            break;
          case 1:
            buffer[index++] = c;
            if ((c == '\n') || (c == '\r'))
            {
              buffer[index] = 0;
              fprintf (stderr, "%s", buffer);
              state = 0;
              index = 0;
            }
            break;
          default:
            break;
        }
      }
    }
    else if (si_stop_stack_trace_x==0)
      break;
  }

  si_close (in_fd[0]);
  si_close (in_fd[1]);
  si_close (out_fd[0]);
  si_close (out_fd[1]);
  m2_end(0);
}

#  endif /* !__OPTIMIZE__ */
#endif /* unix */

/*2
* init signal handlers
*/
void init_signals()
{
  #ifdef SIGSEGV
  si_set_signal(SIGSEGV,(si_hdl_typ)sigsegv_handler);
  #endif
  #ifdef SIGBUS
  si_set_signal(SIGBUS, (si_hdl_typ)sigsegv_handler);
  #endif
  #ifdef SIGFPE
  si_set_signal(SIGFPE, (si_hdl_typ)sigsegv_handler);
  #endif
  #ifdef SIGILL
  si_set_signal(SIGILL, (si_hdl_typ)sigsegv_handler);
  #endif
  #ifdef SIGIOT
  si_set_signal(SIGIOT, (si_hdl_typ)sigsegv_handler);
  #endif
  si_set_signal(SIGINT ,(si_hdl_typ)sigint_handler);
  si_set_signal(SIGCHLD, (si_hdl_typ)sig_chld_hdl);
  si_set_signal(SIGPIPE, (si_hdl_typ)sig_pipe_hdl);
  si_set_signal(SIGTERM, (si_hdl_typ)sig_term_hdl);
}


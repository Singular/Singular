/****************************************
*  Computer Algebra System SINGULAR     *
****************************************/
/* $Id: cntrlc.cc,v 1.40 2002-02-14 15:08:52 Singular Exp $ */
/*
* ABSTRACT - interupt handling
*/

/* includes */
#ifdef DecAlpha_OSF1
#define _XOPEN_SOURCE_EXTENDED
#endif
#include <stdio.h>
#include <stddef.h>
#include <stdlib.h>
#include <strings.h>
#include <signal.h>
#include "mod2.h"
#include "omalloc.h"
#include "tok.h"
#include "ipshell.h"
#include "febase.h"
#include "cntrlc.h"
#include "polys.h"
#include "feOpt.h"
#include "version.h"
#ifdef PAGE_TEST
#include "page.h"
#endif


/* undef, if you don't want GDB to come up on error */

#if !defined(__alpha)
#define CALL_GDB
#endif

#if defined(__OPTIMIZE__) && defined(CALL_GDB)
#undef CALL_GDB
#endif

#ifdef unix
#ifndef hpux
#include <unistd.h>
#include <sys/types.h>

#ifdef TIME_WITH_SYS_TIME
# include <time.h>
# ifdef HAVE_SYS_TIME_H
#   include <sys/time.h>
# endif
#else
# ifdef HAVE_SYS_TIME_H
#   include <sys/time.h>
# else
#   include <time.h>
# endif
#endif
#ifdef HAVE_SYS_TIMES_H
#include <sys/times.h>
#endif

#define INTERACTIVE 0
#define STACK_TRACE 1
#ifdef CALL_GDB
static void debug (int);
static void debug_stop (char **);
#endif
#ifndef __OPTIMIZE__
static void stack_trace (char **);
static void stack_trace_sigchld (int);
#endif
#endif
#endif

/* data */
jmp_buf si_start_jmpbuf;
int siRandomStart;
short si_restart=0;
BOOLEAN siCntrlc = FALSE;

typedef void (*si_hdl_typ)(int);

/*0 implementation*/
#ifndef MSDOS
/* signals are not implemented in DJGCC */
#ifndef macintosh
/* signals are not right implemented in macintosh */
void sigint_handler(int sig);
#endif
#endif

#if defined(linux) && defined(__i386__)
#if defined(HAVE_SIGCONTEXT) || defined(HAVE_ASM_SIGCONTEXT_H)
#include <asm/sigcontext.h>
#else
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
typedef struct sigcontext_struct sigcontext;

/*2
* signal handler for run time errors, linux/i386 version
*/
void sigsegv_handler(int sig, sigcontext s)
{
  fprintf(stderr,"Singular : signal %d (v: %d/%u):\n",sig,SINGULAR_VERSION,feVersionId);
  if (sig!=SIGINT)
  {
    fprintf(stderr,"Segment fault/Bus error occurred at %x because of %x (r:%d)\n"
                   "please inform the authors\n",
                   (int)s.eip,(int)s.cr2,siRandomStart);
  }
#ifdef __OPTIMIZE__
  if(si_restart<3)
  {
    si_restart++;
    fprintf(stderr,"trying to restart...\n");
    init_signals();
    longjmp(si_start_jmpbuf,1);
  }
#endif
#ifdef CALL_GDB
  if (sig!=SIGINT) debug(INTERACTIVE);
#endif
  exit(0);
}

#ifdef PAGE_TEST
#ifndef PAGE_INTERRUPT_TIME
#define PAGE_INTERRUPT_TIME 1
#endif
void sig11_handler(int sig, sigcontext s)
{
  unsigned long base =(unsigned long)(s.cr2&(~4095));
  int i;
  i=mmPage_tab_ind-1;
  while (mmPage_tab[i]!=base) i--;
  mmUse_tab[i]='1';
  mmPage_tab_acc++;
  mmPage_AllowAccess((void *)base);
  signal(SIGSEGV,(si_hdl_typ)sig11_handler);
}

void sigalarm_handler(int sig, sigcontext s)
{
  int i=mmPage_tab_ind-1;
  mmWriteStat();
  for(;i>=0;i--)
  {
    mmPage_DenyAccess((void *)mmPage_tab[i]);
  }
  struct itimerval t,o;
  memset(&t,0,sizeof(t));
  t.it_value.tv_sec     =(unsigned)0;
  t.it_value.tv_usec    =(unsigned) PAGE_INTERRUPT_TIME;
  o.it_value.tv_sec     =(unsigned)0;
  o.it_value.tv_usec    =(unsigned)PAGE_INTERRUPT_TIME;
  setitimer(ITIMER_VIRTUAL,&t,&o);
  signal(SIGVTALRM,(si_hdl_typ)sigalarm_handler);
}
#endif

/*2
* init signal handlers, linux/i386 version
*/
void init_signals()
{
/*4 signal handler: linux*/
#ifdef PAGE_TEST
  signal(SIGSEGV,(si_hdl_typ)sig11_handler);
  struct itimerval t,o;
  memset(&t,0,sizeof(t));
  t.it_value.tv_sec     =(unsigned)0;
  t.it_value.tv_usec    =(unsigned)PAGE_INTERRUPT_TIME;
  o.it_value.tv_sec     =(unsigned)0;
  o.it_value.tv_usec    =(unsigned)PAGE_INTERRUPT_TIME;
  setitimer(ITIMER_VIRTUAL,&t,&o);
  signal(SIGVTALRM,(si_hdl_typ)sigalarm_handler);
#else
  if (SIG_ERR==signal(SIGSEGV,(si_hdl_typ)sigsegv_handler))
  {
    PrintS("cannot set signal handler for SEGV\n");
  }
#endif
  if (SIG_ERR==signal(SIGFPE, (si_hdl_typ)sigsegv_handler))
  {
    PrintS("cannot set signal handler for FPE\n");
  }
  if (SIG_ERR==signal(SIGILL, (si_hdl_typ)sigsegv_handler))
  {
    PrintS("cannot set signal handler for ILL\n");
  }
  if (SIG_ERR==signal(SIGIOT, (si_hdl_typ)sigsegv_handler))
  {
    PrintS("cannot set signal handler for IOT\n");
  }
#ifndef macintosh
  if (SIG_ERR==signal(SIGINT ,sigint_handler))
  {
    PrintS("cannot set signal handler for INT\n");
  }
  signal(SIGCHLD, (void (*)(int))SIG_IGN);
#endif
}

#else
#ifdef SPARC_SUNOS_4
/*2
* signal handler for run time errors, sparc sunos 4 version
*/
void sigsegv_handler(int sig, int code, struct sigcontext *scp, char *addr)
{
  fprintf(stderr,"Singular : signal %d, code %d (v: %d/%u):\n",
    sig,code,SINGULAR_VERSION,feVersionId);
  if ((sig!=SIGINT)&&(sig!=SIGABRT))
  {
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
#endif
#ifdef CALL_GDB
  if (sig!=SIGINT) debug(STACK_TRACE);
#endif
  exit(0);
}

/*2
* init signal handlers, sparc sunos 4 version
*/
void init_signals()
{
/*4 signal handler:*/
  signal(SIGSEGV,sigsegv_handler);
  signal(SIGBUS, sigsegv_handler);
  signal(SIGFPE, sigsegv_handler);
  signal(SIGILL, sigsegv_handler);
  signal(SIGIOT, sigsegv_handler);
  signal(SIGINT ,sigint_handler);
  signal(SIGCHLD, (void (*)(int))SIG_IGN);
}
#else

/*2
* signal handler for run time errors, general version
*/
#ifndef macintosh
void sigsegv_handler(int sig)
{
  fprintf(stderr,"Singular : signal %d (v: %d/%u):\n",
    sig,SINGULAR_VERSION,feVersionId);
  if (sig!=SIGINT)
  {
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
#endif
#ifdef unix
#ifndef hpux
/* debug(..) does not work under HPUX (because ptrace does not work..) */
#ifdef CALL_GDB
#ifndef MSDOS
  if (sig!=SIGINT) debug(STACK_TRACE);
#endif
#endif
#endif
#endif
  exit(0);
}
#endif

/*2
* init signal handlers, general version
*/
void init_signals()
{
#ifndef MSDOS
/* signals are not implemented in DJGCC */
#ifndef macintosh
/* signals are temporaliy removed for macs. */
/*4 signal handler:*/
  signal(SIGSEGV,(void (*) (int))sigsegv_handler);
#ifdef SIGBUS
  signal(SIGBUS, sigsegv_handler);
#endif
#ifdef SIGFPE
  signal(SIGFPE, sigsegv_handler);
#endif
#ifdef SIGILL
  signal(SIGILL, sigsegv_handler);
#endif
#ifdef SIGIOT
  signal(SIGIOT, sigsegv_handler);
#endif
#ifdef SIGXCPU
  signal(SIGXCPU, (void (*)(int))SIG_IGN);
#endif
  signal(SIGINT ,sigint_handler);
  signal(SIGCHLD, (void (*)(int))SIG_IGN);
#endif
#endif
}
#endif
#endif

#ifndef MSDOS
#ifndef macintosh
/*2
* signal handler for SIGINT
*/
void sigint_handler(int sig)
{
  mflush();
  #ifdef HAVE_FEREAD
  if (fe_is_raw_tty) fe_temp_reset();
  #endif
  loop
  {
    int cnt=0;
    int c;
    fprintf(stderr,"// ** Interrupt at cmd:`%s` in line:'%s'\n",
      Tok2Cmdname(iiOp),my_yylinebuf);
    if (feGetOptValue(FE_OPT_EMACS) == NULL)
    {
      fputs("abort command(a), continue(c) or quit Singular(q) ?",stderr);fflush(stderr);
      c = fgetc(stdin);
    }
    else
    {
      c = 'a';
    }
    
    switch(c)
    {
#if defined(MONOM_COUNT) || defined(DIV_COUNT)
              case 'e':
#ifdef MONOM_COUNT
                extern void ResetMonomCount();
                ResetMonomCount();
#endif
#ifdef DIV_COUNT
                extern void ResetDivCount();
                ResetDivCount();
#endif
                break;
              case 'o':
#ifdef MONOM_COUNT
                extern void OutputMonomCount();
                OutputMonomCount();
#endif
#ifdef DIV_COUNT
                extern void OutputDivCount();
                OutputDivCount();
#endif
                break;
#endif // defined(MONOM_COUNT) || defined(DIV_COUNT)
      case 'q':
                m2_end(2);
      case 'r':
                longjmp(si_start_jmpbuf,1);
      case 'b':
                VoiceBackTrack();
                break;
      case 'a':
                siCntrlc++;
      case 'c':
                if (feGetOptValue(FE_OPT_EMACS) == NULL) fgetc(stdin);
                signal(SIGINT ,(si_hdl_typ)sigint_handler);
                return;
                //siCntrlc ++;
                //if (siCntrlc>2) signal(SIGINT,(si_hdl_typ) sigsegv_handler);
                //else            signal(SIGINT,(si_hdl_typ) sigint_handler);
    }
    cnt++;
    if(cnt>5) m2_end(2);
  }
}
#endif
#endif

//#ifdef macintosh
//#include <Types.h>
//#include <Events.h>
//#include <OSEvents.h>
//#include <CursorCtl.h>
//
///*3
//* macintosh only:
//* side effect of ^C is to insert EOF at the end of the current
//* input selection. We must drain input, reach this EOF, then clear it
//*/
//static void flush_intr(void)
//{
//  int c;
//
//  while ((c=getchar())!=EOF);
//  clearerr(stdin);
//}
//
///*3
//* macintosh only:
//* spin beach ball in MPW, allows MPW-tool to go to the background
//* so you can use the finder and interrupts
//*/
//static void beachball(void)
//{
//  Show_Cursor(HIDDEN_CURSOR);
//  SpinCursor(10);
//}
//#endif

#ifndef MSDOS
//void test_int()
//{
//#ifndef macintosh
//  if (siCntrlc!=0)
//  {
//    int saveecho = si_echo;
//    siCntrlc = FALSE;
//    signal(SIGINT ,sigint_handler);
////#ifdef macintosh
////    flush_intr();
////#endif
//    iiDebug();
//    si_echo = saveecho;
//  }
//#endif
//}
#endif

#ifdef unix
#ifndef hpux
#ifndef __OPTIMIZE__
#ifndef MSDOS
int si_stop_stack_trace_x;
#ifdef CALL_GDB
static void debug (int method)
{
  if (feOptValue(FE_OPT_NO_TTY))
  {
    dReportError("Caught Signal 11");
    return;
  }
  int pid;
  char buf[16];
  char *args[4] = { "gdb", "Singularg", NULL, NULL };

  #ifdef HAVE_FEREAD
  if (fe_is_raw_tty) fe_temp_reset();
  #endif

  sprintf (buf, "%d", getpid ());

  args[2] = buf;

  pid = fork ();
  if (pid == 0)
  {
    switch (method)
    {
      case INTERACTIVE:
        fprintf (stderr, "debug_stop\n");
        debug_stop (args);
        break;
      case STACK_TRACE:
        fprintf (stderr, "stack_trace\n");
        stack_trace (args);
        break;
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

static void debug_stop ( char **args)
{
  execvp (args[0], args);
  perror ("exec failed");
  _exit (0);
}
#endif

static int stack_trace_done;

static void stack_trace (char **args)
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

  stack_trace_done = 0;

  signal (SIGCHLD, stack_trace_sigchld);

  if ((pipe (in_fd) == -1) || (pipe (out_fd) == -1))
  {
    perror ("could open pipe");
    m2_end(999);
  }

  pid = fork ();
  if (pid == 0)
  {
    close (0); dup2 (in_fd[0],0);   /* set the stdin to the in pipe */
    close (1); dup2 (out_fd[1],1);  /* set the stdout to the out pipe */
    close (2); dup2 (out_fd[1],2);  /* set the stderr to the out pipe */

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

  write (in_fd[1], "backtrace\n", 10);
  write (in_fd[1], "p si_stop_stack_trace_x = 0\n", 28);
  write (in_fd[1], "quit\n", 5);

  index = 0;
  state = 0;

  loop
  {
    readset = fdset;
    tv.tv_sec = 1;
    tv.tv_usec = 0;

#ifdef hpux
    sel = select (FD_SETSIZE, (int *)readset.fds_bits, NULL, NULL, &tv);
#else
    sel = select (FD_SETSIZE, &readset, NULL, NULL, &tv);
#endif
    if (sel == -1)
      break;

    if ((sel > 0) && (FD_ISSET (out_fd[0], &readset)))
    {
      if (read (out_fd[0], &c, 1))
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
    else if (stack_trace_done)
      break;
  }

  close (in_fd[0]);
  close (in_fd[1]);
  close (out_fd[0]);
  close (out_fd[1]);
  m2_end(0);
}

static void stack_trace_sigchld (int signum)
{
  stack_trace_done = 1;
}

#endif
#endif
#endif
#endif

/* Under HPUX 9, system(...) returns -1 if SIGCHLD does not equal
   SIG_DFL. However, if it stays at SIG_DFL we get zombie processes
   for terminated childs generated by fork. Therefors some special treatment 
   is necessary */
#ifdef HPUX_9
#undef system
extern "C" {
  int  hpux9_system(const char* call)
  {
    int ret;
    signal(SIGCHLD, (void (*)(int))SIG_DFL);
    ret = system(call);
    signal(SIGCHLD, (void (*)(int))SIG_IGN);
    return ret;
  }
}
#endif /* HPUX_9 */

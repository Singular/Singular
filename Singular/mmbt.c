/****************************************
*  Computer Algebra System SINGULAR     *
****************************************/
/* $Id: mmbt.c,v 1.16 1999-10-25 08:32:16 obachman Exp $ */
/*
* ABSTRACT: backtrace: part of memory subsystem (for linux/elf)
* needed programs: - mprpc to set the variable MPRPC
*                  - mprdem: must be in the current directory
*                  - mprnm: must be in thje current directory
* files: - Singularg: the name of the executable
*        - nm.log: temp. file for the map address -> name
*/

/* for simplicity: a fixed size array (currently used: 4437 entries (98111016))
*/
#define MAX_PROCS_BT 6000

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <stddef.h>
#include "mod2.h"
#include "tok.h"
#include "mmprivate.h"
#include "febase.h"
#include "mmbt.h"
#include "febase.h"


#ifdef MTRACK
// #ifndef __OPTIMIZE__
/* does only work in debug mode: 
* requires that -fomit-frame-pointer is not given
*/
#if defined(linux) && defined(__i386__)

static unsigned long mm_lowpc=0, mm_highpc=0;

extern int etext ();

/*
 * Number of words between saved pc and saved fp.
 * All stack frames seen by mmTrack() must conform to
 * the same value of SLOP.
 */
#ifndef SLOP
#define SLOP 0
#endif

#define entrypc(pc)        ((pc >= mm_lowpc && pc <= mm_highpc) || pc >= (unsigned long) &etext || (pc < 4096))
#define getfp(ap)        (ap-2-SLOP)
#define getpc(fp)        (fp[1+SLOP])

int mmTrackInit ()
{
  char *entry = getenv ("MPRPC");

  if ((entry!=NULL) && (strchr (entry, ':')!=NULL))
  {
    mm_lowpc = atoi (entry);
    mm_highpc = atoi (1 + strchr (entry, ':'));
    return 0;
  }
  return 1;
}

static mmTrack_sig11_caught = 0;
typedef void (*si_hdl_typ)(int);

void mmTrack_sig11_handler(int sig)
{
  mmTrack_sig11_caught = 1;
  printf("SIG11 Caught\n");
  fflush(stdout);
}

void mmTrack (unsigned long *bt_stack)
{
  unsigned long pc, *fp = getfp ((unsigned long *) &bt_stack);
  int i=0;
#if 0 /* Geht nicht */
  si_hdl_typ sig11_handler = signal(SIGSEGV, (si_hdl_typ) mmTrack_sig11_handler);
  mmTrack_sig11_caught = 0;
#endif
  
  if (mm_lowpc==0) mmTrackInit();
  
  
  while ((fp!=NULL) && ((unsigned long)fp>4095)  
  && ((unsigned long)fp < ((unsigned long)0xff000000)) 
  && *fp && (pc = getpc (fp)) 
  && !entrypc (pc) && (i<BT_MAXSTACK))
  {
    if ( mmTrack_sig11_caught) break;
    bt_stack[i]=pc; i++;
    fp = (unsigned long *) *fp;
    
  }
/*  signal(SIGSEGV, (si_hdl_typ) sig11_handler); */
  while(i<BT_MAXSTACK)
  {
    bt_stack[i]=0; i++;
  }
}

struct
{
  unsigned long p;
  char *name;
} p2n[MAX_PROCS_BT];

static int mm_p2n_max = -1;
void mmP2cNameInit()
{
  FILE *f;
  int i,j;
  char n[128];
  char s[2000];
  sprintf(s, "%s/mprnm -mprdem %s/mprdem -p %s >nm.log", 
          feGetResource('b'), feGetResource('b'), feGetResource('S'));
  system(s);
  f=fopen("nm.log","r");
  i=0;
  loop
  {
    j=fscanf(f,"%d %s\n",(int *)&p2n[i].p,n);
    if (j!=2) 
    {
      break;
    }
    if (strcmp(n, "___crt_dummy__") != 0 && strcmp(n, "_start") != 0)
    {
      p2n[i].name=strdup(n);
      i++;
    }
  }
  fclose(f);
  unlink("nm.log");
  p2n[i].name="??";
  p2n[i].p=~1;
  mm_p2n_max=i;
}
char * mmP2cName(unsigned long p)
{
  int i, e;
  int a=0;
  if (mm_p2n_max == -1)
    mmP2cNameInit();
  e=mm_p2n_max;
  loop
  {
    if (a>=e-1) 
    {
      if (p2n[a].p<=p) return p2n[a].name;
      else if (a==0) return("??");
      else Print("?? sort %x,%d in %d and %d\n",p,p,a,e);
    }
    i=(a+e)/2;
    if (p>=p2n[i].p)
    {
      a=i;
    }
    else
    {
      e=i;
    }
  }  
#if 0
  for(i=0;i<=mm_p2n_max;i++)
  {
    if (p>=p2n[i].p)
    {
      if (p<p2n[i+1].p) return p2n[i].name;
      if (0==p2n[i+1].p) return p2n[i].name;
    }
  }
  return "??";
#endif
}

void mmPrintStack(FILE *fd, unsigned long *stack, int all)
{
  mmPrintStackFrames(fd, stack, 0, BT_MAXSTACK, all);
}

void mmDBPrintThisStack(FILE *fd, void* memblock, int all, int free)
{
#ifdef MTRACK_FREE
  if (free)
    mmPrintStackFrames(fd, ((DBMCB*) memblock)->bt_freed_stack, 0, BT_MAXSTACK, all);
  else
#endif
    mmPrintStackFrames(fd, ((DBMCB*) memblock)->bt_allocated_stack, 0, BT_MAXSTACK, all);
}
    
void mmDBPrintStack(FILE *fd, void* memblock, int all)
{
  mmPrintStackFrames(fd, ((DBMCB*) memblock)->bt_allocated_stack, 0, BT_MAXSTACK, all);
}

void mmDBPrintStackFrames(FILE *fd, void* memblock, int start, int end)
{
  mmPrintStackFrames(fd, ((DBMCB*) memblock)->bt_allocated_stack, start, end, 
                     MM_PRINT_ALL_STACK);
}

/* print stack */
void mmPrintStackFrames(FILE *fd, unsigned long *bt_stack, int start, int end, int mm) 
{
  int i=start;
  fprintf( fd," ");
  do
  {
    char *s;
    s=mmP2cName(bt_stack[i]); 
    if (s!=NULL && strcmp(s, "??"))
    {
      if ((mm & MM_PRINT_ALL_STACK) || strncmp(s, "mm", 2) !=0)
        fprintf( fd,":%s",s);
      if (strcmp(s, "main") == 0) break;
    }
    else
      fprintf( fd,":%lx",(long)bt_stack[i]);
    i++;
  } while ((i<end) && (bt_stack[i]!=0));
  fprintf( fd,"\n");
}
#endif /* linux, i386 */
// #endif /* not optimize */
#endif /* MTRACK */

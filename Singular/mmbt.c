/****************************************
*  Computer Algebra System SINGULAR     *
****************************************/
/* $Id: mmbt.c,v 1.3 1998-12-02 14:00:13 obachman Exp $ */
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
#define MAX_PROCS_BT 5000

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "mod2.h"
#include "tok.h"
#include "mmprivat.h"

#ifdef MTRACK
#ifndef __OPTIMIZE__
/* does only work in debug mode: 
* requires that -fomit-frame-pointer is not given
*/
#if defined(linux) && defined(__i386__)

#undef ulong
#define ulong unsigned long

extern int etext ();
static ulong mm_lowpc=0, mm_highpc=0;

/*
 * Number of words between saved pc and saved fp.
 * All stack frames seen by mmTrack() must conform to
 * the same value of SLOP.
 */
#ifndef SLOP
#define SLOP 0
#endif

#define entrypc(pc)        ((pc >= mm_lowpc && pc <= mm_highpc) || pc >= (ulong) &etext || (pc < 4096))
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

void mmTrack (ulong *bt_stack)
{
  ulong pc, *fp = getfp ((ulong *) &bt_stack);
  int i=0;

  if (mm_lowpc==0) mmTrackInit();

  while ((fp!=NULL) && ((ulong)fp>4095)  && *fp && (pc = getpc (fp)) 
  && !entrypc (pc) && (i<BT_STACKMAX))
  {
    bt_stack[i]=pc; i++;
    fp = (ulong *) *fp;
  }
  while(i<BT_STACKMAX)
  {
    bt_stack[i]=0; i++;
  }
}

struct
{
  ulong p;
  char *name;
} p2n[MAX_PROCS_BT];

static int mm_p2n_max;
void mmP2cNameInit()
{
  FILE *f;
  int i,j;
  char n[128];
  system("./mprnm -p Singularg >nm.log");
  f=fopen("nm.log","r");
  i=0;
  loop
  {
    j=fscanf(f,"%d %s\n",&p2n[i].p,n);
    if (j!=2) break;
    p2n[i].name=strdup(n);
    i++;
  }
  fclose(f);
  unlink("nm.log");
  p2n[i].name="??";
  p2n[i].p=~1;
  mm_p2n_max=i;
}
char * mmP2cName(ulong p)
{
  int a=0;
  int e=mm_p2n_max;
  int i;
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
#endif /* linux, i386 */
#endif /* not optimize */
#endif /* MTRACK */

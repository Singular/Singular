#ifndef SILINK_H
#define SILINK_H
/****************************************
*  Computer Algebra System SINGULAR     *
****************************************/
/*
* ABSTRACT
*/
/* $Log: not supported by cvs2svn $
*/

#include "structs.h"

// extension links:
typedef BOOLEAN    (*slInitProc)(si_link l,si_link_extension s);
typedef BOOLEAN    (*slOpenReadProc)(si_link l);
typedef BOOLEAN    (*slOpenWriteProc)(si_link l);
typedef BOOLEAN    (*slCloseProc)(si_link l);
typedef leftv      (*slReadProc)(si_link l);
typedef leftv      (*slRead2Proc)(si_link l,leftv a);
typedef BOOLEAN    (*slWriteProc)(si_link l,leftv v);

struct s_si_link_extension
{
  si_link_extension next;
  slInitProc       Init;
  slOpenReadProc   OpenRead;
  slOpenWriteProc  OpenWrite;
  slCloseProc      Close;
  slReadProc       Read;
  slRead2Proc      Read2;
  slWriteProc      Write;
  char             *name;
  int              index;
};

struct sip_link
{
  si_link_extension m; // methods
  char *name;
  char **argv;
  void *data;
  int  linkType;
  int  argc;
  BITSET flags; /*0: open, 1: read, 2: write, ... */
};

BOOLEAN slExtend(si_link_extension s);

// flags:
#define SI_LINK_CLOSE   0
#define SI_LINK_OPEN    1
#define SI_LINK_READ    2
#define SI_LINK_WRITE   4

// tests:
#define SI_LINK_CLOSE_P(l)  (!(l)->flags)
#define SI_LINK_OPEN_P(l)   ((l)->flags & SI_LINK_OPEN)
#define SI_LINK_W_OPEN_P(l) ((l)->flags &  SI_LINK_WRITE)
#define SI_LINK_R_OPEN_P(l) ((l)->flags &  SI_LINK_READ)
#define SI_LINK_RW_OPEN_P(l) (SI_LINK_W_OPEN_P(l) && SI_LINK_R_OPEN_P(l))

#define SI_LINK_SET_CLOSE_P(l)  ((l)->flags = SI_LINK_CLOSE)
#define SI_LINK_SET_OPEN_P(l)   ((l)->flags = (l)->flags | SI_LINK_OPEN)
#define SI_LINK_SET_W_OPEN_P(l) ((l)->flags = (l)->flags | SI_LINK_OPEN | SI_LINK_WRITE)
#define SI_LINK_SET_R_OPEN_P(l) ((l)->flags = (l)->flags | SI_LINK_OPEN | SI_LINK_READ)
#define SI_LINK_SET_RW_OPEN_P(l) ((l)->flags = (l)->flags | SI_LINK_OPEN | SI_LINK_READ | SI_LINK_WRITE)

BOOLEAN slOpenWrite(si_link l);
BOOLEAN slOpenRead(si_link l);
BOOLEAN slClose(si_link l);
leftv   slRead(si_link l,leftv a=NULL);
BOOLEAN slWrite(si_link l, leftv v);
BOOLEAN slInit(si_link l, char *str);
void slKill(si_link l);
void slExtensionInit(si_link_extension s);
void slStandardInit();
BOOLEAN slInitALink(si_link l,si_link_extension s);
#ifdef HAVE_MPSR
void slInitBatchLink(si_link l, int argc, char **argv);
int  Batch_do(int arrgc, char **argv);
#endif
void GetCmdArgs(int *argc, char ***argv, char *str);
#endif

#ifndef  SING_MP_H
#define SING_MP_H
/****************************************
*  Computer Algebra System SINGULAR     *
****************************************/
/*
* ABSTRACT
*/
/* $Id: sing_mp.h,v 1.2 1997-03-24 14:25:51 Singular Exp $ */

#include "subexpr.h"
#include "silink.h"


void FreeCmdArgs(int argc, char** argv);
#ifdef HAVE_MPSR
void slInitBatchLink(si_link l, int argc, char** argv);
si_link_extension slInitMPFile();
si_link_extension slInitMPTcp();
extern BOOLEAN mpsr_IsMPLink(si_link l);
typedef struct MP_Link_t;
typedef MP_Link_t * MP_Link_pt;
extern MP_Link_pt OpenMPFile(char *fn, short mode);
#endif

int Batch_do(int argc, char **argv);
#endif

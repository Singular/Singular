#ifndef  SING_MP_H
#define SING_MP_H
/****************************************
*  Computer Algebra System SINGULAR     *
****************************************/
/*
* ABSTRACT
*/
/* $Id: sing_mp.h,v 1.3 1997-03-28 21:44:39 obachman Exp $ */

#include "subexpr.h"
#include "silink.h"


void FreeCmdArgs(int argc, char** argv);
#ifdef HAVE_MPSR
void slInitBatchLink(si_link l, int argc, char** argv);
si_link_extension slInitMPFile();
si_link_extension slInitMPTcp();
extern BOOLEAN mpsr_IsMPLink(si_link l);
#endif

int Batch_do(int argc, char **argv);
#endif

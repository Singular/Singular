/****************************************
*  Computer Algebra System SINGULAR     *
****************************************/
/***************************************************************
 *  File:    p_Procs_Static.cc
 *  Purpose: source for static version of p_Procs
 *  Author:  obachman (Olaf Bachmann)
 *  Created: 12/00
 *  Version: $Id: p_Procs_Static.cc,v 1.1.1.1 2003-10-06 12:16:00 Singular Exp $
 *******************************************************************/
#include "mod2.h"
#include "structs.h"
#include "p_polys.h"
#include "ring.h"
#include "p_Procs.h"
#include "p_Numbers.h"
#include "p_MemCmp.h"
#include "p_MemAdd.h"
#include "p_MemCopy.h"
#include "kbuckets.h"
#include "dError.h"

BOOLEAN p_procs_dynamic = FALSE;

#define p_Procs_Static
#include "p_Procs_Static.inc"

// include generated configuration
#include "p_Procs_Static.h"
// include general p_Proc stuff
#include "p_Procs_Impl.h"

// define DoSetProc and InitSetProcs
#define SetStaticProcFromArray(what, type, field, length, ord) \
  _p_procs->what = (what##_Proc_Ptr) \
    what##_Proc_##type [index(what##_Proc, field, length, ord)]

#define SetStaticProcFromFuncsArray(what, field, length, ord) \
  SetStaticProcFromArray(what, funcs, field, length, ord)

#ifdef RDEBUG
#define DoSetProc(what, field, length, ord)                     \
do                                                              \
{                                                               \
  if (set_names)                                                \
    SetStaticProcFromArray(what, names, field, length, ord);    \
  else                                                          \
    SetStaticProcFromFuncsArray(what, field, length, ord);      \
}                                                               \
while(0)
#else
#define DoSetProc SetStaticProcFromFuncsArray
#endif

#define InitSetProcs(f, l, o) ((void)0)

// include routines for setting p_ProcsSet

#include "p_Procs_Set.h"





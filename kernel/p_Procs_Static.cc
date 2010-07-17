/****************************************
*  Computer Algebra System SINGULAR     *
****************************************/
/***************************************************************
 *  File:    p_Procs_Static.cc
 *  Purpose: source for static version of p_Procs
 *  Author:  obachman (Olaf Bachmann)
 *  Created: 12/00
 *  Version: $Id$
 *******************************************************************/
#include <kernel/mod2.h>
#include <kernel/structs.h>
#include <kernel/p_polys.h>
#include <kernel/ring.h>
#include <kernel/p_Procs.h>
#include <kernel/p_Numbers.h>
#include <kernel/p_MemCmp.h>
#include <kernel/p_MemAdd.h>
#include <kernel/p_MemCopy.h>
#include <kernel/kbuckets.h>
#include <kernel/dError.h>

BOOLEAN p_procs_dynamic = FALSE;

#define p_Procs_Static
#include <kernel/p_Procs_Static.inc>

// include generated configuration
#include <kernel/p_Procs_Static.h>
// include general p_Proc stuff
#include <kernel/p_Procs_Impl.h>

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

// include routines for setting p_ProcsSet

#include <kernel/p_Procs_Set.h>





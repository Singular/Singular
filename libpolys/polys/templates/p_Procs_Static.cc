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
#include "config.h"
// #include <polys/structs.h>
#include <polys/monomials/ring.h>
#include <polys/monomials/p_polys.h>
#include <polys/templates/p_Procs.h>
#include <polys/templates/p_Numbers.h>
#include <polys/templates/p_MemCmp.h>
#include <polys/templates/p_MemAdd.h>
#include <polys/templates/p_MemCopy.h>
// #include <polys/kbuckets.h>
#include <reporter/reporter.h>

BOOLEAN p_procs_dynamic = FALSE;

#define p_Procs_Static
#include "templates/p_Procs.inc"

// include generated configuration
#include "templates/p_Procs_Static.h"
// include general p_Proc stuff
#include "templates/p_Procs_Impl.h"

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

#include "templates/p_Procs_Set.h"





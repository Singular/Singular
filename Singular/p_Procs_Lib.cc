/****************************************
*  Computer Algebra System SINGULAR     *
****************************************/
/***************************************************************
 *  File:    p_Procs_Lib.cc
 *  Purpose: source for shared library of p_Procs
 *  Author:  obachman (Olaf Bachmann)
 *  Created: 8/00
 *  Version: $Id: p_Procs_Lib.cc,v 1.1 2000-12-07 15:06:39 obachman Exp $
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

#define LINKAGE extern "C"

#include "p_Procs_Lib.inc"


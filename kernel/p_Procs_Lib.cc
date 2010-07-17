/****************************************
*  Computer Algebra System SINGULAR     *
****************************************/
/***************************************************************
 *  File:    p_Procs_Lib.cc
 *  Purpose: source for shared library of p_Procs
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

#include <kernel/p_Procs_Dynamic.inc>


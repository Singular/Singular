/****************************************
*  Computer Algebra System SINGULAR     *
****************************************/
/***************************************************************
 *  File:    pShallowCopyDelete.cc
 *  Purpose: implementation of pShallowCopyDelete routines
 *  Author:  obachman (Olaf Bachmann)
 *  Created: 8/00
 *  Version: $Id: pShallowCopyDelete.h,v 1.1.1.1 2003-10-06 12:15:58 Singular Exp $
 *******************************************************************/

// returns a poly from dest_r which is a ShallowCopy of s_p from source_r
// assumes that source_r->N == dest_r->N and that orderings are the same
typedef poly (*pShallowCopyDeleteProc)(poly s_p, ring source_r, ring dest_r, 
                                       omBin dest_bin);
pShallowCopyDeleteProc pGetShallowCopyDeleteProc(ring source_r, ring dest_r);


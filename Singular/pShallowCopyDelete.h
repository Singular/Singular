/****************************************
*  Computer Algebra System SINGULAR     *
****************************************/
/***************************************************************
 *  File:    pShallowCopyDelete.cc
 *  Purpose: implementation of pShallowCopyDelete routines
 *  Author:  obachman (Olaf Bachmann)
 *  Created: 8/00
 *  Version: $Id: pShallowCopyDelete.h,v 1.2 2001-10-09 16:36:12 Singular Exp $
 *******************************************************************/

// returns a poly from dest_r which is a ShallowCopy of s_p from source_r
typedef poly (*pShallowCopyDeleteProc)(poly s_p, ring source_r, ring dest_r,
                                       omBin dest_bin);
pShallowCopyDeleteProc pGetShallowCopyDeleteProc(ring source_r, ring dest_r);


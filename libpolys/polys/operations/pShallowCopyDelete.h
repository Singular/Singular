/****************************************
*  Computer Algebra System SINGULAR     *
****************************************/
/***************************************************************
 *  File:    pShallowCopyDelete.cc
 *  Purpose: implementation of pShallowCopyDelete routines
 *  Author:  obachman (Olaf Bachmann)
 *  Created: 8/00
 *  Version: $Id$
 *******************************************************************/
#ifndef PSHALLOWCOPYDELETE_H
#define PSHALLOWCOPYDELETE_H

#include <polys/monomials/p_polys.h>
#include <polys/monomials/ring.h>

pShallowCopyDeleteProc pGetShallowCopyDeleteProc(ring source_r, ring dest_r);

#endif // PSHALLOWCOPYDELETE_H

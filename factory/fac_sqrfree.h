/* emacs edit mode for this file is -*- C++ -*- */
/* $Id: fac_sqrfree.h,v 1.5 2008-01-25 14:18:56 Singular Exp $ */

#ifndef INCL_FAC_SQRFREE_H
#define INCL_FAC_SQRFREE_H

#include <config.h>

#include "canonicalform.h"

/*BEGINPUBLIC*/

int Powerup( const int base , const int exp);
CFFList appendCFFL( const CFFList & Inputlist, const CFFactor & TheFactor);
CFFList UnionCFFL(const CFFList & Inputlist1,const CFFList & Inputlist2);


/*ENDPUBLIC*/

CFFList sortCFFList ( CFFList & F );

//CFFList sqrFreeFp ( const CanonicalForm & f );
CFFList sqrFreeFp ( const CanonicalForm & r, const CanonicalForm &mipo=0 );

CFFList sqrFreeZ ( const CanonicalForm & f, const CanonicalForm &mipo=0 );

#endif /* ! INCL_FAC_SQRFREE_H */

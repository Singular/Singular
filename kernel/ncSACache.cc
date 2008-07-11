/****************************************
*  Computer Algebra System SINGULAR     *
****************************************/
/***************************************************************
 *  File:    ncSAMult.cc
 *  Purpose: implementation of multiplication in simple NC subalgebras
 *  Author:  motsak
 *  Created: 
 *  Version: $Id: ncSACache.cc,v 1.1 2008-07-11 15:53:28 motsak Exp $
 *******************************************************************/


#define MYTEST 0
#define OUTPUT 0

#if MYTEST
#define OM_CHECK 4
#define OM_TRACK 5
#endif

#include "mod2.h"

#include <ncSACache.h> // for CCacheHash etc classes

#include <ring.h>
#include <p_polys.h>



virtual void CGlobalCacheHash::History(const CGlobalCacheHash::CExponent a, const CGlobalCacheHash::CExponent b, const EHistoryType t)
{
  Print("GlobalPair!");
}


virtual void CSpecialPairCacheHash::History(const CSpecialPairCacheHash::CExponent a, const CSpecialPairCacheHash::CExponent b, const EHistoryType t)
{
  Print("SpecialPair!\n");
}

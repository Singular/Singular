/****************************************
*  Computer Algebra System SINGULAR     *
****************************************/
/***************************************************************
 *  File:    ncSACache.cc
 *  Purpose: implementation of special Cache+Hash for Multiplier 
 *  Author:  motsak
 *  Created: 
 *  Version: $Id: ncSACache.cc,v 1.2 2008-07-15 16:27:58 motsak Exp $
 *******************************************************************/


#define MYTEST 1
#define OUTPUT 1

#if MYTEST
#define OM_CHECK 4
#define OM_TRACK 5
#endif

#include "mod2.h"

#include <ncSACache.h> // for CCacheHash etc classes

#include <ring.h>
#include <p_polys.h>



void CGlobalCacheHash::History(const CGlobalCacheHash::CExponent a, const CGlobalCacheHash::CExponent b, const EHistoryType t)
{
  Print("History: GlobalPair!");
  PrintS("Left : "); p_Write(a, GetBasering());
  PrintS("Right: "); p_Write(b, GetBasering());    
}


void CSpecialPairCacheHash::History(const CSpecialPairCacheHash::CExponent a, const CSpecialPairCacheHash::CExponent b, const EHistoryType t)
{
  Print("SpecialPair!\n");
  Print("Left : %d, Right: %d\n", a, b);
}

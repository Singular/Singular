/****************************************
*  Computer Algebra System SINGULAR     *
****************************************/
/***************************************************************
 *  File:    ncSACache.cc
 *  Purpose: implementation of special Cache+Hash for Multiplier 
 *  Author:  motsak
 *  Created: 
 *  Version: $Id$
 *******************************************************************/

#define MYTEST 0

#if MYTEST
#define OM_CHECK 4
#define OM_TRACK 5
// these settings must be before "mod2.h" in order to work!!!
#endif


#include "config.h"
#include <misc/auxiliary.h>

#ifndef NDEBUG
#define OUTPUT 1
#else
#define OUTPUT 0
#endif

namespace 
{
  static char* m_Typenames[2] = {"lookup", "store"};
}

#include <coeffs/numbers.h>
#include "coeffrings.h"

#include "nc/ncSACache.h" // for CCacheHash etc classes

#include "monomials/ring.h"
#include "monomials/p_polys.h"



void CGlobalCacheHash::History(const EHistoryType t, const CGlobalCacheHash::CExponent a, const CGlobalCacheHash::CExponent b, const poly p)
{
#if OUTPUT
  Print("History: GlobalPair, Action: %s", m_Typenames[(int)(t)]);
  PrintLn();
  PrintS("Left : "); p_Write(a, GetBasering());
  PrintS("Right: "); p_Write(b, GetBasering());
  if( t == MULT_STORE )
  {
    PrintS("Result: "); p_Write(p, GetBasering());
  }
#endif
}


void  CSpecialPairCacheHash::History(const EHistoryType t, const CSpecialPairCacheHash::CExponent a, const CSpecialPairCacheHash::CExponent b, const poly p)
{
#if OUTPUT
  Print("History: SpecialPair, Action: %s", m_Typenames[(int)(t)]);
  PrintLn();
  Print("Left : %d, Right: %d", a, b);
  PrintLn();
  if( t == MULT_STORE )
  {
    PrintS("Result: "); p_Write(p, GetBasering());
  }
#endif
}

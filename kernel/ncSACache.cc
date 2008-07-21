/****************************************
*  Computer Algebra System SINGULAR     *
****************************************/
/***************************************************************
 *  File:    ncSACache.cc
 *  Purpose: implementation of special Cache+Hash for Multiplier 
 *  Author:  motsak
 *  Created: 
 *  Version: $Id: ncSACache.cc,v 1.4 2008-07-21 00:05:09 motsak Exp $
 *******************************************************************/


#include "mod2.h"

#ifndef NDEBUG

#define MYTEST 1
#define OUTPUT 1

#if MYTEST
#define OM_CHECK 4
#define OM_TRACK 5
#endif

#else

#define MYTEST 0
#define OUTPUT 0

#endif


#include <ncSACache.h> // for CCacheHash etc classes

#include <ring.h>
#include <p_polys.h>

static const char* m_Typenames[2] = {"lookup", "store"};

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

/****************************************
*  Computer Algebra System SINGULAR     *
****************************************/
/***************************************************************
 *  File:    ncSAMult.cc
 *  Purpose: implementation of multiplication in simple NC subalgebras
 *  Author:  motsak
 *  Created: 
 *  Version: $Id: ncSAMult.cc,v 1.2 2008-07-15 16:27:58 motsak Exp $
 *******************************************************************/


#define MYTEST 1
#define OUTPUT 1

#if MYTEST
#define OM_CHECK 4
#define OM_TRACK 5
#endif

#include "mod2.h"

#include <ncSAMult.h> // for CMultiplier etc classes
#include <sca.h> // for SCA




// poly functions defined in p_Procs: ;
static poly gnc_pp_Mult_mm(const poly p, const poly m, const ring r, poly& last)
{
#if OUTPUT  
  Print("gnc_pp_Mult_mm");
  PrintLn();
#endif
  assume( r->GetNC()->GetGlobalMultiplier() != NULL );
  
  return r->GetNC()->GetGlobalMultiplier()->MultiplyPE(p, m);
}

static poly gnc_p_Mult_mm(poly p, const poly m, const ring r)
{
#if OUTPUT  
  Print("gnc_p_Mult_mm");
  PrintLn();
#endif
  assume( r->GetNC()->GetGlobalMultiplier() != NULL );

  return r->GetNC()->GetGlobalMultiplier()->MultiplyPEDestroy(p, m);
}

static poly gnc_mm_Mult_p(const poly m, poly p, const ring r)
{
#if OUTPUT  
  Print("gnc_mm_Mult_p");
  PrintLn();
#endif
  assume( r->GetNC()->GetGlobalMultiplier() != NULL );

  return r->GetNC()->GetGlobalMultiplier()->MultiplyEPDestroy(m, p);
}

static poly gnc_mm_Mult_pp(const poly m, const poly p, const ring r)
{
#if OUTPUT  
  Print("gnc_mm_Mult_pp");
  PrintLn();
#endif
  
  assume( r->GetNC()->GetGlobalMultiplier() != NULL );

  return r->GetNC()->GetGlobalMultiplier()->MultiplyEP(m, p);
}




static void gnc_p_ProcsSet(ring rGR, p_Procs_s* p_Procs = NULL)
{
#if OUTPUT  
  Print("\ngnc_p_ProcsSet()!!!");
  PrintLn();
#endif

  if( p_Procs == NULL )
    p_Procs = rGR->p_Procs;
  
  // "commutative"
  p_Procs->p_Mult_mm  = rGR->p_Procs->p_Mult_mm  = gnc_p_Mult_mm;
  p_Procs->pp_Mult_mm = rGR->p_Procs->pp_Mult_mm = gnc_pp_Mult_mm;

  p_Procs->p_Minus_mm_Mult_qq = rGR->p_Procs->p_Minus_mm_Mult_qq = NULL;

  // non-commutaitve multiplication by monomial from the left
  rGR->GetNC()->p_Procs.mm_Mult_p   = gnc_mm_Mult_p;
  rGR->GetNC()->p_Procs.mm_Mult_pp  = gnc_mm_Mult_pp;

}







bool ncInitSpecialPairMultiplication(ring r)
{
#if OUTPUT  
  Print("ncInitSpecialPairMultiplication(ring), ring: \n");
  rWrite(r);
  PrintLn();
#endif
  assume(rIsPluralRing(r));
  assume(!rIsSCA(r));

  r->GetNC()->GetGlobalMultiplier() = new CGlobalMultiplier(r);

  gnc_p_ProcsSet(r);
  return true;
}


CGlobalMultiplier::CGlobalMultiplier(ring r): CMultiplier<poly>(r)
{
#if OUTPUT  
  Print("CGlobalMultiplier::CGlobalMultiplier(ring)!");
  PrintLn();
#endif

  m_cache = new CGlobalCacheHash(r);
  m_powers = new CPowerMultiplier(r);

}


CGlobalMultiplier::~CGlobalMultiplier()
{
#if OUTPUT  
  Print("CGlobalMultiplier::~CGlobalMultiplier()!");
  PrintLn();
#endif

  delete m_cache;
  delete m_powers;
}



// Exponent * Exponent
// TODO: handle components!!!
poly CGlobalMultiplier::MultiplyEE(const CExponent expLeft, const CExponent expRight)
{
#if OUTPUT  
  Print("CGlobalMultiplier::MultiplyEE(expLeft, expRight)!");
  PrintLn();
#endif

  CCacheHash<poly>::CCacheItem* pLookup;
  int b = m_cache->LookupEE(expLeft, expRight, pLookup);

  // up to now:
  assume( b == -1 );

  // TODO: use PowerMultiplier!!!!
  
  
  // up to now:
  return NULL; 
}

    // Monom * Exponent
poly CGlobalMultiplier::MultiplyME(const poly pMonom, const CExponent expRight)
{
  return MultiplyEE(pMonom, expRight);
}

    // Exponent * Monom
poly CGlobalMultiplier::MultiplyEM(const CExponent expLeft, const poly pMonom)
{
  return MultiplyEE(expLeft, pMonom);
}





// Exponent * Exponent
poly CCommutativeSpecialPairMultiplier::MultiplyEE(const int expLeft, const int expRight)
{
  return NULL;
}

// Monom * Exponent
poly CCommutativeSpecialPairMultiplier::MultiplyME(const poly pMonom, const int expRight)
{
  return NULL;
}

// Exponent * Monom
poly CCommutativeSpecialPairMultiplier::MultiplyEM(const int expLeft, const poly pMonom)
{
  return NULL;
}



// factory method!
CSpecialPairMultiplier* AnalyzePair(const ring r, int i, int j)
{
#if OUTPUT  
  Print("AnalyzePair(ring, i: %d, j: %d)!", i, j);
  PrintLn();
#endif

  const int N = r->N;

  assume(i < j);
  assume(i > 0);
  assume(j <= N);


  const poly c = GetC(r, i, j);
  const poly d = GetD(r, i, j);

#if OUTPUT  
  Print("C_{%d, %d} = ", i, j); p_Write(c, r);
  Print("D_{%d, %d} = ", i, j); p_Write(d, r);
#endif

  if( (d == NULL) && n_IsOne(p_GetCoeff(c, r), r) )
    return new CCommutativeSpecialPairMultiplier(r, i, j);

  return NULL;
}






CPowerMultiplier::CPowerMultiplier(ring r): CMultiplier<CPower>(r)
{
#if OUTPUT  
  Print("CPowerMultiplier::CPowerMultiplier(ring)!");
  PrintLn();
#endif

  m_specialpairs = (CSpecialPairMultiplier**)omAlloc0( ((NVars() * (NVars()-1)) / 2) * sizeof(CSpecialPairMultiplier*) );

  for( int i = 1; i < NVars(); i++ )
    for( int j = i + 1; j <= NVars(); j++ )
      GetPair(i, j) = AnalyzePair(GetBasering(), i, j); // factory method!
}


CPowerMultiplier::~CPowerMultiplier()
{
#if OUTPUT  
  Print("CPowerMultiplier::~CPowerMultiplier()!");
  PrintLn();
#endif

  omFreeSize((ADDRESS)m_specialpairs, ((NVars() * (NVars()-1)) / 2) * sizeof(CSpecialPairMultiplier*) );
}


// Monom * Exponent
// pMonom may NOT be of the form: var(j)^{n}!
poly CPowerMultiplier::MultiplyME(const poly pMonom, const CExponent expRight)
{
  const int j = expRight.Var;
  const int n = expRight.Power;
  
#if OUTPUT  
  Print("CPowerMultiplier::MultiplyME(monom * var(%d)^{%d}!", j, n);
  PrintLn();
#endif

  assume( (j > 0) && (j <= NVars()));

  if( n == 0 )
    return p_Copy(pMonom, GetBasering()); // Copy?!?
  

  int v = NVars();
  int e = p_GetExp(pMonom, v, GetBasering());

  while((v > j) && (e == 0))
    e = p_GetExp(pMonom, --v, GetBasering());

  // TODO: review this!
  if( (e == 0) )
    return expRight.GetPoly(GetBasering());

  if( v == j ) 
  {
    poly p = p_Copy(pMonom, GetBasering());    
    p_SetExp(p, j, e + n, GetBasering());
    return p;
  }
  
  // And now the General Case: v > j!

  poly p = MultiplyEE( CPower(v, e), expRight ); // Easy way!

  --v;
  
  while(v > 0)
  {
    e = p_GetExp(pMonom, v, GetBasering());
    
    if( e > 0 )
      p = MultiplyEPDestroy(CPower(v, e), p);

    --v;
  }

  return p;
}

// Exponent * Monom
// pMonom may NOT be of the form: var(i)^{m}!
poly CPowerMultiplier::MultiplyEM(const CExponent expLeft, const poly pMonom)
{
  // TODO: as above! (difference due to Left/Right semmantics!)
  const int j = expLeft.Var;
  const int n = expLeft.Power;

#if OUTPUT  
  Print("CPowerMultiplier::MultiplyEM(var(%d)^{%d} * monom)!", j, n);
  PrintLn();
#endif

  assume( (j > 0) && (j <= NVars()));

  if( n == 0 )
    return p_Copy(pMonom, GetBasering()); // Copy?!?


  int v = 1; // NVars();
  int e = p_GetExp(pMonom, v, GetBasering());

  while((v < j) && (e == 0))
    e = p_GetExp(pMonom, ++v, GetBasering());

  // TODO: review this!
  if( (e == 0) )
    return expLeft.GetPoly(GetBasering());

  if( v == j ) 
  {
    poly p = p_Copy(pMonom, GetBasering());    
    p_SetExp(p, j, e + n, GetBasering());
    return p;
  }

  // And now the General Case: v > j!

  poly p = MultiplyEE( expLeft, CPower(v, e) ); // Easy way!

  ++v;
  while(v <= NVars())
  {
    e = p_GetExp(pMonom, v, GetBasering());
    
    if( e > 0 )
      p = MultiplyPEDestroy(p, CPower(v, e));
          
    ++v;
  }
  
}


// Exponent * Exponent
// Computes: var(j)^{expLeft} * var(i)^{expRight}
poly CPowerMultiplier::MultiplyEE(const CExponent expLeft, const CExponent expRight)
{
#if OUTPUT  
  Print("CPowerMultiplier::MultiplyEE)!");
  PrintLn();
#endif

  const int i = expRight.Var, j = expLeft.Var;
  const int ei = expRight.Power, ej = expLeft.Power;

#if OUTPUT  
  Print("Input: var(%d)^{%d} * var(%d)^{%d}", j, ej, i, ei);
  PrintLn();
#endif

  poly product;

  if( i >= j )
  {
    // easy!
    product = NULL;
  } else
  {
    assume(1 <= i);
    assume(i <  j);
    assume(j <= NVars());
    assume(ei > 0);
    assume(ej > 0);

    // No Cache Lookup!? :(

    CSpecialPairMultiplier* pSpecialMultiplier = GetPair(i, j);

    poly product = NULL;

    // Special case?
    if( pSpecialMultiplier != NULL )
    {
      assume( pSpecialMultiplier->GetI() == i );
      assume( pSpecialMultiplier->GetJ() == j );
      assume( pSpecialMultiplier->GetBasering() == GetBasering() );

      product = pSpecialMultiplier->MultiplyEE(ej, ei);
    } else
    {
      // Perform general NC Multiplication:
      // TODO

      product = NULL;
    }
  }
  
  return product;  
}






CSpecialPairMultiplier::CSpecialPairMultiplier(ring r, int i, int j):
    CMultiplier<int>(r), m_i(i), m_j(j)
{
  assume(i < j);
  assume(i > 0);
  assume(j <= NVars());
}


// Monom * Exponent
poly CSpecialPairMultiplier::MultiplyME(const poly pMonom, const CExponent expRight)
{
  return MultiplyEE(p_GetExp(pMonom, GetJ(), GetBasering()), expRight);
}

    // Exponent * Monom
poly CSpecialPairMultiplier::MultiplyEM(const CExponent expLeft, const poly pMonom)
{
  return MultiplyEE(expLeft, p_GetExp(pMonom, GetI(), GetBasering()));
}





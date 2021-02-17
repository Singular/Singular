/****************************************
*  Computer Algebra System SINGULAR     *
****************************************/
/***************************************************************
 *  File:    ncSAMult.cc
 *  Purpose: implementation of multiplication in simple NC subalgebras
 *  Author:  motsak
 *  Created:
 *******************************************************************/

#define MYTEST 0

#if MYTEST
#define OM_CHECK 4
#define OM_TRACK 5
// these settings must be before "mod2.h" in order to work!!!
#endif





#include "misc/auxiliary.h"

#ifdef HAVE_PLURAL


#ifndef SING_NDEBUG
#define OUTPUT MYTEST
#else
#define OUTPUT 0
#endif

# define PLURAL_INTERNAL_DECLARATIONS
#include "polys/nc/nc.h"
#include "polys/nc/sca.h"

#include "misc/options.h"
#include "coeffs/numbers.h"


#include "polys/monomials/ring.h"
#include "polys/monomials/p_polys.h"

#include "polys/nc/ncSAMult.h" // for CMultiplier etc classes
// #include "nc/sca.h" // for SCA


namespace
{

// poly functions defined in p_Procs: ;
static poly ggnc_pp_Mult_mm(const poly p, const poly m, const ring r)
{
  if( (p == NULL) || (m == NULL) )
    return NULL;

  assume( (p != NULL) && (m != NULL) && (r != NULL) );

#if OUTPUT
  PrintS("VVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVV ggnc_pp_Mult_mm(p, m) VVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVV ");
  PrintLn();
  PrintS("p: "); p_Write(p, r);
  PrintS("m: "); p_Write(m, r);
#endif
  poly pResult;

  if (p_IsConstant(m, r))
    pResult = __pp_Mult_nn(p, p_GetCoeff(m,r),r);
  else
  {
    CGlobalMultiplier* const pMultiplier = r->GetNC()->GetGlobalMultiplier();
    assume( pMultiplier != NULL );

    poly pMonom = pMultiplier->LM(m, r);
    pResult = pMultiplier->MultiplyPE(p, pMonom);
    p_Delete(&pMonom, r);
    p_Test(pResult, r);
    pResult = __p_Mult_nn(pResult, p_GetCoeff(m, r), r);
  }

#if OUTPUT
  p_Test(pResult, r);

  PrintS("ggnc_pp_Mult_mm(p, m) => "); p_Write(pResult, r);
  PrintS("p: "); p_Write(p, r);
  PrintS("m: "); p_Write(m, r);
  PrintS("^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^ ");
  PrintLn();
#endif

  return pResult;

}

static poly ggnc_p_Mult_mm(poly p, const poly m, const ring r)
{
  if( (p == NULL) || (m == NULL) )
  {
    p_Delete(&p, r);
    return NULL;
  }

  assume( (p != NULL) && (m != NULL) && (r != NULL) );

#if OUTPUT
  PrintS("VVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVV ggnc_p_Mult_mm(p, m) VVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVV ");
  PrintLn();
  PrintS("p: ");
  p_Write(p, r);
  PrintS("m: ");
  p_Write(m, r);
#endif

  poly pResult;

  if (p_IsConstant(m, r))
    pResult = __p_Mult_nn(p, p_GetCoeff(m,r),r);
  else
  {
    CGlobalMultiplier* const pMultiplier = r->GetNC()->GetGlobalMultiplier();
    assume( pMultiplier != NULL );

    poly pMonom = pMultiplier->LM(m, r);
    pResult = pMultiplier->MultiplyPEDestroy(p, pMonom);
    p_Delete(&pMonom, r);
    p_Test(pResult, r);
    pResult = __p_Mult_nn(pResult, p_GetCoeff(m, r), r);
  }

#if OUTPUT
  p_Test(pResult, r);

  PrintS("ggnc_p_Mult_mm(p, m) => "); p_Write(pResult, r);
//  PrintS("p: "); p_Write(p, r);
  PrintS("m: "); p_Write(m, r);
  PrintS("^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^ ");
  PrintLn();
#endif

  return pResult;

}

/* m*p */
static poly ggnc_p_mm_Mult(poly p, const poly m, const ring r)
{
  if( (p == NULL) || (m == NULL) )
  {
    p_Delete(&p, r);
    return NULL;
  }

  assume( (p != NULL) && (m != NULL) && (r != NULL) );

  p_Test(m, r);
  p_Test(p, r);

#if OUTPUT
  PrintS("VVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVV ggnc_p_mm_Mult(p,m) VVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVV ");
  PrintLn();
  PrintS("m: "); p_Write(m, r);
  PrintS("p: "); p_Write(p, r);
#endif

  poly pResult;

  if (p_IsConstant(m, r))
    pResult = __p_Mult_nn(p, p_GetCoeff(m,r),r);
  else
  {
    CGlobalMultiplier* const pMultiplier = r->GetNC()->GetGlobalMultiplier();
    assume( pMultiplier != NULL );

    poly pMonom = pMultiplier->LM(m, r);
    pResult = pMultiplier->MultiplyEPDestroy(pMonom, p);
    p_Delete(&pMonom, r);
    p_Test(pResult, r);
    pResult = __p_Mult_nn(pResult, p_GetCoeff(m, r), r);
  }

#if OUTPUT
  p_Test(pResult, r);

  PrintS("ggnc_p_mm_Mult(p,m) => "); p_Write(pResult, r);
//  PrintS("p: "); p_Write(p, r);
  PrintS("m: "); p_Write(m, r);
  PrintS("^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^ ");
  PrintLn();
#endif

  return pResult;
}

static poly ggnc_pp_mm_Mult(const poly p, const poly m, const ring r)
{
  if( (p == NULL) || (m == NULL) )
  {
    return NULL;
  }

  assume( (p != NULL) && (m != NULL) && (r != NULL) );

  p_Test(m, r);
  p_Test(p, r);

#if OUTPUT
  PrintS("VVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVV ggnc_pp_mm_Mult(m, p) VVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVV ");
  PrintLn();
  PrintS("m: "); p_Write(m, r);
  PrintS("p: "); p_Write(p, r);
#endif

  poly pResult;

  if (p_IsConstant(m, r))
    pResult = __pp_Mult_nn(p, p_GetCoeff(m,r),r);
  else
  {
    CGlobalMultiplier* const pMultiplier = r->GetNC()->GetGlobalMultiplier();
    assume( pMultiplier != NULL );

    poly pMonom = pMultiplier->LM(m, r);
    pResult = pMultiplier->MultiplyEP(pMonom, p);
    p_Delete(&pMonom, r);
    p_Test(pResult, r);
    pResult = __p_Mult_nn(pResult, p_GetCoeff(m, r), r);
  }

#if OUTPUT
  p_Test(pResult, r);

  PrintS("ggnc_pp_mm_Mult(m, p) => "); p_Write(pResult, r);
  PrintS("p: "); p_Write(p, r);
  PrintS("m: "); p_Write(m, r);
  PrintS("^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^ ");
  PrintLn();
#endif

  return pResult;
}

static void ggnc_p_ProcsSet(ring rGR, p_Procs_s* p_Procs)
{
#if OUTPUT
  PrintS("|ggnc_p_ProcsSet()");
  PrintLn();
#endif

  assume( p_Procs != NULL );

  // "commutative"
  p_Procs->p_Mult_mm  = rGR->p_Procs->p_Mult_mm  = ggnc_p_Mult_mm;
  p_Procs->pp_Mult_mm = rGR->p_Procs->pp_Mult_mm = ggnc_pp_Mult_mm;

  p_Procs->p_Minus_mm_Mult_qq = rGR->p_Procs->p_Minus_mm_Mult_qq = NULL;

  // non-commutaitve multiplication by monomial from the left
  rGR->p_Procs->p_mm_Mult   = ggnc_p_mm_Mult;
  rGR->p_Procs->pp_mm_Mult  = ggnc_pp_mm_Mult;

}

}

BOOLEAN ncInitSpecialPairMultiplication(ring r)
{
#if OUTPUT
  PrintS("ncInitSpecialPairMultiplication(ring), ring: \n");
  rWrite(r, TRUE);
  PrintLn();
#endif

  if(!rIsPluralRing(r))// ; // :(((
    return TRUE;

  if(rIsSCA(r))
    return TRUE;

  if( r->GetNC()->GetGlobalMultiplier() != NULL )
  {
    WarnS("Already defined!");
    return TRUE;
  }

  r->GetNC()->GetGlobalMultiplier() = new CGlobalMultiplier(r);

  ggnc_p_ProcsSet(r, r->p_Procs);
  return FALSE; // ok!
}


CGlobalMultiplier::CGlobalMultiplier(ring r):
    CMultiplier<poly>(r), m_RingFormulaMultiplier(GetFormulaPowerMultiplier(r))
{
#if OUTPUT
  PrintS("CGlobalMultiplier::CGlobalMultiplier(ring)!");
  PrintLn();
#endif

//  m_cache = new CGlobalCacheHash(r);
  m_powers = new CPowerMultiplier(r);
}


CGlobalMultiplier::~CGlobalMultiplier()
{
#if OUTPUT
  PrintS("CGlobalMultiplier::~CGlobalMultiplier()!");
  PrintLn();
#endif

//  delete m_cache;
  delete m_powers;

  // we cannot delete m_RingFormulaMultiplier as it belongs to the ring!
}



// Exponent * Exponent
// TODO: handle components!!!
poly CGlobalMultiplier::MultiplyEE(const CGlobalMultiplier::CExponent expLeft, const CGlobalMultiplier::CExponent expRight)
{

  const ring r = GetBasering();

#if OUTPUT
  PrintS("CGlobalMultiplier::MultiplyEE(expLeft, expRight)!");
  PrintLn();
  PrintS("expL: "); p_Write(expLeft, GetBasering());
  PrintS("expR: "); p_Write(expRight, GetBasering());
#endif

//  CCacheHash<poly>::CCacheItem* pLookup;
//
//  int b = m_cache->LookupEE(expLeft, expRight, pLookup);
//  // TODO!!!
//
//  // up to now:
//  assume( b == -1 );

  // TODO: use PowerMultiplier!!!!

  poly product = NULL;

  const int N = NVars();
  int j = N;
  int i = 1;

  int ej = p_GetExp(expLeft, j, r);
  int ei = p_GetExp(expRight, i, r);

  while( (i < j) && !((ej != 0) && (ei != 0)) )
  {
    if( ei == 0 )
      ei = p_GetExp(expRight, ++i, r);

    if( ej == 0 )
      ej = p_GetExp(expLeft, --j, r);
  }


#if OUTPUT
  PrintS("<CGlobalMultiplier::MultiplyEE>");
  PrintLn();
  Print("i: %d, j: %d", i, j);
  PrintLn();
  Print("ei: %d, ej: %d", ei, ej);
  PrintLn();
#endif


  // |  expLeft   | * |  expRight  |
  // |<<<< ej 0..0| , |0..0 ei >>>>|
  // |<<<<  j <<<N| , |1>>>  i >>>>|

  if( i >= j ) // BUG here!!!???
  {
    // either i == j or i = j + 1 => commutative multiple!
    // TODO: it can be done more efficiently! ()
    product = p_Head(expRight, r);

  // |  expLeft     | * |  expRight   |
  // |<<<< ej 0....0| , |0..00 ei >>>>|
  // |<<<<  j i <<<N| , |1>>>j  i >>>>|

    if(i > j)
    {
      --i;
      ei = 0;
    }

    if( i == j )
    {
      if( ej != 0 )
        p_SetExp(product, i, ei + ej, r);
    }

    --i;

    for(; i > 0; --i)
    {
      const int e = p_GetExp(expLeft, i, r);

      if( e > 0 )
        p_SetExp(product, i, e, r);
    }

    p_Setm(product, r);

  } else
  { // i < j, ei != 0, ej != 0

    Enum_ncSAType PairType = _ncSA_notImplemented;

    if( m_RingFormulaMultiplier != NULL )
      PairType = m_RingFormulaMultiplier->GetPair(i, j);


    if( PairType == _ncSA_notImplemented )
      product = m_powers->MultiplyEE( CPower(j, ej), CPower(i, ei) );
//    return ggnc_uu_Mult_ww_vert(i, a, j, b, r);
    else
 //    return m_RingFormulaMultiplier->Multiply(j, i, b, a);
      product = CFormulaPowerMultiplier::Multiply( PairType, i, j, ei, ej, GetBasering());


#if OUTPUT
    PrintS("<CGlobalMultiplier::MultiplyEE> ==> ");
    PrintLn();
    Print("i: %d, j: %d", i, j);
    PrintLn();
    Print("ei: %d, ej: %d", ei, ej);
    PrintLn();
    PrintS("<product>: "); p_Write(product, GetBasering());
#endif


    // TODO: Choose some multiplication strategy!!!

    while( (product != NULL) && !((i == NVars()) && (j == 1)) )
    {

      // make some choice here!:

      if( i < NVars() )
      {
        ei = p_GetExp(expRight, ++i, r);

        while( (ei == 0) && (i < NVars()) )
          ei = p_GetExp(expRight, ++i, r);

        if( ei != 0 )
          product = m_powers->MultiplyPEDestroy(product, CPower(i, ei));
      }

      if( j > 1 )
      {
        ej = p_GetExp(expLeft, --j, r);

        while( (ej == 0) && (1 < j) )
          ej = p_GetExp(expLeft, --j, r);

        if( ej != 0 )
          product = m_powers->MultiplyEPDestroy(CPower(j, ej), product);
      }


#if OUTPUT
      PrintS("<CGlobalMultiplier::MultiplyEE> ==> ");
      PrintLn();
      Print("i: %d, j: %d", i, j);
      PrintLn();
      Print("ei: %d, ej: %d", ei, ej);
      PrintLn();
      PrintS("<product>: "); p_Write(product, GetBasering());
#endif

    }

  }

//  // TODO!
//
//  m_cache->StoreEE( expLeft, expRight, product);
//  // up to now:
  return product;
}

    // Monom * Exponent
poly CGlobalMultiplier::MultiplyME(const poly pMonom, const CGlobalMultiplier::CExponent expRight)
{
#if OUTPUT
  PrintS("CGlobalMultiplier::MultiplyME(monom, expR)!");
  PrintLn();
  PrintS("Monom: "); p_Write(pMonom, GetBasering());
  PrintS("expR: "); p_Write(expRight, GetBasering());
#endif

  return MultiplyEE(pMonom, expRight);
}

    // Exponent * Monom
poly CGlobalMultiplier::MultiplyEM(const CGlobalMultiplier::CExponent expLeft, const poly pMonom)
{
#if OUTPUT
  PrintS("CGlobalMultiplier::MultiplyEM(expL, monom)!");
  PrintLn();
  PrintS("expL: "); p_Write(expLeft, GetBasering());
  PrintS("Monom: "); p_Write(pMonom, GetBasering());
#endif

  return MultiplyEE(expLeft, pMonom);
}





///////////////////////////////////////////////////////////////////////////////////////////
CCommutativeSpecialPairMultiplier::CCommutativeSpecialPairMultiplier(ring r, int i, int j):
    CSpecialPairMultiplier(r, i, j)
{
#if OUTPUT
  Print("CCommutativeSpecialPairMultiplier::CCommutativeSpecialPairMultiplier(ring, i: %d, j: %d)!", i, j);
  PrintLn();
#endif
}


CCommutativeSpecialPairMultiplier::~CCommutativeSpecialPairMultiplier()
{
#if OUTPUT
  PrintS("CCommutativeSpecialPairMultiplier::~CCommutativeSpecialPairMultiplier()");
  PrintLn();
#endif
}

// Exponent * Exponent
poly CCommutativeSpecialPairMultiplier::MultiplyEE(const int expLeft, const int expRight)
{
#if OUTPUT
  Print("CCommutativeSpecialPairMultiplier::MultiplyEE(var(%d)^{%d}, var(%d)^{%d})!", GetJ(), expLeft, GetI(), expRight);
  PrintLn();
#endif

  const ring r = GetBasering();

  return CFormulaPowerMultiplier::ncSA_1xy0x0y0(GetI(), GetJ(), expRight, expLeft, r);
}

///////////////////////////////////////////////////////////////////////////////////////////
CAntiCommutativeSpecialPairMultiplier::CAntiCommutativeSpecialPairMultiplier(ring r, int i, int j):
                CSpecialPairMultiplier(r, i, j)
{
#if OUTPUT
        Print("CAntiCommutativeSpecialPairMultiplier::CAntiCommutativeSpecialPairMultiplier(ring, i: %d, j: %d)!", i, j);
        PrintLn();
#endif
}


CAntiCommutativeSpecialPairMultiplier::~CAntiCommutativeSpecialPairMultiplier()
{
#if OUTPUT
        PrintS("CAntiCommutativeSpecialPairMultiplier::~CAntiCommutativeSpecialPairMultiplier()");
        PrintLn();
#endif
}

// Exponent * Exponent
poly CAntiCommutativeSpecialPairMultiplier::MultiplyEE(const int expLeft, const int expRight)
{
#if OUTPUT
        Print("CAntiCommutativeSpecialPairMultiplier::MultiplyEE(var(%d)^{%d}, var(%d)^{%d})!", GetJ(), expLeft, GetI(), expRight);
        PrintLn();
#endif

        const ring r = GetBasering();

        return CFormulaPowerMultiplier::ncSA_Mxy0x0y0(GetI(), GetJ(), expRight, expLeft, r);
}

///////////////////////////////////////////////////////////////////////////////////////////
CQuasiCommutativeSpecialPairMultiplier::CQuasiCommutativeSpecialPairMultiplier(ring r, int i, int j, number q):
                CSpecialPairMultiplier(r, i, j), m_q(q)
{
#if OUTPUT
        Print("CQuasiCommutativeSpecialPairMultiplier::CQuasiCommutativeSpecialPairMultiplier(ring, i: %d, j: %d, q)!", i, j);
        PrintLn();
        PrintS("Parameter q: ");
        n_Write(q, r);
#endif
}


CQuasiCommutativeSpecialPairMultiplier::~CQuasiCommutativeSpecialPairMultiplier()
{
#if OUTPUT
        PrintS("CQuasiCommutativeSpecialPairMultiplier::~CQuasiCommutativeSpecialPairMultiplier()");
        PrintLn();
#endif
}

// Exponent * Exponent
poly CQuasiCommutativeSpecialPairMultiplier::MultiplyEE(const int expLeft, const int expRight)
{
#if OUTPUT
        Print("CQuasiCommutativeSpecialPairMultiplier::MultiplyEE(var(%d)^{%d}, var(%d)^{%d})!", GetJ(), expLeft, GetI(), expRight);
        PrintLn();
#endif

        const ring r = GetBasering();

        return CFormulaPowerMultiplier::ncSA_Qxy0x0y0(GetI(), GetJ(), expRight, expLeft, m_q, r);
}


///////////////////////////////////////////////////////////////////////////////////////////
CWeylSpecialPairMultiplier::CWeylSpecialPairMultiplier(ring r, int i, int j, number g):
    CSpecialPairMultiplier(r, i, j), m_g(g)
{
#if OUTPUT
  Print("CWeylSpecialPairMultiplier::CWeylSpecialPairMultiplier(ring, i: %d, j: %d, g)!", i, j);
  PrintLn();
  PrintS("Parameter g: ");
  n_Write(g, r);
#endif
}


CWeylSpecialPairMultiplier::~CWeylSpecialPairMultiplier()
{
#if OUTPUT
  PrintS("CWeylSpecialPairMultiplier::~CWeylSpecialPairMultiplier()");
  PrintLn();
#endif
}

// Exponent * Exponent
poly CWeylSpecialPairMultiplier::MultiplyEE(const int expLeft, const int expRight)
{
#if OUTPUT
  Print("CWeylSpecialPairMultiplier::MultiplyEE(var(%d)^{%d}, var(%d)^{%d})!", GetJ(), expLeft, GetI(), expRight);
  PrintLn();
#endif
  // Char == 0, otherwise - problem!


  const ring r = GetBasering();

  assume( expLeft*expRight > 0 );

  return CFormulaPowerMultiplier::ncSA_1xy0x0yG(GetI(), GetJ(), expRight, expLeft, m_g, r);
}

///////////////////////////////////////////////////////////////////////////////////////////
CHWeylSpecialPairMultiplier::CHWeylSpecialPairMultiplier(ring r, int i, int j, int k):
    CSpecialPairMultiplier(r, i, j), m_k(k)
{
#if OUTPUT
  Print("CHWeylSpecialPairMultiplier::CHWeylSpecialPairMultiplier(ring, i: %d, j: %d, k: %d)!", i, j, k);
  PrintLn();
#endif
}


CHWeylSpecialPairMultiplier::~CHWeylSpecialPairMultiplier()
{
#if OUTPUT
  PrintS("CHWeylSpecialPairMultiplier::~CHWeylSpecialPairMultiplier()");
  PrintLn();
#endif
}

// Exponent * Exponent
poly CHWeylSpecialPairMultiplier::MultiplyEE(const int expLeft, const int expRight)
{
#if OUTPUT
  Print("CHWeylSpecialPairMultiplier::MultiplyEE(var(%d)^{%d}, var(%d)^{%d})!", GetJ(), expLeft, GetI(), expRight);
  PrintLn();
#endif
  // Char == 0, otherwise - problem!


  const ring r = GetBasering();

  assume( expLeft*expRight > 0 );

  return CFormulaPowerMultiplier::ncSA_1xy0x0yT2(GetI(), GetJ(), expRight, expLeft, m_k, r);
}


///////////////////////////////////////////////////////////////////////////////////////////
CShiftSpecialPairMultiplier::CShiftSpecialPairMultiplier(ring r, int i, int j, int s, number c):
    CSpecialPairMultiplier(r, i, j), m_shiftCoef(c), m_shiftVar(s)
{
#if OUTPUT
  Print("CShiftSpecialPairMultiplier::CShiftSpecialPairMultiplier(ring, i: %d, j: %d, s: %d, c)!", i, j, s);
  PrintLn();
  PrintS("Parameter c: "); n_Write(c, r);
#endif
}


CShiftSpecialPairMultiplier::~CShiftSpecialPairMultiplier()
{
#if OUTPUT
  PrintS("CShiftSpecialPairMultiplier::~CShiftSpecialPairMultiplier()");
  PrintLn();
#endif
}

// Exponent * Exponent
poly CShiftSpecialPairMultiplier::MultiplyEE(const int expLeft, const int expRight)
{
#if OUTPUT
  Print("CShiftSpecialPairMultiplier::MultiplyEE(var(%d)^{%d}, var(%d)^{%d})!", GetJ(), expLeft, GetI(), expRight);
  PrintLn();
#endif
  // Char == 0, otherwise - problem!

  assume( expLeft*expRight > 0 );

  const ring r = GetBasering();

  if( m_shiftVar != GetI() ) // YX = XY + b*Y?
    return CFormulaPowerMultiplier::ncSA_1xy0xBy0(GetI(), GetJ(), expRight, expLeft, m_shiftCoef, r); // case: (1, 0, beta, 0, 0)
  else
    return CFormulaPowerMultiplier::ncSA_1xyAx0y0(GetI(), GetJ(), expRight, expLeft, m_shiftCoef, r); // case: (1, alpha, 0, 0)

}



///////////////////////////////////////////////////////////////////////////////////////////
CExternalSpecialPairMultiplier::CExternalSpecialPairMultiplier(ring r, int i, int j, Enum_ncSAType type):
    CSpecialPairMultiplier(r, i, j), m_ncSAtype(type)
{
#if OUTPUT
  Print("CExternalSpecialPairMultiplier::CExternalSpecialPairMultiplier(ring, i: %d, j: %d, type: %d, c)!", i, j, (int)type);
  PrintLn();
#endif
}


CExternalSpecialPairMultiplier::~CExternalSpecialPairMultiplier()
{
#if OUTPUT
  PrintS("CExternalSpecialPairMultiplier::~CExternalSpecialPairMultiplier()");
  PrintLn();
#endif
}

// Exponent * Exponent
poly CExternalSpecialPairMultiplier::MultiplyEE(const int expLeft, const int expRight)
{
#if OUTPUT
  Print("CExternalSpecialPairMultiplier::MultiplyEE(var(%d)^{%d}, var(%d)^{%d})!", GetJ(), expLeft, GetI(), expRight);
  PrintLn();
#endif
  // Char == 0, otherwise - problem!

  assume( expLeft*expRight > 0 );

  const ring r = GetBasering();

  return CFormulaPowerMultiplier::Multiply(m_ncSAtype, GetI(), GetJ(), expRight, expLeft, r);

}



///////////////////////////////////////////////////////////////////////////////////////////

// factory method!
CSpecialPairMultiplier* AnalyzePair(const ring r, int i, int j)
{
#if OUTPUT
  Print("AnalyzePair(ring, i: %d, j: %d)!", i, j);
  PrintLn();
#endif

  Enum_ncSAType type = CFormulaPowerMultiplier::AnalyzePair(r, i, j);

  if( type == _ncSA_notImplemented ) return NULL;


  // last possibility:
  return new CExternalSpecialPairMultiplier(r, i, j, type); // For tests!


  if( type == _ncSA_1xy0x0y0 )
    return new CCommutativeSpecialPairMultiplier(r, i, j);

  if( type == _ncSA_Mxy0x0y0 )
    return new CAntiCommutativeSpecialPairMultiplier(r, i, j);

  if( type == _ncSA_Qxy0x0y0 )
  {
    const number q = p_GetCoeff(GetC(r, i, j), r);
    return new CQuasiCommutativeSpecialPairMultiplier(r, i, j, q);
  }

  const poly d = GetD(r, i, j);

  assume( d != NULL );
  assume( pNext(d) == NULL );

  const number g = p_GetCoeff(d, r);

  if( type == _ncSA_1xy0x0yG ) // Weyl
    return new CWeylSpecialPairMultiplier(r, i, j, g);

  if( type == _ncSA_1xyAx0y0 ) // Shift 1
    return new CShiftSpecialPairMultiplier(r, i, j, i, g);

  if( type == _ncSA_1xy0xBy0 ) // Shift 2
    return new CShiftSpecialPairMultiplier(r, i, j, j, g);

  if( type == _ncSA_1xy0x0yT2 ) // simple homogenized Weyl algebra
    return new CHWeylSpecialPairMultiplier(r, i, j, p_IsPurePower(d, r));

}






CPowerMultiplier::CPowerMultiplier(ring r): CMultiplier<CPower>(r)
{
#if OUTPUT
  PrintS("CPowerMultiplier::CPowerMultiplier(ring)!");
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
  PrintS("CPowerMultiplier::~CPowerMultiplier()!");
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

  const ring r = GetBasering();

#if OUTPUT
  Print("CPowerMultiplier::MultiplyME(monom * var(%d)^{%d})!", j, n);
  PrintLn();
  PrintS("Monom: "); p_Write(pMonom, r);
#endif

  assume( (j > 0) && (j <= NVars()));

  if( n == 0 )
    return p_Head(pMonom, r); // Copy?!?


  int v = NVars();
  int e = p_GetExp(pMonom, v, r);

  while((v > j) && (e == 0))
    e = p_GetExp(pMonom, --v, r);

  // TODO: review this!
  if( v == j )
  {
    poly p = p_Head(pMonom, r);
    p_SetExp(p, v, e + n, r);
    p_Setm(p, r);

    return p;
  }

  assume( v > j );
  assume( e > 0 );

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

#if OUTPUT
  PrintS("CPowerMultiplier::MultiplyME() ===> ");
  p_Write(p, GetBasering());
#endif

  return p;
}

// Exponent * Monom
// pMonom may NOT be of the form: var(i)^{m}!
poly CPowerMultiplier::MultiplyEM(const CExponent expLeft, const poly pMonom)
{
  const ring r = GetBasering();

  // TODO: as above! (difference due to Left/Right semmantics!)
  const int j = expLeft.Var;
  const int n = expLeft.Power;

#if OUTPUT
  Print("CPowerMultiplier::MultiplyEM(var(%d)^{%d} * monom)!", j, n);
  PrintLn();
  PrintS("Monom: "); p_Write(pMonom, r);
#endif

  assume( (j > 0) && (j <= NVars()));

  if( n == 0 )
    return p_Head(pMonom, r); // Copy?!?


  int v = 1; // NVars();
  int e = p_GetExp(pMonom, v, r);

  while((v < j) && (e == 0))
    e = p_GetExp(pMonom, ++v, r);

  if( v == j )
  {
    poly p = p_Head(pMonom, r);
    p_SetExp(p, j, e + n, r);
    p_Setm(p, r);

    return p;
  }

  assume( v < j );
  assume( e > 0 );


  // And now the General Case: v > j!

  poly p = MultiplyEE( expLeft, CPower(v, e) ); // Easy way!

  ++v;

  while(v <= NVars())
  {
    e = p_GetExp(pMonom, v, r);

    if( e > 0 )
      p = MultiplyPEDestroy(p, CPower(v, e));

    ++v;
  }

#if OUTPUT
  PrintS("CPowerMultiplier::MultiplyEM() ===> ");
  p_Write(p, r);
#endif

  return p;

}


// Exponent * Exponent
// Computes: var(j)^{expLeft} * var(i)^{expRight}
poly CPowerMultiplier::MultiplyEE(const CExponent expLeft, const CExponent expRight)
{
#if OUTPUT
  PrintS("CPowerMultiplier::MultiplyEE)!");
  PrintLn();
#endif

  const int i = expRight.Var, j = expLeft.Var;
  const int ei = expRight.Power, ej = expLeft.Power;

#if OUTPUT
  Print("Input: var(%d)^{%d} * var(%d)^{%d}", j, ej, i, ei);
  PrintLn();
#endif

  assume(1 <= i);
  assume(j <= NVars());
  assume(1 <= j);
  assume(i <= NVars());
  assume(ei > 0);
  assume(ej > 0);

  if( i >= j )
  {
    const ring r = GetBasering();

    poly product = p_One(r);
    p_SetExp(product, j, ej, r);
    p_SetExp(product, i, ei, r);
    p_Setm(product, r);

    return product;

  } else
  {
    assume(i <  j);

    // No Cache Lookup!? :(

    CSpecialPairMultiplier* pSpecialMultiplier = GetPair(i, j);

    // Special case?
    if( pSpecialMultiplier != NULL )
    {
      assume( pSpecialMultiplier->GetI() == i );
      assume( pSpecialMultiplier->GetJ() == j );
      assume( pSpecialMultiplier->GetBasering() == GetBasering() );

      return pSpecialMultiplier->MultiplyEE(ej, ei);
    } else
    {
      // Perform general NC Multiplication:
      // TODO

      WerrorS("Sorry the general case is not implemented this way yet!!!");
      assume(0);

      // poly product = NULL;
    }
  }

  return NULL;
}






CSpecialPairMultiplier::CSpecialPairMultiplier(ring r, int i, int j):
    CMultiplier<int>(r), m_i(i), m_j(j)
{
#if OUTPUT
  Print("CSpecialPairMultiplier::CSpecialPairMultiplier(ring, i: %d, j: %d)!", i, j);
  PrintLn();
#endif

  assume(i < j);
  assume(i > 0);
  assume(j <= NVars());
}


CSpecialPairMultiplier::~CSpecialPairMultiplier()
{
#if OUTPUT
  PrintS("CSpecialPairMultiplier::~CSpecialPairMultiplier()!");
  PrintLn();
#endif
}



// Monom * Exponent
poly CSpecialPairMultiplier::MultiplyME(const poly pMonom, const CExponent expRight)
{
#if OUTPUT
  Print("CSpecialPairMultiplier::MultiplyME(monom, var(%d)^{%d})!", GetI(), expRight);
  PrintLn();
  PrintS("Monom: "); p_Write(pMonom, GetBasering());
#endif

  return MultiplyEE(p_GetExp(pMonom, GetJ(), GetBasering()), expRight);
}

    // Exponent * Monom
poly CSpecialPairMultiplier::MultiplyEM(const CExponent expLeft, const poly pMonom)
{
#if OUTPUT
  Print("CSpecialPairMultiplier::MultiplyEM(var(%d)^{%d}, monom)!", GetJ(), expLeft);
  PrintLn();
  PrintS("Monom: "); p_Write(pMonom, GetBasering());
#endif

  return MultiplyEE(expLeft, p_GetExp(pMonom, GetI(), GetBasering()));
}

template class CMultiplier<CPower>;
template class CMultiplier<int>;
template class CMultiplier<spolyrec*>;


#endif

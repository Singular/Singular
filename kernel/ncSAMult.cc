/****************************************
*  Computer Algebra System SINGULAR     *
****************************************/
/***************************************************************
 *  File:    ncSAMult.cc
 *  Purpose: implementation of multiplication in simple NC subalgebras
 *  Author:  motsak
 *  Created: 
 *  Version: $Id: ncSAMult.cc,v 1.5 2008-07-21 00:05:09 motsak Exp $
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


#include <ncSAMult.h> // for CMultiplier etc classes
#include <sca.h> // for SCA




// poly functions defined in p_Procs: ;
static poly gnc_pp_Mult_mm(const poly p, const poly m, const ring r, poly& last)
{
  assume( (p != NULL) && (m != NULL) && (r != NULL) );

#if OUTPUT  
  Print("VVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVV gnc_pp_Mult_mm(p, m) VVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVV ");
  PrintLn();
  PrintS("p: "); p_Write(p, r);    
  PrintS("m: "); p_Write(m, r);      
#endif

  
  CGlobalMultiplier* const pMultiplier = r->GetNC()->GetGlobalMultiplier();
  assume( pMultiplier != NULL );

  poly pMonom = pMultiplier->LM(m, r);
  poly pResult = pMultiplier->MultiplyPE(p, pMonom);
  p_Delete(&pMonom, r);
  p_Test(pResult, r);
  pResult = p_Mult_nn(pResult, p_GetCoeff(m, r), r);
  p_Test(pResult, r);

#if OUTPUT  
  Print("gnc_pp_Mult_mm(p, m) => "); p_Write(pResult, r);
  PrintS("p: "); p_Write(p, r);    
  PrintS("m: "); p_Write(m, r);      
  Print("^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^ ");
  PrintLn();
#endif

  return pResult;

}

static poly gnc_p_Mult_mm(poly p, const poly m, const ring r)
{
  assume( (p != NULL) && (m != NULL) && (r != NULL) );

#if OUTPUT  
  Print("VVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVV gnc_p_Mult_mm(p, m) VVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVV ");
  PrintLn();
  PrintS("p: ");
  p_Write(p, r);    
  PrintS("m: ");
  p_Write(m, r);  
#endif
  
  CGlobalMultiplier* const pMultiplier = r->GetNC()->GetGlobalMultiplier();
  assume( pMultiplier != NULL );

  poly pMonom = pMultiplier->LM(m, r);
  poly pResult = pMultiplier->MultiplyPEDestroy(p, pMonom);
  p_Delete(&pMonom, r);
  p_Test(pResult, r);
  pResult = p_Mult_nn(pResult, p_GetCoeff(m, r), r);
  p_Test(pResult, r);

#if OUTPUT  
  Print("gnc_p_Mult_mm(p, m) => "); p_Write(pResult, r);      
//  PrintS("p: "); p_Write(p, r);    
  PrintS("m: "); p_Write(m, r);      
  Print("^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^ ");
  PrintLn();
#endif
  
  return pResult;

}

static poly gnc_mm_Mult_p(const poly m, poly p, const ring r)
{
  assume( (p != NULL) && (m != NULL) && (r != NULL) );

  p_Test(m, r);
  p_Test(p, r);

#if OUTPUT  
  Print("VVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVV gnc_mm_Mult_p(m, p) VVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVV ");
  PrintLn();
  PrintS("m: "); p_Write(m, r);      
  PrintS("p: "); p_Write(p, r);    
#endif
  CGlobalMultiplier* const pMultiplier = r->GetNC()->GetGlobalMultiplier();
  assume( pMultiplier != NULL );

  poly pMonom = pMultiplier->LM(m, r);
  poly pResult = pMultiplier->MultiplyEPDestroy(pMonom, p);
  p_Delete(&pMonom, r);
  p_Test(pResult, r);
  pResult = p_Mult_nn(pResult, p_GetCoeff(m, r), r);
  p_Test(pResult, r);

  
#if OUTPUT  
  Print("gnc_mm_Mult_p(m, p) => "); p_Write(pResult, r);      
//  PrintS("p: "); p_Write(p, r);    
  PrintS("m: "); p_Write(m, r);      
  Print("^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^ ");
  PrintLn();
#endif
  
  return pResult;
}

static poly gnc_mm_Mult_pp(const poly m, const poly p, const ring r)
{
  assume( (p != NULL) && (m != NULL) && (r != NULL) );

  p_Test(m, r);
  p_Test(p, r);
  
#if OUTPUT  
  Print("VVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVV gnc_mm_Mult_pp(m, p) VVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVV ");
  PrintLn();
  PrintS("m: "); p_Write(m, r);      
  PrintS("p: "); p_Write(p, r);    
#endif
  

  CGlobalMultiplier* const pMultiplier = r->GetNC()->GetGlobalMultiplier();
  assume( pMultiplier != NULL );

  poly pMonom = pMultiplier->LM(m, r);
  poly pResult = pMultiplier->MultiplyEP(pMonom, p);
  p_Delete(&pMonom, r);
  p_Test(pResult, r);
  pResult = p_Mult_nn(pResult, p_GetCoeff(m, r), r);
  p_Test(pResult, r);

#if OUTPUT  
  Print("gnc_mm_Mult_pp(m, p) => "); p_Write(pResult, r);      
  PrintS("p: "); p_Write(p, r);    
  PrintS("m: "); p_Write(m, r);      
  Print("^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^ ");
  PrintLn();
#endif
  
  return pResult;
}




static void gnc_p_ProcsSet(ring rGR, p_Procs_s* p_Procs = NULL)
{
#if OUTPUT  
  Print("|gnc_p_ProcsSet()");
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
poly CGlobalMultiplier::MultiplyEE(const CGlobalMultiplier::CExponent expLeft, const CGlobalMultiplier::CExponent expRight)
{

  const ring r = GetBasering();

#if OUTPUT  
  Print("CGlobalMultiplier::MultiplyEE(expLeft, expRight)!");
  PrintLn();
  PrintS("expL: "); p_Write(expLeft, GetBasering());    
  PrintS("expR: "); p_Write(expRight, GetBasering());    
#endif

  CCacheHash<poly>::CCacheItem* pLookup;
  
  int b = m_cache->LookupEE(expLeft, expRight, pLookup);
  // TODO!!!

  // up to now:
  assume( b == -1 );

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
  Print("<CGlobalMultiplier::MultiplyEE>");
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
    product = m_powers->MultiplyEE( CPower(j, ej), CPower(i, ei) );

#if OUTPUT  
    Print("<CGlobalMultiplier::MultiplyEE> ==> ");
    PrintLn();
    Print("i: %d, j: %d", i, j); 
    PrintLn();
    Print("ei: %d, ej: %d", ei, ej); 
    PrintLn();
    Print("<product>: "); p_Write(product, GetBasering());  
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
      Print("<CGlobalMultiplier::MultiplyEE> ==> ");
      PrintLn();
      Print("i: %d, j: %d", i, j); 
      PrintLn();
      Print("ei: %d, ej: %d", ei, ej); 
      PrintLn();
      Print("<product>: "); p_Write(product, GetBasering());  
#endif
      
    }

  }

  // TODO!      
  

  m_cache->StoreEE( expLeft, expRight, product);
  // up to now:
  return product; 
}

    // Monom * Exponent
poly CGlobalMultiplier::MultiplyME(const poly pMonom, const CGlobalMultiplier::CExponent expRight)
{
#if OUTPUT  
  Print("CGlobalMultiplier::MultiplyME(monom, expR)!");  
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
  Print("CGlobalMultiplier::MultiplyEM(expL, monom)!");  
  PrintLn();
  PrintS("expL: "); p_Write(expLeft, GetBasering());    
  PrintS("Monom: "); p_Write(pMonom, GetBasering());    
#endif

  return MultiplyEE(expLeft, pMonom);
}



///////////////////////////////////////////////////////////////////////////////////////////


static inline void CorrectPolyWRTOrdering(poly &pResult, const ring r)
{
  if( pNext(pResult) != NULL )
  {
    const int cmp = p_LmCmp(pResult, pNext(pResult), r);
    assume( cmp != 0 ); // must not be equal!!!
    if( cmp != 1 ) // Wrong order!!!
      pResult = pReverse(pResult); // Reverse!!!
  }

  p_Test(pResult, r);
}


///////////////////////////////////////////////////////////////////////////////////////////
CCommutativeSpecialPairMultiplier::CCommutativeSpecialPairMultiplier(ring r, int i, int j):
    CSpecialPairMultiplier(r, i, j)
{
#if OUTPUT  
  Print("CCommutativeSpecialPairMultiplier::CCommutativeSpecialPairMultiplier(ring, i: %d, j: %d)!", i, j);
  PrintLn();
#endif
};


CCommutativeSpecialPairMultiplier::~CCommutativeSpecialPairMultiplier()
{
#if OUTPUT  
  Print("CCommutativeSpecialPairMultiplier::~CCommutativeSpecialPairMultiplier()");
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

  poly p = p_ISet(1, r);
  p_SetExp(p, GetJ(), expLeft, r);
  p_SetExp(p, GetI(), expRight, r);
  p_Setm(p, r);

  return p;
}


///////////////////////////////////////////////////////////////////////////////////////////
CAntiCommutativeSpecialPairMultiplier::CAntiCommutativeSpecialPairMultiplier(ring r, int i, int j):
		CSpecialPairMultiplier(r, i, j)
{
#if OUTPUT  
	Print("CAntiCommutativeSpecialPairMultiplier::CAntiCommutativeSpecialPairMultiplier(ring, i: %d, j: %d)!", i, j);
	PrintLn();
#endif
};


CAntiCommutativeSpecialPairMultiplier::~CAntiCommutativeSpecialPairMultiplier()
{
#if OUTPUT  
	Print("CAntiCommutativeSpecialPairMultiplier::~CAntiCommutativeSpecialPairMultiplier()");
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
	const int  sign = 1 - 2*((expLeft * expRight) % 2);
	poly p = p_ISet(sign, r);
	p_SetExp(p, GetJ(), expLeft, r);
	p_SetExp(p, GetI(), expRight, r);
	p_Setm(p, r);

	return p;
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
};


CQuasiCommutativeSpecialPairMultiplier::~CQuasiCommutativeSpecialPairMultiplier()
{
#if OUTPUT  
	Print("CQuasiCommutativeSpecialPairMultiplier::~CQuasiCommutativeSpecialPairMultiplier()");
	PrintLn();
#endif
}


number CQuasiCommutativeSpecialPairMultiplier::GetPower(int power)
{
#if OUTPUT  
  Print("CQuasiCommutativeSpecialPairMultiplier::~GetPower(%d)", power);
	PrintLn();
#endif

	number qN;
	n_Power(m_q, power, &qN, GetBasering());

	// TODO: make it more intelegently!!! maybe dynamic cache for
	// previously computed powers of 2!?

	return qN;
}


// Exponent * Exponent
poly CQuasiCommutativeSpecialPairMultiplier::MultiplyEE(const int expLeft, const int expRight)
{
#if OUTPUT  
	Print("CQuasiCommutativeSpecialPairMultiplier::MultiplyEE(var(%d)^{%d}, var(%d)^{%d})!", GetJ(), expLeft, GetI(), expRight);  
	PrintLn();
#endif

	const ring r = GetBasering();

	const int  power = (expLeft * expRight);

	number qN = GetPower(power);
	
	poly p = p_NSet(qN, r);
	p_SetExp(p, GetJ(), expLeft, r);  // y^{expLeft}
	p_SetExp(p, GetI(), expRight, r); // x^{expRight}
	p_Setm(p, r);

	return p;
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
};


CWeylSpecialPairMultiplier::~CWeylSpecialPairMultiplier()
{
#if OUTPUT  
  Print("CWeylSpecialPairMultiplier::~CWeylSpecialPairMultiplier()");
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
  const int m = expLeft;
  const int n = expRight;

  assume( n*m > 0 );

  int kn = n;
  int km = m;

  number c = n_Init(1, r);

  poly p = p_ISet(1, r);

  p_SetExp(p, GetJ(), km--, r); // y ^ (m-k)
  p_SetExp(p, GetI(), kn--, r); // x ^ (n-k)

  p_Setm(p, r); // pResult = x^n * y^m

  
  poly pResult = p;
  poly pLast = p;

  int min = si_min(m, n);

  int k = 1;
  
  for(; k < min; k++ )
  {
    number t = n_Init(km + 1, r);
    n_InpMult(t, m_g, r); // t = ((m - k) + 1) * gamma
    n_InpMult(c, t, r);   // c = c'* ((m - k) + 1) * gamma
    n_Delete(&t, r);

    t = n_Init(kn + 1, r);
    n_InpMult(c, t, r);   // c = (c'* ((m - k) + 1) * gamma) * ((n - k) + 1)
    n_Delete(&t, r);
    
    t = n_Init(k, r);
    c = n_Div(c, t, r);
    n_Delete(&t, r);

//    n_Normalize(c, r);

    t = n_Copy(c, r); // not the last!

    p = p_NSet(t, r);

    p_SetExp(p, GetJ(), km--, r); // y ^ (m-k)
    p_SetExp(p, GetI(), kn--, r); // x ^ (n-k)

    p_Setm(p, r); // pResult = x^n * y^m

    pNext(pLast) = p;
    pLast = p;
  }

  assume(k == min);
  assume((km == 0) || (kn == 0) );

  {
    n_InpMult(c, m_g, r);   // c = c'* gamma

    if( km > 0 )
    {
      number t = n_Init(km + 1, r);
      n_InpMult(c, t, r);   // c = (c'* gamma) * (m - k + 1)
      n_Delete(&t, r);
    }
    
    if( kn > 0 )
    {
      number t = n_Init(kn + 1, r);
      n_InpMult(c, t, r);   // c = (c'* gamma) * (n - k + 1)
      n_Delete(&t, r);
    }

    number t = n_Init(k, r); // c = ((c'* gamma) * ((n - k + 1) * (m - k + 1))) / k;
    c = n_Div(c, t, r);
    n_Delete(&t, r);
  }
  
  p = p_NSet(c, r);

  p_SetExp(p, GetJ(), km, r); // y ^ (m-k)
  p_SetExp(p, GetI(), kn, r); // x ^ (n-k)

  p_Setm(p, r); // 

  pNext(pLast) = p;

  CorrectPolyWRTOrdering(pResult, r);

  return pResult;
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
};


CShiftSpecialPairMultiplier::~CShiftSpecialPairMultiplier()
{
#if OUTPUT  
  Print("CShiftSpecialPairMultiplier::~CShiftSpecialPairMultiplier()");
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

  int m, n, i, j;

  if( m_shiftVar != GetI() ) // YX = XY + a*X
  {
    m = expRight; // case: (1, 0, beta, 0, 0)
    n = expLeft;

    i = GetJ();
    j = GetI();
  } else
  {
    m = expLeft; // case: (1, alpha, 0, 0)
    n = expRight;

    i = GetI();
    j = GetJ();
  }

    
  int k = m; // to 0
  
  number c = n_Init(1, r); // k = m, C_k = 1
  poly p = p_ISet(1, r);

  p_SetExp(p, j, k, r); // Y^{k}
  p_SetExp(p, i, n, r); 

  p_Setm(p, r); // pResult = C_k * x^n * y^k, k == m


  poly pResult = p;
  poly pLast = p;

  number nn =  n_Init(n, r); // number(n)!
  n_InpMult(nn, m_shiftCoef, r); // nn = (alpha*n)

  --k;
  
  int mk = 1; // mk = (m - k)

  for(; k > 0; k-- )
  {
    number t = n_Init(k + 1, r);  // t = k+1
    n_InpMult(c, t, r);           // c = c' * (k+1) 
    n_InpMult(c, nn, r);          // c = (c' * (k+1)) * (alpha * n)

    n_Delete(&t, r);
    t = n_Init(mk++, r);         
    c = n_Div(c, t, r);           // c = ((c' * (k+1))  * (alpha * n)) / (m-k);
    n_Delete(&t, r);

//    n_Normalize(c, r);

    t = n_Copy(c, r); // not the last!

    p = p_NSet(t, r);

    p_SetExp(p, j, k, r); // y^k
    p_SetExp(p, i, n, r); // x^n

    p_Setm(p, r); // pResult = x^n * y^m

    pNext(pLast) = p;
    pLast = p;
  }

  assume(k == 0);

  {
    n_InpMult(c, nn, r);          // c = (c' * (0+1)) * (alpha * n)

    number t = n_Init(m, r);         
    c = n_Div(c, t, r);           // c = ((c' * (0+1))  * (alpha * n)) / (m-0);
    n_Delete(&t, r);
  }

  n_Delete(&nn, r);
  
  p = p_NSet(c, r);

  p_SetExp(p, j, k, r); // y^k
  p_SetExp(p, i, n, r); // x^n

  p_Setm(p, r); // 

  pNext(pLast) = p;

  CorrectPolyWRTOrdering(pResult, r);
 
  return pResult;
}





///////////////////////////////////////////////////////////////////////////////////////////

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

	const number q = p_GetCoeff(c, r);
	
	if( d == NULL)
	{
		
	  if( n_IsOne(q, r) )
			return new CCommutativeSpecialPairMultiplier(r, i, j);

		if( n_IsMOne(q, r) )
			return new CAntiCommutativeSpecialPairMultiplier(r, i, j);

		return new CQuasiCommutativeSpecialPairMultiplier(r, i, j, q);
  } else
  {
    if( n_IsOne(q, r) ) // "Lie" case
    {
      if( pNext(d) == NULL ) // Our Main Special Case! 
      {
        const number g = p_GetCoeff(d, r);

        if( p_LmIsConstantComp(d, r) ) // Weyl
          return new CWeylSpecialPairMultiplier(r, i, j, g);          

        const int k = p_IsPurePower(d, r); // k if not pure power

        if( k > 0 )
          if( p_GetExp(d, k, r) == 1 )
            if( (k == i) || (k == j) )
              return new CShiftSpecialPairMultiplier(r, i, j, k, g);
      }
    }
  }
    



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
  if( (v == j) )
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
  Print("CPowerMultiplier::MultiplyME() ===> ");
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
  Print("CPowerMultiplier::MultiplyEM() ===> ");
  p_Write(p, r);  
#endif

  return p;
  
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

  assume(1 <= i);
  assume(j <= NVars());
  assume(1 <= j);
  assume(i <= NVars());
  assume(ei > 0);
  assume(ej > 0);
  
  if( i >= j )
  {
    const ring r = GetBasering();

    poly product = p_ISet(1, r);
    p_SetExp(product, j, ej, r);
    p_SetExp(product, i, ei, r);
    p_Setm(product, r);

    return product;

  } else
  {
    assume(i <  j);

    // No Cache Lookup!? :(

    CSpecialPairMultiplier* pSpecialMultiplier = GetPair(i, j);

    poly product = NULL;

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

//      product = NULL;
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
  Print("CSpecialPairMultiplier::~CSpecialPairMultiplier()!");
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





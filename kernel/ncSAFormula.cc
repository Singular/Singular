/****************************************
*  Computer Algebra System SINGULAR     *
****************************************/
/***************************************************************
 *  File:    ncSAFormula.cc
 *  Purpose: implementation of multiplication by formulas in simple NC subalgebras
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


#include <kernel/mod2.h>

#ifdef HAVE_PLURAL

#ifndef NDEBUG
#define OUTPUT 1
#else
#define OUTPUT 0
#endif


#include <ncSAFormula.h> // for CFormulaPowerMultiplier

#include <ring.h>
#include <p_polys.h>
#include <febase.h>
#include <sca.h> // for SCA
#include <polys.h> // for p_One




bool ncInitSpecialPowersMultiplication(ring r)
{
#if OUTPUT  
  Print("ncInitSpecialPowersMultiplication(ring), ring: \n");
  rWrite(r);
  PrintLn();
#endif

  assume(rIsPluralRing(r));
  assume(!rIsSCA(r));


  if( r->GetNC()->GetFormulaPowerMultiplier() != NULL )
  {
    WarnS("Already defined!");
    return false;
  }

  
  r->GetNC()->GetFormulaPowerMultiplier() = new CFormulaPowerMultiplier(r);

  return true;
  
};





static inline Enum_ncSAType AnalyzePairType(const ring r, int i, int j)
{
#if OUTPUT  
  Print("AnalyzePair(ring, i: %d, j: %d)!", i, j);
  PrintLn();
#endif

  const int N = r->N;

  assume(i < j);
  assume(i > 0);
  assume(j <= N);


  const number q = p_GetCoeff(GetC(r, i, j), r);
  const poly d = GetD(r, i, j);

#if OUTPUT  
  Print("C_{%d, %d} = ", i, j);  { number t = n_Copy(q, r);  n_Write(t, r);  n_Delete(&t, r); };
  Print("D_{%d, %d} = ", i, j); p_Write(d, r);
#endif

//  const number q = p_GetCoeff(c, r);

  if( d == NULL)
  {

    if( n_IsOne(q, r) ) // commutative
      return _ncSA_1xy0x0y0;

    if( n_IsMOne(q, r) )
      return _ncSA_Mxy0x0y0;

    return _ncSA_Qxy0x0y0;
  } else
  {
    if( n_IsOne(q, r) ) // "Lie" case
    {
      if( pNext(d) == NULL ) // Our Main Special Case! 
      {
        const number g = p_GetCoeff(d, r);

        if( p_LmIsConstantComp(d, r) ) // Weyl
          return _ncSA_1xy0x0yG;

        const int k = p_IsPurePower(d, r); // k if not pure power

        if( k > 0 )
          if( p_GetExp(d, k, r) == 1 )
          {
            if(k == i)
              return _ncSA_1xyAx0y0;

            if(k == j)
              return _ncSA_1xy0xBy0;              
          }
      }
    }
  }

  return _ncSA_notImplemented;
};



CFormulaPowerMultiplier::CFormulaPowerMultiplier(ring r): m_BaseRing(r), m_NVars(r->N)
{
#if OUTPUT  
  Print("CFormulaPowerMultiplier::CFormulaPowerMultiplier(ring)!");
  PrintLn();
#endif

  m_SAPairTypes = (Enum_ncSAType*)omAlloc0( ((NVars() * (NVars()-1)) / 2) * sizeof(Enum_ncSAType) );

  for( int i = 1; i < NVars(); i++ )
    for( int j = i + 1; j <= NVars(); j++ )
      GetPair(i, j) = AnalyzePairType(GetBasering(), i, j);
}




CFormulaPowerMultiplier::~CFormulaPowerMultiplier()
{
#if OUTPUT  
  Print("CFormulaPowerMultiplier::~CFormulaPowerMultiplier()!");
  PrintLn();
#endif

  omFreeSize((ADDRESS)m_SAPairTypes, ((NVars() * (NVars()-1)) / 2) * sizeof(Enum_ncSAType) );
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
static inline poly ncSA_1xy0x0y0(const int i, const int j, const int n, const int m, const ring r)
{
#if OUTPUT  
  Print("ncSA_1xy0x0y0(var(%d)^{%d}, var(%d)^{%d}, r)!", j, m, i, n);  
  PrintLn();
#endif

  poly p = p_One( r);
  p_SetExp(p, j, m, r);
  p_SetExp(p, i, n, r);
  p_Setm(p, r);

  p_Test(p, r);

  return p;

} // 	return ncSA_1xy0x0y0(GetI(), GetJ(), expRight, expLeft, r);
///////////////////////////////////////////////////////////////////////////////////////////
static inline poly ncSA_Mxy0x0y0(const int i, const int j, const int n, const int m, const ring r)
{
#if OUTPUT  
  Print("ncSA_{M = -1}xy0x0y0(var(%d)^{%d}, var(%d)^{%d}, r)!", j, m, i, n);  
  PrintLn();
#endif

  const int  sign = 1 - ((n & (m & 1)) << 1);
  poly p = p_ISet(sign, r);
  p_SetExp(p, j, m, r);
  p_SetExp(p, i, n, r);
  p_Setm(p, r);


  p_Test(p, r);

  return p;

} // 	return ncSA_Mxy0x0y0(GetI(), GetJ(), expRight, expLeft, r);
///////////////////////////////////////////////////////////////////////////////////////////
static inline poly ncSA_Qxy0x0y0(const int i, const int j, const int n, const int m, const number m_q, const ring r)
{
#if OUTPUT  
  Print("ncSA_Qxy0x0y0(var(%d)^{%d}, var(%d)^{%d}, Q, r)!", j, m, i, n);  
  PrintLn();
#endif

  int min, max;

  if( n < m )
  {
    min = n;
    max = m;
  } else
  {
    min = m;
    max = n;
  }

  number qN;

  if( max == 1 )
    qN = n_Copy(m_q, r);
  else
  {
    number t;
    n_Power(m_q, max, &t, r);

    if( min > 1 )
    {
      n_Power(t, min, &qN, r);
      n_Delete(&t, r);      
    }
    else
      qN = t;
  }

  poly p = p_NSet(qN, r);
  p_SetExp(p, j, m, r);
  p_SetExp(p, i, n, r);
  p_Setm(p, r);


  p_Test(p, r);

  return p;

} // 	return ncSA_Qxy0x0y0(GetI(), GetJ(), expRight, expLeft, m_q, r);
///////////////////////////////////////////////////////////////////////////////////////////
static inline poly ncSA_1xy0x0yG(const int i, const int j, const int n, const int m, const number m_g, const ring r)
{
#if OUTPUT  
  Print("ncSA_1xy0x0yG(var(%d)^{%d}, var(%d)^{%d}, G, r)!", j, m, i, n);  
  PrintLn();
  number t = n_Copy(m_g, r);
  PrintS("Parameter G: "); n_Write(t, r);
  n_Delete(&t, r);
#endif

  int kn = n;
  int km = m;

  number c = n_Init(1, r);

  poly p = p_One( r);

  p_SetExp(p, j, km--, r); // y ^ (m-k)
  p_SetExp(p, i, kn--, r); // x ^ (n-k)

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

    p_SetExp(p, j, km--, r); // y ^ (m-k)
    p_SetExp(p, i, kn--, r); // x ^ (n-k)

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

  p_SetExp(p, j, km, r); // y ^ (m-k)
  p_SetExp(p, i, kn, r); // x ^ (n-k)

  p_Setm(p, r); // 

  pNext(pLast) = p;

  CorrectPolyWRTOrdering(pResult, r);

  return pResult;
}
///////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////
static inline poly ncSA_ShiftAx(int i, int j, int n, int m, const number m_shiftCoef, const ring r)
{
  // Char == 0, otherwise - problem!

  int k = m; // to 0

  number c = n_Init(1, r); // k = m, C_k = 1
  poly p = p_One( r);

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
static inline poly ncSA_1xyAx0y0(const int i, const int j, const int n, const int m, const number m_shiftCoef, const ring r)
{
#if OUTPUT  
  Print("ncSA_1xyAx0y0(var(%d)^{%d}, var(%d)^{%d}, A, r)!", j, m, i, n);  
  PrintLn();
  number t = n_Copy(m_shiftCoef, r);
  PrintS("Parameter A: "); n_Write(t, r);
  n_Delete(&t, r);  
#endif

  return ncSA_ShiftAx(i, j, n, m, m_shiftCoef, r);
}
///////////////////////////////////////////////////////////////////////////////////////////
static inline poly ncSA_1xy0xBy0(const int i, const int j, const int n, const int m, const number m_shiftCoef, const ring r)
{
#if OUTPUT  
  Print("ncSA_1xy0xBy0(var(%d)^{%d}, var(%d)^{%d}, B, r)!", j, m, i, n);  
  PrintLn();
  number t = n_Copy(m_shiftCoef, r);
  PrintS("Parameter B: "); n_Write(t, r);
  n_Delete(&t, r);  
#endif

  return ncSA_ShiftAx(j, i, m, n, m_shiftCoef, r);
}
///////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////


static inline poly ncSA_Multiply( Enum_ncSAType type, const int i, const int j, const int n, const int m, const ring r)
{
#if OUTPUT  
  Print("ncSA_Multiply(type: %d, ring, (var(%d)^{%d} * var(%d)^{%d}, r)!", (int)type, j, m, i, n);  
  PrintLn();
#endif

  assume( type != _ncSA_notImplemented );
  assume( (n > 0) && (m > 0) );

  if( type == _ncSA_1xy0x0y0 )
    return ::ncSA_1xy0x0y0(i, j, n, m, r);

  if( type == _ncSA_Mxy0x0y0 )
    return ::ncSA_Mxy0x0y0(i, j, n, m, r);

  if( type == _ncSA_Qxy0x0y0 )
  {
    const number q = p_GetCoeff(GetC(r, i, j), r);
    return ::ncSA_Qxy0x0y0(i, j, n, m, q, r);
  }

  const number g = p_GetCoeff(GetD(r, i, j), r);

  if( type == _ncSA_1xy0x0yG ) // Weyl
    return ::ncSA_1xy0x0yG(i, j, n, m, g, r);

  if( type == _ncSA_1xyAx0y0 ) // Shift 1
    return ::ncSA_1xyAx0y0(i, j, n, m, g, r);

  if( type == _ncSA_1xy0xBy0 ) // Shift 2
    return ::ncSA_1xy0xBy0(i, j, n, m, g, r);

  assume( type == _ncSA_notImplemented );

  return NULL;
}


poly CFormulaPowerMultiplier::Multiply( Enum_ncSAType type, const int i, const int j, const int n, const int m, const ring r)
{
  return ncSA_Multiply( type, i, j, n, m, r);
}


Enum_ncSAType CFormulaPowerMultiplier::AnalyzePair(const ring r, int i, int j)
{
  return ::AnalyzePairType( r, i, j);
}

poly CFormulaPowerMultiplier::Multiply( int i, int j, const int n, const int m)
{
  return ncSA_Multiply( GetPair(i, j), i, j, n, m, GetBasering());
}




poly CFormulaPowerMultiplier::ncSA_1xy0x0y0(const int i, const int j, const int n, const int m, const ring r)
{
  return ::ncSA_1xy0x0y0(i, j, n, m, r);
}

poly CFormulaPowerMultiplier::ncSA_Mxy0x0y0(const int i, const int j, const int n, const int m, const ring r)
{
  return ::ncSA_Mxy0x0y0(i, j, n, m, r);
}

poly CFormulaPowerMultiplier::ncSA_Qxy0x0y0(const int i, const int j, const int n, const int m, const number m_q, const ring r)
{
  return ::ncSA_Qxy0x0y0(i, j, n, m, m_q, r);
};

poly CFormulaPowerMultiplier::ncSA_1xy0x0yG(const int i, const int j, const int n, const int m, const number m_g, const ring r)
{
  return ::ncSA_1xy0x0yG(i, j, n, m, m_g, r);
}

poly CFormulaPowerMultiplier::ncSA_1xyAx0y0(const int i, const int j, const int n, const int m, const number m_shiftCoef, const ring r)
{
  return ::ncSA_1xyAx0y0(i, j, n, m, m_shiftCoef, r);
}

poly CFormulaPowerMultiplier::ncSA_1xy0xBy0(const int i, const int j, const int n, const int m, const number m_shiftCoef, const ring r)
{
  return ::ncSA_1xy0xBy0(i, j, n, m, m_shiftCoef, r);
}
#endif

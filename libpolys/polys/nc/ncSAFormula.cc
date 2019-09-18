/****************************************
*  Computer Algebra System SINGULAR     *
****************************************/
/***************************************************************
 *  File:    ncSAFormula.cc
 *  Purpose: implementation of multiplication by formulas in simple NC subalgebras
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

#define PLURAL_INTERNAL_DECLARATIONS

#ifndef SING_NDEBUG
#define OUTPUT MYTEST
#else
#define OUTPUT 0
#endif

#include "reporter/reporter.h"

#include "coeffs/numbers.h"

#include "nc/ncSAFormula.h"
// for CFormulaPowerMultiplier

#include "monomials/ring.h"
#include "monomials/p_polys.h"

#include "nc/sca.h"




bool ncInitSpecialPowersMultiplication(ring r)
{
#if OUTPUT
  PrintS("ncInitSpecialPowersMultiplication(ring), ring: \n");
  rWrite(r, TRUE);
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

}







// TODO: return q-coeff?
static inline BOOLEAN AreCommutingVariables(const ring r, int i, int j/*, number *qq*/)
{
#if OUTPUT
  Print("AreCommutingVariables(ring, k: %d, i: %d)!\n", j, i);
#endif

  assume(i != j);

  assume(i > 0);
  assume(i <= r->N);


  assume(j > 0);
  assume(j <= r->N);

  const BOOLEAN reverse = (i > j);

  if (reverse) { int k = j; j = i; i = k; }

  assume(i < j);

  {
    const poly d = GetD(r, i, j);

#if OUTPUT
    Print("D_{%d, %d} = ", i, j); p_Write(d, r);
#endif

    if( d != NULL)
      return FALSE;
  }


  {
    const number q = p_GetCoeff(GetC(r, i, j), r);

    if( !n_IsOne(q, r->cf) )
      return FALSE;
  }

  return TRUE; // [THREAD_VAR(I), THREAD_VAR(J)] = 0!!

/*
  if (reverse)
    *qq = n_Invers(q, r);
  else
    *qq = n_Copy(q, r);
  return TRUE;
*/
}

static inline Enum_ncSAType AnalyzePairType(const ring r, int i, int j)
{
#if OUTPUT
  Print("AnalyzePair(ring, i: %d, j: %d):\n", i, j);
#endif

  const int N = r->N;

  assume(i < j);
  assume(i > 0);
  assume(j <= N);


  const poly c = GetC(r, i, j);
  const number q = pGetCoeff(c);
  const poly d = GetD(r, i, j);

#if 0 && OUTPUT
  Print("C_{%d, %d} = ", i, j); p_Write(c, r); PrintLn();
  Print("D_{%d, %d} = ", i, j); p_Write(d, r);
#endif

//  const number q = p_GetCoeff(c, r);

  if( d == NULL)
  {

    if( n_IsOne(q, r->cf) ) // commutative
      return _ncSA_1xy0x0y0;

    if( n_IsMOne(q, r->cf) ) // anti-commutative
      return _ncSA_Mxy0x0y0;

    return _ncSA_Qxy0x0y0; // quasi-commutative
  } else
  {
    if( n_IsOne(q, r->cf) ) // "Lie" case
    {
      if( pNext(d) == NULL ) // Our Main Special Case: d is only a term!
      {
//         const number g = p_GetCoeff(d, r); // not used for now
        if( p_LmIsConstantComp(d, r) ) // Weyl
          return _ncSA_1xy0x0yG;

        const int k = p_IsPurePower(d, r); // k if not pure power

        if( k > 0 ) // d = var(k)^??
        {
          const int exp = p_GetExp(d, k, r);

          if (exp == 1)
          {
            if(k == i) // 2 -ubalgebra in var(i) & var(j), with linear relation...?
              return _ncSA_1xyAx0y0;

            if(k == j)
              return _ncSA_1xy0xBy0;
          } else if ( exp == 2 && k!= i && k != j)  // Homogenized Weyl algebra [x, Dx] = t^2?
          {
//            number qi, qj;
            if (AreCommutingVariables(r, k, i/*, &qi*/) && AreCommutingVariables(r, k, j/*, &qj*/) ) // [x, t] = [Dx, t] = 0?
            {
              const number g = pGetCoeff(d);

              if (n_IsOne(g, r->cf))
                return _ncSA_1xy0x0yT2; // save k!?, G = LC(d) == qi == qj == 1!!!
            }
          }
        }
      }
    }
    // Hmm, what about a more general case of q != 1???
  }
#if OUTPUT
  Print("C_{%d, %d} = ", i, j); p_Write(c, r);
  Print("D_{%d, %d} = ", i, j); p_Write(d, r);
  PrintS("====>>>>_ncSA_notImplemented\n");
#endif

  return _ncSA_notImplemented;
}


CFormulaPowerMultiplier::CFormulaPowerMultiplier(ring r): m_NVars(r->N), m_BaseRing(r)
{
#if OUTPUT
  PrintS("CFormulaPowerMultiplier::CFormulaPowerMultiplier(ring)!");
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
  PrintS("CFormulaPowerMultiplier::~CFormulaPowerMultiplier()!");
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
  Print("ncSA_1xy0x0y0(var(%d)^{%d}, var(%d)^{%d}, r)!\n", j, m, i, n);
#endif

  poly p = p_One( r);
  p_SetExp(p, j, m, r);
  p_SetExp(p, i, n, r);
  p_Setm(p, r);

  p_Test(p, r);

  return p;

} //         return ncSA_1xy0x0y0(GetI(), GetJ(), expRight, expLeft, r);
///////////////////////////////////////////////////////////////////////////////////////////
static inline poly ncSA_Mxy0x0y0(const int i, const int j, const int n, const int m, const ring r)
{
#if OUTPUT
  Print("ncSA_{M = -1}xy0x0y0(var(%d)^{%d}, var(%d)^{%d}, r)!\n", j, m, i, n);
#endif

  const int  sign = 1 - ((n & (m & 1)) << 1);
  poly p = p_ISet(sign, r);
  p_SetExp(p, j, m, r);
  p_SetExp(p, i, n, r);
  p_Setm(p, r);


  p_Test(p, r);

  return p;

} //         return ncSA_Mxy0x0y0(GetI(), GetJ(), expRight, expLeft, r);
///////////////////////////////////////////////////////////////////////////////////////////
static inline poly ncSA_Qxy0x0y0(const int i, const int j, const int n, const int m, const number m_q, const ring r)
{
#if OUTPUT
  Print("ncSA_Qxy0x0y0(var(%d)^{%d}, var(%d)^{%d}, Q, r)!\n", j, m, i, n);
#endif

  int min, max;

  if( n < m )
  {
    min = n;
    max = m;
  }
  else
  {
    min = m;
    max = n;
  }

  number qN;

  if( max == 1 )
    qN = n_Copy(m_q, r->cf);
  else
  {
    number t;
    n_Power(m_q, max, &t, r->cf);

    if( min > 1 )
    {
      n_Power(t, min, &qN, r->cf);
      n_Delete(&t, r->cf);
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

} //         return ncSA_Qxy0x0y0(GetI(), GetJ(), expRight, expLeft, m_q, r);
///////////////////////////////////////////////////////////////////////////////////////////
static inline poly ncSA_1xy0x0yG(const int i, const int j, const int n, const int m, const number m_g, const ring r)
{
#if OUTPUT
  Print("ncSA_1xy0x0yG(var(%d)^{%d}, var(%d)^{%d}, G, r)!\n", j, m, i, n);
  number t = n_Copy(m_g, r->cf);
  PrintS("Parameter G: "); n_Write(t, r->cf);
  n_Delete(&t, r->cf);
#endif

  int kn = n;
  int km = m;

  number c = n_Init(1, r->cf);

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
    number t = n_Init(km + 1, r->cf);
    n_InpMult(t, m_g, r->cf); // t = ((m - k) + 1) * gamma
    n_InpMult(c, t, r->cf);   // c = c'* ((m - k) + 1) * gamma
    n_Delete(&t, r->cf);

    t = n_Init(kn + 1, r->cf);
    n_InpMult(c, t, r->cf);   // c = (c'* ((m - k) + 1) * gamma) * ((n - k) + 1)
    n_Delete(&t, r->cf);

    t = n_Init(k, r->cf);
    c = n_Div(c, t, r->cf);
    n_Delete(&t, r->cf);

//    n_Normalize(c, r->cf);

    t = n_Copy(c, r->cf); // not the last!

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
    n_InpMult(c, m_g, r->cf);   // c = c'* gamma

    if( km > 0 )
    {
      number t = n_Init(km + 1, r->cf);
      n_InpMult(c, t, r->cf);   // c = (c'* gamma) * (m - k + 1)
      n_Delete(&t, r->cf);
    }

    if( kn > 0 )
    {
      number t = n_Init(kn + 1, r->cf);
      n_InpMult(c, t, r->cf);   // c = (c'* gamma) * (n - k + 1)
      n_Delete(&t, r->cf);
    }

    number t = n_Init(k, r->cf); // c = ((c'* gamma) * ((n - k + 1) * (m - k + 1))) / k;
    c = n_Div(c, t, r->cf);
    n_Delete(&t, r->cf);
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
static inline poly ncSA_1xy0x0yT2(const int i, const int j, const int n, const int m, const int m_k, const ring r)
{
#if OUTPUT
  Print("ncSA_1xy0x0yT2(var(%d)^{%d}, var(%d)^{%d}, t: var(%d), r)!\n", j, m, i, n, m_k);
#endif

  int kn = n;
  int km = m;

  // k == 0!
  number c = n_Init(1, r->cf);

  poly p = p_One( r );

  p_SetExp(p, j, km--, r); // y ^ (m)
  p_SetExp(p, i, kn--, r); // x ^ (n)
//  p_SetExp(p, m_k, k << 1, r); // homogenization with var(m_k) ^ (2*k)

  p_Setm(p, r); // pResult = x^n * y^m


  poly pResult = p;
  poly pLast = p;

  int min = si_min(m, n);

  int k = 1;

  for(; k < min; k++ )
  {
    number t = n_Init(km + 1, r->cf);
//    n_InpMult(t, m_g, r->cf); // t = ((m - k) + 1) * gamma
    n_InpMult(c, t, r->cf);   // c = c'* ((m - k) + 1) * gamma
    n_Delete(&t, r->cf);

    t = n_Init(kn + 1, r->cf);
    n_InpMult(c, t, r->cf);   // c = (c'* ((m - k) + 1) * gamma) * ((n - k) + 1)
    n_Delete(&t, r->cf);

    t = n_Init(k, r->cf);
    c = n_Div(c, t, r->cf);
    n_Delete(&t, r->cf);

// //    n_Normalize(c, r);

    t = n_Copy(c, r->cf); // not the last!

    p = p_NSet(t, r);

    p_SetExp(p, j, km--, r); // y ^ (m-k)
    p_SetExp(p, i, kn--, r); // x ^ (n-k)

    p_SetExp(p, m_k, k << 1, r); // homogenization with var(m_k) ^ (2*k)

    p_Setm(p, r); // pResult = x^(n-k) * y^(m-k)

    pNext(pLast) = p;
    pLast = p;
  }

  assume(k == min);
  assume((km == 0) || (kn == 0) );

  {
//    n_InpMult(c, m_g, r);   // c = c'* gamma

    if( km > 0 )
    {
      number t = n_Init(km + 1, r->cf);
      n_InpMult(c, t, r->cf);   // c = (c'* gamma) * (m - k + 1)
      n_Delete(&t, r->cf);
    }

    if( kn > 0 )
    {
      number t = n_Init(kn + 1, r->cf);
      n_InpMult(c, t, r->cf);   // c = (c'* gamma) * (n - k + 1)
      n_Delete(&t, r->cf);
    }

    number t = n_Init(k, r->cf); // c = ((c'* gamma) * ((n - k + 1) * (m - k + 1))) / k;
    c = n_Div(c, t, r->cf);
    n_Delete(&t, r->cf);
  }

  p = p_NSet(c, r);

  p_SetExp(p, j, km, r); // y ^ (m-k)
  p_SetExp(p, i, kn, r); // x ^ (n-k)

  p_SetExp(p, m_k, k << 1, r); // homogenization with var(m_k) ^ (2*k)

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

  number c = n_Init(1, r->cf); // k = m, C_k = 1
  poly p = p_One( r);

  p_SetExp(p, j, k, r); // Y^{k}
  p_SetExp(p, i, n, r);

  p_Setm(p, r); // pResult = C_k * x^n * y^k, k == m


  poly pResult = p;
  poly pLast = p;

  number nn =  n_Init(n, r->cf); // number(n)!
  n_InpMult(nn, m_shiftCoef, r->cf); // nn = (alpha*n)

  --k;

  int mk = 1; // mk = (m - k)

  for(; k > 0; k-- )
  {
    number t = n_Init(k + 1, r->cf);  // t = k+1
    n_InpMult(c, t, r->cf);           // c = c' * (k+1)
    n_InpMult(c, nn, r->cf);          // c = (c' * (k+1)) * (alpha * n)

    n_Delete(&t, r->cf);
    t = n_Init(mk++, r->cf);
    c = n_Div(c, t, r->cf);           // c = ((c' * (k+1))  * (alpha * n)) / (m-k);
    n_Delete(&t, r->cf);

//    n_Normalize(c, r->cf);

    t = n_Copy(c, r->cf); // not the last!

    p = p_NSet(t, r);

    p_SetExp(p, j, k, r); // y^k
    p_SetExp(p, i, n, r); // x^n

    p_Setm(p, r); // pResult = x^n * y^m

    pNext(pLast) = p;
    pLast = p;
  }

  assume(k == 0);

  {
    n_InpMult(c, nn, r->cf);          // c = (c' * (0+1)) * (alpha * n)

    number t = n_Init(m, r->cf);
    c = n_Div(c, t, r->cf);           // c = ((c' * (0+1))  * (alpha * n)) / (m-0);
    n_Delete(&t, r->cf);
  }

  n_Delete(&nn, r->cf);

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
  Print("ncSA_1xyAx0y0(var(%d)^{%d}, var(%d)^{%d}, A, r)!\n", j, m, i, n);
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
  Print("ncSA_1xy0xBy0(var(%d)^{%d}, var(%d)^{%d}, B, r)!\n", j, m, i, n);
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
  Print("ncSA_Multiply(type: %d, ring, (var(%d)^{%d} * var(%d)^{%d}, r)!\n", (int)type, j, m, i, n);
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

  const poly d = GetD(r, i, j);
  const number g = p_GetCoeff(d, r);

  if( type == _ncSA_1xy0x0yG ) // Weyl
    return ::ncSA_1xy0x0yG(i, j, n, m, g, r);

  if( type == _ncSA_1xy0x0yT2 ) // Homogenous Weyl...
    return ::ncSA_1xy0x0yT2(i, j, n, m, p_IsPurePower(d, r), r);

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
}

poly CFormulaPowerMultiplier::ncSA_1xy0x0yG(const int i, const int j, const int n, const int m, const number m_g, const ring r)
{
  return ::ncSA_1xy0x0yG(i, j, n, m, m_g, r);
}

poly CFormulaPowerMultiplier::ncSA_1xy0x0yT2(const int i, const int j, const int n, const int m, const int k, const ring r)
{
  return ::ncSA_1xy0x0yT2(i, j, n, m, k, r);
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

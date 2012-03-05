/****************************************
*  Computer Algebra System SINGULAR     *
****************************************/
/***************************************************************
 *  File:    sca.cc
 *  Purpose: supercommutative kernel procedures
 *  Author:  motsak (Oleksandr Motsak)
 *  Created: 2006/12/18
 *******************************************************************/

// set it here if needed.
#define OUTPUT 0
#define MYTEST 0

#if MYTEST
#define OM_CHECK 4
#define OM_TRACK 5
#endif

// #define PDEBUG 2
#include <kernel/mod2.h>

#ifdef HAVE_PLURAL
// for
#define PLURAL_INTERNAL_DECLARATIONS
#include <kernel/sca.h>
#include <kernel/gring.h>


#include <kernel/febase.h>
#include <kernel/options.h>

#include <kernel/p_polys.h>
#include <kernel/kutil.h>
#include <kernel/ideals.h>
#include <kernel/intvec.h>
#include <kernel/polys.h>

#include <kernel/ring.h>
#include <kernel/numbers.h>
#include <kernel/matpol.h>
#include <kernel/kbuckets.h>
#include <kernel/kstd1.h>
#include <kernel/sbuckets.h>
#include <kernel/prCopy.h>
#include <kernel/p_Mult_q.h>
#include <kernel/p_MemAdd.h>

#include <kernel/kutil.h>
#include <kernel/kstd1.h>

#include <kernel/weight.h>


// poly functions defined in p_Procs :

// return pPoly * pMonom; preserve pPoly and pMonom.
poly sca_pp_Mult_mm(const poly pPoly, const poly pMonom, const ring rRing, poly &);

// return pMonom * pPoly; preserve pPoly and pMonom.
static poly sca_mm_Mult_pp(const poly pMonom, const poly pPoly, const ring rRing);

// return pPoly * pMonom; preserve pMonom, destroy or reuse pPoly.
poly sca_p_Mult_mm(poly pPoly, const poly pMonom, const ring rRing);

// return pMonom * pPoly; preserve pMonom, destroy or reuse pPoly.
static poly sca_mm_Mult_p(const poly pMonom, poly pPoly, const ring rRing);


// compute the spolynomial of p1 and p2
poly sca_SPoly(const poly p1, const poly p2, const ring r);
poly sca_ReduceSpoly(const poly p1, poly p2, const ring r);

// Modified Plural's Buchberger's algorithmus.
ideal sca_gr_bba(const ideal F, const ideal Q, const intvec *, const intvec *, kStrategy strat);

// Modified modern Sinuglar Buchberger's algorithm.
ideal sca_bba(const ideal F, const ideal Q, const intvec *w, const intvec *, kStrategy strat);

// Modified modern Sinuglar Mora's algorithm.
ideal sca_mora(const ideal F, const ideal Q, const intvec *w, const intvec *, kStrategy strat);



////////////////////////////////////////////////////////////////////////////////////////////////////
// Super Commutative Algebra extension by Oleksandr
////////////////////////////////////////////////////////////////////////////////////////////////////


static inline ring assureCurrentRing(ring r)
{
  ring save = currRing;

  if( currRing != r )
    rChangeCurrRing(r);

  return save;
}



// returns the sign of: lm(pMonomM) * lm(pMonomMM),
// preserves input, may return +/-1, 0
static inline int sca_Sign_mm_Mult_mm( const poly pMonomM, const poly pMonomMM, const ring rRing )
{
#ifdef PDEBUG
    p_Test(pMonomM,  rRing);
    p_Test(pMonomMM, rRing);
#endif

    const unsigned int iFirstAltVar = scaFirstAltVar(rRing);
    const unsigned int iLastAltVar  = scaLastAltVar(rRing);

    register unsigned int tpower = 0;
    register unsigned int cpower = 0;

    for( register unsigned int j = iLastAltVar; j >= iFirstAltVar; j-- )
    {
      const unsigned int iExpM  = p_GetExp(pMonomM,  j, rRing);
      const unsigned int iExpMM = p_GetExp(pMonomMM, j, rRing);

#ifdef PDEBUG
      assume( iExpM <= 1);
      assume( iExpMM <= 1);
#endif

      if( iExpMM != 0 ) // TODO: think about eliminating there if-s...
      {
        if( iExpM != 0 )
        {
          return 0; // lm(pMonomM) * lm(pMonomMM) == 0
        }
        tpower ^= cpower; // compute degree of (-1).
      }
      cpower ^= iExpM;
    }

#ifdef PDEBUG
    assume(tpower <= 1);
#endif

    // 1 => -1  // degree is odd => negate coeff.
    // 0 =>  1

    return(1 - (tpower << 1) );
}




// returns and changes pMonomM: lt(pMonomM) = lt(pMonomM) * lt(pMonomMM),
// preserves pMonomMM. may return NULL!
// if result != NULL => next(result) = next(pMonomM), lt(result) = lt(pMonomM) * lt(pMonomMM)
// if result == NULL => pMonomM MUST BE deleted manually!
static inline poly sca_m_Mult_mm( poly pMonomM, const poly pMonomMM, const ring rRing )
{
#ifdef PDEBUG
    p_Test(pMonomM,  rRing);
    p_Test(pMonomMM, rRing);
#endif

    const unsigned int iFirstAltVar = scaFirstAltVar(rRing);
    const unsigned int iLastAltVar = scaLastAltVar(rRing);

    register unsigned int tpower = 0;
    register unsigned int cpower = 0;

    for( register unsigned int j = iLastAltVar; j >= iFirstAltVar; j-- )
    {
      const unsigned int iExpM  = p_GetExp(pMonomM,  j, rRing);
      const unsigned int iExpMM = p_GetExp(pMonomMM, j, rRing);

#ifdef PDEBUG
      assume( iExpM <= 1);
      assume( iExpMM <= 1);
#endif

      if( iExpMM != 0 )
      {
        if( iExpM != 0 ) // result is zero!
        {
          return NULL; // we do nothing with pMonomM in this case!
        }

        tpower ^= cpower; // compute degree of (-1).
      }

      cpower ^= iExpM;
    }

#ifdef PDEBUG
    assume(tpower <= 1);
#endif

    p_ExpVectorAdd(pMonomM, pMonomMM, rRing); // "exponents" are additive!!!

    number nCoeffM = p_GetCoeff(pMonomM, rRing); // no new copy! should be deleted!

    if( (tpower) != 0 ) // degree is odd => negate coeff.
      nCoeffM = n_Neg(nCoeffM, rRing); // negate nCoeff (will destroy the original number)

    const number nCoeffMM = p_GetCoeff(pMonomMM, rRing); // no new copy!

    number nCoeff = n_Mult(nCoeffM, nCoeffMM, rRing); // new number!

    p_SetCoeff(pMonomM, nCoeff, rRing); // delete lc(pMonomM) and set lc(pMonomM) = nCoeff

#ifdef PDEBUG
    p_LmTest(pMonomM, rRing);
#endif

    return(pMonomM);
}

// returns and changes pMonomM: lt(pMonomM) = lt(pMonomMM) * lt(pMonomM),
// preserves pMonomMM. may return NULL!
// if result != NULL => next(result) = next(pMonomM), lt(result) = lt(pMonomMM) * lt(pMonomM)
// if result == NULL => pMonomM MUST BE deleted manually!
static inline poly sca_mm_Mult_m( const poly pMonomMM, poly pMonomM, const ring rRing )
{
#ifdef PDEBUG
    p_Test(pMonomM,  rRing);
    p_Test(pMonomMM, rRing);
#endif

    const unsigned int iFirstAltVar = scaFirstAltVar(rRing);
    const unsigned int iLastAltVar = scaLastAltVar(rRing);

    register unsigned int tpower = 0;
    register unsigned int cpower = 0;

    for( register unsigned int j = iLastAltVar; j >= iFirstAltVar; j-- )
    {
      const unsigned int iExpMM = p_GetExp(pMonomMM, j, rRing);
      const unsigned int iExpM  = p_GetExp(pMonomM,  j, rRing);

#ifdef PDEBUG
      assume( iExpM <= 1);
      assume( iExpMM <= 1);
#endif

      if( iExpM != 0 )
      {
        if( iExpMM != 0 ) // result is zero!
        {
          return NULL; // we do nothing with pMonomM in this case!
        }

        tpower ^= cpower; // compute degree of (-1).
      }

      cpower ^= iExpMM;
    }

#ifdef PDEBUG
    assume(tpower <= 1);
#endif

    p_ExpVectorAdd(pMonomM, pMonomMM, rRing); // "exponents" are additive!!!

    number nCoeffM = p_GetCoeff(pMonomM, rRing); // no new copy! should be deleted!

    if( (tpower) != 0 ) // degree is odd => negate coeff.
      nCoeffM = n_Neg(nCoeffM, rRing); // negate nCoeff (will destroy the original number), creates new number!

    const number nCoeffMM = p_GetCoeff(pMonomMM, rRing); // no new copy!

    number nCoeff = n_Mult(nCoeffM, nCoeffMM, rRing); // new number!

    p_SetCoeff(pMonomM, nCoeff, rRing); // delete lc(pMonomM) and set lc(pMonomM) = nCoeff

#ifdef PDEBUG
    p_LmTest(pMonomM, rRing);
#endif

    return(pMonomM);
}



// returns: result = lt(pMonom1) * lt(pMonom2),
// preserves pMonom1, pMonom2. may return NULL!
// if result != NULL => next(result) = NULL, lt(result) = lt(pMonom1) * lt(pMonom2)
static inline poly sca_mm_Mult_mm( poly pMonom1, const poly pMonom2, const ring rRing )
{
#ifdef PDEBUG
    p_Test(pMonom1, rRing);
    p_Test(pMonom2, rRing);
#endif

    const unsigned int iFirstAltVar = scaFirstAltVar(rRing);
    const unsigned int iLastAltVar = scaLastAltVar(rRing);

    register unsigned int tpower = 0;
    register unsigned int cpower = 0;

    for( register unsigned int j = iLastAltVar; j >= iFirstAltVar; j-- )
    {
      const unsigned int iExp1 = p_GetExp(pMonom1, j, rRing);
      const unsigned int iExp2 = p_GetExp(pMonom2, j, rRing);

#ifdef PDEBUG
      assume( iExp1 <= 1);
      assume( iExp2 <= 1);
#endif

      if( iExp2 != 0 )
      {
        if( iExp1 != 0 ) // result is zero!
        {
          return NULL;
        }
        tpower ^= cpower; // compute degree of (-1).
      }
      cpower ^= iExp1;
    }

#ifdef PDEBUG
    assume(cpower <= 1);
#endif

    poly pResult;
    p_AllocBin(pResult,rRing->PolyBin,rRing);

    p_ExpVectorSum(pResult, pMonom1, pMonom2, rRing); // "exponents" are additive!!!

    pNext(pResult) = NULL;

    const number nCoeff1 = p_GetCoeff(pMonom1, rRing); // no new copy!
    const number nCoeff2 = p_GetCoeff(pMonom2, rRing); // no new copy!

    number nCoeff = n_Mult(nCoeff1, nCoeff2, rRing); // new number!

    if( (tpower) != 0 ) // degree is odd => negate coeff.
      nCoeff = n_Neg(nCoeff, rRing); // negate nCoeff (will destroy the original number)

    p_SetCoeff0(pResult, nCoeff, rRing); // set lc(pResult) = nCoeff, no destruction!

#ifdef PDEBUG
    p_LmTest(pResult, rRing);
#endif

    return(pResult);
}

// returns: result =  x_i * lt(pMonom),
// preserves pMonom. may return NULL!
// if result != NULL => next(result) = NULL, lt(result) = x_i * lt(pMonom)
static inline poly sca_xi_Mult_mm(unsigned int i, const poly pMonom, const ring rRing)
{
#ifdef PDEBUG
    p_Test(pMonom, rRing);
#endif

    assume( i <= scaLastAltVar(rRing));
    assume( scaFirstAltVar(rRing) <= i );

    if( p_GetExp(pMonom, i, rRing) != 0 ) // => result is zero!
      return NULL;

    const unsigned int iFirstAltVar = scaFirstAltVar(rRing);

    register unsigned int cpower = 0;

    for( register unsigned int j = iFirstAltVar; j < i ; j++ )
      cpower ^= p_GetExp(pMonom, j, rRing);

#ifdef PDEBUG
    assume(cpower <= 1);
#endif

    poly pResult = p_LmInit(pMonom, rRing);

    p_SetExp(pResult, i, 1, rRing); // pResult*=X_i &&
    p_Setm(pResult, rRing);         // addjust degree after previous step!

    number nCoeff = n_Copy(p_GetCoeff(pMonom, rRing), rRing); // new number!

    if( cpower != 0 ) // degree is odd => negate coeff.
      nCoeff = n_Neg(nCoeff, rRing); // negate nCoeff (will destroy the original number)

    p_SetCoeff0(pResult, nCoeff, rRing); // set lc(pResult) = nCoeff, no destruction!

#ifdef PDEBUG
    p_LmTest(pResult, rRing);
#endif

    return(pResult);
}

//-----------------------------------------------------------------------------------//

// return poly = pPoly * pMonom; preserve pMonom, destroy or reuse pPoly.
poly sca_p_Mult_mm(poly pPoly, const poly pMonom, const ring rRing)
{
  assume( rIsSCA(rRing) );

#ifdef PDEBUG
//  Print("sca_p_Mult_mm\n"); // !

  p_Test(pPoly, rRing);
  p_Test(pMonom, rRing);
#endif

  if( pPoly == NULL )
    return NULL;

  assume(pMonom !=NULL);
  //if( pMonom == NULL )
  //{
  //  // pPoly != NULL =>
  //  p_Delete( &pPoly, rRing );
  //  return NULL;
  //}

  const int iComponentMonomM = p_GetComp(pMonom, rRing);

  poly p = pPoly; poly* ppPrev = &pPoly;

  loop
  {
#ifdef PDEBUG
    p_Test(p, rRing);
#endif
    const int iComponent = p_GetComp(p, rRing);

    if( iComponent!=0 )
    {
      if( iComponentMonomM!=0 ) // TODO: make global if on iComponentMonomM =?= 0
      {
        // REPORT_ERROR
        Werror("sca_p_Mult_mm: exponent mismatch %d and %d\n", iComponent, iComponentMonomM);
        // what should we do further?!?

        p_Delete( &pPoly, rRing); // delete the result AND rest
        return NULL;
      }
#ifdef PDEBUG
      if(iComponentMonomM==0 )
      {
        dReportError("sca_p_Mult_mm: Multiplication in the left module from the right");
      }
#endif
    }

    // terms will be in the same order because of quasi-ordering!
    poly v = sca_m_Mult_mm(p, pMonom, rRing);

    if( v != NULL )
    {
      ppPrev = &pNext(p); // fixed!

    // *p is changed if v != NULL ( p == v )
      pIter(p);

      if( p == NULL )
        break;
    }
    else
    { // Upps! Zero!!! we must kill this term!

      //
      p = p_LmDeleteAndNext(p, rRing);

      *ppPrev = p;

      if( p == NULL )
        break;
    }
  }

#ifdef PDEBUG
  p_Test(pPoly,rRing);
#endif

  return(pPoly);
}

// return new poly = pPoly * pMonom; preserve pPoly and pMonom.
poly sca_pp_Mult_mm(const poly pPoly, const poly pMonom, const ring rRing, poly &)
{
  assume( rIsSCA(rRing) );

#ifdef PDEBUG
//  Print("sca_pp_Mult_mm\n"); // !

  p_Test(pPoly, rRing);
  p_Test(pMonom, rRing);
#endif

  if( ( pPoly == NULL ) /*|| ( pMonom == NULL )*/ )
    return NULL;

  const int iComponentMonomM = p_GetComp(pMonom, rRing);

  poly pResult = NULL;
  poly* ppPrev = &pResult;

  for( poly p = pPoly; p!= NULL; pIter(p) )
  {
#ifdef PDEBUG
    p_Test(p, rRing);
#endif
    const int iComponent = p_GetComp(p, rRing);

    if( iComponent!=0 )
    {
      if( iComponentMonomM!=0 ) // TODO: make global if on iComponentMonomM =?= 0
      {
        // REPORT_ERROR
        Werror("sca_pp_Mult_mm: exponent mismatch %d and %d\n", iComponent, iComponentMonomM);
        // what should we do further?!?

        p_Delete( &pResult, rRing); // delete the result
        return NULL;
      }

#ifdef PDEBUG
      if(iComponentMonomM==0 )
      {
        dReportError("sca_pp_Mult_mm: Multiplication in the left module from the right");
      }
#endif
    }

    // terms will be in the same order because of quasi-ordering!
    poly v = sca_mm_Mult_mm(p, pMonom, rRing);

    if( v != NULL )
    {
      *ppPrev = v;
      ppPrev = &pNext(v);
    }
  }

#ifdef PDEBUG
  p_Test(pResult,rRing);
#endif

  return(pResult);
}

//-----------------------------------------------------------------------------------//

// return x_i * pPoly; preserve pPoly.
static inline poly sca_xi_Mult_pp(unsigned int i, const poly pPoly, const ring rRing)
{
  assume( rIsSCA(rRing) );

#ifdef PDEBUG
  p_Test(pPoly, rRing);
#endif

  assume(i <= scaLastAltVar(rRing));
  assume(scaFirstAltVar(rRing) <= i);

  if( pPoly == NULL )
    return NULL;

  poly pResult = NULL;
  poly* ppPrev = &pResult;

  for( poly p = pPoly; p!= NULL; pIter(p) )
  {

    // terms will be in the same order because of quasi-ordering!
    poly v = sca_xi_Mult_mm(i, p, rRing);

#ifdef PDEBUG
    p_Test(v, rRing);
#endif

    if( v != NULL )
    {
      *ppPrev = v;
      ppPrev = &pNext(*ppPrev);
    }
  }


#ifdef PDEBUG
  p_Test(pResult, rRing);
#endif

  return(pResult);
}


// return new poly = pMonom * pPoly; preserve pPoly and pMonom.
static poly sca_mm_Mult_pp(const poly pMonom, const poly pPoly, const ring rRing)
{
  assume( rIsSCA(rRing) );

#ifdef PDEBUG
//  Print("sca_mm_Mult_pp\n"); // !

  p_Test(pPoly, rRing);
  p_Test(pMonom, rRing);
#endif

  if ((pPoly==NULL) || (pMonom==NULL)) return NULL;

  assume( (pPoly != NULL) && (pMonom !=NULL));

  const int iComponentMonomM = p_GetComp(pMonom, rRing);

  poly pResult = NULL;
  poly* ppPrev = &pResult;

  for( poly p = pPoly; p!= NULL; pIter(p) )
  {
#ifdef PDEBUG
    p_Test(p, rRing);
#endif
    const int iComponent = p_GetComp(p, rRing);

    if( iComponentMonomM!=0 )
    {
      if( iComponent!=0 ) // TODO: make global if on iComponentMonomM =?= 0
      {
        // REPORT_ERROR
        Werror("sca_mm_Mult_pp: exponent mismatch %d and %d\n", iComponent, iComponentMonomM);
        // what should we do further?!?

        p_Delete( &pResult, rRing); // delete the result
        return NULL;
      }
#ifdef PDEBUG
      if(iComponent==0 )
      {
        dReportError("sca_mm_Mult_pp: Multiplication in the left module from the right!");
//        PrintS("mm = "); p_Write(pMonom, rRing);
//        PrintS("pp = "); p_Write(pPoly, rRing);
//        assume(iComponent!=0);
      }
#endif
    }

    // terms will be in the same order because of quasi-ordering!
    poly v = sca_mm_Mult_mm(pMonom, p, rRing);

    if( v != NULL )
    {
      *ppPrev = v;
      ppPrev = &pNext(*ppPrev); // nice line ;-)
    }
  }

#ifdef PDEBUG
  p_Test(pResult,rRing);
#endif

  return(pResult);
}


// return poly = pMonom * pPoly; preserve pMonom, destroy or reuse pPoly.
static poly sca_mm_Mult_p(const poly pMonom, poly pPoly, const ring rRing) // !!!!! the MOST used procedure !!!!!
{
  assume( rIsSCA(rRing) );

#ifdef PDEBUG
  p_Test(pPoly, rRing);
  p_Test(pMonom, rRing);
#endif

  if( pPoly == NULL )
    return NULL;

  assume(pMonom!=NULL);
  //if( pMonom == NULL )
  //{
  //  // pPoly != NULL =>
  //  p_Delete( &pPoly, rRing );
  //  return NULL;
  //}

  const int iComponentMonomM = p_GetComp(pMonom, rRing);

  poly p = pPoly; poly* ppPrev = &pPoly;

  loop
  {
#ifdef PDEBUG
    if( !p_Test(p, rRing) )
    {
      PrintS("p is wrong!");
      p_Write(p,rRing);
    }
#endif

    const int iComponent = p_GetComp(p, rRing);

    if( iComponentMonomM!=0 )
    {
      if( iComponent!=0 )
      {
        // REPORT_ERROR
        Werror("sca_mm_Mult_p: exponent mismatch %d and %d\n", iComponent, iComponentMonomM);
        // what should we do further?!?

        p_Delete( &pPoly, rRing); // delete the result
        return NULL;
      }
#ifdef PDEBUG
      if(iComponent==0)
      {
        dReportError("sca_mm_Mult_p: Multiplication in the left module from the right!");
//        PrintS("mm = "); p_Write(pMonom, rRing);
//        PrintS("p = "); p_Write(pPoly, rRing);
//        assume(iComponent!=0);
      }
#endif
    }

    // terms will be in the same order because of quasi-ordering!
    poly v = sca_mm_Mult_m(pMonom, p, rRing);

    if( v != NULL )
    {
      ppPrev = &pNext(p);

    // *p is changed if v != NULL ( p == v )
      pIter(p);

      if( p == NULL )
        break;
    }
    else
    { // Upps! Zero!!! we must kill this term!
      p = p_LmDeleteAndNext(p, rRing);

      *ppPrev = p;

      if( p == NULL )
        break;
    }
  }

#ifdef PDEBUG
    if( !p_Test(pPoly, rRing) )
    {
      PrintS("pPoly is wrong!");
      p_Write(pPoly, rRing);
    }
#endif

  return(pPoly);
}

//-----------------------------------------------------------------------------------//

#ifdef PDEBUG
#endif




//-----------------------------------------------------------------------------------//

// GB computation routines:

/*4
* creates the S-polynomial of p1 and p2
* does not destroy p1 and p2
*/
poly sca_SPoly( const poly p1, const poly p2, const ring r )
{
  assume( rIsSCA(r) );

  const long lCompP1 = p_GetComp(p1,r);
  const long lCompP2 = p_GetComp(p2,r);

  if ((lCompP1!=lCompP2) && (lCompP1!=0) && (lCompP2!=0))
  {
#ifdef PDEBUG
    dReportError("sca_SPoly: different non-zero components!\n");
#endif
    return(NULL);
  }

  poly pL = p_Lcm(p1, p2, si_max(lCompP1, lCompP2), r);       // pL = lcm( lm(p1), lm(p2) )

  poly m1 = p_One( r);
  p_ExpVectorDiff(m1, pL, p1, r);                  // m1 = pL / lm(p1)

  //p_SetComp(m1,0,r);
  //p_Setm(m1,r);
#ifdef PDEBUG
  p_Test(m1,r);
#endif


  poly m2 = p_One( r);
  p_ExpVectorDiff (m2, pL, p2, r);                  // m2 = pL / lm(p2)

  //p_SetComp(m2,0,r);
  //p_Setm(m2,r);
#ifdef PDEBUG
  p_Test(m2,r);
#endif

  p_Delete(&pL,r);

  number C1  = n_Copy(p_GetCoeff(p1,r),r);      // C1 = lc(p1)
  number C2  = n_Copy(p_GetCoeff(p2,r),r);      // C2 = lc(p2)

  number C = nGcd(C1,C2,r);                     // C = gcd(C1, C2)

  if (!n_IsOne(C, r))                              // if C != 1
  {
    C1=n_Div(C1, C, r);                              // C1 = C1 / C
    C2=n_Div(C2, C, r);                              // C2 = C2 / C
  }

  n_Delete(&C,r); // destroy the number C

  const int iSignSum = sca_Sign_mm_Mult_mm (m1, p1, r) + sca_Sign_mm_Mult_mm (m2, p2, r);
  // zero if different signs

  assume( (iSignSum*iSignSum == 0) || (iSignSum*iSignSum == 4) );

  if( iSignSum != 0 ) // the same sign!
    C2=n_Neg (C2, r);

  p_SetCoeff(m1, C2, r);                           // lc(m1) = C2!!!
  p_SetCoeff(m2, C1, r);                           // lc(m2) = C1!!!

  poly tmp1 = nc_mm_Mult_pp (m1, pNext(p1), r);    // tmp1 = m1 * tail(p1),
  p_Delete(&m1,r);  //  => n_Delete(&C1,r);

  poly tmp2 = nc_mm_Mult_pp (m2, pNext(p2), r);    // tmp1 = m2 * tail(p2),
  p_Delete(&m2,r);  //  => n_Delete(&C1,r);

  poly spoly = p_Add_q (tmp1, tmp2, r); // spoly = spoly(lt(p1), lt(p2)) + m1 * tail(p1), delete tmp1,2

  if (spoly!=NULL) p_Cleardenom (spoly, r);
//  if (spoly!=NULL) p_Content (spoly); // r?

#ifdef PDEBUG
  p_Test (spoly, r);
#endif

  return(spoly);
}




/*2
* reduction of p2 with p1
* do not destroy p1, but p2
* p1 divides p2 -> for use in NF algorithm
*/
poly sca_ReduceSpoly(const poly p1, poly p2, const ring r)
{
  assume( rIsSCA(r) );

  assume( p1 != NULL );

  const long lCompP1 = p_GetComp (p1, r);
  const long lCompP2 = p_GetComp (p2, r);

  if ((lCompP1!=lCompP2) && (lCompP1!=0) && (lCompP2!=0))
  {
#ifdef PDEBUG
    dReportError("sca_ReduceSpoly: different non-zero components!");
#endif
    return(NULL);
  }

  poly m = p_ISet (1, r);
  p_ExpVectorDiff (m, p2, p1, r);                      // m = lm(p2) / lm(p1)
  //p_Setm(m,r);
#ifdef PDEBUG
  p_Test (m,r);
#endif

  number C1 = n_Copy( p_GetCoeff(p1, r), r);
  number C2 = n_Copy( p_GetCoeff(p2, r), r);

  /* GCD stuff */
  number C = nGcd(C1, C2, r);

  if (!n_IsOne(C, r))
  {
    C1 = n_Div(C1, C, r);
    C2 = n_Div(C2, C, r);
  }
  n_Delete(&C,r);

  const int iSign = sca_Sign_mm_Mult_mm( m, p1, r );

  if(iSign == 1)
    C2 = n_Neg(C2,r);

  p_SetCoeff(m, C2, r);

#ifdef PDEBUG
  p_Test(m,r);
#endif

  p2 = p_LmDeleteAndNext( p2, r );

  p2 = p_Mult_nn(p2, C1, r); // p2 !!!
  n_Delete(&C1,r);

  poly T = nc_mm_Mult_pp(m, pNext(p1), r);
  p_Delete(&m, r);

  p2 = p_Add_q(p2, T, r);

  if ( p2!=NULL ) p_Content(p2,r);

#ifdef PDEBUG
  p_Test(p2,r);
#endif

  return(p2);
}


void addLObject(LObject& h, kStrategy& strat)
{
  if(h.IsNull()) return;

  strat->initEcart(&h);
  h.sev=0; // pGetShortExpVector(h.p);

  // add h into S and L
  int pos=posInS(strat, strat->sl, h.p, h.ecart);

  if ( (pos <= strat->sl) && (pComparePolys(h.p, strat->S[pos])) )
  {
    if (TEST_OPT_PROT)
      PrintS("d\n");
  }
  else
  {
    if (TEST_OPT_INTSTRATEGY)
    {
      p_Cleardenom(h.p, currRing);
    }
    else
    {
      pNorm(h.p);
      p_Content(h.p,currRing);
    }

    if ((strat->syzComp==0)||(!strat->homog))
    {
      h.p = redtailBba(h.p,pos-1,strat);

      if (TEST_OPT_INTSTRATEGY)
      {
//        pCleardenom(h.p);
        p_Content(h.p,currRing);
      }
      else
      {
        pNorm(h.p);
      }
    }

    if(h.IsNull()) return;

    /* statistic */
    if (TEST_OPT_PROT)
    {
      PrintS("s\n");
    }

#ifdef KDEBUG
    if (TEST_OPT_DEBUG)
    {
      PrintS("new s:");
      wrp(h.p);
      PrintLn();
    }
#endif

    enterpairs(h.p, strat->sl, h.ecart, 0, strat);

    pos=0;

    if (strat->sl!=-1) pos = posInS(strat, strat->sl, h.p, h.ecart);
    strat->enterS(h, pos, strat, -1);
//    enterT(h, strat); // ?!

    if (h.lcm!=NULL) pLmFree(h.lcm);
  }


}





ideal sca_gr_bba(const ideal F, const ideal Q, const intvec *, const intvec *, kStrategy strat)
{
#if MYTEST
   PrintS("<sca_gr_bba>\n");
#endif

  assume(rIsSCA(currRing));

#ifndef NDEBUG
  idTest(F);
  idTest(Q);
#endif

#ifdef HAVE_PLURAL
#if MYTEST
  PrintS("currRing: \n");
  rWrite(currRing);
#ifdef RDEBUG
  rDebugPrint(currRing);
#endif

  PrintS("F: \n");
  idPrint(F);
  PrintS("Q: \n");
  idPrint(Q);
#endif
#endif


  const unsigned int m_iFirstAltVar = scaFirstAltVar(currRing);
  const unsigned int m_iLastAltVar  = scaLastAltVar(currRing);

  ideal tempF = id_KillSquares(F, m_iFirstAltVar, m_iLastAltVar, currRing);
  ideal tempQ = Q;

  if(Q == currQuotient)
    tempQ = SCAQuotient(currRing);

  strat->z2homog = id_IsSCAHomogeneous(tempF, NULL, NULL, currRing); // wCx == wCy == NULL!
  // redo: no_prod_crit
  const BOOLEAN bIsSCA  = rIsSCA(currRing) && strat->z2homog; // for Z_2 prod-crit
  strat->no_prod_crit   = ! bIsSCA;

//  strat->homog = strat->homog && strat->z2homog; // ?

#if MYTEST
  {
    PrintS("ideal tempF: \n");
    idPrint(tempF);
    PrintS("ideal tempQ: \n");
    idPrint(tempQ);
  }
#endif

#ifdef KDEBUG
  om_Opts.MinTrack = 5;
#endif

  int srmax, lrmax;
  int olddeg, reduc;
  int red_result = 1;
//  int hilbeledeg = 1, minimcnt = 0;
  int hilbcount = 0;

  initBuchMoraCrit(strat); // set Gebauer, honey, sugarCrit

  nc_gr_initBba(tempF,strat); // set enterS, red, initEcart, initEcartPair

  initBuchMoraPos(strat);


  // ?? set spSpolyShort, reduce ???

  initBuchMora(tempF, tempQ, strat); // SCAQuotient(currRing) instead of Q == squares!!!!!!!

  strat->posInT=posInT110; // !!!

  srmax = strat->sl;
  reduc = olddeg = lrmax = 0;


  /* compute------------------------------------------------------- */
  for(; strat->Ll >= 0;
#ifdef KDEBUG
    strat->P.lcm = NULL,
#endif
    kTest(strat)
    )
  {
    if (strat->Ll > lrmax) lrmax =strat->Ll;/*stat.*/

#ifdef KDEBUG
    if (TEST_OPT_DEBUG) messageSets(strat);
#endif

    if (strat->Ll== 0) strat->interpt=TRUE;

    if (TEST_OPT_DEGBOUND
    && ((strat->honey
    && (strat->L[strat->Ll].ecart+pFDeg(strat->L[strat->Ll].p,currRing)>Kstd1_deg))
       || ((!strat->honey) && (pFDeg(strat->L[strat->Ll].p,currRing)>Kstd1_deg))))
    {
      /*
      *stops computation if
      * 24 IN test and the degree +ecart of L[strat->Ll] is bigger then
      *a predefined number Kstd1_deg
      */
      while (strat->Ll >= 0) deleteInL(strat->L,&strat->Ll,strat->Ll,strat);
      break;
    }

    /* picks the last element from the lazyset L */
    strat->P = strat->L[strat->Ll];
    strat->Ll--;

    //kTest(strat);

//    assume(pNext(strat->P.p) != strat->tail); // !???
    if(strat->P.IsNull()) continue;


    if( pNext(strat->P.p) == strat->tail )
    {
      // deletes the int spoly and computes SPoly
      pLmFree(strat->P.p); // ???
      strat->P.p = nc_CreateSpoly(strat->P.p1, strat->P.p2, currRing);
    }

    if(strat->P.IsNull()) continue;

//     poly save = NULL;
//
//     if(pNext(strat->P.p) != NULL)
//       save = p_Copy(strat->P.p, currRing);

    strat->initEcart(&strat->P); // remove it?

    if (TEST_OPT_PROT)
      message((strat->honey ? strat->P.ecart : 0) + strat->P.pFDeg(), &olddeg,&reduc,strat, red_result);

    // reduction of the element chosen from L wrt S
    strat->red(&strat->P,strat);

    if(strat->P.IsNull()) continue;

    addLObject(strat->P, strat);

    if (strat->sl > srmax) srmax = strat->sl;

    const poly save = strat->P.p;

#ifdef PDEBUG
      p_Test(save, currRing);
#endif
    assume( save != NULL );

    // SCA Specials:

    {
      const poly p_next = pNext(save);

      if( p_next != NULL )
      for( unsigned int i = m_iFirstAltVar; i <= m_iLastAltVar; i++ )
      if( p_GetExp(save, i, currRing) != 0 )
      {
        assume(p_GetExp(save, i, currRing) == 1);

        const poly tt = sca_pp_Mult_xi_pp(i, p_next, currRing);

#ifdef PDEBUG
        p_Test(tt, currRing);
#endif

        if( tt == NULL) continue;

        LObject h(tt); // h = x_i * P

        if (TEST_OPT_INTSTRATEGY)
        {
//           h.pCleardenom(); // also does a p_Content
          p_Content(h.p,currRing);
        }
        else
        {
          h.pNorm();
        }

        strat->initEcart(&h);


//         if (pOrdSgn==-1)
//         {
//           cancelunit(&h);  // tries to cancel a unit
//           deleteHC(&h, strat);
//         }

//         if(h.IsNull()) continue;

//         if (TEST_OPT_PROT)
//           message((strat->honey ? h.ecart : 0) + h.pFDeg(), &olddeg, &reduc, strat, red_result);

//         strat->red(&h, strat); // wrt S
//         if(h.IsNull()) continue;

//         poly save = p_Copy(h.p, currRing);

        int pos;

        if (strat->Ll==-1)
          pos =0;
        else
          pos = strat->posInL(strat->L,strat->Ll,&h,strat);

        h.sev = pGetShortExpVector(h.p);
        enterL(&strat->L,&strat->Ll,&strat->Lmax,h,pos);

  //       h.p = save;
  //       addLObject(h, strat);

//         if (strat->sl > srmax) srmax = strat->sl;
      }

      // p_Delete( &save, currRing );
    }


  } // for(;;)


#ifdef KDEBUG
  if (TEST_OPT_DEBUG) messageSets(strat);
#endif

  if (TEST_OPT_REDSB){
    completeReduce(strat); // ???
  }

  /* release temp data-------------------------------- */
  exitBuchMora(strat);

  if (TEST_OPT_WEIGHTM)
  {
    pFDeg=pFDegOld;
    pLDeg=pLDegOld;
    if (ecartWeights)
    {
      omFreeSize((ADDRESS)ecartWeights,(pVariables+1)*sizeof(int));
      ecartWeights=NULL;
    }
  }

  if (TEST_OPT_PROT) messageStat(srmax,lrmax,hilbcount,strat);

  if (tempQ!=NULL) updateResult(strat->Shdl,tempQ,strat);

  id_Delete(&tempF, currRing);


  /* complete reduction of the standard basis--------- */
  if (TEST_OPT_REDSB){
    ideal I = strat->Shdl;
    ideal erg = kInterRedOld(I,tempQ);
    assume(I!=erg);
    id_Delete(&I, currRing);
    strat->Shdl = erg;
  }


#if MYTEST
//   PrintS("</sca_gr_bba>\n");
#endif

  return (strat->Shdl);
}


// should be used only inside nc_SetupQuotient!
// Check whether this our case:
//  1. rG is  a commutative polynomial ring \otimes anticommutative algebra
//  2. factor ideal rGR->qideal contains squares of all alternating variables.
//
// if yes, make rGR a super-commutative algebra!
// NOTE: Factors of SuperCommutative Algebras are supported this way!
//
//  rG == NULL means that there is no separate base G-algebra in this case take rGR == rG

// special case: bCopy == true (default value: false)
// meaning: rGR copies structure from rG
// (maybe with some minor changes, which don't change the type!)
bool sca_SetupQuotient(ring rGR, ring rG, bool bCopy)
{

  //////////////////////////////////////////////////////////////////////////
  // checks...
  //////////////////////////////////////////////////////////////////////////
  if( rG == NULL )
    rG = rGR;

  assume(rGR != NULL);
  assume(rG  != NULL);
  assume(rIsPluralRing(rG));

#if ((defined(PDEBUG) && OUTPUT) || MYTEST)
  PrintS("sca_SetupQuotient(rGR, rG, bCopy)");

  {
    ring rSaveRing = assureCurrentRing(rG);

    PrintS("\nrG: \n"); rWrite(rG);

    assureCurrentRing(rGR);

    PrintS("\nrGR: \n"); rWrite(rGR);

    PrintLn();

    assureCurrentRing(rSaveRing);
  }
#endif


  if(bCopy)
  {
    if(rIsSCA(rG) && (rG != rGR))
      return sca_Force(rGR, scaFirstAltVar(rG), scaLastAltVar(rG));
    else
      return false;
  }

  assume(!bCopy);

  const int N = rG->N;

  if(N < 2)
    return false;


//  if( (ncRingType(rG) != nc_skew) || (ncRingType(rG) != nc_comm) )
//    return false;

#if OUTPUT
  PrintS("sca_SetupQuotient: qring?\n");
#endif

  if(rGR->qideal == NULL) // there should be a factor!
    return false;

#if OUTPUT
  PrintS("sca_SetupQuotient: qideal!!!\n");
#endif

//  if((rG->qideal != NULL) && (rG != rGR) ) // we cannot change from factor to factor at the time, sorry!
//    return false;


  int iAltVarEnd = -1;
  int iAltVarStart   = N+1;

  const nc_struct* NC = rG->GetNC();
  const ring rBase = rG; //NC->basering;
  const matrix C   = NC->C; // live in rBase!
  const matrix D   = NC->D; // live in rBase!

#if OUTPUT
  PrintS("sca_SetupQuotient: AltVars?!\n");
#endif

  for(int i = 1; i < N; i++)
  {
    for(int j = i + 1; j <= N; j++)
    {
      if( MATELEM(D,i,j) != NULL) // !!!???
      {
#if ((defined(PDEBUG) && OUTPUT) || MYTEST)
        Print("Nonzero D[%d, %d]\n", i, j);
#endif
        return false;
      }


      assume(MATELEM(C,i,j) != NULL); // after CallPlural!
      number c = p_GetCoeff(MATELEM(C,i,j), rBase);

      if( n_IsMOne(c, rBase) ) // !!!???
      {
        if( i < iAltVarStart)
          iAltVarStart = i;

        if( j > iAltVarEnd)
          iAltVarEnd = j;
      } else
      {
        if( !n_IsOne(c, rBase) )
        {
#if ((defined(PDEBUG) && OUTPUT) || MYTEST)
          Print("Wrong Coeff at: [%d, %d]\n", i, j);
#endif
          return false;
        }
      }
    }
  }

#if ((defined(PDEBUG) && OUTPUT) || MYTEST)
  Print("AltVars?1: [%d, %d]\n", iAltVarStart, iAltVarEnd);
#endif


  if( (iAltVarEnd == -1) || (iAltVarStart == (N+1)) )
    return false; // either no alternating varables, or a single one => we are in commutative case!


  for(int i = 1; i < N; i++)
  {
    for(int j = i + 1; j <= N; j++)
    {
      assume(MATELEM(C,i,j) != NULL); // after CallPlural!
      number c = p_GetCoeff(MATELEM(C,i,j), rBase);

      if( (iAltVarStart <= i) && (j <= iAltVarEnd) ) // S <= i < j <= E
      { // anticommutative part
        if( !n_IsMOne(c, rBase) )
        {
#if ((defined(PDEBUG) && OUTPUT) || MYTEST)
           Print("Wrong Coeff at: [%d, %d]\n", i, j);
#endif
          return false;
        }
      } else
      { // should commute
        if( !n_IsOne(c, rBase) )
        {
#if ((defined(PDEBUG) && OUTPUT) || MYTEST)
           Print("Wrong Coeff at: [%d, %d]\n", i, j);
#endif
          return false;
        }
      }
    }
  }

#if ((defined(PDEBUG) && OUTPUT) || MYTEST)
  Print("AltVars!?: [%d, %d]\n", iAltVarStart, iAltVarEnd);
#endif

  assume( 1            <= iAltVarStart );
  assume( iAltVarStart < iAltVarEnd   );
  assume( iAltVarEnd   <= N            );


  ring rSaveRing = assureCurrentRing(rG);


  assume(rGR->qideal != NULL);
  assume(rGR->N == rG->N);
//  assume(rG->qideal == NULL); // ?

  const ideal idQuotient = rGR->qideal;


#if ((defined(PDEBUG) && OUTPUT) || MYTEST)
  PrintS("Analyzing quotient ideal:\n");
  idPrint(idQuotient); // in rG!!!
#endif


  // check for
  // y_{iAltVarStart}^2, y_{iAltVarStart+1}^2, \ldots, y_{iAltVarEnd}^2  (iAltVarEnd > iAltVarStart)
  // to be within quotient ideal.

  bool bSCA = true;

  int b = N+1;
  int e = -1;

  if(rIsSCA(rG))
  {
    b = si_min(b, scaFirstAltVar(rG));
    e = si_max(e, scaLastAltVar(rG));

#if ((defined(PDEBUG) && OUTPUT) || MYTEST)
    Print("AltVars!?: [%d, %d]\n", b, e);
#endif
  }

  for ( int i = iAltVarStart; (i <= iAltVarEnd) && bSCA; i++ )
    if( (i < b) || (i > e) ) // otherwise it's ok since rG is an SCA!
  {
    poly square = p_One( rG);
    p_SetExp(square, i, 2, rG); // square = var(i)^2.
    p_Setm(square, rG);

    // square = NF( var(i)^2 | Q )
    // NOTE: rSaveRing == currRing now!
    // NOTE: there is no better way to check this in general!
    square = kNF(idQuotient, NULL, square, 0, 1); // must ran in currRing == rG!

    if( square != NULL ) // var(i)^2 is not in Q?
    {
      p_Delete(&square, rG);
      bSCA = false;
      break;
    }
  }

  assureCurrentRing(rSaveRing);

  if(!bSCA) return false;


#if ((defined(PDEBUG) && OUTPUT) || MYTEST)
  Print("ScaVars!: [%d, %d]\n", iAltVarStart, iAltVarEnd);
#endif


  //////////////////////////////////////////////////////////////////////////
  // ok... here we go. let's setup it!!!
  //////////////////////////////////////////////////////////////////////////
  ideal tempQ = id_KillSquares(idQuotient, iAltVarStart, iAltVarEnd, rG); // in rG!!!


#if ((defined(PDEBUG) && OUTPUT) || MYTEST)
  PrintS("Quotient: \n");
  iiWriteMatrix((matrix)idQuotient,"__",1);
  PrintS("tempSCAQuotient: \n");
  iiWriteMatrix((matrix)tempQ,"__",1);
#endif

  idSkipZeroes( tempQ );

  ncRingType( rGR, nc_exterior );

  scaFirstAltVar( rGR, iAltVarStart );
  scaLastAltVar( rGR, iAltVarEnd );

  if( idIs0(tempQ) )
    rGR->GetNC()->SCAQuotient() = NULL;
  else
    rGR->GetNC()->SCAQuotient() = idrMoveR(tempQ, rG, rGR); // deletes tempQ!

  nc_p_ProcsSet(rGR, rGR->p_Procs); // !!!!!!!!!!!!!!!!!


#if ((defined(PDEBUG) && OUTPUT) || MYTEST)
  PrintS("SCAQuotient: \n");
  if(tempQ != NULL)
    iiWriteMatrix((matrix)tempQ,"__",1);
  else
    PrintS("(NULL)\n");
#endif

  return true;
}


bool sca_Force(ring rGR, int b, int e)
{
  assume(rGR != NULL);
  assume(rIsPluralRing(rGR));
  assume(!rIsSCA(rGR));

  const int N = rGR->N;

  ring rSaveRing = currRing;

  if(rSaveRing != rGR)
    rChangeCurrRing(rGR);

  const ideal idQuotient = rGR->qideal;

  ideal tempQ = idQuotient;

  if( b <= N && e >= 1 )
    tempQ = id_KillSquares(idQuotient, b, e, rGR);

  idSkipZeroes( tempQ );

  ncRingType( rGR, nc_exterior );

  if( idIs0(tempQ) )
    rGR->GetNC()->SCAQuotient() = NULL;
  else
    rGR->GetNC()->SCAQuotient() = tempQ;


  scaFirstAltVar( rGR, b );
  scaLastAltVar( rGR, e );


  nc_p_ProcsSet(rGR, rGR->p_Procs);

  if(rSaveRing != rGR)
    rChangeCurrRing(rSaveRing);

  return true;
}

// return x_i * pPoly; preserve pPoly.
poly sca_pp_Mult_xi_pp(unsigned int i, const poly pPoly, const ring rRing)
{
  assume(1 <= i);
  assume(i <= (unsigned int)rRing->N);

  if(rIsSCA(rRing))
    return sca_xi_Mult_pp(i, pPoly, rRing);



  poly xi =  p_One( rRing);
  p_SetExp(xi, i, 1, rRing);
  p_Setm(xi, rRing);

  poly pResult = pp_Mult_qq(xi, pPoly, rRing);

  p_Delete( &xi, rRing);

  return pResult;

}


///////////////////////////////////////////////////////////////////////////////////////
//************* SCA BBA *************************************************************//
///////////////////////////////////////////////////////////////////////////////////////

// Under development!!!
ideal sca_bba (const ideal F, const ideal Q, const intvec *w, const intvec * /*hilb*/, kStrategy strat)
{
#if MYTEST
  PrintS("\n\n<sca_bba>\n\n");
#endif

  assume(rIsSCA(currRing));

#ifndef NDEBUG
  idTest(F);
  idTest(Q);
#endif

#if MYTEST
  PrintS("\ncurrRing: \n");
  rWrite(currRing);
#ifdef RDEBUG
//  rDebugPrint(currRing);
#endif

  PrintS("\n\nF: \n");
  idPrint(F);
  PrintS("\n\nQ: \n");
  idPrint(Q);

  PrintLn();
#endif


  const unsigned int m_iFirstAltVar = scaFirstAltVar(currRing);
  const unsigned int m_iLastAltVar  = scaLastAltVar(currRing);

  ideal tempF = id_KillSquares(F, m_iFirstAltVar, m_iLastAltVar, currRing);

  ideal tempQ = Q;

  if(Q == currQuotient)
    tempQ = SCAQuotient(currRing);

  // Q or tempQ will not be used below :(((


#if MYTEST

  PrintS("tempF: \n");
  idPrint(tempF);
  PrintS("tempQ: \n");
  idPrint(tempQ);
#endif

  strat->z2homog = id_IsSCAHomogeneous(tempF, NULL, NULL, currRing); // wCx == wCy == NULL!
   // redo no_prod_crit:
  const BOOLEAN bIsSCA  = rIsSCA(currRing) && strat->z2homog; // for Z_2 prod-crit
  strat->no_prod_crit   = ! bIsSCA;

//  strat->homog = strat->homog && strat->z2homog; // ?



#ifdef KDEBUG
  om_Opts.MinTrack = 5;
#endif

  int   srmax, lrmax, red_result = 1;
  int   olddeg, reduc;

//  int hilbeledeg = 1, minimcnt = 0;
  int hilbcount = 0;

  BOOLEAN withT = FALSE;

  initBuchMoraCrit(strat); // sets Gebauer, honey, sugarCrit // sca - ok???
  initBuchMoraPos(strat); // sets strat->posInL, strat->posInT // check!! (Plural's: )

//   initHilbCrit(F, Q, &hilb, strat);

//  nc_gr_initBba(F,strat);
  initBba(tempF, strat); // set enterS, red, initEcart, initEcartPair

  /*set enterS, spSpolyShort, reduce, red, initEcart, initEcartPair*/
  // ?? set spSpolyShort, reduce ???
  initBuchMora(tempF, tempQ, strat); // tempQ = Q without squares!!!

//   if (strat->minim>0) strat->M = idInit(IDELEMS(F),F->rank);

  srmax = strat->sl;
  reduc = olddeg = lrmax = 0;

#define NO_BUCKETS

#ifndef NO_BUCKETS
  if (!TEST_OPT_NOT_BUCKETS)
    strat->use_buckets = 1;
#endif

  // redtailBBa against T for inhomogenous input
  if (!TEST_OPT_OLDSTD)
    withT = ! strat->homog;

  // strat->posInT = posInT_pLength;
  kTest_TS(strat);

#undef HAVE_TAIL_RING

#ifdef HAVE_TAIL_RING
  if(!idIs0(F) &&(!rField_is_Ring()))  // create strong gcd poly computes with tailring and S[i] ->to be fixed
    kStratInitChangeTailRing(strat);
#endif
  if (BVERBOSE(23))
  {
    if (test_PosInT!=NULL) strat->posInT=test_PosInT;
    if (test_PosInL!=NULL) strat->posInL=test_PosInL;
    kDebugPrint(strat);
  }


  ///////////////////////////////////////////////////////////////
  // SCA:

  //  due to std( SB, p).
  // Note that after initBuchMora :: initSSpecial all these additional
  // elements are in S and T (and some pairs are in L, which also has no initiall
  // elements!!!)
  if(TEST_OPT_SB_1)
  {
    // For all additional elements...
    for (int iNewElement = strat->newIdeal; iNewElement < IDELEMS(tempF); iNewElement++)
    {
      const poly pSave = tempF->m[iNewElement];

      if( pSave != NULL )
      {
//        tempF->m[iNewElement] = NULL;

        const poly p_next = pNext(pSave);

        if(p_next != NULL)
          for( unsigned int i = m_iFirstAltVar; i <= m_iLastAltVar; i++ )
            if( p_GetExp(pSave, i, currRing) != 0 )
            {
              assume(p_GetExp(pSave, i, currRing) == 1);

              const poly p_new = sca_pp_Mult_xi_pp(i, p_next, currRing);

#ifdef PDEBUG
              p_Test(p_new, currRing);
#endif

              if( p_new == NULL) continue;

              LObject h(p_new); // h = x_i * strat->P
              h.is_special = TRUE;

              if (TEST_OPT_INTSTRATEGY)
                h.pCleardenom(); // also does a p_Content
              else
                h.pNorm();

              strat->initEcart(&h);
              h.sev = pGetShortExpVector(h.p);

              int pos = 0;

              if (strat->Ll != -1)
                pos = strat->posInL(strat->L,strat->Ll,&h,strat);

              enterL(&strat->L,&strat->Ll,&strat->Lmax,h,pos);
            }
      }
    }
  }

  /* compute------------------------------------------------------- */
  while (strat->Ll >= 0)
  {
    if (strat->Ll > lrmax) lrmax =strat->Ll;/*stat.*/

#ifdef KDEBUG
//     loop_count++;
    if (TEST_OPT_DEBUG) messageSets(strat);
#endif

    if (strat->Ll== 0) strat->interpt=TRUE;

    if (TEST_OPT_DEGBOUND
        && ((strat->honey && (strat->L[strat->Ll].ecart+pFDeg(strat->L[strat->Ll].p,currRing)>Kstd1_deg))
            || ((!strat->honey) && (pFDeg(strat->L[strat->Ll].p,currRing)>Kstd1_deg))))
    {

#ifdef KDEBUG
//      if (TEST_OPT_DEBUG){PrintS("^^^^?");}
#endif

      /*
       *stops computation if
       * 24 IN test and the degree +ecart of L[strat->Ll] is bigger then
       *a predefined number Kstd1_deg
       */
      while ((strat->Ll >= 0)
        && ( (strat->homog==isHomog) || strat->L[strat->Ll].is_special || ((strat->L[strat->Ll].p1!=NULL) && (strat->L[strat->Ll].p2!=NULL)) )
        && ((strat->honey && (strat->L[strat->Ll].ecart+pFDeg(strat->L[strat->Ll].p,currRing)>Kstd1_deg))
            || ((!strat->honey) && (pFDeg(strat->L[strat->Ll].p,currRing)>Kstd1_deg)))
            )
      {
#ifdef KDEBUG
//        if (TEST_OPT_DEBUG){PrintS("^^^^^^^^^^^^!!!!");}
#endif
        deleteInL(strat->L,&strat->Ll,strat->Ll,strat);
//        if (TEST_OPT_PROT) PrintS("^!");
      }
      if (strat->Ll<0) break;
      else strat->noClearS=TRUE;
    }

    /* picks the last element from the lazyset L */
    strat->P = strat->L[strat->Ll];
    strat->Ll--;


//    assume(pNext(strat->P.p) != strat->tail);

    if(strat->P.IsNull()) continue;

    if (pNext(strat->P.p) == strat->tail)
    {
      // deletes the short spoly
      pLmFree(strat->P.p);

      strat->P.p = nc_CreateSpoly(strat->P.p1, strat->P.p2, currRing);
      if (strat->P.p!=NULL) strat->initEcart(&strat->P);
    }//    else


    if(strat->P.IsNull()) continue;

    if (strat->P.p1 == NULL)
    {
//       if (strat->minim > 0)
//         strat->P.p2=p_Copy(strat->P.p, currRing, strat->tailRing);


      // for input polys, prepare reduction
        strat->P.PrepareRed(strat->use_buckets);
    }

    if (TEST_OPT_PROT)
      message((strat->honey ? strat->P.ecart : 0) + strat->P.pFDeg(),
              &olddeg,&reduc,strat, red_result);

    /* reduction of the element chosen from L */
    red_result = strat->red(&strat->P,strat);


    // reduction to non-zero new poly
    if (red_result == 1)
    {
      /* statistic */
      if (TEST_OPT_PROT) PrintS("s");

      // get the polynomial (canonicalize bucket, make sure P.p is set)
      strat->P.GetP(strat->lmBin);

      int pos = posInS(strat,strat->sl,strat->P.p,strat->P.ecart);

      // reduce the tail and normalize poly
      if (TEST_OPT_INTSTRATEGY)
      {
        strat->P.pCleardenom();
        if ((TEST_OPT_REDSB)||(TEST_OPT_REDTAIL))
        {
          strat->P.p = redtailBba(&(strat->P),pos-1,strat, withT); // !!!
          strat->P.pCleardenom();
        }
      }
      else
      {
        strat->P.pNorm();
        if ((TEST_OPT_REDSB)||(TEST_OPT_REDTAIL))
          strat->P.p = redtailBba(&(strat->P),pos-1,strat, withT);
      }
      strat->P.is_normalized=nIsOne(pGetCoeff(strat->P.p));

#ifdef KDEBUG
      if (TEST_OPT_DEBUG){PrintS(" ns:");p_wrp(strat->P.p,currRing);PrintLn();}
#endif

//       // min_std stuff
//       if ((strat->P.p1==NULL) && (strat->minim>0))
//       {
//         if (strat->minim==1)
//         {
//           strat->M->m[minimcnt]=p_Copy(strat->P.p,currRing,strat->tailRing);
//           p_Delete(&strat->P.p2, currRing, strat->tailRing);
//         }
//         else
//         {
//           strat->M->m[minimcnt]=strat->P.p2;
//           strat->P.p2=NULL;
//         }
//         if (strat->tailRing!=currRing && pNext(strat->M->m[minimcnt])!=NULL)
//           pNext(strat->M->m[minimcnt])
//             = strat->p_shallow_copy_delete(pNext(strat->M->m[minimcnt]),
//                                            strat->tailRing, currRing,
//                                            currRing->PolyBin);
//         minimcnt++;
//       }

      // enter into S, L, and T
      //if(withT)
        enterT(strat->P, strat);

      // L
      enterpairs(strat->P.p,strat->sl,strat->P.ecart,pos,strat, strat->tl);

      // posInS only depends on the leading term
      strat->enterS(strat->P, pos, strat, strat->tl);

//       if (hilb!=NULL) khCheck(Q,w,hilb,hilbeledeg,hilbcount,strat);

//      Print("[%d]",hilbeledeg);
      if (strat->P.lcm!=NULL) pLmFree(strat->P.lcm);

      if (strat->sl>srmax) srmax = strat->sl;

      // //////////////////////////////////////////////////////////
      // SCA:
      const poly pSave = strat->P.p;
      const poly p_next = pNext(pSave);

//       if(0)
      if( p_next != NULL )
      for( unsigned int i = m_iFirstAltVar; i <= m_iLastAltVar; i++ )
      if( p_GetExp(pSave, i, currRing) != 0 )
      {
        assume(p_GetExp(pSave, i, currRing) == 1);
        const poly p_new = sca_pp_Mult_xi_pp(i, p_next, currRing);

#ifdef PDEBUG
        p_Test(p_new, currRing);
#endif

        if( p_new == NULL) continue;

        LObject h(p_new); // h = x_i * strat->P

        h.is_special = TRUE;

        if (TEST_OPT_INTSTRATEGY)
        {
//          p_Content(h.p);
          h.pCleardenom(); // also does a p_Content
        }
        else
        {
          h.pNorm();
        }

        strat->initEcart(&h);
        h.sev = pGetShortExpVector(h.p);

        int pos = 0;

        if (strat->Ll != -1)
          pos = strat->posInL(strat->L,strat->Ll,&h,strat);

        enterL(&strat->L,&strat->Ll,&strat->Lmax,h,pos);
/*
        h.sev = pGetShortExpVector(h.p);
        strat->initEcart(&h);

        h.PrepareRed(strat->use_buckets);

        // reduction of the element chosen from L(?)
        red_result = strat->red(&h,strat);

        // reduction to non-zero new poly
        if (red_result != 1) continue;


        int pos = posInS(strat,strat->sl,h.p,h.ecart);

        // reduce the tail and normalize poly
        if (TEST_OPT_INTSTRATEGY)
        {
          h.pCleardenom();
          if ((TEST_OPT_REDSB)||(TEST_OPT_REDTAIL))
          {
            h.p = redtailBba(&(h),pos-1,strat, withT); // !!!
            h.pCleardenom();
          }
        }
        else
        {
          h.pNorm();
          if ((TEST_OPT_REDSB)||(TEST_OPT_REDTAIL))
            h.p = redtailBba(&(h),pos-1,strat, withT);
        }

#ifdef KDEBUG
        if (TEST_OPT_DEBUG){PrintS(" N:");h.wrp();PrintLn();}
#endif

//        h.PrepareRed(strat->use_buckets); // ???

        h.sev = pGetShortExpVector(h.p);
        strat->initEcart(&h);

        if (strat->Ll==-1)
          pos = 0;
        else
          pos = strat->posInL(strat->L,strat->Ll,&h,strat);

         enterL(&strat->L,&strat->Ll,&strat->Lmax,h,pos);*/

      } // for all x_i \in Ann(lm(P))
    } // if red(P) != NULL

//     else if (strat->P.p1 == NULL && strat->minim > 0)
//     {
//       p_Delete(&strat->P.p2, currRing, strat->tailRing);
//     }

#ifdef KDEBUG
//    memset(&(strat->P), 0, sizeof(strat->P));
#endif

    kTest_TS(strat); // even of T is not used!

//     Print("\n$\n");

  }

#ifdef KDEBUG
  if (TEST_OPT_DEBUG) messageSets(strat);
#endif

  /* complete reduction of the standard basis--------- */

  if (TEST_OPT_REDSB)
  {
    completeReduce(strat);
  }

  /* release temp data-------------------------------- */

  exitBuchMora(strat); // cleanT!

  id_Delete(&tempF, currRing);

  if (TEST_OPT_WEIGHTM)
  {
    pRestoreDegProcs(pFDegOld, pLDegOld);
    if (ecartWeights)
    {
      omFreeSize((ADDRESS)ecartWeights,(pVariables+1)*sizeof(short));
      ecartWeights=NULL;
    }
  }

  if (TEST_OPT_PROT) messageStat(srmax,lrmax,hilbcount,strat);



  if (tempQ!=NULL) updateResult(strat->Shdl,tempQ,strat);


  if (TEST_OPT_REDSB) // ???
  {
    // must be at the very end (after exitBuchMora) as it changes the S set!!!
    ideal I = strat->Shdl;
    ideal erg = kInterRedOld(I,tempQ);
    assume(I!=erg);
    id_Delete(&I, currRing);
    strat->Shdl = erg;
  }

#if MYTEST
  PrintS("\n\n</sca_bba>\n\n");
#endif

  return (strat->Shdl);
}


// //////////////////////////////////////////////////////////////////////////////
// sca mora...

// returns TRUE if mora should use buckets, false otherwise
static BOOLEAN kMoraUseBucket(kStrategy strat)
{
#ifdef MORA_USE_BUCKETS
  if (TEST_OPT_NOT_BUCKETS)
    return FALSE;
  if (strat->red == redFirst)
  {
#ifdef NO_LDEG
    if (!strat->syzComp)
      return TRUE;
#else
    if ((strat->homog || strat->honey) && !strat->syzComp)
      return TRUE;
#endif
  }
  else
  {
    assume(strat->red == redEcart);
    if (strat->honey && !strat->syzComp)
      return TRUE;
  }
#endif
  return FALSE;
}


#ifdef HAVE_ASSUME
static int sca_mora_count = 0;
static int sca_mora_loop_count;
#endif

// ideal sca_mora (ideal F, ideal Q, intvec *w, intvec *, kStrategy strat)
ideal sca_mora(const ideal F, const ideal Q, const intvec *w, const intvec *, kStrategy strat)
{
  assume(rIsSCA(currRing));

  const unsigned int m_iFirstAltVar = scaFirstAltVar(currRing);
  const unsigned int m_iLastAltVar  = scaLastAltVar(currRing);

  ideal tempF = id_KillSquares(F, m_iFirstAltVar, m_iLastAltVar, currRing);

  ideal tempQ = Q;

  if(Q == currQuotient)
    tempQ = SCAQuotient(currRing);

  bool bIdHomog = id_IsSCAHomogeneous(tempF, NULL, NULL, currRing); // wCx == wCy == NULL!

  assume( !bIdHomog || strat->homog ); //  bIdHomog =====[implies]>>>>> strat->homog

  strat->homog = strat->homog && bIdHomog;

#ifdef PDEBUG
  assume( strat->homog == bIdHomog );
#endif /*PDEBUG*/

#ifdef HAVE_ASSUME
  sca_mora_count++;
  sca_mora_loop_count = 0;
#endif

#ifdef KDEBUG
  om_Opts.MinTrack = 5;
#endif


  strat->update = TRUE;
  /*- setting global variables ------------------- -*/
  initBuchMoraCrit(strat);
//   initHilbCrit(F,NULL,&hilb,strat); // no Q!
  initMora(tempF, strat);
  initBuchMoraPos(strat);
  /*Shdl=*/initBuchMora(tempF, tempQ, strat); // temp Q, F!
//   if (TEST_OPT_FASTHC) missingAxis(&strat->lastAxis,strat);
  /*updateS in initBuchMora has Hecketest
  * and could have put strat->kHEdgdeFound FALSE*/
#if 0
  if (ppNoether!=NULL)
  {
    strat->kHEdgeFound = TRUE;
  }
  if (strat->kHEdgeFound && strat->update)
  {
    firstUpdate(strat);
    updateLHC(strat);
    reorderL(strat);
  }
  if (TEST_OPT_FASTHC && (strat->lastAxis) && strat->posInLOldFlag)
  {
    strat->posInLOld = strat->posInL;
    strat->posInLOldFlag = FALSE;
    strat->posInL = posInL10;
    updateL(strat);
    reorderL(strat);
  }
#endif
  strat->use_buckets = kMoraUseBucket(strat);

  kTest_TS(strat);


  int srmax = strat->sl;
  int lrmax = strat->Ll;
  int olddeg = 0;
  int reduc = 0;
  int red_result = 1;
//  int hilbeledeg=1;
  int hilbcount=0;


  /*- compute-------------------------------------------*/

#undef HAVE_TAIL_RING

#ifdef HAVE_TAIL_RING
//  if (strat->homog && strat->red == redFirst)
//     kStratInitChangeTailRing(strat);
#endif





//  due to std( SB, p)
  if(TEST_OPT_SB_1)
  {
    for (int iNewElement = strat->newIdeal; iNewElement < IDELEMS(tempF); iNewElement++)
    {

      const poly pSave = tempF->m[iNewElement];

      if( pSave != NULL )
      {
//        tempF->m[iNewElement] = NULL;

        const poly p_next = pNext(pSave);

        if(p_next != NULL)
          for( unsigned int i = m_iFirstAltVar; i <= m_iLastAltVar; i++ )
            if( p_GetExp(pSave, i, currRing) != 0 )
            {

              assume(p_GetExp(pSave, i, currRing) == 1);

              const poly p_new = sca_pp_Mult_xi_pp(i, p_next, currRing);

#ifdef PDEBUG
              p_Test(p_new, currRing);
#endif

              if( p_new == NULL) continue;

              LObject h(p_new); // h = x_i * strat->P

              if (TEST_OPT_INTSTRATEGY)
                h.pCleardenom(); // also does a p_Content
              else
                h.pNorm();

              strat->initEcart(&h);
              h.sev = pGetShortExpVector(h.p);

              int pos = 0;

              if (strat->Ll != -1)
                pos = strat->posInL(strat->L,strat->Ll,&h,strat);

              enterL(&strat->L,&strat->Ll,&strat->Lmax,h,pos);

              if (strat->Ll>lrmax) lrmax = strat->Ll;
            }
      }

    }
  }




  while (strat->Ll >= 0)
  {
#ifdef HAVE_ASSUME
    sca_mora_loop_count++;
#endif
    if (lrmax< strat->Ll) lrmax=strat->Ll; /*stat*/
    //test_int_std(strat->kIdeal);
#ifdef KDEBUG
    if (TEST_OPT_DEBUG) messageSets(strat);
#endif
    if (TEST_OPT_DEGBOUND
    && (strat->L[strat->Ll].ecart+strat->L[strat->Ll].GetpFDeg()> Kstd1_deg))
    {
      /*
      * stops computation if
      * - 24 (degBound)
      *   && upper degree is bigger than Kstd1_deg
      */
      while ((strat->Ll >= 0)
        && (strat->L[strat->Ll].p1!=NULL) && (strat->L[strat->Ll].p2!=NULL)
        && (strat->L[strat->Ll].ecart+strat->L[strat->Ll].GetpFDeg()> Kstd1_deg)
      )
      {
        deleteInL(strat->L,&strat->Ll,strat->Ll,strat);
        //if (TEST_OPT_PROT)
        //{
        //   PrintS("D"); mflush();
        //}
      }
      if (strat->Ll<0) break;
      else strat->noClearS=TRUE;
    }
    strat->P = strat->L[strat->Ll];/*- picks the last element from the lazyset L -*/
    if (strat->Ll==0) strat->interpt=TRUE;
    strat->Ll--;

    // create the real Spoly
//    assume(pNext(strat->P.p) != strat->tail);

    if(strat->P.IsNull()) continue;


    if( pNext(strat->P.p) == strat->tail )
    {
      // deletes the int spoly and computes SPoly
      pLmFree(strat->P.p); // ???
      strat->P.p = nc_CreateSpoly(strat->P.p1, strat->P.p2, currRing);
    }



    if (strat->P.p1 == NULL)
    {
      // for input polys, prepare reduction (buckets !)
      strat->P.SetLength(strat->length_pLength);
      strat->P.PrepareRed(strat->use_buckets);
    }

    if (!strat->P.IsNull())
    {
      // might be NULL from noether !!!
      if (TEST_OPT_PROT)
        message(strat->P.ecart+strat->P.GetpFDeg(),&olddeg,&reduc,strat, red_result);
      // reduce
      red_result = strat->red(&strat->P,strat);
    }

    if (! strat->P.IsNull())
    {
      strat->P.GetP();
      // statistics
      if (TEST_OPT_PROT) PrintS("s");
      // normalization
      if (!TEST_OPT_INTSTRATEGY)
        strat->P.pNorm();
      // tailreduction
      strat->P.p = redtail(&(strat->P),strat->sl,strat);
      // set ecart -- might have changed because of tail reductions
      if ((!strat->noTailReduction) && (!strat->honey))
        strat->initEcart(&strat->P);
      // cancel unit
      cancelunit(&strat->P);
      // for char 0, clear denominators
      if (TEST_OPT_INTSTRATEGY)
        strat->P.pCleardenom();

      // put in T
      enterT(strat->P,strat);
      // build new pairs
      enterpairs(strat->P.p,strat->sl,strat->P.ecart,0,strat, strat->tl);
      // put in S
      strat->enterS(strat->P,
                    posInS(strat,strat->sl,strat->P.p, strat->P.ecart),
                    strat, strat->tl);


      // clear strat->P
      if (strat->P.lcm!=NULL) pLmFree(strat->P.lcm);
      strat->P.lcm=NULL;

      if (strat->sl>srmax) srmax = strat->sl; /*stat.*/
      if (strat->Ll>lrmax) lrmax = strat->Ll;



      // //////////////////////////////////////////////////////////
      // SCA:
      const poly pSave = strat->P.p;
      const poly p_next = pNext(pSave);

      if(p_next != NULL)
      for( unsigned int i = m_iFirstAltVar; i <= m_iLastAltVar; i++ )
      if( p_GetExp(pSave, i, currRing) != 0 )
      {

        assume(p_GetExp(pSave, i, currRing) == 1);

        const poly p_new = sca_pp_Mult_xi_pp(i, p_next, currRing);

#ifdef PDEBUG
        p_Test(p_new, currRing);
#endif

        if( p_new == NULL) continue;

        LObject h(p_new); // h = x_i * strat->P

        if (TEST_OPT_INTSTRATEGY)
           h.pCleardenom(); // also does a p_Content
        else
          h.pNorm();

        strat->initEcart(&h);
        h.sev = pGetShortExpVector(h.p);

        int pos = 0;

        if (strat->Ll != -1)
          pos = strat->posInL(strat->L,strat->Ll,&h,strat);

        enterL(&strat->L,&strat->Ll,&strat->Lmax,h,pos);

        if (strat->Ll>lrmax) lrmax = strat->Ll;
      }

#ifdef KDEBUG
      // make sure kTest_TS does not complain about strat->P
      memset(&strat->P,0,sizeof(strat->P));
#endif
    }
#if 0
    if (strat->kHEdgeFound)
    {
      if ((TEST_OPT_FINDET)
      || ((TEST_OPT_MULTBOUND) && (scMult0Int((strat->Shdl)) < mu)))
      {
        // obachman: is this still used ???
        /*
        * stops computation if strat->kHEdgeFound and
        * - 27 (finiteDeterminacyTest)
        * or
        * - 23
        *   (multBound)
        *   && multiplicity of the ideal is smaller then a predefined number mu
        */
        while (strat->Ll >= 0) deleteInL(strat->L,&strat->Ll,strat->Ll,strat);
      }
    }
#endif
    kTest_TS(strat);
  }
  /*- complete reduction of the standard basis------------------------ -*/
  if (TEST_OPT_REDSB) completeReduce(strat);
  /*- release temp data------------------------------- -*/
  exitBuchMora(strat);
  /*- polynomials used for HECKE: HC, noether -*/
  if (TEST_OPT_FINDET)
  {
    if (strat->kHEdge!=NULL)
      Kstd1_mu=pFDeg(strat->kHEdge,currRing);
    else
      Kstd1_mu=-1;
  }
  pDelete(&strat->kHEdge);
  strat->update = TRUE; //???
  strat->lastAxis = 0; //???
  pDelete(&strat->kNoether);
  omFreeSize((ADDRESS)strat->NotUsedAxis,(pVariables+1)*sizeof(BOOLEAN));
  if (TEST_OPT_PROT) messageStat(srmax,lrmax,hilbcount,strat);
  if (TEST_OPT_WEIGHTM)
  {
    pRestoreDegProcs(pFDegOld, pLDegOld);
    if (ecartWeights)
    {
      omFreeSize((ADDRESS)ecartWeights,(pVariables+1)*sizeof(short));
      ecartWeights=NULL;
    }
  }
  if (tempQ!=NULL) updateResult(strat->Shdl,tempQ,strat);
  idTest(strat->Shdl);

  id_Delete( &tempF, currRing);

  return (strat->Shdl);
}






void sca_p_ProcsSet(ring rGR, p_Procs_s* p_Procs)
{

  // "commutative" procedures:
  rGR->p_Procs->p_Mult_mm     = sca_p_Mult_mm;
  rGR->p_Procs->pp_Mult_mm    = sca_pp_Mult_mm;

  p_Procs->p_Mult_mm          = sca_p_Mult_mm;
  p_Procs->pp_Mult_mm         = sca_pp_Mult_mm;

  // non-commutaitve
  rGR->GetNC()->p_Procs.mm_Mult_p   = sca_mm_Mult_p;
  rGR->GetNC()->p_Procs.mm_Mult_pp  = sca_mm_Mult_pp;


  if (rHasLocalOrMixedOrdering(rGR))
  {
#ifdef PDEBUG
//           Print("Local case => GB == mora!\n");
#endif
    rGR->GetNC()->p_Procs.GB          = sca_mora; // local ordering => Mora, otherwise - Buchberger!
  }
  else
  {
#ifdef PDEBUG
//           Print("Global case => GB == bba!\n");
#endif
    rGR->GetNC()->p_Procs.GB          = sca_bba; // sca_gr_bba; // sca_bba? // sca_bba;
  }


//   rGR->GetNC()->p_Procs.GlobalGB    = sca_gr_bba;
//   rGR->GetNC()->p_Procs.LocalGB     = sca_mora;


//   rGR->GetNC()->p_Procs.SPoly         = sca_SPoly;
//   rGR->GetNC()->p_Procs.ReduceSPoly   = sca_ReduceSpoly;

#if 0

        // Multiplication procedures:

        p_Procs->p_Mult_mm   = sca_p_Mult_mm;
        _p_procs->p_Mult_mm  = sca_p_Mult_mm;

        p_Procs->pp_Mult_mm  = sca_pp_Mult_mm;
        _p_procs->pp_Mult_mm = sca_pp_Mult_mm;

        r->GetNC()->mmMultP()     = sca_mm_Mult_p;
        r->GetNC()->mmMultPP()    = sca_mm_Mult_pp;

        r->GetNC()->GB()            = sca_gr_bba;
/*
        // ??????????????????????????????????????? coefficients swell...
        r->GetNC()->SPoly()         = sca_SPoly;
        r->GetNC()->ReduceSPoly()   = sca_ReduceSpoly;
*/
//         r->GetNC()->BucketPolyRed() = gnc_kBucketPolyRed;
//         r->GetNC()->BucketPolyRed_Z()= gnc_kBucketPolyRed_Z;

#endif
}


// bi-Degree (x, y) of monomial "m"
// x-es and y-s are weighted by wx and wy resp.
// [optional] components have weights by wCx and wCy.
static inline void m_GetBiDegree(const poly m,
  const intvec *wx, const intvec *wy,
  const intvec *wCx, const intvec *wCy,
  int& dx, int& dy, const ring r)
{
  const unsigned int N  = r->N;

  p_Test(m, r);

  assume( wx != NULL );
  assume( wy != NULL );

  assume( wx->cols() == 1 );
  assume( wy->cols() == 1 );

  assume( (unsigned int)wx->rows() >= N );
  assume( (unsigned int)wy->rows() >= N );

  int x = 0;
  int y = 0;

  for(int i = N; i > 0; i--)
  {
    const int d = p_GetExp(m, i, r);
    x += d * (*wx)[i-1];
    y += d * (*wy)[i-1];
  }

  if( (wCx != NULL) && (wCy != NULL) )
  {
    const int c = p_GetComp(m, r);

    if( wCx->range(c) )
      x += (*wCx)[c];

    if( wCy->range(c) )
      x += (*wCy)[c];
  }

  dx = x;
  dy = y;
}

// returns true if polynom p is bi-homogenous with respect to the given weights
// simultaneously sets bi-Degree
bool p_IsBiHomogeneous(const poly p,
  const intvec *wx, const intvec *wy,
  const intvec *wCx, const intvec *wCy,
  int &dx, int &dy,
  const ring r)
{
  if( p == NULL )
  {
    dx = 0;
    dy = 0;
    return true;
  }

  poly q = p;


  int ddx, ddy;

  m_GetBiDegree( q, wx, wy, wCx, wCy, ddx, ddy, r); // get bi degree of lm(p)

  pIter(q);

  for(; q != NULL; pIter(q) )
  {
    int x, y;

    m_GetBiDegree( q, wx, wy, wCx, wCy, x, y, r); // get bi degree of q

    if ( (x != ddx) || (y != ddy) ) return false;
  }

  dx = ddx;
  dy = ddy;

  return true;
}


// returns true if id is bi-homogenous without respect to the given weights
bool id_IsBiHomogeneous(const ideal id,
  const intvec *wx, const intvec *wy,
  const intvec *wCx, const intvec *wCy,
  const ring r)
{
  if (id == NULL) return true; // zero ideal

  const int iSize = IDELEMS(id);

  if (iSize == 0) return true;

  bool b = true;
  int x, y;

  for(int i = iSize - 1; (i >= 0 ) && b; i--)
    b = p_IsBiHomogeneous(id->m[i], wx, wy, wCx, wCy, x, y, r);

  return b;
}


// returns an intvector with [nvars(r)] integers [1/0]
// 1 - for commutative variables
// 0 - for anticommutative variables
intvec *ivGetSCAXVarWeights(const ring r)
{
  const unsigned int N  = r->N;

  const int CommutativeVariable = 0; // bug correction!
  const int AntiCommutativeVariable = 0;

  intvec* w = new intvec(N, 1, CommutativeVariable);

  if(AntiCommutativeVariable != CommutativeVariable)
  if( rIsSCA(r) )
  {
    const unsigned int m_iFirstAltVar = scaFirstAltVar(r);
    const unsigned int m_iLastAltVar  = scaLastAltVar(r);

    for (unsigned int i = m_iFirstAltVar; i<= m_iLastAltVar; i++)
    {
      (*w)[i-1] = AntiCommutativeVariable;
    }
  }

  return w;
}


// returns an intvector with [nvars(r)] integers [1/0]
// 0 - for commutative variables
// 1 - for anticommutative variables
intvec *ivGetSCAYVarWeights(const ring r)
{
  const unsigned int N  = r->N;

  const int CommutativeVariable = 0;
  const int AntiCommutativeVariable = 1;

  intvec* w = new intvec(N, 1, CommutativeVariable);

  if(AntiCommutativeVariable != CommutativeVariable)
  if( rIsSCA(r) )
  {
    const unsigned int m_iFirstAltVar = scaFirstAltVar(r);
    const unsigned int m_iLastAltVar  = scaLastAltVar(r);

    for (unsigned int i = m_iFirstAltVar; i<= m_iLastAltVar; i++)
    {
      (*w)[i-1] = AntiCommutativeVariable;
    }
  }
  return w;
}




// reduce term lt(m) modulo <y_i^2> , i = iFirstAltVar .. iLastAltVar:
// either create a copy of m or return NULL
static inline poly m_KillSquares(const poly m,
  const unsigned int iFirstAltVar, const unsigned int iLastAltVar,
  const ring r)
{
#ifdef PDEBUG
  p_Test(m, r);
  assume( (iFirstAltVar >= 1) && (iLastAltVar <= r->N) && (iFirstAltVar <= iLastAltVar) );

#if 0
  Print("m_KillSquares, m = "); // !
  p_Write(m, r);
#endif
#endif

  assume( m != NULL );

  for(unsigned int k = iFirstAltVar; k <= iLastAltVar; k++)
    if( p_GetExp(m, k, r) > 1 )
      return NULL;

  return p_Head(m, r);
}


// reduce polynomial p modulo <y_i^2> , i = iFirstAltVar .. iLastAltVar
// returns a new poly!
poly p_KillSquares(const poly p,
  const unsigned int iFirstAltVar, const unsigned int iLastAltVar,
  const ring r)
{
#ifdef PDEBUG
  p_Test(p, r);

  assume( (iFirstAltVar >= 1) && (iLastAltVar <= r->N) && (iFirstAltVar <= iLastAltVar) );

#if 0
  Print("p_KillSquares, p = "); // !
  p_Write(p, r);
#endif
#endif


  if( p == NULL )
    return NULL;

  poly pResult = NULL;
  poly* ppPrev = &pResult;

  for( poly q = p; q!= NULL; pIter(q) )
  {
#ifdef PDEBUG
    p_Test(q, r);
#endif

    // terms will be in the same order because of quasi-ordering!
    poly v = m_KillSquares(q, iFirstAltVar, iLastAltVar, r);

    if( v != NULL )
    {
      *ppPrev = v;
      ppPrev = &pNext(v);
    }

  }

#ifdef PDEBUG
  p_Test(pResult, r);
#if 0
  Print("p_KillSquares => "); // !
  p_Write(pResult, r);
#endif
#endif

  return(pResult);
}




// reduces ideal id modulo <y_i^2> , i = iFirstAltVar .. iLastAltVar
// returns the reduced ideal or zero ideal.
ideal id_KillSquares(const ideal id,
  const unsigned int iFirstAltVar, const unsigned int iLastAltVar,
  const ring r, const bool bSkipZeroes)
{
  if (id == NULL) return id; // zero ideal

  assume( (iFirstAltVar >= 1) && (iLastAltVar <= r->N) && (iFirstAltVar <= iLastAltVar) );

  const int iSize = IDELEMS(id);

  if (iSize == 0) return id;

  ideal temp = idInit(iSize, id->rank);

#if 0
   PrintS("<id_KillSquares>\n");
  {
    PrintS("ideal id: \n");
    for (int i = 0; i < IDELEMS(id); i++)
    {
      Print("; id[%d] = ", i+1);
      p_Write(id->m[i], r);
    }
    PrintS(";\n");
    PrintLn();
  }
#endif


  for (int j = 0; j < iSize; j++)
    temp->m[j] = p_KillSquares(id->m[j], iFirstAltVar, iLastAltVar, r);

  if( bSkipZeroes )
    idSkipZeroes(temp);

#if 0
   PrintS("<id_KillSquares>\n");
  {
    PrintS("ideal temp: \n");
    for (int i = 0; i < IDELEMS(temp); i++)
    {
      Print("; temp[%d] = ", i+1);
      p_Write(temp->m[i], r);
    }
    PrintS(";\n");
    PrintLn();
  }
   PrintS("</id_KillSquares>\n");
#endif

//  temp->rank = idRankFreeModule(temp, r);

  return temp;
}




#endif

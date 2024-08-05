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



#include "misc/auxiliary.h"

#ifdef HAVE_PLURAL

// for
#define PLURAL_INTERNAL_DECLARATIONS

#include "polys/nc/sca.h"
#include "polys/nc/nc.h"
#include "polys/nc/gb_hack.h"

#include "coeffs/numbers.h"

#include "misc/options.h"

#include "polys/monomials/p_polys.h"

#include "polys/simpleideals.h"
#include "misc/intvec.h"

#include "polys/monomials/ring.h"
#include "polys/kbuckets.h"
#include "polys/sbuckets.h"

#include "polys/prCopy.h"

#include "polys/operations/p_Mult_q.h"
#include "polys/templates/p_MemAdd.h"

#include "polys/weight.h"

// poly functions defined in p_Procs :

// return pPoly * pMonom; preserve pPoly and pMonom.
poly sca_pp_Mult_mm(const poly pPoly, const poly pMonom, const ring rRing, poly &);

// return pMonom * pPoly; preserve pPoly and pMonom.
static poly sca_pp_mm_Mult(const poly pPoly, const poly pMonom, const ring rRing);

// return pPoly * pMonom; preserve pMonom, destroy or reuse pPoly.
poly sca_p_Mult_mm(poly pPoly, const poly pMonom, const ring rRing);

// return pMonom * pPoly; preserve pMonom, destroy or reuse pPoly.
static poly sca_p_mm_Mult(poly pPoly, const poly pMonom, const ring rRing);


// compute the spolynomial of p1 and p2
poly sca_SPoly(const poly p1, const poly p2, const ring r);
poly sca_ReduceSpoly(const poly p1, poly p2, const ring r);


////////////////////////////////////////////////////////////////////////////////////////////////////
// Super Commutative Algebra extension by Oleksandr
////////////////////////////////////////////////////////////////////////////////////////////////////

// returns the sign of: lm(pMonomM) * lm(pMonomMM),
// preserves input, may return +/-1, 0
static inline int sca_Sign_mm_Mult_mm( const poly pMonomM, const poly pMonomMM, const ring rRing )
{
#ifdef PDEBUG
    p_Test(pMonomM,  rRing);
    p_Test(pMonomMM, rRing);
#endif

    const short iFirstAltVar = scaFirstAltVar(rRing);
    const short iLastAltVar  = scaLastAltVar(rRing);

    REGISTER unsigned int tpower = 0;
    REGISTER unsigned int cpower = 0;

    for( REGISTER short j = iLastAltVar; j >= iFirstAltVar; j-- )
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

    REGISTER unsigned int tpower = 0;
    REGISTER unsigned int cpower = 0;

    for( REGISTER unsigned int j = iLastAltVar; j >= iFirstAltVar; j-- )
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
      nCoeffM = n_InpNeg(nCoeffM, rRing->cf); // negate nCoeff (will destroy the original number)

    const number nCoeffMM = p_GetCoeff(pMonomMM, rRing); // no new copy!

    number nCoeff = n_Mult(nCoeffM, nCoeffMM, rRing->cf); // new number!

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

    REGISTER unsigned int tpower = 0;
    REGISTER unsigned int cpower = 0;

    for( REGISTER unsigned int j = iLastAltVar; j >= iFirstAltVar; j-- )
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
      nCoeffM = n_InpNeg(nCoeffM, rRing->cf); // negate nCoeff (will destroy the original number), creates new number!

    const number nCoeffMM = p_GetCoeff(pMonomMM, rRing); // no new copy!

    number nCoeff = n_Mult(nCoeffM, nCoeffMM, rRing->cf); // new number!

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

    REGISTER unsigned int tpower = 0;
    REGISTER unsigned int cpower = 0;

    for( REGISTER unsigned int j = iLastAltVar; j >= iFirstAltVar; j-- )
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

    number nCoeff = n_Mult(nCoeff1, nCoeff2, rRing->cf); // new number!

    if( (tpower) != 0 ) // degree is odd => negate coeff.
      nCoeff = n_InpNeg(nCoeff, rRing->cf); // negate nCoeff (will destroy the original number)

    p_SetCoeff0(pResult, nCoeff, rRing); // set lc(pResult) = nCoeff, no destruction!

#ifdef PDEBUG
    p_LmTest(pResult, rRing);
#endif

    return(pResult);
}

// returns: result =  x_i * lt(pMonom),
// preserves pMonom. may return NULL!
// if result != NULL => next(result) = NULL, lt(result) = x_i * lt(pMonom)
static inline poly sca_xi_Mult_mm(short i, const poly pMonom, const ring rRing)
{
#ifdef PDEBUG
    p_Test(pMonom, rRing);
#endif

    assume( i <= scaLastAltVar(rRing));
    assume( scaFirstAltVar(rRing) <= i );

    if( p_GetExp(pMonom, i, rRing) != 0 ) // => result is zero!
      return NULL;

    const short iFirstAltVar = scaFirstAltVar(rRing);

    REGISTER unsigned int cpower = 0;

    for( REGISTER short j = iFirstAltVar; j < i ; j++ )
      cpower ^= p_GetExp(pMonom, j, rRing);

#ifdef PDEBUG
    assume(cpower <= 1);
#endif

    poly pResult = p_LmInit(pMonom, rRing);

    p_SetExp(pResult, i, 1, rRing); // pResult*=X_i &&
    p_Setm(pResult, rRing);         // adjust degree after previous step!

    number nCoeff = n_Copy(pGetCoeff(pMonom), rRing->cf); // new number!

    if( cpower != 0 ) // degree is odd => negate coeff.
      nCoeff = n_InpNeg(nCoeff, rRing->cf); // negate nCoeff (will destroy the original number)

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
//  PrintS("sca_p_Mult_mm\n"); // !

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
poly sca_pp_Mult_mm(const poly pPoly, const poly pMonom, const ring rRing)
{
  assume( rIsSCA(rRing) );

#ifdef PDEBUG
//  PrintS("sca_pp_Mult_mm\n"); // !

  p_Test(pPoly, rRing);
  p_Test(pMonom, rRing);
#endif

  if (/*(*/  pPoly == NULL  /*)*/) /*|| ( pMonom == NULL )*/
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
static inline poly sca_xi_Mult_pp(short i, const poly pPoly, const ring rRing)
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
static poly sca_pp_mm_Mult(const poly pPoly, const poly pMonom, const ring rRing)
{
  assume( rIsSCA(rRing) );

#ifdef PDEBUG
//  PrintS("sca_mm_Mult_pp\n"); // !

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
        Werror("sca_pp_mm_Mult: exponent mismatch %d and %d\n", iComponent, iComponentMonomM);
        // what should we do further?!?

        p_Delete( &pResult, rRing); // delete the result
        return NULL;
      }
#ifdef PDEBUG
      if(iComponent==0 )
      {
        dReportError("sca_pp_mm_Mult: Multiplication in the left module from the right!");
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
static poly sca_p_mm_Mult(poly pPoly, const poly pMonom, const ring rRing) // !!!!! the MOST used procedure !!!!!
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
        Werror("sca_p_mm_Mult: exponent mismatch %d and %d\n", iComponent, iComponentMonomM);
        // what should we do further?!?

        p_Delete( &pPoly, rRing); // delete the result
        return NULL;
      }
#ifdef PDEBUG
      if(iComponent==0)
      {
        dReportError("sca_p_mm_Mult: Multiplication in the left module from the right!");
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

  poly pL = p_Lcm(p1, p2, r);       // pL = lcm( lm(p1), lm(p2) )

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

  number C1  = n_Copy(pGetCoeff(p1),r->cf);      // C1 = lc(p1)
  number C2  = n_Copy(pGetCoeff(p2),r->cf);      // C2 = lc(p2)

  number C = n_Gcd(C1,C2,r->cf);                     // C = gcd(C1, C2)

  if (!n_IsOne(C, r->cf))                              // if C != 1
  {
    C1=n_Div(C1, C, r->cf);                              // C1 = C1 / C
    C2=n_Div(C2, C, r->cf);                              // C2 = C2 / C
  }

  n_Delete(&C,r->cf); // destroy the number C

  const int iSignSum = sca_Sign_mm_Mult_mm (m1, p1, r) + sca_Sign_mm_Mult_mm (m2, p2, r);
  // zero if different signs

  assume( (iSignSum*iSignSum == 0) || (iSignSum*iSignSum == 4) );

  if( iSignSum != 0 ) // the same sign!
    C2=n_InpNeg (C2, r->cf);

  p_SetCoeff(m1, C2, r);                           // lc(m1) = C2!!!
  p_SetCoeff(m2, C1, r);                           // lc(m2) = C1!!!

  poly tmp1 = nc_mm_Mult_pp (m1, pNext(p1), r);    // tmp1 = m1 * tail(p1),
  p_Delete(&m1,r);  //  => n_Delete(&C1,r);

  poly tmp2 = nc_mm_Mult_pp (m2, pNext(p2), r);    // tmp1 = m2 * tail(p2),
  p_Delete(&m2,r);  //  => n_Delete(&C1,r);

  poly spoly = p_Add_q (tmp1, tmp2, r); // spoly = spoly(lt(p1), lt(p2)) + m1 * tail(p1), delete tmp1,2

  if (spoly!=NULL) p_Cleardenom (spoly, r);

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

  number C1 = n_Copy( pGetCoeff(p1), r->cf);
  number C2 = n_Copy( pGetCoeff(p2), r->cf);

  /* GCD stuff */
  number C = n_Gcd(C1, C2, r->cf);

  if (!n_IsOne(C, r->cf))
  {
    C1 = n_Div(C1, C, r->cf);
    C2 = n_Div(C2, C, r->cf);
  }
  n_Delete(&C,r->cf);

  const int iSign = sca_Sign_mm_Mult_mm( m, p1, r );

  if(iSign == 1)
    C2 = n_InpNeg(C2,r->cf);

  p_SetCoeff(m, C2, r);

#ifdef PDEBUG
  p_Test(m,r);
#endif

  p2 = p_LmDeleteAndNext( p2, r );

  p2 = p_Mult_nn(p2, C1, r); // p2 !!!
  n_Delete(&C1,r->cf);

  poly T = nc_mm_Mult_pp(m, pNext(p1), r);
  p_Delete(&m, r);

  p2 = p_Add_q(p2, T, r);

  if ( p2!=NULL ) p_Cleardenom(p2,r);

#ifdef PDEBUG
  p_Test(p2,r);
#endif

  return(p2);
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
    PrintS("\nrG: \n"); rWrite(rG);
    PrintS("\nrGR: \n"); rWrite(rGR);
    PrintLn();
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

      if( n_IsMOne(c, rBase->cf) ) // !!!???
      {
        if( i < iAltVarStart)
          iAltVarStart = i;

        if( j > iAltVarEnd)
          iAltVarEnd = j;
      } else
      {
        if( !n_IsOne(c, rBase->cf) )
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


  if((iAltVarEnd == -1) || (iAltVarStart == (N+1)))
  {
    if (N>1) return false; // no alternating variables
    iAltVarEnd=iAltVarStart=1;
  }

  for(int i = 1; i < N; i++)
  {
    for(int j = i + 1; j <= N; j++)
    {
      assume(MATELEM(C,i,j) != NULL); // after CallPlural!
      number c = p_GetCoeff(MATELEM(C,i,j), rBase);

      if( (iAltVarStart <= i) && (j <= iAltVarEnd) ) // S <= i < j <= E
      { // anticommutative part
        if( !n_IsMOne(c, rBase->cf) )
        {
#if ((defined(PDEBUG) && OUTPUT) || MYTEST)
          Print("Wrong Coeff at: [%d, %d]\n", i, j);
#endif
          return false;
        }
      }
      else
      { // should commute
        if( !n_IsOne(c, rBase->cf) )
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
  assume( iAltVarStart <= iAltVarEnd   );
  assume( iAltVarEnd   <= N            );


//  ring rSaveRing = assureCurrentRing(rG);


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

  for ( int i = iAltVarStart; (i <= iAltVarEnd); i++ )
    if( (i < b) || (i > e) ) // otherwise it's ok since rG is an SCA!
    {
      poly square = p_One( rG);
      p_SetExp(square, i, 2, rG); // square = var(i)^2.
      p_Setm(square, rG);

      // square = NF( var(i)^2 | Q )
      // NOTE: there is no better way to check this in general!
      square = nc_NF(idQuotient, NULL, square, 0, 1, rG); // must ran in currRing == rG!

      if( square != NULL ) // var(i)^2 is not in Q?
      {
        p_Delete(&square, rG);
        return false;
      }
    }

#if ((defined(PDEBUG) && OUTPUT) || MYTEST)
  Print("ScaVars!: [%d, %d]\n", iAltVarStart, iAltVarEnd);
#endif


  //////////////////////////////////////////////////////////////////////////
  // ok... here we go. let's setup it!!!
  //////////////////////////////////////////////////////////////////////////
  ideal tempQ = id_KillSquares(idQuotient, iAltVarStart, iAltVarEnd, rG); // in rG!!!


#if ((defined(PDEBUG) && OUTPUT) || MYTEST)
  PrintS("Quotient: \n");
  iiWriteMatrix((matrix)idQuotient,"__",1, rG, 0);
  PrintS("tempSCAQuotient: \n");
  iiWriteMatrix((matrix)tempQ,"__",1, rG, 0);
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
    iiWriteMatrix((matrix)tempQ,"__",1, rGR, 0);
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

//  ring rSaveRing = currRing;
//  if(rSaveRing != rGR)
//    rChangeCurrRing(rGR);

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

//  if(rSaveRing != rGR)
//    rChangeCurrRing(rSaveRing);

  return true;
}

// return x_i * pPoly; preserve pPoly.
poly sca_pp_Mult_xi_pp(short i, const poly pPoly, const ring rRing)
{
  assume(1 <= i);
  assume(i <= rVar(rRing));

  if(rIsSCA(rRing))
    return sca_xi_Mult_pp(i, pPoly, rRing);



  poly xi =  p_One( rRing);
  p_SetExp(xi, i, 1, rRing);
  p_Setm(xi, rRing);

  poly pResult = pp_Mult_qq(xi, pPoly, rRing);

  p_Delete( &xi, rRing);

  return pResult;

}

void sca_p_ProcsSet(ring rGR, p_Procs_s* p_Procs)
{

  // "commutative" procedures:
  rGR->p_Procs->p_Mult_mm     = sca_p_Mult_mm;
  rGR->p_Procs->pp_Mult_mm    = sca_pp_Mult_mm;

  p_Procs->p_Mult_mm          = sca_p_Mult_mm;
  p_Procs->pp_Mult_mm         = sca_pp_Mult_mm;

  // non-commutaitve
  p_Procs->p_mm_Mult          = sca_p_mm_Mult;
  p_Procs->pp_mm_Mult         = sca_pp_mm_Mult;

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

/*
        // ??????????????????????????????????????? coefficients swell...
        r->GetNC()->SPoly()         = sca_SPoly;
        r->GetNC()->ReduceSPoly()   = sca_ReduceSpoly;
*/
//         r->GetNC()->BucketPolyRed() = gnc_kBucketPolyRed;
//         r->GetNC()->BucketPolyRed_Z()= gnc_kBucketPolyRed_Z;
#endif

  // local ordering => Mora, otherwise - Buchberger!
  if (rHasLocalOrMixedOrdering(rGR))
    rGR->GetNC()->p_Procs.GB          = cast_A_to_vptr(sca_mora);
  else
    rGR->GetNC()->p_Procs.GB          = cast_A_to_vptr(sca_bba); // sca_gr_bba?
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
  const short iFirstAltVar, const short iLastAltVar,
  const ring r)
{
#ifdef PDEBUG
  p_Test(m, r);
  assume( (iFirstAltVar >= 1) && (iLastAltVar <= rVar(r)) && (iFirstAltVar <= iLastAltVar) );

#if 0
  PrintS("m_KillSquares, m = "); // !
  p_Write(m, r);
#endif
#endif

  assume( m != NULL );

  for(short k = iFirstAltVar; k <= iLastAltVar; k++)
    if( p_GetExp(m, k, r) > 1 )
      return NULL;

  return p_Head(m, r);
}


// reduce polynomial p modulo <y_i^2> , i = iFirstAltVar .. iLastAltVar
// returns a new poly!
poly p_KillSquares(const poly p,
  const short iFirstAltVar, const short iLastAltVar,
  const ring r)
{
#ifdef PDEBUG
  p_Test(p, r);

  assume( (iFirstAltVar >= 1) && (iLastAltVar <= r->N) && (iFirstAltVar <= iLastAltVar) );

#if 0
  PrintS("p_KillSquares, p = "); // !
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
  PrintS("p_KillSquares => "); // !
  p_Write(pResult, r);
#endif
#endif

  return(pResult);
}




// reduces ideal id modulo <y_i^2> , i = iFirstAltVar .. iLastAltVar
// returns the reduced ideal or zero ideal.
ideal id_KillSquares(const ideal id,
  const short iFirstAltVar, const short iLastAltVar,
  const ring r, const bool bSkipZeroes)
{
  if (id == NULL) return id; // zero ideal

  assume( (iFirstAltVar >= 1) && (iLastAltVar <= rVar(r)) && (iFirstAltVar <= iLastAltVar) );

  const int iSize = IDELEMS(id);

  if (iSize == 0) return id;

  ideal temp = idInit(iSize, id->rank);

#if 0
   PrintS("<id_KillSquares>\n");
  {
    PrintS("ideal id: \n");
    for (unsigned int i = 0; i < IDELEMS(id); i++)
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

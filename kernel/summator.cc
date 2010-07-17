/****************************************
*  Computer Algebra System SINGULAR     *
****************************************/
/***************************************************************
 *  File:    summator.cc
 *  Purpose: simple Summator usecase implementation
 *  Author:  motsak
 *  Created:
 *  Version: $Id$
 *******************************************************************/


#define MYTEST 0
#define OUTPUT 0

#if MYTEST
#define OM_CHECK 4
#define OM_TRACK 5
#endif

#include <kernel/mod2.h>
#ifdef HAVE_PLURAL
#include <kernel/summator.h>
#include <kernel/options.h>
#include <kernel/ring.h>
#include <kernel/p_polys.h>
#include <kernel/sbuckets.h>


CPolynomialSummator::CPolynomialSummator(ring rBaseRing, bool bUsePolynomial):
    m_basering(rBaseRing), m_bUsePolynomial(bUsePolynomial)
{
#ifdef RDEBUG
  rTest(rBaseRing);
#endif

  if(bUsePolynomial)
    m_temp.m_poly = NULL;
  else
  {
    assume(!TEST_OPT_NOT_BUCKETS);
    m_temp.m_bucket = sBucketCreate(rBaseRing);
  }
}

/*
// no sBucketInit defined :(((
CPolynomialSummator::CPolynomialSummator(ring rBaseRing, poly pInitialSum, int iLength, bool bUsePolynomial):
    m_basering(rBaseRing), m_bUsePolynomial(bUsePolynomial)
{
#ifdef PDEBUG
  p_Test(pInitialSum, rBaseRing);
#endif

  if(bUsePolynomial)
  {
    m_temp.m_poly = pInitialSum;
  }
  else
  {
    assume(!TEST_OPT_NOT_BUCKETS);
    m_temp.m_bucket = sBucketInit(pInitialSum, iLength, rBaseRing);
  }
}
*/

CPolynomialSummator::~CPolynomialSummator()
{
  if(!m_bUsePolynomial)
  {
    poly out;
    int pLength;

    sBucketClearAdd(m_temp.m_bucket, &out, &pLength);
    sBucketDestroy(&m_temp.m_bucket);

    if(out != NULL)
      p_Delete(&out, m_basering);
//    m_temp.m_bucket = NULL;
  }
  else
    if(m_temp.m_poly!=NULL)
    {
#ifdef PDEBUG
      p_Test(m_temp.m_poly, m_basering);
#endif
      p_Delete(&m_temp.m_poly, m_basering);
//      m_temp.m_poly = NULL;
    }
}

void CPolynomialSummator::AddAndDelete(poly pSummand, int iLength)
{
#ifdef PDEBUG
  p_Test(pSummand, m_basering);
#endif

  if(m_bUsePolynomial)
    m_temp.m_poly = p_Add_q(m_temp.m_poly, pSummand, m_basering);
  else
    sBucket_Add_p(m_temp.m_bucket, pSummand, iLength);
}

void CPolynomialSummator::AddAndDelete(poly pSummand)
{
#ifdef PDEBUG
  p_Test(pSummand, m_basering);
#endif

  if(m_bUsePolynomial)
    m_temp.m_poly = p_Add_q(m_temp.m_poly, pSummand, m_basering);
  else
    sBucket_Add_p(m_temp.m_bucket, pSummand, 0);
}

poly CPolynomialSummator::AddUpAndClear()
{
  poly out = NULL;

  if(m_bUsePolynomial)
  {
    out = m_temp.m_poly;
    m_temp.m_poly = NULL;
  }
  else
  {
    int pLength;
    sBucketClearAdd(m_temp.m_bucket, &out, &pLength);
  }

#ifdef PDEBUG
  p_Test(out, m_basering);
#endif

  return out;
}


poly CPolynomialSummator::AddUpAndClear(int *piLength)
{
  poly out = NULL;

  if(m_bUsePolynomial)
  {
    out = m_temp.m_poly;
    m_temp.m_poly = NULL;
    *piLength = pLength(out);
  }
  else
  {
    *piLength = 0;
    sBucketClearAdd(m_temp.m_bucket, &out, piLength);
  }

#ifdef PDEBUG
  p_Test(out, m_basering);
  assume(pLength(out) == *piLength);
#endif

  return out;
}



void CPolynomialSummator::Add(poly pSummand, int iLength)
{
  AddAndDelete(p_Copy(pSummand, m_basering), iLength);
}

void CPolynomialSummator::Add(poly pSummand)
{
  AddAndDelete(p_Copy(pSummand, m_basering));
}



CPolynomialSummator::CPolynomialSummator(const CPolynomialSummator& b): m_bUsePolynomial(b.m_bUsePolynomial), m_basering(b.m_basering)
{
//  try{
    if(m_bUsePolynomial)
      m_temp.m_poly = p_Copy( b.m_temp.m_poly, m_basering);
    else
      m_temp.m_bucket = sBucketCopy(b.m_temp.m_bucket);
//  }
//  catch(...)
//  {
//    assume(false);
//  }
}


#endif

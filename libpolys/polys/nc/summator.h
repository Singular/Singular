#ifndef SUMMATOR_H
#define SUMMATOR_H
/****************************************
*  Computer Algebra System SINGULAR     *
****************************************/

#define HAVE_SUMMATOR 1

#ifdef HAVE_SUMMATOR
#include "polys/monomials/ring.h"
#include "polys/sbuckets.h"

// TODO: redesign into templates with no extra run-time cost!!!
// TODO: make several out of CPolynomialSummator with similar (?) usage
// pattern/interface!!!

// //////////////////////////////////////////////////////////////////////// //
/// CPolynomialSummator: unifies bucket and polynomial summation as the
/// later is broken in buckets :(
class CPolynomialSummator
{
  private:
    const ring& m_basering;
    const bool m_bUsePolynomial;
    union
    {
      sBucket_pt m_bucket;
//      kBucket_pt m_kbucket;
      poly       m_poly;
    } m_temp;
  public:
    CPolynomialSummator(const ring& rBaseRing, bool bUsePolynomial = false);
//    CPolynomialSummator(ring rBaseRing, poly pInitialSum, int iLength = 0, bool bUsePolynomial = false);
    ~CPolynomialSummator();

    // adds and destroes the summand
    void AddAndDelete(poly pSummand, int iLength);
    void AddAndDelete(poly pSummand);

    inline void operator +=(poly pSummand){ AddAndDelete(pSummand); }

    // only adds and keeps the summand
    // please use AddAndDelete instead!
    void Add(poly pSummand, int iLength);
    void Add(poly pSummand);

    // get the final result and clear (set to zero) the summator
    poly AddUpAndClear();
    poly AddUpAndClear(int *piLength);

    inline operator poly() { return AddUpAndClear(); }

    /// Copy constructor
    CPolynomialSummator(const CPolynomialSummator&);
  private:

    /// no assignment operator yet
    CPolynomialSummator& operator= (const CPolynomialSummator&);
};

#endif // ifdef HAVE_SUMMATOR
#endif // ifndef  SUMMATOR_H


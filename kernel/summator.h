#ifndef SUMMATOR_H
#define SUMMATOR_H
/****************************************
*  Computer Algebra System SINGULAR     *
****************************************/
/* $Id: summator.h,v 1.1 2008-07-04 14:19:38 motsak Exp $ */

// #include <summator.h> // for CPolynomialSummator class

#include <structs.h>
// #include <p_polys.h> // due to p_Copy :(


// //////////////////////////////////////////////////////////////////////// //
// CPolynomialSummator: unifies bucket and polynomial summation as the
// later is brocken in buckets :(

class CPolynomialSummator
{
  private:
    ring m_basering;
    bool m_bUsePolynomial;
    union {
      sBucket_pt m_bucket;
      poly       m_poly;
    } m_temp;    
  public:
    CPolynomialSummator(ring rBaseRing, bool bUsePolynomial = false);
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
    
  private: // no copies of this object at the moment!!!
    CPolynomialSummator(const CPolynomialSummator& m);               // Copy constructor
    CPolynomialSummator& operator= (const CPolynomialSummator& m);   // Assignment operator    
};

#endif // ifndef  SUMMATOR_H







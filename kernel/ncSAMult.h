#ifndef GRING_SA_MULT_H
#define GRING_SA_MULT_H
/*****************************************
 *  Computer Algebra System SINGULAR     *
 *****************************************/
/* $Id: ncSAMult.h,v 1.3 2008-07-11 15:53:28 motsak Exp $ */
#ifdef HAVE_PLURAL

// #include <ncSAMult.h> // for CMultiplier etc classes

#include <structs.h>
#include <ring.h>
#include <summator.h> // for CPolynomialSummator class

#include <ncSACache.h> // for CCacheHash etc classes

// //////////////////////////////////////////////////////////////////////// //
//

bool ncInitSpecialPairMultiplication(ring r);


template <typename CExponent>
class CMultiplier
{
  protected:
    ring m_basering;

  public:
    CMultiplier(ring rBaseRing): m_basering(rBaseRing) {};

    const ring GetBasering() const { return m_basering; };


    // Term * Exponent -> Monom * Exponent
    inline poly MultiplyTE(const poly pTerm, const CExponent expRight)
    { return p_Mult_nn(MultiplyME(pTerm, expRight), p_GetCoeff(pTerm, GetBasering()), GetBasering()); }

    // Exponent * Term -> Exponent * Monom
    inline poly MultiplyET(const CExponent expLeft, const poly pTerm)
    { return p_Mult_nn(MultiplyEM(expLeft, pTerm), p_GetCoeff(pTerm, GetBasering()), GetBasering()); }

//
// Main templates!
//

    // Poly * Exponent
    inline poly MultiplyPE(const poly pPoly, const CExponent expRight)
    {
      bool bUsePolynomial = TEST_OPT_NOT_BUCKETS || (pLength(pPoly) < MIN_LENGTH_BUCKET);
      CPolynomialSummator sum(GetBasering(), bUsePolynomial);

      for( poly q = pPoly; q !=NULL; q = pNext(q) )
        sum += MultiplyTE(q, expRight); 

      return sum;
    }

    // Exponent * Poly 
    inline poly MultiplyEP(const CExponent expLeft, const poly pPoly)
    {
      bool bUsePolynomial = TEST_OPT_NOT_BUCKETS || (pLength(pPoly) < MIN_LENGTH_BUCKET);
      CPolynomialSummator sum(GetBasering(), bUsePolynomial);

      for( poly q = pPoly; q !=NULL; q = pNext(q) )
        sum += MultiplyET(expLeft, q); 

      return sum;
    }

    // Poly * Exponent
    inline poly MultiplyPEDestroy(poly pPoly, const CExponent expRight)
    {
      bool bUsePolynomial = TEST_OPT_NOT_BUCKETS || (pLength(pPoly) < MIN_LENGTH_BUCKET);
      CPolynomialSummator sum(GetBasering(), bUsePolynomial);

      for( ; pPoly!=NULL; pPoly  = p_LmDeleteAndNext(pPoly, GetBasering()) )
        sum += MultiplyTE(pPoly, expRight); 

      return sum;
    }

    // Exponent * Poly 
    inline poly MultiplyEPDestroy(const CExponent expLeft, poly pPoly)
    {
      bool bUsePolynomial = TEST_OPT_NOT_BUCKETS || (pLength(pPoly) < MIN_LENGTH_BUCKET);
      CPolynomialSummator sum(GetBasering(), bUsePolynomial);

      for( ; pPoly!=NULL; pPoly  = p_LmDeleteAndNext(pPoly, GetBasering()) )
        sum += MultiplyET(expLeft, pPoly); 

      return sum;
    }


  protected:

    // Exponent * Exponent
    virtual poly MultiplyEE(const CExponent expLeft, const CExponent expRight) = 0;    

    // Monom * Exponent
    virtual poly MultiplyME(const poly pMonom, const CExponent expRight) = 0;
    
    // Exponent * Monom
    virtual poly MultiplyEM(const CExponent expLeft, const poly pMonom) = 0;
      
  private: // no copy constuctors!
    CMultiplier();
    operator =(CMultiplier&);
};


class CSpecialPairMultiplier: public CMultiplier<int> 
{
  private:
    int m_i;
    int m_j;
    
  public:
    CSpecialPairMultiplier(ring r, int i, int j);
  protected:    
    typedef int CExponent;

    // Exponent * Exponent
    virtual poly MultiplyEE(const poly expLeft, const CExponent expRight);    

    // Monom * Exponent
    virtual poly MultiplyME(const poly pMonom, const CExponent expRight);

    // Exponent * Monom
    virtual poly MultiplyEM(const CExponent expLeft, const poly pMonom);

};

/*
class CCommutativeSpecialPairMultiplier: public CSpecialPairMultiplier
{
  public:
    // Exponent * Exponent
    virtual poly MultiplyEE(const poly expLeft, const CExponent expRight);    

    // Monom * Exponent
    virtual poly MultiplyME(const poly pMonom, const CExponent expRight);

    // Exponent * Monom
    virtual poly MultiplyEM(const CExponent expLeft, const poly pMonom);
}
*/




class CGlobalMultiplier: public CMultiplier<poly>
{
  private:
    CSpecialPairMultiplier** m_specialpairs; // upper triangular submatrix of pairs 1 <= i < j <= N of a N x N matrix.
    int m_NVars; // N = number of variables
    CGlobalCacheHash* m_cache;

  public:
    CGlobalMultiplier(ring r);

    inline int NVars() const { return m_NVars; }

    inline CSpecialPairMultiplier* GetPair(int i, int j) const
    {
      assume( m_specialpairs != NULL );
      assume( i > 0 );
      assume( i < j );
      assume( j <= iNVars );

      return m_specialpairs + ( (NVars() * ((i)-1) - ((i) * ((i)-1))/2 + (j)-1) - (i) );
    }


  protected:    
    typedef poly CExponent;

    // the following methods are literally equal!
    
    // Exponent * Exponent
    virtual poly MultiplyEE(const poly expLeft, const CExponent expRight);    

    // Monom * Exponent
    virtual poly MultiplyME(const poly pMonom, const CExponent expRight);

    // Exponent * Monom
    virtual poly MultiplyEM(const CExponent expLeft, const poly pMonom);


    CSpecialPairMultiplier* AnalyzePair(const ring r, int j, int i);
    
};



#endif // HAVE_PLURAL :(
#endif // 

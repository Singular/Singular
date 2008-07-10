#ifndef GRING_SA_MULT_H
#define GRING_SA_MULT_H
/*****************************************
 *  Computer Algebra System SINGULAR     *
 *****************************************/
/* $Id: ncSAMult.h,v 1.2 2008-07-10 15:07:46 motsak Exp $ */
#ifdef HAVE_PLURAL

// #include <ncSAMult.h> // for CMultiplier etc classes

#include <structs.h>
#include <ring.h>
#include <summator.h> // for CPolynomialSummator class

// //////////////////////////////////////////////////////////////////////// //
//
template <typename CExponent>
class CMultiplier
{
  protected:
    ring m_basering;

    enum EHistoryType {
      MULT_START 
    }

  public:
    CMultiplier(ring rBaseRing): m_basering(rBaseRing) {};
    
  protected:
    virtual CExponent GetExponent(const poly pMonom); // ugly!

    virtual poly Multiply(const CExponent a, const CExponent b) = 0;
    
    inline poly MultiplyMonoms(const poly pPoly, const CExponent b)
    { return( p_Mult_nn(Multiply(GetExponent(pPoly), b), p_GetCoeff(pPoly,m_basering), m_basering); }
    
    inline poly MultiplyMonoms(const CExponent a, const poly pPoly)
    { return( p_Mult_nn(Multiply(a, GetExponent(pPoly)), p_GetCoeff(pPoly,m_basering), m_basering); }
    
    
    poly Multiply(const poly pPoly, const CExponent b)
    {
      bool bUsePolynomial = TEST_OPT_NOT_BUCKETS || (pLength(pPoly) < MIN_LENGTH_BUCKET);
      CPolynomialSummator sum(m_basering, bUsePolynomial);

      for( poly q = pPoly; q !=NULL; q = pNext(q) )
        sum += MultiplyMonoms(q, b); 
      
      return sum;
    };
    
    poly Multiply(CExponent a, const poly pPoly)
    {
      bool bUsePolynomial = TEST_OPT_NOT_BUCKETS || (pLength(pPoly) < MIN_LENGTH_BUCKET);
      CPolynomialSummator sum(m_basering, bUsePolynomial);

      for( poly q = pPoly; q !=NULL; q = pNext(q) )
        sum += MultiplyMonoms(a, q);
      
      return sum;
    };

    poly MultiplyAndDestroyPoly(poly pPoly, const CExponent b)
    {
      bool bUsePolynomial = TEST_OPT_NOT_BUCKETS || (pLength(pPoly) < MIN_LENGTH_BUCKET);
      CPolynomialSummator sum(m_basering, bUsePolynomial);


      for( ; pPoly!=NULL; pPoly  = p_LmDeleteAndNext(pPoly, m_basering) )
        sum += MultiplyMonoms(pPoly, b);

      return sum;
    };

    poly MultiplyAndDestroyPoly(CExponent a, poly pPoly)
    {
      bool bUsePolynomial = TEST_OPT_NOT_BUCKETS || (pLength(pPoly) < MIN_LENGTH_BUCKET);
      CPolynomialSummator sum(m_basering, bUsePolynomial);

      for( ; pPoly!=NULL; pPoly  = p_LmDeleteAndNext(pPoly, m_basering) )
        sum += MultiplyMonoms(a, pPoly);

      return sum;
    };
    
        
    void History(CExponent a, CExponent b, EHistoryType t)
    {
      Print("Mult!\n");
    }
  private: // no copy constuctors!
    CMultiplier();
    operator =(CMultiplier&);
};

typedef CMultiplier<int> CSpecialPairMultiplier;
typedef CMultiplier<int*> CGlobalMultiplier;



/*

const int iMaxCacheSize = 20;
template <typename CExponent>
class CCacheHash
{
  public:
    struct CProductItem
    {
      CExponent aExponent;
      CExponent bExponent;
      poly pProduct;
      long lHits;
    }
  
    virtual bool Lookup(CExponent a, CExponent b, );
  protected:
    poly Multiply(poly p, CExponent b);
    poly Multiply(CExponent a, poly p);
    void History(CExponent a, CExponent b);
  private: // no copy constuctors!
    CMultiplier();
    operator =(CMultiplier&);
}




class CCommutativeSpecialPairMultiplier: public CSpecialPairMultiplier
{
  public:
    virtual poly Multiply(int a, int b);
}


CSpecialPairMultiplier* AnalyzePair(const ring r, int j, int i);
bool ncInitSpecialPairMultiplication(ring r);


// //////////////////////////////////////////////////////

// // Macros used to access upper triangle matrices C,D... (which are actually ideals) // afaik
#define UPMATELEM(i,j,nVar) ( (nVar * ((i)-1) - ((i) * ((i)-1))/2 + (j)-1)-(i) )
*/




#endif // HAVE_PLURAL :(
#endif // 

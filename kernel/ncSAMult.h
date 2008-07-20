#ifndef GRING_SA_MULT_H
#define GRING_SA_MULT_H
/*****************************************
 *  Computer Algebra System SINGULAR     *
 *****************************************/
/* $Id: ncSAMult.h,v 1.6 2008-07-20 10:00:14 motsak Exp $ */
#ifdef HAVE_PLURAL

// #include <ncSAMult.h> // for CMultiplier etc classes

#include <structs.h>
#include <ring.h>
#include <summator.h> // for CPolynomialSummator class
#include <febase.h> // for Print!
#include <p_polys.h>
#include <p_Mult_q.h>

#include <ncSACache.h> // for CCacheHash etc classes

// //////////////////////////////////////////////////////////////////////// //
//

bool ncInitSpecialPairMultiplication(ring r);


template <typename CExponent>
class CMultiplier
{
  protected:
    ring m_basering;
    int  m_NVars; // N = number of variables

  public:
    CMultiplier(ring rBaseRing): m_basering(rBaseRing), m_NVars(rBaseRing->N) {};
    virtual ~CMultiplier() {};

    const ring GetBasering() const { return m_basering; };
    inline int NVars() const { return m_NVars; }


    inline poly LM(const poly pTerm, const ring r, int i = 1) const
    {
      poly pMonom = p_LmInit(pTerm, r);
      pSetCoeff0(pMonom, n_Init(i, r));
      return pMonom;
    }

    // Term * Exponent -> Monom * Exponent
    inline poly MultiplyTE(const poly pTerm, const CExponent expRight)
    {
      const ring r = GetBasering();
      poly pMonom = LM(pTerm, r);
      
      poly result = p_Mult_nn(MultiplyME(pMonom, expRight), p_GetCoeff(pTerm, r), r);

      p_Delete(&pMonom, r);

      return result;
    }
    

    // Exponent * Term -> Exponent * Monom
    inline poly MultiplyET(const CExponent expLeft, const poly pTerm)
    {
      const ring r = GetBasering();
      poly pMonom = LM(pTerm, r);

      poly result = p_Mult_nn(MultiplyEM(expLeft, pMonom), p_GetCoeff(pTerm, r), r);

      p_Delete(&pMonom, r);
      return result;
      

    }

    // Main templates:

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


//  protected:

    // Exponent * Exponent
    virtual poly MultiplyEE(const CExponent expLeft, const CExponent expRight) = 0;    

    // Monom * Exponent
    virtual poly MultiplyME(const poly pMonom, const CExponent expRight) = 0;
    
    // Exponent * Monom
    virtual poly MultiplyEM(const CExponent expLeft, const poly pMonom) = 0;
      
  private: // no copy constuctors!
    CMultiplier();
    CMultiplier(const CMultiplier&);
    CMultiplier& operator=(const CMultiplier&);
    
};


class CSpecialPairMultiplier: public CMultiplier<int> 
{
  private:
    int m_i;
    int m_j;

    poly m_c_ij;
    poly m_d_ij;
    
    
  public:
    // 1 <= i < j <= NVars()
    CSpecialPairMultiplier(ring r, int i, int j);
    virtual ~CSpecialPairMultiplier();

    inline int GetI() const { return m_i; }
    inline int GetJ() const { return m_j; }

//  protected:
    typedef int CExponent;

    // Exponent * Exponent
    // Computes: var(j)^{expLeft} * var(i)^{expRight}
    virtual poly MultiplyEE(const CExponent expLeft, const CExponent expRight) = 0;

    // Monom * Exponent
    // pMonom must be of the form: var(j)^{n}
    virtual poly MultiplyME(const poly pMonom, const CExponent expRight);

    // Exponent * Monom
    // pMonom must be of the form: var(i)^{m}
    virtual poly MultiplyEM(const CExponent expLeft, const poly pMonom);

};





struct CPower // represents var(iVar)^{iPower}
{
  int Var;
  int Power;

  CPower(int i, int n): Var(i), Power(n) {};

  inline poly GetPoly(const ring r, int c = 1) const
  {
    poly p = p_ISet(c, r);
    p_SetExp(p, Var, Power, r);
    p_Setm(p, r);
    return p;
  };
 
};






class CPowerMultiplier: public CMultiplier<CPower>
{
  private:
    CSpecialPairMultiplier** m_specialpairs; // upper triangular submatrix of pairs 1 <= i < j <= N of a N x N matrix.


  public:
    CPowerMultiplier(ring r);
    virtual ~CPowerMultiplier();

    inline CSpecialPairMultiplier* GetPair(int i, int j) const
    {
      assume( m_specialpairs != NULL );
      assume( i > 0 );
      assume( i < j );
      assume( j <= NVars() );

      return m_specialpairs[( (NVars() * ((i)-1) - ((i) * ((i)-1))/2 + (j)-1) - (i) )];
    }

    inline CSpecialPairMultiplier*& GetPair(int i, int j)
    {
      assume( m_specialpairs != NULL );
      assume( i > 0 );
      assume( i < j );
      assume( j <= NVars() );

      return m_specialpairs[( (NVars() * ((i)-1) - ((i) * ((i)-1))/2 + (j)-1) - (i) )];
    }
    
//  protected:
    typedef CPower CExponent;

    // Exponent * Exponent
    // Computes: var(j)^{expLeft} * var(i)^{expRight}
    virtual poly MultiplyEE(const CExponent expLeft, const CExponent expRight);

    // Monom * Exponent
    // pMonom may NOT be of the form: var(j)^{n}!
    virtual poly MultiplyME(const poly pMonom, const CExponent expRight);

    // Exponent * Monom
    // pMonom may NOT be of the form: var(i)^{m}!
    virtual poly MultiplyEM(const CExponent expLeft, const poly pMonom);


};




class CGlobalMultiplier: public CMultiplier<poly>
{
  private:
    CGlobalCacheHash* m_cache;
    CPowerMultiplier* m_powers;

  public:
    typedef CMultiplier<poly> CBaseType;
    
    CGlobalMultiplier(ring r);
    virtual ~CGlobalMultiplier();

    
  
//  protected:    
    typedef poly CExponent;

    // the following methods are literally equal!
    
    // Exponent * Exponent
    // TODO: handle components!!!
    virtual poly MultiplyEE(const CExponent expLeft, const CExponent expRight);    

    // Monom * Exponent
    virtual poly MultiplyME(const poly pMonom, const CExponent expRight);

    // Exponent * Monom
    virtual poly MultiplyEM(const CExponent expLeft, const poly pMonom);

};



//////////////////////////////////////////////////////////////////////////
class CCommutativeSpecialPairMultiplier: public CSpecialPairMultiplier
{
	public:
		CCommutativeSpecialPairMultiplier(ring r, int i, int j);
		virtual ~CCommutativeSpecialPairMultiplier();

		// Exponent * Exponent
		virtual poly MultiplyEE(const int expLeft, const int expRight);    
};

//////////////////////////////////////////////////////////////////////////
class CAntiCommutativeSpecialPairMultiplier: public CSpecialPairMultiplier
{
	public:
		CAntiCommutativeSpecialPairMultiplier(ring r, int i, int j);
		virtual ~CAntiCommutativeSpecialPairMultiplier();

		// Exponent * Exponent
		virtual poly MultiplyEE(const int expLeft, const int expRight);    
};


//////////////////////////////////////////////////////////////////////////
class CQuasiCommutativeSpecialPairMultiplier: public CSpecialPairMultiplier
{
	private:
		const number m_q;
		// TODO: make cache for some 'good' powers!?

  public:
		CQuasiCommutativeSpecialPairMultiplier(ring r, int i, int j, number q);
		virtual ~CQuasiCommutativeSpecialPairMultiplier();

		number GetPower(int power);

		// Exponent * Exponent
		virtual poly MultiplyEE(const int expLeft, const int expRight);    
};


#endif // HAVE_PLURAL :(
#endif // 

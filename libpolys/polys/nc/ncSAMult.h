#ifndef GRING_SA_MULT_H
#define GRING_SA_MULT_H
/*****************************************
 *  Computer Algebra System SINGULAR     *
 *****************************************/

#ifdef HAVE_PLURAL

#include "misc/options.h"
#include "polys/monomials/ring.h"
#include "polys/nc/summator.h"// for CPolynomialSummator class
#include "reporter/reporter.h" // for Print!
#include "polys/monomials/p_polys.h"
#include "polys/operations/p_Mult_q.h"

#include "polys/nc/ncSAFormula.h" // for CFormulaPowerMultiplier and enum Enum_ncSAType

// //////////////////////////////////////////////////////////////////////// //
//

BOOLEAN ncInitSpecialPairMultiplication(ring r);


template <typename CExponent>
class CMultiplier
{
  protected:
    const ring m_basering;
    const int  m_NVars; // N = number of variables

  public:
    CMultiplier(ring rBaseRing): m_basering(rBaseRing), m_NVars(rBaseRing->N) {};
    virtual ~CMultiplier() {};

    inline ring GetBasering() const { return m_basering; };
    inline int NVars() const { return m_NVars; }


    inline poly LM(const poly pTerm, const ring r, int i = 1) const
    {
      poly pMonom = p_LmInit(pTerm, r);
      pSetCoeff0(pMonom, n_Init(i, r->cf));
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

//  protected:

    // Exponent * Exponent
    virtual poly MultiplyEE(const CExponent expLeft, const CExponent expRight) = 0;

    // Monom * Exponent
    virtual poly MultiplyME(const poly pMonom, const CExponent expRight) = 0;

    // Exponent * Monom
    virtual poly MultiplyEM(const CExponent expLeft, const poly pMonom) = 0;

  private: // no copy constructors!
    CMultiplier();
    CMultiplier(const CMultiplier&);
    CMultiplier& operator=(const CMultiplier&);

};


class CSpecialPairMultiplier: public CMultiplier<int>
{
  private:
    int m_i; // 2-gen subalgebra in these variables...
    int m_j;

//    poly m_c_ij;
//    poly m_d_ij;


  public:
    // 1 <= i < j <= NVars()
    CSpecialPairMultiplier(ring r, int i, int j);
    virtual ~CSpecialPairMultiplier();

    inline int GetI() const { return m_i; } // X
    inline int GetJ() const { return m_j; } // Y > X!

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

/*
  inline poly GetPoly(const ring r) const // TODO: search for GetPoly(r, 1) and remove "1"!
  {
    poly p = p_One(r);
    p_SetExp(p, Var, Power, r);
    p_Setm(p, r);
    return p;
  };
  inline poly GetPoly(const ring r, int c) const
  {
    poly p = p_ISet(c, r);
    p_SetExp(p, Var, Power, r);
    p_Setm(p, r);
    return p;
  };
*/

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


};



class CGlobalMultiplier: public CMultiplier<poly>
{
  private:
//    CGlobalCacheHash* m_cache;
    CPowerMultiplier* m_powers;
    const CFormulaPowerMultiplier* m_RingFormulaMultiplier;

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


    // Main templates:

    // Poly * Exponent
    inline poly MultiplyPE(const poly pPoly, const CExponent expRight)
    {
      assume( pPoly != NULL );      assume( expRight != NULL );
      const int iComponentMonom = p_GetComp(expRight, GetBasering());

      bool bUsePolynomial = TEST_OPT_NOT_BUCKETS || (pLength(pPoly) < MIN_LENGTH_BUCKET);
      CPolynomialSummator sum(GetBasering(), bUsePolynomial);


      if( iComponentMonom!=0 )
      {
        for( poly q = pPoly; q !=NULL; q = pNext(q) )
        {
#ifdef PDEBUG
          {
            const int iComponent = p_GetComp(q, GetBasering());
            assume(iComponent == 0);
            if( iComponent!=0 )
            {
              Werror("MultiplyPE: both sides have non-zero components: %d and %d!\n", iComponent, iComponentMonom);
              // what should we do further?!?
              return NULL;
            }

          }
#endif
          sum += MultiplyTE(q, expRight); // NO Component!!!
        }
        poly t = sum; p_SetCompP(t, iComponentMonom, GetBasering());
        return t;
      } // iComponentMonom != 0!
      else
      { // iComponentMonom == 0!
        for( poly q = pPoly; q !=NULL; q = pNext(q) )
        {
          const int iComponent = p_GetComp(q, GetBasering());

#ifdef PDEBUG
          if( iComponent!=0 )
          {
            Warn("MultiplyPE: Multiplication in the left module from the right by component %d!\n", iComponent);
            // what should we do further?!?
          }
#endif
          poly t = MultiplyTE(q, expRight); // NO Component!!!
          p_SetCompP(t, iComponent, GetBasering());
          sum += t;
        }
        return sum;
      } // iComponentMonom == 0!
    }

    // Exponent * Poly
    inline poly MultiplyEP(const CExponent expLeft, const poly pPoly)
    {
      assume( pPoly != NULL );      assume( expLeft != NULL );
      const int iComponentMonom = p_GetComp(expLeft, GetBasering());

      bool bUsePolynomial = TEST_OPT_NOT_BUCKETS || (pLength(pPoly) < MIN_LENGTH_BUCKET);
      CPolynomialSummator sum(GetBasering(), bUsePolynomial);

      if( iComponentMonom!=0 )
      {
        for( poly q = pPoly; q !=NULL; q = pNext(q) )
        {
#ifdef PDEBUG
          {
            const int iComponent = p_GetComp(q, GetBasering());
            assume(iComponent == 0);
            if( iComponent!=0 )
            {
              Werror("MultiplyEP: both sides have non-zero components: %d and %d!\n", iComponent, iComponentMonom);
                // what should we do further?!?
              return NULL;
            }
          }
#endif
          sum += MultiplyET(expLeft, q);
        }
        poly t = sum; p_SetCompP(t, iComponentMonom, GetBasering());
        return t;
      } // iComponentMonom != 0!
      else
      { // iComponentMonom == 0!
        for( poly q = pPoly; q !=NULL; q = pNext(q) )
        {
          const int iComponent = p_GetComp(q, GetBasering());

          poly t = MultiplyET(expLeft, q); // NO Component!!!
          p_SetCompP(t, iComponent, GetBasering());
          sum += t;
        }
        return sum;
      } // iComponentMonom == 0!
    }

    // Poly * Exponent
    inline poly MultiplyPEDestroy(poly pPoly, const CExponent expRight)
    {
      assume( pPoly != NULL );      assume( expRight != NULL );
      const int iComponentMonom = p_GetComp(expRight, GetBasering());

      bool bUsePolynomial = TEST_OPT_NOT_BUCKETS || (pLength(pPoly) < MIN_LENGTH_BUCKET);
      CPolynomialSummator sum(GetBasering(), bUsePolynomial);


      if( iComponentMonom!=0 )
      {
        for(poly q = pPoly ; q!=NULL; q = p_LmDeleteAndNext(q, GetBasering()) )
        {
#ifdef PDEBUG
          {
            const int iComponent = p_GetComp(q, GetBasering());
            assume(iComponent == 0);
            if( iComponent!=0 )
            {
              Werror("MultiplyPEDestroy: both sides have non-zero components: %d and %d!\n", iComponent, iComponentMonom);
              // what should we do further?!?
              return NULL;
            }

          }
#endif
          sum += MultiplyTE(q, expRight); // NO Component!!!
        }
        poly t = sum; p_SetCompP(t, iComponentMonom, GetBasering());
        return t;
      } // iComponentMonom != 0!
      else
      { // iComponentMonom == 0!
        for(poly q = pPoly ; q!=NULL; q = p_LmDeleteAndNext(q, GetBasering()) )
        {
          const int iComponent = p_GetComp(q, GetBasering());

#ifdef PDEBUG
          if( iComponent!=0 )
          {
            Warn("MultiplyPEDestroy: Multiplication in the left module from the right by component %d!\n", iComponent);
            // what should we do further?!?
          }
#endif
          poly t = MultiplyTE(q, expRight); // NO Component!!!
          p_SetCompP(t, iComponent, GetBasering());
          sum += t;
        }
        return sum;
      } // iComponentMonom == 0!

    }

    // Exponent * Poly
    inline poly MultiplyEPDestroy(const CExponent expLeft, poly pPoly)
    {

      assume( pPoly != NULL );      assume( expLeft != NULL );
      const int iComponentMonom = p_GetComp(expLeft, GetBasering());

      bool bUsePolynomial = TEST_OPT_NOT_BUCKETS || (pLength(pPoly) < MIN_LENGTH_BUCKET);
      CPolynomialSummator sum(GetBasering(), bUsePolynomial);

      if( iComponentMonom!=0 )
      {
        for(poly q = pPoly ; q!=NULL; q = p_LmDeleteAndNext(q, GetBasering()) )
        {
#ifdef PDEBUG
          {
            const int iComponent = p_GetComp(q, GetBasering());
            assume(iComponent == 0);
            if( iComponent!=0 )
            {
              Werror("MultiplyEPDestroy: both sides have non-zero components: %d and %d!\n", iComponent, iComponentMonom);
                // what should we do further?!?
              return NULL;
            }
          }
#endif
          sum += MultiplyET(expLeft, q);
        }
        poly t = sum; p_SetCompP(t, iComponentMonom, GetBasering());
        return t;
      } // iComponentMonom != 0!
      else
      { // iComponentMonom == 0!
        for(poly q = pPoly ; q!=NULL; q = p_LmDeleteAndNext(q, GetBasering()) )
        {
          const int iComponent = p_GetComp(q, GetBasering());

          poly t = MultiplyET(expLeft, q); // NO Component!!!
          p_SetCompP(t, iComponent, GetBasering());
          sum += t;
        }
        return sum;
      } // iComponentMonom == 0!

    }




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

                // Exponent * Exponent
                virtual poly MultiplyEE(const int expLeft, const int expRight);
};


//////////////////////////////////////////////////////////////////////////
class CWeylSpecialPairMultiplier: public CSpecialPairMultiplier
{
  private:
    const number m_g;
    // TODO: make cache for some 'good' powers!?

  public:
    CWeylSpecialPairMultiplier(ring r, int i, int j, number g);
    virtual ~CWeylSpecialPairMultiplier();

    // Exponent * Exponent
    virtual poly MultiplyEE(const int expLeft, const int expRight);
};

//////////////////////////////////////////////////////////////////////////
class CHWeylSpecialPairMultiplier: public CSpecialPairMultiplier
{
  private:
    const int m_k;
    // TODO: make cache for some 'good' powers!?

  public:
    CHWeylSpecialPairMultiplier(ring r, int i, int j, int k);
    virtual ~CHWeylSpecialPairMultiplier();

    // Exponent * Exponent
    virtual poly MultiplyEE(const int expLeft, const int expRight);
};


//////////////////////////////////////////////////////////////////////////
class CShiftSpecialPairMultiplier: public CSpecialPairMultiplier
{
  private:
    const number m_shiftCoef;
    const int m_shiftVar;
    // TODO: make cache for some 'good' powers!?

  public:
    CShiftSpecialPairMultiplier(ring r, int i, int j, int s, number c);
    virtual ~CShiftSpecialPairMultiplier();

    // Exponent * Exponent
    virtual poly MultiplyEE(const int expLeft, const int expRight);
};



// need: enum Enum_ncSAType;

//////////////////////////////////////////////////////////////////////////
// Using external 'formula' routines
class CExternalSpecialPairMultiplier: public CSpecialPairMultiplier
{
  private:
    Enum_ncSAType m_ncSAtype;
  public:
    CExternalSpecialPairMultiplier(ring r, int i, int j, Enum_ncSAType type);
    virtual ~CExternalSpecialPairMultiplier();

    // Exponent * Exponent
    virtual poly MultiplyEE(const int expLeft, const int expRight);
};


#endif // HAVE_PLURAL :(
#endif //

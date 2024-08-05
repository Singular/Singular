#ifndef GRING_SA_MULT_FORMULA_H
#define GRING_SA_MULT_FORMULA_H
/*****************************************
 *  Computer Algebra System SINGULAR     *
 *****************************************/
#ifdef HAVE_PLURAL

// //////////////////////////////////////////////////////////////////////// //

#include "polys/monomials/ring.h"
#include "polys/nc/nc.h"

bool ncInitSpecialPowersMultiplication(ring r);

enum Enum_ncSAType
{
  _ncSA_notImplemented = -1,
  _ncSA_1xy0x0y0 = 0x00, // commutative
  _ncSA_Mxy0x0y0 = 0x01, // anti-commutative
  _ncSA_Qxy0x0y0 = 0x02, // quasi-commutative
  _ncSA_1xyAx0y0 = 0x10, // shift 1
  _ncSA_1xy0xBy0 = 0x20, // shift 2
  _ncSA_1xy0x0yG = 0x30, // Weyl
  _ncSA_1xy0x0yT2 = 0x100 // homogenized Weyl algebra?
};

class CFormulaPowerMultiplier
{
  private:
    Enum_ncSAType* m_SAPairTypes; // upper triangular submatrix of pairs 1 <= i < j <= N of a N x N matrix.

    const int m_NVars;
    const ring m_BaseRing;



  public:
    inline int NVars() const { return m_NVars; }
    inline ring GetBasering() const { return m_BaseRing; }

    CFormulaPowerMultiplier(ring r);
    virtual ~CFormulaPowerMultiplier();

    inline Enum_ncSAType GetPair(int i, int j) const
    {
      assume( m_SAPairTypes != NULL );
      assume( i > 0 );
      assume( i < j );
      assume( j <= NVars() );

      return m_SAPairTypes[( (NVars() * ((i)-1) - ((i) * ((i)-1))/2 + (j)-1) - (i) )];
    }

    inline Enum_ncSAType& GetPair(int i, int j)
    {
      assume( m_SAPairTypes != NULL );
      assume( i > 0 );
      assume( i < j );
      assume( j <= NVars() );

      return m_SAPairTypes[( (NVars() * ((i)-1) - ((i) * ((i)-1))/2 + (j)-1) - (i) )];
    }

    // Lowest level routines!
    static Enum_ncSAType AnalyzePair(const ring r, int i, int j);
    static poly Multiply( Enum_ncSAType type, const int i, const int j, const int n, const int m, const ring r);

    static poly ncSA_1xy0x0y0(const int i, const int j, const int n, const int m, const ring r);
    static poly ncSA_Mxy0x0y0(const int i, const int j, const int n, const int m, const ring r);

    static poly ncSA_Qxy0x0y0(const int i, const int j, const int n, const int m, const number m_q, const ring r);

    static poly ncSA_1xy0x0yG(const int i, const int j, const int n, const int m, const number m_g, const ring r);
    static poly ncSA_1xy0x0yT2(const int i, const int j, const int n, const int m, const int k, const ring r);

    static poly ncSA_1xyAx0y0(const int i, const int j, const int n, const int m, const number m_shiftCoef, const ring r);
    static poly ncSA_1xy0xBy0(const int i, const int j, const int n, const int m, const number m_shiftCoef, const ring r);



    // Higher level abstraction for keeping track of all the pair types!
    poly Multiply( int i, int j, const int n, const int m);

  private: // no copy constructors!
    CFormulaPowerMultiplier();
    CFormulaPowerMultiplier(const CFormulaPowerMultiplier&);
    CFormulaPowerMultiplier& operator=(const CFormulaPowerMultiplier&);


};


static inline CFormulaPowerMultiplier* GetFormulaPowerMultiplier(const ring r)
{
  return r->GetNC()->GetFormulaPowerMultiplier();
}




#endif // HAVE_PLURAL :(
#endif //

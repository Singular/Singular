#ifndef GRING_SA_MULT_H
#define GRING_SA_MULT_H
/*****************************************
 *  Computer Algebra System SINGULAR     *
 *****************************************/
/* $Id: ncSAMult.h,v 1.1 2008-07-04 16:18:42 motsak Exp $ */
#ifdef HAVE_PLURAL

// #include <ncSAMult.h> // for CMultiplier etc classes

#include <structs.h>
#include <ring.h>

// //////////////////////////////////////////////////////////////////////// //
//
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




template <typename CExponent>
class CMultiplier
{
  public:
    virtual poly Multiply(CExponent a, CExponent b);
  protected:
    poly Multiply(poly p, CExponent b);
    poly Multiply(CExponent a, poly p);
    void History(CExponent a, CExponent b);
  private: // no copy constuctors!
    CMultiplier();
    operator =(CMultiplier&);
}

typedef CMultiplier<int> CSpecialPairMultiplier;
typedef CMultiplier<int*> CGlobalMultiplier;


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

#ifndef GRING_SA_MULT_H
#define GRING_SA_MULT_H
/*****************************************
 *  Computer Algebra System SINGULAR     *
 *****************************************/
/* $Id: ncSACache.h,v 1.1 2008-07-11 15:53:28 motsak Exp $ */

// #include <ncSACache.h> // for CCacheHash etc classes

#include <structs.h>

// //////////////////////////////////////////////////////////////////////// //
//



const int iMaxCacheSize = 20;

template <typename CExponent>
class CCacheHash
{
  public:
    enum EHistoryType {
      MULT_LOOKUP
    }

    struct CCacheItem
    {
      union{
        CExponent aExponent;
        poly aMonom;
      } a;

      union{
        CExponent bExponent;
        poly bMonom;
      } b;

      poly pProduct;

      int iPairType;
      long lHits;
    };

    
    // -1 means no hits!
    int Lookup(CExponent a, CExponent b, CCacheItem*& pItems) const
    {
      History(a, b, MULT_LOOKUP);
      pItems = NULL;
      return -1;
    }

    bool Store(CExponent a, CExponent b, poly pProduct)
    {
      return false; // the pair was not stored!
    };
    
    virtual void History(const CExponent a, const CExponent b, const EHistoryType t)
    {
      Print("Mult!\n");
    }

  private: // no copy constuctors!
    CCacheHash();
    operator =(CCacheHash&);
};



class CGlobalCacheHash: public CCacheHash<poly>
{
  public:
    typedef poly CExponent;

  protected:
    virtual void History(CExponent a, CExponent b, const EHistoryType t);
};



class CSpecialPairCacheHash: public CCacheHash<int>
{
  public:
    typedef int CExponent;
    
  protected:
    virtual void History(const CExponent a, const CExponent b, const EHistoryType t);
};


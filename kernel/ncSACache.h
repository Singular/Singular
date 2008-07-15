#ifndef GRING_SA_CACHEHASH_H
#define GRING_SA_CACHEHASH_H
/*****************************************
 *  Computer Algebra System SINGULAR     *
 *****************************************/
/* $Id: ncSACache.h,v 1.2 2008-07-15 16:27:58 motsak Exp $ */

// #include <ncSACache.h> // for CCacheHash etc classes

#include <structs.h>
#include <febase.h> // for Print!
// //////////////////////////////////////////////////////////////////////// //
//

const int iMaxCacheSize = 20;

template <typename CExponent>
class CCacheHash
{
  private:
    ring m_basering;
    int m_NVars;
    
  public:
    CCacheHash(ring r): m_basering(r), m_NVars(r->N){};

    const ring GetBasering() const { return m_basering; };
    inline int NVars() const { return m_NVars; }
    
    virtual ~CCacheHash(){};
    
    enum EHistoryType {
      MULT_LOOKUP
    };

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
    int LookupEE(CExponent a, CExponent b, CCacheItem*& pItems)
    {
      History(a, b, MULT_LOOKUP);
      pItems = NULL;
      return -1;
    }

    bool StoreEE(CExponent a, CExponent b, poly pProduct)
    {
      Print("StoreEE!\n");
      return false; // the pair was not stored!
    };
    
    virtual void History(const CExponent a, const CExponent b, const EHistoryType t)
    {
      Print("MultHistory!\n");
    }

  private: // no copy constuctors!
    CCacheHash(const CCacheHash&);
    CCacheHash& operator=(const CCacheHash&);
};



class CGlobalCacheHash: public CCacheHash<poly>
{
  public:
    typedef poly CExponent;

    CGlobalCacheHash(ring r): CCacheHash<poly>(r) {};

    virtual ~CGlobalCacheHash() {};

  protected:
    virtual void History(CExponent a, CExponent b, const EHistoryType t);
};

class CSpecialPairCacheHash: public CCacheHash<int>
{
  public:
    typedef int CExponent;

    CSpecialPairCacheHash(ring r): CCacheHash<int>(r) {};

    virtual ~CSpecialPairCacheHash() {};

  protected:
    virtual void History(const CExponent a, const CExponent b, const EHistoryType t);
};



#endif // GRING_SA_CACHEHASH_H



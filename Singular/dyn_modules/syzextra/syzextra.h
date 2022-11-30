// -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*-
/*****************************************************************************\
 * Computer Algebra System SINGULAR
\*****************************************************************************/
/** @file syzextra.h
 *
 * Computation of Syzygies
 *
 * ABSTRACT: Computation of Syzygies due to Schreyer
 *
 * @author Oleksandr Motsak
 *
 **/
/*****************************************************************************/

#ifndef SYZEXTRA_H
#define SYZEXTRA_H

#include <vector>
#include <map>
#include <string.h>
#include <stack>

// include basic definitions
#include "singularxx_defs.h"
#include "kernel/ideals.h"

class idrec; typedef idrec *   idhdl;

class kBucket; typedef kBucket* kBucket_pt;

#ifndef NOPRODUCT
# define NOPRODUCT 1
#endif

// set to 1 if all leading coeffs are assumed to be all =1...
// note the use of simplify on input in SSinit!
#ifndef NODIVISION
# define NODIVISION 1
#endif

poly leadmonom(const poly p, const ring r, const bool bSetZeroComp = true);

/// inplace sorting of the module (ideal) id wrt <_(c,ds)
void Sort_c_ds(const ideal id, const ring r);


class sBucket; typedef sBucket* sBucket_pt;

/** @class SBucketFactory syzextra.h
 *
 * sBucket Factory
 *
 * Cleate/store/reuse buckets
 *
 */
class SBucketFactory: private std::stack <sBucket_pt>
{
  private:
    typedef std::stack <sBucket_pt> Base;

  public:
    typedef Base::value_type Bucket;

    SBucketFactory(const ring r)
    {
      push ( _CreateBucket(r) ); // start with at least one sBucket...?
      assume( top() != NULL );
    };

    ~SBucketFactory()
    {
      while( !empty() )
      {
        _DestroyBucket( top() );
        pop();
      }
    }

    Bucket getBucket(const ring r, const bool remove = true)
    {
      Bucket bt = NULL;

      if( !empty() )
      {
        bt = top();

        if( remove )
          pop();
      }
      else
      {
        bt = _CreateBucket(r);

        if( !remove )
        {
          push(bt);
          assume( bt == top() );
        }
      }

      assume( bt != NULL );

      return bt;
    }

    // TODO: this may be spared if we give-out a smart Bucket (which returns here upon its destructor!)
    void putBucket(const Bucket & bt, const bool replace = false)
    {
      assume( bt != NULL );

      if( empty() )
        push( bt );
      else
      {
        if( replace )
          top() = bt;
        else
        {
          if( bt != top() )
            push( bt );
        }
      }

      assume( bt == top() );
    }

  private:
    /// inital allocation for new buckets
    static Bucket _CreateBucket(const ring r);

    /// we only expect empty buckets to be left at the end for destructor
    /// bt will be set to NULL
    static void _DestroyBucket(Bucket & bt);

  private:
    SBucketFactory();
    SBucketFactory(const SBucketFactory&);
    void operator=(const SBucketFactory&);

};

/// Computation attribute storage
struct SchreyerSyzygyComputationFlags
{
    SchreyerSyzygyComputationFlags(idhdl rootRingHdl);

    SchreyerSyzygyComputationFlags(const SchreyerSyzygyComputationFlags& attr):
        OPT__DEBUG(attr.OPT__DEBUG),
        OPT__LEAD2SYZ(attr.OPT__LEAD2SYZ),  OPT__TAILREDSYZ(attr.OPT__TAILREDSYZ),
        OPT__HYBRIDNF(attr.OPT__HYBRIDNF), OPT__IGNORETAILS(attr.OPT__IGNORETAILS),
        OPT__SYZNUMBER(attr.OPT__SYZNUMBER), OPT__TREEOUTPUT(attr.OPT__TREEOUTPUT),
        OPT__SYZCHECK(attr.OPT__SYZCHECK), OPT__PROT(attr.OPT__PROT),
        OPT__NOCACHING(attr.OPT__NOCACHING),
        m_rBaseRing(attr.m_rBaseRing)
    {}

  /// output all the intermediate states
  const int OPT__DEBUG; // DebugOutput;

  /// ?
  const int OPT__LEAD2SYZ; // TwoLeadingSyzygyTerms;

  /// Reduce syzygy tails wrt the leading syzygy terms
  const int OPT__TAILREDSYZ; // TailReducedSyzygies;

  /// Use the usual NF's S-poly reduction while dropping lower order terms
  /// 2 means - smart selection!
  const int OPT__HYBRIDNF; // UseHybridNF


  /// ignore tails and compute the pure Schreyer frame
  const int OPT__IGNORETAILS; // @IGNORETAILS

  /// Syzygy level (within a resolution)
  mutable int OPT__SYZNUMBER;

  inline void  nextSyzygyLayer() const
  {
     OPT__SYZNUMBER++;
  }

  /// output lifting tree
  const int OPT__TREEOUTPUT;

  /// CheckSyzygyProperty: TODO
  const int OPT__SYZCHECK;

  /// TEST_OPT_PROT
  const bool OPT__PROT;

  /// no caching/stores/lookups
  const int OPT__NOCACHING;

  /// global base ring
  const ring m_rBaseRing;
};

class SchreyerSyzygyComputation;

class CLCM: public SchreyerSyzygyComputationFlags, public std::vector<bool>
{
  public:
    CLCM(const ideal& L, const SchreyerSyzygyComputationFlags& flags);

    bool Check(const poly m) const;

  private:
    bool m_compute;

    const unsigned int m_N; ///< number of ring variables
};


class CLeadingTerm
{
  public:
    CLeadingTerm(unsigned int label,  const poly lt, const ring);

#if NOPRODUCT
    bool DivisibilityCheck(const poly multiplier, const poly t, const unsigned long not_sev, const ring r) const;
#endif
    bool DivisibilityCheck(const poly product, const unsigned long not_sev, const ring r) const;

    bool CheckLT( const ideal & L ) const;

    inline poly lt() const { return m_lt; };
    inline unsigned long sev() const { return m_sev; };
    inline unsigned int label() const { return m_label; };
  private:
    const unsigned long m_sev; ///< not short exp. vector

    // NOTE/TODO: either of the following should be enough:
    const unsigned int  m_label; ///< index in the main L[] + 1

    const poly          m_lt; ///< the leading term itself L[label-1]

    // disable the following:
    CLeadingTerm();
    CLeadingTerm(const CLeadingTerm&);
    void operator=(const CLeadingTerm&);
};


// TODO: needs a specialized variant without a component (hash!)
class CReducerFinder: public SchreyerSyzygyComputationFlags
{
#if NOPRODUCT
  friend class CDivisorEnumerator2;
#endif
  friend class CDivisorEnumerator;

  public:
    typedef long TComponentKey;
    typedef std::vector<const CLeadingTerm*> TReducers;

  private:
    typedef std::map< TComponentKey, TReducers> CReducersHash;

  public:
    /// goes over all leading terms
    CReducerFinder(const ideal L, const SchreyerSyzygyComputationFlags& flags);

    void Initialize(const ideal L);

    ~CReducerFinder();


#if NOPRODUCT
    poly
        FindReducer(const poly multiplier, const poly monom, const poly syzterm, const CReducerFinder& checker) const;

#endif
    // TODO: save shortcut (syz: |-.->) LM(LM(m) * "t") -> syz?
    poly // const_iterator // TODO: return const_iterator it, s.th: it->m_lt is the needed
        FindReducer(const poly product, const poly syzterm, const CReducerFinder& checker) const;

    bool IsDivisible(const poly q) const;


    inline bool IsNonempty() const { return !m_hash.empty(); }

    /// is the term to be "preprocessed" as lower order term or lead to only reducible syzygies...
    int PreProcessTerm(const poly t, CReducerFinder& syzChecker) const;

  private:
    ideal m_L; ///< only for debug

    CReducersHash m_hash; // can also be replaced with a vector indexed by components

  private:
    CReducerFinder(const CReducerFinder&);
    void operator=(const CReducerFinder&);
};

bool my_p_LmCmp (poly, poly, const ring);

typedef poly TCacheKey;
typedef poly TCacheValue;

struct CCacheCompare
{
  const ring & m_ring;

  CCacheCompare();

  CCacheCompare(const ring& r): m_ring(r) { assume(r != NULL); }

  CCacheCompare(const CCacheCompare& lhs): m_ring(lhs.m_ring) { assume(m_ring != NULL); }
  CCacheCompare& operator=(const CCacheCompare& lhs) { assume(lhs.m_ring != NULL); return (const_cast<CCacheCompare&>(lhs)); }

  inline bool operator() (const TCacheKey& l, const TCacheKey& r) const { assume(m_ring != NULL); return my_p_LmCmp(l, r, m_ring); }
};

typedef std::map<TCacheKey, TCacheValue, CCacheCompare> TP2PCache; // deallocation??? !!!
typedef std::map<int, TP2PCache> TCache;


/** @class SchreyerSyzygyComputation syzextra.h
 *
 * Computing syzygies after Schreyer
 *
 * Storing/accumulating data during the computation requires some global
 * object, like this class. Ideally the above global functions should not
 * be used in favour of this class.
 *
 * @sa Schreyer Syzygy Computation Paper & Talk & Python prototype
 */
class SchreyerSyzygyComputation: public SchreyerSyzygyComputationFlags
{
  friend class CLCM;
  friend class CReducerFinder;

  public:
    /// Construct a global object for given input data (separated into leads & tails)
    SchreyerSyzygyComputation(const ideal idLeads, const ideal idTails, const SchreyerSyzygyComputationFlags setting):
        SchreyerSyzygyComputationFlags(setting),
        m_idLeads(idLeads), m_idTails(id_Copy(idTails, setting.m_rBaseRing)),
        m_syzLeads(NULL), m_syzTails(NULL),
        m_LS(NULL), m_lcm(m_idLeads, setting),
        m_div(m_idLeads, setting), m_checker(NULL, setting), m_cache(),
        m_sum_bucket_factory(setting.m_rBaseRing),
        m_spoly_bucket(NULL)
    {
      if( UNLIKELY(OPT__PROT) ) memset( &m_stat, 0, sizeof(m_stat) );
    }

    /// Construct a global object for given input data (separated into leads & tails)
    SchreyerSyzygyComputation(const ideal idLeads, const ideal idTails, const ideal syzLeads, const SchreyerSyzygyComputationFlags setting):
        SchreyerSyzygyComputationFlags(setting),
        m_idLeads(idLeads), m_idTails(id_Copy(idTails, setting.m_rBaseRing)),
        m_syzLeads(syzLeads), m_syzTails(NULL),
        m_LS(syzLeads), m_lcm(m_idLeads, setting),
        m_div(m_idLeads, setting), m_checker(NULL, setting), m_cache(),
        m_sum_bucket_factory(setting.m_rBaseRing),
        m_spoly_bucket(NULL)
    {
      if( UNLIKELY(OPT__PROT) ) memset( &m_stat, 0, sizeof(m_stat) );

      if( LIKELY(OPT__TAILREDSYZ && !OPT__IGNORETAILS) )
      {
        if (syzLeads != NULL)
          m_checker.Initialize(syzLeads);
//        if( idTails != NULL )
//          SetUpTailTerms();
      }
    }

    /// Destructor should not destruct the resulting m_syzLeads, m_syzTails.
    ~SchreyerSyzygyComputation(){ CleanUp(); }

    /// Convert the given ideal of tails into the internal representation (with reducers!)
    /// Preprocess m_idTails as well...?
    void SetUpTailTerms();

    /// print statistics about the used heuristics
    void PrintStats() const;

    /// Read off the results while detaching them from this object
    /// NOTE: no copy!
    inline void ReadOffResult(ideal& syzL, ideal& syzT)
    {
      syzL = m_syzLeads; syzT = m_syzTails;

      m_syzLeads = m_syzTails = NULL; // m_LS ?

      if ( UNLIKELY(OPT__PROT) )
        PrintStats();
    }


    /// The main driver function: computes
    void ComputeSyzygy();

    /// Computes Syz(leads) or only LEAD of it.
    /// The result is stored into m_syzLeads
    void ComputeLeadingSyzygyTerms(bool bComputeSecondTerms = true);



    /// Main HybridNF == 1: poly reduce + LOT + LCM?
    poly SchreyerSyzygyNF(const poly syz_lead, poly syz_2 = NULL) const;


    // Main (HybridNF == 0) Tree Travers + LOT + LCM?
    poly TraverseNF(const poly syz_lead, const poly syz_2 = NULL) const;

    /// High level caching function!!!
    poly TraverseTail(poly multiplier, const int tail) const;

    // REMOVE?
    /// called only from above and from outside (for testing)
    poly TraverseTail(poly multiplier, poly tail) const;

    /// TODO: save shortcut (syz: |-.->) LM(m) * "t" -> ? ???
    poly ReduceTerm(poly multiplier, poly term4reduction, poly syztermCheck) const;

    /// low level computation...
    poly ComputeImage(poly multiplier, const int tail) const;

  public:
    /// just for testing via the wrapper below
    inline poly _FindReducer(const poly product, const poly syzterm) const
        { return m_div.FindReducer(product, syzterm, m_checker); }
 private:
    void CleanUp();
  protected:


    /// just leading terms
    ideal Compute1LeadingSyzygyTerms();

    /// leading + second terms
    ideal Compute2LeadingSyzygyTerms();



  private:
    /// input leading terms
    const ideal m_idLeads;

    /// input tails
    const ideal m_idTails;

    /// output (syzygy) leading terms (+2nd terms?)
    ideal m_syzLeads;

    /// output (syzygy) tails
    ideal m_syzTails;

    /*mutable?*/ ideal m_LS; ///< leading syzygy terms used for reducing syzygy tails


    /// Bitmask for variables occuring in leading terms
    const CLCM m_lcm;

    /// Divisor finder
    const CReducerFinder m_div;

    /// for checking tail-terms and makeing them irreducible (wrt m_LS!)
    CReducerFinder m_checker;

    mutable TCache m_cache; // cacher comp + poly -> poly! // mutable???

    /// used for simple summing up
    mutable SBucketFactory m_sum_bucket_factory; // sBucket_pt

    /// for S-Polynomial reductions
    mutable kBucket_pt m_spoly_bucket; // only used inside of SchreyerSyzygyNF! destruction by CleanUp()!


    /// Statistics:
    ///  0..3: as in SetUpTailTerms()::PreProcessTerm() // TODO!!??
    ///  4: number of terms discarded due to LOT heuristics
    ///  5: number of terms discarded due to LCM heuristics
    ///  6, 7: lookups without & with rescale, 8: stores
    mutable unsigned long m_stat[9];
};

// The following wrappers are just for testing separate functions on highest level (within schreyer.lib)

static inline void ComputeSyzygy(const ideal L, const ideal T, ideal& LL, ideal& TT, const SchreyerSyzygyComputationFlags A)
{
  SchreyerSyzygyComputation syz(L, T, A);
  syz.ComputeSyzygy();
  syz.ReadOffResult(LL, TT);
}

static inline ideal ComputeLeadingSyzygyTerms(const ideal& L, const SchreyerSyzygyComputationFlags A)
{
  SchreyerSyzygyComputation syz(L, NULL, A);
  syz.ComputeLeadingSyzygyTerms(false);
  ideal LL, TT;
  syz.ReadOffResult(LL, TT);
  return LL; // assume TT is NULL!
}

static inline ideal Compute2LeadingSyzygyTerms(const ideal& L, const SchreyerSyzygyComputationFlags A)
{
  SchreyerSyzygyComputation syz(L, NULL, A);
  syz.ComputeLeadingSyzygyTerms(true);
  ideal LL, TT;
  syz.ReadOffResult(LL, TT);
  return LL; // assume TT is NULL!
}

static inline poly FindReducer(poly product, poly syzterm,
                               ideal L, ideal LS, const SchreyerSyzygyComputationFlags A)
{
  SchreyerSyzygyComputation syz(L, NULL, LS, A);
  return syz._FindReducer(product, syzterm);
}

static inline poly TraverseTail(poly multiplier, poly tail,
                                ideal L, ideal T, ideal LS, const SchreyerSyzygyComputationFlags A)
{
  SchreyerSyzygyComputation syz(L, T, LS, A);
  return syz.TraverseTail(multiplier, tail);
}

static inline poly ReduceTerm(poly multiplier, poly term4reduction, poly syztermCheck,
                              ideal L, ideal T, ideal LS, const SchreyerSyzygyComputationFlags A)
{
  SchreyerSyzygyComputation syz(L, T, LS, A);
  return syz.ReduceTerm(multiplier, term4reduction, syztermCheck);
}


static inline poly SchreyerSyzygyNF(poly syz_lead, poly syz_2,
                                    ideal L, ideal T, ideal LS, const SchreyerSyzygyComputationFlags A)
{
  SchreyerSyzygyComputation syz(L, T, LS, A);
  return syz.SchreyerSyzygyNF(syz_lead, syz_2);
}

#endif
/* #ifndef SYZEXTRA_H */

// Vi-modeline: vim: filetype=c:syntax:shiftwidth=2:tabstop=8:textwidth=0:expandtab


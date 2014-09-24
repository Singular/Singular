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

// include basic definitions
#include "singularxx_defs.h"

struct spolyrec; typedef struct spolyrec polyrec; typedef polyrec* poly;
struct ip_sring; typedef struct ip_sring* ring; typedef struct ip_sring const* const_ring;

struct sip_sideal; typedef struct sip_sideal *       ideal;
class idrec; typedef idrec *   idhdl;

class sBucket; typedef sBucket* sBucket_pt;
class kBucket; typedef kBucket* kBucket_pt;


BEGIN_NAMESPACE_SINGULARXX    BEGIN_NAMESPACE(SYZEXTRA)

poly leadmonom(const poly p, const ring r, const bool bSetZeroComp = true);

/// return the tail of a given polynomial or vector
/// returns NULL if input is NULL, otherwise
/// the result is a new polynomial/vector in the ring r
poly p_Tail(const poly p, const ring r);


/// return the tail of a given ideal or module
/// returns NULL if input is NULL, otherwise
/// the result is a new ideal/module in the ring r
/// NOTE: the resulting rank is autocorrected
ideal id_Tail(const ideal id, const ring r);

/// inplace sorting of the module (ideal) id wrt <_(c,ds)
void Sort_c_ds(const ideal id, const ring r);





/// Computation attribute storage
struct SchreyerSyzygyComputationFlags
{
    SchreyerSyzygyComputationFlags(idhdl rootRingHdl);

    SchreyerSyzygyComputationFlags(const SchreyerSyzygyComputationFlags& attr):
        __DEBUG__(attr.__DEBUG__),
  //      __SYZCHECK__(attr.__SYZCHECK__),
        __LEAD2SYZ__(attr.__LEAD2SYZ__),  __TAILREDSYZ__(attr.__TAILREDSYZ__),
        __HYBRIDNF__(attr.__HYBRIDNF__), __IGNORETAILS__(attr.__IGNORETAILS__),
        __SYZNUMBER__(attr.__SYZNUMBER__), __TREEOUTPUT__(attr.__TREEOUTPUT__),
        m_rBaseRing(attr.m_rBaseRing)
    {}

  /// output all the intermediate states
  const int __DEBUG__; // DebugOutput;

  //  const bool __SYZCHECK__; // CheckSyzygyProperty: never tested here...

  /// ?
  const int __LEAD2SYZ__; // TwoLeadingSyzygyTerms;

  /// Reduce syzygy tails wrt the leading syzygy terms
  const int __TAILREDSYZ__; // TailReducedSyzygies;

  /// Use the usual NF's S-poly reduction while dropping lower order terms
  /// 2 means - smart selection!
  const int __HYBRIDNF__; // UseHybridNF


  /// ignore tails and compute the pure Schreyer frame
  const int __IGNORETAILS__; // @IGNORETAILS

  /// Syzygy level (within a resolution)
  mutable int __SYZNUMBER__;

  inline void  nextSyzygyLayer() const
  {
     __SYZNUMBER__++;
  }

  /// output lifting tree
  const int __TREEOUTPUT__;

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

  public:

    const unsigned long m_sev; ///< not short exp. vector
        // NOTE/TODO: either of the following should be enough:
    const unsigned int  m_label; ///< index in the main L[] + 1
    const poly          m_lt; ///< the leading term itself L[label-1]

  public:
    bool DivisibilityCheck(const poly product, const unsigned long not_sev, const ring r) const;
    bool DivisibilityCheck(const poly multiplier, const poly t, const unsigned long not_sev, const ring r) const;

  private:
    // disable the following:
    CLeadingTerm();
    CLeadingTerm(const CLeadingTerm&);
    void operator=(const CLeadingTerm&);
};


// TODO: needs a specialized variant without a component (hash!)
class CReducerFinder: public SchreyerSyzygyComputationFlags
{
  friend class CDivisorEnumerator;
  friend class CDivisorEnumerator2;

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

    // TODO: save shortcut (syz: |-.->) LM(LM(m) * "t") -> syz?
    poly // const_iterator // TODO: return const_iterator it, s.th: it->m_lt is the needed
    FindReducer(const poly product, const poly syzterm, const CReducerFinder& checker) const;

    bool IsDivisible(const poly q) const;

    inline bool IsNonempty() const { return !m_hash.empty(); }

    /// is the term to be "preprocessed" as lower order term or lead to only reducible syzygies...
    int PreProcessTerm(const poly t, CReducerFinder& syzChecker) const;

    poly FindReducer(const poly multiplier, const poly monom, const poly syzterm, const CReducerFinder& checker) const;

#ifndef NDEBUG
    void DebugPrint() const;
#endif

  private:
    ideal m_L; ///< only for debug

    CReducersHash m_hash; // can also be replaced with a vector indexed by components

  private:
    CReducerFinder(const CReducerFinder&);
    void operator=(const CReducerFinder&);
};

extern ideal id_Copy (const ideal, const ring);
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
        m_sum_bucket(NULL), m_spoly_bucket(NULL)
    {
    }

    /// Construct a global object for given input data (separated into leads & tails)
    SchreyerSyzygyComputation(const ideal idLeads, const ideal idTails, const ideal syzLeads, const SchreyerSyzygyComputationFlags setting):
        SchreyerSyzygyComputationFlags(setting),
        m_idLeads(idLeads), m_idTails(id_Copy(idTails, setting.m_rBaseRing)),
        m_syzLeads(syzLeads), m_syzTails(NULL),
        m_LS(syzLeads), m_lcm(m_idLeads, setting),
        m_div(m_idLeads, setting), m_checker(NULL, setting), m_cache(),
        m_sum_bucket(NULL), m_spoly_bucket(NULL)
    {
      if( __TAILREDSYZ__ && !__IGNORETAILS__)
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

    /// Read off the results while detaching them from this object
    /// NOTE: no copy!
    inline void ReadOffResult(ideal& syzL, ideal& syzT)
    {
      syzL = m_syzLeads; syzT = m_syzTails;

      m_syzLeads = m_syzTails = NULL; // m_LS ?
    }

    /// The main driver function: computes
    void ComputeSyzygy();

    /// Computes Syz(leads) or only LEAD of it.
    /// The result is stored into m_syzLeads
    void ComputeLeadingSyzygyTerms(bool bComputeSecondTerms = true);

    poly SchreyerSyzygyNF(const poly syz_lead, poly syz_2 = NULL) const;

    /// High level caching function!!!
    poly TraverseTail(poly multiplier, const int tail) const;

    // REMOVE?
    /// called only from above and from outside (for testing)
    poly TraverseTail(poly multiplier, poly tail) const;

    /// TODO: save shortcut (syz: |-.->) LM(m) * "t" -> ? ???
    poly ReduceTerm(poly multiplier, poly term4reduction, poly syztermCheck) const;

    /// low level computation...
    poly ComputeImage(poly multiplier, const int tail) const;

    //
    poly TraverseNF(const poly syz_lead, const poly syz_2 = NULL) const;


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

    /*
    // need more data here:
    // (m_idLeads : m_tailterm) = (m, pos, compl), s.th: compl * m_tailterm divides m_idLeads[pos]
    // but resulting sysygy compl * gen(pos) should not be in
    // Idea: extend CReducerFinder??!!
    struct CTailTerm
    {
      const poly m_tailterm;

      const CReducerFinder m_reducers; // positions are labels (in m_idLeads)...
      // compl - to be computed if needed?

      CTailTerm(const poly tt, const CReducerFinder reds): m_tailterm(tt), m_reducers(reds) {}
    };

    typedef std::vector<const CTailTerm*> TTail;
    typedef std::vector<TTail> TTailTerms;

    TTailTerms m_idTailTerms;
    */

    mutable TCache m_cache; // cacher comp + poly -> poly! // mutable???

/// TODO: look into m_idTailTerms!!!!!!!!!!!!!!!!!!!!!!!! map? heaps???
    // NOTE/TODO: the following globally shared buckets violate reentrance - they should rather belong to TLS!

    /// used for simple summing up
    mutable sBucket_pt m_sum_bucket;

    /// for S-Polynomial reductions
    mutable kBucket_pt m_spoly_bucket;
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

END_NAMESPACE

END_NAMESPACE_SINGULARXX

#endif
/* #ifndef SYZEXTRA_H */

// Vi-modeline: vim: filetype=c:syntax:shiftwidth=2:tabstop=8:textwidth=0:expandtab


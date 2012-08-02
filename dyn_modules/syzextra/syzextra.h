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

struct  spolyrec;
typedef struct spolyrec    polyrec;
typedef polyrec *          poly;

struct ip_sring;
typedef struct ip_sring *         ring;

struct sip_sideal;
typedef struct sip_sideal *       ideal;

class idrec;
typedef idrec *   idhdl;

BEGIN_NAMESPACE_SINGULARXX    BEGIN_NAMESPACE(SYZEXTRA)

poly leadmonom(const poly p, const ring r);

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
      __HYBRIDNF__(attr.__HYBRIDNF__),  m_rBaseRing(attr.m_rBaseRing)
  {}
      

  /// output all the intermediate states
  const bool __DEBUG__; // DebugOutput;

//  const bool __SYZCHECK__; // CheckSyzygyProperty: never tested here...

  /// ?
  const bool __LEAD2SYZ__; // TwoLeadingSyzygyTerms;

  /// Reduce syzygy tails wrt the leading syzygy terms
  const bool __TAILREDSYZ__; // TailReducedSyzygies;

  /// Use the usual NF's S-poly reduction while dropping lower order terms
  const bool __HYBRIDNF__; // UseHybridNF

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


class CReducerFinder: public SchreyerSyzygyComputationFlags
{
  private:
    class CLeadingTerm
    {
      public: 
        CLeadingTerm(unsigned int id,  const poly p, const ring);

      private:
        CLeadingTerm();

      public:

        const unsigned long m_sev; ///< not short exp. vector
        // NOTE/TODO: either of the following should be enough:
        const unsigned int  m_label; ///< index in the main L[] + 1
        const poly          m_lt; ///< the leading term itself L[label-1]
    };

    typedef long TComponentKey;
    typedef std::vector<const CLeadingTerm*> TReducers;
    typedef std::map< TComponentKey, TReducers> CReducersHash;
    
  public:
    /// goes over all leading terms
    CReducerFinder(const ideal L, const SchreyerSyzygyComputationFlags& flags);

    void Initialize(const ideal L);

    ~CReducerFinder();

    // TODO: save shortcut (syz: |-.->) LM(LM(m) * "t") -> syz?    
    poly FindReducer(const poly product, const poly syzterm, const CReducerFinder& checker) const;

    bool IsDivisible(const poly q) const;

    bool IsNonempty() const { return (m_hash.size() > 0); }

  private:
    ideal m_L; ///< only for debug

    CReducersHash m_hash; // can also be replaced with a vector indexed by components
};



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
        m_idLeads(idLeads), m_idTails(idTails), 
        m_syzLeads(NULL), m_syzTails(NULL), m_LS(NULL),
        m_lcm(m_idLeads, setting), m_div(m_idLeads, setting), m_checker(NULL, setting)
    {
    }

    /// Construct a global object for given input data (separated into leads & tails)
    SchreyerSyzygyComputation(const ideal idLeads, const ideal idTails, const ideal syzLeads, const SchreyerSyzygyComputationFlags setting):
        SchreyerSyzygyComputationFlags(setting),
        m_idLeads(idLeads), m_idTails(idTails), 
        m_syzLeads(syzLeads), m_syzTails(NULL), m_LS(syzLeads), 
        m_lcm(m_idLeads, setting), m_div(m_idLeads, setting), m_checker(NULL, setting)
    {
      if( __TAILREDSYZ__ && syzLeads != NULL )
        m_checker.Initialize(syzLeads);
    }

    
    /// Destructor should not destruct the resulting m_syzLeads, m_syzTails. 
    ~SchreyerSyzygyComputation(){ CleanUp(); }

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

    poly SchreyerSyzygyNF(poly syz_lead, poly syz_2) const;

    // TODO: store m * @tail -.-^-.-^-.--> ?
    poly TraverseTail(poly multiplier, poly tail) const;

    // TODO: save shortcut (syz: |-.->) LM(m) * "t" -> ?
    poly ReduceTerm(poly multiplier, poly term4reduction, poly syztermCheck) const;

  public:
    /// just for testing via the wrapper below
    inline poly _FindReducer(const poly product, const poly syzterm) const
        { return m_div.FindReducer(product, syzterm, m_checker); }  
    
  protected:

    /// just leading terms
    ideal Compute1LeadingSyzygyTerms();

    /// leading + second terms
    ideal Compute2LeadingSyzygyTerms();

    /// Clean up all the accumulated data
    void CleanUp() {}

  private:
    /// input leading terms
    const ideal m_idLeads;

    /// input tails
    const ideal m_idTails;

    /// output (syzygy) leading terms (+2nd terms?)
    ideal m_syzLeads;

    /// output (syzygy) tails
    ideal m_syzTails;

    /// Bitmask for variables occuring in leading terms
    const CLCM m_lcm;

    /// Divisor finder
    const CReducerFinder m_div;

    /// for checking tail-terms and makeing them irreducible (wrt m_LS!)
    CReducerFinder m_checker;

    /*mutable?*/ ideal m_LS; ///< leading syzygy terms used for reducing syzygy tails
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


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
  
  /// output all the intermediate states
  const bool __DEBUG__; // DebugOutput;

  /// ?
  const bool __SYZCHECK__; // CheckSyzygyProperty;

  /// ?
  const bool __LEAD2SYZ__; // TwoLeadingSyzygyTerms;

  /// Reduce syzygy tails wrt the leading syzygy terms
  const bool __TAILREDSYZ__; // TailReducedSyzygies;

  /// Use the usual NF's S-poly reduction while dropping lower order terms
  const bool __HYBRIDNF__; // UseHybridNF

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
class SchreyerSyzygyComputation
{
  public:
    /// Construct a global object for given input data (separated into leads & tails)
    SchreyerSyzygyComputation(const ideal idLeads, const ideal idTails, const ring rBaseRing, const SchreyerSyzygyComputationFlags attribues):
        m_rBaseRing(rBaseRing),
        m_idLeads(idLeads), m_idTails(idTails), 
        m_syzLeads(NULL), m_syzTails(NULL), m_LS(NULL), m_atttributes(attribues) {}


    /// Construct a global object for given input data (separated into leads & tails)
    SchreyerSyzygyComputation(const ideal idLeads, const ideal idTails, const ideal syzLeads, const ring rBaseRing, const SchreyerSyzygyComputationFlags attribues):
        m_rBaseRing(rBaseRing),
        m_idLeads(idLeads), m_idTails(idTails), 
        m_syzLeads(NULL), m_syzTails(NULL), m_LS(syzLeads), m_atttributes(attribues) {}

    
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

    // TODO: save shortcut (syz: |-.->) LM(LM(m) * "t") -> syz?
    poly FindReducer(poly product, poly syzterm) const;
    
    poly SchreyerSyzygyNF(poly syz_lead, poly syz_2) const;

    // TODO: store m * @tail -.-^-.-^-.--> ?
    poly TraverseTail(poly multiplier, poly tail) const;

    // TODO: save shortcut (syz: |-.->) LM(m) * "t" -> ?
    poly ReduceTerm(poly multiplier, poly term4reduction, poly syztermCheck) const;

  protected:
    
    /// Clean up all the accumulated data
    void CleanUp() {}

  private:
    /// global base ring
    const ring  m_rBaseRing;

    /// input leading terms
    const ideal m_idLeads;

    /// input tails
    const ideal m_idTails;

    /// output (syzygy) leading terms (+2nd terms?)
    ideal m_syzLeads;

    /// output (syzygy) tails
    ideal m_syzTails;

    /*mutable?*/ ideal m_LS; ///< leading syzygy terms used for reducing syzygy tails

    const SchreyerSyzygyComputationFlags m_atttributes;
};


// The following wrappers are just for testing separate functions on highest level (within schreyer.lib)

static inline void ComputeSyzygy(const ideal L, const ideal T, ideal& LL, ideal& TT, const ring R, const SchreyerSyzygyComputationFlags A)
{
  SchreyerSyzygyComputation syz(L, T, R, A);
  syz.ComputeSyzygy();
  syz.ReadOffResult(LL, TT);
}

static inline ideal ComputeLeadingSyzygyTerms(const ideal& L, const ring R, const SchreyerSyzygyComputationFlags A)
{
  SchreyerSyzygyComputation syz(L, NULL, R, A);
  syz.ComputeLeadingSyzygyTerms(false);
  ideal LL, TT;
  syz.ReadOffResult(LL, TT);
  return LL; // assume TT is NULL!
}

static inline ideal Compute2LeadingSyzygyTerms(const ideal& L, const ring R, const SchreyerSyzygyComputationFlags A)
{
  SchreyerSyzygyComputation syz(L, NULL, R, A);
  syz.ComputeLeadingSyzygyTerms(true);
  ideal LL, TT;
  syz.ReadOffResult(LL, TT);
  return LL; // assume TT is NULL!
}

static inline poly FindReducer(poly product, poly syzterm,
                               ideal L, ideal LS, const ring R, const SchreyerSyzygyComputationFlags A)
{
  SchreyerSyzygyComputation syz(L, NULL, LS, R, A);
  return syz.FindReducer(product, syzterm);
}

static inline poly TraverseTail(poly multiplier, poly tail, 
                                ideal L, ideal T, ideal LS, const ring R, const SchreyerSyzygyComputationFlags A)
{
  SchreyerSyzygyComputation syz(L, T, LS, R, A);
  return syz.TraverseTail(multiplier, tail);
}

static inline poly ReduceTerm(poly multiplier, poly term4reduction, poly syztermCheck,
                              ideal L, ideal T, ideal LS, const ring R, const SchreyerSyzygyComputationFlags A)
{
  SchreyerSyzygyComputation syz(L, T, LS, R, A);
  return syz.ReduceTerm(multiplier, term4reduction, syztermCheck);
}


static inline poly SchreyerSyzygyNF(poly syz_lead, poly syz_2,
                                    ideal L, ideal T, ideal LS, const ring R, const SchreyerSyzygyComputationFlags A)
{
  SchreyerSyzygyComputation syz(L, T, LS, R, A);
  return syz.SchreyerSyzygyNF(syz_lead, syz_2);
}

END_NAMESPACE
   
END_NAMESPACE_SINGULARXX

#endif 
/* #ifndef SYZEXTRA_H */

// Vi-modeline: vim: filetype=c:syntax:shiftwidth=2:tabstop=8:textwidth=0:expandtab


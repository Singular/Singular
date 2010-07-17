#ifndef SCA_H
#define SCA_H

/****************************************
*  Computer Algebra System SINGULAR     *
****************************************/
/* $Id$ */

#include <kernel/ring.h>
#include <kernel/gring.h>
#include <kernel/structs.h>
#include <kernel/intvec.h>


// we must always have this test!
inline bool rIsSCA(const ring r)
{
#ifdef HAVE_PLURAL
  return rIsPluralRing(r) && (ncRingType(r) == nc_exterior);
#else
  return false;
#endif
}


#ifdef HAVE_PLURAL
// we must always have this test!
inline ideal SCAQuotient(const ring r)
{
  assume(rIsSCA(r));
  return r->GetNC()->SCAQuotient();
}

#include <kernel/gring.h>


inline unsigned int scaFirstAltVar(ring r)
{
  assume(rIsSCA(r));

  return (r->GetNC()->FirstAltVar());
};

inline unsigned int scaLastAltVar(ring r)
{
  assume(rIsSCA(r));

  return (r->GetNC()->LastAltVar());
};


// The following inlines are just helpers for setup functions.
inline void scaFirstAltVar(ring r, int n)
{
  assume(rIsSCA(r));

  r->GetNC()->FirstAltVar() = n;
};

inline void scaLastAltVar(ring r, int n)
{
  assume(rIsSCA(r));

  r->GetNC()->LastAltVar() = n;
};



///////////////////////////////////////////////////////////////////////////////////////////
// fast procedures for for SuperCommutative Algebras:
///////////////////////////////////////////////////////////////////////////////////////////

// this is not a basic operation... but it for efficiency we did it specially for SCA:
// return x_i * pPoly; preserve pPoly.
poly sca_pp_Mult_xi_pp(unsigned int i, const poly pPoly, const ring rRing);

// set pProcs for r and the variable p_Procs
// should be used by nc_p_ProcsSet in "gring.h"
void sca_p_ProcsSet(ring rGR, p_Procs_s* p_Procs);

//////////////////////////////////////////////////////////////////////////////////////

// TODO: correct the following descriptions...

// tests whether p is bi-homogeneous with respect to the given variable'(component')-weights
// ps: polynomial is bi-homogeneous iff all terms have the same bi-degree (x,y).
bool p_IsBiHomogeneous(const poly p,
  const intvec *wx, const intvec *wy,
  const intvec *wCx, const intvec *wCy,
  int &dx, int &dy,
  const ring r);


//////////////////////////////////////////////////////////////////////////////////////

// tests whether p is bi-homogeneous with respect to the given variable'(component')-weights
// ps: ideal is bi-homogeneous iff all its generators are bi-homogeneous polynomials.
bool id_IsBiHomogeneous(const ideal id,
  const intvec *wx, const intvec *wy,
  const intvec *wCx, const intvec *wCy,
  const ring r);


//////////////////////////////////////////////////////////////////////////////////////

// Scecial for SCA:

// returns an intvector with [nvars(r)] integers [1/0]
// 1 - for commutative variables
// 0 - for anticommutative variables
intvec *ivGetSCAXVarWeights(const ring r);

// returns an intvector with [nvars(r)] integers [1/0]
// 0 - for commutative variables
// 1 - for anticommutative variables
intvec *ivGetSCAYVarWeights(const ring r);


inline bool p_IsSCAHomogeneous(const poly p,
  const intvec *wCx, const intvec *wCy,
  const ring r)
{
  // inefficient! don't use it in time-critical code!
  intvec *wx = ivGetSCAXVarWeights(r);
  intvec *wy = ivGetSCAYVarWeights(r);

  int x,y;

  bool homog = p_IsBiHomogeneous( p, wx, wy, wCx, wCy, x, y, r );

  delete wx;
  delete wy;

  return homog;
}


inline bool id_IsSCAHomogeneous(const ideal id,
  const intvec *wCx, const intvec *wCy,
  const ring r)
{
  // inefficient! don't use it in time-critical code!
  intvec *wx = ivGetSCAXVarWeights(r);
  intvec *wy = ivGetSCAYVarWeights(r);

  bool homog = id_IsBiHomogeneous( id, wx, wy, wCx, wCy, r );

  delete wx;
  delete wy;

  return homog;
}


//////////////////////////////////////////////////////////////////////////////////////

// reduce polynomial p modulo <y_i^2> , i = iFirstAltVar .. iLastAltVar
poly p_KillSquares(const poly p,
  const unsigned int iFirstAltVar, const unsigned int iLastAltVar,
  const ring r);

//////////////////////////////////////////////////////////////////////////////////////

// reduce ideal id modulo <y_i^2> , i = iFirstAltVar .. iLastAltVar
// optional argument bSkipZeroes allow skipping of zero entries, by
// default - no skipping!
ideal id_KillSquares(const ideal id,
  const unsigned int iFirstAltVar, const unsigned int iLastAltVar,
  const ring r, const bool bSkipZeroes = false);

// for benchmarking
bool sca_Force(ring rGR, int b, int e);


#ifdef PLURAL_INTERNAL_DECLARATIONS

// should be used only inside nc_SetupQuotient!
// Check whether this our case:
//  1. rG is  a commutative polynomial ring \otimes anticommutative algebra
//  2. factor ideal rGR->qideal contains squares of all alternating variables.
//
// if yes, make rGR a super-commutative algebra!
// NOTE: Factors of SuperCommutative Algebras are supported this way!
//
//  rG == NULL means that there is no separate base G-algebra in this
//  case take rGR == rG

// special case: bCopy == true (default value: false)
// meaning: rGR copies structure from rG
// (maybe with some minor changes, which don't change the type!)
bool sca_SetupQuotient(ring rGR, ring rG, bool bCopy);

#endif // PLURAL_INTERNAL_DECLARATIONS


#else
// these must not be used at all.
// #define scaFirstAltVar(R) 0
// #define scaLastAltVar(R) 0
#endif
#endif // #ifndef SCA_H

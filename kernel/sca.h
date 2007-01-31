#ifndef GRING_SUPER_COMMUTATIVE_ALGEBRA_H
#define GRING_SUPER_COMMUTATIVE_ALGEBRA_H

/****************************************
*  Computer Algebra System SINGULAR     *
****************************************/
/* $Id: sca.h,v 1.7 2007-01-31 23:51:25 motsak Exp $ */

#include <ring.h>
#include <gring.h>
#include <structs.h>


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
#include <gring.h>


inline unsigned int scaFirstAltVar(ring r)
{
  assume(rIsSCA(r));

  return (r->nc->FirstAltVar());
};

inline unsigned int scaLastAltVar(ring r)
{
  assume(rIsSCA(r));

  return (r->nc->LastAltVar());
};


// The following inlines are just helpers for setup functions.
inline void scaFirstAltVar(ring r, int n)
{
  assume(rIsSCA(r));

  r->nc->FirstAltVar() = n;
};

inline void scaLastAltVar(ring r, int n)
{
  assume(rIsSCA(r));

  r->nc->LastAltVar() = n;
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

// tests whether p is bi-homogeneous without respect to the actual weigths(=>all ones)
// Polynomial is bi-homogeneous iff all monomials have the same bi-degree (x,y).
// Y are ones from iFirstAltVar up to iLastAltVar
bool p_IsBiHomogeneous(const poly p, 
  const unsigned int iFirstAltVar, const unsigned int iLastAltVar, 
  const ring r);

//////////////////////////////////////////////////////////////////////////////////////

// returns true if id is bi-homogenous without respect to the aktual weights(=> all ones)
// Ideal is bi-homogeneous iff all its generators are bi-homogeneous.
bool id_IsBiHomogeneous(const ideal id, 
  const unsigned int iFirstAltVar, const unsigned int iLastAltVar, 
  const ring r);

//////////////////////////////////////////////////////////////////////////////////////

// reduce polynomial p modulo <y_i^2> , i = iFirstAltVar .. iLastAltVar
poly p_KillSquares(const poly p, 
  const unsigned int iFirstAltVar, const unsigned int iLastAltVar, 
  const ring r); 

//////////////////////////////////////////////////////////////////////////////////////

// reduce ideal id modulo <y_i^2> , i = iFirstAltVar .. iLastAltVar
ideal id_KillSquares(const ideal id, 
  const unsigned int iFirstAltVar, const unsigned int iLastAltVar, 
  const ring r); 


#ifdef PLURAL_INTERNAL_DECLARATIONS

// should be used only inside nc_SetupQuotient!
// Check whether this our case:
//  1. rG is  a commutative polynomial ring \otimes anticommutative algebra
//  2. factor ideal rGR->qideal contains squares of all alternating variables.
// 
// if yes, make rGR a super-commutative algebra!
// NOTE: Factors of SuperCommutative Algebras are supported this way!
bool sca_SetupQuotient(ring rGR, const ring rG);

#endif // PLURAL_INTERNAL_DECLARATIONS


#else
// these must not be used at all. 
// #define scaFirstAltVar(R) 0
// #define scaLastAltVar(R) 0
#endif
#endif // #ifndef GRING_SUPER_COMMUTATIVE_ALGEBRA_H

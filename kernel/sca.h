#ifndef GRING_SUPER_COMMUTATIVE_ALGEBRA_H
#define GRING_SUPER_COMMUTATIVE_ALGEBRA_H

/****************************************
*  Computer Algebra System SINGULAR     *
****************************************/
/* $Id: sca.h,v 1.2 2007-01-09 11:21:15 Singular Exp $ */

#include <structs.h>
// #include <polys-impl.h>
// #include <ring.h>
#include <gring.h>

inline bool rIsSCA(ring r)
{
  if(!rIsPluralRing(r))
    return false;

  const bool result = (ncRingType(r) == nc_exterior);

//   if( result )
//     assume( ((scaFirstAltVar(r) != 0) && (scaLastAltVar(r) != 0)) );

  return(result);
}


inline unsigned int scaFirstAltVar(ring r)
{
  assume(rIsSCA(r));

#ifdef HAVE_PLURAL
  return (r->nc->FirstAltVar());
#else
  return (0); //
#endif
};

inline unsigned int scaLastAltVar(ring r)
{
  assume(rIsSCA(r));

#ifdef HAVE_PLURAL
  return (r->nc->LastAltVar());
#else
  return (0); //
#endif
};

inline void scaFirstAltVar(ring r, int n)
{
  assume(rIsSCA(r));

#ifdef HAVE_PLURAL
  r->nc->FirstAltVar() = n;
#endif
};

inline void scaLastAltVar(ring r, int n)
{
  assume(rIsSCA(r));

#ifdef HAVE_PLURAL
  r->nc->LastAltVar() = n;
#endif
};





///////////////////////////////////////////////////////////////////////////////////////////
// fast procedures for for SuperCommutative Algebras:
///////////////////////////////////////////////////////////////////////////////////////////

// this is not a basic operation... but it for efficiency we did it specially for SCA:
// return x_i * pPoly; preserve pPoly.
poly sca_pp_Mult_xi_pp(unsigned int i, const poly pPoly, const ring rRing);

// set pProcs for r and the variable p_Procs
// should be used by p_ProcsSet in "p_Procs_Set.h"
void SetProcsSCA(ring& rGR, p_Procs_s* p_Procs);


// is this an exterior algebra or a commutative polynomial ring \otimes exterior algebra?
// we should check whether qr->qideal is of the form: y_i^2, y_{i+1}^2, \ldots, y_j^2 (j > i)
// if yes, setup qr->nc->type, etc.
// should be used inside QRing definition!
// NOTE: (&TODO): Factors of SuperCommutative Algebras are not supported this way!
bool SetupSCA(ring& rGR, const ring rG);




// tests whether p is sca(y)-homogeneous without respect to the actual weigths(=>all ones)
BOOLEAN p_IsYHomogeneous(const poly p, const ring r);

// returns true if id is sca(y)-homogenous without respect to the aktual weights(=> all ones)
BOOLEAN id_IsYHomogeneous(const ideal id, const ring r);



// #define PLURAL_INTERNAL_DECLARATIONS

#ifdef PLURAL_INTERNAL_DECLARATIONS
// poly functions defined in p_Procs :

// return pPoly * pMonom; preserve pPoly and pMonom.
poly sca_pp_Mult_mm(const poly pPoly, const poly pMonom, const ring rRing, poly &);

// return pMonom * pPoly; preserve pPoly and pMonom.
poly sca_mm_Mult_pp(const poly pMonom, const poly pPoly, const ring rRing);

// return pPoly * pMonom; preserve pMonom, destroy or reuse pPoly.
poly sca_p_Mult_mm(poly pPoly, const poly pMonom, const ring rRing);

// return pMonom * pPoly; preserve pMonom, destroy or reuse pPoly.
poly sca_mm_Mult_p(const poly pMonom, poly pPoly, const ring rRing);


// compute the spolynomial of p1 and p2
poly sca_SPoly(const poly p1, const poly p2, const ring r);
poly sca_ReduceSpoly(const poly p1, poly p2, const ring r);

// Modified Plural's Buchberger's algorithmus.
ideal sca_gr_bba(const ideal F, const ideal Q, const intvec *, const intvec *, kStrategy strat);

// Modified modern Sinuglar Buchberger's algorithm.
ideal sca_bba(const ideal F, const ideal Q, const intvec *w, const intvec *, kStrategy strat);

// Modified modern Sinuglar Mora's algorithm.
ideal sca_mora(const ideal F, const ideal Q, const intvec *w, const intvec *, kStrategy strat);
#endif


#endif // #ifndef GRING_SUPER_COMMUTATIVE_ALGEBRA_H


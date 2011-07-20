#ifndef GRING_H
#define GRING_H
/****************************************
*  Computer Algebra System SINGULAR     *
****************************************/
/* $Id$ */
/*
* ABSTRACT additional defines etc for --with-plural
*/

#ifdef HAVE_PLURAL


#include <kernel/structs.h>
#include <kernel/ring.h>
#include <kernel/matpol.h>

// BOOLEAN nc_CheckOrdCondition(matrix D, ring r);
// BOOLEAN nc_CheckOrdCondition(ring r); // with D == r->GetNC()->D

BOOLEAN nc_CheckSubalgebra(poly PolyVar, ring r);

// BOOLEAN nc_InitMultiplication(ring r); // should call nc_p_ProcsSet!
// NOTE: instead of constructing nc_struct and calling nc_InitMultiplication yourself - just create C, D and call nc_CallPlural!!!






// used by "rSum" from ring.cc only! 
// purpose init nc structure for initially commutative ring:
// "creates a commutative nc extension; "converts" comm.ring to a Plural ring"
ring nc_rCreateNCcomm(ring r); 


// poly _gnc_p_Mult_q(poly p, poly q, const int copy, const ring r);

// general multiplication:
poly _nc_p_Mult_q(poly p, poly q, const ring r);
poly _nc_pp_Mult_qq(const poly p, const poly q, const ring r);


void nc_PolyPolyRed(poly &b, poly p, number *c);




/* twostd: */
ideal twostd(ideal I);
/* Ann: */
ideal Approx_Step(ideal L);

/* complete reduction routines */

matrix nc_PrintMat(int a, int b, ring r, int metric);

poly p_CopyEmbed(poly p, ring srcRing, int shift, int par_shift);



// //////////////////////////////////////////////////////////////////////// //
// NC inlines

// //////////////////////////////////////////////////////

// returns m*p, does neither destroy p nor m
inline poly nc_mm_Mult_pp(const poly m, const poly p, const ring r)
{
  assume(rIsPluralRing(r));
  assume(r->GetNC()->p_Procs.mm_Mult_pp!=NULL);
  return r->GetNC()->p_Procs.mm_Mult_pp(m, p, r);
//  return pp_Mult_mm( p, m, r);
}


// returns m*p, does destroy p, preserves m
inline poly nc_mm_Mult_p(const poly m, poly p, const ring r)
{
  assume(rIsPluralRing(r));
  assume(r->GetNC()->p_Procs.mm_Mult_p!=NULL);
  return r->GetNC()->p_Procs.mm_Mult_p(m, p, r);
//   return p_Mult_mm( p, m, r);
}

inline poly nc_CreateSpoly(const poly p1, const poly p2, const ring r)
{
  assume(rIsPluralRing(r));
  assume(r->GetNC()->p_Procs.SPoly!=NULL);
  return r->GetNC()->p_Procs.SPoly(p1, p2, r);
}

inline poly nc_ReduceSpoly(const poly p1, poly p2, const ring r)
{
  assume(rIsPluralRing(r));
  assume(r->GetNC()->p_Procs.ReduceSPoly!=NULL);
#ifdef PDEBUG
//  assume(p_LmDivisibleBy(p1, p2, r));
#endif
  return r->GetNC()->p_Procs.ReduceSPoly(p1, p2, r);
}

/*
inline void nc_PolyReduce(poly &b, const poly p, number *c, const ring r) // nc_PolyPolyRed
{
  assume(rIsPluralRing(r));
//  assume(r->GetNC()->p_Procs.PolyReduce!=NULL);
//  r->GetNC()->p_Procs.PolyReduce(b, p, c, r);
}
*/

inline void nc_kBucketPolyRed(kBucket_pt b, poly p, number *c)
{
  assume(rIsPluralRing(currRing));

//   return gnc_kBucketPolyRedNew(b, p, c);

  assume(currRing->GetNC()->p_Procs.BucketPolyRed!=NULL);
  return currRing->GetNC()->p_Procs.BucketPolyRed(b, p, c);
}

inline void nc_BucketPolyRed_Z(kBucket_pt b, poly p, number *c)
{
  assume(rIsPluralRing(currRing));

//   return gnc_kBucketPolyRed_ZNew(b, p, c);

  assume(currRing->GetNC()->p_Procs.BucketPolyRed_Z!=NULL);
  return currRing->GetNC()->p_Procs.BucketPolyRed_Z(b, p, c);

}

inline ideal nc_GB(const ideal F, const ideal Q, const intvec *w, const intvec *hilb, kStrategy strat)
{
  assume(rIsPluralRing(currRing));

  assume(currRing->GetNC()->p_Procs.GB!=NULL);
  return currRing->GetNC()->p_Procs.GB(F, Q, w, hilb, strat);

/*
  if (rHasLocalOrMixedOrder(currRing))
  {
    assume(currRing->GetNC()->p_Procs.LocalGB!=NULL);
    return currRing->GetNC()->p_Procs.LocalGB(F, Q, w, hilb, strat);
  } else
  {
    assume(currRing->GetNC()->p_Procs.GlobalGB!=NULL);
    return currRing->GetNC()->p_Procs.GlobalGB(F, Q, w, hilb, strat);
  }
*/
}




#ifdef PLURAL_INTERNAL_DECLARATIONS

// we need nc_gr_initBba for sca_gr_bba and gr_bba.
void nc_gr_initBba(ideal F,kStrategy strat); 
#endif // PLURAL_INTERNAL_DECLARATIONS

#endif // HAVE_PLURAL :(
#endif // 

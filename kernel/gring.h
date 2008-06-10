#ifndef GRING_H
#define GRING_H
/****************************************
*  Computer Algebra System SINGULAR     *
****************************************/
/* $Id: gring.h,v 1.21 2008-06-10 10:17:31 motsak Exp $ */
/*
* ABSTRACT additional defines etc for --with-plural
*/

#ifdef HAVE_PLURAL


#include <structs.h>
#include <ring.h>

// the part, related to the interface
// Changes r, Assumes that all other input belongs to currRing
BOOLEAN nc_CallPlural(matrix CC, matrix DD, poly CN, poly DN, ring r,
                      bool bSetupQuotient = false,
                      bool bCopyInput = true,
                      bool bBeQuiet = false,
                      ring curr = currRing);

// BOOLEAN nc_CheckOrdCondition(matrix D, ring r);
// BOOLEAN nc_CheckOrdCondition(ring r); // with D == r->GetNC()->D

BOOLEAN nc_CheckSubalgebra(poly PolyVar, ring r);

// BOOLEAN nc_InitMultiplication(ring r); // should call nc_p_ProcsSet!
// NOTE: instead of constructing nc_struct and calling nc_InitMultiplication yourself - just create C, D and call nc_CallPlural!!!


BOOLEAN rIsLikeOpposite(ring rBase, ring rCandidate);


// set pProcs table for rGR and global variable p_Procs
// this should be used by p_ProcsSet in p_Procs_Set.h
void nc_p_ProcsSet(ring rGR, p_Procs_s* p_Procs);

// this function should be used inside QRing definition!
// we go from rG into factor ring rGR with factor ideal rGR->qideal.
bool nc_SetupQuotient(ring rGR, const ring rG = NULL); // rG == NULL means that there is no base G-algebra


// used by "rSum" from ring.cc only! 
// purpose init nc structure for initially commutative ring:
// "creates a commutative nc extension; "converts" comm.ring to a Plural ring"
ring nc_rCreateNCcomm(ring r); 

void ncCleanUp(nc_struct* p); // just free memory!
void ncCleanUp(ring r); // smaller than kill: just free mem
void ncKill(ring r); // complete destructor

BOOLEAN nc_rComplete(const ring src, ring dest, bool bSetupQuotient = true); // in ring.cc


// share the same nc-structure with a new copy ``res'' of ``r''.
// used by rCopy only.
// additionally inits multipication on ``res''!
void nc_rCopy0(ring res, const ring r);

// for p_Minus_mm_Mult_qq in pInline2.h
poly nc_p_Minus_mm_Mult_qq(poly p, const poly m, const poly q, int &lp,
                                    const int, const poly, const ring r);

// // for p_Plus_mm_Mult_qq in pInline2.h
// returns p + m*q destroys p, const: q, m
poly nc_p_Plus_mm_Mult_qq(poly p, const poly m, const poly q, int &lp,
                              const int, const ring r);


// poly _gnc_p_Mult_q(poly p, poly q, const int copy, const ring r);

// general multiplication:
poly _nc_p_Mult_q(poly p, poly q, const ring r);
poly _nc_pp_Mult_qq(const poly p, const poly q, const ring r);


/* subst: */
poly nc_pSubst(poly p, int n, poly e);

/* copy : */
poly nc_p_CopyGet(poly a, const ring r);
poly nc_p_CopyPut(poly a, const ring r);

void nc_PolyPolyRed(poly &b, poly p, number *c);



poly nc_CreateShortSpoly(poly p1, poly p2, const ring r=currRing);


/* brackets: */
poly nc_p_Bracket_qq(poly p, poly q);

/* twostd: */
ideal twostd(ideal I);
/* Ann: */
ideal Approx_Step(ideal L);

/* complete reduction routines */

matrix nc_PrintMat(int a, int b, ring r, int metric);

poly p_CopyEmbed(poly p, ring srcRing, int shift, int par_shift);
poly pOppose(ring Rop, poly p);
ideal idOppose(ring Rop, ideal I);




// returns the LCM of the head terms of a and b with given component
poly p_Lcm(const poly a, const poly b, const long lCompM, const ring r);

// returns the LCM of the head terms of a and b with component = max comp. of a & b
poly p_Lcm(const poly a, const poly b, const ring r);



// //////////////////////////////////////////////////////////////////////// //
// NC inlines

inline nc_type& ncRingType(nc_struct* p)
{
  assume(p!=NULL);
  return (p->type);
};

inline nc_type& ncRingType(ring r) // get and set
{
  assume(rIsPluralRing(r));
  return (ncRingType(r->GetNC()));
};

inline void ncRingType(ring r, nc_type t) // Set
{
  assume((r != NULL) && (r->GetNC() != NULL));
  ncRingType(r) = t;
};

inline nc_struct*& GetNC(ring r)
{
  return r->GetNC();
}; 




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
  if (pOrdSgn==-1)
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


// Macros used to access upper triangle matrices C,D... (which are actually ideals) // afaik
#define UPMATELEM(i,j,nVar) ( (nVar * ((i)-1) - ((i) * ((i)-1))/2 + (j)-1)-(i) )


#ifdef PLURAL_INTERNAL_DECLARATIONS

// we need nc_gr_initBba for sca_gr_bba and gr_bba.
void nc_gr_initBba(ideal F,kStrategy strat); 
BOOLEAN gnc_InitMultiplication(ring r, bool bSetupQuotient = false); // just for a moment

#endif // PLURAL_INTERNAL_DECLARATIONS

#endif // HAVE_PLURAL :(
#endif // 

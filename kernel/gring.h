#ifndef GRING_H
#define GRING_H
/****************************************
*  Computer Algebra System SINGULAR     *
****************************************/
/* $Id: gring.h,v 1.19 2007-02-07 10:49:40 Singular Exp $ */
/*
* ABSTRACT additional defines etc for --with-plural
*/

#ifdef HAVE_PLURAL

#include <structs.h>
#include <ring.h>

/* the part, related to the interface */
BOOLEAN nc_CallPlural(matrix CC, matrix DD, poly CN, poly DN, ring r);

BOOLEAN nc_CheckOrdCondition(matrix D, ring r);
BOOLEAN nc_CheckSubalgebra(poly PolyVar, ring r);
BOOLEAN nc_InitMultiplication(ring r); // should call nc_p_ProcsSet!
BOOLEAN rIsLikeOpposite(ring rBase, ring rCandidate);


// set pProcs table for rGR and global variable p_Procs
// this should be used by p_ProcsSet in p_Procs_Set.h
void nc_p_ProcsSet(ring rGR, p_Procs_s* p_Procs);

// this function should be used inside QRing definition!
// we go from rG into factor ring rGR with factor ideal rGR->qideal.
bool nc_SetupQuotient(ring rGR, const ring rG);


// used by "rSum" from ring.cc only! 
// purpose init nc structure for initially commutative ring:
// "creates a commutative nc extension; "converts" comm.ring to a Plural ring"
ring nc_rCreateNCcomm(ring r); 

void ncCleanUp(ring r); /* smaller than kill */
void ncKill(ring r);


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



// //////////////////////////////////////////////////////////////////////// //
// NC inlines



inline void ncRingType(ring r, nc_type t)
{
  assume((r != NULL) && (r->nc != NULL));
  r->nc->type = t;
};

inline nc_type ncRingType(ring r)
{
  assume(rIsPluralRing(r));

  return (r->nc->type);
};




// //////////////////////////////////////////////////////

// returns m*p, does neither destroy p nor m
inline poly nc_mm_Mult_pp(const poly m, const poly p, const ring r)
{
  assume(rIsPluralRing(r));
  assume(r->nc->p_Procs.mm_Mult_pp!=NULL);
  return r->nc->p_Procs.mm_Mult_pp(m, p, r);
//  return pp_Mult_mm( p, m, r);
}


// returns m*p, does destroy p, preserves m
inline poly nc_mm_Mult_p(const poly m, poly p, const ring r)
{
  assume(rIsPluralRing(r));
  assume(r->nc->p_Procs.mm_Mult_p!=NULL);
  return r->nc->p_Procs.mm_Mult_p(m, p, r);
//   return p_Mult_mm( p, m, r);
}

inline poly nc_CreateSpoly(const poly p1, const poly p2, const ring r)
{
  assume(rIsPluralRing(r));
  assume(r->nc->p_Procs.SPoly!=NULL);
  return r->nc->p_Procs.SPoly(p1, p2, r);
}

inline poly nc_ReduceSpoly(const poly p1, poly p2, const ring r)
{
  assume(rIsPluralRing(r));
  assume(r->nc->p_Procs.ReduceSPoly!=NULL);
  return r->nc->p_Procs.ReduceSPoly(p1, p2, r);
}

inline void nc_kBucketPolyRed(kBucket_pt b, poly p, number *c)
{
  assume(rIsPluralRing(currRing));

//   return gnc_kBucketPolyRedNew(b, p, c);

  assume(currRing->nc->p_Procs.BucketPolyRed!=NULL);
  return currRing->nc->p_Procs.BucketPolyRed(b, p, c);
}

inline void nc_BucketPolyRed_Z(kBucket_pt b, poly p, number *c)
{
  assume(rIsPluralRing(currRing));

//   return gnc_kBucketPolyRed_ZNew(b, p, c);

  assume(currRing->nc->p_Procs.BucketPolyRed_Z!=NULL);
  return currRing->nc->p_Procs.BucketPolyRed_Z(b, p, c);

}

inline ideal nc_GB(const ideal F, const ideal Q, const intvec *w, const intvec *hilb, kStrategy strat)
{
  assume(rIsPluralRing(currRing));

  assume(currRing->nc->p_Procs.GB!=NULL);
  return currRing->nc->p_Procs.GB(F, Q, w, hilb, strat);

/*
  if (pOrdSgn==-1)
  {
    assume(currRing->nc->p_Procs.LocalGB!=NULL);
    return currRing->nc->p_Procs.LocalGB(F, Q, w, hilb, strat);
  } else
  {
    assume(currRing->nc->p_Procs.GlobalGB!=NULL);
    return currRing->nc->p_Procs.GlobalGB(F, Q, w, hilb, strat);
  }
*/
}


// Macros used to access upper triangle matrices C,D... (which are actually ideals) // afaik
#define UPMATELEM(i,j,nVar) ( (nVar * ((i)-1) - ((i) * ((i)-1))/2 + (j)-1)-(i) )


#ifdef PLURAL_INTERNAL_DECLARATIONS

// we need nc_gr_initBba for sca_gr_bba and gr_bba.
void nc_gr_initBba(ideal F,kStrategy strat); 

#endif // PLURAL_INTERNAL_DECLARATIONS

#endif // HAVE_PLURAL :(
#endif // 

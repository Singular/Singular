#ifndef GRING_H
#define GRING_H
/****************************************
*  Computer Algebra System SINGULAR     *
****************************************/
/* $Id: gring.h,v 1.15 2007-01-03 00:17:10 motsak Exp $ */
/*
* ABSTRACT additional defines etc for --with-plural
*/

// #ifdef HAVE_PLURAL
#include "structs.h"

/* the part, related to the interface */
BOOLEAN nc_CallPlural(matrix CC, matrix DD, poly CN, poly DN, ring r);
BOOLEAN nc_CheckOrdCondition(matrix D, ring r);
BOOLEAN nc_CheckSubalgebra(poly PolyVar, ring r);
BOOLEAN nc_InitMultiplication(ring r);
BOOLEAN rIsLikeOpposite(ring rBase, ring rCandidate);

// set pProcs for r and global variable p_Procs
void SetProcsGNC(ring& rGR, p_Procs_s* p_Procs);

ring nc_rCreateNCcomm(ring r);
void ncKill(ring r);
void ncCleanUp(ring r); /* smaller than kill */

/* poly functions defined in p_Procs : */

/* other routines we need in addition : */
// poly gnc_p_Minus_mm_Mult_qq(poly p, const poly m, poly q, const ring r);

// #define PLURAL_INTERNAL_DECLARATIONS

#ifdef PLURAL_INTERNAL_DECLARATIONS
// // poly gnc_p_Minus_mm_Mult_qq_ign(poly p, const poly m, poly q, int & d1, poly d2, const ring ri, poly &d3);


poly gnc_pp_Mult_mm(const poly p, const poly m, const ring r, poly &last);
poly gnc_p_Mult_mm(poly p, const poly m, const ring r);
poly gnc_mm_Mult_p(const poly m, poly p, const ring r);
poly gnc_mm_Mult_pp(const poly m, const poly p, const ring r);
#endif



// // for p_Minus_mm_Mult_qq in pInline2.h
// poly nc_p_Minus_mm_Mult_qq(poly p, const poly m, const poly q, int &lp, int lq, const ring r);
// // for p_Plus_mm_Mult_qq in pInline2.h
// poly nc_p_Plus_mm_Mult_qq (poly p, const poly m, const poly q, int &lp, int lq, const ring r);


//
poly nc_p_Minus_mm_Mult_qq(poly p, const poly m, const poly q, int &lp,
                                    const int, const poly, const ring r);
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


#define PLURAL_INTERNAL_DECLARATIONS
#ifdef PLURAL_INTERNAL_DECLARATIONS
/* syzygies : */
poly gnc_CreateSpolyOld(const poly p1, const poly p2/*, poly spNoether*/, const ring r);
poly gnc_ReduceSpolyOld(const poly p1, poly p2/*, poly spNoether*/, const ring r);

poly gnc_CreateSpolyNew(const poly p1, const poly p2/*, poly spNoether*/, const ring r);
poly gnc_ReduceSpolyNew(const poly p1, poly p2/*, poly spNoether*/, const ring r);



void gnc_kBucketPolyRedNew(kBucket_pt b, poly p, number *c);
void gnc_kBucketPolyRed_ZNew(kBucket_pt b, poly p, number *c);

void gnc_kBucketPolyRedOld(kBucket_pt b, poly p, number *c);
void gnc_kBucketPolyRed_ZOld(kBucket_pt b, poly p, number *c);


// poly gnc_ReduceSpolyNew(poly p1, poly p2, poly spNoether, const ring r);
// void gnc_ReduceSpolyTail(poly p1, poly q, poly q2, poly spNoether, const ring r);

/* void nc_kBucketPolyRed(kBucket_pt b, poly p); */
void gr_initBba(ideal F,kStrategy strat);

ideal gnc_gr_bba (const ideal F, const ideal Q, const intvec *, const intvec *, kStrategy strat);
ideal gnc_gr_mora(const ideal, const ideal, const intvec *, const intvec *, kStrategy); // Not yet!
#endif


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



inline BOOLEAN rIsPluralRing(ring r)
{
#ifdef HAVE_PLURAL
  return ((r != NULL) && (r->nc != NULL) && (r->nc->type != nc_error));
#else
  return 0;
#endif
}

inline void ncRingType(ring r, nc_type t)
{
  assume((r != NULL) && (r->nc != NULL));
#ifdef HAVE_PLURAL
  r->nc->type = t;
#endif
};

inline nc_type ncRingType(ring r)
{
  assume(rIsPluralRing(r));

#ifdef HAVE_PLURAL
  return (r->nc->type);
 #else
  return (nc_error); //
#endif
};




// //////////////////////////////////////////////////////

// returns m*p, does neither destroy p nor m
inline poly mm_Mult_pp(const poly m, const poly p, const ring r)
{
#ifdef HAVE_PLURAL
  assume(rIsPluralRing(r));
  assume(r->nc->p_Procs.mm_Mult_pp!=NULL);
  return r->nc->p_Procs.mm_Mult_pp(m, p, r);
#else
  Werror("Wrong call to NC function\n");
//  return pp_Mult_mm( p, m, r);
#endif
}


// returns m*p, does destroy p, preserves m
inline poly mm_Mult_p(const poly m, poly p, const ring r)
{
#ifdef HAVE_PLURAL
  assume(rIsPluralRing(r));
  assume(r->nc->p_Procs.mm_Mult_p!=NULL);
  return r->nc->p_Procs.mm_Mult_p(m, p, r);
#else
  Werror("Wrong call to NC function\n");
//   return p_Mult_mm( p, m, r);
#endif
}

inline poly nc_SPoly(const poly p1, const poly p2, const ring r)
{
#ifdef HAVE_PLURAL
  assume(rIsPluralRing(r));
  assume(r->nc->p_Procs.SPoly!=NULL);
  return r->nc->p_Procs.SPoly(p1, p2, r);
#else
  Werror("Wrong call to NC function\n");
#endif
}

inline poly nc_ReduceSPoly(const poly p1, poly p2, const ring r)
{
#ifdef HAVE_PLURAL
  assume(rIsPluralRing(r));
  assume(r->nc->p_Procs.ReduceSPoly!=NULL);
  return r->nc->p_Procs.ReduceSPoly(p1, p2, r);
#else
  Werror("Wrong call to NC function\n");
#endif
}

inline void nc_BucketPolyRed(kBucket_pt b, poly p, number *c)
{
#ifdef HAVE_PLURAL
  assume(rIsPluralRing(currRing));

//   return gnc_kBucketPolyRedNew(b, p, c);

  assume(currRing->nc->p_Procs.BucketPolyRed!=NULL);
  return currRing->nc->p_Procs.BucketPolyRed(b, p, c);
#else
  Werror("Wrong call to NC function\n");
#endif
}

inline void nc_BucketPolyRed_Z(kBucket_pt b, poly p, number *c)
{
#ifdef HAVE_PLURAL
  assume(rIsPluralRing(currRing));

//   return gnc_kBucketPolyRed_ZNew(b, p, c);

  assume(currRing->nc->p_Procs.BucketPolyRed_Z!=NULL);
  return currRing->nc->p_Procs.BucketPolyRed_Z(b, p, c);

#else
  Werror("Wrong call to NC function\n");
#endif
}

inline ideal nc_GB(const ideal F, const ideal Q, const intvec *w, const intvec *hilb, kStrategy strat)
{
#ifdef HAVE_PLURAL
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
#else
  Werror("Wrong call to NC function\n");
#endif
}


// Macros used to access upper triangle matrices C,D... (which are actually ideals) // afaik
#define UPMATELEM(i,j,nVar) ( (nVar * ((i)-1) - ((i) * ((i)-1))/2 + (j)-1)-(i) )

#endif

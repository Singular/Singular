/****************************************
*  Computer Algebra System SINGULAR     *
****************************************/
/***************************************************************
 *  File:    ratgring.cc
 *  Purpose: Ore-noncommutative kernel procedures
 *  Author:  levandov (Viktor Levandovsky)
 *  Created: 8/00 - 11/00
 *  Version: $Id: ratgring.cc,v 1.9 2008-01-31 13:23:25 Singular Exp $
 *******************************************************************/
#include "mod2.h"
#include "ratgring.h"
#ifdef HAVE_RATGRING
#include "gring.h"
#include "febase.h"
#include "ring.h"
#include "polys.h"
#include "numbers.h"
#include "ideals.h"
#include "matpol.h"
#include "kbuckets.h"
#include "kstd1.h"
#include "sbuckets.h"
#include "prCopy.h"
#include "p_Mult_q.h"
#include "clapsing.h"

void pLcmRat(poly a, poly b, poly m, int rat_shift)
{
  /* shift is the last exp one should count with */
  int i;
  for (i=pVariables; i=rat_shift; i--)
  {
    pSetExp(m,i, si_max( pGetExp(a,i), pGetExp(b,i)));
  }
  pSetComp(m, si_max(pGetComp(a), pGetComp(b)));
  /* Don't do a pSetm here, otherwise hres/lres chockes */ 
}

// void pLcmRat(poly a, poly b, poly m, poly pshift)
// {
//   /* shift is the exp of rational elements */
//   int i;
//   for (i=pVariables; i; i--)
//   {
//     if (!pGetExp(pshift,i))
//     {
//       pSetExp(m,i, si_max( pGetExp(a,i), pGetExp(b,i)));
//     }
//     else
//     {
//       /* do we really need it? */
//       pSetExp(m,i,0);
//     }
//   }
//   pSetComp(m, si_max(pGetComp(a), pGetComp(b)));
//   /* Don't do a pSetm here, otherwise hres/lres chockes */ 
// }

/* returns a subpoly of p, s.t. its monomials have the same D-part */

poly p_HeadRat(poly p, int ishift, ring r)
{
  poly q   = pNext(p);
  if (q == NULL) return p;
  poly res = p_Head(p,r);
  while ( (q!=NULL) && (p_Comp_k_n(p, q, ishift+1, r)))
  {
    res = p_Add_q(res,p_Head(q,r),r);
    q   = pNext(q);
  }
  return res;
}

/* TO TEST!!! */
/* returns x-coeff of p, i.e. a poly in x, s.t. corresponding xd-monomials 
have the same D-part */

poly p_GetCoeffRat(poly p, int ishift, ring r)
{
  poly q   = pNext(p);
  poly res; // = p_Head(p,r);
  res = p_GetExp_k_n(p, ishift+1, r->N, r);
  p_SetCoeff(res,n_Copy(p_GetCoeff(p,r),r),r);
  poly s;
  while ((q!= NULL) && (p_Comp_k_n(p, q, ishift+1, r)))
  {
    s   = p_GetExp_k_n(q, ishift+1, r->N, r);
    p_SetCoeff(s,n_Copy(p_GetCoeff(q,r),r),r);
    res = p_Add_q(res,s,r);
    q   = pNext(q);
  }
  return res;
}

void p_LmDeleteAndNextRat(poly *p, int ishift, ring r)
{
  /* modifies p*/
  Print("start: "); Print(" "); p_wrp(*p,r);
  p_LmCheckPolyRing2(*p, r);
  poly q = p_Head(*p,r);
  // in the next line ishift is correct
  while ( ( (*p)!=NULL ) && ( p_Comp_k_n(*p, q, ishift, r) ))
  {
    p_LmDelete(p,r);
    Print("while: ");p_wrp(*p,r);Print(" ");
  }
  p_wrp(*p,r);Print(" ");
  PrintS("end\n"); 
  p_LmDelete(&q,r);
}

/* to test!!! */
/* ExpVector(pr) = ExpVector(p1) - ExpVector(p2) */
void p_ExpVectorDiffRat(poly pr, poly p1, poly p2, int ishift, ring r)
{
  p_LmCheckPolyRing1(p1, r);
  p_LmCheckPolyRing1(p2, r);
  p_LmCheckPolyRing1(pr, r); 
  int i;
  poly t=pr;
  Exponent_t e1,e2;
  for (i=ishift+1 ; i<=r->N; i++)
  {
    e1 = p_GetExp(p1, i, r);
    e2 = p_GetExp(p2, i, r);
    //    pAssume1(p_GetExp(p1, i, r) >= p_GetExp(p2, i, r));
    if (e1 < e2)
    {
#ifdef PDEBUG
      Print("negative ExpVectorDiff\n");
#endif    
      p_Delete(&t,r);
      break;
    }
    else
    {
      p_SetExp(t,i, e1-e2,r);
    }
  }
  p_Setm(t,r);
}




/* returns ideal (u,v) s.t. up + vq = 0 */

ideal ncGCD(poly p, poly q, const ring r)
{
  intvec *w = NULL;
  ideal h = idInit(2,1);
  h->m[0] = p_Copy(p,r);
  h->m[1] = p_Copy(q,r);
#ifdef PDEBUG
  Print("running syzygy comp. for nc_GCD:\n");
#endif
  ideal sh = idSyzygies(h, testHomog, &w);
#ifdef PDEBUG
  Print("done syzygy comp. for nc_GCD\n");
#endif
  /* in comm case, there is only 1 syzygy */
  /*   singclap_gcd(); */
  poly K, K1, K2;
  K  = sh->m[0]; /* take just the first element - to be enhanced later */
  K1 = pTakeOutComp(&K, 1); // 1st component is taken out from K
//  pShift(&K,-2); // 2nd component to 0th comp.
  K2 = pTakeOutComp(&K, 1);
//  K2 = K; 

  Print("syz1: "); p_wrp(K1,r);
  Print("syz2: "); p_wrp(K2,r);

  /* checking signs before multiplying */    
  number ck1 = p_GetCoeff(K1,r);
  number ck2 = p_GetCoeff(K2,r);
  BOOLEAN bck1, bck2;
  bck1 = n_GreaterZero(ck1,r);
  bck2 = n_GreaterZero(ck2,r);
  /* K1 <0, K2 <0 (-K1,-K2)    */
//   if ( !(bck1 && bck2) ) /* - , - */
//   {
//     K1 = p_Neg(K1,r);
//     K2 = p_Neg(K2,r);
//   }
  id_Delete(&h,r);
  h = idInit(2,1);
  h->m[0] = p_Copy(K1,r);
  h->m[1] = p_Copy(K2,r);
  id_Delete(&sh,r);
  return(h);
}



/* PINLINE1 void p_ExpVectorDiff 
   remains as is -> BUT we can do memory shift on smaller number of exp's */


/*4 - follow the numbering of gring.cc
* creates the S-polynomial of p1 and p2
* do not destroy p1 and p2
*/
// poly nc_rat_CreateSpoly(poly p1, poly p2, poly spNoether, int ishift, const ring r)
// {
//   if ((p_GetComp(p1,r)!=p_GetComp(p2,r))
//   && (p_GetComp(p1,r)!=0)
//   && (p_GetComp(p2,r)!=0))
//   {
// #ifdef PDEBUG
//     Print("nc_CreateSpoly : different components!");
// #endif
//     return(NULL);
//   }
//   /* prod. crit does not apply yet */
// //   if ((r->nc->type==nc_lie) && pHasNotCF(p1,p2)) /* prod crit */
// //   {
// //     return(nc_p_Bracket_qq(pCopy(p2),p1));
// //   }
//   poly pL=pOne();
//   poly m1=pOne();
//   poly m2=pOne();
//   /* define shift */
//   int is = ishift; /* TODO */
//   pLcmRat(p1,p2,pL,is);
//   p_Setm(pL,r);
//   poly pr1 = p_GetExp_k_n(p1,1,ishift-1,r); /* rat D-exp of p1 */
//   poly pr2 = p_GetExp_k_n(p2,1,ishift-1,r); /* rat D-exp of p2 */
// #ifdef PDEBUG
//   p_Test(pL,r);
// #endif
//   p_ExpVectorDiff(m1,pL,p1,r); /* purely in D part by construction */
//   //p_SetComp(m1,0,r);
//   //p_Setm(m1,r);
// #ifdef PDEBUG
//   p_Test(m1,r);
// #endif
//   p_ExpVectorDiff(m2,pL,p2,r); /* purely in D part by construction */
//   //p_SetComp(m2,0,r);
//   //p_Setm(m2,r);
// #ifdef PDEBUG
//   p_Test(m2,r);
// #endif
//   p_Delete(&pL,r);
//   /* zero exponents ! */

//   /* EXTRACT LEADCOEF */

//   poly H1  = p_HeadRat(p1,is,r);
//   poly M1  = r->nc->p_Procs.mm_Mult_p(m1,p_Copy(H1,r),r);

//   /* POLY:  number C1  = n_Copy(p_GetCoeff(M1,r),r); */
//   /* RAT: */

//   poly C1  = p_GetCoeffRat(M1,ishift,r);

//   poly H2  = p_HeadRat(p2,is,r);
//   poly M2  = r->nc->p_Procs.mm_Mult_p(m2,p_Copy(H2,r),r);

//   /* POLY:  number C2  = n_Copy(p_GetCoeff(M2,r),r); */
//   /* RAT: */

//   poly C2  = p_GetCoeffRat(M2,ishift,r);

// /* we do not assume that X's commute */
// /* we just run NC syzygies */

// /* NEW IDEA: change the ring to K<X>, map things there
//    and return the result back; seems to be a good optimization */
// /* to be done later */
// /* problem: map to subalgebra. contexts, induced (non-unique) orderings etc. */

//   intvec *w = NULL;
//   ideal h = idInit(2,1);
//   h->m[0] = p_Copy(C1,r);
//   h->m[1] = p_Copy(C2,r);
// #ifdef PDEBUG
//   Print("running syzygy comp. for coeffs");
// #endif
//   ideal sh = idSyzygies(h, testHomog, &w);
//   /* in comm case, there is only 1 syzygy */
//   /*   singclap_gcd(); */
//   poly K,K1,K2;
//   K  = sh->m[0];
//   K1 = pTakeOutComp(&K, 1); // 1st component is taken out from K
//   pShift(&K,-2); // 2nd component to 0th comp.
//   K2 = K;

//   /* checking signs before multiplying */    
//   number ck1 = p_GetCoeff(K1,r);
//   number ck2 = p_GetCoeff(K2,r);
//   BOOLEAN bck1, bck2;
//   bck1 = n_GreaterZero(ck1,r);
//   bck2 = n_GreaterZero(ck2,r);
//   /* K1 >0, K2 >0 (K1,-K2)    */
//   /* K1 >0, K2 <0 (K1,-K2)    */
//   /* K1 <0, K2 >0 (-K1,K2)    */
//   /* K1 <0, K2 <0 (-K1,K2)    */
//   if ( (bck1) && (bck2) ) /* +, + */
//   {
//     K2 = p_Neg(K2,r);
//   }
//   if ( (bck1) && (!bck2) ) /* + , - */
//   {
//     K2 = p_Neg(K2,r);
//   }
//   if ( (!bck1) && (bck2) ) /* - , + */
//   {
//     K1 = p_Neg(K1,r);
//   }
//   if ( !(bck1 && bck2) ) /* - , - */
//   {
//     K1 = p_Neg(K1,r);
//   }

//   poly P1,P2;

//   //  p_LmDeleteRat(M1,ishift,r); // get tail(D^(gamma-alpha) * lm(p1)) = h_f
//   P1 = p_Copy(p1,r);
//   p_LmDeleteAndNextRat(P1,ishift,r); // get tail(p1) = t_f
//   P1 = r->nc->p_Procs.mm_Mult_p(m1,P1,r);
//   P1 = p_Add_q(P1,M1,r);

//   //  p_LmDeleteRat(M2,ishift,r);
//   P2 = p_Copy(p2,r);
//   p_LmDeleteAndNextRat(P2,ishift,r);// get tail(p2)=t_g
//   P2 = r->nc->p_Procs.mm_Mult_p(m2,P2,r);
//   P2 = p_Add_q(P2,M2,r);

//   /* coeff business */

//   P1 = p_Mult_q(P1,K1,r);
//   P2 = p_Mult_q(P2,K2,r);
//   P1 = p_Add_q(P1,P2,r);

//   /* cleaning up */

// #ifdef PDEBUG
//   p_Test(p1,r);
// #endif
//   /* questionable: */
//   if (P1!=NULL) pCleardenom(P1);
//   if (P1!=NULL) pContent(P1);
//   return(P1);
// }

/*2
* reduction of p2 with p1
* do not destroy p1, but p2
* p1 divides p2 -> for use in NF algorithm
* works in an integer fashion
*/

poly nc_rat_ReduceSpolyNew(const poly p1, poly p2, int ishift, const ring r)
{
  const long lCompP1 = p_GetComp(p1,r);
  const long lCompP2 = p_GetComp(p2,r);

  if ((lCompP1!=lCompP2) && (lCompP1!=0) && (lCompP2!=0))
  {
#ifdef PDEBUG
    Werror("nc_rat_ReduceSpolyNew: different non-zero components!");
#endif
    return(NULL);
  }

  int is = ishift; /* TODO */

  poly m = pOne();
  p_ExpVectorDiffRat(m, p2, p1, ishift, r); // includes X and D parts
  //p_Setm(m,r);
  //  m = p_GetExp_k_n(m,1,ishift,r); /* rat D-exp of m */
#ifdef PDEBUG
  p_Test(m,r);
#endif

  /* pSetComp(m,r)=0? */
  poly HH = NULL;
  poly H  = NULL;
  HH = p_Copy(p_HeadRat(p1,is,r),r); // lm_D(g)
  H  = r->nc->p_Procs.mm_Mult_p(m, p_Copy(HH, r), r); // d^aplha lm_D(g)

  poly K  = p_Copy( p_GetCoeffRat(H,  is, r), r);
  Print("k: "); p_wrp(K,r); PrintS("\n");
  poly P  = p_Copy( p_GetCoeffRat(p2, is, r), r);
  Print("p: "); p_wrp(P,r); PrintS("\n");

  //  HH = p_Neg(HH, r);
  //  poly out = NULL;
  //  out = p_Add_q(p_Copy(p1,r), HH, r); // out == t_g

  Print("f: "); p_wrp(p2,r); PrintS("\n");
  Print("g: "); p_wrp(p1,r); PrintS("\n");

  // alt:
  poly out = p1; //p_Copy(p1,r);
  p_LmDeleteAndNextRat(&out, is+1, r); // out == t_g

  Print("t_g: "); p_wrp(out,r);

  ideal ncsyz = ncGCD(P,K,r);
  poly KK = p_Copy(ncsyz->m[0],r); // k'
  poly PP = p_Copy(ncsyz->m[1],r); // p'
  
//    HH = p_Copy(p_HeadRat(p2,is,r),r);
//    HH = p_Neg(HH, r);
//    p2 = p_Add_q(p2, HH, r); // t_f

  // alt:
  p_LmDeleteAndNextRat(&p2, is+1, r); // t_f

  Print("t_f: "); p_wrp(p2,r);

//    HH = p_Copy(p_HeadRat(H,is,r),r);
//    HH = p_Neg(HH, r);
//    H  = p_Add_q(H, HH, r); // r_g

  // alt:
  p_LmDeleteAndNextRat(&H, is+1, r); // r_g

  Print("r_g: "); p_wrp(H,r);

  p2 = p_Mult_q(KK, p2, r); // p2 = k' t_f
  p_Test(p2,r);
//  p_Delete(&KK,r);

  Print("k' t_f: "); p_wrp(p2,r);

  out = r->nc->p_Procs.mm_Mult_p(m, out, r); // d^aplha t_g
  p_Delete(&m,r);

  Print("d^a t_g: "); p_wrp(out,r);
  PrintS(" end reduction");
  out = p_Add_q(H, out, r);
  p_Test(out,r);
  out  = p_Mult_q(PP, out, r);

  out = p_Add_q(p2,out,r); // delete out, p2
  p_Test(out,r);
  if ( out!=NULL ) pContent(out);
  return(out);
}

// return: FALSE, if there exists i in ishift..r->N,
//                 such that a->exp[i] > b->exp[i]
//         TRUE, otherwise

BOOLEAN p_DivisibleByRat(poly a, poly b, int ishift, const ring r)
{
  int i;
  for(i=r->N;i>ishift;i--)
  {
    if (p_GetExp(a,i,r) > p_GetExp(b,i,r)) return FALSE;
  }
  return ((p_GetComp(a,r)==p_GetComp(b,r)) || (p_GetComp(a,r)==0));
}
/*2
*reduces h with elements from reducer choosing the best possible
* element in t with respect to the given red_length
* arrays reducer and red_length are [0..(rl-1)]
*/
int redRat (poly* h, poly *reducer, int *red_length, int rl, int ishift, ring r)
{
  if ((*h)==NULL) return 0;

  int j,i,l;

  loop
  {
    j=rl;l=MAX_INT_VAL;
    for(i=rl-1;i>=0;i--)
    {
      if ((l>red_length[i]) && (p_DivisibleByRat(reducer[i],*h,ishift,r)))
      {
        j=i; l=red_length[i];
      }
    }
    if (j >=rl)
    {
      return 1; // not reducible
    }

    if (TEST_OPT_DEBUG)
    {
      PrintS("reduce ");
      p_wrp(*h,r);
      PrintS(" with ");
      p_wrp(reducer[j],r);
    }
    poly hh=nc_rat_ReduceSpolyNew(reducer[j], *h, ishift, r);
    //    p_Delete(h,r); 
    *h=hh;
    if (TEST_OPT_DEBUG)
    {
      PrintS(" to ");
      p_wrp(*h,r);
      PrintLn();
    }
    if ((*h)==NULL)
    {
      return 0;
    }
  }
}
#endif

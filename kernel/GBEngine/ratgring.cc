/****************************************
*  Computer Algebra System SINGULAR     *
****************************************/
/***************************************************************
 *  File:    ratgring.cc
 *  Purpose: Ore-noncommutative kernel procedures
 *  Author:  levandov (Viktor Levandovsky)
 *  Created: 8/00 - 11/00
 *******************************************************************/



#include "kernel/mod2.h"
#ifdef HAVE_RATGRING
#include "kernel/GBEngine/ratgring.h"
#include "polys/nc/nc.h"
#include "polys/monomials/ring.h"
#include "kernel/polys.h"
#include "coeffs/numbers.h"
#include "kernel/ideals.h"
#include "polys/matpol.h"
#include "polys/kbuckets.h"
#include "kernel/GBEngine/kstd1.h"
#include "polys/sbuckets.h"
#include "polys/prCopy.h"
#include "polys/operations/p_Mult_q.h"
#include "polys/clapsing.h"
#include "misc/options.h"

void pLcmRat(poly a, poly b, poly m, int rat_shift)
{
  /* rat_shift is the last exp one should count with */
  int i;
  for (i=(currRing->N); i>=rat_shift; i--)
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
//   for (i=(currRing->N); i; i--)
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
  const long cmp = p_GetComp(p, r);
  while ( (q!=NULL) && (p_Comp_k_n(p, q, ishift+1, r)) && (p_GetComp(q, r) == cmp) )
  {
    res = p_Add_q(res,p_Head(q,r),r);
    q   = pNext(q);
  }
  p_SetCompP(res,cmp,r);
  return res;
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
  int e1,e2;
  for (i=ishift+1; i<=r->N; i++)
  {
    e1 = p_GetExp(p1, i, r);
    e2 = p_GetExp(p2, i, r);
    //    pAssume1(p_GetExp(p1, i, r) >= p_GetExp(p2, i, r));
    if (e1 < e2)
    {
#ifdef PDEBUG
      PrintS("negative ExpVectorDiff\n");
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

ideal ncGCD2(poly p, poly q, const ring r)
{
  // todo: must destroy p,q
  intvec *w = NULL;
  ideal h = idInit(2,1);
  h->m[0] = p_Copy(p,r);
  h->m[1] = p_Copy(q,r);
#ifdef PDEBUG
  PrintS("running syzygy comp. for nc_GCD:\n");
#endif
  ideal sh = idSyzygies(h, testHomog, &w);
#ifdef PDEBUG
  PrintS("done syzygy comp. for nc_GCD\n");
#endif
  /* in comm case, there is only 1 syzygy */
  /*   singclap_gcd(); */
  poly K, K1, K2;
  K  = sh->m[0]; /* take just the first element - to be enhanced later */
  K1 = pTakeOutComp(&K, 1); // 1st component is taken out from K
//  pShift(&K,-2); // 2nd component to 0th comp.
  K2 = pTakeOutComp(&K, 1);
//  K2 = K;

  PrintS("syz1: "); p_wrp(K1,r);
  PrintS("syz2: "); p_wrp(K2,r);

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

/* returns ideal (u,v) s.t. up + vq = 0 */

ideal ncGCD(poly p, poly q, const ring r)
{
  // destroys p and q
  // assume: p,q are in the comm. ring
  // to be used in the coeff business
#ifdef PDEBUG
  PrintS(" GCD_start:");
#endif
  poly g = singclap_gcd(p_Copy(p,r),p_Copy(q,r), r);
#ifdef PDEBUG
  p_wrp(g,r);
  PrintS(" GCD_end;\n");
#endif
  poly u = singclap_pdivide(q, g, r); //q/g
  poly v = singclap_pdivide(p, g, r); //p/g
  v = p_Neg(v,r);
  p_Delete(&p,r);
  p_Delete(&q,r);
  ideal h = idInit(2,1);
  h->m[0] = u; // p_Copy(u,r);
  h->m[1] = v; // p_Copy(v,r);
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
//   return(P1);
// }

#undef CC

/*4 - follow the numbering of gring.cc
* creates the S-polynomial of p1 and p2
* do not destroy p1 and p2
*/
poly nc_rat_CreateSpoly(poly pp1, poly pp2, int ishift, const ring r)
{

  poly p1 = p_Copy(pp1,r);
  poly p2 = p_Copy(pp2,r);

  const long lCompP1 = p_GetComp(p1,r);
  const long lCompP2 = p_GetComp(p2,r);

  if ((lCompP1!=lCompP2) && (lCompP1!=0) && (lCompP2!=0))
  {
#ifdef PDEBUG
    WerrorS("nc_rat_CreateSpoly: different non-zero components!");
#endif
    return(NULL);
  }

  if ( (p_LmIsConstantRat(p1,r)) || (p_LmIsConstantRat(p2,r)) )
  {
    p_Delete(&p1,r);
    p_Delete(&p2,r);
    return( NULL );
  }


/* note: prod. crit does not apply! */
  poly pL=pOne();
  poly m1=pOne();
  poly m2=pOne();
  int is = ishift; /* TODO */
  pLcmRat(p1,p2,pL,is);
  p_Setm(pL,r);
#ifdef PDEBUG
  p_Test(pL,r);
#endif
  poly pr1 = p_GetExp_k_n(p1,1,ishift,r); /* rat D-exp of p1 */
  poly pr2 = p_GetExp_k_n(p2,1,ishift,r); /* rat D-exp of p2 */
  p_ExpVectorDiff(m1,pL,pr1,r); /* purely in D part by construction */
  p_ExpVectorDiff(m2,pL,pr2,r); /* purely in D part by construction */
  p_Delete(&pr1,r);
  p_Delete(&pr2,r);
  p_Delete(&pL,r);
#ifdef PDEBUG
  p_Test(m1,r);
  PrintS("d^{gamma-alpha} = "); p_wrp(m1,r); PrintLn();
  p_Test(m2,r);
  PrintS("d^{gamma-beta} = "); p_wrp(m2,r); PrintLn();
#endif

  poly HF = NULL;
  HF = p_HeadRat(p1,is,r); // lm_D(f)
  HF  = nc_mm_Mult_p(m1, HF, r); // // d^{gamma-alpha} lm_D(f)
  poly C  = p_GetCoeffRat(HF,  is, r); // c = lc_D(h_f) in the paper

  poly HG = NULL;
  HG = p_HeadRat(p2,is,r); // lm_D(g)
  HG  = nc_mm_Mult_p(m2, HG, r); // // d^{gamma-beta} lm_D(g)
  poly K  = p_GetCoeffRat(HG,  is, r); // k = lc_D(h_g) in the paper

#ifdef PDEBUG
  PrintS("f: "); p_wrp(p1,r); PrintS("\n");
  PrintS("c: "); p_wrp(C,r); PrintS("\n");
  PrintS("g: "); p_wrp(p2,r); PrintS("\n");
  PrintS("k: "); p_wrp(K,r); PrintS("\n");
#endif

  ideal ncsyz = ncGCD(C,K,r);
  poly KK = ncsyz->m[0]; ncsyz->m[0]=NULL; //p_Copy(ncsyz->m[0],r); // k'
  poly CC = ncsyz->m[1]; ncsyz->m[1]= NULL; //p_Copy(ncsyz->m[1],r); // c'
  id_Delete(&ncsyz,r);

  p_LmDeleteAndNextRat(&p1, is, r); // t_f
  p_LmDeleteAndNextRat(&HF, is, r); // r_f = h_f - lt_D(h_f)

  p_LmDeleteAndNextRat(&p2, is, r); // t_g
  p_LmDeleteAndNextRat(&HG, is, r); // r_g = h_g - lt_D(h_g)


#ifdef PDEBUG
  PrintS(" t_f: "); p_wrp(p1,r); PrintS("\n");
  PrintS(" t_g: "); p_wrp(p2,r); PrintS("\n");
  PrintS(" r_f: "); p_wrp(HF,r); PrintS("\n");
  PrintS(" r_g: "); p_wrp(HG,r); PrintS("\n");
  PrintS(" c': "); p_wrp(CC,r); PrintS("\n");
  PrintS(" k': "); p_wrp(KK,r); PrintS("\n");

#endif

  // k'(r_f + d^{gamma-alpha} t_f)

  p1 = p_Mult_q(m1, p1, r); // p1 = d^{gamma-alpha} t_f
  p1 = p_Add_q(p1,HF,r); // p1 = r_f + d^{gamma-alpha} t_f
  p1 = p_Mult_q(KK,p1,r); // p1 = k'(r_f + d^{gamma-alpha} t_f)

  // c'(r_f + d^{gamma-beta} t_g)

  p2 = p_Mult_q(m2, p2, r); // p2 = d^{gamma-beta} t_g
  p2 = p_Add_q(p2,HG,r); // p2 = r_g + d^{gamma-beta} t_g
  p2 = p_Mult_q(CC,p2,r); // p2 = c'(r_g + d^{gamma-beta} t_g)

#ifdef PDEBUG
  p_Test(p1,r);
  p_Test(p2,r);
  PrintS(" k'(r_f + d^{gamma-alpha} t_f): "); p_wrp(p1,r);
  PrintS(" c'(r_g + d^{gamma-beta} t_g): "); p_wrp(p2,r);
#endif

  poly out = p_Add_q(p1,p2,r); // delete p1, p2; // the sum

#ifdef PDEBUG
  p_Test(out,r);
#endif

  //  if ( out!=NULL ) pCleardenom(out); // postponed to enterS
  return(out);
}


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
    WerrorS("nc_rat_ReduceSpolyNew: different non-zero components!");
#endif
    return(NULL);
  }

  if (p_LmIsConstantRat(p1,r))
  {
    return( NULL );
  }


  int is = ishift; /* TODO */

  poly m = pOne();
  p_ExpVectorDiffRat(m, p2, p1, ishift, r); // includes X and D parts
  //p_Setm(m,r);
  //  m = p_GetExp_k_n(m,1,ishift,r); /* rat D-exp of m */
#ifdef PDEBUG
  p_Test(m,r);
  PrintS("d^alpha = "); p_wrp(m,r); PrintLn();
#endif

  /* pSetComp(m,r)=0? */
  poly HH = NULL;
  poly H  = NULL;
  HH = p_HeadRat(p1,is,r); //p_Copy(p_HeadRat(p1,is,r),r); // lm_D(g)
//  H  = r->nc->p_Procs.mm_Mult_p(m, p_Copy(HH, r), r); // d^alpha lm_D(g)
  H  = nc_mm_Mult_p(m, HH, r); // d^alpha lm_D(g) == h_g in the paper

  poly K  = p_GetCoeffRat(H,  is, r); //p_Copy( p_GetCoeffRat(H,  is, r), r); // k in the paper
  poly P  = p_GetCoeffRat(p2, is, r); //p_Copy( p_GetCoeffRat(p2, is, r), r); // lc_D(p_2) == lc_D(f)

#ifdef PDEBUG
  PrintS("k: "); p_wrp(K,r); PrintS("\n");
  PrintS("p: "); p_wrp(P,r); PrintS("\n");
  PrintS("f: "); p_wrp(p2,r); PrintS("\n");
  PrintS("g: "); p_wrp(p1,r); PrintS("\n");
#endif
  // alt:
  poly out = p_Copy(p1,r);
  p_LmDeleteAndNextRat(&out, is, r); // out == t_g

  ideal ncsyz = ncGCD(P,K,r);
  poly KK = ncsyz->m[0]; ncsyz->m[0]=NULL; //p_Copy(ncsyz->m[0],r); // k'
  poly PP = ncsyz->m[1]; ncsyz->m[1]= NULL; //p_Copy(ncsyz->m[1],r); // p'

#ifdef PDEBUG
  PrintS("t_g: "); p_wrp(out,r);
  PrintS("k': "); p_wrp(KK,r); PrintS("\n");
  PrintS("p': "); p_wrp(PP,r); PrintS("\n");
#endif
  id_Delete(&ncsyz,r);
  p_LmDeleteAndNextRat(&p2, is, r); // t_f
  p_LmDeleteAndNextRat(&H, is, r); // r_g = h_g - lt_D(h_g)

#ifdef PDEBUG
  PrintS(" t_f: "); p_wrp(p2,r);
  PrintS(" r_g: "); p_wrp(H,r);
#endif

  p2 = p_Mult_q(KK, p2, r); // p2 = k' t_f

#ifdef PDEBUG
  p_Test(p2,r);
  PrintS(" k' t_f: "); p_wrp(p2,r);
#endif

//  out = r->nc->p_Procs.mm_Mult_p(m, out, r); // d^alpha t_g
  out = nc_mm_Mult_p(m, out, r); // d^alpha t_g
  p_Delete(&m,r);

#ifdef PDEBUG
  PrintS(" d^a t_g: "); p_wrp(out,r);
  PrintS(" end reduction\n");
#endif

  out = p_Add_q(H, out, r); // r_g + d^a t_g

#ifdef PDEBUG
  p_Test(out,r);
#endif
  out = p_Mult_q(PP, out, r); // p' (r_g + d^a t_g)
  out = p_Add_q(p2,out,r); // delete out, p2; // the sum

#ifdef PDEBUG
  p_Test(out,r);
#endif

  //  if ( out!=NULL ) pCleardenom(out); // postponed to enterS
  return(out);
}

// return: FALSE, if there exists i in ishift..r->N,
//                 such that a->exp[i] > b->exp[i]
//         TRUE, otherwise

BOOLEAN p_DivisibleByRat(poly a, poly b, int ishift, const ring r)
{
#ifdef PDEBUG
  PrintS("invoke p_DivByRat with a = ");
  p_wrp(p_Head(a,r),r);
  PrintS(" and b= ");
  p_wrp(p_Head(b,r),r);
  PrintLn();
#endif
  int i;
  for(i=r->N; i>ishift; i--)
  {
#ifdef PDEBUG
    Print("i=%d,",i);
#endif
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
      //      Print("test %d, l=%d (curr=%d, l=%d\n",i,red_length[i],j,l);
      if ((l>red_length[i]) && (p_DivisibleByRat(reducer[i],*h,ishift,r)))
      {
        j=i; l=red_length[i];
        //        PrintS(" yes\n");
      }
      //      else PrintS(" no\n");
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

// test if monomial is a constant, i.e. if all exponents and the component
// is zero
BOOLEAN p_LmIsConstantRat(const poly p, const ring r)
{
  if (p_LmIsConstantCompRat(p, r))
    return (p_GetComp(p, r) == 0);
  return FALSE;
}

// test if the monomial is a constant as a vector component
// i.e., test if all exponents are zero
BOOLEAN p_LmIsConstantCompRat(const poly p, const ring r)
{
  int i = r->real_var_end;

  while ( (p_GetExp(p,i,r)==0) && (i>=r->real_var_start))
  {
    i--;
  }
  return ( i+1 == r->real_var_start );
}

#endif

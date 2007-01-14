/****************************************
*  Computer Algebra System SINGULAR     *
****************************************/
/***************************************************************
 *  File:    ratgring.cc
 *  Purpose: Ore-noncommutative kernel procedures
 *  Author:  levandov (Viktor Levandovsky)
 *  Created: 8/00 - 11/00
 *  Version: $Id: ratgring.cc,v 1.1 2007-01-14 22:12:33 levandov Exp $
 *******************************************************************/
#include "mod2.h"
#ifdef HAVE_PLURAL
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

void pLcmRat(poly a, poly b, poly m, poly pshift)
{
  /* shift is the exp of rational elements */
  int i;
  for (i=pVariables; i; i--)
  {
    if (!pGetExp(pshift,i))
    {
      pSetExp(m,i, si_max( pGetExp(a,i), pGetExp(b,i)));
    }
    else
    {
      /* do we really need it? */
      pSetExp(m,i,0);
    }
  }
  pSetComp(m, si_max(pGetComp(a), pGetComp(b)));
  /* Don't do a pSetm here, otherwise hres/lres chockes */ 
}

/* returns a subpoly of p, s.t. its monomials have the same D-part */

poly p_HeadRat(poly p, int ishift, ring r)
{
  poly q   = p_Next(p,r);
  poly res = p_Head(p,r);
  while ( p_Comp_k_n(p, q, ishift, r) )
  {
    res = p_Add_q(res,p_Head(q,r));
    q   = p_Next(p,r);
  }
  return res;
}

/* returns x-coeff of p, i.e. a poly in x, s.t. corresponding xd-monomials 
have the same D-part */

poly p_GetCoeffRat(poly p, int ishift, ring r)
{
  poly q   = p_Next(p,r);
  poly res = p_Head(p,r);
  poly s;
  while ( p_Comp_k_n(p, q, ishift, r) )
  {
    s   = p_GetExp_k_n(q, ishift, r->N, r);
    res = p_Add_q(res,s);
    q   = p_Next(p,r);
  }
  return res;
}

poly p_LmDeleteAndNextRat(poly p, int ishift, ring r)
{
  p_LmCheckPolyRing2(p, r);
  while ( p_Comp_k_n(p, p_Next(p,r), ishift, r) )
  {
    p_LmDelete(&p,r);
  }
}

/* PINLINE1 void p_ExpVectorDiff 
   remains as is -> BUT we can do memory shift on smaller number of exp's */


/*4 - follow the numbering of gring.cc
* creates the S-polynomial of p1 and p2
* do not destroy p1 and p2
*/
poly nc_rat_CreateSpoly(poly p1, poly p2, poly spNoether, int ishift, const ring r)
{
  if ((p_GetComp(p1,r)!=p_GetComp(p2,r))
  && (p_GetComp(p1,r)!=0)
  && (p_GetComp(p2,r)!=0))
  {
#ifdef PDEBUG
    Print("nc_CreateSpoly : different components!");
#endif
    return(NULL);
  }
  /* prod. crit does not apply yet */
//   if ((r->nc->type==nc_lie) && pHasNotCF(p1,p2)) /* prod crit */
//   {
//     return(nc_p_Bracket_qq(pCopy(p2),p1));
//   }
  poly pL=pOne();
  poly m1=pOne();
  poly m2=pOne();
  /* define shift */
  int is = ishift; /* TODO */
  pLcmRat(p1,p2,pL,is);
  p_Setm(pL,r);
  poly pr1 = p_GetExp_k_n(p1,1,ishift-1); /* rat D-exp of p1 */
  poly pr2 = p_GetExp_k_n(p2,1,ishift-1); /* rat D-exp of p2 */
#ifdef PDEBUG
  p_Test(pL,r);
#endif
  p_ExpVectorDiff(m1,pL,p1,r); /* purely in D part by construction */
  //p_SetComp(m1,0,r);
  //p_Setm(m1,r);
#ifdef PDEBUG
  p_Test(m1,r);
#endif
  p_ExpVectorDiff(m2,pL,p2,r); /* purely in D part by construction */
  //p_SetComp(m2,0,r);
  //p_Setm(m2,r);
#ifdef PDEBUG
  p_Test(m2,r);
#endif
  p_Delete(&pL,r);
  /* zero exponents ! */

  /* EXTRACT LEADCOEF */

  poly H1  = p_HeadRat(p1,r);
  poly M1  = nc_mm_Mult_p(m1,p_Copy(H1,r),r);

  /* POLY:  number C1  = n_Copy(p_GetCoeff(M1,r),r); */
  /* RAT: */

  poly C1  = p_GetCoeffRat(M1,ishift,r);

  poly H2  = p_HeadRat(p2,r);
  poly M2  = nc_mm_Mult_p(m2,p_Copy(H2,r),r);

  /* POLY:  number C2  = n_Copy(p_GetCoeff(M2,r),r); */
  /* RAT: */

  poly C2  = p_GetCoeffRat(M2,ishift,r);

/* we do not assume that X's commute */
/* we just run NC syzygies */

/* NEW IDEA: change the ring to K<X>, map things there
   and return the result back; seems to be a good optimization */
/* to be done later */

  intvec *w = NULL;
  ideal h = idInit(2,1);
  h->m[0] = p_Copy(C1,r);
  h->m[1] = p_Copy(C2,r);
#ifdef PDEBUG
  Print("running syzygy comp. for coeffs");
#endif
  ideal sh = idSyzygies(h, testHomog, &w);
  /* in comm case, there is only 1 syzygy */
  /*   singclap_gcd(); */
  poly K,K1,K2;
  K  = sh->m[0];
  K1 = pTakeOutComp(&K, 1); // 1st component is taken out from K
  K2 = pShift(&K,-2); // 2nd component to 0th comp.

  /* checking signs before multiplying */    
  number ck1 = p_GetCoeff(K1,r);
  number ck2 = p_GetCoeff(K2,r);
  BOOLEAN bck1, bck2;
  bck1 = n_GreaterZero(ck1);
  bck2 = n_GreaterZero(ck2);
  /* K1 >0, K2 >0 (K1,-K2)    */
  /* K1 >0, K2 <0 (K1,-K2)    */
  /* K1 <0, K2 >0 (-K1,K2)    */
  /* K1 <0, K2 <0 (-K1,K2)    */
  if ( (bck1) && (bck2) ) /* +, + */
  {
    K2 = p_Neg(K2,r);
  }
  if ( (bck1) && (!bck2) ) /* + , - */
  {
    K2 = p_Neg(K2,r);
  }
  if ( (!bck1) && (bck2) ) /* - , + */
  {
    K1 = p_Neg(K1,r);
  }
  if ( !(bck1 && bck2) ) /* - , - */
  {
    K1 = p_Neg(K1,r);
  }

  poly P1,P2;

  p_LmDeleteRat(M1,ishift,r); // get tail(D^(gamma-alpha) * lm(p1)) = h_f
  P1 = p_LmDeleteAndNextRat(p_Copy(p1,r),ishift,r); // get tail(p1) = t_f
  P1 = nc_mm_Mult_p(m1,P1,r);
  P1 = p_Add_q(P1,M1,r);

  p_LmDeleteRat(M2,ishift,r);
  P2 = p_LmDeleteRat(p_Copy(p2,r),ishift,r);// get tail(p2)=t_g
  P2 = nc_mm_Mult_p(m2,P2,r);
  P2 = p_Add_q(P2,M2,r);

  /* coeff business */

  P1 = p_Mult_nn(P1,K1,r);
  P2 = p_Mult_nn(P2,K2,r);
  P1 = p_Add_q(P1,P2,r);

  /* cleaning up */

#ifdef PDEBUG
  p_Test(p1,r);
#endif
  /* questionable: */
  if (P1!=NULL) pCleardenom(P1);
  if (P1!=NULL) pContent(P1);
  return(P1);
}

#endif

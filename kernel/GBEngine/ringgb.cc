/****************************************
*  Computer Algebra System SINGULAR     *
****************************************/
/*
* ABSTRACT: ringgb interface
*/
//#define HAVE_TAIL_RING
#define NO_BUCKETS

#include "kernel/mod2.h"
#include "kernel/GBEngine/kutil.h"
#include "kernel/structs.h"
#include "kernel/polys.h"
#include "polys/monomials/p_polys.h"
#include "kernel/ideals.h"
#include "kernel/GBEngine/kstd1.h"
#include "kernel/GBEngine/khstd.h"
#include "polys/kbuckets.h"
#include "polys/weight.h"
#include "misc/intvec.h"
#include "kernel/polys.h"
#ifdef HAVE_PLURAL
#include "polys/nc/nc.h"
#endif

#include "kernel/GBEngine/ringgb.h"

poly reduce_poly_fct(poly p, ring r)
{
   return kFindZeroPoly(p, r, r);
}

/*
 * Returns maximal k, such that
 * 2^k | n
 */
int indexOf2(number n)
{
  long test = (long) n;
  int i = 0;
  while (test%2 == 0)
  {
    i++;
    test = test / 2;
  }
  return i;
}

/***************************************************************
 *
 * Lcm business
 *
 ***************************************************************/
// get m1 = LCM(LM(p1), LM(p2))/LM(p1)
//     m2 = LCM(LM(p1), LM(p2))/LM(p2)
BOOLEAN ring2toM_GetLeadTerms(const poly p1, const poly p2, const ring p_r,
                               poly &m1, poly &m2, const ring m_r)
{
  int i;
  int x;
  m1 = p_Init(m_r);
  m2 = p_Init(m_r);

  for (i = p_r->N; i; i--)
  {
    x = p_GetExpDiff(p1, p2, i, p_r);
    if (x > 0)
    {
      p_SetExp(m2,i,x, m_r);
      p_SetExp(m1,i,0, m_r);
    }
    else
    {
      p_SetExp(m1,i,-x, m_r);
      p_SetExp(m2,i,0, m_r);
    }
  }
  p_Setm(m1, m_r);
  p_Setm(m2, m_r);
  long cp1 = (long) pGetCoeff(p1);
  long cp2 = (long) pGetCoeff(p2);
  if (cp1 != 0 && cp2 != 0)
  {
    while (cp1%2 == 0 && cp2%2 == 0)
    {
      cp1 = cp1 / 2;
      cp2 = cp2 / 2;
    }
  }
  p_SetCoeff(m1, (number) cp2, m_r);
  p_SetCoeff(m2, (number) cp1, m_r);
  return TRUE;
}

void printPolyMsg(const char * start, poly f, const char * end)
{
  PrintS(start);
  wrp(f);
  PrintS(end);
}

poly spolyRing2toM(poly f, poly g, ring r)
{
  poly m1 = NULL;
  poly m2 = NULL;
  ring2toM_GetLeadTerms(f, g, r, m1, m2, r);
  // printPolyMsg("spoly: m1=", m1, " | ");
  // printPolyMsg("m2=", m2, "");
  // PrintLn();
  poly sp = pSub(p_Mult_mm(f, m1, r), pp_Mult_mm(g, m2, r));
  pDelete(&m1);
  pDelete(&m2);
  return(sp);
}

poly ringRedNF (poly f, ideal G, ring r)
{
  // If f = 0, then normal form is also 0
  if (f == NULL) { return NULL; }
  poly h = NULL;
  poly g = pCopy(f);
  int c = 0;
  while (g != NULL)
  {
    Print("%d-step RedNF - g=", c);
    wrp(g);
    PrintS(" | h=");
    wrp(h);
    PrintLn();
    g = ringNF(g, G, r);
    if (g != NULL) {
      h = pAdd(h, pHead(g));
      pLmDelete(&g);
    }
    c++;
  }
  return h;
}

/*
 * Find an index i from G, such that
 * LT(rside) = x * LT(G[i]) has a solution
 * or -1 if rside is not in the
 * ideal of the leading coefficients
 * of the suitable g from G.
 */
int findRingSolver(poly rside, ideal G, ring r)
{
  if (rside == NULL) return -1;
  int i;
//  int iO2rside = indexOf2(pGetCoeff(rside));
  for (i = 0; i < IDELEMS(G); i++)
  {
    if // (indexOf2(pGetCoeff(G->m[i])) <= iO2rside &&    / should not be necessary any more
       (p_LmDivisibleBy(G->m[i], rside, r))
    {
      return i;
    }
  }
  return -1;
}

poly plain_spoly(poly f, poly g)
{
  number cf = nCopy(pGetCoeff(f)), cg = nCopy(pGetCoeff(g));
  (void)ksCheckCoeff(&cf, &cg, currRing->cf); // gcd and zero divisors
  poly fm, gm;
  k_GetLeadTerms(f, g, currRing, fm, gm, currRing);
  pSetCoeff0(fm, cg);
  pSetCoeff0(gm, cf);  // and now, m1 * LT(p1) == m2 * LT(p2)
  poly sp = pSub(ppMult_mm(f, fm), ppMult_mm(g, gm));
  pDelete(&fm);
  pDelete(&gm);
  return(sp);
}

/*2
* Generates spoly(0, h) if applicable. Assumes ring has zero divisors
*/
poly plain_zero_spoly(poly h)
{
  poly p = NULL;
  number zero=n_Init(0,currRing->cf);
  number gcd = n_Gcd(zero, pGetCoeff(h), currRing->cf);
  if (!n_IsOne( gcd,  currRing->cf ))
  {
    number tmp=n_Ann(gcd,currRing->cf);
    p = p_Copy(h->next, currRing);
    p = __p_Mult_nn(p, tmp, currRing);
    n_Delete(&tmp,currRing->cf);
  }
   n_Delete(&zero,currRing->cf);
  return p;
}

poly ringNF(poly f, ideal G, ring r)
{
  // If f = 0, then normal form is also 0
  if (f == NULL) { return NULL; }
  poly tmp = NULL;
  poly h = pCopy(f);
  int i = findRingSolver(h, G, r);
  int c = 1;
  while (h != NULL && i >= 0) {
//    Print("%d-step NF - h:", c);
//    wrp(h);
//    PrintS(" ");
//    PrintS("G->m[i]:");
//    wrp(G->m[i]);
//    PrintLn();
    tmp = h;
    h = plain_spoly(h, G->m[i]);
    pDelete(&tmp);
//    PrintS("=> h=");
//    wrp(h);
//    PrintLn();
    i = findRingSolver(h, G, r);
    c++;
  }
  return h;
}

int testGB(ideal I, ideal GI) {
  poly f, g, h, nf;
  int i = 0;
  int j = 0;
  PrintS("I included?");
  for (i = 0; i < IDELEMS(I); i++) {
    if (ringNF(I->m[i], GI, currRing) != NULL) {
      PrintS("Not reduced to zero from I: ");
      wrp(I->m[i]);
      PrintS(" --> ");
      wrp(ringNF(I->m[i], GI, currRing));
      PrintLn();
      return(0);
    }
    PrintS("-");
  }
  PrintS(" Yes!\nspoly --> 0?");
  for (i = 0; i < IDELEMS(GI); i++)
  {
    for (j = i + 1; j < IDELEMS(GI); j++)
    {
      f = pCopy(GI->m[i]);
      g = pCopy(GI->m[j]);
      h = plain_spoly(f, g);
      nf = ringNF(h, GI, currRing);
      if (nf != NULL)
      {
        PrintS("spoly(");
        wrp(GI->m[i]);
        PrintS(", ");
        wrp(GI->m[j]);
        PrintS(") = ");
        wrp(h);
        PrintS(" --> ");
        wrp(nf);
        PrintLn();
        return(0);
      }
      pDelete(&f);
      pDelete(&g);
      pDelete(&h);
      pDelete(&nf);
      PrintS("-");
    }
  }
  if (!(rField_is_Domain(currRing)))
  {
    PrintS(" Yes!\nzero-spoly --> 0?");
    for (i = 0; i < IDELEMS(GI); i++)
    {
      f = plain_zero_spoly(GI->m[i]);
      nf = ringNF(f, GI, currRing);
      if (nf != NULL) {
        PrintS("spoly(");
        wrp(GI->m[i]);
        PrintS(", ");
        wrp(0);
        PrintS(") = ");
        wrp(h);
        PrintS(" --> ");
        wrp(nf);
        PrintLn();
        return(0);
      }
      pDelete(&f);
      pDelete(&nf);
      PrintS("-");
    }
  }
  PrintS(" Yes!");
  PrintLn();
  return(1);
}

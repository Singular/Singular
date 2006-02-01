/****************************************
*  Computer Algebra System SINGULAR     *
****************************************/
/* $Id: ringgb.cc,v 1.5 2006-02-01 13:49:35 Singular Exp $ */
/*
* ABSTRACT: ringgb interface
*/
//#define HAVE_TAIL_RING
#define NO_BUCKETS

#include "mod2.h"
#include "kutil.h"
#include "structs.h"
#include "omalloc.h"
#include "polys.h"
#include "p_polys.h"
#include "ideals.h"
#include "febase.h"
#include "kstd1.h"
#include "khstd.h"
#include "kbuckets.h"
#include "weight.h"
#include "intvec.h"
#include "pInline1.h"
#ifdef HAVE_PLURAL
#include "gring.h"
#endif

#include "ringgb.h"

poly reduce_poly_fct(poly p, ring r) {
   return kFindZeroPoly(p, r, r);
}

/*
 * Returns maximal k, such that
 * 2^k | n
 */
int indexOf2(number n) {
  long test = (long) n;
  int i = 0;
  while (test%2 == 0) {
    i++;
    test = test / 2;
  }
  return i;
}

/*
 * Find an index i from G, such that
 * LT(rside) = x * LT(G[i]) has a solution
 * or -1 if rside is not in the
 * ideal of the leading coefficients
 * of the suitable g from G.
 */
int findRing2toMsolver(poly rside, ideal G, ring r) {
  if (rside == NULL) return -1;
  int i;
  int iO2rside = indexOf2(pGetCoeff(rside));
  for (i = 0; i < IDELEMS(G); i++) {
    if (indexOf2(pGetCoeff(G->m[i])) <= iO2rside && p_LmDivisibleBy(G->m[i], rside, r)) {
      return i;
    }
  }
  return -1;
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
  Exponent_t x;
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
  if (cp1 != 0 && cp2 != 0) {
    while (cp1%2 == 0 && cp2%2 == 0) {
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

poly spolyRing2toM(poly f, poly g, ring r) {
  poly m1 = NULL;
  poly m2 = NULL;
  ring2toM_GetLeadTerms(f, g, r, m1, m2, r);
  printPolyMsg("spoly: m1=", m1, " | ");
  printPolyMsg("m2=", m2, "");
  PrintLn();
  return pSub(pp_Mult_mm(f, m1, r), pp_Mult_mm(g, m2, r));
}

poly ringNF(poly f, ideal G, ring r) {
  // If f = 0, then normal form is also 0
  if (f == NULL) { return NULL; }
  poly h = pCopy(f);
  int i = findRing2toMsolver(h, G, r);
  int c = 1;
  while (h != NULL && i >= 0 && c < 20) {
    Print("%d-step NF - h:", c);
    wrp(h);
    PrintS(" ");
    PrintS("G->m[i]:");
    wrp(G->m[i]);
    PrintLn();
    h = spolyRing2toM(h, G->m[i], r);
    PrintS("=> h=");
    wrp(h);
    PrintLn();
    i = findRing2toMsolver(h, G, r);
    c++;
  }
  return h;
}

poly ringRedNF (poly f, ideal G, ring r) {
  // If f = 0, then normal form is also 0
  if (f == NULL) { return NULL; }
  poly h = NULL;
  poly g = pCopy(f);
  int c = 0;
  while (g != NULL && c < 20) {
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

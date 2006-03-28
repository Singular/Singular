/****************************************
*  Computer Algebra System SINGULAR     *
****************************************/
/* $Id: weight.cc,v 1.25 2006-03-28 12:56:05 Singular Exp $ */

/*
* ABSTRACT:
*/

#include <math.h>
#include "mod2.h"
#include "tok.h"
#include "omalloc.h"
#include "polys.h"
#include "intvec.h"
#include "febase.h"
#include "ipid.h"
#include "ipshell.h"
#include "subexpr.h"
#include "ideals.h"
#include "weight.h"

/*0 implementation*/
extern "C" double (*wFunctional)(int *degw, int *lpol, int npol,
       double *rel, double wx);
extern "C" double wFunctionalMora(int *degw, int *lpol, int npol,
       double *rel, double wx);
extern "C" double wFunctionalBuch(int *degw, int *lpol, int npol,
       double *rel, double wx);
extern "C" void wAdd(int *A, int mons, int kn, int xx);
extern "C" void wNorm(int *degw, int *lpol, int npol, double *rel);
extern "C" void wFirstSearch(int *A, int *x, int mons,
        int *lpol, int npol, double *rel, double *fopt);
extern "C" void wSecondSearch(int *A, int *x, int *lpol,
        int npol, int mons, double *rel, double *fk);
extern "C" void wGcd(int *x, int n);
extern double wNsqr;

static void wDimensions(polyset s, int sl, int *lpol, int *npol, int *mons)
{
  int  i, i1, j, k;
  poly p, q;

  i1 = j = 0;
  for (i = 0; i <= sl; i++)
  {
    p = s[i];
    if (p!=NULL)
    {
      k = 1;
      q = pNext(p);
      while (q!=NULL)
      {
        k++;
        q = pNext(q);
      }
      if (k > 1)
      {
        lpol[i1++] = k;
        j += k;
      }
    }
  }
  *npol = i1;
  *mons = j;
}


static void wInit(polyset s, int sl, int mons, int *A)
{
  int  n, a, i, j, *B, *C;
  poly p, q;
  int *pl;

  B = A;
  n = pVariables;
  a = (n + 1) * sizeof(int);
  pl = (int *)omAlloc(a);
  for (i = 0; i <= sl; i++)
  {
    p = s[i];
    if (p!=NULL)
    {
      q = pNext(p);
      if (q!=NULL)
      {
        C = B;
        B++;
        pGetExpV(p, pl);
        for (j = 0; j < n; j++)
        {
          *C = pl[j+1];
          C += mons;
        }
      }
      while (q!=NULL)
      {
        C = B;
        B++;
        pGetExpV(q, pl);
        for (j = 0; j < n; j++)
        {
          *C = pl[j+1];
          C += mons;
        }
        pIter(q);
      }
    }
  }
  omFreeSize((ADDRESS)pl, a);
}

static void wCall(polyset s, int sl, int *x)
{
  int  n, q, npol, mons, i;
  int  *A, *xopt, *lpol, *degw;
  double  f1, fx, eps, *rel;
  void *adr;

  n = pVariables;
  lpol = (int * )omAlloc((sl + 1) * sizeof(int));
  wDimensions(s, sl, lpol, &npol, &mons);
  xopt = x + (n + 1);
  for (i = n; i!=0; i--)
    xopt[i] = 1;
  if (mons==0)
  {
    omFreeSize((ADDRESS)lpol, (sl + 1) * sizeof(int));
    return;
  }
  adr = (void * )omAllocAligned(npol * sizeof(double));
  rel = (double*)adr;
  q = (n + 1) * mons * sizeof(int);
  A = (int * )omAlloc(q);
  wInit(s, sl, mons, A);
  degw = A + (n * mons);
  memset(degw, 0, mons * sizeof(int));
  for (i = n; i!=0; i--)
    wAdd(A, mons, i, 1);
  wNorm(degw, lpol, npol, rel);
  f1 = (*wFunctional)(degw, lpol, npol, rel, (double)1.0);
  if (TEST_OPT_PROT) Print("// %e\n",f1);
  eps = f1;
  fx = (double)2.0 * eps;
  memset(x, 0, (n + 1) * sizeof(int));
  wFirstSearch(A, x, mons, lpol, npol, rel, &fx);
  if (TEST_OPT_PROT) Print("// %e\n",fx);
  memcpy(x + 1, xopt + 1, n * sizeof(int));
  memset(degw, 0, mons * sizeof(int));
  for (i = n; i!=0; i--)
  {
    x[i] *= 16;
    wAdd(A, mons, i, x[i]);
  }
  wSecondSearch(A, x, lpol, npol, mons, rel, &fx);
  if (TEST_OPT_PROT) Print("// %e\n",fx);
  if (fx >= eps)
  {
    for (i = n; i!=0; i--)
      xopt[i] = 1;
  }
  else
  {
    wGcd(xopt, n);
//    if (BTEST1(22))
//    {
//      f1 = fx + (double)0.1 * (f1 - fx);
//      wSimple(x, n);
//      memset(degw, 0, mons * sizeof(int));
//      for (i = n; i!=0; i--)
//        wAdd(A, mons, i, x[i]);
//      eps = wPrWeight(x, n);
//      fx = (*wFunctional)(degw, lpol, npol, rel, eps);
//      if (fx < f1)
//      {
//        if (TEST_OPT_PROT) Print("// %e\n",fx);
//        memcpy(xopt + 1, x + 1, n * sizeof(int));
//      }
//    }
  }
  omFreeSize((ADDRESS)A, q);
  omFreeSize((ADDRESS)lpol, (sl + 1) * sizeof(int));
  omFreeSize((ADDRESS)adr, npol * sizeof(double));
}


void kEcartWeights(polyset s, int sl, short *eweight)
{
  int  n, i;
  int  *x;

  *eweight = 0;
  n = pVariables;
  wNsqr = (double)2.0 / (double)n;
  if (pOrdSgn == -1)
    wFunctional = wFunctionalMora;
  else
    wFunctional = wFunctionalBuch;
  x = (int * )omAlloc(2 * (n + 1) * sizeof(int));
  wCall(s, sl, x);
  for (i = n; i!=0; i--)
    eweight[i] = x[i + n + 1];
  omFreeSize((ADDRESS)x, 2 * (n + 1) * sizeof(int));
}


BOOLEAN kWeight(leftv res,leftv id)
{
  ideal F=(ideal)id->Data();
  intvec * iv = new intvec(pVariables);
  polyset s;
  int  sl, n, i;
  int  *x;

  res->data=(char *)iv;
  s = F->m;
  sl = IDELEMS(F) - 1;
  n = pVariables;
  wNsqr = (double)2.0 / (double)n;
  wFunctional = wFunctionalBuch;
  x = (int * )omAlloc(2 * (n + 1) * sizeof(int));
  wCall(s, sl, x);
  for (i = n; i!=0; i--)
    (*iv)[i-1] = x[i + n + 1];
  omFreeSize((ADDRESS)x, 2 * (n + 1) * sizeof(int));
  return FALSE;
}

BOOLEAN kQHWeight(leftv res,leftv v)
{
  res->data=(char *)idQHomWeight((ideal)v->Data());
  if (res->data==NULL)
    res->data=(char *)new intvec(pVariables);
  return FALSE;
}

short * iv2array(intvec * iv)
{
  short *s=(short *)omAlloc0((pVariables+1)*sizeof(short));
  int len=0;
  if(iv!=NULL)
    len=iv->length();
  int i;
  //for(i=pVariables;i>len;i--) s[i]=1;
  for(i=len;i>0;i--)               s[i]=(*iv)[i-1];
  return s;
}

/*2
*computes the degree of the leading term of the polynomial
*with respect to given ecartWeights
*used for Graebes method if BTEST1(31) is set
*/
long totaldegreeWecart(poly p, ring r)
{
  int i;
  long j =0;

  for (i=r->N; i; i--)
    j += (int)(p_GetExp(p,i,r) * ecartWeights[i]);
  return  j;
}

/*2
*computes the degree of the leading term of the polynomial
*with respect to given weights
*/
long totaldegreeWecart_IV(poly p, ring r, const short *w)
{
  int i;
  long j =0;

  for (i=r->N; i>0; i--)
    j += (long)((int)(p_GetExp(p,i,r) * (int)w[i]));
  return  j;
}

/*2
*computes the maximal degree of all terms of the polynomial
*with respect to given ecartWeights and
*computes the length of the polynomial
*used for Graebes method if BTEST1(31) is set
*/
long maxdegreeWecart(poly p,int *l, ring r)
{
  short k=p_GetComp(p, r);
  int ll=1;
  long t,max;

  max=totaldegreeWecart(p, r);
  pIter(p);
  while ((p!=NULL) && (p_GetComp(p, r)==k))
  {
    t=totaldegreeWecart(p, r);
    if (t>max) max=t;
    ll++;
    pIter(p);
  }
  *l=ll;
  return max;
}

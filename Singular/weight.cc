/****************************************
*  Computer Algebra System SINGULAR     *
****************************************/
/* $Id: weight.cc,v 1.9 1998-06-13 14:20:07 Singular Exp $ */

/*
* ABSTRACT:
*/

#include <math.h>
#include "mod2.h"
#include "tok.h"
#include "mmemory.h"
#include "polys.h"
#include "intvec.h"
#include "febase.h"
#include "ipid.h"
#include "ipshell.h"
#include "subexpr.h"
#include "ideals.h"
#include "weight.h"

/*0 implementation*/

pFDegProc pFDegOld;
pLDegProc pLDegOld;
#ifdef ALIGN_8
#define wDouble(A) ((double *)A)
#else
extern "C" double * wDouble(void *adr);
#endif

#ifndef __MWERKS__
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
#else
short * ecartWeights=NULL;

double wNsqr;
double (*wFunctional)(int *degw, int *lpol, int npol,
       double *rel, double wx);


static double wFunctionalMora(int *degw, int *lpol, int npol,
       double *rel, double wx)
{
  int  i, j, e1, ecu, ecl, ec;
  int  *ex;
  double gfmax, gecart, ghom, pfmax;
  double *r;

  ex = degw;
  r = rel;
  gfmax = (double)0.0;
  gecart = (double)0.4 + (double)npol;
  ghom = (double)1.0;
  for (i = 0; i < npol; i++)
  {
    ecl = ecu = e1 = *ex++;
    for (j = lpol[i] - 1; j!=0; j--)
    {
      ec = *ex++;
      if (ec > ecu)
        ecu = ec;
      else if (ec < ecl)
        ecl = ec;
    }
    pfmax = (double)ecl / (double)ecu;
    if (pfmax < ghom)
      ghom = pfmax;
    pfmax = (double)e1 / (double)ecu;
    if (pfmax > (double)0.5)
      gecart -= (pfmax * pfmax);
    else
      gecart -= (double)0.25;
    ecu = 2 * ecu - ecl;
    gfmax += (double)(ecu * ecu) * (*r++);
  }
  if (ghom > (double)0.8)
  {
    ghom *= (double)5.0;
    gecart *= ((double)5.0 - ghom);
  }
  return (gfmax * gecart) / pow(wx, wNsqr);
}


static double wFunctionalBuch(int *degw, int *lpol, int npol,
       double *rel, double wx)
{
  int  i, j, ecl, ecu, ec;
  int  *ex;
  double gfmax, ghom, pfmax;
  double *r;

  ex = degw;
  r = rel;
  gfmax = (double)0.0;
  ghom = (double)1.0;
  for (i = 0; i < npol; i++)
  {
    ecu = ecl = *ex++;
    for (j = lpol[i] - 1; j!=0; j--)
    {
      ec = *ex++;
      if (ec < ecl)
        ecl = ec;
      else if (ec > ecu)
        ecu = ec;
    }
    pfmax = (double)ecl / (double)ecu;
    if (pfmax < ghom)
      ghom = pfmax;
    gfmax += (double)(ecu * ecu) * (*r++);
  }
  if (ghom > (double)0.5)
    gfmax *= ((double)1.0 - (ghom * ghom)) / (double)0.75;
  return gfmax / pow(wx, wNsqr);
}


static void wSub(int *A, int mons, int kn, int xx)
{
  int  i, *B, *ex;

  B = A + ((kn - 1) * mons);
  ex = A + (pVariables * mons);
  if (xx == 1)
  {
    for (i = mons; i!=0; i--)
      *ex++ -= *B++;
  }
  else
  {
    for (i = mons; i!=0; i--)
      *ex++ -= (*B++) * xx;
  }
}


static void wAdd(int *A, int mons, int kn, int xx)
{
  int  i, *B, *ex;

  B = A + ((kn - 1) * mons);
  ex = A + (pVariables * mons);
  if (xx == 1)
  {
    for (i = mons; i!=0; i--)
      *ex++ += *B++;
  }
  else
  {
    for (i = mons; i!=0; i--)
      *ex++ += (*B++) * xx;
  }
}


static void wFirstSearch(int *A, int *x, int mons,
  int *lpol, int npol, double *rel, double *fopt)
{
  int  a0, a, n, xn, t, xx, y1;
  int  *y, *degw, *xopt;
  double  fy, fmax, wx;
  double *pr;
  void *adr;

  fy = *fopt;
  n = pVariables;
  xn = n + 6 + (21 / n);
  a0 = n * sizeof(double) + 8;
  a = n * sizeof(int);
  y = (int * )Alloc(a);
  adr = (void * )Alloc(a0);
  pr = wDouble(adr);
  *pr = (double)1.0;
  *y = 0;
  degw = A + (n * mons);
  xopt = x + (n + 2);
  t = 1;
  loop
  {
    while (t < n)
    {
      xx = x[t] + 1;
      wx = pr[t-1] * (double)xx;
      y1 = y[t-1] + xx;
      if ((y1 + n - t) <= xn)
      {
        pr[t] = wx;
        y[t] = y1;
        x[t] = xx;
        if (xx > 1)
          wAdd(A, mons, t, 1);
        t++;
      }
      else
      {
        xx = x[t] - 1;
        x[t] = 0;
        if (xx!=0)
          wSub(A, mons, t, xx);
        t--;
        if (t==0)
        {
          *fopt = fy;
          Free((ADDRESS)y, a);
          Free((ADDRESS)adr, a0);
          return;
        }
      }
    }
    xx = xn - y[t-1];
    wx = pr[t-1] * (double)xx;
    x[t] = xx;
    xx--;
    if (xx!=0)
      wAdd(A, mons, t, xx);
    fmax = (*wFunctional)(degw, lpol, npol, rel, wx);
    if (xx!=0)
      wSub(A, mons, t, xx);
    if (fmax < fy)
    {
      fy = fmax;
      memcpy(xopt, x + 1, a);
    }
    t--;
  } /* end loop */
}


static double wPrWeight(int *x, int n)
{
  int i;
  double y;

  y = (double)x[n];
  for (i = n - 1; i!=0; i--)
    y *= (double)x[i];
  return y;
}


static void wEstimate(int *A, int *x, int *lpol, int npol, int mons,
double wx, double *rel, double *fopt, int *s0, int *s1, int *s2)
{
  int  n, i1, i2, k0 = 0, k1 = 0, k2 = 0;
  int  *degw;
  double fo1, fo2, fmax, wx1, wx2;

  n = pVariables;
  degw = A + (n * mons);
  fo2 = fo1 = (double)1.0e10;
  for (i1 = n; i1!=0; i1--)
  {
    if (x[i1] > 1)
    {
      wSub(A, mons, i1, 1);
      wx1 = wx - wx / (double)x[i1];
      x[i1]--;
      fmax = (*wFunctional)(degw, lpol, npol, rel, wx1);
      if (fmax < fo1)
      {
        fo1 = fmax;
        k0 = i1;
      }
      for (i2 = i1; i2!=0; i2--)
      {
        if (x[i2] > 1)
        {
          wSub(A, mons, i2, 1);
          wx2 = wx1 - wx1 / (double)x[i2];
          fmax = (*wFunctional)(degw, lpol, npol, rel, wx2);
          if (fmax < fo2)
          {
            fo2 = fmax;
            k1 = i1;
            k2 = i2;
          }
          wAdd(A, mons, i2, 1);
        }
      }
      wAdd(A, mons, i1, 1);
      x[i1]++;
    }
  }
  if (fo1 < fo2)
  {
    *fopt = fo1;
    *s0 = k0;
  }
  else
  {
    *fopt = fo2;
    *s0 = 0;
  }
  *s1 = k1;
  *s2 = k2;
}


static void wSecondSearch(int *A, int *x, int *lpol,
int npol, int mons, double *rel, double *fk)
{
  int  n, s0, s1, s2, *xopt;
  double  one, fx, fopt, wx;

  n = pVariables;
  xopt = x + (n + 2);
  fopt = *fk * (double)0.999999999999;
  wx = wPrWeight(x, n);
  one = (double)1.0;
  loop
  {
    wEstimate(A, x, lpol, npol, mons, wx, rel, &fx, &s0, &s1, &s2);
    if (fx > fopt)
    {
      if (s0!=0)
        x[s0]--;
      else if (s1!=0)
      {
        x[s1]--;
        x[s2]--;
      }
      else
        break;
    }
    else
    {
      fopt = fx;
      if (s0!=0)
      {
        x[s0]--;
        memcpy(xopt, x + 1, n * sizeof(int));
        if (s1==0)
          break;
      }
      else if (s1!=0)
      {
        x[s1]--;
        x[s2]--;
        memcpy(xopt, x + 1, n * sizeof(int));
      }
      else
        break;
    }
    if (s0!=0)
      wSub(A, mons, s0, 1);
    else
    {
      wSub(A, mons, s1, 1);
      wSub(A, mons, s2, 1);
    }
    wx = wPrWeight(x, n);
  }
  *fk = fopt;
}


static void wGcd(int *x, int n)
{
  int i, b, a, h;

  i = n;
  b = x[i];
  loop
  {
    i--;
    if (i==0)
      break;
    a = x[i];
    if (a < b)
    {
      h = a;
      a = b;
      b = h;
    }
    do
    {
      h = a % b;
      a = b;
      b = h;
    }
    while (b!=0);
    b = a;
    if (b == 1)
      return;
  }
  for (i = n; i; i--)
    x[i] /= b;
}


static void wSimple(int *x, int n)
{
  int g, min, c, d, f, kopt, k, i;
  int *xopt;
  double sopt, s1, s2;

  xopt = x + (n + 1);
  kopt = k = g = 0;
  min = 1000000;
  for (i = n; i!=0; i--)
  {
    c = xopt[i];
    if (c > 1)
    {
      if (c < min)
        min = c;
      if (c > k)
        k = c;
    }
    else
      g = 1;
  }
  k -= min;
  if ((g==0) && (k < 4))
    return;
  if (k < min)
    min = k+1;
  sopt = (double)1.0e10;
  for (k = min; k > 1; k--)
  {
    s2 = s1 = (double)0.0;
    for(i = n; i!=0; i--)
    {
      c = xopt[i];
      if (c > 1)
      {
        d = c / k;
        d *= k;
        f = d = c - d;
        if (f!=0)
        {
          f = k - f;
          if (f < d)
            s2 += (double)f / (double)c;
          else
            s1 += (double)d / (double)c;
        }
      }
    }
    s1 += s2 + sqrt(s1 * s2);
    s1 -= (double)0.01 * sqrt((double)k);
    if (s1 < sopt)
    {
      sopt = s1;
      kopt = k;
    }
  }
  for(i = n; i!=0; i--)
  {
    x[i] = 1;
    c = xopt[i];
    if (c > 1)
    {
      d = c / kopt;
      d *= kopt;
      x[i] = d;
      d = c - d;
      if ((d!=0) && (kopt < 2 * d))
        x[i] += kopt;
    }
  }
  if (g==0)
    wGcd(x, n);
}


static void wNorm(int *degw, int *lpol, int npol, double *rel)
{
  int  i, j, ecu, ec;
  int  *ex;
  double *r;

  ex = degw;
  r = rel;
  for (i = 0; i < npol; i++)
  {
    ecu = *ex++;
    for (j = lpol[i] - 1; j!=0; j--)
    {
      ec = *ex++;
      if (ec > ecu)
        ecu = ec;
    }
    *r = (double)1.0 / (double)(ecu * ecu);
    r++;
  }
}
#endif /* __MWERKS */


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
  Exponent_t *pl;

  B = A;
  n = pVariables;
  a = (n + 1) * sizeof(Exponent_t);
  pl = (Exponent_t * )Alloc(a);
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
  Free((ADDRESS)pl, a);
}

static void wCall(polyset s, int sl, int *x)
{
  int  n, q, npol, mons, i;
  int  *A, *xopt, *lpol, *degw;
  double  f1, fx, eps, *rel;
  void *adr;

  n = pVariables;
  lpol = (int * )Alloc((sl + 1) * sizeof(int));
  wDimensions(s, sl, lpol, &npol, &mons);
  xopt = x + (n + 1);
  for (i = n; i!=0; i--)
    xopt[i] = 1;
  if (mons==0)
  {
    Free((ADDRESS)lpol, (sl + 1) * sizeof(int));
    return;
  }
  adr = (void * )Alloc(npol * sizeof(double) + 8);
  rel = wDouble(adr);
  q = (n + 1) * mons * sizeof(int);
  A = (int * )Alloc(q);
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
  Free((ADDRESS)A, q);
  Free((ADDRESS)lpol, (sl + 1) * sizeof(int));
  Free((ADDRESS)adr, npol * sizeof(double) + 8);
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
  x = (int * )Alloc(2 * (n + 1) * sizeof(int));
  wCall(s, sl, x);
  for (i = n; i!=0; i--)
    eweight[i] = x[i + n + 1];
  Free((ADDRESS)x, 2 * (n + 1) * sizeof(int));
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
  x = (int * )Alloc(2 * (n + 1) * sizeof(int));
  wCall(s, sl, x);
  for (i = n; i!=0; i--)
    (*iv)[i-1] = x[i + n + 1];
  Free((ADDRESS)x, 2 * (n + 1) * sizeof(int));
  return FALSE;
}

BOOLEAN kQHWeight(leftv res,leftv v)
{
  res->data=(char *)idQHomWeights((ideal)v->Data());
  if (res->data==NULL)
    res->data=(char *)new intvec(pVariables);
  return FALSE;
}

short * iv2array(intvec * iv)
{
  short *s=(short *)Alloc((pVariables+1)*sizeof(short));
  int len=iv->length();
  int i;

  for (i=pVariables;i>len;i--)
    s[i]= 1;
  for (;i>0;i--)
    s[i]= (*iv)[i-1];
  return s;
}

/*2
*computes the degree of the leading term of the polynomial
*with respect to given ecartWeights
*used for Graebes method if BTEST1(31) is set
*/
int totaldegreeWecart(poly p)
{
  int i;
  int j =0;

  for (i=pVariables; i; i--)
    j += (int)(pGetExp(p,i) * ecartWeights[i]);
  return  j;
}

/*2
*computes the maximal degree of all terms of the polynomial
*with respect to given ecartWeights and
*computes the length of the polynomial
*used for Graebes method if BTEST1(31) is set
*/
int maxdegreeWecart(poly p,int *l)
{
  short k=pGetComp(p);
  int ll=1;
  int  t,max;

  max=totaldegreeWecart(p);
  pIter(p);
  while ((p!=NULL) && (pGetComp(p)==k))
  {
    t=totaldegreeWecart(p);
    if (t>max) max=t;
    ll++;
    pIter(p);
  }
  *l=ll;
  return max;
}

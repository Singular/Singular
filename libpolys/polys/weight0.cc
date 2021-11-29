/****************************************
*  Computer Algebra System SINGULAR     *
****************************************/

/*
* ABSTRACT:
*/

#include "misc/auxiliary.h"
#include "omalloc/omalloc.h"

#include <math.h>
#include <string.h>

double wFunctionalMora(int *degw, int *lpol, int npol,
       double *rel, double wx, double wwNsqr);
double wFunctionalBuch(int *degw, int *lpol, int npol,
       double *rel, double wx, double wwNsqr);
void wAdd(int *A, int mons, int kn, int xx, int rvar);
void wNorm(int *degw, int *lpol, int npol, double *rel);
void wFirstSearch(int *A, int *x, int mons,
        int *lpol, int npol, double *rel, double *fopt, double wNsqr, int rvar);
void wSecondSearch(int *A, int *x, int *lpol,
        int npol, int mons, double *rel, double *fk, double wNsqr, int rvar);
void wGcd(int *x, int n);
/*0 implementation*/

VAR short * ecartWeights=NULL;

VAR double (*wFunctional)(int *degw, int *lpol, int npol,
       double *rel, double wx, double wNsqr);


double wFunctionalMora(int *degw, int *lpol, int npol,
       double *rel, double wx, double wNsqr)
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


double wFunctionalBuch(int *degw, int *lpol, int npol,
       double *rel, double wx, double wNsqr)
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
    for (j = lpol[i] - 1; j!=0 ; j--)
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


static void wSub(int *A, int mons, int kn, int xx,int rvar)
{
  int  i, *B, *ex;

  B = A + ((kn - 1) * mons);
  ex = A + (rvar * mons);
  i = mons;
  if (xx == 1)
  {
    for (/* i=mons */; i!=0 ; i--)
      *ex++ -= *B++;
  }
  else
  {
    for (/* i=mons */; i!=0 ; i--)
      *ex++ -= (*B++) * xx;
  }
}


void wAdd(int *A, int mons, int kn, int xx, int rvar)
{
  int  i, *B, *ex;

  B = A + ((kn - 1) * mons);
  ex = A + (rvar * mons);
  i = mons;
  if (xx == 1)
  {
    for (/* i=mons */; i!=0 ; i--)
      *ex++ += *B++;
  }
  else
  {
    for (/* i=mons */; i!=0 ; i--)
      *ex++ += (*B++) * xx;
  }
}


void wFirstSearch(int *A, int *x, int mons,
  int *lpol, int npol, double *rel, double *fopt, double wNsqr, int rvar)
{
  int  a0, a, n, xn, t, xx, y1;
  int  *y, *degw, *xopt;
  double  fy, fmax, wx;
  double *pr;
  void *adr;

  fy = *fopt;
  n = rvar;
  xn = n + 6 + (21 / n);
  a0 = n * sizeof(double);
  a = n * sizeof(int);
  y = (int * )omAlloc((long)a);
  adr = (void * )omAllocAligned((long)a0);
  pr = (double*)adr;
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
          wAdd(A, mons, t, 1, rvar);
        t++;
      }
      else
      {
        xx = x[t] - 1;
        x[t] = 0;
        if (xx!=0)
          wSub(A, mons, t, xx, rvar);
        t--;
        if (t==0)
        {
          *fopt = fy;
          omFreeSize((ADDRESS)y, (long)a);
          omFreeSize((ADDRESS)adr, (long)a0);
          return;
        }
      }
    }
    xx = xn - y[t-1];
    wx = pr[t-1] * (double)xx;
    x[t] = xx;
    xx--;
    if (xx!=0)
      wAdd(A, mons, t, xx, rvar);
    fmax = (*wFunctional)(degw, lpol, npol, rel, wx,wNsqr);
    if (xx!=0)
      wSub(A, mons, t, xx, rvar);
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
  for (i = n - 1; i!=0 ; i--)
    y *= (double)x[i];
  return y;
}


static void wEstimate(int *A, int *x, int *lpol, int npol, int mons,
double wx, double *rel, double *fopt, int *s0, int *s1, int *s2, double wNsqr, int rvar)
{
  int  n, i1, i2, k0 = 0, k1 = 0, k2 = 0;
  int  *degw;
  double fo1, fo2, fmax, wx1, wx2;

  n = rvar;
  degw = A + (n * mons);
  fo2 = fo1 = (double)1.0e10;
  for (i1 = n; i1!=0 ; i1--)
  {
    if (x[i1] > 1)
    {
      wSub(A, mons, i1, 1, rvar);
      wx1 = wx - wx / (double)x[i1];
      x[i1]--;
      fmax = (*wFunctional)(degw, lpol, npol, rel, wx1,wNsqr);
      if (fmax < fo1)
      {
        fo1 = fmax;
        k0 = i1;
      }
      for (i2 = i1; i2!=0 ; i2--)
      {
        if (x[i2] > 1)
        {
          wSub(A, mons, i2, 1, rvar);
          wx2 = wx1 - wx1 / (double)x[i2];
          fmax = (*wFunctional)(degw, lpol, npol, rel, wx2, wNsqr);
          if (fmax < fo2)
          {
            fo2 = fmax;
            k1 = i1;
            k2 = i2;
          }
          wAdd(A, mons, i2, 1, rvar);
        }
      }
      wAdd(A, mons, i1, 1, rvar);
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


void wSecondSearch(int *A, int *x, int *lpol,
int npol, int mons, double *rel, double *fk, double wNsqr, int rvar)
{
  int  n, s0, s1, s2, *xopt;
  double  fx, fopt, wx;

  n = rvar;
  xopt = x + (n + 2);
  fopt = *fk * (double)0.999999999999;
  wx = wPrWeight(x, n);
  loop
  {
    wEstimate(A, x, lpol, npol, mons, wx, rel, &fx, &s0, &s1, &s2, wNsqr, rvar);
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
      wSub(A, mons, s0, 1, rvar);
    else
    {
      wSub(A, mons, s1, 1, rvar);
      wSub(A, mons, s2, 1, rvar);
    }
    wx = wPrWeight(x, n);
  }
  *fk = fopt;
}


void wGcd(int *x, int n)
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
  for (i = n; i!=0 ; i--)
    x[i] /= b;
}


#if 0 /*currently unused*/
static void wSimple(int *x, int n)
{
  int g, min, c, d, f, kopt, k, i;
  int *xopt;
  double sopt, s1, s2;

  xopt = x + (n + 1);
  kopt = k = g = 0;
  min = 1000000;
  for (i = n; i!=0 ; i--)
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
    for(i = n; i!=0 ; i--)
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
  for(i = n; i!=0 ; i--)
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
#endif

void wNorm(int *degw, int *lpol, int npol, double *rel)
{
  int  i, j, ecu, ec;
  int  *ex;
  double *r;

  ex = degw;
  r = rel;
  for (i = 0; i < npol; i++)
  {
    ecu = *ex++;
    for (j = lpol[i] - 1; j!=0 ; j--)
    {
      ec = *ex++;
      if (ec > ecu)
        ecu = ec;
    }
    *r = (double)1.0 / (double)(ecu * ecu);
    r++;
  }
}

/****************************************
*  Computer Algebra System SINGULAR     *
****************************************/
static char  rcsid[] = "$Header: /exports/cvsroot-2/cvsroot/Singular/hilb.cc,v 1.1.1.1 1997-03-19 13:18:45 obachman Exp $";
/* $Log: not supported by cvs2svn $*/
/*
*  ABSTRACT -  Hilbert series
*/

#include "mod2.h"
#include "tok.h"
#include "febase.h"
#include "mmemory.h"
#include "polys.h"
#include "intvec.h"
#include "hutil.h"
#include "stairc.h"

static int  **Qpol;
static int  *Q0, *Ql;
static int  hLength;

static void hHilbEst(scfmon stc, int Nstc, varset var, int Nvar)
{
  int  i, j;
  int  x, y, z = 1;
  int  *p;
  for (i = Nvar; i>0; i--)
  {
    x = 0;
    for (j = 0; j < Nstc; j++)
    {
      y = stc[j][var[i]];
      if (y > x)
        x = y;
    }
    z += x;
    j = i - 1;
    if (z > Ql[j])
    {
      p = (int *)Alloc(z * sizeof(int));
      if (Ql[j]!=0)
      {
        if (j==0)
          memcpy(p, Qpol[j], Ql[j] * sizeof(int));
        Free((ADDRESS)Qpol[j], Ql[j] * sizeof(int));
      }
      if (j==0)
      {
        for (x = Ql[j]; x < z; x++)
          p[x] = 0;
      }
      Ql[j] = z;
      Qpol[j] = p;
    }
  }
}

static int  *hAddHilb(int Nv, int x, int *pol, int *lp)
{
  int  l = *lp, ln, i;
  int  *pon;
  *lp = ln = l + x;
  pon = Qpol[Nv];
  memcpy(pon, pol, l * sizeof(int));
  if (l > x)
  {
    for (i = x; i < l; i++)
      pon[i] -= pol[i - x];
    for (i = l; i < ln; i++)
      pon[i] = -pol[i - x];
  }
  else
  {
    for (i = l; i < x; i++)
      pon[i] = 0;
    for (i = x; i < ln; i++)
      pon[i] = -pol[i - x];
  }
  return pon;
}

static void hLastHilb(scmon pure, int Nv, varset var, int *pol, int lp)
{
  int  l = lp, x, i, j;
  int  *p, *pl;
  p = pol;
  for (i = Nv; i>0; i--)
  {
    x = pure[var[i + 1]];
    if (x!=0)
      p = hAddHilb(i, x, p, &l);
  }
  pl = *Qpol;
  j = Q0[Nv + 1];
  for (i = 0; i < l; i++)
    pl[i + j] += p[i];
  x = pure[var[1]];
  if (x!=0)
  {
    j += x;
    for (i = 0; i < l; i++)
      pl[i + j] -= p[i];
  }
  j += l;
  if (j > hLength)
    hLength = j;
}

static void hHilbStep(scmon pure, scfmon stc, int Nstc, varset var,
 int Nvar, int *pol, int Lpol)
{
  int  iv = Nvar -1, ln, a, a0, a1, b, i;
  short  x, x0;
  scmon pn;
  scfmon sn;
  int  *pon;
  if (Nstc==0)
  {
    hLastHilb(pure, iv, var, pol, Lpol);
    return;
  }
  x = a = 0;
  pn = hGetpure(pure);
  sn = hGetmem(Nstc, stc, stcmem[iv]);
  hStepS(sn, Nstc, var, Nvar, &a, &x);
  Q0[iv] = Q0[Nvar];
  ln = Lpol;
  pon = pol;
  if (a == Nstc)
  {
    x = pure[var[Nvar]];
    if (x!=0)
      pon = hAddHilb(iv, x, pon, &ln);
    hHilbStep(pn, sn, a, var, iv, pon, ln);
    return;
  }
  else
  {
    pon = hAddHilb(iv, x, pon, &ln);
    hHilbStep(pn, sn, a, var, iv, pon, ln);
  }
  b = a;
  x0 = 0;
  loop
  {
    Q0[iv] += (x - x0);
    a0 = a;
    x0 = x;
    hStepS(sn, Nstc, var, Nvar, &a, &x);
    hElimS(sn, &b, a0, a, var, iv);
    a1 = a;
    hPure(sn, a0, &a1, var, iv, pn, &i);
    hLex2S(sn, b, a0, a1, var, iv, hwork);
    b += (a1 - a0);
    ln = Lpol;
    if (a < Nstc)
    {
      pon = hAddHilb(iv, x - x0, pol, &ln);
      hHilbStep(pn, sn, b, var, iv, pon, ln);
    }
    else
    {
      x = pure[var[Nvar]];
      if (x!=0)
        pon = hAddHilb(iv, x - x0, pol, &ln);
      else
        pon = pol;
      hHilbStep(pn, sn, b, var, iv, pon, ln);
      return;
    }
  }
}

/*
*basic routines
*/

intvec * hSeries(ideal S, intvec *modulweight, ideal Q, int notstc)
{
  intvec *work, *hseries1=NULL;
  short  mc;
  int  *p0;
  int  i, j, k, l, ii;
  hexist = hInit(S, Q, &hNexist);
  if (hNexist==0)
  {
    hseries1=new intvec(1);
    (*hseries1)[0]=1;
    return hseries1;
  }
  p0 = (int *)Alloc(sizeof(int));
  *p0 = 1;
  hwork = (scfmon)Alloc(hNexist * sizeof(scmon));
  hvar = (varset)Alloc((pVariables + 1) * sizeof(int));
  hpure = (scmon)Alloc((1 + (pVariables * pVariables)) * sizeof(short));
  stcmem = hCreate(pVariables - 1);
  Qpol = (int **)Alloc((pVariables + 1) * sizeof(int *));
  Ql = (int *)Alloc0((pVariables + 1) * sizeof(int));
  Q0 = (int *)Alloc((pVariables + 1) * sizeof(int));
  *Qpol = NULL;
  hLength = k = j = 0;
  mc = hisModule;
  if (mc!=0)
    hstc = (scfmon)Alloc(hNexist * sizeof(scmon));
  else
  {
    hstc = hexist;
    hNstc = hNexist;
  }
  loop
  {
    if (mc!=0)
    {
      hComp(hexist, hNexist, mc, hstc, &hNstc);
      if (modulweight != NULL)
        j = (*modulweight)[mc-1];
    }
    if (hNstc!=0)
    {
      hNvar = pVariables;
      for (i = hNvar; i; i--)
        hvar[i] = i;
      if (notstc)
        hStaircase(hstc, &hNstc, hvar, hNvar);
      hSupp(hstc, hNstc, hvar, &hNvar);
      if (hNvar!=0)
      {
        if ((hNvar > 2) && (hNstc > 10))
          hOrdSupp(hstc, hNstc, hvar, hNvar);
        hHilbEst(hstc, hNstc, hvar, hNvar);
        memset(hpure, 0, (pVariables + 1) * sizeof(short));
        hPure(hstc, 0, &hNstc, hvar, hNvar, hpure, &hNpure);
        hLexS(hstc, hNstc, hvar, hNvar);
        Q0[hNvar] = 0;
        hHilbStep(hpure, hstc, hNstc, hvar, hNvar, p0, 1);
      }
    }
    else
    {
      if(*Qpol!=NULL)
        (**Qpol)++;
      else
      {
        *Qpol = (int *)Alloc(sizeof(int));
        hLength = *Ql = **Qpol = 1;
      }
    }
    if (*Qpol!=NULL)
    {
      i = hLength;
      while ((i > 0) && ((*Qpol)[i - 1] == 0))
        i--;
      if (i > 0)
      {
        l = i + j;
        if (l > k)
        {
          work = new intvec(l);
          for (ii=0; ii<k; ii++)
            (*work)[ii] = (*hseries1)[ii];
          if (hseries1 != NULL)
            delete hseries1;
          hseries1 = work;
          k = l;
        }
        while (i > 0)
        {
          (*hseries1)[i + j - 1] += (*Qpol)[i - 1];
          (*Qpol)[i - 1] = 0;
          i--;
        }
      }
    }
    mc--;
    if (mc <= 0)
      break;
  }
  if (k==0)
  {
    hseries1=new intvec(1);
    (*hseries1)[0]=0;
  }
  else
  {
    l = k;
    while ((*hseries1)[l-1]==0) l--;
    if (l<k)
    {
      work = new intvec(l);
      for (ii=0; ii<l; ii++)
        (*work)[ii] = (*hseries1)[ii];
      delete hseries1;
      hseries1 = work;
    }
  }
  for (i = 0; i <= pVariables; i++)
    if (Ql[i]!=0)
      Free((ADDRESS)Qpol[i], Ql[i] * sizeof(int));
  Free((ADDRESS)Q0, (pVariables + 1) * sizeof(int));
  Free((ADDRESS)Ql, (pVariables + 1) * sizeof(int));
  Free((ADDRESS)Qpol, (pVariables + 1) * sizeof(int *));
  hKill(stcmem, pVariables - 1);
  Free((ADDRESS)hpure, (1 + (pVariables * pVariables)) * sizeof(short));
  Free((ADDRESS)hvar, (pVariables + 1) * sizeof(int));
  Free((ADDRESS)hwork, hNexist * sizeof(scmon));
  Free((ADDRESS)hexist, hNexist * sizeof(scmon));
  Free((ADDRESS)p0, sizeof(int));
  if (hisModule!=0)
    Free((ADDRESS)hstc, hNexist * sizeof(scmon));
  return hseries1;
}


intvec * hHstdSeries(ideal S, intvec *modulweight, ideal Q)
{
  return hSeries(S, modulweight, Q, 0);
}

intvec * hFirstSeries(ideal S, intvec *modulweight, ideal Q)
{
  return hSeries(S, modulweight, Q, 1);
}

intvec * hSecondSeries(intvec *hseries1)
{
  intvec *work, *hseries2;
  int i, j, k, s, t, l;
  if (hseries1 == NULL)
    return NULL;
  hseries2 = new intvec(hseries1);
  k = l = hseries2->length();
  s = 0;
  for (i = k-1; i >= 0; i--)
    s += (*hseries2)[i];
  loop
  {
    if ((s != 0) || (k == 1))
      break;
    s = 0;
    t = (*hseries2)[k-1];
    k--;
    for (i = k-1; i >= 0; i--)
    {
      j = (*hseries2)[i];
      (*hseries2)[i] = -t;
      s += t;
      t += j;
    }
  }
  if (k < l)
  {
    work = new intvec(k);
    for (i = k-1; i >= 0; i--)
      (*work)[i] = (*hseries2)[i];
    delete hseries2;
    hseries2 = work;
  }
  return hseries2;
}

intvec * hThirdSeries(intvec *hseries, int dim)
{
  intvec *hseries3;
  int i, j, k,  t, l;
  if (hseries == NULL)
    return NULL;
  l = hseries->length();
  t = 0;
  for (i = l-1; i >= 0; i--)
    t += (*hseries)[i];
  if (t == 0)
  {
    hseries3 = hSecondSeries(hseries);
    k = hseries3->length();
    dim = pVariables+k-l;
    l = k;
  }
  else
    hseries3 = new intvec(hseries);
  k = dim;
  loop
  {
    if ((k == 0) || (l == 1))
      break;
    t = (*hseries3)[l-1];
    k--;
    l--;
    for (i = l-1; i >= 0; i--)
    {
      j = (*hseries3)[i];
      (*hseries3)[i] = -t;
      t += j;
    }
    (*hseries3)[l] = t;
  }
  return hseries3;
}

void hDegreeSeries(intvec *s1, intvec *s2, int *co, int *mu)
{
  int m, i, j, k;
  *co = *mu = 0;
  if ((s1 == NULL) || (s2 == NULL))
    return;
  i = s1->length();
  j = s2->length();
  if (j > i)
    return;
  m = 0;
  for(k=0; k<j; k++)
    m += (*s2)[k];
  *mu = m;
  *co = i - j;
}

void hPrintHilb(intvec *hseries, int dim)
{
  int  i, j, l, k;
  if (hseries == NULL)
    return;
  l = hseries->length();
  k = l-dim;
  for (i = 0; i < k; i++)
  {
    j = (*hseries)[i];
    if (j != 0)
    {
      Print("//  %8d t^%d\n", j, i);
    }
  }
  if (k < 0)
  {
    k--;
    for (i = 0; i < l; i++)
    {
      j = (*hseries)[i];
      if (j != 0)
      {
        Print("//  %8d (1-t)^-%d\n", j, i-k);
      }
    }
  }
  else
  {
    i = k;
    k--;
    for (; i < l; i++)
    {
      j = (*hseries)[i];
      if (j != 0)
      {
        Print("//  %8d (1-t)^-%d\n", j, i-k);
      }
    }
  }
}

/*
*caller
*/

static void hPrintDegree(int co, int mu)
{
  if (pOrdSgn == 1)
    Print("// codimension = %d\n// dimension   = %d\n// degree      = %d\n",
      co, pVariables - co, mu);
  else
    Print("// codimension  = %d\n// dimension    = %d\n// multiplicity = %d\n",
      co, pVariables - co, mu);
}

void hLookSeries(ideal S, intvec *modulweight, ideal Q)
{
  int co, mu, l;
  intvec *hseries2;
//  intvec *hseries3;
  intvec *hseries1 = hFirstSeries(S, modulweight, Q);
  hPrintHilb(hseries1, 0);
  l = hseries1->length();
  if (l > 1)
    hseries2 = hSecondSeries(hseries1);
  else
    hseries2 = hseries1;
  hDegreeSeries(hseries1, hseries2, &co, &mu);
//  if (co > 0)
//  {
    PrintLn();
    hPrintHilb(hseries2, 0);
//  }
  if ((l == 1) &&(mu == 0))
    hPrintDegree(pVariables+1, 0);
  else
    hPrintDegree(co, mu);
//  if (co < pVariables)
//  {
//    hseries3 = hThirdSeries(hseries2,pVariables-co);
//    PrintLn();
//    hPrintHilb(hseries3, pVariables-co);
//    delete hseries3;
//  }
  if (l>1)
    delete hseries1;
  delete hseries2;
}


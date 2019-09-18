/****************************************
*  Computer Algebra System SINGULAR     *
****************************************/
/*
* ABSTRACT: Utilities for staircase operations
*/

#include "kernel/mod2.h"

#include "polys/simpleideals.h"
#include "polys/monomials/p_polys.h"

#include "kernel/polys.h"
#include "kernel/combinatorics/hutil.h"

THREAD_VAR scfmon hexist, hstc, hrad, hwork;
THREAD_VAR scmon hpure, hpur0;
THREAD_VAR varset hvar, hsel;
THREAD_VAR int  hNexist, hNstc, hNrad, hNvar, hNpure;
THREAD_VAR int hisModule;
THREAD_VAR monf stcmem, radmem;

// Making a global "security" copy of the allocated exponent vectors
// is a dirty fix for correct memory disallocation: It would be
// better, if either the fields of heist are never touched
// (i.e. changed) except in hInit, or, if hInit would return the
// "security" copy as well. But then, all the relevant data is held in
// global variables, so we might do that here, as well.
THREAD_VAR static scfmon hsecure= NULL;

scfmon hInit(ideal S, ideal Q, int *Nexist, ring tailRing)
{
  id_TestTail(S, currRing, tailRing);
  if (Q!=NULL) id_TestTail(Q, currRing, tailRing);

//   if (tailRing != currRing)
    hisModule = id_RankFreeModule(S, currRing, tailRing);
//  else
//    hisModule = id_RankFreeModule(S, currRing);

  if (hisModule < 0)
    hisModule = 0;

  int  sl, ql, i, k = 0;
  polyset si, qi, ss;
  scfmon ex, ek;

  if (S!=NULL)
  {
    si = S->m;
    sl = IDELEMS(S);
  }
  else
  {
    si = NULL;
    sl = 0;
  }
  if (Q!=NULL)
  {
    qi = Q->m;
    ql = IDELEMS(Q);
  }
  else
  {
    qi = NULL;
    ql = 0;
  }
  if ((sl + ql) == 0)
  {
    *Nexist = 0;
    return NULL;
  }
  ss = si;
  for (i = sl; i>0; i--)
  {
    if (*ss!=0)
      k++;
    ss++;
  }
  ss = qi;
  for (i = ql; i>0; i--)
  {
    if (*ss!=0)
      k++;
    ss++;
  }
  *Nexist = k;
  if (k==0)
    return NULL;
  ek = ex = (scfmon)omAlloc0(k * sizeof(scmon));
  hsecure = (scfmon) omAlloc0(k * sizeof(scmon));
  for (i = sl; i>0; i--)
  {
    if (*si!=NULL)
    {
      *ek = (scmon) omAlloc(((currRing->N)+1)*sizeof(int));
      p_GetExpV(*si, *ek, currRing);
      ek++;
    }
    si++;
  }
  for (i = ql; i>0; i--)
  {
    if (*qi!=NULL)
    {
      *ek = (scmon) omAlloc(((currRing->N)+1)*sizeof(int));
      p_GetExpV(*qi, *ek, currRing);
      ek++;
    }
    qi++;
  }
  memcpy(hsecure, ex, k * sizeof(scmon));
  return ex;
}

#if 0
void hWeight()
{
  int i, k;
  int x;

  i = (currRing->N);
  loop
  {
    if (pWeight(i) != 1) break;
    i--;
    if (i == 0) return;
  }
  for (i=(currRing->N); i>0; i--)
  {
    x = pWeight(i);
    if (x != 1)
    {
      for (k=hNexist-1; k>=0; k--)
      {
        hexist[k][i] *= x;
      }
    }
  }
}
#endif

void hDelete(scfmon ev, int ev_length)
{
  int i;

  if (ev_length>0)
  {
    for (i=ev_length-1;i>=0;i--)
      omFreeSize(hsecure[i],((currRing->N)+1)*sizeof(int));
    omFreeSize(hsecure, ev_length*sizeof(scmon));
    omFreeSize(ev,  ev_length*sizeof(scmon));
  }
}


void hComp(scfmon exist, int Nexist, int ak, scfmon stc, int *Nstc)
{
  int k = 0;
  scfmon ex = exist, co = stc;
  int i;

  for (i = Nexist; i>0; i--)
  {
    if (((**ex) == 0) || ((**ex) == ak))
    {
      *co = *ex;
      co++;
      k++;
    }
    ex++;
  }
  *Nstc = k;
}


void hSupp(scfmon stc, int Nstc, varset var, int *Nvar)
{
  int  nv, i0, i1, i, j;
  nv = i0 = *Nvar;
  i1 = 0;
  for (i = 1; i <= nv; i++)
  {
    j = 0;
    loop
    {
      if (stc[j][i]>0)
      {
        i1++;
        var[i1] = i;
        break;
      }
      j++;
      if (j == Nstc)
      {
        var[i0] = i;
        i0--;
        break;
      }
    }
  }
  *Nvar = i1;
}

void hOrdSupp(scfmon stc, int Nstc, varset var, int Nvar)
{
  int  i, i1, j, jj, k, l;
  int  x;
  scmon temp, count;
  float o, h, g, *v1;

  v1 = (float *)omAlloc(Nvar * sizeof(float));
  temp = (int *)omAlloc(Nstc * sizeof(int));
  count = (int *)omAlloc(Nstc * sizeof(int));
  for (i = 1; i <= Nvar; i++)
  {
    i1 = var[i];
    *temp = stc[0][i1];
    *count = 1;
    jj = 1;
    for (j = 1; j < Nstc; j++)
    {
      x = stc[j][i1];
      k = 0;
      loop
      {
        if (x > temp[k])
        {
          k++;
          if (k == jj)
          {
            temp[k] = x;
            count[k] = 1;
            jj++;
            break;
          }
        }
        else if (x < temp[k])
        {
          for (l = jj; l > k; l--)
          {
            temp[l] = temp[l-1];
            count[l] = count[l-1];
          }
          temp[k] = x;
          count[k] = 1;
          jj++;
          break;
        }
        else
        {
          count[k]++;
          break;
        }
      }
    }
    h = 0.0;
    o = (float)Nstc/(float)jj;
    for(j = 0; j < jj; j++)
    {
       g = (float)count[j];
       if (g > o)
         g -= o;
       else
         g = o - g;
       if (g > h)
         h = g;
    }
    v1[i-1] = h * (float)jj;
  }
  omFreeSize((ADDRESS)count, Nstc * sizeof(int));
  omFreeSize((ADDRESS)temp, Nstc * sizeof(int));
  for (i = 1; i < Nvar; i++)
  {
    i1 = var[i+1];
    h = v1[i];
    j = 0;
    loop
    {
      if (h > v1[j])
      {
        for (l = i; l > j; l--)
        {
          v1[l] = v1[l-1];
          var[l+1] = var[l];
        }
        v1[j] = h;
        var[j+1] = i1;
        break;
      }
      j++;
      if (j == i)
        break;
    }
  }
  omFreeSize((ADDRESS)v1, Nvar * sizeof(float));
}


static void hShrink(scfmon co, int a, int Nco)
{
  while ((co[a]!=NULL) && (a<Nco)) a++;
  int i = a;
  int j;
  for (j = a; j < Nco; j++)
  {
    if (co[j]!=NULL)
    {
      co[i] = co[j];
      i++;
    }
  }
}


void hStaircase(scfmon stc, int *Nstc, varset var, int Nvar)
{
  int  nc = *Nstc;
  if (nc < 2)
    return;
  int z = 0;
  int i = 0;
  int j = 1;
  scmon n = stc[1 /*j*/];
  scmon o = stc[0];
  int k = Nvar;
  loop
  {
    int k1 = var[k];
    if (o[k1] > n[k1])
    {
      loop
      {
        k--;
        if (k==0)
        {
          stc[i] = NULL;
          z++;
          break;
        }
        else
        {
          k1 = var[k];
          if (o[k1] < n[k1])
            break;
        }
      }
      k = Nvar;
    }
    else if (o[k1] < n[k1])
    {
      loop
      {
        k--;
        if (k==0)
        {
          stc[j] = NULL;
          z++;
          break;
        }
        else
        {
          k1 = var[k];
          if (o[k1] > n[k1])
            break;
        }
      }
      k = Nvar;
    }
    else
    {
      k--;
      if (k==0)
      {
        stc[j] = NULL;
        z++;
        k = Nvar;
      }
    }
    if (k == Nvar)
    {
      if (stc[j]==NULL)
        i = j - 1;
      loop
      {
        i++;
        if (i == j)
        {
          i = -1;
          j++;
          if (j < nc)
            n = stc[j];
          else
          {
            if (z!=0)
            {
              *Nstc -= z;
              hShrink(stc, 0, nc);
            }
            return;
          }
        }
        else if (stc[i]!=NULL)
        {
          o = stc[i];
          break;
        }
      }
    }
  }
}


void hRadical(scfmon rad, int *Nrad, int Nvar)
{
  int  nc = *Nrad, z = 0, i, j, k;
  scmon n, o;
  if (nc < 2)
    return;
  i = 0;
  j = 1;
  n = rad[j];
  o = rad[0];
  k = Nvar;
  loop
  {
    if ((o[k]!=0) && (n[k]==0))
    {
      loop
      {
        k--;
        if (k==0)
        {
          rad[i] = NULL;
          z++;
          break;
        }
        else
        {
          if ((o[k]==0) && (n[k]!=0))
            break;
        }
      }
      k = Nvar;
    }
    else if (!o[k] && n[k])
    {
      loop
      {
        k--;
        if (!k)
        {
          rad[j] = NULL;
          z++;
          break;
        }
        else
        {
          if (o[k] && !n[k])
            break;
        }
      }
      k = Nvar;
    }
    else
    {
      k--;
      if (!k)
      {
        rad[j] = NULL;
        z++;
        k = Nvar;
      }
    }
    if (k == Nvar)
    {
      if (!rad[j])
        i = j - 1;
      loop
      {
        i++;
        if (i == j)
        {
          i = -1;
          j++;
          if (j < nc)
            n = rad[j];
          else
          {
            if (z)
            {
              *Nrad -= z;
              hShrink(rad, 0, nc);
            }
            return;
          }
        }
        else if (rad[i])
        {
          o = rad[i];
          break;
        }
      }
    }
  }
}


void hLexS(scfmon stc, int Nstc, varset var, int Nvar)
{
  if (Nstc < 2)
    return;
  int  j = 1, i = 0;
  scmon n = stc[j];
  scmon o = stc[0];
  int k = Nvar;
  loop
  {
    int k1 = var[k];
    if (o[k1] < n[k1])
    {
      i++;
      if (i < j)
      {
        o = stc[i];
        k = Nvar;
      }
      else
      {
        j++;
        if (j < Nstc)
        {
          i = 0;
          o = stc[0];
          n = stc[j];
          k = Nvar;
        }
        else
          return;
      }
    }
    else if (o[k1] > n[k1])
    {
      int tmp_k;
      for (tmp_k = j; tmp_k > i; tmp_k--)
        stc[tmp_k] = stc[tmp_k - 1];
      stc[i] = n;
      j++;
      if (j < Nstc)
      {
        i = 0;
        o = stc[0];
        n = stc[j];
        k = Nvar;
      }
      else
        return;
    }
    else
    {
      k--;
      if (k<=0) return;
    }
  }
}


void hLexR(scfmon rad, int Nrad, varset var, int Nvar)
{
  int  j = 1, i = 0, k, k1;
  scmon n, o;
  if (Nrad < 2)
    return;
  n = rad[j];
  o = rad[0];
  k = Nvar;
  loop
  {
    k1 = var[k];
    if (!o[k1] && n[k1])
    {
      i++;
      if (i < j)
      {
        o = rad[i];
        k = Nvar;
      }
      else
      {
        j++;
        if (j < Nrad)
        {
          i = 0;
          o = rad[0];
          n = rad[j];
          k = Nvar;
        }
        else
          return;
      }
    }
    else if (o[k1] && !n[k1])
    {
      for (k = j; k > i; k--)
        rad[k] = rad[k - 1];
      rad[i] = n;
      j++;
      if (j < Nrad)
      {
        i = 0;
        o = rad[0];
        n = rad[j];
        k = Nvar;
      }
      else
        return;
    }
    else
      k--;
  }
}


void hPure(scfmon stc, int a, int *Nstc, varset var, int Nvar,
 scmon pure, int *Npure)
{
  int  nc = *Nstc, np = 0, nq = 0, j, i, i1, c, l;
  scmon x;
  for (j = a; j < nc; j++)
  {
    x = stc[j];
    i = Nvar;
    c = 2;
    l = 0;
    loop
    {
      i1 = var[i];
      if (x[i1])
      {
        c--;
        if (!c)
        {
          l = 0;
          break;
        }
        else if (c == 1)
          l = i1;
      }
      i--;
      if (!i)
        break;
    }
    if (l)
    {
      if (!pure[l])
      {
        np++;
        pure[l] = x[l];
      }
      else if (x[l] < pure[l])
        pure[l] = x[l];
      stc[j] = NULL;
      nq++;
    }
  }
  *Npure = np;
  if (nq!=0)
  {
    *Nstc -= nq;
    hShrink(stc, a, nc);
  }
}


void hElimS(scfmon stc, int *e1, int a2, int e2, varset var, int Nvar)
{
  int  nc = *e1, z = 0, i, j, k, k1;
  scmon n, o;
  if (!nc || (a2 == e2))
    return;
  j = 0;
  i = a2;
  o = stc[i];
  n = stc[0];
  k = Nvar;
  loop
  {
    k1 = var[k];
    if (o[k1] > n[k1])
    {
      k = Nvar;
      i++;
      if (i < e2)
        o = stc[i];
      else
      {
        j++;
        if (j < nc)
        {
          i = a2;
          o = stc[i];
          n = stc[j];
        }
        else
        {
          if (z!=0)
          {
            *e1 -= z;
            hShrink(stc, 0, nc);
          }
          return;
        }
      }
    }
    else
    {
      k--;
      if (k==0)
      {
        stc[j] = NULL;
        z++;
        j++;
        if (j < nc)
        {
          i = a2;
          o = stc[i];
          n = stc[j];
          k = Nvar;
        }
        else
        {
          if (z!=0)
          {
            *e1 -= z;
            hShrink(stc, 0, nc);
          }
          return;
        }
      }
    }
  }
}


void hElimR(scfmon rad, int *e1, int a2, int e2, varset var, int Nvar)
{
  int  nc = *e1, z = 0, i, j, k, k1;
  scmon n, o;
  if (!nc || (a2 == e2))
    return;
  j = 0;
  i = a2;
  o = rad[i];
  n = rad[0];
  k = Nvar;
  loop
  {
    k1 = var[k];
    if (o[k1] && !n[k1])
    {
      k = Nvar;
      i++;
      if (i < e2)
        o = rad[i];
      else
      {
        j++;
        if (j < nc)
        {
          i = a2;
          o = rad[i];
          n = rad[j];
        }
        else
        {
          if (z!=0)
          {
            *e1 -= z;
            hShrink(rad, 0, nc);
          }
          return;
        }
      }
    }
    else
    {
      k--;
      if (!k)
      {
        rad[j] = NULL;
        z++;
        j++;
        if (j < nc)
        {
          i = a2;
          o = rad[i];
          n = rad[j];
          k = Nvar;
        }
        else
        {
          if (z!=0)
          {
            *e1 -= z;
            hShrink(rad, 0, nc);
          }
          return;
        }
      }
    }
  }
}


void hLex2S(scfmon rad, int e1, int a2, int e2, varset var,
 int Nvar, scfmon w)
{
  int  j0 = 0, j = 0, i = a2, k, k1;
  scmon n, o;
  if (!e1)
  {
    for (; i < e2; i++)
      rad[i - a2] = rad[i];
    return;
  }  else if (i == e2)
    return;
  n = rad[j];
  o = rad[i];
  loop
  {
    k = Nvar;
    loop
    {
      k1 = var[k];
      if (o[k1] < n[k1])
      {
        w[j0] = o;
        j0++;
        i++;
        if (i < e2)
        {
          o = rad[i];
          break;
        }
        else
        {
          for (; j < e1; j++)
          {
            w[j0] = rad[j];
            j0++;
          }
          memcpy(rad, w, (e1 + e2 - a2) * sizeof(scmon));
          return;
        }
      }
      else if (o[k1] > n[k1])
      {
        w[j0] = n;
        j0++;
        j++;
        if (j < e1)
        {
          n = rad[j];
          break;
        }
        else
        {
          for (; i < e2; i++)
          {
            w[j0] = rad[i];
            j0++;
          }
          memcpy(rad, w, (e1 + e2 - a2) * sizeof(scmon));
          return;
        }
      }
      k--;
    }
  }
}


void hLex2R(scfmon rad, int e1, int a2, int e2, varset var,
 int Nvar, scfmon w)
{
  int  j0 = 0, j = 0, i = a2, k, k1;
  scmon n, o;
  if (!e1)
  {
    for (; i < e2; i++)
      rad[i - a2] = rad[i];
    return;
  }
  else if (i == e2)
    return;
  n = rad[j];
  o = rad[i];
  loop
  {
    k = Nvar;
    loop
    {
      k1 = var[k];
      if (!o[k1] && n[k1])
      {
        w[j0] = o;
        j0++;
        i++;
        if (i < e2)
        {
          o = rad[i];
          break;
        }
        else
        {
          for (; j < e1; j++)
          {
            w[j0] = rad[j];
            j0++;
          }
          memcpy(rad, w, (e1 + e2 - a2) * sizeof(scmon));
          return;
        }
      }
      else if (o[k1] && !n[k1])
      {
        w[j0] = n;
        j0++;
        j++;
        if (j < e1)
        {
          n = rad[j];
          break;
        }
        else
        {
          for (; i < e2; i++)
          {
            w[j0] = rad[i];
            j0++;
          }
          memcpy(rad, w, (e1 + e2 - a2) * sizeof(scmon));
          return;
        }
      }
      k--;
    }
  }
}


void hStepS(scfmon stc, int Nstc, varset var, int Nvar, int *a, int *x)
{
  int  k1, i;
  int  y;
  k1 = var[Nvar];
  y = *x;
  i = *a;
  loop
  {
    if (y < stc[i][k1])
    {
      *a = i;
      *x = stc[i][k1];
      return;
    }
    i++;
    if (i == Nstc)
    {
      *a = i;
      return;
    }
  }
}


void hStepR(scfmon rad, int Nrad, varset var, int Nvar, int *a)
{
  int  k1, i;
  k1 = var[Nvar];
  i = 0;
  loop
  {
    if (rad[i][k1])
    {
      *a = i;
      return;
    }
    i++;
    if (i == Nrad)
    {
      *a = i;
      return;
    }
  }
}


monf hCreate(int Nvar)
{
  monf xmem;
  int  i;
  xmem = (monf)omAlloc((Nvar + 1) * sizeof(monp));
  for (i = Nvar; i>0; i--)
  {
    xmem[i] = (monp)omAlloc(LEN_MON);
    xmem[i]->mo = NULL;
  }
  return xmem;
}


void hKill(monf xmem, int Nvar)
{
  int  i;
  for (i = Nvar; i!=0; i--)
  {
    if (xmem[i]->mo!=NULL)
      omFreeSize((ADDRESS)xmem[i]->mo, xmem[i]->a * sizeof(scmon));
    omFreeSize((ADDRESS)xmem[i], LEN_MON);
  }
  omFreeSize((ADDRESS)xmem, (Nvar + 1) * sizeof(monp));
}


scfmon hGetmem(int lm, scfmon old, monp monmem)
{
  scfmon x = monmem->mo;
  int  lx = monmem->a;
  if ((x==NULL) || (lm > lx))
  {
    /* according to http://www.singular.uni-kl.de:8002/trac/ticket/463#comment:4
     * we need to work around a compiler bug:
    * if ((x!=NULL)&&(lx>0)) omFreeSize((ADDRESS)x, lx * sizeof(scmon));
    */
    if (x!=NULL) if (lx>0) omFreeSize((ADDRESS)x, lx * sizeof(scmon));
    monmem->mo = x = (scfmon)omAlloc(lm * sizeof(scmon));
    monmem->a = lm;
  }
  memcpy(x, old, lm * sizeof(scmon));
  return x;
}

/*
* a bug in Metrowerks with "lifetime analysis"
*scmon hGetpure(scmon p)
*{
*  scmon p1, pn;
*  p1 = p + 1;
*  pn = p1 + (currRing->N);
*  memcpy(pn, p1, (currRing->N) * sizeof(int));
*  return pn - 1;
*}
*/
scmon hGetpure(scmon p)
{
  scmon p1 = p;
  scmon pn;
  p1++;
  pn = p1;
  pn += (currRing->N);
  memcpy(pn, p1, (currRing->N) * sizeof(int));
  return pn - 1;
}


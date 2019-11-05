/****************************************
*  Computer Algebra System SINGULAR     *
****************************************/
/*
*  ABSTRACT -  dimension, multiplicity, HC, kbase
*/

#include "kernel/mod2.h"

#include "misc/intvec.h"
#include "coeffs/numbers.h"

#include "kernel/structs.h"
#include "kernel/ideals.h"
#include "kernel/polys.h"

#include "kernel/combinatorics/hutil.h"
#include "kernel/combinatorics/hilb.h"
#include "kernel/combinatorics/stairc.h"

VAR int  hCo, hMu, hMu2;
VAR omBin indlist_bin = omGetSpecBin(sizeof(indlist));

/*0 implementation*/

// dimension

void hDimSolve(scmon pure, int Npure, scfmon rad, int Nrad,
 varset var, int Nvar)
{
  int  dn, iv, rad0, b, c, x;
  scmon pn;
  scfmon rn;
  if (Nrad < 2)
  {
    dn = Npure + Nrad;
    if (dn < hCo)
      hCo = dn;
    return;
  }
  if (Npure+1 >= hCo)
    return;
  iv = Nvar;
  while(pure[var[iv]]) iv--;
  hStepR(rad, Nrad, var, iv, &rad0);
  if (rad0!=0)
  {
    iv--;
    if (rad0 < Nrad)
    {
      pn = hGetpure(pure);
      rn = hGetmem(Nrad, rad, radmem[iv]);
      hDimSolve(pn, Npure + 1, rn, rad0, var, iv);
      b = rad0;
      c = Nrad;
      hElimR(rn, &rad0, b, c, var, iv);
      hPure(rn, b, &c, var, iv, pn, &x);
      hLex2R(rn, rad0, b, c, var, iv, hwork);
      rad0 += (c - b);
      hDimSolve(pn, Npure + x, rn, rad0, var, iv);
    }
    else
    {
      hDimSolve(pure, Npure, rad, Nrad, var, iv);
    }
  }
  else
    hCo = Npure + 1;
}

int  scDimInt(ideal S, ideal Q)
{
  id_Test(S, currRing);
  if( Q!=NULL ) id_Test(Q, currRing);

  int  mc;
  hexist = hInit(S, Q, &hNexist, currRing);
  if (!hNexist)
    return (currRing->N);
  hwork = (scfmon)omAlloc(hNexist * sizeof(scmon));
  hvar = (varset)omAlloc(((currRing->N) + 1) * sizeof(int));
  hpure = (scmon)omAlloc((1 + ((currRing->N) * (currRing->N))) * sizeof(int));
  mc = hisModule;
  if (!mc)
  {
    hrad = hexist;
    hNrad = hNexist;
  }
  else
    hrad = (scfmon)omAlloc(hNexist * sizeof(scmon));
  radmem = hCreate((currRing->N) - 1);
  hCo = (currRing->N) + 1;
  loop
  {
    if (mc)
      hComp(hexist, hNexist, mc, hrad, &hNrad);
    if (hNrad)
    {
      hNvar = (currRing->N);
      hRadical(hrad, &hNrad, hNvar);
      hSupp(hrad, hNrad, hvar, &hNvar);
      if (hNvar)
      {
        memset(hpure, 0, ((currRing->N) + 1) * sizeof(int));
        hPure(hrad, 0, &hNrad, hvar, hNvar, hpure, &hNpure);
        hLexR(hrad, hNrad, hvar, hNvar);
        hDimSolve(hpure, hNpure, hrad, hNrad, hvar, hNvar);
      }
    }
    else
    {
      hCo = 0;
      break;
    }
    mc--;
    if (mc <= 0)
      break;
  }
  hKill(radmem, (currRing->N) - 1);
  omFreeSize((ADDRESS)hpure, (1 + ((currRing->N) * (currRing->N))) * sizeof(int));
  omFreeSize((ADDRESS)hvar, ((currRing->N) + 1) * sizeof(int));
  omFreeSize((ADDRESS)hwork, hNexist * sizeof(scmon));
  hDelete(hexist, hNexist);
  if (hisModule)
    omFreeSize((ADDRESS)hrad, hNexist * sizeof(scmon));
  return (currRing->N) - hCo;
}

int  scDimIntRing(ideal vid, ideal Q)
{
#ifdef HAVE_RING
  if (rField_is_Ring(currRing))
  {
    int i = idPosConstant(vid);
    if ((i != -1) && (n_IsUnit(pGetCoeff(vid->m[i]),currRing->cf)))
    { /* ideal v contains unit; dim = -1 */
      return(-1);
    }
    ideal vv = id_Head(vid,currRing);
    idSkipZeroes(vv);
    int j = idPosConstant(vv);
    long d;
    if(j == -1)
    {
      d = (long)scDimInt(vv, Q);
      if(rField_is_Z(currRing))
        d++;
    }
    else
    {
      if(n_IsUnit(pGetCoeff(vv->m[j]),currRing->cf))
        d = -1;
      else
        d = (long)scDimInt(vv, Q);
    }
    //Anne's Idea for std(4,2x) = 0 bug
    long dcurr = d;
    for(unsigned ii=0;ii<(unsigned)IDELEMS(vv);ii++)
    {
      if(vv->m[ii] != NULL && !n_IsUnit(pGetCoeff(vv->m[ii]),currRing->cf))
      {
        ideal vc = idCopy(vv);
        poly c = pInit();
        pSetCoeff0(c,nCopy(pGetCoeff(vv->m[ii])));
        idInsertPoly(vc,c);
        idSkipZeroes(vc);
        for(unsigned jj = 0;jj<(unsigned)IDELEMS(vc)-1;jj++)
        {
          if((vc->m[jj]!=NULL)
          && (n_DivBy(pGetCoeff(vc->m[jj]),pGetCoeff(c),currRing->cf)))
          {
            pDelete(&vc->m[jj]);
          }
        }
        idSkipZeroes(vc);
        j = idPosConstant(vc);
        if (j != -1) pDelete(&vc->m[j]);
        dcurr = (long)scDimInt(vc, currRing->qideal);
        // the following assumes the ground rings to be either zero- or one-dimensional
        if((j==-1) && rField_is_Z(currRing))
        {
          // should also be activated for other euclidean domains as groundfield
          dcurr++;
        }
        idDelete(&vc);
      }
      if(dcurr > d)
          d = dcurr;
    }
    idDelete(&vv);
    return d;
  }
#endif
  return scDimInt(vid,Q);
}

// independent set
STATIC_VAR scmon hInd;

static void hIndSolve(scmon pure, int Npure, scfmon rad, int Nrad,
 varset var, int Nvar)
{
  int  dn, iv, rad0, b, c, x;
  scmon pn;
  scfmon rn;
  if (Nrad < 2)
  {
    dn = Npure + Nrad;
    if (dn < hCo)
    {
      hCo = dn;
      for (iv=(currRing->N); iv; iv--)
      {
        if (pure[iv])
          hInd[iv] = 0;
        else
          hInd[iv] = 1;
      }
      if (Nrad)
      {
        pn = *rad;
        iv = Nvar;
        loop
        {
          x = var[iv];
          if (pn[x])
          {
            hInd[x] = 0;
            break;
          }
          iv--;
        }
      }
    }
    return;
  }
  if (Npure+1 >= hCo)
    return;
  iv = Nvar;
  while(pure[var[iv]]) iv--;
  hStepR(rad, Nrad, var, iv, &rad0);
  if (rad0)
  {
    iv--;
    if (rad0 < Nrad)
    {
      pn = hGetpure(pure);
      rn = hGetmem(Nrad, rad, radmem[iv]);
      pn[var[iv + 1]] = 1;
      hIndSolve(pn, Npure + 1, rn, rad0, var, iv);
      pn[var[iv + 1]] = 0;
      b = rad0;
      c = Nrad;
      hElimR(rn, &rad0, b, c, var, iv);
      hPure(rn, b, &c, var, iv, pn, &x);
      hLex2R(rn, rad0, b, c, var, iv, hwork);
      rad0 += (c - b);
      hIndSolve(pn, Npure + x, rn, rad0, var, iv);
    }
    else
    {
      hIndSolve(pure, Npure, rad, Nrad, var, iv);
    }
  }
  else
  {
    hCo = Npure + 1;
    for (x=(currRing->N); x; x--)
    {
      if (pure[x])
        hInd[x] = 0;
      else
        hInd[x] = 1;
    }
    hInd[var[iv]] = 0;
  }
}

intvec * scIndIntvec(ideal S, ideal Q)
{
  id_Test(S, currRing);
  if( Q!=NULL ) id_Test(Q, currRing);

  intvec *Set=new intvec((currRing->N));
  int  mc,i;
  hexist = hInit(S, Q, &hNexist, currRing);
  if (hNexist==0)
  {
    for(i=0; i<(currRing->N); i++)
      (*Set)[i]=1;
    return Set;
  }
  hwork = (scfmon)omAlloc(hNexist * sizeof(scmon));
  hvar = (varset)omAlloc(((currRing->N) + 1) * sizeof(int));
  hpure = (scmon)omAlloc((1 + ((currRing->N) * (currRing->N))) * sizeof(int));
  hInd = (scmon)omAlloc0((1 + (currRing->N)) * sizeof(int));
  mc = hisModule;
  if (mc==0)
  {
    hrad = hexist;
    hNrad = hNexist;
  }
  else
    hrad = (scfmon)omAlloc(hNexist * sizeof(scmon));
  radmem = hCreate((currRing->N) - 1);
  hCo = (currRing->N) + 1;
  loop
  {
    if (mc!=0)
      hComp(hexist, hNexist, mc, hrad, &hNrad);
    if (hNrad!=0)
    {
      hNvar = (currRing->N);
      hRadical(hrad, &hNrad, hNvar);
      hSupp(hrad, hNrad, hvar, &hNvar);
      if (hNvar!=0)
      {
        memset(hpure, 0, ((currRing->N) + 1) * sizeof(int));
        hPure(hrad, 0, &hNrad, hvar, hNvar, hpure, &hNpure);
        hLexR(hrad, hNrad, hvar, hNvar);
        hIndSolve(hpure, hNpure, hrad, hNrad, hvar, hNvar);
      }
    }
    else
    {
      hCo = 0;
      break;
    }
    mc--;
    if (mc <= 0)
      break;
  }
  for(i=0; i<(currRing->N); i++)
    (*Set)[i] = hInd[i+1];
  hKill(radmem, (currRing->N) - 1);
  omFreeSize((ADDRESS)hpure, (1 + ((currRing->N) * (currRing->N))) * sizeof(int));
  omFreeSize((ADDRESS)hInd, (1 + (currRing->N)) * sizeof(int));
  omFreeSize((ADDRESS)hvar, ((currRing->N) + 1) * sizeof(int));
  omFreeSize((ADDRESS)hwork, hNexist * sizeof(scmon));
  hDelete(hexist, hNexist);
  if (hisModule)
    omFreeSize((ADDRESS)hrad, hNexist * sizeof(scmon));
  return Set;
}

VAR indset ISet, JSet;

static BOOLEAN hNotZero(scfmon rad, int Nrad, varset var, int Nvar)
{
  int  k1, i;
  k1 = var[Nvar];
  i = 0;
  loop
  {
    if (rad[i][k1]==0)
      return FALSE;
    i++;
    if (i == Nrad)
      return TRUE;
  }
}

static void hIndep(scmon pure)
{
  int iv;
  intvec *Set;

  Set = ISet->set = new intvec((currRing->N));
  for (iv=(currRing->N); iv!=0 ; iv--)
  {
    if (pure[iv])
      (*Set)[iv-1] = 0;
    else
      (*Set)[iv-1] = 1;
  }
  ISet = ISet->nx = (indset)omAlloc0Bin(indlist_bin);
  hMu++;
}

void hIndMult(scmon pure, int Npure, scfmon rad, int Nrad,
 varset var, int Nvar)
{
  int  dn, iv, rad0, b, c, x;
  scmon pn;
  scfmon rn;
  if (Nrad < 2)
  {
    dn = Npure + Nrad;
    if (dn == hCo)
    {
      if (Nrad==0)
        hIndep(pure);
      else
      {
        pn = *rad;
        for (iv = Nvar; iv!=0; iv--)
        {
          x = var[iv];
          if (pn[x])
          {
            pure[x] = 1;
            hIndep(pure);
            pure[x] = 0;
          }
        }
      }
    }
    return;
  }
  iv = Nvar;
  dn = Npure+1;
  if (dn >= hCo)
  {
    if (dn > hCo)
      return;
    loop
    {
      if(!pure[var[iv]])
      {
        if(hNotZero(rad, Nrad, var, iv))
        {
          pure[var[iv]] = 1;
          hIndep(pure);
          pure[var[iv]] = 0;
        }
      }
      iv--;
      if (!iv)
        return;
    }
  }
  while(pure[var[iv]]) iv--;
  hStepR(rad, Nrad, var, iv, &rad0);
  iv--;
  if (rad0 < Nrad)
  {
    pn = hGetpure(pure);
    rn = hGetmem(Nrad, rad, radmem[iv]);
    pn[var[iv + 1]] = 1;
    hIndMult(pn, Npure + 1, rn, rad0, var, iv);
    pn[var[iv + 1]] = 0;
    b = rad0;
    c = Nrad;
    hElimR(rn, &rad0, b, c, var, iv);
    hPure(rn, b, &c, var, iv, pn, &x);
    hLex2R(rn, rad0, b, c, var, iv, hwork);
    rad0 += (c - b);
    hIndMult(pn, Npure + x, rn, rad0, var, iv);
  }
  else
  {
    hIndMult(pure, Npure, rad, Nrad, var, iv);
  }
}

/*3
* consider indset x := !pure
* (for all i) (if(sm(i) > x) return FALSE)
* else return TRUE
*/
static BOOLEAN hCheck1(indset sm, scmon pure)
{
  int iv;
  intvec *Set;
  while (sm->nx != NULL)
  {
    Set = sm->set;
    iv=(currRing->N);
    loop
    {
      if (((*Set)[iv-1] == 0) && (pure[iv] == 0))
        break;
      iv--;
      if (iv == 0)
        return FALSE;
    }
    sm = sm->nx;
  }
  return TRUE;
}

/*3
* consider indset x := !pure
* (for all i) if(x > sm(i)) delete sm(i))
* return (place for x)
*/
static indset hCheck2(indset sm, scmon pure)
{
  int iv;
  intvec *Set;
  indset be, a1 = NULL;
  while (sm->nx != NULL)
  {
    Set = sm->set;
    iv=(currRing->N);
    loop
    {
      if ((pure[iv] == 1) && ((*Set)[iv-1] == 1))
        break;
      iv--;
      if (iv == 0)
      {
        if (a1 == NULL)
        {
          a1 = sm;
        }
        else
        {
          hMu2--;
          be->nx = sm->nx;
          delete Set;
          omFreeBin((ADDRESS)sm, indlist_bin);
          sm = be;
        }
        break;
      }
    }
    be = sm;
    sm = sm->nx;
  }
  if (a1 != NULL)
  {
    return a1;
  }
  else
  {
    hMu2++;
    sm->set = new intvec((currRing->N));
    sm->nx = (indset)omAlloc0Bin(indlist_bin);
    return sm;
  }
}

/*2
*  definition x >= y
*      x(i) == 0 => y(i) == 0
*      > ex. j with x(j) == 1 and y(j) == 0
*/
static void hCheckIndep(scmon pure)
{
  intvec *Set;
  indset res;
  int iv;
  if (hCheck1(ISet, pure))
  {
    if (hCheck1(JSet, pure))
    {
      res = hCheck2(JSet,pure);
      if (res == NULL)
        return;
      Set = res->set;
      for (iv=(currRing->N); iv; iv--)
      {
        if (pure[iv])
          (*Set)[iv-1] = 0;
        else
          (*Set)[iv-1] = 1;
      }
    }
  }
}

void hIndAllMult(scmon pure, int Npure, scfmon rad, int Nrad,
 varset var, int Nvar)
{
  int  dn, iv, rad0, b, c, x;
  scmon pn;
  scfmon rn;
  if (Nrad < 2)
  {
    dn = Npure + Nrad;
    if (dn > hCo)
    {
      if (!Nrad)
        hCheckIndep(pure);
      else
      {
        pn = *rad;
        for (iv = Nvar; iv; iv--)
        {
          x = var[iv];
          if (pn[x])
          {
            pure[x] = 1;
            hCheckIndep(pure);
            pure[x] = 0;
          }
        }
      }
    }
    return;
  }
  iv = Nvar;
  while(pure[var[iv]]) iv--;
  hStepR(rad, Nrad, var, iv, &rad0);
  iv--;
  if (rad0 < Nrad)
  {
    pn = hGetpure(pure);
    rn = hGetmem(Nrad, rad, radmem[iv]);
    pn[var[iv + 1]] = 1;
    hIndAllMult(pn, Npure + 1, rn, rad0, var, iv);
    pn[var[iv + 1]] = 0;
    b = rad0;
    c = Nrad;
    hElimR(rn, &rad0, b, c, var, iv);
    hPure(rn, b, &c, var, iv, pn, &x);
    hLex2R(rn, rad0, b, c, var, iv, hwork);
    rad0 += (c - b);
    hIndAllMult(pn, Npure + x, rn, rad0, var, iv);
  }
  else
  {
    hIndAllMult(pure, Npure, rad, Nrad, var, iv);
  }
}

// multiplicity

static int hZeroMult(scmon pure, scfmon stc, int Nstc, varset var, int Nvar)
{
  int  iv = Nvar -1, sum, a, a0, a1, b, i;
  int  x, x0;
  scmon pn;
  scfmon sn;
  if (!iv)
    return pure[var[1]];
  else if (!Nstc)
  {
    sum = 1;
    for (i = Nvar; i; i--)
      sum *= pure[var[i]];
    return sum;
  }
  x = a = 0;
  pn = hGetpure(pure);
  sn = hGetmem(Nstc, stc, stcmem[iv]);
  hStepS(sn, Nstc, var, Nvar, &a, &x);
  if (a == Nstc)
    return pure[var[Nvar]] * hZeroMult(pn, sn, a, var, iv);
  else
    sum = x * hZeroMult(pn, sn, a, var, iv);
  b = a;
  loop
  {
    a0 = a;
    x0 = x;
    hStepS(sn, Nstc, var, Nvar, &a, &x);
    hElimS(sn, &b, a0, a, var, iv);
    a1 = a;
    hPure(sn, a0, &a1, var, iv, pn, &i);
    hLex2S(sn, b, a0, a1, var, iv, hwork);
    b += (a1 - a0);
    if (a < Nstc)
    {
      sum += (x - x0) * hZeroMult(pn, sn, b, var, iv);
    }
    else
    {
      sum += (pure[var[Nvar]] - x0) * hZeroMult(pn, sn, b, var, iv);
      return sum;
    }
  }
}

static void hProject(scmon pure, varset sel)
{
  int  i, i0, k;
  i0 = 0;
  for (i = 1; i <= (currRing->N); i++)
  {
    if (pure[i])
    {
      i0++;
      sel[i0] = i;
    }
  }
  i = hNstc;
  memcpy(hwork, hstc, i * sizeof(scmon));
  hStaircase(hwork, &i, sel, i0);
  if ((i0 > 2) && (i > 10))
    hOrdSupp(hwork, i, sel, i0);
  memset(hpur0, 0, ((currRing->N) + 1) * sizeof(int));
  hPure(hwork, 0, &i, sel, i0, hpur0, &k);
  hLexS(hwork, i, sel, i0);
  hMu += hZeroMult(hpur0, hwork, i, sel, i0);
}

static void hDimMult(scmon pure, int Npure, scfmon rad, int Nrad,
 varset var, int Nvar)
{
  int  dn, iv, rad0, b, c, x;
  scmon pn;
  scfmon rn;
  if (Nrad < 2)
  {
    dn = Npure + Nrad;
    if (dn == hCo)
    {
      if (!Nrad)
        hProject(pure, hsel);
      else
      {
        pn = *rad;
        for (iv = Nvar; iv; iv--)
        {
          x = var[iv];
          if (pn[x])
          {
            pure[x] = 1;
            hProject(pure, hsel);
            pure[x] = 0;
          }
        }
      }
    }
    return;
  }
  iv = Nvar;
  dn = Npure+1;
  if (dn >= hCo)
  {
    if (dn > hCo)
      return;
    loop
    {
      if(!pure[var[iv]])
      {
        if(hNotZero(rad, Nrad, var, iv))
        {
          pure[var[iv]] = 1;
          hProject(pure, hsel);
          pure[var[iv]] = 0;
        }
      }
      iv--;
      if (!iv)
        return;
    }
  }
  while(pure[var[iv]]) iv--;
  hStepR(rad, Nrad, var, iv, &rad0);
  iv--;
  if (rad0 < Nrad)
  {
    pn = hGetpure(pure);
    rn = hGetmem(Nrad, rad, radmem[iv]);
    pn[var[iv + 1]] = 1;
    hDimMult(pn, Npure + 1, rn, rad0, var, iv);
    pn[var[iv + 1]] = 0;
    b = rad0;
    c = Nrad;
    hElimR(rn, &rad0, b, c, var, iv);
    hPure(rn, b, &c, var, iv, pn, &x);
    hLex2R(rn, rad0, b, c, var, iv, hwork);
    rad0 += (c - b);
    hDimMult(pn, Npure + x, rn, rad0, var, iv);
  }
  else
  {
    hDimMult(pure, Npure, rad, Nrad, var, iv);
  }
}

static void hDegree(ideal S, ideal Q)
{
  id_Test(S, currRing);
  if( Q!=NULL ) id_Test(Q, currRing);

  int  di;
  int  mc;
  hexist = hInit(S, Q, &hNexist, currRing);
  if (!hNexist)
  {
    hCo = 0;
    hMu = 1;
    return;
  }
  //hWeight();
  hwork = (scfmon)omAlloc(hNexist * sizeof(scmon));
  hvar = (varset)omAlloc(((currRing->N) + 1) * sizeof(int));
  hsel = (varset)omAlloc(((currRing->N) + 1) * sizeof(int));
  hpure = (scmon)omAlloc((1 + ((currRing->N) * (currRing->N))) * sizeof(int));
  hpur0 = (scmon)omAlloc((1 + ((currRing->N) * (currRing->N))) * sizeof(int));
  mc = hisModule;
  hrad = (scfmon)omAlloc(hNexist * sizeof(scmon));
  if (!mc)
  {
    memcpy(hrad, hexist, hNexist * sizeof(scmon));
    hstc = hexist;
    hNrad = hNstc = hNexist;
  }
  else
    hstc = (scfmon)omAlloc(hNexist * sizeof(scmon));
  radmem = hCreate((currRing->N) - 1);
  stcmem = hCreate((currRing->N) - 1);
  hCo = (currRing->N) + 1;
  di = hCo + 1;
  loop
  {
    if (mc)
    {
      hComp(hexist, hNexist, mc, hrad, &hNrad);
      hNstc = hNrad;
      memcpy(hstc, hrad, hNrad * sizeof(scmon));
    }
    if (hNrad)
    {
      hNvar = (currRing->N);
      hRadical(hrad, &hNrad, hNvar);
      hSupp(hrad, hNrad, hvar, &hNvar);
      if (hNvar)
      {
        hCo = hNvar;
        memset(hpure, 0, ((currRing->N) + 1) * sizeof(int));
        hPure(hrad, 0, &hNrad, hvar, hNvar, hpure, &hNpure);
        hLexR(hrad, hNrad, hvar, hNvar);
        hDimSolve(hpure, hNpure, hrad, hNrad, hvar, hNvar);
      }
    }
    else
    {
      hNvar = 1;
      hCo = 0;
    }
    if (hCo < di)
    {
      di = hCo;
      hMu = 0;
    }
    if (hNvar && (hCo == di))
    {
      if (di && (di < (currRing->N)))
        hDimMult(hpure, hNpure, hrad, hNrad, hvar, hNvar);
      else if (!di)
        hMu++;
      else
      {
        hStaircase(hstc, &hNstc, hvar, hNvar);
        if ((hNvar > 2) && (hNstc > 10))
          hOrdSupp(hstc, hNstc, hvar, hNvar);
        memset(hpur0, 0, ((currRing->N) + 1) * sizeof(int));
        hPure(hstc, 0, &hNstc, hvar, hNvar, hpur0, &hNpure);
        hLexS(hstc, hNstc, hvar, hNvar);
        hMu += hZeroMult(hpur0, hstc, hNstc, hvar, hNvar);
      }
    }
    mc--;
    if (mc <= 0)
      break;
  }
  hCo = di;
  hKill(stcmem, (currRing->N) - 1);
  hKill(radmem, (currRing->N) - 1);
  omFreeSize((ADDRESS)hpur0, (1 + ((currRing->N) * (currRing->N))) * sizeof(int));
  omFreeSize((ADDRESS)hpure, (1 + ((currRing->N) * (currRing->N))) * sizeof(int));
  omFreeSize((ADDRESS)hsel, ((currRing->N) + 1) * sizeof(int));
  omFreeSize((ADDRESS)hvar, ((currRing->N) + 1) * sizeof(int));
  omFreeSize((ADDRESS)hwork, hNexist * sizeof(scmon));
  omFreeSize((ADDRESS)hrad, hNexist * sizeof(scmon));
  hDelete(hexist, hNexist);
  if (hisModule)
    omFreeSize((ADDRESS)hstc, hNexist * sizeof(scmon));
}

int  scMultInt(ideal S, ideal Q)
{
  id_Test(S, currRing);
  if( Q!=NULL ) id_Test(Q, currRing);

  hDegree(S, Q);
  return hMu;
}

void scPrintDegree(int co, int mu)
{
  int di = (currRing->N)-co;
  if (currRing->OrdSgn == 1)
  {
    if (di>0)
      Print("// dimension (proj.)  = %d\n// degree (proj.)   = %d\n", di-1, mu);
    else
      Print("// dimension (affine) = 0\n// degree (affine)  = %d\n",       mu);
  }
  else
    Print("// dimension (local)   = %d\n// multiplicity = %d\n", di, mu);
}

void scDegree(ideal S, intvec *modulweight, ideal Q)
{
  id_Test(S, currRing);
  if( Q!=NULL ) id_Test(Q, currRing);

  int co, mu, l;
  intvec *hseries2;
  intvec *hseries1 = hFirstSeries(S, modulweight, Q);
  l = hseries1->length()-1;
  if (l > 1)
    hseries2 = hSecondSeries(hseries1);
  else
    hseries2 = hseries1;
  hDegreeSeries(hseries1, hseries2, &co, &mu);
  if ((l == 1) &&(mu == 0))
    scPrintDegree((currRing->N)+1, 0);
  else
    scPrintDegree(co, mu);
  if (l>1)
    delete hseries1;
  delete hseries2;
}

static void hDegree0(ideal S, ideal Q, const ring tailRing)
{
  id_TestTail(S, currRing, tailRing);
  if (Q!=NULL) id_TestTail(Q, currRing, tailRing);

  int  mc;
  hexist = hInit(S, Q, &hNexist, tailRing);
  if (!hNexist)
  {
    hMu = -1;
    return;
  }
  else
    hMu = 0;

  const ring r = currRing;

  hwork = (scfmon)omAlloc(hNexist * sizeof(scmon));
  hvar = (varset)omAlloc(((r->N) + 1) * sizeof(int));
  hpur0 = (scmon)omAlloc((1 + ((r->N) * (r->N))) * sizeof(int));
  mc = hisModule;
  if (!mc)
  {
    hstc = hexist;
    hNstc = hNexist;
  }
  else
    hstc = (scfmon)omAlloc(hNexist * sizeof(scmon));
  stcmem = hCreate((r->N) - 1);
  loop
  {
    if (mc)
    {
      hComp(hexist, hNexist, mc, hstc, &hNstc);
      if (!hNstc)
      {
        hMu = -1;
        break;
      }
    }
    hNvar = (r->N);
    for (int i = hNvar; i; i--)
      hvar[i] = i;
    hStaircase(hstc, &hNstc, hvar, hNvar);
    hSupp(hstc, hNstc, hvar, &hNvar);
    if ((hNvar == (r->N)) && (hNstc >= (r->N)))
    {
      if ((hNvar > 2) && (hNstc > 10))
        hOrdSupp(hstc, hNstc, hvar, hNvar);
      memset(hpur0, 0, ((r->N) + 1) * sizeof(int));
      hPure(hstc, 0, &hNstc, hvar, hNvar, hpur0, &hNpure);
      if (hNpure == hNvar)
      {
        hLexS(hstc, hNstc, hvar, hNvar);
        hMu += hZeroMult(hpur0, hstc, hNstc, hvar, hNvar);
      }
      else
        hMu = -1;
    }
    else if (hNvar)
      hMu = -1;
    mc--;
    if (mc <= 0 || hMu < 0)
      break;
  }
  hKill(stcmem, (r->N) - 1);
  omFreeSize((ADDRESS)hpur0, (1 + ((r->N) * (r->N))) * sizeof(int));
  omFreeSize((ADDRESS)hvar, ((r->N) + 1) * sizeof(int));
  omFreeSize((ADDRESS)hwork, hNexist * sizeof(scmon));
  hDelete(hexist, hNexist);
  if (hisModule)
    omFreeSize((ADDRESS)hstc, hNexist * sizeof(scmon));
}

int  scMult0Int(ideal S, ideal Q, const ring tailRing)
{
  id_TestTail(S, currRing, tailRing);
  if (Q!=NULL) id_TestTail(Q, currRing, tailRing);

  hDegree0(S, Q, tailRing);
  return hMu;
}


// HC

STATIC_VAR poly pWork;

static void hHedge(poly hEdge)
{
  pSetm(pWork);
  if (pLmCmp(pWork, hEdge) == currRing->OrdSgn)
  {
    for (int i = hNvar; i>0; i--)
      pSetExp(hEdge,i, pGetExp(pWork,i));
    pSetm(hEdge);
  }
}


static void hHedgeStep(scmon pure, scfmon stc,
                       int Nstc, varset var, int Nvar,poly hEdge)
{
  int  iv = Nvar -1, k = var[Nvar], a, a0, a1, b, i;
  int  x/*, x0*/;
  scmon pn;
  scfmon sn;
  if (iv==0)
  {
    pSetExp(pWork, k, pure[k]);
    hHedge(hEdge);
    return;
  }
  else if (Nstc==0)
  {
    for (i = Nvar; i>0; i--)
      pSetExp(pWork, var[i], pure[var[i]]);
    hHedge(hEdge);
    return;
  }
  x = a = 0;
  pn = hGetpure(pure);
  sn = hGetmem(Nstc, stc, stcmem[iv]);
  hStepS(sn, Nstc, var, Nvar, &a, &x);
  if (a == Nstc)
  {
    pSetExp(pWork, k, pure[k]);
    hHedgeStep(pn, sn, a, var, iv,hEdge);
    return;
  }
  else
  {
    pSetExp(pWork, k, x);
    hHedgeStep(pn, sn, a, var, iv,hEdge);
  }
  b = a;
  loop
  {
    a0 = a;
    // x0 = x;
    hStepS(sn, Nstc, var, Nvar, &a, &x);
    hElimS(sn, &b, a0, a, var, iv);
    a1 = a;
    hPure(sn, a0, &a1, var, iv, pn, &i);
    hLex2S(sn, b, a0, a1, var, iv, hwork);
    b += (a1 - a0);
    if (a < Nstc)
    {
      pSetExp(pWork, k, x);
      hHedgeStep(pn, sn, b, var, iv,hEdge);
    }
    else
    {
      pSetExp(pWork, k, pure[k]);
      hHedgeStep(pn, sn, b, var, iv,hEdge);
      return;
    }
  }
}

void scComputeHC(ideal S, ideal Q, int ak, poly &hEdge, ring tailRing)
{
  id_TestTail(S, currRing, tailRing);
  if (Q!=NULL) id_TestTail(Q, currRing, tailRing);

  int  i;
  int  k = ak;
  #if HAVE_RINGS
  if (rField_is_Ring(currRing) && (currRing->OrdSgn == -1))
  {
    //consider just monic generators (over rings with zero-divisors)
    ideal SS=id_Copy(S,tailRing);
    for(i=0;i<=idElem(S);i++)
    {
      if((SS->m[i]!=NULL)
      && ((p_IsPurePower(SS->m[i],tailRing)==0)
        ||(!n_IsUnit(pGetCoeff(SS->m[i]), tailRing->cf))))
      {
        p_Delete(&SS->m[i],tailRing);
      }
    }
    S=id_Copy(SS,tailRing);
    idSkipZeroes(S);
  }
  #if 0
  printf("\nThis is HC:\n");
  for(int ii=0;ii<=idElem(S);ii++)
  {
    pWrite(S->m[ii]);
  }
  //getchar();
  #endif
  #endif
  if(idElem(S) == 0)
    return;
  hNvar = (currRing->N);
  hexist = hInit(S, Q, &hNexist, tailRing); // tailRing?
  if (k!=0)
    hComp(hexist, hNexist, k, hexist, &hNstc);
  else
    hNstc = hNexist;
  assume(hNexist > 0);
  hwork = (scfmon)omAlloc(hNexist * sizeof(scmon));
  hvar = (varset)omAlloc((hNvar + 1) * sizeof(int));
  hpure = (scmon)omAlloc((1 + (hNvar * hNvar)) * sizeof(int));
  stcmem = hCreate(hNvar - 1);
  for (i = hNvar; i>0; i--)
    hvar[i] = i;
  hStaircase(hexist, &hNstc, hvar, hNvar);
  if ((hNvar > 2) && (hNstc > 10))
    hOrdSupp(hexist, hNstc, hvar, hNvar);
  memset(hpure, 0, (hNvar + 1) * sizeof(int));
  hPure(hexist, 0, &hNstc, hvar, hNvar, hpure, &hNpure);
  hLexS(hexist, hNstc, hvar, hNvar);
  if (hEdge!=NULL)
    pLmFree(hEdge);
  hEdge = pInit();
  pWork = pInit();
  hHedgeStep(hpure, hexist, hNstc, hvar, hNvar,hEdge);
  pSetComp(hEdge,ak);
  hKill(stcmem, hNvar - 1);
  omFreeSize((ADDRESS)hwork, hNexist * sizeof(scmon));
  omFreeSize((ADDRESS)hvar, (hNvar + 1) * sizeof(int));
  omFreeSize((ADDRESS)hpure, (1 + (hNvar * hNvar)) * sizeof(int));
  hDelete(hexist, hNexist);
  pLmFree(pWork);
}



//  kbase

STATIC_VAR poly last;
STATIC_VAR scmon act;

static void scElKbase()
{
  poly q = pInit();
  pSetCoeff0(q,nInit(1));
  pSetExpV(q,act);
  pNext(q) = NULL;
  last = pNext(last) = q;
}

static int scMax( int i, scfmon stc, int Nvar)
{
  int x, y=stc[0][Nvar];
  for (; i;)
  {
    i--;
    x = stc[i][Nvar];
    if (x > y) y = x;
  }
  return y;
}

static int scMin( int i, scfmon stc, int Nvar)
{
  int x, y=stc[0][Nvar];
  for (; i;)
  {
    i--;
    x = stc[i][Nvar];
    if (x < y) y = x;
  }
  return y;
}

static int scRestrict( int &Nstc, scfmon stc, int Nvar)
{
  int x, y;
  int i, j, Istc = Nstc;

  y = MAX_INT_VAL;
  for (i=Nstc-1; i>=0; i--)
  {
    j = Nvar-1;
    loop
    {
      if(stc[i][j] != 0) break;
      j--;
      if (j == 0)
      {
        Istc--;
        x = stc[i][Nvar];
        if (x < y) y = x;
        stc[i] = NULL;
        break;
      }
    }
  }
  if (Istc < Nstc)
  {
    for (i=Nstc-1; i>=0; i--)
    {
      if (stc[i] && (stc[i][Nvar] >= y))
      {
        Istc--;
        stc[i] = NULL;
      }
    }
    j = 0;
    while (stc[j]) j++;
    i = j+1;
    for(; i<Nstc; i++)
    {
      if (stc[i])
      {
        stc[j] = stc[i];
        j++;
      }
    }
    Nstc = Istc;
    return y;
  }
  else
    return -1;
}

static void scAll( int Nvar, int deg)
{
  int i;
  int d = deg;
  if (d == 0)
  {
    for (i=Nvar; i; i--) act[i] = 0;
    scElKbase();
    return;
  }
  if (Nvar == 1)
  {
    act[1] = d;
    scElKbase();
    return;
  }
  do
  {
    act[Nvar] = d;
    scAll(Nvar-1, deg-d);
    d--;
  } while (d >= 0);
}

static void scAllKbase( int Nvar, int ideg, int deg)
{
  do
  {
    act[Nvar] = ideg;
    scAll(Nvar-1, deg-ideg);
    ideg--;
  } while (ideg >= 0);
}

static void scDegKbase( scfmon stc, int Nstc, int Nvar, int deg)
{
  int  Ivar, Istc, i, j;
  scfmon sn;
  int x, ideg;

  if (deg == 0)
  {
    for (i=Nstc-1; i>=0; i--)
    {
      for (j=Nvar;j;j--){ if(stc[i][j]) break; }
      if (j==0){return;}
    }
    for (i=Nvar; i; i--) act[i] = 0;
    scElKbase();
    return;
  }
  if (Nvar == 1)
  {
    for (i=Nstc-1; i>=0; i--) if(deg >= stc[i][1]) return;
    act[1] = deg;
    scElKbase();
    return;
  }
  Ivar = Nvar-1;
  sn = hGetmem(Nstc, stc, stcmem[Ivar]);
  x = scRestrict(Nstc, sn, Nvar);
  if (x <= 0)
  {
    if (x == 0) return;
    ideg = deg;
  }
  else
  {
    if (deg < x) ideg = deg;
    else ideg = x-1;
    if (Nstc == 0)
    {
      scAllKbase(Nvar, ideg, deg);
      return;
    }
  }
  loop
  {
    x = scMax(Nstc, sn, Nvar);
    while (ideg >= x)
    {
      act[Nvar] = ideg;
      scDegKbase(sn, Nstc, Ivar, deg-ideg);
      ideg--;
    }
    if (ideg < 0) return;
    Istc = Nstc;
    for (i=Nstc-1; i>=0; i--)
    {
      if (ideg < sn[i][Nvar])
      {
        Istc--;
        sn[i] = NULL;
      }
    }
    if (Istc == 0)
    {
      scAllKbase(Nvar, ideg, deg);
      return;
    }
    j = 0;
    while (sn[j]) j++;
    i = j+1;
    for (; i<Nstc; i++)
    {
      if (sn[i])
      {
        sn[j] = sn[i];
        j++;
      }
    }
    Nstc = Istc;
  }
}

static void scInKbase( scfmon stc, int Nstc, int Nvar)
{
  int  Ivar, Istc, i, j;
  scfmon sn;
  int x, ideg;

  if (Nvar == 1)
  {
    ideg = scMin(Nstc, stc, 1);
    while (ideg > 0)
    {
      ideg--;
      act[1] = ideg;
      scElKbase();
    }
    return;
  }
  Ivar = Nvar-1;
  sn = hGetmem(Nstc, stc, stcmem[Ivar]);
  x = scRestrict(Nstc, sn, Nvar);
  if (x == 0) return;
  ideg = x-1;
  loop
  {
    x = scMax(Nstc, sn, Nvar);
    while (ideg >= x)
    {
      act[Nvar] = ideg;
      scInKbase(sn, Nstc, Ivar);
      ideg--;
    }
    if (ideg < 0) return;
    Istc = Nstc;
    for (i=Nstc-1; i>=0; i--)
    {
      if (ideg < sn[i][Nvar])
      {
        Istc--;
        sn[i] = NULL;
      }
    }
    j = 0;
    while (sn[j]) j++;
    i = j+1;
    for (; i<Nstc; i++)
    {
      if (sn[i])
      {
        sn[j] = sn[i];
        j++;
      }
    }
    Nstc = Istc;
  }
}

static ideal scIdKbase(poly q, const int rank)
{
  ideal res = idInit(pLength(q), rank);
  polyset mm = res->m;
  do
  {
    *mm = q; ++mm;

    const poly p = pNext(q);
    pNext(q) = NULL;
    q = p;

  } while (q!=NULL);

  id_Test(res, currRing);   // WRONG RANK!!!???
  return res;
}

ideal scKBase(int deg, ideal s, ideal Q, intvec * mv)
{
  if( Q!=NULL) id_Test(Q, currRing);

  int  i, di;
  poly p;

  if (deg < 0)
  {
    di = scDimInt(s, Q);
    if (di != 0)
    {
      //Werror("KBase not finite");
      return idInit(1,s->rank);
    }
  }
  stcmem = hCreate((currRing->N) - 1);
  hexist = hInit(s, Q, &hNexist, currRing);
  p = last = pInit();
  /*pNext(p) = NULL;*/
  act = (scmon)omAlloc(((currRing->N) + 1) * sizeof(int));
  *act = 0;
  if (!hNexist)
  {
    scAll((currRing->N), deg);
    goto ende;
  }
  if (!hisModule)
  {
    if (deg < 0) scInKbase(hexist, hNexist, (currRing->N));
    else scDegKbase(hexist, hNexist, (currRing->N), deg);
  }
  else
  {
    hstc = (scfmon)omAlloc(hNexist * sizeof(scmon));
    for (i = 1; i <= hisModule; i++)
    {
      *act = i;
      hComp(hexist, hNexist, i, hstc, &hNstc);
      int deg_ei=deg;
      if (mv!=NULL) deg_ei -= (*mv)[i-1];
      if ((deg < 0) || (deg_ei>=0))
      {
        if (hNstc)
        {
          if (deg < 0) scInKbase(hstc, hNstc, (currRing->N));
          else scDegKbase(hstc, hNstc, (currRing->N), deg_ei);
        }
        else
          scAll((currRing->N), deg_ei);
      }
    }
    omFreeSize((ADDRESS)hstc, hNexist * sizeof(scmon));
  }
ende:
  hDelete(hexist, hNexist);
  omFreeSize((ADDRESS)act, ((currRing->N) + 1) * sizeof(int));
  hKill(stcmem, (currRing->N) - 1);
  pLmFree(&p);
  if (p == NULL)
    return idInit(1,s->rank);

  last = p;
  return scIdKbase(p, s->rank);
}

#if 0 //-- alternative implementation of scComputeHC
/*
void scComputeHCw(ideal ss, ideal Q, int ak, poly &hEdge, ring tailRing)
{
  id_TestTail(ss, currRing, tailRing);
  if (Q!=NULL) id_TestTail(Q, currRing, tailRing);

  int  i, di;
  poly p;

  if (hEdge!=NULL)
    pLmFree(hEdge);

  ideal s=idInit(IDELEMS(ss),ak);
  for(i=IDELEMS(ss)-1;i>=0;i--)
  {
    if (ss->m[i]!=NULL) s->m[i]=pHead(ss->m[i]);
  }
  di = scDimInt(s, Q);
  stcmem = hCreate((currRing->N) - 1);
  hexist = hInit(s, Q, &hNexist, currRing);
  p = last = pInit();
  // pNext(p) = NULL;
  act = (scmon)omAlloc(((currRing->N) + 1) * sizeof(int));
  *act = 0;
  if (!hNexist)
  {
    scAll((currRing->N), -1);
    goto ende;
  }
  if (!hisModule)
  {
    scInKbase(hexist, hNexist, (currRing->N));
  }
  else
  {
    hstc = (scfmon)omAlloc(hNexist * sizeof(scmon));
    for (i = 1; i <= hisModule; i++)
    {
      *act = i;
      hComp(hexist, hNexist, i, hstc, &hNstc);
      if (hNstc)
      {
        scInKbase(hstc, hNstc, (currRing->N));
      }
      else
        scAll((currRing->N), -1);
    }
    omFreeSize((ADDRESS)hstc, hNexist * sizeof(scmon));
  }
ende:
  hDelete(hexist, hNexist);
  omFreeSize((ADDRESS)act, ((currRing->N) + 1) * sizeof(int));
  hKill(stcmem, (currRing->N) - 1);
  pDeleteLm(&p);
  idDelete(&s);
  if (p == NULL)
  {
    return; // no HEdge
  }
  else
  {
    last = p;
    ideal res=scIdKbase(p, ss->rank);
    poly p_ind=res->m[0]; int ind=0;
    for(i=IDELEMS(res)-1;i>0;i--)
    {
      if (pCmp(res->m[i],p_ind)==-1) { p_ind=res->m[i]; ind=i; }
    }
    assume(p_ind!=NULL);
    assume(res->m[ind]==p_ind);
    hEdge=p_ind;
    res->m[ind]=NULL;
    nDelete(&pGetCoeff(hEdge));
    pGetCoeff(hEdge)=NULL;
    for(i=(currRing->N);i>0;i--)
      pIncrExp(hEdge,i);
    pSetm(hEdge);

    idDelete(&res);
    return;
  }
}
 */
#endif

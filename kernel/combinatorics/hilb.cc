/****************************************
*  Computer Algebra System SINGULAR     *
****************************************/
/*
*  ABSTRACT -  Hilbert series
*/

#include "kernel/mod2.h"

#include "misc/mylimits.h"
#include "misc/intvec.h"

#include "kernel/combinatorics/hilb.h"
#include "kernel/combinatorics/stairc.h"
#include "kernel/combinatorics/hutil.h"

#include "polys/monomials/ring.h"
#include "polys/monomials/p_polys.h"
#include "polys/simpleideals.h"


// #include "kernel/structs.h"
// #include "kernel/polys.h"
//ADICHANGES:
#include "kernel/ideals.h"
// #include "kernel/GBEngine/kstd1.h"

# define omsai 1
#if omsai

#include "polys/ext_fields/transext.h"
#include "coeffs/coeffs.h"
#include "kernel/linear_algebra/linearAlgebra.h"
#include "coeffs/numbers.h"
#include <vector>
#include "Singular/ipshell.h"


#include <Singular/ipshell.h>
#include <ctime>
#include <iostream>
#endif

THREAD_VAR static int  **Qpol;
THREAD_VAR static int  *Q0, *Ql;
THREAD_VAR static int  hLength;


static int hMinModulweight(intvec *modulweight)
{
  int i,j,k;

  if(modulweight==NULL) return 0;
  j=(*modulweight)[0];
  for(i=modulweight->rows()-1;i!=0;i--)
  {
    k=(*modulweight)[i];
    if(k<j) j=k;
  }
  return j;
}

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
      if (z>(MAX_INT_VAL)/2)
      {
       WerrorS("internal arrays too big");
       return;
      }
      p = (int *)omAlloc((unsigned long)z * sizeof(int));
      if (Ql[j]!=0)
      {
        if (j==0)
          memcpy(p, Qpol[j], Ql[j] * sizeof(int));
        omFreeSize((ADDRESS)Qpol[j], Ql[j] * sizeof(int));
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
  {/*pon[i] -= pol[i - x];*/
    for (i = x; i < l; i++)
    { int64 t=pon[i];
      int64 t2=pol[i - x];
      t-=t2;
      if ((t>=INT_MIN)&&(t<=INT_MAX)) pon[i]=t;
      else if (!errorreported) WerrorS("int overflow in hilb 1");
    }
    for (i = l; i < ln; i++)
    { /*pon[i] = -pol[i - x];*/
      int64 t= -pol[i - x];
      if ((t>=INT_MIN)&&(t<=INT_MAX)) pon[i]=t;
      else if (!errorreported) WerrorS("int overflow in hilb 2");
    }
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
  int  *pl;
  int  *p;
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
  { /* pl[i + j] += p[i];*/
    int64 t=pl[i+j];
    int64 t2=p[i];
    t+=t2;
    if ((t>=INT_MIN)&&(t<=INT_MAX)) pl[i+j]=t;
    else if (!errorreported) WerrorS("int overflow in hilb 3");
  }
  x = pure[var[1]];
  if (x!=0)
  {
    j += x;
    for (i = 0; i < l; i++)
    { /* pl[i + j] -= p[i];*/
      int64 t=pl[i+j];
      int64 t2=p[i];
      t-=t2;
      if ((t>=INT_MIN)&&(t<=INT_MAX)) pl[i+j]=t;
      else if (!errorreported) WerrorS("int overflow in hilb 4");
    }
  }
  j += l;
  if (j > hLength)
    hLength = j;
}

static void hHilbStep(scmon pure, scfmon stc, int Nstc, varset var,
 int Nvar, int *pol, int Lpol)
{
  int  iv = Nvar -1, ln, a, a0, a1, b, i;
  int  x, x0;
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
static void hWDegree(intvec *wdegree)
{
  int i, k;
  int x;

  for (i=(currRing->N); i; i--)
  {
    x = (*wdegree)[i-1];
    if (x != 1)
    {
      for (k=hNexist-1; k>=0; k--)
      {
        hexist[k][i] *= x;
      }
    }
  }
}
// ---------------------------------- ADICHANGES ---------------------------------------------
//!!!!!!!!!!!!!!!!!!!!! Just for Monomial Ideals !!!!!!!!!!!!!!!!!!!!!!!!!!!!

#if 0 // unused
//Tests if the ideal is sorted by degree
static bool idDegSortTest(ideal I)
{
    if((I == NULL)||(idIs0(I)))
    {
        return(TRUE);
    }
    for(int i = 0; i<IDELEMS(I)-1; i++)
    {
        if(p_Totaldegree(I->m[i],currRing)>p_Totaldegree(I->m[i+1],currRing))
        {
            idPrint(I);
            WerrorS("Ideal is not deg sorted!!");
            return(FALSE);
        }
    }
    return(TRUE);
}
#endif

//adds the new polynomial at the coresponding position
//and simplifies the ideal, destroys p
static void SortByDeg_p(ideal I, poly p)
{
  int i,j;
  if(idIs0(I))
  {
    I->m[0] = p;
    return;
  }
  idSkipZeroes(I);
  #if 1
  for(i = 0; (i<IDELEMS(I)) && (p_Totaldegree(I->m[i],currRing)<=p_Totaldegree(p,currRing)); i++)
  {
    if(p_DivisibleBy( I->m[i],p, currRing))
    {
      p_Delete(&p,currRing);
      return;
    }
  }
  for(i = IDELEMS(I)-1; (i>=0) && (p_Totaldegree(I->m[i],currRing)>=p_Totaldegree(p,currRing)); i--)
  {
    if(p_DivisibleBy(p,I->m[i], currRing))
    {
      p_Delete(&I->m[i],currRing);
    }
  }
  if(idIs0(I))
  {
    idSkipZeroes(I);
    I->m[0] = p;
    return;
  }
  #endif
  idSkipZeroes(I);
  //First I take the case when all generators have the same degree
  if(p_Totaldegree(I->m[0],currRing) == p_Totaldegree(I->m[IDELEMS(I)-1],currRing))
  {
    if(p_Totaldegree(p,currRing)<p_Totaldegree(I->m[0],currRing))
    {
      idInsertPoly(I,p);
      idSkipZeroes(I);
      for(i=IDELEMS(I)-1;i>=1; i--)
      {
        I->m[i] = I->m[i-1];
      }
      I->m[0] = p;
      return;
    }
    if(p_Totaldegree(p,currRing)>=p_Totaldegree(I->m[IDELEMS(I)-1],currRing))
    {
      idInsertPoly(I,p);
      idSkipZeroes(I);
      return;
    }
  }
  if(p_Totaldegree(p,currRing)<=p_Totaldegree(I->m[0],currRing))
  {
    idInsertPoly(I,p);
    idSkipZeroes(I);
    for(i=IDELEMS(I)-1;i>=1; i--)
    {
      I->m[i] = I->m[i-1];
    }
    I->m[0] = p;
    return;
  }
  if(p_Totaldegree(p,currRing)>=p_Totaldegree(I->m[IDELEMS(I)-1],currRing))
  {
    idInsertPoly(I,p);
    idSkipZeroes(I);
    return;
  }
  for(i = IDELEMS(I)-2; ;)
  {
    if(p_Totaldegree(p,currRing)==p_Totaldegree(I->m[i],currRing))
    {
      idInsertPoly(I,p);
      idSkipZeroes(I);
      for(j = IDELEMS(I)-1; j>=i+1;j--)
      {
        I->m[j] = I->m[j-1];
      }
      I->m[i] = p;
      return;
    }
    if(p_Totaldegree(p,currRing)>p_Totaldegree(I->m[i],currRing))
    {
      idInsertPoly(I,p);
      idSkipZeroes(I);
      for(j = IDELEMS(I)-1; j>=i+2;j--)
      {
        I->m[j] = I->m[j-1];
      }
      I->m[i+1] = p;
      return;
    }
    i--;
  }
}

//it sorts the ideal by the degrees
static ideal SortByDeg(ideal I)
{
  if(idIs0(I))
  {
    return id_Copy(I,currRing);
  }
  int i;
  ideal res;
  idSkipZeroes(I);
  res = idInit(1,1);
  for(i = 0; i<=IDELEMS(I)-1;i++)
  {
    SortByDeg_p(res, I->m[i]);
    I->m[i]=NULL; // I->m[i] is now in res
  }
  idSkipZeroes(res);
  //idDegSortTest(res);
  return(res);
}

//idQuot(I,p) for I monomial ideal, p a ideal with a single monomial.
ideal idQuotMon(ideal Iorig, ideal p)
{
    if(idIs0(Iorig))
    {
      ideal res = idInit(1,1);
      res->m[0] = poly(0);
      return(res);
    }
    if(idIs0(p))
    {
      ideal res = idInit(1,1);
      res->m[0] = pOne();
      return(res);
    }
    ideal I = id_Head(Iorig,currRing);
    ideal res = idInit(IDELEMS(I),1);
    int i,j;
    int dummy;
    for(i = 0; i<IDELEMS(I); i++)
    {
      res->m[i] = p_Head(I->m[i], currRing);
      for(j = 1; (j<=currRing->N) ; j++)
      {
        dummy = p_GetExp(p->m[0], j, currRing);
        if(dummy > 0)
        {
          if(p_GetExp(I->m[i], j, currRing) < dummy)
          {
            p_SetExp(res->m[i], j, 0, currRing);
          }
          else
          {
            p_SetExp(res->m[i], j, p_GetExp(I->m[i], j, currRing) - dummy, currRing);
          }
        }
      }
      p_Setm(res->m[i], currRing);
      if(p_Totaldegree(res->m[i],currRing) == p_Totaldegree(I->m[i],currRing))
      {
        p_Delete(&res->m[i],currRing);
      }
      else
      {
        p_Delete(&I->m[i],currRing);
      }
    }
    idSkipZeroes(res);
    idSkipZeroes(I);
    if(!idIs0(res))
    {
      for(i = 0; i<=IDELEMS(res)-1; i++)
      {
        SortByDeg_p(I,res->m[i]);
        res->m[i]=NULL; // is now in I
      }
    }
    id_Delete(&res,currRing);
    //idDegSortTest(I);
    return(I);
}

//id_Add for monomials
static void idAddMon(ideal I, ideal p)
{
  SortByDeg_p(I,p->m[0]);
  p->m[0]=NULL; // is now in I
  //idSkipZeroes(I);
}

//searches for a variable that is not yet used (assumes that the ideal is sqrfree)
static poly ChoosePVar (ideal I)
{
    bool flag=TRUE;
    int i,j;
    poly res;
    for(i=1;i<=currRing->N;i++)
    {
        flag=TRUE;
        for(j=IDELEMS(I)-1;(j>=0)&&(flag);j--)
        {
            if(p_GetExp(I->m[j], i, currRing)>0)
            {
                flag=FALSE;
            }
        }

        if(flag == TRUE)
        {
            res = p_ISet(1, currRing);
            p_SetExp(res, i, 1, currRing);
            p_Setm(res,currRing);
            return(res);
        }
        else
        {
            p_Delete(&res, currRing);
        }
    }
    return(NULL); //i.e. it is the maximal ideal
}

#if 0 // unused
//choice XL: last entry divided by x (xy10z15 -> y9z14)
static poly ChoosePXL(ideal I)
{
    int i,j,dummy=0;
    poly m;
    for(i = IDELEMS(I)-1; (i>=0) && (dummy == 0); i--)
    {
        for(j = 1; (j<=currRing->N) && (dummy == 0); j++)
        {
            if(p_GetExp(I->m[i],j, currRing)>1)
            {
                dummy = 1;
            }
        }
    }
    m = p_Copy(I->m[i+1],currRing);
    for(j = 1; j<=currRing->N; j++)
    {
        dummy = p_GetExp(m,j,currRing);
        if(dummy >= 1)
        {
            p_SetExp(m, j, dummy-1, currRing);
        }
    }
    if(!p_IsOne(m, currRing))
    {
        p_Setm(m, currRing);
        return(m);
    }
    m = ChoosePVar(I);
    return(m);
}
#endif

#if 0 // unused
//choice XF: first entry divided by x (xy10z15 -> y9z14)
static poly ChoosePXF(ideal I)
{
    int i,j,dummy=0;
    poly m;
    for(i =0 ; (i<=IDELEMS(I)-1) && (dummy == 0); i++)
    {
        for(j = 1; (j<=currRing->N) && (dummy == 0); j++)
        {
            if(p_GetExp(I->m[i],j, currRing)>1)
            {
                dummy = 1;
            }
        }
    }
    m = p_Copy(I->m[i-1],currRing);
    for(j = 1; j<=currRing->N; j++)
    {
        dummy = p_GetExp(m,j,currRing);
        if(dummy >= 1)
        {
            p_SetExp(m, j, dummy-1, currRing);
        }
    }
    if(!p_IsOne(m, currRing))
    {
        p_Setm(m, currRing);
        return(m);
    }
    m = ChoosePVar(I);
    return(m);
}
#endif

#if 0 // unused
//choice OL: last entry the first power (xy10z15 -> xy9z15)
static poly ChoosePOL(ideal I)
{
    int i,j,dummy;
    poly m;
    for(i = IDELEMS(I)-1;i>=0;i--)
    {
        m = p_Copy(I->m[i],currRing);
        for(j=1;j<=currRing->N;j++)
        {
            dummy = p_GetExp(m,j,currRing);
            if(dummy > 0)
            {
                p_SetExp(m,j,dummy-1,currRing);
                p_Setm(m,currRing);
            }
        }
        if(!p_IsOne(m, currRing))
        {
            return(m);
        }
        else
        {
            p_Delete(&m,currRing);
        }
    }
    m = ChoosePVar(I);
    return(m);
}
#endif

#if 0 // unused
//choice OF: first entry the first power (xy10z15 -> xy9z15)
static poly ChoosePOF(ideal I)
{
    int i,j,dummy;
    poly m;
    for(i = 0 ;i<=IDELEMS(I)-1;i++)
    {
        m = p_Copy(I->m[i],currRing);
        for(j=1;j<=currRing->N;j++)
        {
            dummy = p_GetExp(m,j,currRing);
            if(dummy > 0)
            {
                p_SetExp(m,j,dummy-1,currRing);
                p_Setm(m,currRing);
            }
        }
        if(!p_IsOne(m, currRing))
        {
            return(m);
        }
        else
        {
            p_Delete(&m,currRing);
        }
    }
    m = ChoosePVar(I);
    return(m);
}
#endif

#if 0 // unused
//choice VL: last entry the first variable with power (xy10z15 -> y)
static poly ChoosePVL(ideal I)
{
    int i,j,dummy;
    bool flag = TRUE;
    poly m = p_ISet(1,currRing);
    for(i = IDELEMS(I)-1;(i>=0) && (flag);i--)
    {
        flag = TRUE;
        for(j=1;(j<=currRing->N) && (flag);j++)
        {
            dummy = p_GetExp(I->m[i],j,currRing);
            if(dummy >= 2)
            {
                p_SetExp(m,j,1,currRing);
                p_Setm(m,currRing);
                flag = FALSE;
            }
        }
        if(!p_IsOne(m, currRing))
        {
            return(m);
        }
    }
    m = ChoosePVar(I);
    return(m);
}
#endif

#if 0 // unused
//choice VF: first entry the first variable with power (xy10z15 -> y)
static poly ChoosePVF(ideal I)
{
    int i,j,dummy;
    bool flag = TRUE;
    poly m = p_ISet(1,currRing);
    for(i = 0;(i<=IDELEMS(I)-1) && (flag);i++)
    {
        flag = TRUE;
        for(j=1;(j<=currRing->N) && (flag);j++)
        {
            dummy = p_GetExp(I->m[i],j,currRing);
            if(dummy >= 2)
            {
                p_SetExp(m,j,1,currRing);
                p_Setm(m,currRing);
                flag = FALSE;
            }
        }
        if(!p_IsOne(m, currRing))
        {
            return(m);
        }
    }
    m = ChoosePVar(I);
    return(m);
}
#endif

//choice JL: last entry just variable with power (xy10z15 -> y10)
static poly ChoosePJL(ideal I)
{
  int i,j,dummy;
  bool flag = TRUE;
  poly m = p_ISet(1,currRing);
  for(i = IDELEMS(I)-1;(i>=0) && (flag);i--)
  {
    flag = TRUE;
    for(j=1;(j<=currRing->N) && (flag);j++)
    {
      dummy = p_GetExp(I->m[i],j,currRing);
      if(dummy >= 2)
      {
        p_SetExp(m,j,dummy-1,currRing);
        p_Setm(m,currRing);
        flag = FALSE;
      }
    }
    if(!p_IsOne(m, currRing))
    {
      return(m);
    }
  }
  p_Delete(&m,currRing);
  m = ChoosePVar(I);
  return(m);
}

#if 0
//choice JF: last entry just variable with power -1 (xy10z15 -> y9)
static poly ChoosePJF(ideal I)
{
    int i,j,dummy;
    bool flag = TRUE;
    poly m = p_ISet(1,currRing);
    for(i = 0;(i<=IDELEMS(I)-1) && (flag);i++)
    {
        flag = TRUE;
        for(j=1;(j<=currRing->N) && (flag);j++)
        {
            dummy = p_GetExp(I->m[i],j,currRing);
            if(dummy >= 2)
            {
                p_SetExp(m,j,dummy-1,currRing);
                p_Setm(m,currRing);
                flag = FALSE;
            }
        }
        if(!p_IsOne(m, currRing))
        {
            return(m);
        }
    }
    m = ChoosePVar(I);
    return(m);
}
#endif

//chooses 1 \neq p \not\in S. This choice should be made optimal
static poly ChooseP(ideal I)
{
  poly m;
  //  TEST TO SEE WHICH ONE IS BETTER
  //m = ChoosePXL(I);
  //m = ChoosePXF(I);
  //m = ChoosePOL(I);
  //m = ChoosePOF(I);
  //m = ChoosePVL(I);
  //m = ChoosePVF(I);
  m = ChoosePJL(I);
  //m = ChoosePJF(I);
  return(m);
}

///searches for a monomial of degree d>=2 and divides it by a variable (result monomial of deg d-1)
static poly SearchP(ideal I)
{
    int i,j,exp;
    poly res;
    if(p_Totaldegree(I->m[IDELEMS(I)-1],currRing)<=1)
    {
        res = ChoosePVar(I);
        return(res);
    }
    i = IDELEMS(I)-1;
    res = p_Copy(I->m[i], currRing);
    for(j=1;j<=currRing->N;j++)
    {
        exp = p_GetExp(I->m[i], j, currRing);
        if(exp > 0)
        {
            p_SetExp(res, j, exp - 1, currRing);
            p_Setm(res,currRing);
            break;
        }
    }
    assume( j <= currRing->N );
    return(res);
}

//test if the ideal is of the form (x1, ..., xr)
static bool JustVar(ideal I)
{
    #if 0
    int i,j;
    bool foundone;
    for(i=0;i<=IDELEMS(I)-1;i++)
    {
        foundone = FALSE;
        for(j = 1;j<=currRing->N;j++)
        {
            if(p_GetExp(I->m[i], j, currRing)>0)
            {
                if(foundone == TRUE)
                {
                    return(FALSE);
                }
                foundone = TRUE;
            }
        }
    }
    return(TRUE);
    #else
    if(p_Totaldegree(I->m[IDELEMS(I)-1],currRing)>1)
    {
        return(FALSE);
    }
    return(TRUE);
    #endif
}

//computes the Euler Characteristic of the ideal
static void eulerchar (ideal I, int variables, mpz_ptr ec)
{
  loop
  {
    mpz_t dummy;
    if(JustVar(I) == TRUE)
    {
      if(IDELEMS(I) == variables)
      {
        mpz_init(dummy);
        if((variables % 2) == 0)
          mpz_set_ui(dummy, 1);
        else
          mpz_set_si(dummy, -1);
        mpz_add(ec, ec, dummy);
        mpz_clear(dummy);
      }
      return;
    }
    ideal p = idInit(1,1);
    p->m[0] = SearchP(I);
    //idPrint(I);
    //idPrint(p);
    //printf("\nNow get in idQuotMon\n");
    ideal Ip = idQuotMon(I,p);
    //idPrint(Ip);
    //Ip = SortByDeg(Ip);
    int i,howmanyvarinp = 0;
    for(i = 1;i<=currRing->N;i++)
    {
      if(p_GetExp(p->m[0],i,currRing)>0)
      {
        howmanyvarinp++;
      }
    }
    eulerchar(Ip, variables-howmanyvarinp, ec);
    id_Delete(&Ip, currRing);
    idAddMon(I,p);
    id_Delete(&p, currRing);
  }
}

//tests if an ideal is Square Free, if no, returns the variable which appears at powers >1
static poly SqFree (ideal I)
{
    int i,j;
    bool flag=TRUE;
    poly notsqrfree = NULL;
    if(p_Totaldegree(I->m[IDELEMS(I)-1],currRing)<=1)
    {
        return(notsqrfree);
    }
    for(i=IDELEMS(I)-1;(i>=0)&&(flag);i--)
    {
        for(j=1;(j<=currRing->N)&&(flag);j++)
        {
            if(p_GetExp(I->m[i],j,currRing)>1)
            {
                flag=FALSE;
                notsqrfree = p_ISet(1,currRing);
                p_SetExp(notsqrfree,j,1,currRing);
            }
        }
    }
    if(notsqrfree != NULL)
    {
        p_Setm(notsqrfree,currRing);
    }
    return(notsqrfree);
}

//checks if a polynomial is in I
static bool IsIn(poly p, ideal I)
{
  //assumes that I is ordered by degree
  if(idIs0(I))
  {
    if(p==poly(0))
    {
      return(TRUE);
    }
    else
    {
      return(FALSE);
    }
  }
  if(p==poly(0))
  {
    return(FALSE);
  }
  int i,j;
  bool flag;
  for(i = 0;i<IDELEMS(I);i++)
  {
    flag = TRUE;
    for(j = 1;(j<=currRing->N) &&(flag);j++)
    {
      if(p_GetExp(p, j, currRing)<p_GetExp(I->m[i], j, currRing))
      {
        flag = FALSE;
      }
    }
    if(flag)
    {
      return(TRUE);
    }
  }
  return(FALSE);
}

//computes the lcm of min I, I monomial ideal
static poly LCMmon(ideal I)
{
  if(idIs0(I))
  {
    return(NULL);
  }
  poly m;
  int dummy,i,j;
  m = p_ISet(1,currRing);
  for(i=1;i<=currRing->N;i++)
  {
    dummy=0;
    for(j=IDELEMS(I)-1;j>=0;j--)
    {
      if(p_GetExp(I->m[j],i,currRing) > dummy)
      {
        dummy = p_GetExp(I->m[j],i,currRing);
      }
    }
    p_SetExp(m,i,dummy,currRing);
  }
  p_Setm(m,currRing);
  return(m);
}

//the Roune Slice Algorithm
void rouneslice(ideal I, ideal S, poly q, poly x, int &prune, int &moreprune, int &steps, int &NNN, mpz_ptr &hilbertcoef, int* &hilbpower)
{
  loop
  {
    (steps)++;
    int i,j;
    int dummy;
    poly m;
    ideal p;
    //----------- PRUNING OF S ---------------
    //S SHOULD IN THIS POINT BE ORDERED BY DEGREE
    for(i=IDELEMS(S)-1;i>=0;i--)
    {
      if(IsIn(S->m[i],I))
      {
        p_Delete(&S->m[i],currRing);
        prune++;
      }
    }
    idSkipZeroes(S);
    //----------------------------------------
    for(i=IDELEMS(I)-1;i>=0;i--)
    {
      m = p_Head(I->m[i],currRing);
      for(j=1;j<=currRing->N;j++)
      {
        dummy = p_GetExp(m,j,currRing);
        if(dummy > 0)
        {
          p_SetExp(m,j,dummy-1,currRing);
        }
      }
      p_Setm(m, currRing);
      if(IsIn(m,S))
      {
        p_Delete(&I->m[i],currRing);
        //printf("\n Deleted, since pi(m) is in S\n");pWrite(m);
      }
      p_Delete(&m,currRing);
    }
    idSkipZeroes(I);
    //----------- MORE PRUNING OF S ------------
    m = LCMmon(I);
    if(m != NULL)
    {
      for(i=0;i<IDELEMS(S);i++)
      {
        if(!(p_DivisibleBy(S->m[i], m, currRing)))
        {
          S->m[i] = NULL;
          j++;
          moreprune++;
        }
        else
        {
          if(pLmEqual(S->m[i],m))
          {
            S->m[i] = NULL;
            moreprune++;
          }
        }
      }
      idSkipZeroes(S);
    }
    p_Delete(&m,currRing);
    /*printf("\n---------------------------\n");
    printf("\n      I\n");idPrint(I);
    printf("\n      S\n");idPrint(S);
    printf("\n      q\n");pWrite(q);
    getchar();*/

    if(idIs0(I))
    {
      id_Delete(&I, currRing);
      id_Delete(&S, currRing);
      break;
    }
    m = LCMmon(I);
    if(!p_DivisibleBy(x,m, currRing))
    {
      //printf("\nx does not divide lcm(I)");
      //printf("\nEmpty set");pWrite(q);
      id_Delete(&I, currRing);
      id_Delete(&S, currRing);
      p_Delete(&m, currRing);
      break;
    }
    p_Delete(&m, currRing);
    m = SqFree(I);
    if(m==NULL)
    {
      //printf("\n      Corner: ");
      //pWrite(q);
      //printf("\n      With the facets of the dual simplex:\n");
      //idPrint(I);
      mpz_t ec;
      mpz_init(ec);
      mpz_ptr ec_ptr = ec;
      eulerchar(I, currRing->N, ec_ptr);
      bool flag = FALSE;
      if(NNN==0)
      {
        hilbertcoef = (mpz_ptr)omAlloc((NNN+1)*sizeof(mpz_t));
        hilbpower = (int*)omAlloc((NNN+1)*sizeof(int));
        mpz_init_set( &hilbertcoef[NNN], ec);
        hilbpower[NNN] = p_Totaldegree(q,currRing);
        NNN++;
      }
      else
      {
        //I look if the power appears already
        for(i = 0;(i<NNN)&&(flag == FALSE)&&(p_Totaldegree(q,currRing)>=hilbpower[i]);i++)
        {
          if((hilbpower[i]) == (p_Totaldegree(q,currRing)))
          {
            flag = TRUE;
            mpz_add(&hilbertcoef[i],&hilbertcoef[i],ec_ptr);
          }
        }
        if(flag == FALSE)
        {
          hilbertcoef = (mpz_ptr)omRealloc(hilbertcoef, (NNN+1)*sizeof(mpz_t));
          hilbpower = (int*)omRealloc(hilbpower, (NNN+1)*sizeof(int));
          mpz_init(&hilbertcoef[NNN]);
          for(j = NNN; j>i; j--)
          {
            mpz_set(&hilbertcoef[j],&hilbertcoef[j-1]);
            hilbpower[j] = hilbpower[j-1];
          }
          mpz_set(  &hilbertcoef[i], ec);
          hilbpower[i] = p_Totaldegree(q,currRing);
          NNN++;
        }
      }
      mpz_clear(ec);
      id_Delete(&I, currRing);
      id_Delete(&S, currRing);
      break;
    }
    else
      p_Delete(&m, currRing);
    m = ChooseP(I);
    p = idInit(1,1);
    p->m[0] = m;
    ideal Ip = idQuotMon(I,p);
    ideal Sp = idQuotMon(S,p);
    poly pq = pp_Mult_mm(q,m,currRing);
    rouneslice(Ip, Sp, pq, x, prune, moreprune, steps, NNN, hilbertcoef,hilbpower);
    idAddMon(S,p);
    p->m[0]=NULL;
    id_Delete(&p, currRing); // p->m[0] was also in S
    p_Delete(&pq,currRing);
  }
}

//it computes the first hilbert series by means of Roune Slice Algorithm
void slicehilb(ideal I)
{
    //printf("Adi changes are here: \n");
    int i, NNN = 0;
    int steps = 0, prune = 0, moreprune = 0;
    mpz_ptr hilbertcoef;
    int *hilbpower;
    ideal S = idInit(1,1);
    poly q = p_One(currRing);
    ideal X = idInit(1,1);
    X->m[0]=p_One(currRing);
    for(i=1;i<=currRing->N;i++)
    {
      p_SetExp(X->m[0],i,1,currRing);
    }
    p_Setm(X->m[0],currRing);
    I = id_Mult(I,X,currRing);
    ideal Itmp = SortByDeg(I);
    id_Delete(&I,currRing);
    I = Itmp;
    //printf("\n-------------RouneSlice--------------\n");
    rouneslice(I,S,q,X->m[0],prune, moreprune, steps, NNN, hilbertcoef, hilbpower);
    id_Delete(&X,currRing);
    p_Delete(&q,currRing);
    //printf("\nIn total Prune got rid of %i elements\n",prune);
    //printf("\nIn total More Prune got rid of %i elements\n",moreprune);
    //printf("\nSteps of rouneslice: %i\n\n", steps);
    printf("\n//  %8d t^0",1);
    for(i = 0; i<NNN; i++)
    {
      if(mpz_sgn(&hilbertcoef[i])!=0)
      {
        gmp_printf("\n//  %8Zd t^%d",&hilbertcoef[i],hilbpower[i]);
      }
    }
    PrintLn();
    omFreeSize(hilbertcoef, (NNN)*sizeof(mpz_t));
    omFreeSize(hilbpower, (NNN)*sizeof(int));
    //printf("\n-------------------------------------\n");
}

// -------------------------------- END OF CHANGES -------------------------------------------
static intvec * hSeries(ideal S, intvec *modulweight,
                int /*notstc*/, intvec *wdegree, ideal Q, ring tailRing)
{
//  id_TestTail(S, currRing, tailRing);

  intvec *work, *hseries1=NULL;
  int  mc;
  int  p0;
  int  i, j, k, l, ii, mw;
  hexist = hInit(S, Q, &hNexist, tailRing);
  if (hNexist==0)
  {
    hseries1=new intvec(2);
    (*hseries1)[0]=1;
    (*hseries1)[1]=0;
    return hseries1;
  }

  #if 0
  if (wdegree == NULL)
    hWeight();
  else
    hWDegree(wdegree);
  #else
  if (wdegree != NULL) hWDegree(wdegree);
  #endif

  p0 = 1;
  hwork = (scfmon)omAlloc(hNexist * sizeof(scmon));
  hvar = (varset)omAlloc(((currRing->N) + 1) * sizeof(int));
  hpure = (scmon)omAlloc((1 + ((currRing->N) * (currRing->N))) * sizeof(int));
  stcmem = hCreate((currRing->N) - 1);
  Qpol = (int **)omAlloc(((currRing->N) + 1) * sizeof(int *));
  Ql = (int *)omAlloc0(((currRing->N) + 1) * sizeof(int));
  Q0 = (int *)omAlloc(((currRing->N) + 1) * sizeof(int));
  *Qpol = NULL;
  hLength = k = j = 0;
  mc = hisModule;
  if (mc!=0)
  {
    mw = hMinModulweight(modulweight);
    hstc = (scfmon)omAlloc(hNexist * sizeof(scmon));
  }
  else
  {
    mw = 0;
    hstc = hexist;
    hNstc = hNexist;
  }
  loop
  {
    if (mc!=0)
    {
      hComp(hexist, hNexist, mc, hstc, &hNstc);
      if (modulweight != NULL)
        j = (*modulweight)[mc-1]-mw;
    }
    if (hNstc!=0)
    {
      hNvar = (currRing->N);
      for (i = hNvar; i>=0; i--)
        hvar[i] = i;
      //if (notstc) // TODO: no mon divides another
        hStaircase(hstc, &hNstc, hvar, hNvar);
      hSupp(hstc, hNstc, hvar, &hNvar);
      if (hNvar!=0)
      {
        if ((hNvar > 2) && (hNstc > 10))
          hOrdSupp(hstc, hNstc, hvar, hNvar);
        hHilbEst(hstc, hNstc, hvar, hNvar);
        memset(hpure, 0, ((currRing->N) + 1) * sizeof(int));
        hPure(hstc, 0, &hNstc, hvar, hNvar, hpure, &hNpure);
        hLexS(hstc, hNstc, hvar, hNvar);
        Q0[hNvar] = 0;
        hHilbStep(hpure, hstc, hNstc, hvar, hNvar, &p0, 1);
      }
    }
    else
    {
      if(*Qpol!=NULL)
        (**Qpol)++;
      else
      {
        *Qpol = (int *)omAlloc(sizeof(int));
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
    hseries1=new intvec(2);
    (*hseries1)[0]=0;
    (*hseries1)[1]=0;
  }
  else
  {
    l = k+1;
    while ((*hseries1)[l-2]==0) l--;
    if (l!=k)
    {
      work = new intvec(l);
      for (ii=l-2; ii>=0; ii--)
        (*work)[ii] = (*hseries1)[ii];
      delete hseries1;
      hseries1 = work;
    }
    (*hseries1)[l-1] = mw;
  }
  for (i = 0; i <= (currRing->N); i++)
  {
    if (Ql[i]!=0)
      omFreeSize((ADDRESS)Qpol[i], Ql[i] * sizeof(int));
  }
  omFreeSize((ADDRESS)Q0, ((currRing->N) + 1) * sizeof(int));
  omFreeSize((ADDRESS)Ql, ((currRing->N) + 1) * sizeof(int));
  omFreeSize((ADDRESS)Qpol, ((currRing->N) + 1) * sizeof(int *));
  hKill(stcmem, (currRing->N) - 1);
  omFreeSize((ADDRESS)hpure, (1 + ((currRing->N) * (currRing->N))) * sizeof(int));
  omFreeSize((ADDRESS)hvar, ((currRing->N) + 1) * sizeof(int));
  omFreeSize((ADDRESS)hwork, hNexist * sizeof(scmon));
  hDelete(hexist, hNexist);
  if (hisModule!=0)
    omFreeSize((ADDRESS)hstc, hNexist * sizeof(scmon));
  return hseries1;
}


intvec * hHstdSeries(ideal S, intvec *modulweight, intvec *wdegree, ideal Q, ring tailRing)
{
  id_TestTail(S, currRing, tailRing);
  if (Q!=NULL) id_TestTail(Q, currRing, tailRing);
  return hSeries(S, modulweight, 0, wdegree, Q, tailRing);
}

intvec * hFirstSeries(ideal S, intvec *modulweight, ideal Q, intvec *wdegree, ring tailRing)
{
  id_TestTail(S, currRing, tailRing);
  if (Q!= NULL) id_TestTail(Q, currRing, tailRing);

  intvec *hseries1= hSeries(S, modulweight, 1, wdegree, Q, tailRing);
  if (errorreported) { delete hseries1; hseries1=NULL; }
  return hseries1;
}

intvec * hSecondSeries(intvec *hseries1)
{
  intvec *work, *hseries2;
  int i, j, k, t, l;
  int s;
  if (hseries1 == NULL)
    return NULL;
  work = new intvec(hseries1);
  k = l = work->length()-1;
  s = 0;
  for (i = k-1; i >= 0; i--)
    s += (*work)[i];
  loop
  {
    if ((s != 0) || (k == 1))
      break;
    s = 0;
    t = (*work)[k-1];
    k--;
    for (i = k-1; i >= 0; i--)
    {
      j = (*work)[i];
      (*work)[i] = -t;
      s += t;
      t += j;
    }
  }
  hseries2 = new intvec(k+1);
  for (i = k-1; i >= 0; i--)
    (*hseries2)[i] = (*work)[i];
  (*hseries2)[k] = (*work)[l];
  delete work;
  return hseries2;
}

void hDegreeSeries(intvec *s1, intvec *s2, int *co, int *mu)
{
  int i, j, k;
  int m;
  *co = *mu = 0;
  if ((s1 == NULL) || (s2 == NULL))
    return;
  i = s1->length();
  j = s2->length();
  if (j > i)
    return;
  m = 0;
  for(k=j-2; k>=0; k--)
    m += (*s2)[k];
  *mu = m;
  *co = i - j;
}

static void hPrintHilb(intvec *hseries,intvec *modul_weight)
{
  int  i, j, l, k;
  if (hseries == NULL)
    return;
  l = hseries->length()-1;
  k = (*hseries)[l];
  if ((modul_weight!=NULL)&&(modul_weight->compare(0)!=0))
  {
    char *s=modul_weight->ivString(1,0,1);
    Print("module weights:%s\n",s);
    omFree(s);
  }
  for (i = 0; i < l; i++)
  {
    j = (*hseries)[i];
    if (j != 0)
    {
      Print("//  %8d t^%d\n", j, i+k);
    }
  }
}

/*
*caller
*/
void hLookSeries(ideal S, intvec *modulweight, ideal Q, intvec *wdegree, ring tailRing)
{
  id_TestTail(S, currRing, tailRing);

  intvec *hseries1 = hFirstSeries(S, modulweight, Q, wdegree, tailRing);
  if (errorreported) return;

  hPrintHilb(hseries1,modulweight);

  const int l = hseries1->length()-1;

  intvec *hseries2 = (l > 1) ? hSecondSeries(hseries1) : hseries1;

  int co, mu;
  hDegreeSeries(hseries1, hseries2, &co, &mu);

  PrintLn();
  hPrintHilb(hseries2,modulweight);
  if ((l == 1) &&(mu == 0))
    scPrintDegree(rVar(currRing)+1, 0);
  else
    scPrintDegree(co, mu);
  if (l>1)
    delete hseries1;
  delete hseries2;
}

/***********************************************************************
 Computation of Hilbert series of non-commutative monomial algebras
************************************************************************/


static void idInsertMonomial(ideal I, poly p)
{
  /*
   * It adds monomial in I and if required,
   * enlarge the size of poly-set by 16.
   * It does not make copy of  p.
   */

  if(I == NULL)
  {
    return;
  }

  int j = IDELEMS(I) - 1;
  while ((j >= 0) && (I->m[j] == NULL))
  {
    j--;
  }
  j++;
  if (j == IDELEMS(I))
  {
    pEnlargeSet(&(I->m), IDELEMS(I), 16);
    IDELEMS(I) +=16;
  }
  I->m[j] = p;
}

static int comapreMonoIdBases(ideal J, ideal Ob)
{
  /*
   * Monomials of J and Ob are assumed to
   * be already sorted. J and Ob are
   * represented by the minimal generating set.
   */
  int i, s;
  s = 1;
  int JCount = IDELEMS(J);
  int ObCount = IDELEMS(Ob);

  if(idIs0(J))
  {
    return(1);
  }
  if(JCount != ObCount)
  {
    return(0);
  }

  for(i = 0; i < JCount; i++)
  {
    if(!(p_LmEqual(J->m[i], Ob->m[i], currRing)))
    {
      return(0);
    }
  }
  return(s);
}

static int CountOnIdUptoTruncationIndex(ideal I, int tr)
{
  /*
   * The ideal I must be sorted in increasing total degree.
   * It counts the number of monomials in I up to
   * degree less than or equal to tr.
   */

  //case when I=1;
  if(p_Totaldegree(I->m[0], currRing) == 0)
  {
    return(1);
  }

  int count = 0;
  for(int i = 0; i < IDELEMS(I); i++)
  {
    if(p_Totaldegree(I->m[i], currRing) > tr)
    {
      return (count);
    }
    count = count + 1;
  }

  return(count);
}

static int comapreMonoIdBases_IG_Case(ideal J, int JCount, ideal Ob, int ObCount)
{
  /*
   * Monomials of J and Ob are assumed to
   * be already sorted in increasing degrees.
   * J and Ob are represented by the minimal
   * generating set.  It checks if J and Ob have
   * same monomials  up to deg <=tr.
   */

  int i, s;
  s = 1;
  //when J is null
  //
  if(JCount != ObCount)
  {
    return(0);
  }

  if(JCount == 0)
  {
    return(1);
  }

  for(i = 0; i< JCount; i++)
  {
    if(!(p_LmEqual(J->m[i], Ob->m[i], currRing)))
    {
      return(0);
    }
  }

  return(s);
}

static int positionInOrbit_IG_Case(ideal I, poly w, std::vector<ideal> idorb, std::vector<poly> polist, int trInd, int trunDegHs)
{
  /*
   * It compares the ideal I with ideals in the set 'idorb'
   * up to total degree =
   * trInd - max(deg of w, deg of word in polist) polynomials.
   *
   * It returns 0 if I is not equal to any ideal in the
   * 'idorb' else returns position of the matched ideal.
   */

  int ps = 0;
  int i, s = 0;
  int orbCount = idorb.size();

  if(idIs0(I))
  {
    return(1);
  }

  int degw = p_Totaldegree(w, currRing);
  int degp;
  int dtr;
  int dtrp;

  dtr = trInd - degw;
  int IwCount;

   IwCount = CountOnIdUptoTruncationIndex(I, dtr);

  if(IwCount == 0)
  {
    return(1);
  }

  int ObCount;

  bool flag2 = FALSE;

  for(i = 1;i < orbCount; i++)
  {
    degp = p_Totaldegree(polist[i], currRing);
    if(degw > degp)
    {
      dtr = trInd - degw;

      ObCount = 0;
      ObCount = CountOnIdUptoTruncationIndex(idorb[i], dtr);
      if(ObCount == 0)
      {continue;}
      if(flag2)
      {
        IwCount = 0;
        IwCount = CountOnIdUptoTruncationIndex(I, dtr);
        flag2 = FALSE;
      }
    }
    else
    {
      flag2 = TRUE;
      dtrp = trInd - degp;
      ObCount = 0;
      ObCount = CountOnIdUptoTruncationIndex(idorb[i], dtrp);
      IwCount = 0;
      IwCount = CountOnIdUptoTruncationIndex(I, dtrp);
    }

    s = comapreMonoIdBases_IG_Case(I, IwCount, idorb[i], ObCount);

    if(s)
    {
      ps = i + 1;
      break;
    }
  }
  return(ps);
}

static int positionInOrbit_FG_Case(ideal I, poly, std::vector<ideal> idorb, std::vector<poly>, int, int)
{
  /*
   * It compares the ideal I with ideals in the set 'idorb'.
   * I and ideals of 'idorb' are sorted.
   *
   * It returns 0 if I is not equal to any ideal of 'idorb'
   * else returns position of the matched ideal.
   */
  int ps = 0;
  int i, s = 0;
  int OrbCount = idorb.size();

  if(idIs0(I))
  {
    return(1);
  }

  for(i = 1; i < OrbCount; i++)
  {
    s = comapreMonoIdBases(I, idorb[i]);
    if(s)
    {
      ps = i + 1;
      break;
    }
  }

  return(ps);
}

static int positionInOrbitTruncationCase(ideal I, poly w, std::vector<ideal> idorb, std::vector<poly> polist, int , int trunDegHs)
{
  /*
   * It compares the ideal I with ideals in the set 'idorb'.
   * I and ideals in 'idorb' are sorted.

   * returns 0 if I is not equal to any ideal of 'idorb'
   * else returns position of the matched ideal.
   */

  int ps = 0;
  int i, s = 0;
  int OrbCount = idorb.size();
  int dtr=0; int IwCount, ObCount;
  dtr = trunDegHs - 1 - p_Totaldegree(w, currRing);

  if(idIs0(I))
  {
    for(i = 1; i < OrbCount; i++)
    {
      if(p_Totaldegree(w, currRing) == p_Totaldegree(polist[i], currRing))
      {
        if(idIs0(idorb[i]))
          return(i+1);
        ObCount=0;
        ObCount = CountOnIdUptoTruncationIndex(idorb[i], dtr);
        if(ObCount==0)
        {
          ps = i + 1;
          break;
        }
      }
    }

    return(ps);
  }

  IwCount = CountOnIdUptoTruncationIndex(I, dtr);

  if(p_Totaldegree(I->m[0], currRing)==0)
  {
    for(i = 1; i < OrbCount; i++)
    {
       if(idIs0(idorb[i]))
         continue;
       if(p_Totaldegree(idorb[i]->m[0], currRing)==0)
       {
         ps = i + 1;
         break;
       }
     }
     return(ps);
  }

  for(i = 1; i < OrbCount; i++)
  {
    if(p_Totaldegree(w, currRing) == p_Totaldegree(polist[i], currRing))
    {
      if(idIs0(idorb[i]))
        continue;
      ObCount=0;
      ObCount = CountOnIdUptoTruncationIndex(idorb[i], dtr);
      s = comapreMonoIdBases_IG_Case(I, IwCount, idorb[i], ObCount);
      if(s)
      {
        ps = i + 1;
        break;
      }
    }
  }

  return(ps);
}

static int monCompare( const void *m, const void *n)
{
  /* compares monomials */

 return(p_Compare(*(poly*) m, *(poly*)n, currRing));
}

void sortMonoIdeal_pCompare(ideal I)
{
  /*
   * sorts monomial ideal in ascending order
   * order must be a total degree
   */

  qsort(I->m, IDELEMS(I), sizeof(poly), monCompare);

}



static ideal  minimalMonomialGenSet(ideal I)
{
  /*
   * eliminates monomials which
   * can be generated by others in I
   */
  //first sort monomials of the ideal

  idSkipZeroes(I);

  sortMonoIdeal_pCompare(I);

  int i, k;
  int ICount = IDELEMS(I);

  for(k = ICount - 1; k >=1; k--)
  {
    for(i = 0; i < k; i++)
    {

      if(p_LmDivisibleBy(I->m[i], I->m[k], currRing))
      {
        pDelete(&(I->m[k]));
        break;
      }
    }
  }

  idSkipZeroes(I);
  return(I);
}

static poly shiftInMon(poly p, int i, int lV, const ring r)
{
  /*
   * shifts the varibles of monomial p in the  i^th layer,
   * p remains unchanged,
   * creates new poly and returns it for the colon ideal
   */
  poly smon = p_One(r);
  int j, sh, cnt;
  cnt = r->N;
  sh = i*lV;
  int *e=(int *)omAlloc((r->N+1)*sizeof(int));
  int *s=(int *)omAlloc0((r->N+1)*sizeof(int));
  p_GetExpV(p, e, r);

  for(j = 1; j <= cnt; j++)
  {
    if(e[j] == 1)
    {
      s[j+sh] = e[j];
    }
  }

  p_SetExpV(smon, s, currRing);
  omFree(e);
  omFree(s);

  p_SetComp(smon, p_GetComp(p, currRing), currRing);
  p_Setm(smon, currRing);

  return(smon);
}

static poly deleteInMon(poly w, int i, int lV, const ring r)
{
  /*
   * deletes the variables upto i^th layer of monomial w
   * w remains unchanged
   * creates new poly and returns it for the colon ideal
   */

  poly dw = p_One(currRing);
  int *e = (int *)omAlloc((r->N+1)*sizeof(int));
  int *s=(int *)omAlloc0((r->N+1)*sizeof(int));
  p_GetExpV(w, e, r);
  int j, cnt;
  cnt = i*lV;
  /*
  for(j=1;j<=cnt;j++)
  {
    e[j]=0;
  }*/
  for(j = (cnt+1); j < (r->N+1); j++)
  {
    s[j] = e[j];
  }

  p_SetExpV(dw, s, currRing);//new exponents
  omFree(e);
  omFree(s);

  p_SetComp(dw, p_GetComp(w, currRing), currRing);
  p_Setm(dw, currRing);

  return(dw);
}

static void TwordMap(poly p, poly w, int lV, int d, ideal Jwi, bool &flag)
{
  /*
   * computes T_w(p) in a new poly object and places it
   * in Jwi which stores elements of colon ideal of I,
   * p and w remain unchanged,
   * the new polys for Jwi are constructed by sub-routines
   * deleteInMon, shiftInMon, p_MDivide,
   * places the result in Jwi and deletes the new polys
   * coming in dw, smon, qmon
   */
  int i;
  poly smon, dw;
  poly qmonp = NULL;
  bool del;

  for(i = 0;i <= d - 1; i++)
  {
    dw = deleteInMon(w, i, lV, currRing);
    smon = shiftInMon(p, i, lV, currRing);
    del = TRUE;

    if(pLmDivisibleBy(smon, w))
    {
      flag = TRUE;
      del  = FALSE;

      pDelete(&dw);
      pDelete(&smon);

      //delete all monomials of Jwi
      //and make Jwi =1

      for(int j = 0;j < IDELEMS(Jwi); j++)
      {
        pDelete(&Jwi->m[j]);
      }

      idInsertMonomial(Jwi, p_One(currRing));
      break;
    }

    if(pLmDivisibleBy(dw, smon))
    {
      del = FALSE;
      qmonp = p_MDivide(smon, dw, currRing);
      idInsertMonomial(Jwi, shiftInMon(qmonp, -d, lV, currRing));
      pLmFree(&qmonp);
      pDelete(&dw);
      pDelete(&smon);
    }
    //in case both if are false, delete dw and smon
    if(del)
    {
      pDelete(&dw);
      pDelete(&smon);
    }
  }

}

static ideal colonIdeal(ideal S, poly w, int lV, ideal Jwi, int trunDegHs)
{
  /*
   * It computes the right colon ideal of a two-sided ideal S
   * w.r.t. word w and save it in a new object Jwi.
   * It keeps S and w unchanged.
   */

  if(idIs0(S))
  {
    return(S);
  }

  int i, d;
  d = p_Totaldegree(w, currRing);
  if(trunDegHs !=0 && d >= trunDegHs)
  {
   idInsertMonomial(Jwi, p_One(currRing));
   return(Jwi);
  }
  bool flag = FALSE;
  int SCount = IDELEMS(S);
  for(i = 0; i < SCount; i++)
  {
    TwordMap(S->m[i], w, lV, d, Jwi, flag);
    if(flag)
    {
      break;
    }
  }

  Jwi = minimalMonomialGenSet(Jwi);
  return(Jwi);
}

void HilbertSeries_OrbitData(ideal S, int lV, bool IG_CASE, bool mgrad, bool odp, int trunDegHs)
{
  /*
   * This is based on iterative right colon operations on a
   * two-sided monomial ideal of the free associative algebra.
   * The algorithm terminates for those monomial ideals
   * whose monomials define  "regular formal languages",
   * that is, all monomials of the input ideal can be obtained
   * from finite languages by applying finite number of
   * rational operations.
   */

  int trInd;
  S = minimalMonomialGenSet(S);
  if( !idIs0(S) && p_Totaldegree(S->m[0], currRing)==0)
  {
    PrintS("Hilbert Series:\n 0\n");
    return;
  }
  int (*POS)(ideal, poly, std::vector<ideal>, std::vector<poly>, int, int);
  if(trunDegHs != 0)
  {
    Print("\nTruncation degree = %d\n",trunDegHs);
    POS=&positionInOrbitTruncationCase;
  }
  else
  {
    if(IG_CASE)
    {
      if(idIs0(S))
      {
        WerrorS("wrong input: it is not an infinitely gen. case");
        return;
      }
      trInd = p_Totaldegree(S->m[IDELEMS(S)-1], currRing);
      POS = &positionInOrbit_IG_Case;
    }
    else
    POS = &positionInOrbit_FG_Case;
  }
  std::vector<ideal > idorb;
  std::vector< poly > polist;

  ideal orb_init = idInit(1, 1);
  idorb.push_back(orb_init);

  polist.push_back( p_One(currRing));

  std::vector< std::vector<int> > posMat;
  std::vector<int> posRow(lV,0);
  std::vector<int> C;

  int ds, is, ps;
  int lpcnt = 0;

  poly w, wi;
  ideal Jwi;

  while(lpcnt < idorb.size())
  {
    w = NULL;
    w = polist[lpcnt];
    if(lpcnt >= 1 && idIs0(idorb[lpcnt]) == FALSE)
    {
      if(p_Totaldegree(idorb[lpcnt]->m[0], currRing) != 0)
      {
        C.push_back(1);
      }
      else
        C.push_back(0);
    }
    else
    {
      C.push_back(1);
    }

    ds = p_Totaldegree(w, currRing);
    lpcnt++;

    for(is = 1; is <= lV; is++)
    {
      wi = NULL;
      //make new copy 'wi' of word w=polist[lpcnt]
      //and update it (for the colon operation).
      //if corresponding to wi, right colon operation gives
      //a new (right colon) ideal of S,
      //keep 'wi' in the polist else delete it

      wi = pCopy(w);
      p_SetExp(wi, (ds*lV)+is, 1, currRing);
      p_Setm(wi, currRing);
      Jwi = NULL;
      //Jwi stores (right) colon ideal of S w.r.t. word
      //wi if colon operation gives a new ideal place it
      //in the vector of ideals  'idorb'
      //otherwise delete it

      Jwi = idInit(1,1);

      Jwi = colonIdeal(S, wi, lV, Jwi, trunDegHs);
      ps = (*POS)(Jwi, wi, idorb, polist, trInd, trunDegHs);

      if(ps == 0)  // finds a new ideal
      {
        posRow[is-1] = idorb.size();

        idorb.push_back(Jwi);
        polist.push_back(wi);
      }
      else // ideal is already there  in the set
      {
        posRow[is-1]=ps-1;
        idDelete(&Jwi);
        pDelete(&wi);
      }
    }
    posMat.push_back(posRow);
    posRow.resize(lV,0);
  }
  int lO = C.size();//size of the orbit
  PrintLn();
  Print("maximal length of words = %ld\n", p_Totaldegree(polist[lO-1], currRing));
  Print("\nlength of the Orbit = %d", lO);
  PrintLn();

  if(odp)
  {
    Print("words description of the Orbit: \n");
    for(is = 0; is < lO; is++)
    {
      pWrite0(polist[is]);
      PrintS("    ");
    }
    PrintLn();
    PrintS("\nmaximal degree,  #(sum_j R(w,w_j))");
    PrintLn();
    for(is = 0; is < lO; is++)
    {
      if(idIs0(idorb[is]))
      {
        PrintS("NULL\n");
      }
      else
      {
        Print("%ld,  %d \n",p_Totaldegree(idorb[is]->m[IDELEMS(idorb[is])-1], currRing),IDELEMS(idorb[is]));
      }
    }
  }

  for(is = idorb.size()-1; is >= 0; is--)
  {
    idDelete(&idorb[is]);
  }
  for(is = polist.size()-1; is >= 0; is--)
  {
    pDelete(&polist[is]);
  }

  idorb.resize(0);
  polist.resize(0);

  int adjMatrix[lO][lO];
  memset(adjMatrix, 0, lO*lO*sizeof(int));
  int rowCount, colCount;
  int tm = 0;
  if(!mgrad)
  {
    for(rowCount = 0; rowCount < lO; rowCount++)
    {
      for(colCount = 0; colCount < lV; colCount++)
      {
        tm = posMat[rowCount][colCount];
        adjMatrix[rowCount][tm] = adjMatrix[rowCount][tm] + 1;
      }
    }
  }

  ring r = currRing;
  int npar;
  char** tt;
  TransExtInfo p;
  if(!mgrad)
  {
    tt=(char**)omAlloc(sizeof(char*));
    tt[0] = omStrDup("t");
    npar = 1;
  }
  else
  {
    tt=(char**)omalloc(lV*sizeof(char*));
    for(is = 0; is < lV; is++)
    {
      tt[is] = (char*)omAlloc(7*sizeof(char)); //if required enlarge it later
      sprintf (tt[is], "t%d", is+1);
    }
    npar = lV;
  }

  p.r = rDefault(0, npar, tt);
  coeffs cf = nInitChar(n_transExt, &p);
  char** xx = (char**)omAlloc(sizeof(char*));
  xx[0] = omStrDup("x");
  ring R = rDefault(cf, 1, xx);
  rChangeCurrRing(R);//rWrite(R);
  /*
   * matrix corresponding to the orbit of the ideal
   */
  matrix mR = mpNew(lO, lO);
  matrix cMat = mpNew(lO,1);
  poly rc;

  if(!mgrad)
  {
    for(rowCount = 0; rowCount < lO; rowCount++)
    {
      for(colCount = 0; colCount < lO; colCount++)
      {
        if(adjMatrix[rowCount][colCount] != 0)
        {
          MATELEM(mR, rowCount + 1, colCount + 1) = p_ISet(adjMatrix[rowCount][colCount], R);
          p_SetCoeff(MATELEM(mR, rowCount + 1, colCount + 1), n_Mult(pGetCoeff(mR->m[lO*rowCount+colCount]),n_Param(1, R->cf), R->cf), R);
        }
      }
    }
  }
  else
  {
     for(rowCount = 0; rowCount < lO; rowCount++)
     {
       for(colCount = 0; colCount < lV; colCount++)
       {
          rc=NULL;
          rc=p_One(R);
          p_SetCoeff(rc, n_Mult(pGetCoeff(rc), n_Param(colCount+1, R->cf),R->cf), R);
          MATELEM(mR, rowCount +1, posMat[rowCount][colCount]+1)=p_Add_q(rc,MATELEM(mR, rowCount +1, posMat[rowCount][colCount]+1), R);
       }
     }
  }

  for(rowCount = 0; rowCount < lO; rowCount++)
  {
    if(C[rowCount] != 0)
    {
      MATELEM(cMat, rowCount + 1, 1) = p_ISet(C[rowCount], R);
    }
  }

  matrix u;
  unitMatrix(lO, u); //unit matrix
  matrix gMat = mp_Sub(u, mR, R);

  char* s;

  if(odp)
  {
    PrintS("\nlinear system:\n");
    if(!mgrad)
    {
      for(rowCount = 0; rowCount < lO; rowCount++)
      {
        Print("H(%d) = ", rowCount+1);
        for(colCount = 0; colCount < lV; colCount++)
        {
          StringSetS(""); nWrite(n_Param(1, R->cf));
          s = StringEndS(); PrintS(s);
          Print("*"); omFree(s);
          Print("H(%d) + ", posMat[rowCount][colCount] + 1);
        }
        Print(" %d\n", C[rowCount] );
      }
      PrintS("where H(1) represents the series corresp. to input ideal\n");
      PrintS("and i^th summand in the rhs of an eqn. is according\n");
      PrintS("to the right colon map corresp. to the i^th variable\n");
    }
    else
    {
      for(rowCount = 0; rowCount < lO; rowCount++)
      {
        Print("H(%d) = ", rowCount+1);
        for(colCount = 0; colCount < lV; colCount++)
        {
           StringSetS(""); nWrite(n_Param(colCount+1, R->cf));
           s = StringEndS(); PrintS(s);
           Print("*");omFree(s);
           Print("H(%d) + ", posMat[rowCount][colCount] + 1);
        }
        Print(" %d\n", C[rowCount] );
      }
      PrintS("where H(1) represents the series corresp. to input ideal\n");
    }
  }
  PrintLn();
  posMat.resize(0);
  C.resize(0);
  matrix pMat;
  matrix lMat;
  matrix uMat;
  matrix H_serVec = mpNew(lO, 1);
  matrix Hnot;

  //std::clock_t start;
  //start = std::clock();

  luDecomp(gMat, pMat, lMat, uMat, R);
  luSolveViaLUDecomp(pMat, lMat, uMat, cMat, H_serVec, Hnot);

  //to print system solving time
  //if(odp){
  //std::cout<<"solving time of the system = "<<(std::clock()-start)/(double)(CLOCKS_PER_SEC / 1000)<<" ms"<<std::endl;}

  mp_Delete(&mR, R);
  mp_Delete(&u, R);
  mp_Delete(&pMat, R);
  mp_Delete(&lMat, R);
  mp_Delete(&uMat, R);
  mp_Delete(&cMat, R);
  mp_Delete(&gMat, R);
  mp_Delete(&Hnot, R);
  //print the Hilbert series and length of the Orbit
  PrintLn();
  Print("Hilbert series:");
  PrintLn();
  pWrite(H_serVec->m[0]);
  if(!mgrad)
  {
    omFree(tt[0]);
  }
  else
  {
    for(is = lV-1; is >= 0; is--)

      omFree( tt[is]);
  }
  omFree(tt);
  omFree(xx[0]);
  omFree(xx);
  rChangeCurrRing(r);
  rKill(R);
}

ideal RightColonOperation(ideal S, poly w, int lV)
{
  /*
   * This returns right colon ideal of a monomial two-sided ideal of
   * the free associative algebra with respect to a monomial 'w'
   * (S:_R w).
   */
  S = minimalMonomialGenSet(S);
  ideal Iw = idInit(1,1);
  Iw = colonIdeal(S, w, lV, Iw, 0);
  return (Iw);
}


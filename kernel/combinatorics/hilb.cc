/****************************************
*  Computer Algebra System SINGULAR     *
****************************************/
/*
*  ABSTRACT -  Hilbert series
*/




#include <kernel/mod2.h>
#include <kernel/structs.h>
#include <kernel/febase.h>
#include <omalloc/omalloc.h>
#include <kernel/polys.h>
#include <misc/intvec.h>
#include <kernel/combinatorics/hutil.h>
#include <kernel/GBEngine/stairc.h>
//ADICHANGES:
#include <kernel/ideals.h>
#include <kernel/GBEngine/kstd1.h>
#include<gmp.h>
#include<vector>


static int  **Qpol;
static int  *Q0, *Ql;
static int  hLength;
   

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
       Werror("interal arrays too big");
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

//returns the degree of the monomial
static int DegMon(poly p)
{
    #if 1
    int i,deg;
    deg = 0;
    for(i=1;i<=currRing->N;i++)
    {
        deg = deg + p_GetExp(p, i, currRing);
    }
    return(deg);
    #else
    return(p_Deg(p, currRing));
    #endif
}

//Tests if the ideal is sorted by degree
static bool idDegSortTest(ideal I)
{
    if((I == NULL)||(idIs0(I)))
    {
        return(TRUE);
    }
    for(int i = 0; i<IDELEMS(I)-1; i++)
    {
        if(DegMon(I->m[i])>DegMon(I->m[i+1]))
        {
            idPrint(I);
            Werror("Ideal is not deg sorted!!");
            return(FALSE);
        }
    }
    return(TRUE);
}

//adds the new polynomial at the coresponding position
//and simplifies the ideal
static ideal SortByDeg_p(ideal I, poly p)
{
    int i,j;
    if((I == NULL) || (idIs0(I)))
    {
        ideal res = idInit(1,1);
        res->m[0] = p;
        return(res);
    }
    idSkipZeroes(I);
    #if 1
    for(i = 0; (i<IDELEMS(I)) && (DegMon(I->m[i])<=DegMon(p)); i++)
    {
        if(p_DivisibleBy( I->m[i],p, currRing))
        {
            return(I);
        }
    }
    for(i = IDELEMS(I)-1; (i>=0) && (DegMon(I->m[i])>=DegMon(p)); i--)
    {
        if(p_DivisibleBy(p,I->m[i], currRing))
        {
            I->m[i] = NULL;
        }
    }
    if(idIs0(I))
    {
        idSkipZeroes(I);
        I->m[0] = p;
        return(I);
    }
    #endif
    idSkipZeroes(I);
    //First I take the case when all generators have the same degree
    if(DegMon(I->m[0]) == DegMon(I->m[IDELEMS(I)-1]))
    {
        if(DegMon(p)<DegMon(I->m[0]))
        {
            idInsertPoly(I,p);
            idSkipZeroes(I);
            for(i=IDELEMS(I)-1;i>=1; i--)
            {
                I->m[i] = I->m[i-1];
            }
            I->m[0] = p;
            return(I);
        }
        if(DegMon(p)>=DegMon(I->m[IDELEMS(I)-1]))
        {
            idInsertPoly(I,p);
            idSkipZeroes(I);
            return(I);
        }
    }
    if(DegMon(p)<=DegMon(I->m[0]))
    {
        idInsertPoly(I,p);
        idSkipZeroes(I);
        for(i=IDELEMS(I)-1;i>=1; i--)
        {
            I->m[i] = I->m[i-1];
        }
        I->m[0] = p;
        return(I);
    }
    if(DegMon(p)>=DegMon(I->m[IDELEMS(I)-1]))
    {
        idInsertPoly(I,p);
        idSkipZeroes(I);
        return(I);
    }
    for(i = IDELEMS(I)-2; ;)
    {
        if(DegMon(p)==DegMon(I->m[i]))
        {
            idInsertPoly(I,p);
            idSkipZeroes(I);
            for(j = IDELEMS(I)-1; j>=i+1;j--)
            {
                I->m[j] = I->m[j-1];
            }
            I->m[i] = p;
            return(I);
        }
        if(DegMon(p)>DegMon(I->m[i]))
        {
            idInsertPoly(I,p);
            idSkipZeroes(I);
            for(j = IDELEMS(I)-1; j>=i+2;j--)
            {
                I->m[j] = I->m[j-1];
            }
            I->m[i+1] = p;
            return(I);
        }
        i--;
    }
}

//it sorts the ideal by the degrees
static ideal SortByDeg(ideal I)
{
    if(idIs0(I))
    {
        return(I);
    }
    idSkipZeroes(I);
    int i;
    ideal res;
    idSkipZeroes(I);
    res = idInit(1,1);
    res->m[0] = poly(0);
    for(i = 0; i<=IDELEMS(I)-1;i++)
    {
        res = SortByDeg_p(res, I->m[i]);
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
    ideal I = idCopy(Iorig);
    ideal res = idInit(IDELEMS(I),1);
    int i,j;
    int dummy;
    for(i = 0; i<IDELEMS(I); i++)
    {
        res->m[i] = p_Copy(I->m[i], currRing);
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
        if(DegMon(res->m[i]) == DegMon(I->m[i]))
        {
            res->m[i] = NULL;
        }
        else
        {
            I->m[i] = NULL;
        }
    }
    idSkipZeroes(res);
    idSkipZeroes(I);
    if(!idIs0(res))
    {
    for(i = 0; i<=IDELEMS(res)-1; i++)
    {
        I = SortByDeg_p(I,res->m[i]);
    }
    }
    //idDegSortTest(I);
    return(I);
}

//id_Add for monomials
static ideal idAddMon(ideal I, ideal p)
{
    #if 1
    I = SortByDeg_p(I,p->m[0]);
    #else
    I = id_Add(I,p,currRing);
    #endif
    //idSkipZeroes(I);
    return(I);
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
    m = ChoosePVar(I);
    return(m);
}

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

//searches for a monomial of degree d>=2 and divides it by a variable (result monomial of deg d-1)
static poly SearchP(ideal I)
{
    int i,j,exp;
    poly res;
    if(DegMon(I->m[IDELEMS(I)-1])<=1)
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
            return(res);
        }
    }
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
    if(DegMon(I->m[IDELEMS(I)-1])>1)
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
                {mpz_set_si(dummy, 1);}
            else
                {mpz_set_si(dummy, -1);}
            mpz_add(ec, ec, dummy);
        }
        //mpz_clear(dummy);
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
    I = idAddMon(I,p);
  }
}

//tests if an ideal is Square Free, if no, returns the variable which appears at powers >1
static poly SqFree (ideal I)
{
    int i,j;
    bool flag=TRUE;
    poly notsqrfree = NULL;
    if(DegMon(I->m[IDELEMS(I)-1])<=1)
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
    ideal p, koszsimp;
    //----------- PRUNING OF S ---------------
    //S SHOULD IN THIS POINT BE ORDERED BY DEGREE
    for(i=IDELEMS(S)-1;i>=0;i--)
    {
        if(IsIn(S->m[i],I))
        {
            S->m[i]=NULL;
            prune++;
        }
    }
    idSkipZeroes(S);
    //----------------------------------------
    for(i=IDELEMS(I)-1;i>=0;i--)
    {
        m = p_Copy(I->m[i],currRing);
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
            I->m[i]=NULL;
            //printf("\n Deleted, since pi(m) is in S\n");pWrite(m);
        }
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
    /*printf("\n---------------------------\n");
    printf("\n      I\n");idPrint(I);
    printf("\n      S\n");idPrint(S);
    printf("\n      q\n");pWrite(q);
    getchar();*/
    
    if(idIs0(I))
    {
        id_Delete(&I, currRing);
        id_Delete(&S, currRing);
        p_Delete(&m, currRing);
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
                mpz_init( &hilbertcoef[NNN]);
                mpz_set(  &hilbertcoef[NNN], ec);
                mpz_clear(ec);
                hilbpower[NNN] = DegMon(q);
                NNN++;
            }
        else
        {
            //I look if the power appears already
            for(i = 0;(i<NNN)&&(flag == FALSE)&&(DegMon(q)>=hilbpower[i]);i++)
            {
                if((hilbpower[i]) == (DegMon(q)))
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
                mpz_clear(ec);
                hilbpower[i] = DegMon(q);
                NNN++;
            }
        }
        break;
    }
    m = ChooseP(I);
    p = idInit(1,1);
    p->m[0] = m;
    ideal Ip = idQuotMon(I,p);
    ideal Sp = idQuotMon(S,p);
    poly pq = pp_Mult_mm(q,m,currRing);
    rouneslice(Ip, Sp, pq, x, prune, moreprune, steps, NNN, hilbertcoef,hilbpower);
    //id_Delete(&Ip, currRing);
    //id_Delete(&Sp, currRing);
    S = idAddMon(S,p);
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
    poly q = p_ISet(1,currRing);
    ideal X = idInit(1,1);
    X->m[0]=p_One(currRing);
    for(i=1;i<=currRing->N;i++)
    {
            p_SetExp(X->m[0],i,1,currRing);   
    }
    p_Setm(X->m[0],currRing);
    I = id_Mult(I,X,currRing);
    I = SortByDeg(I);
    //printf("\n-------------RouneSlice--------------\n");
    rouneslice(I,S,q,X->m[0],prune, moreprune, steps, NNN, hilbertcoef, hilbpower);
    //printf("\nIn total Prune got rid of %i elements\n",prune);
    //printf("\nIn total More Prune got rid of %i elements\n",moreprune);
    //printf("\nSteps of rouneslice: %i\n\n", steps);
    mpz_t coefhilb;
    mpz_t dummy;
    mpz_init(coefhilb);
    mpz_init(dummy);
    printf("\n//  %8d t^0",1);
    for(i = 0; i<NNN; i++)
    {
        if(mpz_sgn(&hilbertcoef[i])!=0)
        {
            gmp_printf("\n//  %8Zd t^%d",&hilbertcoef[i],hilbpower[i]);
        }
    }
    omFreeSize(hilbertcoef, (NNN)*sizeof(mpz_t));
    omFreeSize(hilbpower, (NNN)*sizeof(int));
    //printf("\n-------------------------------------\n");
}

// -------------------------------- END OF CHANGES -------------------------------------------
static intvec * hSeries(ideal S, intvec *modulweight,
                int /*notstc*/, intvec *wdegree, ideal Q, ring tailRing)
{
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
  return hSeries(S, modulweight, 0, wdegree, Q, tailRing);
}

intvec * hFirstSeries(ideal S, intvec *modulweight, ideal Q, intvec *wdegree, ring tailRing)
{
  return hSeries(S, modulweight, 1, wdegree, Q, tailRing);
}

intvec * hSecondSeries(intvec *hseries1)
{
  intvec *work, *hseries2;
  int i, j, k, s, t, l;
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
  int m, i, j, k;
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

static void hPrintHilb(intvec *hseries)
{
  int  i, j, l, k;
  if (hseries == NULL)
    return;
  l = hseries->length()-1;
  k = (*hseries)[l];
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
void hLookSeries(ideal S, intvec *modulweight, ideal Q)
{
  int co, mu, l;
  intvec *hseries2;
  intvec *hseries1 = hFirstSeries(S, modulweight, Q);
  hPrintHilb(hseries1);
  l = hseries1->length()-1;
  if (l > 1)
    hseries2 = hSecondSeries(hseries1);
  else
    hseries2 = hseries1;
  hDegreeSeries(hseries1, hseries2, &co, &mu);
  PrintLn();
  hPrintHilb(hseries2);
  if ((l == 1) &&(mu == 0))
    scPrintDegree((currRing->N)+1, 0);
  else
    scPrintDegree(co, mu);
  if (l>1)
    delete hseries1;
  delete hseries2;
}




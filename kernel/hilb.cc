/****************************************
*  Computer Algebra System SINGULAR     *
****************************************/
/*
*  ABSTRACT -  Hilbert series
*/

#ifdef HAVE_CONFIG_H
#include "singularconfig.h"
#endif /* HAVE_CONFIG_H */
#include <kernel/mod2.h>
#include <kernel/structs.h>
#include <kernel/febase.h>
#include <omalloc/omalloc.h>
#include <kernel/polys.h>
#include <misc/intvec.h>
#include <kernel/hutil.h>
#include <kernel/stairc.h>
//ADICHANGES:
#include <kernel/ideals.h>
#include <kernel/kstd1.h>
#include<gmp.h>
#include<vector>
//TIMER
#include<time.h>


static int  **Qpol;
static int  *Q0, *Ql;
static int  hLength;
//ADICHANGES
static int NNN;
static mpz_t ec;
static mpz_ptr hilbertcoef = (mpz_ptr)omAlloc(NNN*sizeof(mpz_t));
static std::vector<int> hilbertpowers;
static std::vector<int> hilbertpowerssorted;
static std::vector<int> degsort;
static std::vector<int> eulerprop;
static ideal eulersimp;
static int steps, prune = 0, moreprune = 0, eulerstep, eulerskips=0;
   

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

//idQuot(I,p) for I monomial ideal, p a ideal with a single monomial.
ideal idQuotMon(ideal I, ideal p)
{
    //printf("\nOriginal:\n");idPrint(idQuot(I,p,TRUE,TRUE));
    #if 1
    if(idIs0(I))
    {
        ideal res = idInit(1,1);
        res->m[0] = poly(0);
        /*printf("\n      ------ idQuotMon\n");
    idPrint(I);
    idPrint(p);
    idPrint(I);getchar();*/
        return(res);
    }
    if(idIs0(p))
    {
        ideal res = idInit(1,1);
        res->m[0] = pOne();
        /*printf("\n      ------ idQuotMon\n");
    idPrint(I);
    idPrint(p);
    idPrint(res);getchar();*/
        return(res);
    }
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
    }
    res = idMinBase(res);
    idSkipZeroes(res);
    #else
    ideal res;
    res = idQuot(I,p,TRUE,TRUE);
    #endif
    /*printf("\n      ------ idQuotMon\n");
    idPrint(I);
    idPrint(p);
    idPrint(res);*/
    return(res);
} 

//puts in hilbertpowerssorted the two new entries and shifts if needed the old ones
static void PutInVector(int degvalue, int oldposition, int where)
{
    hilbertpowerssorted.resize(hilbertpowerssorted.size()+2);
    int i;
    for(i=hilbertpowerssorted.size()-1;i>=where+2;i--)
    {
        hilbertpowerssorted[i] = hilbertpowerssorted[i-2];
    }
    hilbertpowerssorted[where] = degvalue;
    hilbertpowerssorted[where+1] = oldposition;
}

//sorts hilbertpowers (new finds the positions)
static void SortPowerVec()
{
    int i,j;
    int test;
    bool flag;
    //printf("Size of hilbertpowers: %i", hilbertpowers.size());
    hilbertpowerssorted.resize(2);
    hilbertpowerssorted[0] = hilbertpowers[0];
    hilbertpowerssorted[1] = 0;
    for(i=1;i<hilbertpowers.size();i++)
    {
        flag=TRUE;
        if(hilbertpowers[i]<=hilbertpowerssorted[0])
        {
            flag=FALSE;
            PutInVector(hilbertpowers[i], i, 0);
        }
        if((hilbertpowers[i]>=hilbertpowerssorted[hilbertpowerssorted.size()-2]) && (flag == TRUE))
        {
            flag=FALSE;
            PutInVector(hilbertpowers[i], i, hilbertpowerssorted.size()-2);
        }
        if(flag==TRUE)
        {
            for(j=2;(j<=hilbertpowerssorted.size()-4)&&(flag);j=j+2)
            {
                if(hilbertpowers[i]<=hilbertpowerssorted[j])
                {
                    flag=FALSE;
                    PutInVector(hilbertpowers[i], i, j);
                }
            }
        }
    }
}

//returns the degree of the monomial
static int DegMon(poly p)
{
    int i,deg;
    deg = 0;
    for(i=1;i<=currRing->N;i++)
    {
        deg = deg + p_GetExp(p, i, currRing);
    }
    return(deg);
}

//divides all monomials by the last entry 
static ideal idSimplify(ideal I)
{
    int i,j;
    bool flag;
    #if 0
    for(i=IDELEMS(I)-2;i>=0;i--)
    {
        flag=TRUE;
        for(j=1;(j<=currRing->N)&&(flag);j++)
        {
            if(p_GetExp(I->m[i], j, currRing) < p_GetExp(I->m[IDELEMS(I)-1], j, currRing))
            {
                flag=FALSE;
            }
        }
        if(flag)
        {
            I->m[i]=NULL;
        }
    }
    #else
    for(i=IDELEMS(I)-2;i>=0;i--)
    {
        if(p_DivisibleBy(I->m[IDELEMS(I)-1], I->m[i], currRing))
        {
            I->m[i] = NULL;
        }
    }
    #endif
    idSkipZeroes(I);
    return(I);
}

//Puts in sorted vector the degrees of generators of I
static void PutIn(int degvalue, int oldposition, int where)
{
    int i;
    for(i=degsort.size()-1;i>=where+2;i--)
    {
        degsort[i] = degsort[i-2];
    }
    degsort[where] = degvalue;
    degsort[where+1] = oldposition;
}

//it sorts the ideal by the degrees
static ideal SortByDeg(ideal I)
{
    //idPrint(I);
    clock_t t;
    t=clock();
    if(idIs0(I))
    {
        return(I);
    }
    ideal res = idInit(IDELEMS(I),1);
    std::vector<int> deg;
    deg.resize(IDELEMS(I));
    int i,j;
    bool flag;
    for(i=0;i<IDELEMS(I);i++)
    {
        deg[i] = DegMon(I->m[i]);
    }
    degsort.resize(2);
    degsort[0] = deg[0];
    degsort[1] = 0;
    for(i=1;i<IDELEMS(I);i++)
    {
        degsort.resize(degsort.size()+2);
        flag=TRUE;
        if(deg[i]<=degsort[0])
        {
            flag=FALSE;
            PutIn(deg[i], i, 0);
        }
        if((deg[i]>=degsort[degsort.size()-2]) && (flag == TRUE))
        {
            flag=FALSE;
            PutIn(deg[i], i, degsort.size()-2);
        }
        if(flag==TRUE)
        {
            for(j=2;(j<=degsort.size()-4)&&(flag);j=j+2)
            {
                if(deg[i]<=degsort[j])
                {
                    flag=FALSE;
                    PutIn(deg[i], i, j);
                }
            }
        }
    }
    //for(i = 0;i<degsort.size();i++)
    //{printf(" %i",degsort[i]);}printf("\n");printf("I has %i el",IDELEMS(I));getchar();
    for(i=0;i<IDELEMS(I);i++)
    {
        res->m[i] = pCopy(I->m[degsort[2*i+1]]);
    }
    //idPrint(res);
    degsort.clear();
    degsort.resize(0);
    /*if(((float)(clock()-t)/CLOCKS_PER_SEC)!=0)
    {
        printf("\nSortDeg took %f\n",(float)(clock()-t)/CLOCKS_PER_SEC);
    }*/
    return(res);
}

//id_Add for monomials (hoping it is faster)
static ideal idAddMon(ideal I, ideal p)
{
    //printf("\nI and p\n");idPrint(I);pWrite(p->m[0]);
    #if 1
    idInsertPoly(I,p->m[0]);
    #else
    I = id_Add(I,p,currRing);
    #endif
    idSkipZeroes(I);
    I = idSimplify(I);
    //printf("\nBLA in idAddMon\n");idPrint(I);
    I = SortByDeg(I);
    return(I);
}

#if 0
//Constructs a list of the simplices (for which we have to compute the Euler Characteristic)
static void eulerSimpAdd(ideal newsimp)
{
    if(idIs0(newsimp))
    {
        return;
    }
    ideal P;
    int i;
    bool flag;
    P = idInit(1,1);
    for(i=0;i<IDELEMS(newsimp);i++)
    {
        P->m[0] = pAdd(P->m[0], pCopy(newsimp->m[i]));
    }
    poly p = pCopy(P->m[0]);
    //idPrint(eulersimp);
    //idPrint(newsimp);
    //idPrint(P);
    int terms = 0, deg = 0, howmanyvars = 0;
    terms = IDELEMS(newsimp);
    deg = DegMon(newsimp->m[0]);
    flag=TRUE;
    for(i=1;(i<IDELEMS(newsimp))&&(flag==TRUE);i++)
    {
        
        if(deg != DegMon(newsimp->m[i]))
        {
            //The ideal is not homogenous
            flag=FALSE;
        }
    }
    if(!flag)
    {
        deg = 0;
    }
    int* e;
    howmanyvars = pGetVariables(p, e);
    flag = FALSE;
    int thisone;
    //idPrint(eulersimp);
    if(idIs0(eulersimp))
    {
        eulersimp = idAddMon(eulersimp, P);
        eulerprop.resize(eulerprop.size()+3);
        eulerprop[eulerprop.size()-3] = terms ;
        eulerprop[eulerprop.size()-2] = deg ;
        eulerprop[eulerprop.size()-1] = howmanyvars ;
        return;
    }
    //for(i=0;i<IDELEMS(eulersimp);i++)
    //{
    //    printf("\n%i %i %i\n",eulerprop[3*i],eulerprop[3*i+1],eulerprop[3*i+2]);
    //}
    i=0;
    
    for(i = 0; (i<IDELEMS(eulersimp)) && (!flag);i++)
    {
        if((eulerprop[3*i] == terms) && (eulerprop[3*i+1] == deg) && (eulerprop[3*i+2] == howmanyvars))
        {
            //  !!!!!!!!!!!!!!!!!!!have to do it!!!!!!!!!!!!!!!!!!!!!!!!!!
            //if(TestIfIsSame(p, eulersimp->m[i]))
            {
                flag = TRUE;
                thisone = i;
                //It is the the same simplex, and hence we already know it's euler characteristic
            }
        }
    }
    if(!flag)
    {
        eulersimp = idAddMon(eulersimp, P);
        eulerprop.resize(eulerprop.size()+3);
        eulerprop[eulerprop.size()-3] = terms ;
        eulerprop[eulerprop.size()-2] = deg ;
        eulerprop[eulerprop.size()-1] = howmanyvars ;
    }
    if(flag)
    {
        eulerskips++;
        hilbertcoef = (mpz_ptr)omRealloc(hilbertcoef, (NNN+1)*sizeof(mpz_t));
        mpz_init(&hilbertcoef[NNN]);
        mpz_set(&hilbertcoef[NNN], &hilbertcoef[thisone]);
        hilbertpowers.resize(NNN+1);
        NNN++;
    }
    return;
}
#endif

//searches for a variable that is not yet used (assumes that the ideal is sqrfree)
static poly ChoosePVar (ideal I)
{
    idPrint(I);
    printf("\nThere is a variable which is not yet used... ChangePVar\n");getchar();
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
    //idPrint(I);
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
    //printf("\nMy choice was \n");pWrite(m);getchar();
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
static void eulerchar (ideal I, int variables)
{
  loop
  {
    //idPrint(I);printf("%i", variables);
    eulerstep++;
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
    //pWrite(p->m[0]);
    ideal Ip = idQuotMon(I,p);
    Ip = SortByDeg(Ip);
    int i,howmanyvarinp = 0;
    for(i = 1;i<=currRing->N;i++)
    {
        if(p_GetExp(p->m[0],i,currRing)>0)
        {
            howmanyvarinp++;
        }
    }    
    eulerchar(Ip, variables-howmanyvarinp);
    id_Delete(&Ip, currRing);
    //ideal Iplusp = idAddMon(I,p);
    //eulerchar(Iplusp, variables);
    //id_Delete(&Iplusp, currRing);
    //return;
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
    //if(DegMon(p)<DegMon(I->m[IDELEMS(I)-1]))
    //{
    //    return(FALSE);
    //}
    #if 0
    if(p == NULL)
    {
        if(IDELEMS(I) == 0)
        {
            return(TRUE);
        }
        if(IDELEMS(I) == 1)
        {
            if(I->m[0] == poly(0))
            {
                return(TRUE);
            }
        }
        return(FALSE);
    }
    if(IDELEMS(I)==0)
    {
        return(FALSE);
    }
    if(IDELEMS(I) == 1)
    {
        if(I->m[0] == poly(0))
            {
                return(FALSE);
            }    
    }
    #endif
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

#if 0
//return TRUE if a | b, a, b monomials
static bool Divides(poly a, poly b)
{
    if(a == NULL)
    {
        return(FALSE);
    }
    if(b == NULL)
    {
        return(TRUE);
    }
    int i;
    for(i=1;i<=currRing->N;i++)
    {
        if(p_GetExp(a,i,currRing) > p_GetExp(b,i,currRing))
        {
            return(FALSE);
        }
    }
    return(TRUE);
}
#endif

//the Roune Slice Algorithm
void rouneslice(ideal I, ideal S, poly q, poly x)
{
  loop
  {
    steps++;
    int i,j;
    int dummy;
    mpz_t dummympz;
    poly m;
    clock_t t;
    t=clock();
    ideal p, koszsimp;
    //----------- PRUNING OF S ---------------
    //S SHOULD IN THIS POINT BE ORDERED BY DEGREE
    for(i=IDELEMS(S)-1;i>=0;i--)
    {
        if(IsIn(S->m[i],I))
        {
            //idPrint(I);idPrint(S);getchar();
            S->m[i]=NULL;
            prune++;
        }
    }
    idSkipZeroes(S);
    /*if(((float)(clock()-t)/CLOCKS_PER_SEC)!=0)
    {
        printf("\nPruning S took %f\n",(float)(clock()-t)/CLOCKS_PER_SEC);
        //idPrint(I);idPrint(S);getchar();
    }*/
    //----------------------------------------
    t = clock();
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
        //printf("\n Check if m is in S: %i\n", IsIn(m,S));pWrite(m);idPrint(S);
        if(IsIn(m,S))
        {
            //printf("\nIt was deleted: \n");pWrite(I->m[i]);
            I->m[i]=NULL;
            //printf("\n Deleted, since pi(m) is in S\n");pWrite(m);
        }
    }
    idSkipZeroes(I);
    /*if(((float)(clock()-t)/CLOCKS_PER_SEC)!=0)
    {
        printf("\nConstructing I' took %f\n",(float)(clock()-t)/CLOCKS_PER_SEC);
    }*/
    //----------- MORE PRUNING OF S ------------
    t = clock();
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
    /*if(((float)(clock()-t)/CLOCKS_PER_SEC)!=0)
    {
        printf("\nMore Pruning S took %f\n",(float)(clock()-t)/CLOCKS_PER_SEC);
    }*/
    //------------------------------------------
    I = SortByDeg(I);
    /*printf("\n---------------------------\n");
    printf("\n      I\n");idPrint(I);
    printf("\n      S\n");idPrint(S);
    printf("\n      q\n");pWrite(q);
    getchar();*/
    
    if(idIs0(I))
    {
        /*if(!pIsConstantPoly(q))
        {
            mpz_init(dummympz);
            mpz_set_si(dummympz, -1);
            hilbertcoef = (mpz_ptr)omRealloc(hilbertcoef, (NNN+1)*sizeof(mpz_t));
            mpz_init(&hilbertcoef[NNN]);
            mpz_set(&hilbertcoef[NNN], dummympz);
            hilbertpowers.resize(NNN+1);
            hilbertpowers[NNN] = DegMon(q);
            NNN++;
            pWrite(q);
            //printf("\nOld ERROR\n");getchar();
        }*/
        id_Delete(&I, currRing);
        id_Delete(&S, currRing);
        p_Delete(&m, currRing);
        break;
        //return;
    }
    S = SortByDeg(S);
    m = LCMmon(I);
    if(!p_DivisibleBy(x,m, currRing))
    {
        //printf("\nx does not divide lcm(I)");
        //pWrite(x);pWrite(m);idPrint(I);
        //printf("\nEmpty set");pWrite(q);
        //idPrint(S);pWrite(q);
        //getchar();
        id_Delete(&I, currRing);
        id_Delete(&S, currRing);
        p_Delete(&m, currRing);
        //return;
        break;
    }
    m = SqFree(I);
    koszsimp = idInit(IDELEMS(I),1);
    if(m==NULL)
    {
        //printf("\n      Corner: ");
        //pWrite(q);
        //printf("\n      With the facets of the dual simplex:\n");
        for(i=IDELEMS(I)-1;i>=0;i--)
        {
            koszsimp->m[i] = I->m[i];
            //pWrite(koszsimp->m[i]);
            /*for(j = 1;j<=currRing->N;j++)
            {
                if(p_GetExp(koszsimp->m[i],j,currRing)!=0)
                {
                    p_SetExp(koszsimp->m[i],j,0,currRing);
                }
                else
                {
                    p_SetExp(koszsimp->m[i],j,1,currRing);
                }
            }*/
            //pWrite(koszsimp->m[i]);
        }
        //idPrint(koszsimp);
        //getchar();
        //printf("\n Euler Characteristic = ");
        mpz_init(dummympz);
        t = clock();
        eulerstep=0;
        //idPrint(koszsimp);
        //getchar();
        eulerchar(koszsimp, currRing->N);
        if(((float)(clock()-t)/CLOCKS_PER_SEC)!=0)
        {
            //printf("\nEulerChar took %f \n",(float)(clock()-t)/CLOCKS_PER_SEC);
            //idPrint(koszsimp);getchar();
        }
        //printf("\nEulerChar tooked %i steps\n", eulerstep);
        //gmp_printf("dummy %Zd \n", dummympz);
        //gmp_printf("ec %Zd \n", ec);
        //getchar();
        //if(DegMon(q)==7) {getchar();}
        mpz_set(dummympz, ec);
        mpz_sub(ec, ec, ec);
        hilbertcoef = (mpz_ptr)omRealloc(hilbertcoef, (NNN+1)*sizeof(mpz_t));
        mpz_init(&hilbertcoef[NNN]);
        mpz_set(&hilbertcoef[NNN], dummympz);
        mpz_clear(dummympz);
        hilbertpowers.resize(NNN+1);
        hilbertpowers[NNN] = DegMon(q);
        NNN++;
        break;
        //return;
    }
    m = ChooseP(I);
    p = idInit(1,1);
    p->m[0] = m;
    //idPrint(I);idPrint(p);
    //idTest(I);idTest(p);
    ideal Ip = idQuotMon(I,p);
    Ip = SortByDeg(Ip);
    ideal Sp = idQuotMon(I,p);
    Sp = SortByDeg(Sp);
    //printf("\np, q = \n");pWrite(m);pWrite(q);
    poly pq = pp_Mult_mm(q,m,currRing);
    rouneslice(Ip, Sp, pq, x);
    //idPrint(Ip);
    //id_Delete(&Ip, currRing);
    //id_Delete(&Sp, currRing);
    S = idAddMon(S,p);
    //p->m[0]=NULL; id_Delete(&p, currRing); // p->m[0] was also in S
    p_Delete(&pq,currRing);
    //Splusp = idSimplify(Splusp);
  //  rouneslice(I, S, q, x);  tail-recursion solved via loop

    //id_Delete(&Splusp, currRing);
    //return;
  }
}


//it computes the first hilbert series by means of Roune Slice Algorithm
void slicehilb(ideal I)
{
    I = SortByDeg(I);
    printf("Adi changes are here: \n");
    int i;
    eulersimp = idInit(1,1);
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
    printf("\n-------------RouneSlice--------------\n");
    steps=0;
    rouneslice(I,S,q,X->m[0]);
    printf("\nIn total Prune got rid of %i elements\n",prune);
    printf("\nIn total More Prune got rid of %i elements\n",moreprune);
    //printf("\nWe skipped computing the EulerChar for %i simplices\n",eulerskips);
    printf("\nSteps of rouneslice: %i\n\n", steps);
    /*for(i=0;i<NNN;i++)
    {
        gmp_printf(" %Zd ", &hilbertcoef[i]);
    }
    printf("\n");
    for(i=0;i<NNN;i++)
    {
        printf(" %d ", hilbertpowers[i]);
    }*/
    SortPowerVec();
    /*printf("\n");
    for(i=0;i<hilbertpowerssorted.size();i++)
    {
        printf(" %d ", hilbertpowerssorted[i]);
    }*/
    mpz_t coefhilb;
    mpz_t dummy;
    mpz_init(coefhilb);
    mpz_init(dummy);
    printf("\n//  %8d t^0",1);
    for(i=0;i<hilbertpowerssorted.size();i=i+2)
    {
        mpz_set(dummy, &hilbertcoef[hilbertpowerssorted[i+1]]);
        mpz_add(coefhilb, coefhilb, dummy);
        while((hilbertpowerssorted[i]==hilbertpowerssorted[i+2]) && (i<=hilbertpowerssorted.size()-2))
        {
            i=i+2;
            mpz_add(coefhilb, coefhilb, &hilbertcoef[hilbertpowerssorted[i+1]]);
        }
        if(mpz_sgn(coefhilb)!=0)
        {
            gmp_printf("\n//  %8Zd t^%d",coefhilb,hilbertpowerssorted[i]);
        }
        mpz_sub(coefhilb, coefhilb, coefhilb);
    }
    omFreeSize(hilbertcoef, NNN*sizeof(mpz_t));
    printf("\n-------------------------------------\n");
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
  clock_t t;    // ADICHANGES
  t = clock(); // ADICHANGES
  int co, mu, l;
  intvec *hseries2;
  intvec *hseries1 = hFirstSeries(S, modulweight, Q);
  hPrintHilb(hseries1);
  printf("\nTime for Original: %f\n",((float)(clock()-t))/CLOCKS_PER_SEC); getchar(); // ADICHANGES
  #if 0
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
  #endif
  clock_t now;    // ADICHANGES
  now = clock(); // ADICHANGES
  slicehilb(S); // ADICHANGES
  printf("\nTime for Slice Algorithm: %f \n\n", ((float)(clock()-now))/CLOCKS_PER_SEC);    // ADICHANGES
  
}




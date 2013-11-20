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


static int  **Qpol;
static int  *Q0, *Ql;
static int  hLength;
//ADICHANGES
static int NNN;
static mpz_t ec;
static mpz_ptr hilbertcoef = (mpz_ptr)omAlloc(NNN*sizeof(mpz_t));
std::vector<int> hilbertpowers;
std::vector<int> hilbertpowerssorted;
    

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
void PutInVector(int degvalue, int oldposition, int where)
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

void SortPowerVec()
{
    int i,j;
    int test;
    bool flag;
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
        if(hilbertpowers[i]>=hilbertpowerssorted[hilbertpowerssorted.size()-2])
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
        //printf("\n----------------TEST----------------\n");
        //for(test=0;test<hilbertpowerssorted.size();test++)
        //    {
        //        printf(" %d ", hilbertpowerssorted[test]);
        //    }   
        
    }
}

int DegMon(poly p, ring tailRing)
{
    int i,deg;
    deg = 0;
    for(i=0;i<=tailRing->N;i++)
    {
        deg = deg + p_GetExp(p, i, tailRing);
    }
    return(deg);
}

poly SearchP(ideal I, ring tailRing)
{
    int i,j,exp;
    poly res;
    for(i=IDELEMS(I)-1;i>=0;i--)
    {
        if(DegMon(I->m[i], tailRing)>=2)
        {
            res = p_ISet(1, tailRing);
            res = p_Copy(I->m[i], tailRing);
            for(j=0;j<=tailRing->N;j++)
            {
                exp = p_GetExp(I->m[i], j, tailRing);
                if(exp > 0)
                {
                    p_SetExp(res, j, exp - 1, tailRing);
                    return(res);
                }
            }
        }
    }
    return(NULL);
}

poly ChoosePVar (ideal I, ring tailRing)
{
    bool flag=TRUE;
    int i,j;
    poly res;
    for(i=1;i<=tailRing->N;i++)
    {
        flag=TRUE;
        for(j=IDELEMS(I)-1;(j>=0)&&(flag);j--)
        {
            if(p_GetExp(I->m[j], i, tailRing)>0)
            {
                flag=FALSE;
            }
        }
        
        if(flag == TRUE)
        {
            res = p_ISet(1, tailRing);
            p_SetExp(res, i, 1, tailRing);
            //idPrint(I);
            //pWrite(res);
            return(res);
        }
    }
    return(NULL); //i.e. it is the maximal ideal
}

ideal idSimplify(ideal I, ring tailRing)
{
    int i,j;
    bool flag;
    /*std::vector<int>  var;
    for(i=0;i<=tailRing->N;i++)
    {
        if(p_GetExp(I->[IDELEMS(I)-1], tailRing)>0)
        {
            var.resize(var.size()+1);
            var[var.size()-1] = i;
        }
    }
    */
    for(i=IDELEMS(I)-2;i>=0;i--)
    {
        flag=TRUE;
        for(j=0;(j<=tailRing->N)&&(flag);j++)
        {
            if(p_GetExp(I->m[i], j, tailRing) < p_GetExp(I->m[IDELEMS(I)-1], j, tailRing))
            {
                flag=FALSE;
            }
        }
        if(flag)
        {
            I->m[i]=NULL;
        }
    }
    idSkipZeroes(I);
    return(I);
}


void eulerchar (ideal I, ring tailRing)
{
    //gmp_printf("\nEuler char: %Zd\n", ec);
    mpz_t dummy;
    if( idElem(I) == 0)
    {
        mpz_init(dummy);
        //change: era 1 in loc de 0
        mpz_set_si(dummy, 1);
        mpz_sub(ec, ec, dummy);
        return;
    }
    if( idElem(I) == 1)
    {
        if(!p_IsOne(I->m[0], tailRing))
        {
            //aici nu era nimic
            //mpz_set_si(dummy, 1);
            //mpz_add(ec, ec, dummy);
            return;
        }
        else
        {
            mpz_init(dummy);
            mpz_set_si(dummy, 1);
            mpz_sub(ec, ec, dummy);
            return;
        }
    }
    ideal p = idInit(1,1);
    p->m[0] = SearchP(I, tailRing);
    //idPrint(I);
    //printf("\nSearchP founded: \n");pWrite(p->m[0]);
    if(p->m[0] == NULL)
    {
        mpz_init(dummy);
        mpz_set_si(dummy, IDELEMS(I)-1);
        mpz_add(ec, ec, dummy);
        return;
    }
    ideal Ip = idQuot(I,p,TRUE,TRUE);
    ideal Iplusp = id_Add(I,p,tailRing);
    Iplusp=idSimplify(Iplusp, tailRing);
    //mpz_t i,j;
    //i = eulerchar(Ip, ec, tailRing);
    //j = eulerchar(Iplusp, ec, tailRing);
    //mpz_add(ec, i,j);
    eulerchar(Ip, tailRing);
    eulerchar(Iplusp, tailRing);
    return;
}

poly SqFree (ideal I, ring tailRing)
{
    int i,j;
    bool flag=TRUE;
    poly notsqrfree = NULL;
    for(i=IDELEMS(I)-1;(i>=0)&&(flag);i--)
    {
        for(j=0;(j<=tailRing->N)&&(flag);j++)
        {
            if(p_GetExp(I->m[i],j,tailRing)>1)
            {
                flag=FALSE;
                notsqrfree = p_ISet(1,tailRing);
                p_SetExp(notsqrfree,j,1,tailRing);
            }
        }
    }
    return(notsqrfree);
}

void rouneslice(ideal I, ideal S, poly q, ring tailRing, poly x)
{
    int i,j;
    int dummy;
    mpz_t dummympz;
    poly m;
    ideal p, koszsimp;
    I = idMinBase(I);
    
    //Work on it
    //----------- PRUNING OF S ---------------
    S = idMinBase(S);
    for(i=IDELEMS(S)-1;i>=0;i--)
    {
        if(kNF(I,NULL,S->m[i],NULL,NULL)==NULL)
        {
            S->m[i]=NULL;
        }
    }
    idSkipZeroes(S);
    //----------------------------------------
    for(i=IDELEMS(I)-1;i>=0;i--)
    {
        m = p_Copy(I->m[i],tailRing);
        for(j=0;j<=tailRing->N;j++)
        {
            dummy = p_GetExp(m,j,tailRing);
            if(dummy > 0)
            {
                p_SetExp(m,j,dummy-1,tailRing);
            }       
        }
        if(kNF(S,NULL,m,NULL,NULL)==NULL)
        {
            I->m[i]=NULL;
            //printf("\n Check if m is in S: \n");pWrite(m);idPrint(S);
            //printf("\n Deleted, since pi(m) is in S\n");pWrite(m);
        }
    }
    idSkipZeroes(I);
    
    //----------- MORE PRUNING OF S ------------
    //strictly divide???    
    //------------------------------------------

    //printf("Ideal I: \n");idPrint(I);
    //printf("Ideal S: \n");idPrint(S);
    //printf("Poly  q: \n");pWrite(q);
    if(idElem(I)==0)
    {
        //I = 0
        //printf("\nx does not divide lcm(I)");
        //pWrite(x);pWrite(m);idPrint(I);
        printf("\nEmpty Set: ");pWrite(q);
        return;
    }
    m = p_ISet(1,tailRing);
    for(i=0;i<=tailRing->N;i++)
    {
        dummy=0;
        for(j=IDELEMS(I)-1;j>=0;j--)
        {
            if(p_GetExp(I->m[j],i,tailRing) > dummy)
            {
                dummy = p_GetExp(I->m[j],i,tailRing);
            }
        }
        p_SetExp(m,i,dummy,tailRing);
    }
    p_Setm(m,tailRing);
    if(p_DivisibleBy(x,m,tailRing)==FALSE)
    {
        //printf("\nx does not divide lcm(I)");
        //pWrite(x);pWrite(m);idPrint(I);
        //printf("\nEmpty set");pWrite(q);
        return;
    }
    m = SqFree(I, tailRing);
    koszsimp = idInit(IDELEMS(I),1);
    if(m==NULL)
    {
        printf("\n      Corner: ");
        pWrite(q);
        printf("\n      With the facets of the simplex:\n");
        for(i=IDELEMS(I)-1;i>=0;i--)
        {
            m = p_ISet(1,tailRing);
            //m = p_Divide(x,I->m[i],tailRing);
            for(j=tailRing->N; j>=0; j--)
            {
                p_SetExp(m,j, p_GetExp(x,j,tailRing)- p_GetExp(I->m[i],j,tailRing),tailRing);
            }
            printf("    ");
            p_Setm(m, tailRing);
            p_Write(m, tailRing);
            koszsimp->m[i] = p_Copy(m, tailRing);
        }
        printf("\n Euler Characteristic = ");
        mpz_init(dummympz);
        eulerchar(koszsimp, tailRing);
        //gmp_printf("dummy %Zd \n", dummympz);
        gmp_printf("ec %Zd \n", ec);
        mpz_set(dummympz, ec);
        mpz_sub(ec, ec, ec);
        hilbertcoef = (mpz_ptr)omRealloc(hilbertcoef, (NNN+1)*sizeof(mpz_t));
        mpz_init(&hilbertcoef[NNN]);
        mpz_set(&hilbertcoef[NNN], dummympz);
        hilbertpowers.resize(NNN+1);
        hilbertpowers[NNN] = DegMon(q, tailRing);
        NNN++;
        return;
    }
    if(IDELEMS(S)!=1)
    {
        m = SearchP(S, tailRing);
        if(m == NULL)
        {
            m = ChoosePVar(S, tailRing);
        }
    }
    p = idInit(1,1);
    p->m[0] = m;
    printf("Poly  p: \n");pWrite(m);
    ideal Ip = idQuot(I,p,TRUE,TRUE);
    ideal Sp = idQuot(S,p,TRUE,TRUE);
    ideal Splusp = id_Add(S,p,tailRing);
    Splusp = idSimplify(Splusp, tailRing);
    poly pq = pp_Mult_mm(q,m,tailRing);
    rouneslice(Ip, Sp, pq, tailRing, x);
    rouneslice(I, Splusp, q, tailRing, x);
    return;
}

void slicehilb(ideal I, ring tailRing)
{
    printf("Adi changes are here: \n");
    int i;
    ideal S = idInit(0,1);
    poly q = p_ISet(1,tailRing);
    ideal X = idInit(1,1);
    X->m[0]=p_One(tailRing);
    for(i=1;i<=tailRing->N;i++)
    {
            p_SetExp(X->m[0],i,1,tailRing);   
    }
    p_Setm(X->m[0],tailRing);
    I = id_Mult(I,X,tailRing);
    printf("\n-------------RouneSlice--------------\n");
    rouneslice(I,S,q,tailRing,X->m[0]);
    for(i=0;i<NNN;i++)
    {
        gmp_printf(" %Zd ", &hilbertcoef[i]);
    }
    printf("\n");
    for(i=0;i<NNN;i++)
    {
        printf(" %d ", hilbertpowers[i]);
    }
    SortPowerVec(); 
    printf("\n");
    for(i=0;i<hilbertpowerssorted.size();i++)
    {
        printf(" %d ", hilbertpowerssorted[i]);
    }
    mpz_t coefhilb;
    mpz_t dummy;
    mpz_init(coefhilb);
    mpz_init(dummy);
    printf("\n//        1 t^0");
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
            gmp_printf("\n//        %Zd t^%d",coefhilb,hilbertpowerssorted[i]);
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
  slicehilb(S,tailRing); // ADICHANGES
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




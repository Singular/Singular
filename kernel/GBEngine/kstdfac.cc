/****************************************
*  Computer Algebra System SINGULAR     *
****************************************/
/*
*  ABSTRACT -  Kernel: factorizing alg. of Buchberger
*/

#include "kernel/mod2.h"
#include "misc/options.h"
#include "kernel/polys.h"
#include "kernel/ideals.h"
#include "kernel/GBEngine/kutil.h"
#include "kernel/GBEngine/kstd1.h"
#include "kernel/GBEngine/khstd.h"
#include "polys/weight.h"
#include "misc/intvec.h"
#include "polys/clapsing.h"
#include "kernel/ideals.h"
#include "kernel/GBEngine/kstdfac.h"

THREAD_VAR int strat_nr=0;
/*3
* copy o->T to n->T, assumes that n->S is already copied
*/
static void copyT (kStrategy o,kStrategy n)
{
  int i,j;
  poly  p;
  TSet t=(TSet)omAlloc0(o->tmax*sizeof(TObject));
  TObject** r = (TObject**)omAlloc0(o->tmax*sizeof(TObject*));

  for (j=0; j<=o->tl; j++)
  {
    t[j] = o->T[j];
    r[t[j].i_r] = &t[j];
    p = o->T[j].p;
    i = -1;
    loop
    {
      i++;
      if (i>o->sl)
      {
        t[j].p=pCopy(p);
        break;
      }
      if (p == o->S[i])
      {
        t[j].p=n->S[i];
        break;
      }
    }
    t[j].t_p = NULL; // ?? or t[j].p ??
    t[j].max_exp = NULL; // ?? or p_GetMaxExpP(t[j].t_p,o->tailRing); ??
    t[j].pLength =  pLength(p);
  }
  n->T=t;
  n->R=r;
}

/*3
* copy o->L to n->L, assumes that n->T,n->tail is already copied
*/
static void copyL (kStrategy o,kStrategy n)
{
  int i,j;
  poly  p;
  LSet l=(LSet)omAlloc(o->Lmax*sizeof(LObject));

  for (j=0; j<=o->Ll; j++)
  {
    l[j] = o->L[j];
    // copy .p ----------------------------------------------
    if (pNext(o->L[j].p)!=o->tail)
      l[j].p=pCopy(o->L[j].p);
    else
    {
      l[j].p=p_LmInit(o->L[j].p,currRing);
      if (pGetCoeff(o->L[j].p)!=NULL) pSetCoeff0(l[j].p,nCopy(pGetCoeff(o->L[j].p)));
      pNext(l[j].p)=n->tail;
    }
    // copy .lcm ----------------------------------------------
    if (o->L[j].lcm!=NULL)
      l[j].lcm=pLmInit(o->L[j].lcm);
    else
      l[j].lcm=NULL;
    l[j].p1=NULL;
    l[j].p2=NULL;
    l[j].t_p = NULL;

    // copy .p1 , i_r1----------------------------------------------
    p = o->L[j].p1;
    i = -1;
    loop
    {
      if(p==NULL) break;
      i++;
      if(i>o->tl)
      {
        WarnS("poly p1 not found in T:");wrp(p);PrintLn();
        l[j].p1=pCopy(p);
        l[j].i_r1=-1;
        break;
      }
      if (p == o->T[i].p)
      {
        l[j].p1=n->T[i].p;
        l[j].i_r1=n->T[i].i_r;
        break;
      }
    }

    // copy .p2 , i_r2----------------------------------------------
    p = o->L[j].p2;
    i = -1;
    loop
    {
      if(p==NULL) break;
      i++;
      if(i>o->tl)
      {
        WarnS("poly p2 not found in T:");wrp(p);PrintLn();
        l[j].p2=pCopy(p);
        l[j].i_r2=-1;
        break;
      }
      if (p == o->T[i].p)
      {
        l[j].p2=n->T[i].p;
        l[j].i_r2=n->T[i].i_r;
        break;
      }
    }

    // copy .ecart ---------------------------------------------
    l[j].ecart=o->L[j].ecart;
    // copy .length --------------------------------------------
    l[j].length=o->L[j].length;
    // copy .pLength -------------------------------------------
    l[j].pLength=o->L[j].pLength;
    // copy .sev -----------------------------------------------
    l[j].sev=o->L[j].sev;
    l[j].i_r = o->L[j].i_r;
    //l[j].i_r1 = o->L[j].i_r1;
    //l[j].i_r2 = o->L[j].i_r2;
  }
  n->L=l;
}

kStrategy kStratCopy(kStrategy o)
{
  // int i;
  kTest_TS(o);
  kStrategy s=new skStrategy;
  s->next=NULL;
  s->red=o->red;
  s->initEcart=o->initEcart;
  s->posInT=o->posInT;
  s->posInL=o->posInL;
  s->enterS=o->enterS;
  s->initEcartPair=o->initEcartPair;
  s->posInLOld=o->posInLOld;
  s->enterOnePair=o->enterOnePair;
  s->chainCrit=o->chainCrit;
  s->Shdl=idCopy(o->Shdl);
  s->S=s->Shdl->m;
  s->tailRing = o->tailRing;
  if (o->D!=NULL) s->D=idCopy(o->D);
  else            s->D=NULL;
  s->ecartS=(int *)omAlloc(IDELEMS(o->Shdl)*sizeof(int));
  memcpy(s->ecartS,o->ecartS,IDELEMS(o->Shdl)*sizeof(int));
  s->sevS=(unsigned long *)omAlloc(IDELEMS(o->Shdl)*sizeof(unsigned long));
  memcpy(s->sevS,o->sevS,IDELEMS(o->Shdl)*sizeof(unsigned long));
  s->S_2_R=(int*)omAlloc(IDELEMS(o->Shdl)*sizeof(int));
  memcpy(s->S_2_R,o->S_2_R,IDELEMS(o->Shdl)*sizeof(int));
  s->sevT=(unsigned long *)omAlloc(o->tmax*sizeof(unsigned long));
  memcpy(s->sevT,o->sevT,o->tmax*sizeof(unsigned long));
  if(o->fromQ!=NULL)
  {
    s->fromQ=(int *)omAlloc(IDELEMS(o->Shdl)*sizeof(int));
    memcpy(s->fromQ,o->fromQ,IDELEMS(o->Shdl)*sizeof(int));
  }
  else
    s->fromQ=NULL;
  copyT(o,s);//s->T=...
  s->tail = pInit();
  copyL(o,s);//s->L=...
  s->B=initL();
  s->kHEdge=pCopy(o->kHEdge);
  s->kNoether=pCopy(o->kNoether);
  if (o->NotUsedAxis!=NULL)
  {
    s->NotUsedAxis=(BOOLEAN *)omAlloc(currRing->N*sizeof(BOOLEAN));
    memcpy(s->NotUsedAxis,o->NotUsedAxis,currRing->N*sizeof(BOOLEAN));
  }
  //s->P=s->L[s->Ll+1];
  s->P.Init(o->tailRing);
  s->update=o->update;
  s->posInLOldFlag=o->posInLOldFlag;
  s->kModW = o->kModW;
//   if (o->kModW!=NULL)
//     s->kModW=ivCopy(o->kModW);
//   else
//     s->kModW=NULL;
  s->pairtest=NULL;
  s->sl=o->sl;
  s->mu=o->mu;
  s->tl=o->tl;
  s->tmax=o->tmax;
  s->Ll=o->Ll;
  s->Lmax=o->Lmax;
  s->Bl=-1;
  s->Bmax=setmaxL;
  s->ak=o->ak;
  s->syzComp=o->syzComp;
  s->LazyPass=o->LazyPass;
  s->LazyDegree=o->LazyDegree;
  s->HCord=o->HCord;
  s->lastAxis=o->lastAxis;
  s->interpt=o->interpt;
  s->homog=o->homog;
  s->news=o->news;
  s->newt=o->newt;
  s->kHEdgeFound=o->kHEdgeFound;
  s->honey=o->honey;
  s->sugarCrit=o->sugarCrit;
  s->Gebauer=o->Gebauer;
  s->noTailReduction=o->noTailReduction;
  s->fromT=o->fromT;
  s->noetherSet=o->noetherSet;
#ifdef HAVE_PLURAL
  s->no_prod_crit=o->no_prod_crit;
#endif
  kTest_TS(s);
  return s;
}

BOOLEAN k_factorize(poly p,ideal &rfac, ideal &fac_copy)
{
  int facdeg=currRing->pFDeg(p,currRing);
  ideal fac=singclap_factorize(pCopy(p),NULL,1,currRing);
  int fac_elems;
  fac_elems=IDELEMS(fac);
  rfac=fac;
  fac_copy=idInit(fac_elems,1);

  if ((fac_elems!=1)||(facdeg!=currRing->pFDeg(fac->m[0],currRing)))
  {
    if (TEST_OPT_DEBUG)
    {
      Print("%d factors:\n",fac_elems);
      pWrite(p); PrintS(" ->\n");
      int ii=fac_elems;
      while(ii>0) { ii--;pWrite(fac->m[ii]); }
    }
    else if (TEST_OPT_PROT)
    {
      int ii=fac_elems;
      if (ii>1)
      {
        while(ii>0) { PrintS("F"); ii--; }
      }
    }
    return TRUE;
  }
  else
  {
    pDelete(&(fac->m[0]));
    fac->m[0]=pCopy(p);
  }
  return FALSE;
}

static void completeReduceFac (kStrategy strat, ideal_list FL)
{
  int si;

  strat->noTailReduction = FALSE;
  if (TEST_OPT_PROT)
  {
    PrintLn();
//    if (timerv) writeTime("standard base computed:");
  }
  if (TEST_OPT_PROT)
  {
    Print("(S:%d)",strat->sl);mflush();
  }
  for (si=strat->sl; si>0; si--)
  {
    strat->S[si] = redtailBba(strat->S[si],si-1,strat);
    if (TEST_OPT_INTSTRATEGY)
    {
      strat->S[si]=p_Cleardenom(strat->S[si], currRing);
    }
    if (TEST_OPT_PROT)
    {
      PrintS("-");mflush();
    }
    int i;
    if (strat->redTailChange)
    {
      for(i=strat->tl;i>=0;i--)
      {
        strat->initEcart(&strat->T[i]);
      }
    }
    ideal fac;
    ideal fac_copy;

    if (!k_factorize(strat->S[si],fac,fac_copy))
    {
      idDelete(&fac);
      idDelete(&fac_copy);
      continue;
    }

    deleteInS(si,strat);

    for(i=IDELEMS(fac)-1;i>=0;i--)
    {
      kStrategy n=strat;
      if (i>=1)
      {
        n=kStratCopy(strat); // includes: memset(&n->P,0,sizeof(n->P));
        n->next=strat->next;
        strat->next=n;
      }
      else
      {
        n->P.Init(strat->tailRing);
      }

      n->P.p=fac->m[i];
      //n->P.pLength=pLength(n->P.p); // by initEcart
      n->initEcart(&n->P);
      /* enter P.p into s and L */
      int pos;
      if (n->sl==-1) pos=0;
      else pos=posInS(n,n->sl,n->P.p,n->P.ecart);
      if (TEST_OPT_INTSTRATEGY)
      {
        n->P.p = redtailBba(n->P.p,pos-1,n);
        n->P.pCleardenom();
      }
      else
      {
        pNorm(n->P.p);
        n->P.p = redtailBba(n->P.p,pos-1,n);
      }
      n->P.pLength=pLength(n->P.p);
      if (TEST_OPT_DEBUG)
      {
        Print("new s(%d)->S:",n->nr);
        pWrite(n->P.p);
      }
      enterpairs(n->P.p,n->sl,n->P.ecart,pos,n);
      enterT(n->P,n);
      n->enterS(n->P,pos,n, n->tl);

      /* construct D */
      if (IDELEMS(fac)>1)
      {
        if (n->D==NULL)
        {
          n->D=idCopy(fac_copy);
          idSkipZeroes(n->D);
        }
        else
        {
          idTest(n->D);
          ideal r=idAdd(n->D,fac_copy);
          idDelete(&n->D);
          n->D=r;
        }
        if (TEST_OPT_DEBUG)
        {
          Print("new s(%d)->D:\n",n->nr);
          iiWriteMatrix((matrix)n->D,"D",1,currRing,0);
          PrintLn();
        }
      }

      fac_copy->m[i]=pCopy(fac->m[i]);
      fac->m[i]=NULL;

      /* check for empty sets */
      if (n->D!=NULL)
      {
        int j=IDELEMS(n->D)-1;
        while(j>=0)
        {
          if (n->D->m[j]!=NULL)
          {
            poly r=kNF(n->Shdl,NULL,n->D->m[j],0,KSTD_NF_LAZY | KSTD_NF_NONORM);
            if (r==NULL)
            {
              if (TEST_OPT_DEBUG)
              {
                Print("empty set s(%d) because D[%d]:",n->nr,j);
                pWrite(n->D->m[j]);
                messageSets(n);
              }
              while (n->Ll >= 0) deleteInL(n->L,&n->Ll,n->Ll,n);
              while (n->tl >= 0)
              {
                int i=n->sl;
                while (i>=0)
                {
                  if (n->S[i]==n->T[n->tl].p)
                  {
                    n->T[n->tl].p=NULL; n->S[i]=NULL;
                    break;
                  }
                  i--;
                }
                pDelete(&n->T[n->tl].p);
                n->tl--;
              }
              memset(n->Shdl->m,0,IDELEMS(n->Shdl)*sizeof(poly));
              n->sl=-1;
              if (strat==n) si=-1;
              break;
            }
            else
            {
              pDelete(&r);
            }
          }
          j--;
        }
      }
      /* check for empty sets */
      {
        ideal_list Lj=FL;
        while (Lj!=NULL)
        {
          if ((n->sl>=0)&&(n->S[0]!=NULL))
          {
            ideal r=kNF(n->Shdl,NULL,Lj->d,0,KSTD_NF_LAZY | KSTD_NF_NONORM);
            if (idIs0(r))
            {
              if (TEST_OPT_DEBUG)
              {
                Print("empty set because:L[%p]\n",(void *)Lj);
                iiWriteMatrix((matrix)Lj->d,"L",1,currRing,0);
              }
              while (n->Ll >= 0) deleteInL(n->L,&n->Ll,n->Ll,n);
              while (n->tl >= 0)
              {
                int i=n->sl;
                while (i>=0)
                {
                  if (n->S[i]==n->T[n->tl].p)
                  {
                    n->T[n->tl].p=NULL; n->S[i]=NULL;
                    break;
                  }
                  i--;
                }
                pDelete(&n->T[n->tl].p);
                n->tl--;
              }
              memset(n->Shdl->m,0,IDELEMS(n->Shdl)*sizeof(poly));
              n->sl=-1;
              if (strat==n) si=-1;
              idDelete(&r);
              break;
            }
            idDelete(&r);
          }
          Lj=Lj->next;
        }
      }
    } /* for */
    for(i=0;i<IDELEMS(fac);i++) fac->m[i]=NULL;
    idDelete(&fac);
    idDelete(&fac_copy);
    if ((strat->Ll>=0) && (strat->sl>=0)) break;
    else si=strat->sl+1;
  }
}

ideal bbafac (ideal /*F*/, ideal Q,intvec */*w*/,kStrategy strat, ideal_list FL)
{
  int   olddeg,reduc=0;
  int red_result = 1;
  reduc = olddeg = 0;
  /* compute------------------------------------------------------- */
  if ((strat->Ll==-1) && (strat->sl>=0))
  {
    if (TEST_OPT_REDSB) completeReduceFac(strat,FL);
  }
  kTest_TS(strat);
  while (strat->Ll >= 0)
  {
    if (TEST_OPT_DEBUG) messageSets(strat);
    if (strat->Ll== 0) strat->interpt=TRUE;
    if (TEST_OPT_DEGBOUND
    && ((strat->honey
        && (strat->L[strat->Ll].ecart+currRing->pFDeg(strat->L[strat->Ll].p,currRing)>Kstd1_deg))
      || ((!strat->honey) && (currRing->pFDeg(strat->L[strat->Ll].p,currRing)>Kstd1_deg))))
    {
      /*
      *stops computation if
      * 24 IN test and the degree +ecart of L[strat->Ll] is bigger then
      *a predefined number Kstd1_deg
      */
      while (strat->Ll >= 0) deleteInL(strat->L,&strat->Ll,strat->Ll,strat);
      break;
    }
    /* picks the last element from the lazyset L */
    strat->P = strat->L[strat->Ll];
    strat->Ll--;
    if (pNext(strat->P.p) == strat->tail)
    {
      /* deletes the short spoly and computes */
      pLmFree(strat->P.p);
      /* the real one */
      strat->P.p = ksOldCreateSpoly(strat->P.p1,
                                    strat->P.p2,
                                    strat->kNoether);
    }
    if (strat->honey)
    {
      if (TEST_OPT_PROT)
        message(strat->P.ecart+currRing->pFDeg(strat->P.p,currRing),&olddeg,&reduc,strat, red_result);
    }
    else
    {
      if (TEST_OPT_PROT)
        message(currRing->pFDeg(strat->P.p,currRing),&olddeg,&reduc,strat, red_result);
    }
    /* reduction of the element chosen from L */
    kTest_TS(strat);
    red_result = strat->red(&strat->P,strat);
    if (strat->P.p != NULL)
    {
      /* statistic */
      if (TEST_OPT_PROT) PrintS("s");
      ideal fac;
      ideal fac_copy;

      if (!k_factorize(strat->P.p,fac,fac_copy))
      {
        if (TEST_OPT_INTSTRATEGY)
        {
          strat->P.p = redtailBba(strat->P.p,strat->sl,strat);
          if (strat->redTailChange) strat->P.pCleardenom();
        }
        else
        {
          pNorm(strat->P.p);
          strat->P.p = redtailBba(strat->P.p,strat->sl,strat);
        }
        if (strat->redTailChange)
        {
          idDelete(&fac);
          idDelete(&fac_copy);
          if (!k_factorize(strat->P.p,fac,fac_copy))
          {
            pDelete(&(fac->m[0]));
            fac->m[0]=strat->P.p;
            strat->P.p=NULL;
          }
          else
          {
            pDelete(&strat->P.p);
          }
        }
      }
      kDeleteLcm(&strat->P);
      int i;

      for(i=IDELEMS(fac)-1;i>=0;i--)
      {
        int ii;
        kStrategy n=strat;
        if (i>=1)
        {
          n=kStratCopy(strat); // includes memset(&n->P,0,sizeof(n->P));
          kTest_TS(n);
          n->next=strat->next;
          strat->next=n;
        }
        else
        {
          n->P.Init(strat->tailRing);
        }

        n->P.p=fac->m[i];
        //n->P.pLength=pLength(n->P.p); // by initEcart
        n->initEcart(&n->P);
        kTest_TS(n);

        /* enter P.p into s and L */
        int pos;
        if (n->sl==-1) pos=0;
        else pos=posInS(n,n->sl,n->P.p,n->P.ecart);

        // we have already reduced all elements from fac....
        if (TEST_OPT_INTSTRATEGY)
        {
          n->P.p = redtailBba(n->P.p,pos-1,n);
          if (n->redTailChange)
          {
            n->P.pCleardenom();
            n->P.pLength=pLength(n->P.p);
          }
        }
        else
        {
          pNorm(n->P.p);
          n->P.p = redtailBba(n->P.p,pos-1,n);
          if (n->redTailChange)
          {
            n->P.pLength=pLength(n->P.p);
          }
        }
        kTest_TS(n);

        if (TEST_OPT_DEBUG)
        {
          PrintS("new s:");
          wrp(n->P.p);
          PrintLn();
        }
        enterpairs(n->P.p,n->sl,n->P.ecart,pos,n);
        enterT(n->P,n);
        n->enterS(n->P,pos,n, n->tl);
        {
          int i=n->Ll;
          for(;i>=0;i--)
          {
            n->L[i].i_r1= -1;
            for(ii=0; ii<=n->tl; ii++)
            {
              if (n->R[ii]->p==n->L[i].p1)  { n->L[i].i_r1=ii;break; }
            }
            n->L[i].i_r2= -1;
            for(ii=0; ii<=n->tl; ii++)
            {
              if (n->R[ii]->p==n->L[i].p2)  { n->L[i].i_r2=ii;break; }
            }
          }
        }
        kTest_TS(n);
        /* construct D */
        if (IDELEMS(fac)>1)
        {
          if (n->D==NULL)
          {
            n->D=idCopy(fac_copy);
            idSkipZeroes(n->D);
          }
          else
          {
            idTest(n->D);
            ideal r=idAdd(n->D,fac_copy);
            idDelete(&n->D);
            n->D=r;
          }
          if (TEST_OPT_DEBUG)
          {
            PrintS("new D:\n");
            iiWriteMatrix((matrix)n->D,"D",1,currRing,0);
            PrintLn();
          }
        }

        fac_copy->m[i]=pCopy(fac->m[i]);
        fac->m[i]=NULL;

        /* check for empty sets */
        if (n->D!=NULL)
        {
          int j=IDELEMS(n->D)-1;
          while(j>=0)
          {
            if (n->D->m[j]!=NULL)
            {
              poly r=kNF(n->Shdl,NULL,n->D->m[j],0,KSTD_NF_LAZY | KSTD_NF_NONORM);
              if (r==NULL)
              {
                if (TEST_OPT_DEBUG)
                {
                  Print("empty set s(%d) because: D[%d]:", n->nr,j);
                  pWrite(n->D->m[j]);
                  messageSets(n);
                }
                //if (n->Ll >=0) Print("Ll:%d|",n->Ll);
                while (n->Ll >= 0) deleteInL(n->L,&n->Ll,n->Ll,n);
                //if (n->tl >=0) Print("tl:%d|",n->tl);
                while (n->tl >= 0)
                {
                  int i=n->sl;
                  while (i>=0)
                  {
                    if (n->S[i]==n->T[n->tl].p)
                    {
                      n->T[n->tl].p=NULL; n->S[i]=NULL;
                      break;
                    }
                    i--;
                  }
                  pDelete(&n->T[n->tl].p);
                  n->tl--;
                }
                memset(n->Shdl->m,0,IDELEMS(n->Shdl)*sizeof(poly));
                n->sl=-1;
                break;
              }
              else
              {
                pDelete(&r);
              }
            }
            j--;
          }
        }

        /* check for empty sets */
        {
          ideal_list Lj=FL;
          while (Lj!=NULL)
          {
            if ((n->sl>=0)&&(n->S[0]!=NULL))
            {
              ideal r=kNF(n->Shdl,NULL,Lj->d,0,KSTD_NF_LAZY | KSTD_NF_NONORM);
              if (idIs0(r))
              {
                if (TEST_OPT_DEBUG)
                {
                  #ifdef KDEBUG
                  Print("empty set s(%d) because:L[%d]\n",n->nr,Lj->nr);
                  #else
                  Print("empty set s(%d) because:\n",n->nr);
                  #endif
                  iiWriteMatrix((matrix)Lj->d,"L",1,currRing,0);
                }
                while (n->Ll >= 0) deleteInL(n->L,&n->Ll,n->Ll,n);
                while (n->tl >= 0)
                {
                  int i=n->sl;
                  while (i>=0)
                  {
                    if (n->S[i]==n->T[n->tl].p)
                    {
                      n->T[n->tl].p=NULL; n->S[i]=NULL;
                      break;
                    }
                    i--;
                  }
                  pDelete(&n->T[n->tl].p);
                  n->tl--;
                }
                memset(n->Shdl->m,0,IDELEMS(n->Shdl)*sizeof(poly));
                n->sl=-1;
                idDelete(&r);
                break;
              }
              idDelete(&r);
            }
            Lj=Lj->next;
          }
        }
      } /* for */
      for(i=0;i<IDELEMS(fac);i++) fac->m[i]=NULL;
      idDelete(&fac);
      idDelete(&fac_copy);
    }
#ifdef KDEBUG
    strat->P.lcm=NULL;
#endif
    kTest_TS(strat);
    if ((strat->Ll==-1) && (strat->sl>=0))
    {
      if (TEST_OPT_REDSB) completeReduceFac(strat,FL);
    }
    kTest_TS(strat);
  }
#ifdef KDEBUG
  if (TEST_OPT_DEBUG) messageSets(strat);
#endif
  /* complete reduction of the standard basis--------- */
  /* release temp data-------------------------------- */
  if (TEST_OPT_WEIGHTM)
  {
    pRestoreDegProcs(currRing,strat->pOrigFDeg, strat->pOrigLDeg);
    if (ecartWeights)
    {
      omFreeSize((ADDRESS)ecartWeights,((currRing->N)+1)*sizeof(short));
      ecartWeights=NULL;
    }
  }
  exitBuchMora(strat);
  if (TEST_OPT_PROT) { PrintLn(); messageStat(0,strat); }
  if (Q!=NULL) updateResult(strat->Shdl,Q,strat);
  return (strat->Shdl);
}

ideal_list kStdfac(ideal F, ideal Q, tHomog h,intvec ** w,ideal D)
{
  ideal r;
  BOOLEAN b=currRing->pLexOrder,toReset=FALSE;
  BOOLEAN delete_w=(w==NULL);
  kStrategy strat=new skStrategy;
  kStrategy orgstrat=strat;
  ideal_list L=NULL;

  if (rField_has_simple_inverse(currRing))
    strat->LazyPass=20;
  else
    strat->LazyPass=2;
  strat->LazyDegree = 1;
  strat->ak = id_RankFreeModule(F,currRing);
  if (h==testHomog)
  {
    if (strat->ak==0)
    {
      h = (tHomog)idHomIdeal(F,Q);
      w=NULL;
    }
    else
      h = (tHomog)idHomModule(F,Q,w);
  }
  if (h==isHomog)
  {
    if ((w!=NULL) && (*w!=NULL))
    {
      kModW = *w;
      strat->kModW = *w;
      strat->pOrigFDeg = currRing->pFDeg;
      strat->pOrigLDeg = currRing->pLDeg;
      pSetDegProcs(currRing,kModDeg);
      toReset = TRUE;
    }
    currRing->pLexOrder = TRUE;
    strat->LazyPass*=2;
  }
  strat->homog=h;
  initBuchMoraCrit(strat); /*set Gebauer, honey, sugarCrit*/
  initBuchMoraPos(strat);
  initBba(strat);
  initBuchMora(F, Q,strat);
  if (D!=NULL)
  {
    strat->D=idCopy(D);
  }
// Ende der Initalisierung
  while (strat!=NULL)
  {
    if (TEST_OPT_DEBUG)
      PrintS("====================================\n");
    if (w!=NULL)
      r=bbafac(F,Q,*w,strat,L);
    else
      r=bbafac(F,Q,NULL,strat,L);
#ifdef KDEBUG
    int i;
    for (i=0; i<IDELEMS(r); i++) pTest(r->m[i]);
#endif
    idSkipZeroes(r);
    // Testausgabe:
    //if (!idIs0(r))
    //{
    //  PrintS("===================================================\n");
    //  iiWriteMatrix((matrix)r,"S",1,currRing,0);
    //  PrintS("\n===================================================\n");
    //}
    //else
    //{
    //  PrintS("=========empty============================\n");
    //}
    if(!idIs0(r))
    {
      ideal_list LL=(ideal_list)omAlloc(sizeof(*LL));
      LL->d=r;
#ifndef SING_NDEBUG
      LL->nr=strat->nr;
#endif
      LL->next=L;
      L=LL;
    }
    strat=strat->next;
  }
  /* check for empty sets */
  if (L!=NULL)
  {
    ideal_list Lj=L->next;
    ideal_list Lj_prev=L;
    while (Lj!=NULL)
    {
      ideal_list Li=L;
      while(Li!=Lj)
      {
        ideal r=kNF(Lj->d,NULL,Li->d,0,KSTD_NF_LAZY | KSTD_NF_NONORM);
        if (idIs0(r))
        {
#ifdef KDEBUG
          if (TEST_OPT_DEBUG)
          {
            Print("empty set L[%p] because:L[%p]\n",(void*)Lj,(void*)Li);
          }
#endif
          // delete L[j],
          Li=L;
          if (Lj_prev!=NULL)
          {
            Lj=Lj_prev;
            if (Lj==L) Lj_prev=NULL;
            else
            {
              Lj_prev=L;
              while(Lj_prev->next!=Lj) Lj_prev=Lj_prev->next;
            }
          }
          else Lj=NULL;
        }
        else
        {
          Li=Li->next;
        }
        idDelete (&r);
      }
      if (Lj!=NULL) Lj=Lj->next;
    }
  }
// Ende: aufraeumen
  if (toReset)
  {
    pRestoreDegProcs(currRing,strat->pOrigFDeg, strat->pOrigLDeg);
    kModW = NULL;
  }
  currRing->pLexOrder = b;
  delete(strat);
  strat=orgstrat;
  while (strat!=NULL)
  {
    orgstrat=strat->next;
    delete(strat);
    strat=orgstrat;
  }
  if ((delete_w)&&(w!=NULL)&&(*w!=NULL)) delete *w;
  return L;
}

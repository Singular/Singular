/****************************************
*  Computer Algebra System SINGULAR     *
****************************************/
/* $Id: kstdfac.cc,v 1.43 2000-10-19 15:00:15 obachman Exp $ */
/*
*  ABSTRACT -  Kernel: factorizing alg. of Buchberger
*/

#include "mod2.h"
#include "tok.h"
#include "omalloc.h"
#include "polys.h"
#include "ideals.h"
#include "febase.h"
#include "kutil.h"
#include "kstd1.h"
#include "khstd.h"
#include "cntrlc.h"
#include "weight.h"
#include "ipshell.h"
#include "intvec.h"
#ifdef HAVE_FACTORY
#include "clapsing.h"
#endif
#include "lists.h"
#include "ideals.h"
#include "timer.h"
#include "kstdfac.h"

#ifdef HAVE_FACTORY
/*3
* copy o->T to n->T, assumes that n->S is already copied
*/
static void copyT (kStrategy o,kStrategy n)
{
  int i,j;
  poly  p;
  TSet t=(TSet)omAlloc0(o->tmax*sizeof(TObject));

  for (j=0; j<=o->tl; j++)
  {
    t[j] = o->T[j];
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
    t[j].t_p = NULL;
  }
  n->T=t;
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
      l[j].p=pHead(o->L[j].p);
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
    
    // copy .p1 ----------------------------------------------
    p = o->L[j].p1;
    i = -1;
    loop
    {
      if(p==NULL) break;
      i++;
      if(i>o->tl)
      {
        Warn("poly p1 not found in T:");wrp(p);PrintLn();
        l[j].p1=pCopy(p);
        break;
      }
      if (p == o->T[i].p)
      {
        l[j].p1=n->T[i].p;
        break;
      }
    }

    // copy .p2 ----------------------------------------------
    p = o->L[j].p2;
    i = -1;
    loop
    {
      if(p==NULL) break;
      i++;
      if(i>o->tl)
      {
        Warn("poly p2 not found in T:");wrp(p);PrintLn();
        l[j].p2=pCopy(p);
        break;
      }
      if (p == o->T[i].p)
      {
        l[j].p2=n->T[i].p;
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
  }
  n->L=l;
}

kStrategy kStratCopy(kStrategy o)
{
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
  s->pOldFDeg=o->pOldFDeg;
  s->Shdl=idCopy(o->Shdl);
  s->S=s->Shdl->m;
  s->tailRing = o->tailRing;
  if (o->D!=NULL) s->D=idCopy(o->D);
  else            s->D=NULL;
  s->ecartS=(int *)omAlloc(IDELEMS(o->Shdl)*sizeof(int));
  memcpy(s->ecartS,o->ecartS,IDELEMS(o->Shdl)*sizeof(int));
  s->sevS=(unsigned long *)omAlloc(IDELEMS(o->Shdl)*sizeof(unsigned long));
  memcpy(s->sevS,o->sevS,IDELEMS(o->Shdl)*sizeof(unsigned long));
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
  s->kIdeal=NULL;
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
  s->Bmax=setmax;
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
  kTest_TS(s);
  return s;
}

static void completeReduceFac (kStrategy strat, lists FL)
{
  int si;

  strat->noTailReduction = FALSE;
  if (TEST_OPT_PROT)
  {
    PrintLn();
    if (timerv) writeTime("standard base computed:");
  }
  if (TEST_OPT_PROT)
  {
    Print("(S:%d)",strat->sl);mflush();
  }
  for (si=strat->sl; si>0; si--)
  {
    //if (strat->interpt) test_int_std(strat->kIdeal);
    strat->S[si] = redtailBba(strat->S[si],si-1,strat);
    if (TEST_OPT_INTSTRATEGY)
    {
      pCleardenom(strat->S[si]);
    }
    if (TEST_OPT_PROT)
    {
      PrintS("-");mflush();
    }
    int facdeg=pFDeg(strat->S[si]);

    ideal fac=singclap_factorize(strat->S[si],NULL,1);
#ifndef HAVE_LIBFAC_P
    if (fac==NULL)
    {
      fac=idInit(1,1);
      fac->m[0]=pCopy(strat->S[si]);
    }
#endif

    if ((IDELEMS(fac)==1)&&(facdeg==pFDeg(fac->m[0])))
    {
      idDelete(&fac);
      continue;
    }
    if (TEST_OPT_DEBUG)
    {
      wrp(strat->S[si]);
      Print(" (=S[%d]) -> %d factors\n",si,IDELEMS(fac));
    }
    else if (TEST_OPT_PROT)
    {
      int ii=IDELEMS(fac);
      if (ii>1)
      {
        while(ii>0) { PrintS("F"); ii--; }
      }
    }
    ideal fac_copy=idInit(IDELEMS(fac),1);
    deleteInS(si,strat);
    int i;
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
        memset(&n->P,0,sizeof(n->P));

      n->P.p=fac->m[i];
      n->initEcart(&n->P);

      /* enter P.p into s and L */
      int pos;
      if (n->sl==-1) pos=0;
      else pos=posInS(n->S,n->sl,n->P.p);
      if (TEST_OPT_INTSTRATEGY)
      {
        n->P.p = redtailBba(n->P.p,pos-1,n);
        pCleardenom(n->P.p);
      }
      else
      {
        pNorm(n->P.p);
        n->P.p = redtailBba(n->P.p,pos-1,n);
      }
      if (TEST_OPT_DEBUG)
      {
        PrintS("new s:");
        wrp(n->P.p);
        PrintLn();
      }
      enterpairs(n->P.p,n->sl,n->P.ecart,pos,n);
      n->enterS(n->P,pos,n);
      enterTBba(n->P,n->posInT(n->T,n->tl,n->P),n);

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
          iiWriteMatrix((matrix)n->D,"D",1,0);
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
            poly r=kNF(n->Shdl,NULL,n->D->m[j],0,TRUE);
            if (r==NULL)
            {
              if (TEST_OPT_DEBUG)
              {
                PrintS("empty set because:");
                wrp(n->D->m[j]);
                PrintLn();
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
        int j=FL->nr;
        while (j>=0)
        {
          if ((n->sl>=0)&&(n->S[0]!=NULL))
          {
            ideal r=kNF(n->Shdl,NULL,(ideal)FL->m[j].Data(),0,TRUE);
            if (idIs0(r))
            {
              if (TEST_OPT_DEBUG)
              {
                Print("empty set because:L[%d]\n",j);
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
          j--;
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

ideal bbafac (ideal F, ideal Q,intvec *w,kStrategy strat, lists FL)
{
  int   srmax,lrmax;
  int   olddeg,reduc;

  srmax = strat->sl;
  reduc = olddeg = lrmax = 0;
  /* compute------------------------------------------------------- */
  if ((strat->Ll==-1) && (strat->sl>=0))
  {
    if (TEST_OPT_REDSB) completeReduceFac(strat,FL);
  }
  kTest_TS(strat);
  while (strat->Ll >= 0)
  {
    if (strat->Ll > lrmax) lrmax =strat->Ll;/*stat.*/
    if (TEST_OPT_DEBUG) messageSets(strat);
    //test_int_std(strat->kIdeal);
    if (strat->Ll== 0) strat->interpt=TRUE;
    if (TEST_OPT_DEGBOUND
    && ((strat->honey
        && (strat->L[strat->Ll].ecart+pFDeg(strat->L[strat->Ll].p)>Kstd1_deg))
      || ((!strat->honey) && (pFDeg(strat->L[strat->Ll].p)>Kstd1_deg))))
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
        message(strat->P.ecart+pFDeg(strat->P.p),&olddeg,&reduc,strat);
    }
    else
    {
      if (TEST_OPT_PROT)
        message(pFDeg(strat->P.p),&olddeg,&reduc,strat);
    }
    /* reduction of the element choosen from L */
    kTest_TS(strat);
    strat->red(&strat->P,strat);
    kTest_TS(strat);
    if (strat->P.p != NULL)
    {
      int facdeg=pFDeg(strat->P.p);
      /* statistic */
      if (TEST_OPT_PROT) PrintS("s");

      ideal fac=singclap_factorize(strat->P.p,NULL,1);
#ifndef HAVE_LIBFAC_P
      if (fac==NULL)
      {
        fac=idInit(1,1);
        fac->m[0]=pCopy(strat->P.p);
      }
#endif
      ideal fac_copy=idInit(IDELEMS(fac),1);

      if (TEST_OPT_DEBUG)
      {
        Print("-> %d factors\n",IDELEMS(fac));
      }
      else if (TEST_OPT_PROT)
      {
        int ii=IDELEMS(fac);
        if (ii>1)
        {
          while(ii>0) { PrintS("F"); ii--; }
        }
      }
      if ((IDELEMS(fac)==1)&&(facdeg==pFDeg(fac->m[0])))
      {
        if (TEST_OPT_INTSTRATEGY)
        {
          strat->P.p = redtailBba(strat->P.p,strat->sl,strat);
          if (strat->redTailChange) pCleardenom(strat->P.p);
        }
        else
        {
          pNorm(strat->P.p);
          strat->P.p = redtailBba(strat->P.p,strat->sl,strat);
        }
        if (strat->redTailChange)
        {
          idDelete(&fac);
          fac=singclap_factorize(strat->P.p,NULL,1);
#ifndef HAVE_LIBFAC_P
          if (fac==NULL)
          {
            fac=idInit(1,1);
            fac->m[0]=pCopy(strat->P.p);
          }
#endif
          idDelete(&fac_copy);
          fac_copy=idInit(IDELEMS(fac),1);
        }
        if ((IDELEMS(fac)==1)&&(facdeg==pFDeg(fac->m[0])))
        {
          pDelete(&(fac->m[0]));
          fac->m[0]=strat->P.p;
        }
      }
      if (strat->P.lcm!=NULL) pLmFree(strat->P.lcm);
      int i;

      for(i=IDELEMS(fac)-1;i>=0;i--)
      {
        kStrategy n=strat;
        if (i>=1)
        {
          n=kStratCopy(strat); // includes memset(&n->P,0,sizeof(n->P));
          n->next=strat->next;
          strat->next=n;
        }
        else
        {
          n->P.Init(strat->tailRing);
        }

        n->P.p=fac->m[i];
        n->initEcart(&n->P);

        /* enter P.p into s and L */
        int pos;
        if (n->sl==-1) pos=0;
        else pos=posInS(n->S,n->sl,n->P.p);

        // we have already reduced all elements from fac....
        if (TEST_OPT_INTSTRATEGY)
        {
          n->P.p = redtailBba(n->P.p,pos-1,n);
          if (n->redTailChange) pCleardenom(n->P.p);
        }
        else
        {
          pNorm(n->P.p);
          n->P.p = redtailBba(n->P.p,pos-1,n);
        }

        if (TEST_OPT_DEBUG)
        {
          PrintS("new s:");
          wrp(n->P.p);
          PrintLn();
        }
        enterpairs(n->P.p,n->sl,n->P.ecart,pos,n);
        n->enterS(n->P,pos,n);
        if (n->sl>srmax) srmax = n->sl;
        enterTBba(n->P,n->posInT(n->T,n->tl,n->P),n);

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
            iiWriteMatrix((matrix)n->D,"D",1,0);
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
              poly r=kNF(n->Shdl,NULL,n->D->m[j],0,TRUE);
              if (r==NULL)
              {
                if (TEST_OPT_DEBUG)
                {
                  PrintS("empty set because:");
                  wrp(n->D->m[j]);
                  PrintLn();
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
          int j=FL->nr;
          while (j>=0)
          {
            if ((n->sl>=0)&&(n->S[0]!=NULL))
            {
              ideal r=kNF(n->Shdl,NULL,(ideal)FL->m[j].Data(),0,TRUE);
              if (idIs0(r))
              {
                if (TEST_OPT_DEBUG)
                {
                  Print("empty set because:L[%d]\n",j);
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
            j--;
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
  if (TEST_OPT_DEBUG) messageSets(strat);
  /* complete reduction of the standard basis--------- */
  /* release temp data-------------------------------- */
  exitBuchMora(strat);
  if (TEST_OPT_WEIGHTM)
  {
    pFDeg=pFDegOld;
    pLDeg=pLDegOld;
    if (ecartWeights)
    {
      omFreeSize((ADDRESS)ecartWeights,(pVariables+1)*sizeof(short));
      ecartWeights=NULL;
    }
  }
  if (TEST_OPT_PROT) messageStat(srmax,lrmax,0,strat);
  if (Q!=NULL) updateResult(strat->Shdl,Q,strat);
  return (strat->Shdl);
}
#endif

lists kStdfac(ideal F, ideal Q, tHomog h,intvec ** w,ideal D)
{
#ifdef HAVE_FACTORY
  ideal r;
  BOOLEAN b=pLexOrder,toReset=FALSE;
  BOOLEAN delete_w=(w==NULL);
  kStrategy strat=new skStrategy;
  kStrategy orgstrat=strat;
  lists L=(lists)omAllocBin(slists_bin); L->Init(0);
  sleftv v; memset(&v,0,sizeof(v));

  if (rField_has_simple_inverse())
    strat->LazyPass=20;
  else
    strat->LazyPass=2;
  strat->LazyDegree = 1;
  strat->ak = idRankFreeModule(F);
  if ((h==testHomog))
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
      pOldFDeg = pFDeg;
      pFDeg = kModDeg;
      toReset = TRUE;
    }
    pLexOrder = TRUE;
    strat->LazyPass*=2;
  }
  strat->homog=h;
  initBuchMoraCrit(strat); /*set Gebauer, honey, sugarCrit*/
  initBuchMoraPos(strat);
  initBba(F,strat);
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
    //  iiWriteMatrix((matrix)r,"S",1,0);
    //  PrintS("\n===================================================\n");
    //}
    //else
    //{
    //  PrintS("=========empty============================\n");
    //}
    strat=strat->next;
    if(!idIs0(r))
    {
      v.rtyp=IDEAL_CMD;
      v.data=(void *)r;
      lists LL=lInsert0(L,&v,0);
      L=LL;
    }
  }
  /* check for empty sets */
  {
    int j=L->nr;
    while (j>0)
    {
      int i=0;
      while(i<j)
      {
        ideal r=kNF((ideal)L->m[j].Data(),NULL,(ideal)L->m[i].Data(),0,TRUE);
        if (idIs0(r))
        {
          if (TEST_OPT_DEBUG)
          {
            Print("empty set L[%d] because:L[%d]\n",j,i);
          }
          // delete L[j],
          i=0; j--;
        }
        else
        {
          i++;
        }
        idDelete(&r);
      }
      j--;
    }
  }
// Ende: aufraeumen
  if (toReset)
  {
    kModW = NULL;
    pFDeg = pOldFDeg;
  }
  pLexOrder = b;
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
#else
  return NULL;
#endif
}

/****************************************
*  Computer Algebra System SINGULAR     *
****************************************/
/*
* ABSTRACT: resolutions
*/

#include "kernel/mod2.h"

#include "misc/mylimits.h"
#include "misc/options.h"
#include "misc/intvec.h"

#include "coeffs/coeffs.h"
#include "coeffs/numbers.h"

#include "polys/monomials/ring.h"
#include "polys/kbuckets.h"
#include "polys/prCopy.h"

#include "kernel/combinatorics/stairc.h"
#include "kernel/combinatorics/hilb.h"

#include "kernel/GBEngine/kstd1.h"
#include "kernel/GBEngine/kutil.h"
#include "kernel/GBEngine/syz.h"

#include "kernel/ideals.h"
#include "kernel/polys.h"


//#define SHOW_PROT
//#define SHOW_RED
//#define SHOW_HILB
//#define SHOW_RESULT
//#define INVERT_PAIRS
//#define SHOW_CRIT
//#define SHOW_SPRFL
#define USE_CHAINCRIT
#define poly_write(p) wrp(p);PrintLn()
/*--- some heuristics to optimize the pair sets---*/
/*--- chose only one (!!!) at the same time ------*/
//#define USE_HEURISTIC1
#define USE_HEURISTIC2

#ifdef SHOW_CRIT
THREAD_VAR static int crit;
THREAD_VAR static int crit1;
THREAD_VAR static int spfl;
THREAD_VAR static int cons_pairs;
THREAD_VAR static int crit_fails;
#endif
typedef struct sopen_pairs open_pairs;
typedef open_pairs* crit_pairs;
struct sopen_pairs
{
  crit_pairs next;
  int first_poly;
  int second_poly;
};
/*3
* computes pairs from the new elements (beginning with the element newEl)
* in the module index
*/
static void syCreateNewPairs_Hilb(syStrategy syzstr, int index,
            int actdeg)
{
  SObject tso;
  poly toHandle,p,pp;
  int r1,r2=0,rr,l=(*syzstr->Tl)[index];
  int i,j,r=0,ti;
  BOOLEAN toComp=FALSE;
#ifdef USE_CHAINCRIT
  crit_pairs cp=NULL,tcp;
#endif
  actdeg += index;

  while ((l>0) && ((syzstr->resPairs[index])[l-1].lcm==NULL)) l--;
  rr = l-1;
  while ((rr>=0) && (((syzstr->resPairs[index])[rr].p==NULL) ||
        ((syzstr->resPairs[index])[rr].order>actdeg))) rr--;
  r2 = rr+1;
  while ((rr>=0) && ((syzstr->resPairs[index])[rr].order==actdeg)
         && ((syzstr->resPairs[index])[rr].syzind<0))
  {
    rr--;
    r++;
  }
  if (r==0) return;
  ideal nP=idInit(l,syzstr->res[index]->rank);
#ifdef INVERT_PAIRS
  r1 = rr+1;
#else
  r1 = r2-1;
#endif
/*---------- there are new pairs ------------------------------*/
  loop
  {
/*--- chose first new elements --------------------------------*/
    toComp = FALSE;
    toHandle = (syzstr->resPairs[index])[r1].p;
    if (toHandle!=NULL)
    {
      int tc=pGetComp(toHandle);
      (syzstr->resPairs[index])[r1].syzind = 0;
      for (i=0; i<r1;i++)
      {
        if (((syzstr->resPairs[index])[i].p!=NULL) &&
            (pGetComp((syzstr->resPairs[index])[i].p)==tc))
        {
#ifdef USE_CHAINCRIT
          tcp = cp;
          if (tcp!=NULL)
          {
            while ((tcp!=NULL) &&
              ((tcp->first_poly!=i)||(tcp->second_poly!=r1))) tcp = tcp->next;
          }
          if (tcp==NULL)
          {
#endif
            p = pOne();
            pLcm((syzstr->resPairs[index])[i].p,toHandle,p);
            pSetm(p);
            j = 0;
            while (j<i)
            {
              if (nP->m[j]!=NULL)
              {
                if (pLmDivisibleByNoComp(nP->m[j],p))
                {
                  pDelete(&p);
                  /* p = NULL;*/
                  break;
                }
                else if (pLmDivisibleByNoComp(p,nP->m[j]))
                {
                  pDelete(&(nP->m[j]));
                  /* nP->m[j] = NULL;*/
                }
#ifdef USE_CHAINCRIT
                else
                {
                  poly p1,p2;
                  int ip= currRing->N;
                  p1 = pMDivide(p,(syzstr->resPairs[index])[i].p);
                  p2 = pMDivide(nP->m[j],(syzstr->resPairs[index])[j].p);
                  while ((ip>0) && (pGetExp(p1,ip)*pGetExp(p2,ip)==0)) ip--;
                  if (ip==0)
                  {
#ifdef SHOW_SPRFL
Print("Hier: %d, %d\n",j,i);
#endif
                    if (i>rr)
                    {
                      tcp=(crit_pairs)omAlloc0(sizeof(sopen_pairs));
                      tcp->next = cp;
                      tcp->first_poly = j;
                      tcp->second_poly = i;
                      cp = tcp;
                      tcp = NULL;
                    }
                    else
                    {
                      ti=0;
                      while ((ti<l) && (((syzstr->resPairs[index])[ti].ind1!=j)||
                             ((syzstr->resPairs[index])[ti].ind2!=i))) ti++;
                      if (ti<l)
                      {
#ifdef SHOW_SPRFL
Print("gefunden in Mod %d: ",index); poly_write((syzstr->resPairs[index])[ti].lcm);
#endif
                        syDeletePair(&(syzstr->resPairs[index])[ti]);
#ifdef SHOW_CRIT
                        crit1++;
#endif
                        toComp = TRUE;
                      }
                    }
                  }
                  pLmFree(&p1);
                  pLmFree(&p2);
                }
#endif
              }
              j++;
            }
            if (p!=NULL)
            {
              nP->m[i] = p;
            }
#ifdef USE_CHAINCRIT
          }
          else
          {
#ifdef SHOW_CRIT
            crit1++;
#endif
          }
#endif
        }
      }
      if (toComp) syCompactify1(syzstr->resPairs[index],&l,r1);
      for (i=0;i<r1;i++)
      {
        if (nP->m[i]!=NULL)
        {
          tso.lcm = p = nP->m[i];
          nP->m[i] = NULL;
          tso.order = pTotaldegree(p);
          if ((syzstr->cw!=NULL) && (index>0) && (pGetComp(p)>0))
          {
            int ii=index-1,jj=pGetComp(p);
            while (ii>0)
            {
              jj = pGetComp(syzstr->res[ii]->m[jj-1]);
              ii--;
            }
            tso.order += (*syzstr->cw)[jj-1];
          }
          tso.p1 = (syzstr->resPairs[index])[i].p;
          tso.p2 = toHandle;
          tso.ind1 = i;
          tso.ind2 = r1;
          tso.syzind = -1;
          tso.isNotMinimal = (poly)1;
          tso.p = NULL;
          tso.length = -1;
          number coefgcd =
            n_SubringGcd(pGetCoeff(tso.p1),pGetCoeff(tso.p2),currRing->cf);
          tso.syz = pCopy((syzstr->resPairs[index])[i].syz);
          poly tt = pMDivide(tso.lcm,tso.p1);
          pSetCoeff0(tt,nDiv(pGetCoeff(tso.p1),coefgcd));
          tso.syz = pMult_mm(tso.syz,tt);
          pLmDelete(&tt);
          coefgcd = nInpNeg(coefgcd);
          pp = pCopy((syzstr->resPairs[index])[r1].syz);
          tt = pMDivide(tso.lcm,tso.p2);
          pSetCoeff0(tt,nDiv(pGetCoeff(tso.p2),coefgcd));
          pp = pMult_mm(pp,tt);
          pLmDelete(&tt);
          tso.syz = pAdd(pp,tso.syz);
          nDelete(&coefgcd);
          pSetComp(tso.lcm,pGetComp((syzstr->resPairs[index])[r1].syz));
#ifdef SHOW_PROT
Print("erzeuge Paar im Modul %d,%d mit: \n",index,tso.order);
PrintS("poly1: ");poly_write(tso.p1);
PrintS("poly2: ");poly_write(tso.p2);
PrintS("syz: ");poly_write(tso.syz);
PrintS("sPoly: ");poly_write(tso.p);
PrintLn();
#endif
          syEnterPair(syzstr,&tso,&l,index);
        }
      }
    }
#ifdef INVERT_PAIRS
    r1++;
    if (r1>=r2) break;
#else
    r1--;
    if (r1<=rr) break;
#endif
  }
  idDelete(&nP);
#ifdef USE_CHAINCRIT
  while (cp!=NULL)
  {
    tcp = cp;
    cp = cp->next;
    omFreeSize((ADDRESS)tcp,sizeof(sopen_pairs));
  }
#endif
}

/*3
* determines the place of a polynomial in the right ordered resolution
* set the vectors of truecomponents
*/
static void syOrder_Hilb(poly p,syStrategy syzstr,int index)
{
  int i=IDELEMS(syzstr->orderedRes[index]);

  while ((i>0) && (syzstr->orderedRes[index]->m[i-1]==NULL)) i--;
  syzstr->orderedRes[index]->m[i] = p;
}

static void syHalfPair(poly syz, int newEl, syStrategy syzstr, int index)
{
  SObject tso;
  memset(&tso,0,sizeof(tso));
  int l=(*syzstr->Tl)[index];

  while ((l>0) && ((syzstr->resPairs[index])[l-1].syz==NULL)) l--;
  if ((syzstr->cw!=NULL) && (index>0) && (pGetComp(syz)>0))
  {
    int ii=index-1,jj=pGetComp(syz);
    while (ii>0)
    {
      jj = pGetComp(syzstr->res[ii]->m[jj-1]);
      ii--;
    }
    tso.order += (*syzstr->cw)[jj-1];
  }
  tso.p1 = NULL;
  tso.p2 = NULL;
  tso.ind1 = 0;
  tso.ind2 = 0;
  tso.syzind = -1;
  tso.isNotMinimal = NULL;
  tso.p = syz;
  tso.order = pTotaldegree(syz);
  tso.syz = pHead(syz);
  pSetComp(tso.syz,newEl+1);
  pSetm(tso.syz);
  tso.lcm = pHead(tso.syz);
  tso.length = pLength(syz);
  syOrder_Hilb(syz,syzstr,index);
#ifdef SHOW_PROT
Print("erzeuge Halbpaar im Module %d,%d mit: \n",index,tso.order);
PrintS("syz: ");poly_write(tso.syz);
PrintS("sPoly: ");poly_write(tso.p);
PrintLn();
#endif
  syEnterPair(syzstr,&tso,&l,index);
}
/*3
* computes the order of pairs of same degree
* must be monoton
*/
static intvec* syLinStrat2(SSet nextPairs, syStrategy syzstr,
                          int howmuch, int index,intvec ** secondpairs)
{
  ideal o_r=syzstr->res[index+1];
  int i=0,i1=0,i2=0,l,ll=IDELEMS(o_r);
  intvec *result=new intvec(howmuch+1);
  BOOLEAN isDivisible;
  SObject tso;

#ifndef USE_HEURISTIC2
  while (i1<howmuch)
  {
    (*result)[i1] = i1+1;
    i1++;
  }
  return result;
#else
  while ((ll>0) && (o_r->m[ll-1]==NULL)) ll--;
  while (i<howmuch)
  {
    tso = nextPairs[i];
    isDivisible = FALSE;
    l = 0;
    while ((l<ll) && (!isDivisible))
    {
      if (o_r->m[l]!=NULL)
      {
        isDivisible = isDivisible ||
          pLmDivisibleBy(o_r->m[l],tso.lcm);
      }
      l++;
    }
    if (isDivisible)
    {
#ifdef SHOW_PROT
Print("streiche Paar im Modul %d,%d mit: \n",index,nextPairs[i].order);
PrintS("poly1: ");poly_write(nextPairs[i].p1);
PrintS("poly2: ");poly_write(nextPairs[i].p2);
PrintS("syz: ");poly_write(nextPairs[i].syz);
PrintS("sPoly: ");poly_write(nextPairs[i].p);
PrintLn();
#endif
      //syDeletePair(&nextPairs[i]);
      if (*secondpairs==NULL) *secondpairs = new intvec(howmuch);
      (**secondpairs)[i2] = i+1;
      i2++;
#ifdef SHOW_CRIT
      crit++;
#endif
    }
    else
    {
//      nextPairs[i].p = sySPoly(tso.p1, tso.p2,tso.lcm);
      (*result)[i1] = i+1;
      i1++;
    }
    i++;
  }
  return result;
#endif
}

inline void sySPRedSyz(syStrategy syzstr,sSObject redWith,poly q=NULL)
{
  poly p=pMDivide(q,redWith.p);
  pSetCoeff0(p,nDiv(pGetCoeff(q),pGetCoeff(redWith.p)));
  int il=-1;
  kBucket_Minus_m_Mult_p(syzstr->syz_bucket,p,redWith.syz,&il,NULL);
  pLmDelete(&p);
}

static poly syRed_Hilb(poly toRed,syStrategy syzstr,int index)
{
  ideal redWith=syzstr->res[index];
  if (redWith==NULL) return toRed;
  int j=IDELEMS(redWith),i;
  poly q,result=NULL,resultp;

  while ((j>0) && (redWith->m[j-1]==NULL)) j--;
  if ((toRed==NULL) || (j==0)) return toRed;
  kBucketInit(syzstr->bucket,toRed,-1);
  q = kBucketGetLm(syzstr->bucket);
  loop
  {
    if (q==NULL)
    {
      break;
    }
    i = 0;
    loop
    {
      if (pLmDivisibleBy(redWith->m[i],q))
      {
        number up = kBucketPolyRed(syzstr->bucket,redWith->m[i],
                         pLength(redWith->m[i]), NULL);
        nDelete(&up);
        q = kBucketGetLm(syzstr->bucket);
        if (toRed==NULL) break;
        i = 0;
      }
      else
      {
        i++;
      }
      if ((i>=j) || (q==NULL)) break;
    }
    if (q!=NULL)
    {
      if (result==NULL)
      {
        resultp = result = kBucketExtractLm(syzstr->bucket);
      }
      else
      {
        pNext(resultp) = kBucketExtractLm(syzstr->bucket);
        pIter(resultp);
      }
      q = kBucketGetLm(syzstr->bucket);
    }
  }
  kBucketClear(syzstr->bucket,&q,&i);
  if (q!=NULL) PrintS("Hier ist was schief gelaufen!\n");
  return result;
}

#ifdef USE_HEURISTIC1
intvec *ivStrip(intvec* arg)
{
  int l=arg->rows()*arg->cols(),i=0,ii=0;
  intvec *tempV=new intvec(l);

  while (i+ii<l)
  {
    if ((*arg)[i+ii]==0)
    {
      ii++;
    }
    else
    {
      (*tempV)[i] = (*arg)[i+ii];
      i++;
    }
  }
  if (i==0)
  {
    delete tempV;
    return NULL;
  }
  intvec * result=new intvec(i+1);
  for (ii=0;ii<i;ii++)
   (*result)[ii] = (*tempV)[ii];
  delete tempV;
  return result;
}
#endif

/*3
* reduces all pairs of degree deg in the module index
* put the reduced generators to the resolvente which contains
* the truncated kStd
*/
static void syRedNextPairs_Hilb(SSet nextPairs, syStrategy syzstr,
               int howmuch, int index,int actord,int* toSub,
               int *maxindex,int *maxdeg)
{
  int i,j,k=IDELEMS(syzstr->res[index]);
  int ks=IDELEMS(syzstr->res[index+1]),kk;
  int ks1=IDELEMS(syzstr->orderedRes[index+1]);
  int kres=(*syzstr->Tl)[index];
  int toGo=0;
  int il;
  SSet redset=syzstr->resPairs[index];
  poly q;
  intvec *spl1;
  SObject tso;
  intvec *spl3=NULL;
#ifdef USE_HEURISTIC1
  intvec *spl2=new intvec(howmuch+1,howmuch+1,0);
  int there_are_superfluous=0;
  int step=1,jj,j1,j2;
#endif
  assume((syzstr->truecomponents[index]) != NULL && (syzstr->ShiftedComponents[index]) != NULL);

  actord += index;
  if ((nextPairs==NULL) || (howmuch==0)) return;
  while ((k>0) && (syzstr->res[index]->m[k-1]==NULL)) k--;
  while ((ks>0) && (syzstr->res[index+1]->m[ks-1]==NULL)) ks--;
  while ((ks1>0) && (syzstr->orderedRes[index+1]->m[ks1-1]==NULL)) ks1--;
  while ((kres>0) &&
        ((redset[kres-1].p==NULL) || (redset[kres-1].order>actord))) kres--;
  while ((kres<(*syzstr->Tl)[index]) &&
         (redset[kres-1].order!=0) && (redset[kres-1].order<=actord)) kres++;
  spl1 = syLinStrat2(nextPairs,syzstr,howmuch,index,&spl3);
#ifdef SHOW_PROT
PrintS("spl1 ist hier: ");spl1->show(0,0);
#endif
  i=0;
  kk = (*spl1)[i]-1;
  if (index==1)
  {
    intvec * temp1_hilb = hHstdSeries(syzstr->res[index],NULL,NULL,NULL);
    if (actord<temp1_hilb->length())
    {
      toGo = (*temp1_hilb)[actord];
#ifdef SHOW_HILB
Print("\nStze toGo im Modul %d und Grad %d auf: %d\n",1,actord-1,toGo);
#endif
    }
    delete temp1_hilb;
  }
  else
  {
    if (actord<=(syzstr->hilb_coeffs[index])->length())
    {
      toGo = (*syzstr->hilb_coeffs[index])[actord-1];
#ifdef SHOW_HILB
Print("\nStze toGo im Modul %d und Grad %d auf: %d\n",index,actord-1,toGo);
#endif
    }
  }
  if ((syzstr->hilb_coeffs[index+1]!=NULL) &&
      (actord<=(syzstr->hilb_coeffs[index+1])->length()))
  {
    toGo += (*syzstr->hilb_coeffs[index+1])[actord-1];
#ifdef SHOW_HILB
Print("\nAddiere zu toGo aus Modul %d und Grad %d: %d\n",index+1,actord-1,(*syzstr->hilb_coeffs[index+1])[actord-1]);
#endif
  }
#ifdef SHOW_HILB
Print("<H%d>",toGo);
#endif
  while (kk>=0)
  {
    if (toGo==0)
    {
      while (kk>=0)
      {
        pDelete(&nextPairs[kk].p);
        pDelete(&nextPairs[kk].syz);
        syDeletePair(&nextPairs[kk]);
        nextPairs[kk].p = nextPairs[kk].syz = nextPairs[kk].lcm = NULL;
        i++;
        kk = (*spl1)[i]-1;
#ifdef USE_HEURISTIC2
        if (kk<0)
        {
          i = 0;
          delete spl1;
          spl1 = spl3;
          spl3 = NULL;
          if (spl1!=NULL)
            kk = (*spl1)[i]-1;
        }
#endif
      }
      if (spl1!=NULL) delete spl1;
      break;
    }
    tso = nextPairs[kk];
    if ((tso.p1!=NULL) && (tso.p2!=NULL))
    {
#ifdef SHOW_CRIT
      cons_pairs++;
#endif
      //tso.p = sySPoly(tso.p1, tso.p2,tso.lcm);
      tso.p = ksOldCreateSpoly(tso.p2, tso.p1);
#ifdef SHOW_PROT
PrintS("reduziere Paar mit: \n");
PrintS("poly1: ");poly_write(tso.p1);
PrintS("poly2: ");poly_write(tso.p2);
PrintS("syz: ");poly_write(tso.syz);
PrintS("sPoly: ");poly_write(tso.p);
#endif
      if (tso.p != NULL)
      {
        kBucketInit(syzstr->bucket,tso.p,-1);
        kBucketInit(syzstr->syz_bucket,tso.syz,-1);
        q = kBucketGetLm(syzstr->bucket);
        j = 0;
        while (j<kres)
        {
          if ((redset[j].p!=NULL) && (pLmDivisibleBy(redset[j].p,q))
              && ((redset[j].ind1!=tso.ind1) || (redset[j].ind2!=tso.ind2)))
          {
#ifdef SHOW_RED
kBucketClear(syzstr->bucket,&tso.p,&tso.length);
kBucketClear(syzstr->syz_bucket,&tso.syz,&il);
PrintS("reduziere: ");poly_write(tso.p);
PrintS("syz: ");poly_write(tso.syz);
PrintS("mit: ");poly_write(redset[j].p);
PrintS("syz: ");poly_write(redset[j].syz);
kBucketInit(syzstr->bucket,tso.p,tso.length);
kBucketInit(syzstr->syz_bucket,tso.syz,il);
#endif
            sySPRedSyz(syzstr,redset[j],q);
            number up = kBucketPolyRed(syzstr->bucket,redset[j].p,
                         redset[j].length, NULL);
            nDelete(&up);
            q = kBucketGetLm(syzstr->bucket);
#ifdef SHOW_RED
kBucketClear(syzstr->bucket,&tso.p,&tso.length);
kBucketClear(syzstr->syz_bucket,&tso.syz,&il);
PrintS("zu: ");poly_write(tso.p);
PrintS("syz: ");poly_write(tso.syz);
kBucketInit(syzstr->bucket,tso.p,tso.length);
kBucketInit(syzstr->syz_bucket,tso.syz,il);
PrintLn();
#endif
            if (q==NULL) break;
            j = 0;
          }
          else
          {
            j++;
          }
        }
        kBucketClear(syzstr->bucket,&tso.p,&tso.length);
        kBucketClear(syzstr->syz_bucket,&tso.syz,&il);
      }
#ifdef SHOW_PROT
PrintS("erhalte Paar mit: \n");
PrintS("syz: ");poly_write(tso.syz);
PrintS("sPoly: ");poly_write(tso.p);
PrintLn();
#endif
#ifdef SHOW_SPRFL
//PrintLn();
wrp(tso.lcm);
Print(" mit index %d, %d ",tso.ind1,tso.ind2);
#endif
      if (tso.p != NULL)
      {
        if (TEST_OPT_PROT) PrintS("g");
        (*toSub)++;
        toGo--;
        if (!nIsOne(pGetCoeff(tso.p)))
        {
          number n=nInvers(pGetCoeff(tso.p));
          pNorm(tso.p);
          tso.syz=__p_Mult_nn(tso.syz,n,currRing);
          nDelete(&n);
        }
        if (k==IDELEMS((syzstr->res)[index]))
          syEnlargeFields(syzstr,index);
        syzstr->res[index]->m[k] = tso.p;
        k++;
      }
      else
      {
        if (ks==IDELEMS(syzstr->res[index+1]))
          syEnlargeFields(syzstr,index+1);
        syzstr->res[index+1]->m[ks] = syRed_Hilb(tso.syz,syzstr,index+1);
        if (syzstr->res[index+1]->m[ks]!=NULL)
        {
          if (TEST_OPT_PROT) PrintS("s");
          toGo--;
          pNorm(syzstr->res[index+1]->m[ks]);
          syHalfPair(syzstr->res[index+1]->m[ks],ks1,syzstr,index+1);
          ks++;
          ks1++;
          if (index+1>*maxindex) *maxindex = index+1;
          if (actord-index>*maxdeg) *maxdeg = actord-index;
        }
        else
        {
          if (TEST_OPT_PROT) PrintS("-");
#ifdef SHOW_CRIT
          spfl++;
#endif
#ifdef USE_HEURISTIC1
          if (there_are_superfluous>=0)
          {
            j = i+1;
            jj = (*spl1)[j]-1;
            j1 = 1;
            while (jj>=0)
            {
              if (tso.ind2==nextPairs[jj].ind2)
              {
                IMATELEM(*spl2,j1,step) = jj+1;
                j1++;
                for (j2=j;j2<spl1->length()-1;j2++)
                {
                  (*spl1)[j2] = (*spl1)[j2+1];
                }
              }
              else
              {
                j++;
              }
              jj = (*spl1)[j]-1;
            }
            step++;
            if (there_are_superfluous==0) there_are_superfluous = 1;
          }
#endif
#ifdef SHOW_SPRFL
Print("ist ueberfluessig in Mod %d",index);
//Print("\n ueberfluessig in Mod %d:",index);
//wrp(tso.lcm);
//PrintLn();
#endif
        }
        tso.syz = NULL;
        syDeletePair(&tso);
        tso.p = tso.syz = tso.lcm = NULL;
      }
      nextPairs[kk] = tso;
    }
#ifdef SHOW_SPRFL
PrintLn();
#endif
    i++;
#ifdef SHOW_PROT
PrintS("spl1 ist hier: ");spl1->show(0,0);
Print("naechstes i ist: %d",i);
#endif
    kk = (*spl1)[i]-1;
#ifdef USE_HEURISTIC1
    if ((kk<0) && (there_are_superfluous>0))
    {
      i = 0;
      delete spl1;
      spl1 = ivStrip(spl2);
      delete spl2;
      if (spl1!=NULL)
      {
        there_are_superfluous = -1;
        kk = (*spl1)[i]-1;
      }
    }
#endif
#ifdef USE_HEURISTIC2
    if ((kk<0) && (toGo>0))
    {
#ifdef SHOW_CRIT
      crit_fails++;
#endif
      i = 0;
      delete spl1;
      spl1 = spl3;
      spl3 = NULL;
      if (spl1!=NULL)
        kk = (*spl1)[i]-1;
    }
#endif
  }
  delete spl1;
  if (spl3!=NULL) delete spl3;
}

void sySetNewHilb(syStrategy syzstr, int toSub,int index,int actord)
{
  int i;
  actord += index;
  intvec * temp_hilb = hHstdSeries(syzstr->res[index+1],NULL,NULL,NULL);
  intvec * cont_hilb = hHstdSeries(syzstr->res[index],NULL,NULL,NULL);
  if ((index+1<syzstr->length) && (syzstr->hilb_coeffs[index+1]==NULL))
  {
    syzstr->hilb_coeffs[index+1] = new intvec(16*((actord/16)+1));
  }
  else if (actord>=syzstr->hilb_coeffs[index+1]->length())
  {
    intvec * ttt=new intvec(16*((actord/16)+1));
    for (i=syzstr->hilb_coeffs[index+1]->length()-1;i>=0;i--)
    {
      (*ttt)[i] = (*(syzstr->hilb_coeffs[index+1]))[i];
    }
    delete syzstr->hilb_coeffs[index+1];
    syzstr->hilb_coeffs[index+1] = ttt;
  }
  if (actord+1<temp_hilb->length())
  {
#ifdef SHOW_HILB
Print("\nSetze fuer Modul %d im Grad %d die Wert: \n",index+1,actord);
(temp_hilb)->show(0,0);
#endif
    int k=si_min(temp_hilb->length()-1,(syzstr->hilb_coeffs[index+1])->length());
    for (int j=k;j>actord;j--)
      (*(syzstr->hilb_coeffs[index+1]))[j-1] = (*temp_hilb)[j];
  }
  else
  {
    (*(syzstr->hilb_coeffs[index+1]))[actord] = 0;
  }
  delete temp_hilb;
  if ((index>1) && (actord<=syzstr->hilb_coeffs[index]->length()))
  {
#ifdef SHOW_HILB
Print("\nSubtrahiere im Modul %d im Grad %d den Wert: %d\n",index,actord-1,toSub);
#endif
    (*syzstr->hilb_coeffs[index])[actord-1]-=toSub;
  }
  if (syzstr->hilb_coeffs[index]!=NULL)
  {
    if (cont_hilb->length()>syzstr->hilb_coeffs[index]->length())
      syzstr->hilb_coeffs[index]->resize(cont_hilb->length());
    for (int j=cont_hilb->length()-1;j>actord;j--)
      (*(syzstr->hilb_coeffs[index]))[j-1] = (*cont_hilb)[j];
  }
  delete cont_hilb;
#ifdef SHOW_HILB
Print("<h,%d>",(*(syzstr->hilb_coeffs[index+1]))[actord]);
#endif
}

/*3
* reduces the generators of the module index in degree deg
* (which are actual syzygies of the module index-1)
* wrt. the ideal generated by elements of lower degrees
*/
static void syRedGenerOfCurrDeg_Hilb(syStrategy syzstr, int deg,int *maxindex,int *maxdeg)
{
  ideal res=syzstr->res[1];
  int i=0,k=IDELEMS(res),k1=IDELEMS(syzstr->orderedRes[1]);
  SSet sPairs=syzstr->resPairs[0];

  while ((k>0) && (res->m[k-1]==NULL)) k--;
  while ((k1>0) && (syzstr->orderedRes[1]->m[k1-1]==NULL)) k1--;
  while ((i<(*syzstr->Tl)[0]) && (((sPairs)[i].syz==NULL) ||
          ((sPairs)[i].order<deg)))
    i++;
  if ((i>=(*syzstr->Tl)[0]) || ((sPairs)[i].order>deg)) return;
  while ((i<(*syzstr->Tl)[0]) && (((sPairs)[i].syz==NULL) ||
         ((sPairs)[i].order==deg)))
  {
    if ((sPairs)[i].syz!=NULL)
    {
#ifdef SHOW_PROT
PrintS("reduziere Erzeuger: \n");
PrintS("syz: ");poly_write((sPairs)[i].syz);
#endif
      (sPairs)[i].syz = syRed_Hilb((sPairs)[i].syz,syzstr,1);
#ifdef SHOW_PROT
PrintS("erhalte Erzeuger: \n");
PrintS("syz: ");poly_write((sPairs)[i].syz);
PrintLn();
#endif
      if ((sPairs)[i].syz != NULL)
      {
        if (k==IDELEMS(res))
        {
          syEnlargeFields(syzstr,1);
          res=syzstr->res[1];
        }
        if (TEST_OPT_DEBUG)
        {
          if ((sPairs)[i].isNotMinimal==NULL)
          {
            PrintS("\nminimal generator: ");
            pWrite((syzstr->resPairs[0])[i].syz);
            PrintS("comes from: ");pWrite((syzstr->resPairs[0])[i].p1);
            PrintS("and: ");pWrite((syzstr->resPairs[0])[i].p2);
          }
        }
        res->m[k] = (sPairs)[i].syz;
        pNorm(res->m[k]);
        syHalfPair(res->m[k],k1,syzstr,1);
        k1++;
        k++;
        if (1>*maxindex) *maxindex = 1;
        if (deg-1>*maxdeg) *maxdeg = deg-1;
      }
    }
    i++;
  }
}

/*3
* reorders the result (stored in orderedRes) according
* to the seqence given by res
*/
static void syReOrdResult_Hilb(syStrategy syzstr,int maxindex,int maxdeg)
{
  ideal reor,toreor;
  int k,l,m,togo;
  syzstr->betti = new intvec(maxdeg,maxindex+1,0);
  if (syzstr->betti->length()>0)
  {
    (*syzstr->betti)[0] = 1;
    for (int i=1;i<=syzstr->length;i++)
    {
      if ((syzstr->orderedRes[i]!=NULL) && !idIs0(syzstr->orderedRes[i]))
      {
        toreor = syzstr->orderedRes[i];
        k = IDELEMS(toreor);
        while ((k>0) && (toreor->m[k-1]==NULL)) k--;
        reor = idInit(k,toreor->rank);
        togo = IDELEMS(syzstr->res[i]);
        for (int j=0;j<k;j++)
        {
          if (toreor->m[j]!=NULL) (IMATELEM(*syzstr->betti,p_FDeg(toreor->m[j],currRing)-i+1,i+1))++;
          reor->m[j] = toreor->m[j];
          toreor->m[j] = NULL;
        }
        m = 0;
        for (int j=0;j<togo;j++)
        {
          if (syzstr->res[i]->m[j]!=NULL)
          {
            l = 0;
            while ((l<k) && (syzstr->res[i]->m[j]!=reor->m[l])) l++;
            if (l<k)
            {
              toreor->m[m] = reor->m[l];
              reor->m[l] = NULL;
              m++;
            }
          }
        }
        idDelete(&reor);
      }
    }
  }
}

/*2
* the CoCoA-algorithm for free resolutions, using a formula
* for remaining pairs based on Hilbert-functions
*/
syStrategy syHilb(ideal arg,int * length)
{
  int i,j,actdeg=32000,index=0;
  int howmuch,toSub=0;
  int maxindex=0,maxdeg=0;
  ideal temp=NULL;
  SSet nextPairs;
  ring origR = currRing;
  syStrategy syzstr=(syStrategy)omAlloc0(sizeof(ssyStrategy));

  if ((idIs0(arg)) || (id_RankFreeModule(arg,currRing)>0))
  {
    syzstr->minres = (resolvente)omAllocBin(sip_sideal_bin);
    syzstr->length = 1;
    syzstr->minres[0] = idInit(1,arg->rank);
    return syzstr;
  }

  // Creare dp,S ring and change to it
  syzstr->syRing = rAssure_dp_C(origR);
  rChangeCurrRing(syzstr->syRing);

  // set initial ShiftedComps
  currcomponents = (int*)omAlloc0((arg->rank+1)*sizeof(int));
  currShiftedComponents = (long*)omAlloc0((arg->rank+1)*sizeof(long));

/*--- initializes the data structures---------------*/
#ifdef SHOW_CRIT
  crit = 0;
  crit1 = 0;
  spfl = 0;
  cons_pairs = 0;
  crit_fails = 0;
#endif
  syzstr->length = *length = currRing->N+2;
  syzstr->Tl = new intvec(*length+1);
  temp = idInit(IDELEMS(arg),arg->rank);
  for (i=0;i<IDELEMS(arg);i++)
  {
    if (origR != syzstr->syRing)
      temp->m[i] = prCopyR( arg->m[i], origR, syzstr->syRing);
    else
      temp->m[i] = pCopy( arg->m[i]);
    if (temp->m[i]!=NULL)
    {
      j = pTotaldegree(temp->m[i]);
      if (j<actdeg) actdeg = j;
    }
  }
  idTest(temp);
  idSkipZeroes(temp);
  syzstr->resPairs = syInitRes(temp,length,syzstr->Tl,syzstr->cw);
  omFreeSize((ADDRESS)currcomponents,(arg->rank+1)*sizeof(int));
  omFreeSize((ADDRESS)currShiftedComponents,(arg->rank+1)*sizeof(int));
  syzstr->res = (resolvente)omAlloc0((*length+1)*sizeof(ideal));
  syzstr->orderedRes = (resolvente)omAlloc0((*length+1)*sizeof(ideal));
  syzstr->elemLength = (int**)omAlloc0((*length+1)*sizeof(int*));
  syzstr->truecomponents = (int**)omAlloc0((*length+1)*sizeof(int*));
  syzstr->backcomponents = (int**)omAlloc0((*length+1)*sizeof(int*));
  syzstr->ShiftedComponents = (long**)omAlloc0((*length+1)*sizeof(long*));
  syzstr->Howmuch = (int**)omAlloc0((*length+1)*sizeof(int*));
  syzstr->Firstelem = (int**)omAlloc0((*length+1)*sizeof(int*));
  syzstr->hilb_coeffs = (intvec**)omAlloc0((*length+1)*sizeof(intvec*));
  syzstr->sev = (unsigned long **)omAlloc0((*length+1)*sizeof(unsigned long*));
  syzstr->bucket = kBucketCreate(currRing);
  syzstr->syz_bucket = kBucketCreate(currRing);
  nextPairs = syChosePairs(syzstr,&index,&howmuch,&actdeg);
/*--- computes the resolution ----------------------*/
  while (nextPairs!=NULL)
  {
#ifdef SHOW_PROT
Print("compute %d Paare im Module %d im Grad %d \n",howmuch,index,actdeg+index);
#endif
    if (TEST_OPT_PROT) Print("%d",actdeg);
    if (TEST_OPT_PROT) Print("(m%d)",index);
    if (index==0)
      i = syInitSyzMod(syzstr,index,id_RankFreeModule(arg, origR)+1);
    else
      i = syInitSyzMod(syzstr,index);
    j = syInitSyzMod(syzstr,index+1);
    if (index>0)
    {
      syRedNextPairs_Hilb(nextPairs,syzstr,howmuch,index,actdeg,&toSub,&maxindex,&maxdeg);
      syzstr->res[index+1]->rank=id_RankFreeModule(syzstr->res[index+1],currRing);
      sySetNewHilb(syzstr,toSub,index,actdeg);
      toSub = 0;
      syCompactifyPairSet(syzstr->resPairs[index],(*syzstr->Tl)[index],0);
    }
    else
      syRedGenerOfCurrDeg_Hilb(syzstr,actdeg,&maxindex,&maxdeg);
/*--- creates new pairs -----------------------------*/
#ifdef SHOW_PROT
Print("Bilde neue Paare in Modul %d!\n",index);
#endif
    syCreateNewPairs_Hilb(syzstr,index,actdeg);
    if (index<(*length)-1)
    {
#ifdef SHOW_PROT
Print("Bilde neue Paare in Modul %d!\n",index+1);
#endif
      syCreateNewPairs_Hilb(syzstr,index+1,actdeg-1);
    }
    index++;
    nextPairs = syChosePairs(syzstr,&index,&howmuch,&actdeg);
  }
  syReOrdResult_Hilb(syzstr,maxindex,maxdeg);
#ifdef SHOW_RESULT
PrintS("minimal resolution:\n");
for (int ti=1;ti<=*length;ti++)
{
  if (!idIs0(syzstr->orderedRes[ti])) idPrint(syzstr->orderedRes[ti]);
}
PrintS("full resolution:\n");
for (int ti=1;ti<=*length;ti++)
{
  if (!idIs0(syzstr->res[ti])) idPrint(syzstr->res[ti]);
}
#endif
#ifdef SHOW_CRIT
Print("Criterion %d times applied\n",crit);
Print("Criterion1 %d times applied\n",crit1);
Print("%d superfluous pairs\n",spfl);
Print("%d pairs considered\n",cons_pairs);
Print("Criterion fails %d times\n",crit_fails);
crit = 0;
crit1 = 0;
spfl = 0;
cons_pairs = 0;
crit_fails = 0;
#endif
  if (temp!=NULL) idDelete(&temp);
  kBucketDestroy(&(syzstr->bucket));
  kBucketDestroy(&(syzstr->syz_bucket));
  if (origR != syzstr->syRing)
    rChangeCurrRing(origR);
  else
    currRing =  origR;
  if (TEST_OPT_PROT) PrintLn();
  return syzstr;
}

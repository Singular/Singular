/****************************************
*  Computer Algebra System SINGULAR     *
****************************************/
/* $Id: syz.cc,v 1.27 1999-10-22 11:14:18 obachman Exp $ */

/*
* ABSTRACT: resolutions
*/


#include "mod2.h"
#include "tok.h"
#include "mmemory.h"
#include "polys.h"
#include "febase.h"
#include "kstd1.h"
#include "kutil.h"
#include "stairc.h"
#include "cntrlc.h"
#include "ipid.h"
#include "intvec.h"
#include "ipshell.h"
#include "numbers.h"
#include "ideals.h"
#include "intvec.h"
#include "ring.h"
#include "syz.h"

void syDeleteRes(resolvente * res,int length)
{
  for (int i=0;i<length;i++)
  {
    if (!idIs0((*res)[i]))
      idDelete(&((*res)[i]));
  }
  Free((ADDRESS)res,length*sizeof(ideal));
  *res = NULL;
}

static intvec * syPrepareModComp(ideal arg,intvec ** w)
{
  intvec *w1 = NULL;
  int i;
  BOOLEAN isIdeal=FALSE;

  if ((w==NULL) || (*w==NULL)) return w1;
  int maxxx = (*w)->length();
  if (maxxx==1)
  {
    maxxx = 2;
    isIdeal = TRUE;
  }
  w1 = NewIntvec1(maxxx+IDELEMS(arg));
  if (!isIdeal)
  {
    for (i=0;i<maxxx;i++)
    {
      (*w1)[i] = (**w)[i];
    }
  }
  for (i=maxxx;i<maxxx+IDELEMS(arg);i++)
  {
    if (arg->m[i-maxxx]!=NULL)
    {
      (*w1)[i] = pFDeg(arg->m[i-maxxx]);
      if (pGetComp(arg->m[i-maxxx])!=0)
      {
        (*w1)[i]+=(**w)[pGetComp(arg->m[i-maxxx])-1];
      }
    }
  }
  delete (*w);
  *w = NewIntvec1(IDELEMS(arg)+1);
  for (i=0;i<IDELEMS(arg);i++)
  {
     (**w)[i+1] = (*w1)[i+maxxx];
  }
  return w1;
}

static void syDeleteAbove(ideal up, int k)
{
  if (up!=NULL)
  {
    for (int i=0;i<IDELEMS(up);i++)
    {
      if (up->m[i]!=NULL)
        pDeleteComp(&(up->m[i]),k+1);
    }
  }
}

/*2
*minimizes the module mod and cancel superfluous syzygies
*from syz
*/
static void syMinStep(ideal mod,ideal syz,BOOLEAN final=FALSE,ideal up=NULL,
                      tHomog h=isNotHomog)
{
  ideal deg0=NULL;
  poly Unit1,Unit2,actWith;
  int len,i,j,ModComp,m,k,l;
  BOOLEAN searchUnit,existsUnit;

  if (TEST_OPT_PROT) PrintS("m");
  if ((final) && (h==isHomog))
  /*minim is TRUE, we are in the module: maxlength, maxlength <>0*/
  {
    deg0=idJet(syz,0);
    idSkipZeroes(deg0);
    syz=deg0;
  }
/*--cancels empty entees and their related components above--*/
  j = IDELEMS(syz);
  while ((j>0) && (!syz->m[j-1])) j--;
  k = 0;
  while (k<j)
  {
    if (syz->m[k]!=NULL)
      k++;
    else
    {
      if (TEST_OPT_PROT) PrintS(".");
      for (l=k;l<j-1;l++) syz->m[l] = syz->m[l+1];
      syz->m[j-1] = NULL;
      syDeleteAbove(up,k);
      j--;
    }
  }
/*--searches for syzygies coming from superfluous elements
* in the module below--*/
  searchUnit = TRUE;
  while (searchUnit)
  {
    i=0;
    j=IDELEMS(syz);
    while ((j>0) && (syz->m[j-1]==NULL)) j--;
    existsUnit = FALSE;
    if (currRing->OrdSgn == 1)
    {
      while ((i<j) && (!existsUnit))
      {
        existsUnit = pVectorHasUnitB(syz->m[i],&ModComp);
        i++;
      }
    }
    else
    {
      int I=0;
      l = 0;
      len=0;
      for (i=0;i<IDELEMS(syz);i++)
      {
        if (syz->m[i]!=NULL)
        {
          pVectorHasUnit(syz->m[i],&m, &l);
          if ((len==0) ||((l>0) && (l<len)))
          {
            len = l;
            ModComp = m;
            I = i;
          }
        }
      }
//Print("Laenge ist: %d\n",len);
      if (len>0) existsUnit = TRUE;
      i = I+1;
    }
    if (existsUnit)
    {
      i--;
//--takes out the founded syzygy--
      if (TEST_OPT_PROT) PrintS("f");
      actWith = syz->m[i];
      if (!rField_has_simple_inverse()) pCleardenom(actWith);
//Print("actWith: ");pWrite(actWith);
      syz->m[i] = NULL;
      for (k=i;k<j-1;k++)  syz->m[k] = syz->m[k+1];
      syz->m[j-1] = NULL;
      syDeleteAbove(up,i);
      j--;
//--makes Gauss alg. for the column ModComp--
      Unit1 = pTakeOutComp(&(actWith), ModComp);
//PrintS("actWith now: ");pWrite(actWith);
//Print("Unit1: ");pWrite(Unit1);
      k=0;
//Print("j= %d",j);
      while (k<j)
      {
        if (syz->m[k]!=NULL)
        {
          Unit2 = pTakeOutComp(&(syz->m[k]), ModComp);
//Print("element %d: ",k);pWrite(syz->m[k]);
//PrintS("Unit2: ");pWrite(Unit2);
          syz->m[k] = pMult(syz->m[k],pCopy(Unit1));
          syz->m[k] = pSub(syz->m[k],
            pMult(pCopy(actWith),Unit2));
          if (syz->m[k]==NULL)
          {
            for (l=k;l<j-1;l++)
              syz->m[l] = syz->m[l+1];
            syz->m[j-1] = NULL;
            j--;
            syDeleteAbove(up,k);
            k--;
          }
        }
        k++;
      }
      pDelete(&actWith);
      pDelete(&Unit1);
//--deletes superfluous elements from the module below---
      pDelete(&(mod->m[ModComp-1]));
      for (k=ModComp-1;k<IDELEMS(mod)-1;k++)
        mod->m[k] = mod->m[k+1];
      mod->m[IDELEMS(mod)-1] = NULL;
    }
    else
      searchUnit = FALSE;
  }
  if (TEST_OPT_PROT) PrintLn();
  idSkipZeroes(mod);
  idSkipZeroes(syz);
  if (deg0!=NULL)
    idDelete(&deg0);
}

/*2
* make Gauss for one element
*/
static void syGaussForOne(ideal syz, int elnum, int ModComp)
{
  int k,j,i,lu;
  poly unit1,unit2;
  poly actWith=syz->m[elnum];

  syz->m[elnum] = NULL;
  if (!rField_has_simple_inverse()) pCleardenom(actWith);
/*--makes Gauss alg. for the column ModComp--*/
  //unit1 = pTakeOutComp1(&(actWith), ModComp);
  pTakeOutComp(&(actWith), ModComp, &unit1, &lu);
  k=0;
  while (k<IDELEMS(syz))
  {
    if (syz->m[k]!=NULL)
    {
      //unit2 = pTakeOutComp1(&(syz->m[k]), ModComp);
      pTakeOutComp(&(syz->m[k]), ModComp, &unit2, &lu);
      syz->m[k] = pMult(syz->m[k],pCopy(unit1));
      syz->m[k] = pSub(syz->m[k],
        pMult(pCopy(actWith),unit2));
      if (syz->m[k]==NULL)
      {
        PrintS("Hier muss noch was gemacht werden\n");
      }
    }
    k++;
  }
  pDelete(&actWith);
  pDelete(&unit1);
}
static void syDeleteAbove1(ideal up, int k)
{
  poly p,pp;
  if (up!=NULL)
  {
    for (int i=0;i<IDELEMS(up);i++)
    {
      p = up->m[i];
      while ((p!=NULL) && (pGetComp(p)==k))
      {
        pp = pNext(p);
        pNext(p) = NULL;
        pDelete(&p);
        p = pp;
      }
      up->m[i] = p;
      if (p!=NULL)
      {
        while (pNext(p)!=NULL)
        {
          if (pGetComp(pNext(p))==k)
          {
            pp = pNext(pNext(p));
            pNext(pNext(p)) = NULL;
            pDelete(&pNext(p));
            pNext(p) = pp;
          }
          else
            pIter(p);
        }
      }
    }
  }
}
/*2
*minimizes the resolution res
*assumes homogeneous or local case
*/
static void syMinStep1(resolvente res, int length)
{
  int i,j,k,l,index=0;
  poly p;
  ideal deg0=NULL,reddeg0=NULL;
  intvec *have_del=NULL,*to_del=NULL;

  while ((index<length) && (res[index]!=NULL))
  {
/*---we take out dependend elements from syz---------------------*/
    if (res[index+1]!=NULL)
    {
      deg0 = idJet(res[index+1],0);
      reddeg0 = kInterRed(deg0);
      idDelete(&deg0);
      have_del = NewIntvec1(IDELEMS(res[index]));
      for (i=0;i<IDELEMS(reddeg0);i++)
      {
        if (reddeg0->m[i]!=NULL)
        {
          j = pGetComp(reddeg0->m[i]);
          pDelete(&(res[index]->m[j-1]));
          res[index]->m[j-1] = NULL;
          (*have_del)[j-1] = 1;
        }
      }
      idDelete(&reddeg0);
    }
    if (index>0)
    {
/*--- we search for units and perform Gaussian elimination------*/
      j = to_del->length();
      while (j>0)
      {
        if ((*to_del)[j-1]==1)
        {
          k = 0;
          while (k<IDELEMS(res[index]))
          {
            p = res[index]->m[k];
            while ((p!=NULL) && ((!pIsConstantComp(p)) || (pGetComp(p)!=j)))
              pIter(p);
            if ((p!=NULL) && (pIsConstantComp(p)) && (pGetComp(p)==j)) break;
            k++;
          }
          if (k>=IDELEMS(res[index]))
          {
            PrintS("out of range\n");
          }
          syGaussForOne(res[index],k,j);
          if (res[index+1]!=NULL)
            syDeleteAbove1(res[index+1],k+1);
          (*to_del)[j-1] = 0;
        }
        j--;
      }
    }
    if (to_del!=NULL) delete to_del;
    to_del = have_del;
    have_del = NULL;
    index++;
  }
  if (TEST_OPT_PROT) PrintLn();
  syKillEmptyEntres(res,length);
}

void syMinimizeResolvente(resolvente res, int length, int first)
{
  int syzIndex=first;
  intvec *dummy;

  if (syzIndex<1) syzIndex=1;
  if ((syzIndex==1) && (idHomModule(res[0],currQuotient,&dummy)))
  {
    syMinStep1(res,length);
    delete dummy;
    return;
  }
  while ((syzIndex<length-1) && (res[syzIndex]!=NULL) && (res[syzIndex+1]!=NULL))
  {
    syMinStep(res[syzIndex-1],res[syzIndex],FALSE,res[syzIndex+1]);
    syzIndex++;
  }
  if (res[syzIndex]!=NULL)
    syMinStep(res[syzIndex-1],res[syzIndex]);
}

/*2
* resolution of ideal/module arg, <=maxlength steps, (r[0..maxlength])
*   no limitation in length if maxlength==0
* input:arg
*       minim: TRUE means mres cmd, FALSE res cmd.
*       if *len!=0: modul weights: weights[0]
*          (and weights is defined:weights[0..len-1]
*
* output:resolvente r[0..length-1],
*        modul weights: weights[0..length-1]
*/
resolvente syResolvente(ideal arg, int maxlength, int * length,
                        intvec *** weights, BOOLEAN minim)
{
  resolvente res;
  resolvente newres;
  tHomog hom=isNotHomog;
  ideal temp=NULL;
  intvec *w,*w1=NULL,**tempW;
  int i,k,syzIndex = 0,j;
  int Kstd1_OldDeg=Kstd1_deg;
  BOOLEAN completeMinim;
  BOOLEAN oldDegBound=TEST_OPT_DEGBOUND;
  int wlength=*length;

  if (maxlength!=-1) *length = maxlength+1;
  else              *length = 5;
  if ((wlength!=0) && (*length!=wlength))
  {
    intvec **wtmp = (intvec**)Alloc0((*length)*sizeof(intvec*));
    wtmp[0]=(*weights)[0];
    Free((ADDRESS)*weights,wlength*sizeof(intvec*));
    *weights=wtmp;
  }
  res = (resolvente)Alloc0((*length)*sizeof(ideal));
  res[0] = idCopy(arg);
  if ((weights==NULL) || (*weights==NULL) || ((*weights)[0]==NULL))
  {
    hom=(tHomog)idHomModule(res[0],currQuotient,&w);
    if (hom==isHomog)
    {
      *weights = (intvec**)Alloc0((*length)*sizeof(intvec*));
      if (w!=NULL) (*weights)[0] = ivCopy(w);
    }
  }
  else
  {
    if ((weights!=NULL) && (*weights!=NULL)&& ((*weights)[0]!=NULL))
    {
      w = ivCopy((*weights)[0]);
      hom = isHomog;
    }
  }
  if (hom==isHomog)
  {
    w1 = syPrepareModComp(res[0],&w);
    if (w!=NULL) { delete w;w=NULL; }
  }
  while ((!idIs0(res[syzIndex])) &&
         ((maxlength==-1) || (syzIndex<=maxlength)))
   // (syzIndex<maxlength+(int)minim)))
  /*compute one step more for minimizing*/
  {
    if (Kstd1_deg!=0) Kstd1_deg++;
    if (syzIndex+1==*length)
    {
      newres = (resolvente)Alloc0((*length+5)*sizeof(ideal));
      tempW = (intvec**)Alloc0((*length+5)*sizeof(intvec*));
      for (j=0;j<*length;j++)
      {
        newres[j] = res[j];
        if (*weights!=NULL) tempW[j] = (*weights)[j];
        /*else              tempW[j] = NULL;*/
      }
      Free((ADDRESS)res,*length*sizeof(ideal));
      Free((ADDRESS)*weights,*length*sizeof(intvec*));
      *length += 5;
      res=newres;
      *weights = tempW;
    }
    if (minim || (syzIndex!=0))
    {
      temp = kInterRed(res[syzIndex],currQuotient);
      idDelete(&res[syzIndex]);
      idSkipZeroes(temp);
      res[syzIndex] = temp;
    }
    temp = NULL;
    if ((currQuotient==NULL)&&(syzIndex==0)&& (!TEST_OPT_DEGBOUND))
    {
      res[/*syzIndex+*/1] = idSyzygies(res[0/*syzIndex*/],
        NULL/*currQuotient*/,hom,&w1,TRUE,Kstd1_deg);
      if ((!TEST_OPT_NOTREGULARITY) && (Kstd1_deg>0)) test |= Sy_bit(OPT_DEGBOUND);
    }
    else
      res[syzIndex+1] = idSyzygies(res[syzIndex],currQuotient,hom,&w1);
    completeMinim=(syzIndex!=maxlength) || (maxlength ==-1) || (hom!=isHomog);
    syzIndex++;
    if (TEST_OPT_PROT) Print("[%d]\n",syzIndex);
    if ((minim)||(syzIndex>1))
      syMinStep(res[syzIndex-1],res[syzIndex],!completeMinim,NULL,hom);
    if (!completeMinim)
    /*minim is TRUE, we are in the module: maxlength, maxlength <>0*/
    {
      idDelete(&res[syzIndex]);
    }
    if ((hom == isHomog) && (!idIs0(res[syzIndex])))
    {
//Print("die %d Modulegewichte sind:\n",w1->length());
//w1->show();
//PrintLn();
      k = idRankFreeModule(res[syzIndex]);
      w = NewIntvec1(k+IDELEMS(res[syzIndex]));
      (*weights)[syzIndex] = NewIntvec1(k);
      for (i=0;i<k;i++)
      {
        if (res[syzIndex-1]->m[i]!=NULL) // hs
        {
          (*w)[i] = pFDeg(res[syzIndex-1]->m[i]);
          if (pGetComp(res[syzIndex-1]->m[i])>0)
            (*w)[i] += (*w1)[pGetComp(res[syzIndex-1]->m[i])-1];
          (*((*weights)[syzIndex]))[i] = (*w)[i];
        }
      }
      for (i=k;i<k+IDELEMS(res[syzIndex]);i++)
      {
        if (res[syzIndex]->m[i-k]!=NULL)
          (*w)[i] = pFDeg(res[syzIndex]->m[i-k])
                    +(*w)[pGetComp(res[syzIndex]->m[i-k])-1];
      }
      delete w1;
      w1 = w;
      w = NULL;
    }
  }
  if ((syzIndex!=0) && (res[syzIndex]!=NULL) && (idIs0(res[syzIndex])))
    idDelete(&res[syzIndex]);
  if (w1!=NULL) delete w1;
  //if ((maxlength!=-1) && (!minim))
  //{
  //  while (syzIndex<=maxlength)
  //  {
  //    res[syzIndex]=idInit(1,1);
  //    syzIndex++;
  //    if (syzIndex<=maxlength)
  //    {
  //      res[syzIndex]=idFreeModule(1);
  //      syzIndex++;
  //    }
  //  }
  //}
  Kstd1_deg=Kstd1_OldDeg;
  if (!oldDegBound)
    test &= ~Sy_bit(OPT_DEGBOUND);
  return res;
}

syStrategy syResolution(ideal arg, int maxlength,intvec * w, BOOLEAN minim)
{
  int typ0;
  syStrategy result=(syStrategy)Alloc0SizeOf(ssyStrategy);

  if (w!=NULL)
  {
    result->weights = (intvec**)Alloc0SizeOf(void_ptr);
    (result->weights)[0] = ivCopy(w);
    result->length = 1;
  }
  resolvente fr = syResolvente(arg,maxlength,&(result->length),&(result->weights),minim),fr1;
  if (minim)
  {
    result->minres = (resolvente)Alloc0((result->length+1)*sizeof(ideal));
    fr1 =  result->minres;
  }
  else
  {
    result->fullres = (resolvente)Alloc0((result->length+1)*sizeof(ideal));
    fr1 =  result->fullres;
  }
  for (int i=result->length-1;i>=0;i--)
  {
    if (fr[i]!=NULL)
      fr1[i] = fr[i];
    fr[i] = NULL;
  }
  Free((ADDRESS)fr,(result->length)*sizeof(ideal));
  return result;
}

int syDetect(ideal id,int index,BOOLEAN homog,int * degrees,int * tocancel)
{
  int i, j, k, ModComp,subFromRank=0, lu;
  poly p, q, qq, Unit1, Unit2;
  ideal temp;

  if (idIs0(id)) return 0;
  temp = idInit(IDELEMS(id),id->rank);
  for (i=0;i<IDELEMS(id);i++)
  {
    p = id->m[i];
    while (p!=NULL)
    {
      if (pIsConstantComp(p))
      {
        if (temp->m[i]==NULL)
        {
          temp->m[i] = pHead(p);
          q = temp->m[i];
        }
        else
        {
          pNext(q) = pHead(p);
          pIter(q);
        }
      }
      pIter(p);
    }
  }
  i = IDELEMS(id);
  while ((i>0) && (temp->m[i-1]==NULL)) i--;
  if (i==0)
  {
    idDelete(&temp);
    return 0;
  }
  j = 0;
  while ((j<i) && (temp->m[j]==NULL)) j++;
  if (j<i)
  {
    p = temp->m[j];
    temp->m[j] = NULL;
  }
  else
  {
    p = NULL;
  }
  while (p!=NULL)
  {
    if (homog)
    {
      k = pFDeg(id->m[j])+degrees[pGetComp(id->m[j])];
      if (k>=index) tocancel[k-index]++;
      if ((k>=0) && (index==0)) subFromRank++;
    }
    else
    {
      tocancel[0]--;
    }
    ModComp = pGetComp(p);
//Print("Element %d: ",j);pWrite(p);
    //Unit1 = pTakeOutComp1(&p,ModComp);
    pTakeOutComp(&p,ModComp, &Unit1, &lu);
    pSetComp(Unit1,0);
    for (k=j+1;k<i;k++)
    {
      if (temp->m[k]!=NULL)
      {
//Print("erst %d: ",k);pWrite(temp->m[k]);
        //Unit2 = pTakeOutComp1(&(temp->m[k]),ModComp);
        pTakeOutComp(&(temp->m[k]),ModComp,&Unit2,&lu);
        if (Unit2!=NULL)
        {
          number tmp=nCopy(pGetCoeff(Unit2));
          tmp=nNeg(tmp);
          qq = pCopy(p);
          pMultN(qq,tmp);
          nDelete(&tmp);
          pMultN(temp->m[k],pGetCoeff(Unit1));
          temp->m[k] = pAdd(temp->m[k],qq);
//Print("dann %d: ",k);pWrite(temp->m[k]);
          pDelete(&Unit2);
        }
      }
    }
//PrintLn();
    pDelete(&Unit1);
    pDelete(&p);
    j++;
    while ((j<i) && (temp->m[j]==NULL)) j++;
    if (j>=i)
    {
      idDelete(&temp);
      return  subFromRank;
    }
    p = temp->m[j];
    temp->m[j] = NULL;
  }
  idDelete(&temp);
  return subFromRank;
}

void syDetect(ideal id,int index,int rsmin, BOOLEAN homog,
              intvec * degrees,intvec * tocancel)
{
  int * deg=NULL;
  int * tocan=(int*) Alloc0(tocancel->length()*sizeof(int));
  int i;

  if (homog)
  {
    deg = (int*) Alloc0(degrees->length()*sizeof(int));
    for (i=degrees->length();i>0;i--)
      deg[i-1] = (*degrees)[i-1]-rsmin;
  }
  int dummy=syDetect(id,index,homog,deg,tocan);
  for (i=tocancel->length();i>0;i--)
    (*tocancel)[i-1] = tocan[i-1];
  if (homog)
    Free((ADDRESS)deg,degrees->length()*sizeof(int));
  Free((ADDRESS)tocan,tocancel->length()*sizeof(int));
}

/*2
* computes the betti numbers from a given resolution
* of length 'length' (0..length-1), not necessairily minimal,
* (if weights are given, they are used)
* returns the int matrix of betti numbers
* and the regularity
*/
intvec * syBetti(resolvente res,int length, int * regularity,
                 intvec* weights,BOOLEAN tomin)
{
  //tomin = FALSE;
  int i,j=0,k=0,l,rows,cols,mr;
  int *temp1,*temp2,*temp3;/*used to compute degrees*/
  int *tocancel; /*(BOOLEAN)tocancel[i]=element is superfluous*/

  /*------ compute size --------------*/
  *regularity = -1;
  cols = length;
  while ((cols>0)
  && ((res[cols-1]==NULL)
    || (idIs0(res[cols-1]))))
  {
    cols--;
  }
  intvec * result;
  if (idIs0(res[0]))
  {
    if (res[0]==NULL)
      result = NewIntvec3(1,1,1);
    else
      result = NewIntvec3(1,1,res[0]->rank);
    return result;
  }
  intvec *w=NULL;
  if (idHomModule(res[0],currQuotient,&w)!=isHomog)
  {
    Warn("betti-command: Input is not homogeneous!");
  }
  delete w;
  int rkl=l = max(idRankFreeModule(res[0]),res[0]->rank);
  i = 0;
  while ((i<length) && (res[i]!=NULL))
  {
    if (IDELEMS(res[i])>l) l = IDELEMS(res[i]);
    i++;
  }
  temp1 = (int*)Alloc0((l+1)*sizeof(int));
  temp2 = (int*)Alloc((l+1)*sizeof(int));
  rows = 1;
  mr = 1;
  cols++;
  for (i=0;i<cols-1;i++)
  {
    if ((i==0) && (weights!=NULL)) pSetModDeg(weights);
    memset(temp2,0,(l+1)*sizeof(int));
    for (j=0;j<IDELEMS(res[i]);j++)
    {
      if (res[i]->m[j]!=NULL)
     {
        if ((pGetComp(res[i]->m[j])>l)
        || ((i>1) && (res[i-1]->m[pGetComp(res[i]->m[j])-1]==NULL)))
        {
          WerrorS("input not a resolvent");
          Free((ADDRESS)temp1,(l+1)*sizeof(int));
          Free((ADDRESS)temp2,(l+1)*sizeof(int));
          return NULL;
        }
        temp2[j+1] = pFDeg(res[i]->m[j])+temp1[pGetComp(res[i]->m[j])];
        if (temp2[j+1]-i>rows) rows = temp2[j+1]-i;
        if (temp2[j+1]-i<mr) mr = temp2[j+1]-i;
      }
    }
    if ((i==0) && (weights!=NULL)) pSetModDeg(NULL);
    temp3 = temp1;
    temp1 = temp2;
    temp2 = temp3;
  }
  mr--;
  /*------ computation betti numbers --------------*/
  rows -= mr;
  result = NewIntvec3(rows,cols,0);
  (*result)[(-mr)*cols] = /*idRankFreeModule(res[0])*/ rkl;
  if ((!idIs0(res[0])) && ((*result)[(-mr)*cols]==0))
    (*result)[(-mr)*cols] = 1;
  tocancel = (int*)Alloc0((rows+1)*sizeof(int));
  memset(temp2,0,l*sizeof(int));
  memset(temp1,0,(l+1)*sizeof(int));
  int dummy = syDetect(res[0],0,TRUE,temp2,tocancel);
  if (tomin)
    (*result)[(-mr)*cols] -= dummy;
  for (i=0;i<cols-1;i++)
  {
    if ((i==0) && (weights!=NULL)) pSetModDeg(weights);
    memset(temp2,0,l*sizeof(int));
    for (j=0;j<IDELEMS(res[i]);j++)
    {
      if (res[i]->m[j]!=NULL)
      {
        temp2[j+1] = pFDeg(res[i]->m[j])+temp1[pGetComp(res[i]->m[j])];
        //(*result)[i+1+(temp2[j+1]-i-1)*cols]++;
        //if (temp2[j+1]>i) IMATELEM((*result),temp2[j+1]-i-mr,i+2)++;
        IMATELEM((*result),temp2[j+1]-i-mr,i+2)++;
      }
    }
  /*------ computation betti numbers, if res not minimal --------------*/
    if (tomin)
    {
      for (j=mr;j<rows+mr;j++)
      {
        //(*result)[i+1+j*cols] -= tocancel[j+1];
        IMATELEM((*result),j+1-mr,i+2) -= tocancel[j+1];
      }
      if ((i<length-1) && (res[i+1]!=NULL))
      {
        memset(tocancel,0,(rows+1)*sizeof(int));
        dummy = syDetect(res[i+1],i+1,TRUE,temp2,tocancel);
        for (j=0;j<rows;j++)
        {
          //(*result)[i+1+j*cols] -= tocancel[j];
          IMATELEM((*result),j+1,i+2) -= tocancel[j];
        }
      }
    }
    temp3 = temp1;
    temp1 = temp2;
    temp2 = temp3;
    for (j=0;j<rows;j++)
    {
    //  if (((*result)[i+1+j*cols]!=0) && (j>*regularity)) *regularity = j;
      if ((IMATELEM((*result),j+1,i+2)!=0) && (j>*regularity)) *regularity = j;
    }
    if ((i==0) && (weights!=NULL)) pSetModDeg(NULL);
  }
  /*------ clean up --------------*/
  Free((ADDRESS)tocancel,(rows+1)*sizeof(int));
  Free((ADDRESS)temp1,(l+1)*sizeof(int));
  Free((ADDRESS)temp2,(l+1)*sizeof(int));
  if ((tomin) && (mr<0))  // deletes the first (zero) line
  {
    for (j=1;j<=rows+mr;j++)
    {
      for (k=1;k<=cols;k++)
      {
        IMATELEM((*result),j,k) = IMATELEM((*result),j-mr,k);
      }
    }
    for (j=rows+mr+1;j<=rows;j++)
    {
      for (k=1;k<=cols;k++)
      {
        IMATELEM((*result),j,k) = 0;
      }
    }
  }
  j = 0;
  k = 0;
  for (i=0;i<rows*cols;i++)
  {
    if ((*result)[i] != 0)
    {
      if (i/cols>j) j = i/cols;
      if (i>k+(i/cols)*cols) k = i-(i/cols)*cols;
    }
  }
  intvec * exactresult=NewIntvec3(j+1,k+1,0);
  for (i=0;i<exactresult->rows();i++)
  {
    for (j=0;j<exactresult->cols();j++)
    {
      IMATELEM(*exactresult,i+1,j+1) = IMATELEM(*result,i+1,j+1);
    }
  }
  delete result;
  return exactresult;
}

/*2
* minbare via syzygies
*/
ideal syMinBase(ideal arg)
{
  intvec ** weights=NULL;
  int leng;
  if (idIs0(arg)) return idInit(1,arg->rank);
  resolvente res=syResolvente(arg,1,&leng,&weights,TRUE);
  ideal result=res[0];
  Free((ADDRESS)res,leng*sizeof(ideal));
  if (weights!=NULL)
  {
    if (*weights!=NULL)
    {
      delete (*weights);
      *weights=NULL;
    }
    if ((leng>=1) && (*(weights+1)!=NULL))
    {
      delete *(weights+1);
      *(weights+1)=NULL;
    }
  }
  idSkipZeroes(result);
  return result;
}

/*2
* computes Betti-numbers from a resolvente of
* (non-)homogeneous objects
* the numbers of entrees !=NULL in res and weights must be equal
* and < length
*/
intvec * syNewBetti(resolvente res, intvec ** weights, int length)
{
  intvec * result,*tocancel;
  int i,j,k,rsmin=0,rsmax=0,rs=0;
  BOOLEAN homog=TRUE;

  if (weights!=NULL)           //---homogeneous Betti numbers
  {
/*--------------computes size of the field----------------------*/
    for (i=1;i<length;i++)
    {
      if (weights[i] != NULL)
      {
        for (j=1;j<(weights[i])->length();j++)
        {
          if ((*(weights[i]))[j]-i<rsmin) rsmin = (*(weights[i]))[j]-i;
          if ((*(weights[i]))[j]-i>rsmax) rsmax = (*(weights[i]))[j]-i;
        }
      }
    }
    i = 0;
    while (weights[i] != NULL) i++;
    i--;
    for (j=0;j<IDELEMS(res[i]);j++)
    {
      if (res[i]->m[j]!=NULL)
      {
        k = pFDeg(res[i]->m[j])+(*(weights[i]))[pGetComp(res[i]->m[j])]-i-1;
        if (k>rsmax) rsmax = k;
        if (k<rsmin) rsmin = k;
      }
    }
    for (j=1;j<(weights[0])->length();j++)
    {
      if ((*weights[0])[j]>rsmax) rsmax = (*weights[0])[j];
      if ((*weights[0])[j]<rsmin) rsmin = (*weights[0])[j];
    }
//Print("rsmax = %d\n",rsmax);
//Print("rsmin = %d\n",rsmin);
    rs = rsmax-rsmin+1;
    result = NewIntvec3(rs,i+2,0);
    tocancel = NewIntvec1(rs);
/*-----------enter the Betti numbers-------------------------------*/
    if (/*idRankFreeModule(res[0])*/ res[0]->rank==0)
    {
      IMATELEM(*result,1-rsmin,1)=1;
    }
    else
    {
      for (i=1;i<(weights[0])->length();i++)
        IMATELEM(*result,(*weights[0])[i]+1-rsmin,1)++;
    }
    i = 1;
    while (weights[i]!=NULL)
    {
      for (j=1;j<(weights[i])->length();j++)
      {
        IMATELEM(*result,(*(weights[i]))[j]-i+1-rsmin,i+1)++;
      }
      i++;
    }
    i--;
    for (j=0;j<IDELEMS(res[i]);j++)
    {
      if (res[i]->m[j]!=NULL)
      {
        k = pFDeg(res[i]->m[j])+(*(weights[i]))[pGetComp(res[i]->m[j])]-i;
        IMATELEM(*result,k-rsmin,i+2)++;
      }
    }
  }
  else                //-----the non-homgeneous case
  {
    homog = FALSE;
    tocancel = NewIntvec1(1);
    k = length;
    while ((k>0) && (idIs0(res[k-1]))) k--;
    result = NewIntvec3(1,k+1,0);
    (*result)[0] = res[0]->rank;
    for (i=0;i<length;i++)
    {
      if (res[i]!=NULL)
      {
        for (j=0;j<IDELEMS(res[i]);j++)
        {
          if (res[i]->m[j]!=NULL) (*result)[i+1]++;
        }
      }
    }
  }
/*--------computes the Betti numbers for the minimized reolvente----*/

  i = 1;
  while ((res[i]!=NULL) && (weights[i]!=NULL))
  {
    syDetect(res[i],i,rsmin,homog,weights[i],tocancel);
    if (homog)
    {
      for (j=0;j<rs-1;j++)
      {
        IMATELEM((*result),j+1,i+1) -= (*tocancel)[j];
        IMATELEM((*result),j+1,i+2) -= (*tocancel)[j+1];
      }
      IMATELEM((*result),rs,i+1) -= (*tocancel)[rs-1];
    }
    else
    {
      (*result)[i+1] -= (*tocancel)[0];
      (*result)[i+2] -= (*tocancel)[0];
    }
    i++;
  }

/*--------print Betti numbers for control---------------------------*/
  for(i=rsmin;i<=rsmax;i++)
  {
    Print("%2d:",i);
    for(j=1;j<=result->cols();j++)
    {
      Print(" %5d",IMATELEM(*result,i-rsmin+1,j));
    }
    PrintLn();
  }
  return result;
}

/*2
* is looking for the minimal minimized module of a resolvente
* i.e. returns 0 if res comes from a mres-command and 1 in the
* case of res-commands
*/
int syIsMinimizedFrom(resolvente res,int length)
{
  poly p;
  int i,j=length;

  while ((j>0) && (res[j-1]==NULL)) j--;
  while (j>0)
  {
    for (i=0;i<IDELEMS(res[j-1]);i++)
    {
      p = res[j-1]->m[i];
      while (p!=NULL)
      {
        if (pIsConstantComp(p)) return j;
        p = pNext(p);
      }
    }
    j--;
  }
  return j;
}

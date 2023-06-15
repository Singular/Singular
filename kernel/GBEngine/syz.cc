/****************************************
*  Computer Algebra System SINGULAR     *
****************************************/

/*
* ABSTRACT: resolutions
*/

#include "kernel/mod2.h"
#include "misc/options.h"
#include "kernel/polys.h"
#include "kernel/GBEngine/kstd1.h"
#include "kernel/GBEngine/kutil.h"
#include "kernel/combinatorics/stairc.h"
#include "misc/intvec.h"
#include "coeffs/numbers.h"
#include "kernel/ideals.h"
#include "misc/intvec.h"
#include "polys/monomials/ring.h"
#include "kernel/GBEngine/syz.h"
#include "polys/prCopy.h"

#include "polys/nc/sca.h"

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
  w1 = new intvec(maxxx+IDELEMS(arg));
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
      (*w1)[i] = p_FDeg(arg->m[i-maxxx],currRing);
      if (pGetComp(arg->m[i-maxxx])!=0)
      {
        (*w1)[i]+=(**w)[pGetComp(arg->m[i-maxxx])-1];
      }
    }
  }
  delete (*w);
  *w = new intvec(IDELEMS(arg)+1);
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
static void syMinStep(ideal mod,ideal &syz,BOOLEAN final=FALSE,ideal up=NULL,
                      tHomog h=isNotHomog)
{
  poly Unit1,Unit2,actWith;
  int len,i,j,ModComp,m,k,l;
  BOOLEAN searchUnit,existsUnit;

  if (TEST_OPT_PROT) PrintS("m");
  if ((final) && (h==isHomog))
  /*minim is TRUE, we are in the module: maxlength, maxlength <>0*/
  {
    ideal deg0=id_Jet(syz,0,currRing);
    id_Delete(&syz,currRing);
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
  int curr_syz_limit = rGetCurrSyzLimit(currRing);
  BOOLEAN bHasGlobalOrdering=rHasGlobalOrdering(currRing);
  BOOLEAN bField_has_simple_inverse=rField_has_simple_inverse(currRing);
  while (searchUnit)
  {
    i=0;
    j=IDELEMS(syz);
    while ((j>0) && (syz->m[j-1]==NULL)) j--;
    existsUnit = FALSE;
    if (bHasGlobalOrdering)
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
      if (!bField_has_simple_inverse) p_Cleardenom(actWith, currRing);
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
          syz->m[k] = pMult(pCopy(Unit1),syz->m[k]);
          syz->m[k] = pSub(syz->m[k],
            pMult(Unit2,pCopy(actWith)));
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
      pDelete(&(mod->m[ModComp-1 - curr_syz_limit]));
      for (k=ModComp-1 - curr_syz_limit;k<IDELEMS(mod)-1;k++)
        mod->m[k] = mod->m[k+1];
      mod->m[IDELEMS(mod)-1] = NULL;
    }
    else
      searchUnit = FALSE;
  }
  if (TEST_OPT_PROT) PrintLn();
  idSkipZeroes(mod);
  idSkipZeroes(syz);
}

/*2
* make Gauss with the element elnum in the module component ModComp
* for the generators from - till
*/
void syGaussForOne(ideal syz, int elnum, int ModComp,int from,int till)
{
  int /*k,j,i,*/lu;
  poly unit1,unit2;
  poly actWith=syz->m[elnum];

  if (from<0) from = 0;
  if ((till<=0) || (till>IDELEMS(syz))) till = IDELEMS(syz);
  syz->m[elnum] = NULL;
  if (!rField_has_simple_inverse(currRing)) p_Cleardenom(actWith, currRing);
/*--makes Gauss alg. for the column ModComp--*/
  pTakeOutComp(&(actWith), ModComp, &unit1, &lu);
  while (from<till)
  {
    poly tmp=syz->m[from];
    if (/*syz->m[from]*/ tmp!=NULL)
    {
      pTakeOutComp(&(tmp), ModComp, &unit2, &lu);
      tmp = pMult(pCopy(unit1),tmp);
      syz->m[from] = pSub(tmp,
        pMult(unit2,pCopy(actWith)));
    }
    from++;
  }
  pDelete(&actWith);
  pDelete(&unit1);
}
static void syDeleteAbove1(ideal up, int k)
{
  poly p/*,pp*/;
  if (up!=NULL)
  {
    for (int i=0;i<IDELEMS(up);i++)
    {
      p = up->m[i];
      while ((p!=NULL) && (pGetComp(p)==k))
      {
        /*
        pp = pNext(p);
        pNext(p) = NULL;
        pDelete(&p);
        p = pp;
        */
        pLmDelete(&p);
      }
      up->m[i] = p;
      if (p!=NULL)
      {
        while (pNext(p)!=NULL)
        {
          if (pGetComp(pNext(p))==k)
          {
            /*
            pp = pNext(pNext(p));
            pNext(pNext(p)) = NULL;
            pDelete(&pNext(p));
            pNext(p) = pp;
            */
            pLmDelete(&pNext(p));
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
  int i,j,k,index=0;
  poly p;
  intvec *have_del=NULL,*to_del=NULL;

  while ((index<length) && (res[index]!=NULL))
  {
/*---we take out dependent elements from syz---------------------*/
    if (res[index+1]!=NULL)
    {
      ideal deg0 = id_Jet(res[index+1],0,currRing);
      ideal reddeg0 = kInterRedOld(deg0);
      idDelete(&deg0);
      have_del = new intvec(IDELEMS(res[index]));
      for (i=0;i<IDELEMS(reddeg0);i++)
      {
        if (reddeg0->m[i]!=NULL)
        {
          j = pGetComp(reddeg0->m[i]);
          pDelete(&(res[index]->m[j-1]));
          /*res[index]->m[j-1] = NULL;*/
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
            while ((p!=NULL) && ((!pLmIsConstantComp(p)) || (pGetComp(p)!=j)))
              pIter(p);
            if ((p!=NULL) && (pLmIsConstantComp(p)) && (pGetComp(p)==j)) break;
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
  if (to_del!=NULL) delete to_del;
}

void syMinimizeResolvente(resolvente res, int length, int first)
{
  int syzIndex=first;
  intvec *dummy;

  if (syzIndex<1) syzIndex=1;
  if ((syzIndex==1) && (!rIsPluralRing(currRing)) && (idHomModule(res[0],currRing->qideal,&dummy)))
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
  if (!idIs0(res[0]))
    idMinEmbedding(res[0],TRUE);
}

/*2
* resolution of ideal/module arg, <=maxlength steps, (r[0..maxlength])
*   no limitation in length if maxlength==0
* input:arg
*       minim: TRUE means mres cmd, FALSE nres cmd.
*       if *len!=0: module weights: weights[0]
*          (and weights is defined:weights[0..len-1]
*
* output:resolvente r[0..length-1],
*        module weights: weights[0..length-1]
*/
resolvente syResolvente(ideal arg, int maxlength, int * length,
                        intvec *** weights, BOOLEAN minim)
{
  BITSET save1;
  SI_SAVE_OPT1(save1);
  resolvente newres;
  tHomog hom=isNotHomog;
  intvec *w = NULL,**tempW;
  int i,k,syzIndex = 0,j,rk_arg=si_max(1,(int)id_RankFreeModule(arg,currRing));
  int Kstd1_OldDeg=Kstd1_deg;
  BOOLEAN completeMinim;
  BOOLEAN oldDegBound=TEST_OPT_DEGBOUND;
  BOOLEAN setRegularity=TRUE;
  int wlength=*length;

  if (maxlength!=-1) *length = maxlength+1;
  else              *length = 5;
  if ((wlength!=0) && (*length!=wlength))
  {
    intvec **wtmp = (intvec**)omAlloc0((*length)*sizeof(intvec*));
    wtmp[0]=(*weights)[0];
    omFreeSize((ADDRESS)*weights,wlength*sizeof(intvec*));
    *weights=wtmp;
  }
  resolvente res = (resolvente)omAlloc0((*length)*sizeof(ideal));

/*--- initialize the syzygy-ring -----------------------------*/
  ring origR = currRing;
  ring syz_ring = rAssure_SyzComp(origR, TRUE); // will do rChangeCurrRing if needed
  rSetSyzComp(rk_arg, syz_ring);

  if (syz_ring != origR)
  {
    rChangeCurrRing(syz_ring);
    res[0] = idrCopyR_NoSort(arg, origR, syz_ring);
  }
  else
  {
    res[0] = idCopy(arg);
  }

/*--- creating weights for the module components ---------------*/
  if ((weights!=NULL) && (*weights!=NULL)&& ((*weights)[0]!=NULL))
  {
    if (!idTestHomModule(res[0],currRing->qideal,(*weights)[0]))
    {
      WarnS("wrong weights given(1):"); (*weights)[0]->show();PrintLn();
      idHomModule(res[0],currRing->qideal,&w);
      w->show();PrintLn();
      *weights=NULL;
    }
  }

  if ((weights==NULL) || (*weights==NULL) || ((*weights)[0]==NULL))
  {
    hom=(tHomog)idHomModule(res[0],currRing->qideal,&w);
    if (hom==isHomog)
    {
      *weights = (intvec**)omAlloc0((*length)*sizeof(intvec*));
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

#ifdef HAVE_PLURAL
  if (rIsPluralRing(currRing) && !rIsSCA(currRing) )
  {
// quick solution; need theory to apply homog GB stuff for G-Algebras
    hom = isNotHomog;
  }
#endif // HAVE_PLURAL

  if (hom==isHomog)
  {
    intvec *w1 = syPrepareModComp(res[0],&w);
    if (w!=NULL) { delete w;w=NULL; }
    w = w1;
    j = 0;
    while ((j<IDELEMS(res[0])) && (res[0]->m[j]==NULL)) j++;
    if (j<IDELEMS(res[0]))
    {
      if (p_FDeg(res[0]->m[j],currRing)!=pTotaldegree(res[0]->m[j]))
        setRegularity = FALSE;
    }
  }
  else
  {
    setRegularity = FALSE;
  }

/*--- the main loop --------------------------------------*/
  while ((res[syzIndex]!=NULL) && (!idIs0(res[syzIndex])) &&
         ((maxlength==-1) || (syzIndex<=maxlength)))
   // (syzIndex<maxlength+(int)minim)))
/*--- compute one step more for minimizing-----------------*/
  {
    if (Kstd1_deg!=0) Kstd1_deg++;
    if (syzIndex+1==*length)
    {
      newres = (resolvente)omAlloc0((*length+5)*sizeof(ideal));
      tempW = (intvec**)omAlloc0((*length+5)*sizeof(intvec*));
      for (j=0;j<*length;j++)
      {
        newres[j] = res[j];
        if (*weights!=NULL) tempW[j] = (*weights)[j];
        /*else              tempW[j] = NULL;*/
      }
      omFreeSize((ADDRESS)res,*length*sizeof(ideal));
      if (*weights != NULL) omFreeSize((ADDRESS)*weights,*length*sizeof(intvec*));
      *length += 5;
      res=newres;
      *weights = tempW;
    }
/*--- interreducing first -----------------------------------*/
    if (syzIndex>0)
    {
      int rkI=id_RankFreeModule(res[syzIndex],currRing);
      rSetSyzComp(rkI, currRing);
    }
    if(! TEST_OPT_NO_SYZ_MINIM )
    if (minim || (syzIndex!=0))
    {
      ideal temp = kInterRedOld(res[syzIndex],currRing->qideal);
      idDelete(&res[syzIndex]);
      idSkipZeroes(temp);
      res[syzIndex] = temp;
    }
/*--- computing the syzygy modules --------------------------------*/
    if ((currRing->qideal==NULL)&&(syzIndex==0)&& (!TEST_OPT_DEGBOUND))
    {
      res[/*syzIndex+*/1] = idSyzygies(res[0/*syzIndex*/],hom,&w,FALSE,setRegularity,&Kstd1_deg);
      if ((!TEST_OPT_NOTREGULARITY) && (Kstd1_deg>0)
      && (!rField_is_Ring(currRing))
      ) si_opt_1 |= Sy_bit(OPT_DEGBOUND);
    }
    else
    {
        res[syzIndex+1] = idSyzygies(res[syzIndex],hom,&w,FALSE);
    }
    completeMinim=(syzIndex!=maxlength) || (maxlength ==-1) || (hom!=isHomog);
    syzIndex++;
    if (TEST_OPT_PROT) Print("[%d]\n",syzIndex);

    if(! TEST_OPT_NO_SYZ_MINIM )
    {
      if ((minim)||(syzIndex>1))
        syMinStep(res[syzIndex-1],res[syzIndex],!completeMinim,NULL,hom);
      if (!completeMinim)
      /*minim is TRUE, we are in the module: maxlength, maxlength <>0*/
      {
        idDelete(&res[syzIndex]);
      }
    }
/*---creating the iterated weights for module components ---------*/
    if ((hom == isHomog) && (res[syzIndex]!=NULL) && (!idIs0(res[syzIndex])))
    {
//Print("die %d Modulegewichte sind:\n",w1->length());
//w1->show();
//PrintLn();
      int max_comp = id_RankFreeModule(res[syzIndex],currRing);
      k = max_comp - rGetCurrSyzLimit(currRing);
      assume(w != NULL);
      if (w != NULL)
        w->resize(max_comp+IDELEMS(res[syzIndex]));
      else
        w = new intvec(max_comp+IDELEMS(res[syzIndex]));
      (*weights)[syzIndex] = new intvec(k);
      for (i=0;i<k;i++)
      {
        if (res[syzIndex-1]->m[i]!=NULL) // hs
        {
          (*w)[i + rGetCurrSyzLimit(currRing)] = p_FDeg(res[syzIndex-1]->m[i],currRing);
          if (pGetComp(res[syzIndex-1]->m[i])>0)
            (*w)[i + rGetCurrSyzLimit(currRing)]
              += (*w)[pGetComp(res[syzIndex-1]->m[i])-1];
          (*((*weights)[syzIndex]))[i] = (*w)[i+rGetCurrSyzLimit(currRing)];
        }
      }
      for (i=k;i<k+IDELEMS(res[syzIndex]);i++)
      {
        if (res[syzIndex]->m[i-k]!=NULL)
          (*w)[i+rGetCurrSyzLimit(currRing)] = p_FDeg(res[syzIndex]->m[i-k],currRing)
                    +(*w)[pGetComp(res[syzIndex]->m[i-k])-1];
      }
    }
  }
/*--- end of the main loop --------------------------------------*/
/*--- deleting the temporare data structures --------------------*/
  if ((syzIndex!=0) && (res[syzIndex]!=NULL) && (idIs0(res[syzIndex])))
    idDelete(&res[syzIndex]);
  if (w !=NULL) delete w;

  Kstd1_deg=Kstd1_OldDeg;
  if (!oldDegBound)
    si_opt_1 &= ~Sy_bit(OPT_DEGBOUND);

  for (i=1; i<=syzIndex; i++)
  {
    if ((res[i]!=NULL) && ! idIs0(res[i]))
    {
      id_Shift(res[i],-rGetMaxSyzComp(i, currRing),currRing);
      res[i]->rank=idElem(res[i-1]);
    }
  }
/*--- going back to the original ring -------------------------*/
  if (origR != syz_ring)
  {
    rChangeCurrRing(origR); // should not be needed now?
    for (i=0; i<=syzIndex; i++)
    {
      res[i] = idrMoveR_NoSort(res[i], syz_ring, origR);
    }
    rDelete(syz_ring);
  }
  SI_RESTORE_OPT1(save1);
  return res;
}

syStrategy syResolution(ideal arg, int maxlength,intvec * w, BOOLEAN minim)
{

#ifdef HAVE_PLURAL
  const ideal idSaveCurrRingQuotient = currRing->qideal;
  if( rIsSCA(currRing) )
  {
    if( ncExtensions(TESTSYZSCAMASK) )
    {
      currRing->qideal = SCAQuotient(currRing);
    }
    const unsigned int m_iFirstAltVar = scaFirstAltVar(currRing);
    const unsigned int m_iLastAltVar  = scaLastAltVar(currRing);
    arg = id_KillSquares(arg, m_iFirstAltVar, m_iLastAltVar, currRing, false); // kill suares in input!
  }
#endif

  syStrategy result=(syStrategy)omAlloc0(sizeof(ssyStrategy));

  if ((w!=NULL) && (!idTestHomModule(arg,currRing->qideal,w))) // is this right in SCA case???
  {
    WarnS("wrong weights given(2):");w->show();PrintLn();
    idHomModule(arg,currRing->qideal,&w);
    w->show();PrintLn();
    w=NULL;
  }
  if (w!=NULL)
  {
    result->weights = (intvec**)omAlloc0Bin(char_ptr_bin);
    (result->weights)[0] = ivCopy(w);
    result->length = 1;
  }
  resolvente fr = syResolvente(arg,maxlength,&(result->length),&(result->weights),minim);
  resolvente fr1;
  if (minim)
  {
    result->minres = (resolvente)omAlloc0((result->length+1)*sizeof(ideal));
    fr1 =  result->minres;
  }
  else
  {
    result->fullres = (resolvente)omAlloc0((result->length+1)*sizeof(ideal));
    fr1 =  result->fullres;
  }
  for (int i=result->length-1;i>=0;i--)
  {
    if (fr[i]!=NULL)
      fr1[i] = fr[i];
    fr[i] = NULL;
  }
  omFreeSize((ADDRESS)fr,(result->length)*sizeof(ideal));

#ifdef HAVE_PLURAL
  if( rIsSCA(currRing) )
  {
    if( ncExtensions(TESTSYZSCAMASK) )
    {
      currRing->qideal = idSaveCurrRingQuotient;
    }
    id_Delete(&arg, currRing);
  }
#endif

  return result;
}

static poly sypCopyConstant(poly inp)
{
  poly outp=NULL,q;

  while (inp!=NULL)
  {
    if (pLmIsConstantComp(inp))
    {
      if (outp==NULL)
      {
        q = outp = pHead(inp);
      }
      else
      {
        pNext(q) = pHead(inp);
        pIter(q);
      }
    }
    pIter(inp);
  }
  return outp;
}
int syDetect(ideal id,int index,BOOLEAN homog,int * degrees,int * tocancel)
{
  int i, j, k, subFromRank=0;
  ideal temp;

  if (idIs0(id)) return 0;
  temp = idInit(IDELEMS(id),id->rank);
  for (i=0;i<IDELEMS(id);i++)
  {
    temp->m[i] = sypCopyConstant(id->m[i]);
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
  while (j<i)
  {
    if (homog)
    {
      if (index==0) k = p_FDeg(temp->m[j],currRing)+degrees[pGetComp(temp->m[j])];
      else          k = degrees[pGetComp(temp->m[j])];
      if (k>=index) tocancel[k-index]++;
      if ((k>=0) && (index==0)) subFromRank++;
    }
    else
    {
      tocancel[0]--;
    }
    syGaussForOne(temp,j,pGetComp(temp->m[j]),j+1,i);
    j++;
    while ((j<i) && (temp->m[j]==NULL)) j++;
  }
  idDelete(&temp);
  return subFromRank;
}

void syDetect(ideal id,int index,int rsmin, BOOLEAN homog,
              intvec * degrees,intvec * tocancel)
{
  int * deg=NULL;
  int * tocan=(int*) omAlloc0(tocancel->length()*sizeof(int));
  int i;

  if (homog)
  {
    deg = (int*) omAlloc0(degrees->length()*sizeof(int));
    for (i=degrees->length();i>0;i--)
      deg[i-1] = (*degrees)[i-1]-rsmin;
  }
  syDetect(id,index,homog,deg,tocan);
  for (i=tocancel->length();i>0;i--)
    (*tocancel)[i-1] = tocan[i-1];
  if (homog)
    omFreeSize((ADDRESS)deg,degrees->length()*sizeof(int));
  omFreeSize((ADDRESS)tocan,tocancel->length()*sizeof(int));
}

/*2
* computes the betti numbers from a given resolution
* of length 'length' (0..length-1), not necessairily minimal,
* (if weights are given, they are used)
* returns the int matrix of betti numbers
* and the regularity
*/
intvec * syBetti(resolvente res,int length, int * regularity,
                 intvec* weights,BOOLEAN tomin,int * row_shift)
{
//#define BETTI_WITH_ZEROS
  //tomin = FALSE;
  int i,j=0,k=0,l,rows,cols,mr;
  int *temp1,*temp2,*temp3;/*used to compute degrees*/
  int *tocancel; /*(BOOLEAN)tocancel[i]=element is superfluous*/
  int r0_len;

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
      result = new intvec(1,1,1);
    else
      result = new intvec(1,1,res[0]->rank);
    return result;
  }
  intvec *w=NULL;
  if (weights!=NULL)
  {
    if (!idTestHomModule(res[0],currRing->qideal,weights))
    {
      WarnS("wrong weights given(3):");weights->show();PrintLn();
      idHomModule(res[0],currRing->qideal,&w);
      if (w!=NULL) { w->show();PrintLn();}
      weights=NULL;
    }
  }
#if 0
  if (idHomModule(res[0],currRing->qideal,&w)!=isHomog)
  {
    WarnS("betti-command: Input is not homogeneous!");
    weights=NULL;
  }
#endif
  if (weights==NULL) weights=w;
  else delete w;
  r0_len=IDELEMS(res[0]);
  while ((r0_len>0) && (res[0]->m[r0_len-1]==NULL)) r0_len--;
  #ifdef SHOW_W
  PrintS("weights:");if (weights!=NULL) weights->show(); else Print("NULL"); PrintLn();
  #endif
  int rkl=l = si_max(id_RankFreeModule(res[0],currRing),res[0]->rank);
  i = 0;
  while ((i<length) && (res[i]!=NULL))
  {
    if (IDELEMS(res[i])>l) l = IDELEMS(res[i]);
    i++;
  }
  temp1 = (int*)omAlloc0((l+1)*sizeof(int));
  temp2 = (int*)omAlloc((l+1)*sizeof(int));
  rows = 1;
  mr = 1;
  cols++;
  for (i=0;i<cols-1;i++)
  {
    if ((i==0) && (weights!=NULL)) p_SetModDeg(weights, currRing);
    memset(temp2,0,(l+1)*sizeof(int));
    for (j=0;j<IDELEMS(res[i]);j++)
    {
      if (res[i]->m[j]!=NULL)
      {
        if ((pGetComp(res[i]->m[j])>l)
	// usual resolutions do not the following, but artifulal built may: (tr. #763)
        //|| ((i>1) && (res[i-1]->m[pGetComp(res[i]->m[j])-1]==NULL))
	)
        {
          WerrorS("input not a resolution");
          omFreeSize((ADDRESS)temp1,(l+1)*sizeof(int));
          omFreeSize((ADDRESS)temp2,(l+1)*sizeof(int));
          return NULL;
        }
        temp2[j+1] = p_FDeg(res[i]->m[j],currRing)+temp1[pGetComp(res[i]->m[j])];
        if (temp2[j+1]-i>rows) rows = temp2[j+1]-i;
        if (temp2[j+1]-i<mr) mr = temp2[j+1]-i;
      }
    }
    if ((i==0) && (weights!=NULL)) p_SetModDeg(NULL, currRing);
    temp3 = temp1;
    temp1 = temp2;
    temp2 = temp3;
  }
  mr--;
  if (weights!=NULL)
  {
    for(j=0;j<weights->length();j++)
    {
      if (rows <(*weights)[j]+1) rows=(-mr)+(*weights)[j]+1;
    }
  }
  /*------ computation betti numbers --------------*/
  rows -= mr;
  result = new intvec(rows+1,cols,0);
  if (weights!=NULL)
  {
    for(j=0;j<weights->length();j++)
    {
      IMATELEM((*result),(-mr)+(*weights)[j]+1,1) ++;
      //Print("imat(%d,%d)++ -> %d\n",(-mr)+(*weights)[j]+1, 1, IMATELEM((*result),(-mr)+(*weights)[j]+1,1));
    }
  }
  else
  {
    (*result)[(-mr)*cols] = /*idRankFreeModule(res[0])*/ rkl;
    if ((!idIs0(res[0])) && ((*result)[(-mr)*cols]==0))
      (*result)[(-mr)*cols] = 1;
  }
  tocancel = (int*)omAlloc0((rows+1)*sizeof(int));
  memset(temp1,0,(l+1)*sizeof(int));
  if (weights!=NULL)
  {
    memset(temp2,0,l*sizeof(int));
    p_SetModDeg(weights, currRing);
  }
  else
    memset(temp2,0,l*sizeof(int));
  syDetect(res[0],0,TRUE,temp2,tocancel);
  if (weights!=NULL) p_SetModDeg(NULL, currRing);
  if (tomin)
  {
    //(*result)[(-mr)*cols] -= dummy;
    for(j=0;j<=rows+mr;j++)
    {
      //Print("tocancel[%d]=%d imat(%d,%d)=%d\n",j,tocancel[j],(-mr)+j+1,1,IMATELEM((*result),(-mr)+j+1,1));
      IMATELEM((*result),(-mr)+j+1,1) -= tocancel[j];
    }
  }
  for (i=0;i<cols-1;i++)
  {
    if ((i==0) && (weights!=NULL)) p_SetModDeg(weights, currRing);
    memset(temp2,0,l*sizeof(int));
    for (j=0;j<IDELEMS(res[i]);j++)
    {
      if (res[i]->m[j]!=NULL)
      {
        temp2[j+1] = p_FDeg(res[i]->m[j],currRing)+temp1[pGetComp(res[i]->m[j])];
        //(*result)[i+1+(temp2[j+1]-i-1)*cols]++;
        //if (temp2[j+1]>i) IMATELEM((*result),temp2[j+1]-i-mr,i+2)++;
        IMATELEM((*result),temp2[j+1]-i-mr,i+2)++;
      }
      else if (i==0)
      {
        if (j<r0_len) IMATELEM((*result),-mr,2)++;
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
        syDetect(res[i+1],i+1,TRUE,temp2,tocancel);
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
    for (j=0;j<=rows;j++)
    {
    //  if (((*result)[i+1+j*cols]!=0) && (j>*regularity)) *regularity = j;
      if ((IMATELEM((*result),j+1,i+2)!=0) && (j>*regularity)) *regularity = j;
    }
    if ((i==0) && (weights!=NULL)) p_SetModDeg(NULL, currRing);
  }
  // Print("nach minim:\n"); result->show(); PrintLn();
  /*------ clean up --------------*/
  omFreeSize((ADDRESS)tocancel,(rows+1)*sizeof(int));
  omFreeSize((ADDRESS)temp1,(l+1)*sizeof(int));
  omFreeSize((ADDRESS)temp2,(l+1)*sizeof(int));
  if ((tomin) && (mr<0))  // deletes the first (zero) line
  {
    for (j=1;j<=rows+mr+1;j++)
    {
      for (k=1;k<=cols;k++)
      {
        IMATELEM((*result),j,k) = IMATELEM((*result),j-mr,k);
      }
    }
    for (j=rows+mr+1;j<=rows+1;j++)
    {
      for (k=1;k<=cols;k++)
      {
        IMATELEM((*result),j,k) = 0;
      }
    }
  }
  j = 0;
  k = 0;
  for (i=1;i<=result->rows();i++)
  {
    for(l=1;l<=result->cols();l++)
    if (IMATELEM((*result),i,l) != 0)
    {
      j = si_max(j, i-1);
      k = si_max(k, l-1);
    }
  }
  intvec * exactresult=new intvec(j+1,k+1,0);
  for (i=0;i<exactresult->rows();i++)
  {
    for (j=0;j<exactresult->cols();j++)
    {
      IMATELEM(*exactresult,i+1,j+1) = IMATELEM(*result,i+1,j+1);
    }
  }
  if (row_shift!=NULL) *row_shift = mr;
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
  omFreeSize((ADDRESS)res,leng*sizeof(ideal));
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

#if 0  /* currently used: syBetti */
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
        k = p_FDeg(res[i]->m[j],currRing)+(*(weights[i]))[pGetComp(res[i]->m[j])]-i-1;
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
    result = new intvec(rs,i+2,0);
    tocancel = new intvec(rs);
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
        k = p_FDeg(res[i]->m[j],currRing)+(*(weights[i]))[pGetComp(res[i]->m[j])]-i;
        IMATELEM(*result,k-rsmin,i+2)++;
      }
    }
  }
  else                //-----the non-homgeneous case
  {
    homog = FALSE;
    tocancel = new intvec(1);
    k = length;
    while ((k>0) && (idIs0(res[k-1]))) k--;
    result = new intvec(1,k+1,0);
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
#endif


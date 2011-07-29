/****************************************
*  Computer Algebra System SINGULAR     *
****************************************/
/* $Id: ideals.cc 14320 2011-07-04 14:48:27Z hannes $ */
/*
* ABSTRACT - all basic methods to manipulate ideals
*/

/* includes */
#include "mod2.h"


#ifndef NDEBUG
# define MYTEST 0
#else /* ifndef NDEBUG */
# define MYTEST 0
#endif /* ifndef NDEBUG */

#include <omalloc/omalloc.h>

#include <misc/options.h>
#include <misc/intvec.h>

#include <coeffs/coeffs.h>
#include <coeffs/numbers.h>

#include <polys/polys.h>
#include <polys/monomials/ring.h>
#include <polys/matpol.h>
#include <polys/weight.h>
#include <polys/sparsmat.h>
#include <polys/prCopy.h>
#include <polys/nc/nc.h>


#include <kernel/ideals.h>

#include <kernel/febase.h>
#include <kernel/kstd1.h>
#include <kernel/syz.h>

#include <kernel/longrat.h>


/* #define WITH_OLD_MINOR */
#define pCopy_noCheck(p) pCopy(p)

static poly * idpower;
/*collects the monomials in makemonoms, must be allocated befor*/
static int idpowerpoint;
/*index of the actual monomial in idpower*/
static poly * givenideal;
/*the ideal from which a power is computed*/

/*0 implementation*/

/*2
*returns a minimized set of generators of h1
*/
ideal idMinBase (ideal h1)
{
  ideal h2, h3,h4,e;
  int j,k;
  int i,l,ll;
  intvec * wth;
  BOOLEAN homog;

  homog = idHomModule(h1,currQuotient,&wth);
  if (rHasGlobalOrdering(currRing))
  {
    if(!homog)
    {
      WarnS("minbase applies only to the local or homogeneous case");
      e=idCopy(h1);
      return e;
    }
    else
    {
      ideal re=kMin_std(h1,currQuotient,(tHomog)homog,&wth,h2,NULL,0,3);
      idDelete(&re);
      return h2;
    }
  }
  e=idInit(1,h1->rank);
  if (idIs0(h1))
  {
    return e;
  }
  pEnlargeSet(&(e->m),IDELEMS(e),15);
  IDELEMS(e) = 16;
  h2 = kStd(h1,currQuotient,isNotHomog,NULL);
  h3 = idMaxIdeal(1);
  h4=idMult(h2,h3);
  idDelete(&h3);
  h3=kStd(h4,currQuotient,isNotHomog,NULL);
  k = IDELEMS(h3);
  while ((k > 0) && (h3->m[k-1] == NULL)) k--;
  j = -1;
  l = IDELEMS(h2);
  while ((l > 0) && (h2->m[l-1] == NULL)) l--;
  for (i=l-1; i>=0; i--)
  {
    if (h2->m[i] != NULL)
    {
      ll = 0;
      while ((ll < k) && ((h3->m[ll] == NULL)
      || !pDivisibleBy(h3->m[ll],h2->m[i])))
        ll++;
      if (ll >= k)
      {
        j++;
        if (j > IDELEMS(e)-1)
        {
          pEnlargeSet(&(e->m),IDELEMS(e),16);
          IDELEMS(e) += 16;
        }
        e->m[j] = pCopy(h2->m[i]);
      }
    }
  }
  idDelete(&h2);
  idDelete(&h3);
  idDelete(&h4);
  if (currQuotient!=NULL)
  {
    h3=idInit(1,e->rank);
    h2=kNF(h3,currQuotient,e);
    idDelete(&h3);
    idDelete(&e);
    e=h2;
  }
  idSkipZeroes(e);
  return e;
}


/*3
*multiplies p with t (!cas) or  (t-1)
*the index of t is:1, so we have to shift all variables
*p is NOT in the actual ring, it has no t
*/
static poly pMultWithT (poly p,BOOLEAN cas)
{
  /*qp is the working pointer in p*/
  /*result is the result, qresult is the working pointer*/
  /*pp is p in the actual ring(shifted), qpp the working pointer*/
  poly result,qp,pp;
  poly qresult=NULL;
  poly qpp=NULL;
  int  i,j,lex;
  number n;

  pp = NULL;
  result = NULL;
  qp = p;
  while (qp != NULL)
  {
    i = 0;
    if (result == NULL)
    {/*first monomial*/
      result = pInit();
      qresult = result;
    }
    else
    {
      qresult->next = pInit();
      pIter(qresult);
    }
    for (j=(currRing->N)-1; j>0; j--)
    {
      lex = pGetExp(qp,j);
      pSetExp(qresult,j+1,lex);/*copy all variables*/
    }
    lex = pGetComp(qp);
    pSetComp(qresult,lex);
    n=nCopy(pGetCoeff(qp));
    pSetCoeff0(qresult,n);
    qresult->next = NULL;
    pSetm(qresult);
    /*qresult is now qp brought into the actual ring*/
    if (cas)
    { /*case: mult with t-1*/
      pSetExp(qresult,1,0);
      pSetm(qresult);
      if (pp == NULL)
      { /*first monomial*/
        pp = pCopy(qresult);
        qpp = pp;
      }
      else
      {
        qpp->next = pCopy(qresult);
        pIter(qpp);
      }
      pGetCoeff(qpp)=nNeg(pGetCoeff(qpp));
      /*now qpp contains -1*qp*/
    }
    pSetExp(qresult,1,1);/*this is mult. by t*/
    pSetm(qresult);
    pIter(qp);
  }
  /*
  *now p is processed:
  *result contains t*p
  * if cas: pp contains -1*p (in the new ring)
  */
  if (cas)  qresult->next = pp;
  /*  else      qresult->next = NULL;*/
  return result;
}

/*2
*initialized a field with r numbers between beg and end for the
*procedure idNextChoise
*/
ideal idSectWithElim (ideal h1,ideal h2)
// does not destroy h1,h2
{
  if (TEST_OPT_PROT) PrintS("intersect by elimination method\n");
  assume(!idIs0(h1));
  assume(!idIs0(h2));
  assume(IDELEMS(h1)<=IDELEMS(h2));
  assume(id_RankFreeModule(h1,currRing)==0);
  assume(id_RankFreeModule(h2,currRing)==0);
  // add a new variable:
  int j;
  ring origRing=currRing;
  ring r=rCopy0(origRing);
  r->N++;
  r->block0[0]=1;
  r->block1[0]= r->N;
  omFree(r->order);
  r->order=(int*)omAlloc0(3*sizeof(int*));
  r->order[0]=ringorder_dp;
  r->order[1]=ringorder_C;
  char **names=(char**)omAlloc0(rVar(r) * sizeof(char_ptr));
  for (j=0;j<r->N-1;j++) names[j]=r->names[j];
  names[r->N-1]=omStrDup("@");
  omFree(r->names);
  r->names=names;
  rComplete(r,TRUE);
  // fetch h1, h2
  ideal h;
  h1=idrCopyR(h1,origRing,r);
  h2=idrCopyR(h2,origRing,r);
  // switch to temp. ring r
  rChangeCurrRing(r);
  // create 1-t, t
  poly omt=p_One(currRing);
  p_SetExp(omt,r->N,1,currRing);
  poly t=p_Copy(omt,currRing);
  p_Setm(omt,currRing);
  omt=p_Neg(omt,currRing);
  omt=p_Add_q(omt,pOne(),currRing);
  // compute (1-t)*h1
  h1=(ideal)mp_MultP((matrix)h1,omt,currRing);
  // compute t*h2
  h2=(ideal)mp_MultP((matrix)h2,pCopy(t),currRing);
  // (1-t)h1 + t*h2
  h=idInit(IDELEMS(h1)+IDELEMS(h2),1);
  int l;
  for (l=IDELEMS(h1)-1; l>=0; l--)
  {
    h->m[l] = h1->m[l];  h1->m[l]=NULL;
  }
  j=IDELEMS(h1);
  for (l=IDELEMS(h2)-1; l>=0; l--)
  {
    h->m[l+j] = h2->m[l];  h2->m[l]=NULL;
  }
  idDelete(&h1);
  idDelete(&h2);
  // eliminate t:

  ideal res=idElimination(h,t);
  // cleanup 
  idDelete(&h);
  res=idrMoveR(res,r,origRing);
  rChangeCurrRing(origRing);
  rKill(r);
  return res;
}
/*2
* h3 := h1 intersect h2
*/
ideal idSect (ideal h1,ideal h2)
{
  int i,j,k,length;
  int flength = id_RankFreeModule(h1,currRing);
  int slength = id_RankFreeModule(h2,currRing);
  int rank=si_min(flength,slength);
  if ((idIs0(h1)) || (idIs0(h2)))  return idInit(1,rank);

  ideal first,second,temp,temp1,result;
  poly p,q;

  if (IDELEMS(h1)<IDELEMS(h2))
  {
    first = h1;
    second = h2;
  }
  else
  {
    first = h2;
    second = h1;
    int t=flength; flength=slength; slength=t;
  }
  length  = si_max(flength,slength);
  if (length==0)
  {
    if ((currQuotient==NULL)
    && (currRing->OrdSgn==1)
    && (!rIsPluralRing(currRing))
    && ((TEST_V_INTERSECT_ELIM) || (!TEST_V_INTERSECT_SYZ)))
      return idSectWithElim(first,second);
    else length = 1;
  }
  if (TEST_OPT_PROT) PrintS("intersect by syzygy methods\n");
  j = IDELEMS(first);

  ring orig_ring=currRing;
  ring syz_ring=rAssure_SyzComp(orig_ring,TRUE);
  rChangeCurrRing(syz_ring);
  rSetSyzComp(length, syz_ring);

  while ((j>0) && (first->m[j-1]==NULL)) j--;
  temp = idInit(j /*IDELEMS(first)*/+IDELEMS(second),length+j);
  k = 0;
  for (i=0;i<j;i++)
  {
    if (first->m[i]!=NULL)
    {
      if (syz_ring==orig_ring)
        temp->m[k] = pCopy(first->m[i]);
      else
        temp->m[k] = prCopyR(first->m[i], orig_ring, syz_ring);
      q = pOne();
      pSetComp(q,i+1+length);
      pSetmComp(q);
      if (flength==0) p_Shift(&(temp->m[k]),1,currRing);
      p = temp->m[k];
      while (pNext(p)!=NULL) pIter(p);
      pNext(p) = q;
      k++;
    }
  }
  for (i=0;i<IDELEMS(second);i++)
  {
    if (second->m[i]!=NULL)
    {
      if (syz_ring==orig_ring)
        temp->m[k] = pCopy(second->m[i]);
      else
        temp->m[k] = prCopyR(second->m[i], orig_ring,currRing);
      if (slength==0) p_Shift(&(temp->m[k]),1,currRing);
      k++;
    }
  }
  intvec *w=NULL;
  temp1 = kStd(temp,currQuotient,testHomog,&w,NULL,length);
  if (w!=NULL) delete w;
  idDelete(&temp);
  if(syz_ring!=orig_ring)
    rChangeCurrRing(orig_ring);

  result = idInit(IDELEMS(temp1),rank);
  j = 0;
  for (i=0;i<IDELEMS(temp1);i++)
  {
    if ((temp1->m[i]!=NULL)
    && (p_GetComp(temp1->m[i],syz_ring)>length))
    {
      if(syz_ring==orig_ring)
      {
        p = temp1->m[i];
      }
      else
      {
        p = prMoveR(temp1->m[i], syz_ring,orig_ring);
      }
      temp1->m[i]=NULL;
      while (p!=NULL)
      {
        q = pNext(p);
        pNext(p) = NULL;
        k = pGetComp(p)-1-length;
        pSetComp(p,0);
        pSetmComp(p);
        /* Warning! multiply only from the left! it's very important for Plural */
        result->m[j] = pAdd(result->m[j],pMult(p,pCopy(first->m[k])));
        p = q;
      }
      j++;
    }
  }
  if(syz_ring!=orig_ring)
  {
    rChangeCurrRing(syz_ring);
    idDelete(&temp1);
    rChangeCurrRing(orig_ring);
    rKill(syz_ring);
  }
  else
  {
    idDelete(&temp1);
  }

  idSkipZeroes(result);
  if (TEST_OPT_RETURN_SB)
  {
     w=NULL;
     temp1=kStd(result,currQuotient,testHomog,&w);
     if (w!=NULL) delete w;
     idDelete(&result);
     idSkipZeroes(temp1);
     return temp1;
  }
  else //temp1=kInterRed(result,currQuotient);
    return result;
}

/*2
* ideal/module intersection for a list of objects
* given as 'resolvente'
*/
ideal idMultSect(resolvente arg, int length)
{
  int i,j=0,k=0,syzComp,l,maxrk=-1,realrki;
  ideal bigmat,tempstd,result;
  poly p;
  int isIdeal=0;
  intvec * w=NULL;

  /* find 0-ideals and max rank -----------------------------------*/
  for (i=0;i<length;i++)
  {
    if (!idIs0(arg[i]))
    {
      realrki=id_RankFreeModule(arg[i],currRing);
      k++;
      j += IDELEMS(arg[i]);
      if (realrki>maxrk) maxrk = realrki;
    }
    else
    {
      if (arg[i]!=NULL)
      {
        return idInit(1,arg[i]->rank);
      }
    }
  }
  if (maxrk == 0)
  {
    isIdeal = 1;
    maxrk = 1;
  }
  /* init -----------------------------------------------------------*/
  j += maxrk;
  syzComp = k*maxrk;

  ring orig_ring=currRing;
  ring syz_ring=rAssure_SyzComp(orig_ring,TRUE);
  rChangeCurrRing(syz_ring);
  rSetSyzComp(syzComp, syz_ring);

  bigmat = idInit(j,(k+1)*maxrk);
  /* create unit matrices ------------------------------------------*/
  for (i=0;i<maxrk;i++)
  {
    for (j=0;j<=k;j++)
    {
      p = pOne();
      pSetComp(p,i+1+j*maxrk);
      pSetmComp(p);
      bigmat->m[i] = pAdd(bigmat->m[i],p);
    }
  }
  /* enter given ideals ------------------------------------------*/
  i = maxrk;
  k = 0;
  for (j=0;j<length;j++)
  {
    if (arg[j]!=NULL)
    {
      for (l=0;l<IDELEMS(arg[j]);l++)
      {
        if (arg[j]->m[l]!=NULL)
        {
          if (syz_ring==orig_ring)
            bigmat->m[i] = pCopy(arg[j]->m[l]);
          else
            bigmat->m[i] = prCopyR(arg[j]->m[l], orig_ring,currRing);
          p_Shift(&(bigmat->m[i]),k*maxrk+isIdeal,currRing);
          i++;
        }
      }
      k++;
    }
  }
  /* std computation --------------------------------------------*/
  tempstd = kStd(bigmat,currQuotient,testHomog,&w,NULL,syzComp);
  if (w!=NULL) delete w;
  idDelete(&bigmat);

  if(syz_ring!=orig_ring)
    rChangeCurrRing(orig_ring);

  /* interprete result ----------------------------------------*/
  result = idInit(IDELEMS(tempstd),maxrk);
  k = 0;
  for (j=0;j<IDELEMS(tempstd);j++)
  {
    if ((tempstd->m[j]!=NULL) && (p_GetComp(tempstd->m[j],syz_ring)>syzComp))
    {
      if (syz_ring==orig_ring)
        p = pCopy(tempstd->m[j]);
      else
        p = prCopyR(tempstd->m[j], syz_ring,currRing);
      p_Shift(&p,-syzComp-isIdeal,currRing);
      result->m[k] = p;
      k++;
    }
  }
  /* clean up ----------------------------------------------------*/
  if(syz_ring!=orig_ring)
    rChangeCurrRing(syz_ring);
  idDelete(&tempstd);
  if(syz_ring!=orig_ring)
  {
    rChangeCurrRing(orig_ring);
    rKill(syz_ring);
  }
  idSkipZeroes(result);
  return result;
}

/*2
*computes syzygies of h1,
*if quot != NULL it computes in the quotient ring modulo "quot"
*works always in a ring with ringorder_s
*/
static ideal idPrepare (ideal  h1, tHomog hom, int syzcomp, intvec **w)
{
  ideal   h2, h3;
  int     i;
  int     j,jj=0,k;
  poly    p,q;

  if (idIs0(h1)) return NULL;
  k = id_RankFreeModule(h1,currRing);
  h2=idCopy(h1);
  i = IDELEMS(h2)-1;
  if (k == 0)
  {
    for (j=0; j<=i; j++) p_Shift(&(h2->m[j]),1,currRing);
    k = 1;
  }
  if (syzcomp<k)
  {
    Warn("syzcomp too low, should be %d instead of %d",k,syzcomp);
    syzcomp = k;
    rSetSyzComp(k,currRing);
  }
  h2->rank = syzcomp+i+1;

  //if (hom==testHomog)
  //{
  //  if(idHomIdeal(h1,currQuotient))
  //  {
  //    hom=TRUE;
  //  }
  //}

#if MYTEST
#ifdef RDEBUG
  Print("Prepare::h2: ");
  idPrint(h2);

  for(j=0;j<IDELEMS(h2);j++) pTest(h2->m[j]);

#endif
#endif

  for (j=0; j<=i; j++)
  {
    p = h2->m[j];
    q = pOne();
    pSetComp(q,syzcomp+1+j);
    pSetmComp(q);
    if (p!=NULL)
    {
      while (pNext(p)) pIter(p);
      p->next = q;
    }
    else
      h2->m[j]=q;
  }

#ifdef PDEBUG
  for(j=0;j<IDELEMS(h2);j++) pTest(h2->m[j]);

#if MYTEST
#ifdef RDEBUG
  Print("Prepare::Input: ");
  idPrint(h2);

  Print("Prepare::currQuotient: ");
  idPrint(currQuotient);
#endif
#endif

#endif

  idTest(h2);

  h3 = kStd(h2,currQuotient,hom,w,NULL,syzcomp);

#if MYTEST
#ifdef RDEBUG
  Print("Prepare::Output: ");
  idPrint(h3);
  for(j=0;j<IDELEMS(h2);j++) pTest(h3->m[j]);
#endif
#endif


  idDelete(&h2);
  return h3;
}

/*2
* compute the syzygies of h1 in R/quot,
* weights of components are in w
* if setRegularity, return the regularity in deg
* do not change h1,  w
*/
ideal idSyzygies (ideal  h1, tHomog h,intvec **w, BOOLEAN setSyzComp,
                  BOOLEAN setRegularity, int *deg)
{
  ideal s_h1;
  poly  p;
  int   j, k, length=0,reg;
  BOOLEAN isMonomial=TRUE;
  int ii, idElemens_h1;

  assume(h1 != NULL);

  idElemens_h1=IDELEMS(h1);
#ifdef PDEBUG
  for(ii=0;ii<idElemens_h1 /*IDELEMS(h1)*/;ii++) pTest(h1->m[ii]);
#endif
  if (idIs0(h1))
  {
    ideal result=idFreeModule(idElemens_h1/*IDELEMS(h1)*/);
    int curr_syz_limit=rGetCurrSyzLimit(currRing);
    if (curr_syz_limit>0)
    for (ii=0;ii<idElemens_h1/*IDELEMS(h1)*/;ii++)
    {
      if (h1->m[ii]!=NULL)
        p_Shift(&h1->m[ii],curr_syz_limit,currRing);
    }
    return result;
  }
  int slength=(int)id_RankFreeModule(h1,currRing);
  k=si_max(1,slength /*id_RankFreeModule(h1)*/);

  assume(currRing != NULL);
  ring orig_ring=currRing;
  ring syz_ring=rAssure_SyzComp(orig_ring,TRUE);
  rChangeCurrRing(syz_ring);

  if (setSyzComp)
    rSetSyzComp(k,syz_ring);

  if (orig_ring != syz_ring)
  {
    s_h1=idrCopyR_NoSort(h1,orig_ring,syz_ring);
  }
  else
  {
    s_h1 = h1;
  }

  idTest(s_h1);

  ideal s_h3=idPrepare(s_h1,h,k,w); // main (syz) GB computation

  if (s_h3==NULL)
  {
    return idFreeModule( idElemens_h1 /*IDELEMS(h1)*/);
  }

  if (orig_ring != syz_ring)
  {
    idDelete(&s_h1);
    for (j=0; j<IDELEMS(s_h3); j++)
    {
      if (s_h3->m[j] != NULL)
      {
        if (p_MinComp(s_h3->m[j],syz_ring) > k)
          p_Shift(&s_h3->m[j], -k,currRing);
        else
          pDelete(&s_h3->m[j]);
      }
    }
    idSkipZeroes(s_h3);
    s_h3->rank -= k;
    rChangeCurrRing(orig_ring);
    s_h3 = idrMoveR_NoSort(s_h3, syz_ring, orig_ring);
    rKill(syz_ring);
    #ifdef HAVE_PLURAL
    if (rIsPluralRing(currRing))
    {
      idDelMultiples(s_h3);
      idSkipZeroes(s_h3);
    }
    #endif
    idTest(s_h3);
    return s_h3;
  }

  ideal e = idInit(IDELEMS(s_h3), s_h3->rank);

  for (j=IDELEMS(s_h3)-1; j>=0; j--)
  {
    if (s_h3->m[j] != NULL)
    {
      if (p_MinComp(s_h3->m[j],syz_ring) <= k)
      {
        e->m[j] = s_h3->m[j];
        isMonomial=isMonomial && (pNext(s_h3->m[j])==NULL);
        pDelete(&pNext(s_h3->m[j]));
        s_h3->m[j] = NULL;
      }
    }
  }

  idSkipZeroes(s_h3);
  idSkipZeroes(e);

  if ((deg != NULL)
  && (!isMonomial)
  && (!TEST_OPT_NOTREGULARITY)
  && (setRegularity)
  && (h==isHomog)
  && (!rIsPluralRing(currRing))
  )
  {
    ring dp_C_ring = rAssure_dp_C(syz_ring);
    if (dp_C_ring != syz_ring)
    {
      rChangeCurrRing(dp_C_ring);
      e = idrMoveR_NoSort(e, syz_ring, dp_C_ring);
    }
    resolvente res = sySchreyerResolvente(e,-1,&length,TRUE, TRUE);
    intvec * dummy = syBetti(res,length,&reg, *w);
    *deg = reg+2;
    delete dummy;
    for (j=0;j<length;j++)
    {
      if (res[j]!=NULL) idDelete(&(res[j]));
    }
    omFreeSize((ADDRESS)res,length*sizeof(ideal));
    idDelete(&e);
    if (dp_C_ring != syz_ring)
    {
      rChangeCurrRing(syz_ring);
      rKill(dp_C_ring);
    }
  }
  else
  {
    idDelete(&e);
  }
  idTest(s_h3);
  if (currQuotient != NULL)
  {
    ideal ts_h3=kStd(s_h3,currQuotient,h,w);
    idDelete(&s_h3);
    s_h3 = ts_h3;
  }
  return s_h3;
}

/*2
*/
ideal idXXX (ideal  h1, int k)
{
  ideal s_h1;
  int j;
  intvec *w=NULL;

  assume(currRing != NULL);
  ring orig_ring=currRing;
  ring syz_ring=rAssure_SyzComp(orig_ring,TRUE);
  rChangeCurrRing(syz_ring);

  rSetSyzComp(k,syz_ring);

  if (orig_ring != syz_ring)
  {
    s_h1=idrCopyR_NoSort(h1,orig_ring, syz_ring);
  }
  else
  {
    s_h1 = h1;
  }

  ideal s_h3=kStd(s_h1,NULL,testHomog,&w,NULL,k);

  if (s_h3==NULL)
  {
    return idFreeModule(IDELEMS(h1));
  }

  if (orig_ring != syz_ring)
  {
    idDelete(&s_h1);
    idSkipZeroes(s_h3);
    rChangeCurrRing(orig_ring);
    s_h3 = idrMoveR_NoSort(s_h3, syz_ring, orig_ring);
    rKill(syz_ring);
    idTest(s_h3);
    return s_h3;
  }

  idSkipZeroes(s_h3);
  idTest(s_h3);
  return s_h3;
}

/*
*computes a standard basis for h1 and stores the transformation matrix
* in ma
*/
ideal idLiftStd (ideal  h1, matrix* ma, tHomog hi, ideal * syz)
{
  int   i, j, k, t, inputIsIdeal=id_RankFreeModule(h1,currRing);
  poly  p=NULL, q, qq;
  intvec *w=NULL;

  idDelete((ideal*)ma);
  BOOLEAN lift3=FALSE;
  if (syz!=NULL) { lift3=TRUE; idDelete(syz); }
  if (idIs0(h1))
  {
    *ma=mpNew(1,0);
    if (lift3)
    {
      *syz=idFreeModule(IDELEMS(h1));
      int curr_syz_limit=rGetCurrSyzLimit(currRing);
      if (curr_syz_limit>0)
      for (int ii=0;ii<IDELEMS(h1);ii++)
      {
        if (h1->m[ii]!=NULL)
          p_Shift(&h1->m[ii],curr_syz_limit,currRing);
      }
    }
    return idInit(1,h1->rank);
  }

  BITSET save_verbose=verbose;

  k=si_max(1,(int)id_RankFreeModule(h1,currRing));

  if ((k==1) && (!lift3)) verbose |=Sy_bit(V_IDLIFT);

  ring orig_ring = currRing;
  ring syz_ring = rAssure_SyzComp(orig_ring,TRUE);
  rChangeCurrRing(syz_ring);
  rSetSyzComp(k,syz_ring);

  ideal s_h1=h1;

  if (orig_ring != syz_ring)
    s_h1 = idrCopyR_NoSort(h1,orig_ring,syz_ring);
  else
    s_h1 = h1;

  ideal s_h3=idPrepare(s_h1,hi,k,&w); // main (syz) GB computation

  ideal s_h2 = idInit(IDELEMS(s_h3), s_h3->rank);

  if (lift3) (*syz)=idInit(IDELEMS(s_h3),IDELEMS(h1));

  if (w!=NULL) delete w;
  i = 0;

  // now sort the result, SB : leave in s_h3
  //                      T:  put in s_h2
  //                      syz: put in *syz
  for (j=0; j<IDELEMS(s_h3); j++)
  {
    if (s_h3->m[j] != NULL)
    {
      //if (p_MinComp(s_h3->m[j],syz_ring) <= k)
      if (pGetComp(s_h3->m[j]) <= k) // syz_ring == currRing
      {
        i++;
        q = s_h3->m[j];
        while (pNext(q) != NULL)
        {
          if (pGetComp(pNext(q)) > k)
          {
            s_h2->m[j] = pNext(q);
            pNext(q) = NULL;
          }
          else
          {
            pIter(q);
          }
        }
        if (!inputIsIdeal) p_Shift(&(s_h3->m[j]), -1,currRing);
      }
      else
      {
        // we a syzygy here:
        if (lift3)
        {
          p_Shift(&s_h3->m[j], -k,currRing);
          (*syz)->m[j]=s_h3->m[j];
          s_h3->m[j]=NULL;
        }
        else
          pDelete(&(s_h3->m[j]));
      }
    }
  }
  idSkipZeroes(s_h3);
  //extern char * iiStringMatrix(matrix im, int dim,char ch);
  //PrintS("SB: ----------------------------------------\n");
  //PrintS(iiStringMatrix((matrix)s_h3,k,'\n'));
  //PrintLn();
  //PrintS("T: ----------------------------------------\n");
  //PrintS(iiStringMatrix((matrix)s_h2,h1->rank,'\n'));
  //PrintLn();

  if (lift3) idSkipZeroes(*syz);

  j = IDELEMS(s_h1);


  if (syz_ring!=orig_ring)
  {
    idDelete(&s_h1);
    rChangeCurrRing(orig_ring);
  }

  *ma = mpNew(j,i);

  i = 1;
  for (j=0; j<IDELEMS(s_h2); j++)
  {
    if (s_h2->m[j] != NULL)
    {
      q = prMoveR( s_h2->m[j], syz_ring,orig_ring);
      s_h2->m[j] = NULL;

      while (q != NULL)
      {
        p = q;
        pIter(q);
        pNext(p) = NULL;
        t=pGetComp(p);
        pSetComp(p,0);
        pSetmComp(p);
        MATELEM(*ma,t-k,i) = pAdd(MATELEM(*ma,t-k,i),p);
      }
      i++;
    }
  }
  idDelete(&s_h2);

  for (i=0; i<IDELEMS(s_h3); i++)
  {
    s_h3->m[i] = prMoveR_NoSort(s_h3->m[i], syz_ring,orig_ring);
  }
  if (lift3)
  {
    for (i=0; i<IDELEMS(*syz); i++)
    {
      (*syz)->m[i] = prMoveR_NoSort((*syz)->m[i], syz_ring,orig_ring);
    }
  }

  if (syz_ring!=orig_ring) rKill(syz_ring);
  verbose = save_verbose;
  return s_h3;
}

static void idPrepareStd(ideal s_temp, int k)
{
  int j,rk=id_RankFreeModule(s_temp,currRing);
  poly p,q;

  if (rk == 0)
  {
    for (j=0; j<IDELEMS(s_temp); j++)
    {
      if (s_temp->m[j]!=NULL) pSetCompP(s_temp->m[j],1);
    }
    k = si_max(k,1);
  }
  for (j=0; j<IDELEMS(s_temp); j++)
  {
    if (s_temp->m[j]!=NULL)
    {
      p = s_temp->m[j];
      q = pOne();
      //pGetCoeff(q)=nNeg(pGetCoeff(q));   //set q to -1
      pSetComp(q,k+1+j);
      pSetmComp(q);
      while (pNext(p)) pIter(p);
      pNext(p) = q;
    }
  }
}

/*2
*computes a representation of the generators of submod with respect to those
* of mod
*/

ideal idLift(ideal mod, ideal submod,ideal *rest, BOOLEAN goodShape,
             BOOLEAN isSB, BOOLEAN divide, matrix *unit)
{
  int lsmod =id_RankFreeModule(submod,currRing), i, j, k;
  int comps_to_add=0;
  poly p;

  if (idIs0(submod))
  {
    if (unit!=NULL)
    {
      *unit=mpNew(1,1);
      MATELEM(*unit,1,1)=pOne();
    }
    if (rest!=NULL)
    {
      *rest=idInit(1,mod->rank);
    }
    return idInit(1,mod->rank);
  }
  if (idIs0(mod)) /* and not idIs0(submod) */
  {
    WerrorS("2nd module does not lie in the first");
    #if 0
    if (unit!=NULL)
    {
      i=IDELEMS(submod);
      *unit=mpNew(i,i);
      for (j=i;j>0;j--)
      {
        MATELEM(*unit,j,j)=pOne();
      }
    }
    if (rest!=NULL)
    {
      *rest=idCopy(submod);
    }
    return idInit(1,mod->rank);
    #endif
    return idInit(IDELEMS(submod),submod->rank);
  }
  if (unit!=NULL)
  {
    comps_to_add = IDELEMS(submod);
    while ((comps_to_add>0) && (submod->m[comps_to_add-1]==NULL))
      comps_to_add--;
  }
  k=si_max(id_RankFreeModule(mod,currRing),id_RankFreeModule(submod,currRing));
  if  ((k!=0) && (lsmod==0)) lsmod=1;
  k=si_max(k,(int)mod->rank);
  if (k<submod->rank) { WarnS("rk(submod) > rk(mod) ?");k=submod->rank; }

  ring orig_ring=currRing;
  ring syz_ring=rAssure_SyzComp(orig_ring,TRUE);
  rChangeCurrRing(syz_ring);
  rSetSyzComp(k,syz_ring);

  ideal s_mod, s_temp;
  if (orig_ring != syz_ring)
  {
    s_mod = idrCopyR_NoSort(mod,orig_ring,syz_ring);
    s_temp = idrCopyR_NoSort(submod,orig_ring,syz_ring);
  }
  else
  {
    s_mod = mod;
    s_temp = idCopy(submod);
  }
  ideal s_h3;
  if (isSB)
  {
    s_h3 = idCopy(s_mod);
    idPrepareStd(s_h3, k+comps_to_add);
  }
  else
  {
    s_h3 = idPrepare(s_mod,(tHomog)FALSE,k+comps_to_add,NULL);
  }
  if (!goodShape)
  {
    for (j=0;j<IDELEMS(s_h3);j++)
    {
      if ((s_h3->m[j] != NULL) && (pMinComp(s_h3->m[j]) > k))
        pDelete(&(s_h3->m[j]));
    }
  }
  idSkipZeroes(s_h3);
  if (lsmod==0)
  {
    for (j=IDELEMS(s_temp);j>0;j--)
    {
      if (s_temp->m[j-1]!=NULL)
        p_Shift(&(s_temp->m[j-1]),1,currRing);
    }
  }
  if (unit!=NULL)
  {
    for(j = 0;j<comps_to_add;j++)
    {
      p = s_temp->m[j];
      if (p!=NULL)
      {
        while (pNext(p)!=NULL) pIter(p);
        pNext(p) = pOne();
        pIter(p);
        pSetComp(p,1+j+k);
        pSetmComp(p);
        p = pNeg(p);
      }
    }
  }
  ideal s_result = kNF(s_h3,currQuotient,s_temp,k);
  s_result->rank = s_h3->rank;
  ideal s_rest = idInit(IDELEMS(s_result),k);
  idDelete(&s_h3);
  idDelete(&s_temp);

  for (j=0;j<IDELEMS(s_result);j++)
  {
    if (s_result->m[j]!=NULL)
    {
      if (pGetComp(s_result->m[j])<=k)
      {
        if (!divide)
        {
          if (isSB)
          {
            WarnS("first module not a standardbasis\n"
              "// ** or second not a proper submodule");
          }
          else
            WerrorS("2nd module does not lie in the first");
          idDelete(&s_result);
          idDelete(&s_rest);
          s_result=idInit(IDELEMS(submod),submod->rank);
          break;
        }
        else
        {
          p = s_rest->m[j] = s_result->m[j];
          while ((pNext(p)!=NULL) && (pGetComp(pNext(p))<=k)) pIter(p);
          s_result->m[j] = pNext(p);
          pNext(p) = NULL;
        }
      }
      p_Shift(&(s_result->m[j]),-k,currRing);
      pNeg(s_result->m[j]);
    }
  }
  if ((lsmod==0) && (!idIs0(s_rest)))
  {
    for (j=IDELEMS(s_rest);j>0;j--)
    {
      if (s_rest->m[j-1]!=NULL)
      {
        p_Shift(&(s_rest->m[j-1]),-1,currRing);
        s_rest->m[j-1] = s_rest->m[j-1];
      }
    }
  }
  if(syz_ring!=orig_ring)
  {
    idDelete(&s_mod);
    rChangeCurrRing(orig_ring);
    s_result = idrMoveR_NoSort(s_result, syz_ring, orig_ring);
    s_rest = idrMoveR_NoSort(s_rest, syz_ring, orig_ring);
    rKill(syz_ring);
  }
  if (rest!=NULL)
    *rest = s_rest;
  else
    idDelete(&s_rest);
//idPrint(s_result);
  if (unit!=NULL)
  {
    *unit=mpNew(comps_to_add,comps_to_add);
    int i;
    for(i=0;i<IDELEMS(s_result);i++)
    {
      poly p=s_result->m[i];
      poly q=NULL;
      while(p!=NULL)
      {
        if(pGetComp(p)<=comps_to_add)
        {
          pSetComp(p,0);
          if (q!=NULL)
          {
            pNext(q)=pNext(p);
          }
          else
          {
            pIter(s_result->m[i]);
          }
          pNext(p)=NULL;
          MATELEM(*unit,i+1,i+1)=pAdd(MATELEM(*unit,i+1,i+1),p);
          if(q!=NULL)   p=pNext(q);
          else          p=s_result->m[i];
        }
        else
        {
          q=p;
          pIter(p);
        }
      }
      p_Shift(&s_result->m[i],-comps_to_add,currRing);
    }
  }
  return s_result;
}

/*2
*computes division of P by Q with remainder up to (w-weighted) degree n
*P, Q, and w are not changed
*/
void idLiftW(ideal P,ideal Q,int n,matrix &T, ideal &R,short *w)
{
  long N=0;
  int i;
  for(i=IDELEMS(Q)-1;i>=0;i--)
    if(w==NULL)
      N=si_max(N,pDeg(Q->m[i]));
    else
      N=si_max(N,pDegW(Q->m[i],w));
  N+=n;

  T=mpNew(IDELEMS(Q),IDELEMS(P));
  R=idInit(IDELEMS(P),P->rank);

  for(i=IDELEMS(P)-1;i>=0;i--)
  {
    poly p;
    if(w==NULL)
      p=ppJet(P->m[i],N);
    else
      p=ppJetW(P->m[i],N,w);

    int j=IDELEMS(Q)-1;
    while(p!=NULL)
    {
      if(pDivisibleBy(Q->m[j],p))
      {
        poly p0=p_DivideM(pHead(p),pHead(Q->m[j]),currRing);
        if(w==NULL)
          p=pJet(pSub(p,ppMult_mm(Q->m[j],p0)),N);
        else
          p=pJetW(pSub(p,ppMult_mm(Q->m[j],p0)),N,w);
        pNormalize(p);
        if((w==NULL)&&(pDeg(p0)>n)||(w!=NULL)&&(pDegW(p0,w)>n))
          pDelete(&p0);
        else
          MATELEM(T,j+1,i+1)=pAdd(MATELEM(T,j+1,i+1),p0);
        j=IDELEMS(Q)-1;
      }
      else
      {
        if(j==0)
        {
          poly p0=p;
          pIter(p);
          pNext(p0)=NULL;
          if(((w==NULL)&&(pDeg(p0)>n))
          ||((w!=NULL)&&(pDegW(p0,w)>n)))
            pDelete(&p0);
          else
            R->m[i]=pAdd(R->m[i],p0);
          j=IDELEMS(Q)-1;
        }
        else
          j--;
      }
    }
  }
}

/*2
*computes the quotient of h1,h2 : internal routine for idQuot
*BEWARE: the returned ideals may contain incorrectly ordered polys !
*
*/
static ideal idInitializeQuot (ideal  h1, ideal h2, BOOLEAN h1IsStb,
                               BOOLEAN *addOnlyOne, int *kkmax)
{
  ideal temph1;
  poly     p,q = NULL;
  int i,l,ll,k,kkk,kmax;
  int j = 0;
  int k1 = id_RankFreeModule(h1,currRing);
  int k2 = id_RankFreeModule(h2,currRing);
  tHomog   hom=isNotHomog;

  k=si_max(k1,k2);
  if (k==0)
    k = 1;
  if ((k2==0) && (k>1)) *addOnlyOne = FALSE;

  intvec * weights;
  hom = (tHomog)idHomModule(h1,currQuotient,&weights);
  if (/**addOnlyOne &&*/ (!h1IsStb))
    temph1 = kStd(h1,currQuotient,hom,&weights,NULL);
  else
    temph1 = idCopy(h1);
  if (weights!=NULL) delete weights;
  idTest(temph1);
/*--- making a single vector from h2 ---------------------*/
  for (i=0; i<IDELEMS(h2); i++)
  {
    if (h2->m[i] != NULL)
    {
      p = pCopy(h2->m[i]);
      if (k2 == 0)
        p_Shift(&p,j*k+1,currRing);
      else
        p_Shift(&p,j*k,currRing);
      q = pAdd(q,p);
      j++;
    }
  }
  *kkmax = kmax = j*k+1;
/*--- adding a monomial for the result (syzygy) ----------*/
  p = q;
  while (pNext(p)!=NULL) pIter(p);
  pNext(p) = pOne();
  pIter(p);
  pSetComp(p,kmax);
  pSetmComp(p);
/*--- constructing the big matrix ------------------------*/
  ideal h4 = idInit(16,kmax+k-1);
  h4->m[0] = q;
  if (k2 == 0)
  {
    if (k > IDELEMS(h4))
    {
      pEnlargeSet(&(h4->m),IDELEMS(h4),k-IDELEMS(h4));
      IDELEMS(h4) = k;
    }
    for (i=1; i<k; i++)
    {
      if (h4->m[i-1]!=NULL)
      {
        p = pCopy_noCheck(h4->m[i-1]);
        p_Shift(&p,1,currRing);
        h4->m[i] = p;
      }
    }
  }
  idSkipZeroes(h4);
  kkk = IDELEMS(h4);
  i = IDELEMS(temph1);
  for (l=0; l<i; l++)
  {
    if(temph1->m[l]!=NULL)
    {
      for (ll=0; ll<j; ll++)
      {
        p = pCopy(temph1->m[l]);
        if (k1 == 0)
          p_Shift(&p,ll*k+1,currRing);
        else
          p_Shift(&p,ll*k,currRing);
        if (kkk >= IDELEMS(h4))
        {
          pEnlargeSet(&(h4->m),IDELEMS(h4),16);
          IDELEMS(h4) += 16;
        }
        h4->m[kkk] = p;
        kkk++;
      }
    }
  }
/*--- if h2 goes in as single vector - the h1-part is just SB ---*/
  if (*addOnlyOne)
  {
    idSkipZeroes(h4);
    p = h4->m[0];
    for (i=0;i<IDELEMS(h4)-1;i++)
    {
      h4->m[i] = h4->m[i+1];
    }
    h4->m[IDELEMS(h4)-1] = p;
    test |= Sy_bit(OPT_SB_1);
  }
  idDelete(&temph1);
  return h4;
}
/*2
*computes the quotient of h1,h2
*/
ideal idQuot (ideal  h1, ideal h2, BOOLEAN h1IsStb, BOOLEAN resultIsIdeal)
{
  // first check for special case h1:(0)
  if (idIs0(h2))
  {
    ideal res;
    if (resultIsIdeal)
    {
      res = idInit(1,1);
      res->m[0] = pOne();
    }
    else
      res = idFreeModule(h1->rank);
    return res;
  }
  BITSET old_test=test;
  int i,l,ll,k,kkk,kmax;
  BOOLEAN  addOnlyOne=TRUE;
  tHomog   hom=isNotHomog;
  intvec * weights1;

  ideal s_h4 = idInitializeQuot (h1,h2,h1IsStb,&addOnlyOne,&kmax);

  hom = (tHomog)idHomModule(s_h4,currQuotient,&weights1);

  ring orig_ring=currRing;
  ring syz_ring=rAssure_SyzComp(orig_ring,TRUE);
  rChangeCurrRing(syz_ring);
  rSetSyzComp(kmax-1,syz_ring);
  if (orig_ring!=syz_ring)
  //  s_h4 = idrMoveR_NoSort(s_h4,orig_ring, syz_ring);
    s_h4 = idrMoveR(s_h4,orig_ring, syz_ring);
  idTest(s_h4);
  #if 0
  void ipPrint_MA0(matrix m, const char *name);
  matrix m=idModule2Matrix(idCopy(s_h4));
  PrintS("start:\n");
  ipPrint_MA0(m,"Q");
  idDelete((ideal *)&m);
  PrintS("last elem:");wrp(s_h4->m[IDELEMS(s_h4)-1]);PrintLn();
  #endif
  ideal s_h3;
  if (addOnlyOne)
  {
    s_h3 = kStd(s_h4,currQuotient,hom,&weights1,NULL,0/*kmax-1*/,IDELEMS(s_h4)-1);
  }
  else
  {
    s_h3 = kStd(s_h4,currQuotient,hom,&weights1,NULL,kmax-1);
  }
  test = old_test;
  #if 0
  // only together with the above debug stuff
  idSkipZeroes(s_h3);
  m=idModule2Matrix(idCopy(s_h3));
  Print("result, kmax=%d:\n",kmax);
  ipPrint_MA0(m,"S");
  idDelete((ideal *)&m);
  #endif
  idTest(s_h3);
  if (weights1!=NULL) delete weights1;
  idDelete(&s_h4);

  for (i=0;i<IDELEMS(s_h3);i++)
  {
    if ((s_h3->m[i]!=NULL) && (pGetComp(s_h3->m[i])>=kmax))
    {
      if (resultIsIdeal)
        p_Shift(&s_h3->m[i],-kmax,currRing);
      else
        p_Shift(&s_h3->m[i],-kmax+1,currRing);
    }
    else
      pDelete(&s_h3->m[i]);
  }
  if (resultIsIdeal)
    s_h3->rank = 1;
  else
    s_h3->rank = h1->rank;
  if(syz_ring!=orig_ring)
  {
    rChangeCurrRing(orig_ring);
    s_h3 = idrMoveR_NoSort(s_h3, syz_ring, orig_ring);
    rKill(syz_ring);
  }
  idSkipZeroes(s_h3);
  idTest(s_h3);
  return s_h3;
}

/*2
* eliminate delVar (product of vars) in h1
*/
ideal idElimination (ideal h1,poly delVar,intvec *hilb)
{
  int    i,j=0,k,l;
  ideal  h,hh, h3;
  int    *ord,*block0,*block1;
  int    ordersize=2;
  int    **wv;
  tHomog hom;
  intvec * w;
  ring tmpR;
  ring origR = currRing;

  if (delVar==NULL)
  {
    return idCopy(h1);
  }
  if ((currQuotient!=NULL) && rIsPluralRing(origR))
  {
    WerrorS("cannot eliminate in a qring");
    return idCopy(h1);
  }
  if (idIs0(h1)) return idInit(1,h1->rank);
#ifdef HAVE_PLURAL
  if (rIsPluralRing(origR))
    /* in the NC case, we have to check the admissibility of */
    /* the subalgebra to be intersected with */
  {
    if ((ncRingType(origR) != nc_skew) && (ncRingType(origR) != nc_exterior)) /* in (quasi)-commutative algebras every subalgebra is admissible */
    {
      if (nc_CheckSubalgebra(delVar,origR))
      {
        WerrorS("no elimination is possible: subalgebra is not admissible");
        return idCopy(h1);
      }
    }
  }
#endif
  hom=(tHomog)idHomModule(h1,NULL,&w); //sets w to weight vector or NULL
  h3=idInit(16,h1->rank);
  for (k=0;; k++)
  {
    if (origR->order[k]!=0) ordersize++;
    else break;
  }
#if 0
  if (rIsPluralRing(origR)) // we have too keep the odering: it may be needed
                            // for G-algebra
  {
    for (k=0;k<ordersize-1; k++)
    {
      block0[k+1] = origR->block0[k];
      block1[k+1] = origR->block1[k];
      ord[k+1] = origR->order[k];
      if (origR->wvhdl[k]!=NULL) wv[k+1] = (int*) omMemDup(origR->wvhdl[k]);
    }
  }
  else
  {
    block0[1] = 1;
    block1[1] = (currRing->N);
    if (origR->OrdSgn==1) ord[1] = ringorder_wp;
    else                  ord[1] = ringorder_ws;
    wv[1]=(int*)omAlloc0((currRing->N)*sizeof(int));
    double wNsqr = (double)2.0 / (double)(currRing->N);
    wFunctional = wFunctionalBuch;
    int  *x= (int * )omAlloc(2 * ((currRing->N) + 1) * sizeof(int));
    int sl=IDELEMS(h1) - 1;
    wCall(h1->m, sl, x, wNsqr);
    for (sl = (currRing->N); sl!=0; sl--)
      wv[1][sl-1] = x[sl + (currRing->N) + 1];
    omFreeSize((ADDRESS)x, 2 * ((currRing->N) + 1) * sizeof(int));

    ord[2]=ringorder_C;
    ord[3]=0;
  }
#else
#endif
  if ((hom==TRUE) && (origR->OrdSgn==1) && (!rIsPluralRing(origR)))
  {
    #if 1
    // we change to an ordering:
    // aa(1,1,1,...,0,0,0),wp(...),C
    // this seems to be better than version 2 below,
    // according to Tst/../elimiate_[3568].tat (- 17 %)
    ord=(int*)omAlloc0(4*sizeof(int));
    block0=(int*)omAlloc0(4*sizeof(int));
    block1=(int*)omAlloc0(4*sizeof(int));
    wv=(int**) omAlloc0(4*sizeof(int**));
    block0[0] = block0[1] = 1;
    block1[0] = block1[1] = rVar(origR);
    wv[0]=(int*)omAlloc0((rVar(origR) + 1)*sizeof(int));
    // use this special ordering: like ringorder_a, except that pFDeg, pWeights
    // ignore it
    ord[0] = ringorder_aa;
    for (j=0;j<rVar(origR);j++)
      if (pGetExp(delVar,j+1)!=0) wv[0][j]=1;
    BOOLEAN wp=FALSE;
    for (j=0;j<rVar(origR);j++)
      if (pWeight(j+1,origR)!=1) { wp=TRUE;break; }
    if (wp)
    {
      wv[1]=(int*)omAlloc0((rVar(origR) + 1)*sizeof(int));
      for (j=0;j<rVar(origR);j++)
        wv[1][j]=pWeight(j+1,origR);
      ord[1] = ringorder_wp;
    }
    else
      ord[1] = ringorder_dp;
    #else
    // we change to an ordering:
    // a(w1,...wn),wp(1,...0.....),C
    ord=(int*)omAlloc0(4*sizeof(int));
    block0=(int*)omAlloc0(4*sizeof(int));
    block1=(int*)omAlloc0(4*sizeof(int));
    wv=(int**) omAlloc0(4*sizeof(int**));
    block0[0] = block0[1] = 1;
    block1[0] = block1[1] = rVar(origR);
    wv[0]=(int*)omAlloc0((rVar(origR) + 1)*sizeof(int));
    wv[1]=(int*)omAlloc0((rVar(origR) + 1)*sizeof(int));
    ord[0] = ringorder_a;
    for (j=0;j<rVar(origR);j++)
      wv[0][j]=pWeight(j+1,origR);
    ord[1] = ringorder_wp;
    for (j=0;j<rVar(origR);j++)
      if (pGetExp(delVar,j+1)!=0) wv[1][j]=1;
    #endif
    ord[2] = ringorder_C;
    ord[3] = 0;
  }
  else
  {
    // we change to an ordering:
    // aa(....),orig_ordering
    ord=(int*)omAlloc0(ordersize*sizeof(int));
    block0=(int*)omAlloc0(ordersize*sizeof(int));
    block1=(int*)omAlloc0(ordersize*sizeof(int));
    wv=(int**) omAlloc0(ordersize*sizeof(int**));
    for (k=0;k<ordersize-1; k++)
    {
      block0[k+1] = origR->block0[k];
      block1[k+1] = origR->block1[k];
      ord[k+1] = origR->order[k];
      if (origR->wvhdl[k]!=NULL) wv[k+1] = (int*) omMemDup(origR->wvhdl[k]);
    }
    block0[0] = 1;
    block1[0] = rVar(origR);
    wv[0]=(int*)omAlloc0((rVar(origR) + 1)*sizeof(int));
    for (j=0;j<rVar(origR);j++)
      if (pGetExp(delVar,j+1)!=0) wv[0][j]=1;
    // use this special ordering: like ringorder_a, except that pFDeg, pWeights
    // ignore it
    ord[0] = ringorder_aa;
  }
  // fill in tmp ring to get back the data later on
  tmpR  = rCopy0(origR,FALSE,FALSE); // qring==NULL
  //rUnComplete(tmpR);
  tmpR->p_Procs=NULL;
  tmpR->order = ord;
  tmpR->block0 = block0;
  tmpR->block1 = block1;
  tmpR->wvhdl = wv;
  rComplete(tmpR, 1);

#ifdef HAVE_PLURAL
  /* update nc structure on tmpR */
  if (rIsPluralRing(origR))
  {
    if ( nc_rComplete(origR, tmpR, false) ) // no quotient ideal!
    {
      Werror("no elimination is possible: ordering condition is violated");
      // cleanup
      rDelete(tmpR);
      if (w!=NULL)
        delete w;
      return idCopy(h1);
    }
  }
#endif
  // change into the new ring
  //pChangeRing((currRing->N),currRing->OrdSgn,ord,block0,block1,wv);
  rChangeCurrRing(tmpR);

  //h = idInit(IDELEMS(h1),h1->rank);
  // fetch data from the old ring
  //for (k=0;k<IDELEMS(h1);k++) h->m[k] = prCopyR( h1->m[k], origR);
  h=idrCopyR(h1,origR,currRing);
  if (origR->qideal!=NULL)
  {
    WarnS("eliminate in q-ring: experimental");
    ideal q=idrCopyR(origR->qideal,origR,currRing);
    ideal s=idSimpleAdd(h,q);
    idDelete(&h);
    idDelete(&q);
    h=s;
  }
  // compute kStd
#if 1
  //rWrite(tmpR);PrintLn();
  BITSET save=test;
  //test |=1;
  //Print("h: %d gen, rk=%d\n",IDELEMS(h),h->rank);
  //extern char * showOption();
  //Print("%s\n",showOption());
  hh = kStd(h,NULL,hom,&w,hilb);
  test=save;
  idDelete(&h);
#else
  extern ideal kGroebner(ideal F, ideal Q);
  hh=kGroebner(h,NULL);
#endif
  // go back to the original ring
  rChangeCurrRing(origR);
  i = IDELEMS(hh)-1;
  while ((i >= 0) && (hh->m[i] == NULL)) i--;
  j = -1;
  // fetch data from temp ring
  for (k=0; k<=i; k++)
  {
    l=(currRing->N);
    while ((l>0) && (p_GetExp( hh->m[k],l,tmpR)*pGetExp(delVar,l)==0)) l--;
    if (l==0)
    {
      j++;
      if (j >= IDELEMS(h3))
      {
        pEnlargeSet(&(h3->m),IDELEMS(h3),16);
        IDELEMS(h3) += 16;
      }
      h3->m[j] = prMoveR( hh->m[k], tmpR,origR);
      hh->m[k] = NULL;
    }
  }
  id_Delete(&hh, tmpR);
  idSkipZeroes(h3);
  rDelete(tmpR);
  if (w!=NULL)
    delete w;
  return h3;
}

/*2
* compute the which-th ar-minor of the matrix a
*/
poly idMinor(matrix a, int ar, unsigned long which, ideal R)
{
  int     i,j,k,size;
  unsigned long curr;
  int *rowchoise,*colchoise;
  BOOLEAN rowch,colch;
  ideal result;
  matrix tmp;
  poly p,q;

  i = binom(a->rows(),ar);
  j = binom(a->cols(),ar);

  rowchoise=(int *)omAlloc(ar*sizeof(int));
  colchoise=(int *)omAlloc(ar*sizeof(int));
  if ((i>512) || (j>512) || (i*j >512)) size=512;
  else size=i*j;
  result=idInit(size,1);
  tmp=mpNew(ar,ar);
  k = 0; /* the index in result*/
  curr = 0; /* index of current minor */
  idInitChoise(ar,1,a->rows(),&rowch,rowchoise);
  while (!rowch)
  {
    idInitChoise(ar,1,a->cols(),&colch,colchoise);
    while (!colch)
    {
      if (curr == which)
      {
        for (i=1; i<=ar; i++)
        {
          for (j=1; j<=ar; j++)
          {
            MATELEM(tmp,i,j) = MATELEM(a,rowchoise[i-1],colchoise[j-1]);
          }
        }
        p = mp_DetBareiss(tmp,currRing);
        if (p!=NULL)
        {
          if (R!=NULL)
          {
            q = p;
            p = kNF(R,currQuotient,q);
            pDelete(&q);
          }
          /*delete the matrix tmp*/
          for (i=1; i<=ar; i++)
          {
            for (j=1; j<=ar; j++) MATELEM(tmp,i,j) = NULL;
          }
          idDelete((ideal*)&tmp);
          omFreeSize((ADDRESS)rowchoise,ar*sizeof(int));
          omFreeSize((ADDRESS)colchoise,ar*sizeof(int));
          return (p);
        }
      }
      curr++;
      idGetNextChoise(ar,a->cols(),&colch,colchoise);
    }
    idGetNextChoise(ar,a->rows(),&rowch,rowchoise);
  }
  return (poly) 1;
}

#ifdef WITH_OLD_MINOR
/*2
* compute all ar-minors of the matrix a
*/
ideal idMinors(matrix a, int ar, ideal R)
{
  int     i,j,k,size;
  int *rowchoise,*colchoise;
  BOOLEAN rowch,colch;
  ideal result;
  matrix tmp;
  poly p,q;

  i = binom(a->rows(),ar);
  j = binom(a->cols(),ar);

  rowchoise=(int *)omAlloc(ar*sizeof(int));
  colchoise=(int *)omAlloc(ar*sizeof(int));
  if ((i>512) || (j>512) || (i*j >512)) size=512;
  else size=i*j;
  result=idInit(size,1);
  tmp=mpNew(ar,ar);
  k = 0; /* the index in result*/
  idInitChoise(ar,1,a->rows(),&rowch,rowchoise);
  while (!rowch)
  {
    idInitChoise(ar,1,a->cols(),&colch,colchoise);
    while (!colch)
    {
      for (i=1; i<=ar; i++)
      {
        for (j=1; j<=ar; j++)
        {
          MATELEM(tmp,i,j) = MATELEM(a,rowchoise[i-1],colchoise[j-1]);
        }
      }
      p = mp_DetBareiss(tmp,vcurrRing);
      if (p!=NULL)
      {
        if (R!=NULL)
        {
          q = p;
          p = kNF(R,currQuotient,q);
          pDelete(&q);
        }
        if (p!=NULL)
        {
          if (k>=size)
          {
            pEnlargeSet(&result->m,size,32);
            size += 32;
          }
          result->m[k] = p;
          k++;
        }
      }
      idGetNextChoise(ar,a->cols(),&colch,colchoise);
    }
    idGetNextChoise(ar,a->rows(),&rowch,rowchoise);
  }
  /*delete the matrix tmp*/
  for (i=1; i<=ar; i++)
  {
    for (j=1; j<=ar; j++) MATELEM(tmp,i,j) = NULL;
  }
  idDelete((ideal*)&tmp);
  if (k==0)
  {
    k=1;
    result->m[0]=NULL;
  }
  omFreeSize((ADDRESS)rowchoise,ar*sizeof(int));
  omFreeSize((ADDRESS)colchoise,ar*sizeof(int));
  pEnlargeSet(&result->m,size,k-size);
  IDELEMS(result) = k;
  return (result);
}
#else
/*2
* compute all ar-minors of the matrix a
* the caller of mpRecMin
* the elements of the result are not in R (if R!=NULL)
*/
ideal idMinors(matrix a, int ar, ideal R)
{
  int elems=0;
  int r=a->nrows,c=a->ncols;
  int i;
  matrix b;
  ideal result,h;
  ring origR;
  ring tmpR;
  long bound;

  if((ar<=0) || (ar>r) || (ar>c))
  {
    Werror("%d-th minor, matrix is %dx%d",ar,r,c);
    return NULL;
  }
  h = idMatrix2Module(mp_Copy(a,currRing));
  bound = sm_ExpBound(h,c,r,ar,currRing);
  idDelete(&h);
  tmpR=sm_RingChange(origR,bound);
  b = mpNew(r,c);
  for (i=r*c-1;i>=0;i--)
  {
    if (a->m[i])
      b->m[i] = prCopyR(a->m[i],origR,currRing);
  }
  if (R!=NULL)
  {
    R = idrCopyR(R,origR,currRing);
    //if (ar>1) // otherwise done in mpMinorToResult
    //{
    //  matrix bb=(matrix)kNF(R,currQuotient,(ideal)b);
    //  bb->rank=b->rank; bb->nrows=b->nrows; bb->ncols=b->ncols;
    //  idDelete((ideal*)&b); b=bb;
    //}
  }
  result=idInit(32,1);
  if(ar>1) mpRecMin(ar-1,result,elems,b,r,c,NULL,R);
  else mpMinorToResult(result,elems,b,r,c,R);
  idDelete((ideal *)&b);
  if (R!=NULL) idDelete(&R);
  idSkipZeroes(result);
  rChangeCurrRing(origR);
  result = idrMoveR(result,tmpR,origR);
  smKillModifiedRing(tmpR);
  idTest(result);
  return result;
}
#endif

/*2
*returns TRUE if id1 is a submodule of id2
*/
BOOLEAN idIsSubModule(ideal id1,ideal id2)
{
  int i;
  poly p;

  if (idIs0(id1)) return TRUE;
  for (i=0;i<IDELEMS(id1);i++)
  {
    if (id1->m[i] != NULL)
    {
      p = kNF(id2,currQuotient,id1->m[i]);
      if (p != NULL)
      {
        pDelete(&p);
        return FALSE;
      }
    }
  }
  return TRUE;
}

BOOLEAN idTestHomModule(ideal m, ideal Q, intvec *w)
{
  if ((Q!=NULL) && (!idHomIdeal(Q,NULL)))  { PrintS(" Q not hom\n"); return FALSE;}
  if (idIs0(m)) return TRUE;

  int cmax=-1;
  int i;
  poly p=NULL;
  int length=IDELEMS(m);
  polyset P=m->m;
  for (i=length-1;i>=0;i--)
  {
    p=P[i];
    if (p!=NULL) cmax=si_max(cmax,(int)pMaxComp(p)+1);
  }
  if (w != NULL)
  if (w->length()+1 < cmax)
  {
    // Print("length: %d - %d \n", w->length(),cmax);
    return FALSE;
  }

  if(w!=NULL)
    pSetModDeg(w);

  for (i=length-1;i>=0;i--)
  {
    p=P[i];
    poly q=p;
    if (p!=NULL)
    {
      int d=currRing->pFDeg(p,currRing);
      loop
      {
        pIter(p);
        if (p==NULL) break;
        if (d!=currRing->pFDeg(p,currRing))
        {
          //pWrite(q); wrp(p); Print(" -> %d - %d\n",d,pFDeg(p,currRing));
          if(w!=NULL)
            pSetModDeg(NULL);
          return FALSE;
        }
      }
    }
  }

  if(w!=NULL)
    pSetModDeg(NULL);

  return TRUE;
}

int idMinDegW(ideal M,intvec *w)
{
  int d=-1;
  for(int i=0;i<IDELEMS(M);i++)
  {
    int d0=pMinDeg(M->m[i],w);
    if(-1<d0&&(d0<d||d==-1))
      d=d0;
  }
  return d;
}

ideal idSeries(int n,ideal M,matrix U,intvec *w)
{
  for(int i=IDELEMS(M)-1;i>=0;i--)
  {
    if(U==NULL)
      M->m[i]=pSeries(n,M->m[i],NULL,w);
    else
    {
      M->m[i]=pSeries(n,M->m[i],MATELEM(U,i+1,i+1),w);
      MATELEM(U,i+1,i+1)=NULL;
    }
  }
  if(U!=NULL)
    idDelete((ideal*)&U);
  return M;
}

matrix idDiff(matrix i, int k)
{
  int e=MATCOLS(i)*MATROWS(i);
  matrix r=mpNew(MATROWS(i),MATCOLS(i));
  r->rank=i->rank;
  int j;
  for(j=0; j<e; j++)
  {
    r->m[j]=pDiff(i->m[j],k);
  }
  return r;
}

matrix idDiffOp(ideal I, ideal J,BOOLEAN multiply)
{
  matrix r=mpNew(IDELEMS(I),IDELEMS(J));
  int i,j;
  for(i=0; i<IDELEMS(I); i++)
  {
    for(j=0; j<IDELEMS(J); j++)
    {
      MATELEM(r,i+1,j+1)=pDiffOp(I->m[i],J->m[j],multiply);
    }
  }
  return r;
}

/*3
*handles for some ideal operations the ring/syzcomp managment
*returns all syzygies (componentwise-)shifted by -syzcomp
*or -syzcomp-1 (in case of ideals as input)
static ideal idHandleIdealOp(ideal arg,int syzcomp,int isIdeal=FALSE)
{
  ring orig_ring=currRing;
  ring syz_ring=rCurrRingAssure_SyzComp();
  rSetSyzComp(length,syz_ring);

  ideal s_temp;
  if (orig_ring!=syz_ring)
    s_temp=idrMoveR_NoSort(arg,orig_ring, syz_ring);
  else
    s_temp=arg;

  ideal s_temp1 = kStd(s_temp,currQuotient,testHomog,&w,NULL,length);
  if (w!=NULL) delete w;

  if (syz_ring!=orig_ring)
  {
    idDelete(&s_temp);
    rChangeCurrRing(orig_ring);
  }

  idDelete(&temp);
  ideal temp1=idRingCopy(s_temp1,syz_ring);

  if (syz_ring!=orig_ring)
  {
    rChangeCurrRing(syz_ring);
    idDelete(&s_temp1);
    rChangeCurrRing(orig_ring);
    rKill(syz_ring);
  }

  for (i=0;i<IDELEMS(temp1);i++)
  {
    if ((temp1->m[i]!=NULL)
    && (pGetComp(temp1->m[i])<=length))
    {
      pDelete(&(temp1->m[i]));
    }
    else
    {
      p_Shift(&(temp1->m[i]),-length,currRing);
    }
  }
  temp1->rank = rk;
  idSkipZeroes(temp1);

  return temp1;
}
*/
/*2
* represents (h1+h2)/h2=h1/(h1 intersect h2)
*/
//ideal idModulo (ideal h2,ideal h1)
ideal idModulo (ideal h2,ideal h1, tHomog hom, intvec ** w)
{
  intvec *wtmp=NULL;

  int i,j,k,rk,flength=0,slength,length;
  poly p,q;

  if (idIs0(h2))
    return idFreeModule(si_max(1,h2->ncols));
  if (!idIs0(h1))
    flength = id_RankFreeModule(h1,currRing);
  slength = id_RankFreeModule(h2,currRing);
  length  = si_max(flength,slength);
  if (length==0)
  {
    length = 1;
  }
  ideal temp = idInit(IDELEMS(h2),length+IDELEMS(h2));
  if ((w!=NULL)&&((*w)!=NULL))
  {
    //Print("input weights:");(*w)->show(1);PrintLn();
    int d;
    int k;
    wtmp=new intvec(length+IDELEMS(h2));
    for (i=0;i<length;i++)
      ((*wtmp)[i])=(**w)[i];
    for (i=0;i<IDELEMS(h2);i++)
    {
      poly p=h2->m[i];
      if (p!=NULL)
      {
        d = pDeg(p);
        k= pGetComp(p);
        if (slength>0) k--;
        d +=((**w)[k]);
        ((*wtmp)[i+length]) = d;
      }
    }
    //Print("weights:");wtmp->show(1);PrintLn();
  }
  for (i=0;i<IDELEMS(h2);i++)
  {
    temp->m[i] = pCopy(h2->m[i]);
    q = pOne();
    pSetComp(q,i+1+length);
    pSetmComp(q);
    if(temp->m[i]!=NULL)
    {
      if (slength==0) p_Shift(&(temp->m[i]),1,currRing);
      p = temp->m[i];
      while (pNext(p)!=NULL) pIter(p);
      pNext(p) = q;
    }
    else
      temp->m[i]=q;
  }
  rk = k = IDELEMS(h2);
  if (!idIs0(h1))
  {
    pEnlargeSet(&(temp->m),IDELEMS(temp),IDELEMS(h1));
    IDELEMS(temp) += IDELEMS(h1);
    for (i=0;i<IDELEMS(h1);i++)
    {
      if (h1->m[i]!=NULL)
      {
        temp->m[k] = pCopy(h1->m[i]);
        if (flength==0) p_Shift(&(temp->m[k]),1,currRing);
        k++;
      }
    }
  }

  ring orig_ring=currRing;
  ring syz_ring=rAssure_SyzComp(orig_ring.TRUE);
  rChangeCurrRing(syz_ring);
  rSetSyzComp(length, syz_ring);
  ideal s_temp;

  if (syz_ring != orig_ring)
  {
    s_temp = idrMoveR_NoSort(temp, orig_ring, syz_ring);
  }
  else
  {
    s_temp = temp;
  }

  idTest(s_temp);
  ideal s_temp1 = kStd(s_temp,currQuotient,hom,&wtmp,NULL,length);

  //if (wtmp!=NULL)  Print("output weights:");wtmp->show(1);PrintLn();
  if ((w!=NULL) && (*w !=NULL) && (wtmp!=NULL))
  {
    delete *w;
    *w=new intvec(IDELEMS(h2));
    for (i=0;i<IDELEMS(h2);i++)
      ((**w)[i])=(*wtmp)[i+length];
  }
  if (wtmp!=NULL) delete wtmp;

  for (i=0;i<IDELEMS(s_temp1);i++)
  {
    if ((s_temp1->m[i]!=NULL)
    && (pGetComp(s_temp1->m[i])<=length))
    {
      pDelete(&(s_temp1->m[i]));
    }
    else
    {
      p_Shift(&(s_temp1->m[i]),-length,currRing);
    }
  }
  s_temp1->rank = rk;
  idSkipZeroes(s_temp1);

  if (syz_ring!=orig_ring)
  {
    rChangeCurrRing(orig_ring);
    s_temp1 = idrMoveR_NoSort(s_temp1, syz_ring, orig_ring);
    rKill(syz_ring);
    // Hmm ... here seems to be a memory leak
    // However, simply deleting it causes memory trouble
    // idDelete(&s_temp);
  }
  else
  {
    idDelete(&temp);
  }
  idTest(s_temp1);
  return s_temp1;
}

/*
*computes module-weights for liftings of homogeneous modules
*/
intvec * idMWLift(ideal mod,intvec * weights)
{
  if (idIs0(mod)) return new intvec(2);
  int i=IDELEMS(mod);
  while ((i>0) && (mod->m[i-1]==NULL)) i--;
  intvec *result = new intvec(i+1);
  while (i>0)
  {
    (*result)[i]=currRing->pFDeg(mod->m[i],currRing)+(*weights)[pGetComp(mod->m[i])];
  }
  return result;
}

/*2
*sorts the kbase for idCoef* in a special way (lexicographically
*with x_max,...,x_1)
*/
ideal idCreateSpecialKbase(ideal kBase,intvec ** convert)
{
  int i;
  ideal result;

  if (idIs0(kBase)) return NULL;
  result = idInit(IDELEMS(kBase),kBase->rank);
  *convert = idSort(kBase,FALSE);
  for (i=0;i<(*convert)->length();i++)
  {
    result->m[i] = pCopy(kBase->m[(**convert)[i]-1]);
  }
  return result;
}

/*2
*returns the index of a given monom in the list of the special kbase
*/
int idIndexOfKBase(poly monom, ideal kbase)
{
  int j=IDELEMS(kbase);

  while ((j>0) && (kbase->m[j-1]==NULL)) j--;
  if (j==0) return -1;
  int i=(currRing->N);
  while (i>0)
  {
    loop
    {
      if (pGetExp(monom,i)>pGetExp(kbase->m[j-1],i)) return -1;
      if (pGetExp(monom,i)==pGetExp(kbase->m[j-1],i)) break;
      j--;
      if (j==0) return -1;
    }
    if (i==1)
    {
      while(j>0)
      {
        if (pGetComp(monom)==pGetComp(kbase->m[j-1])) return j-1;
        if (pGetComp(monom)>pGetComp(kbase->m[j-1])) return -1;
        j--;
      }
    }
    i--;
  }
  return -1;
}

/*2
*decomposes the monom in a part of coefficients described by the
*complement of how and a monom in variables occuring in how, the
*index of which in kbase is returned as integer pos (-1 if it don't
*exists)
*/
poly idDecompose(poly monom, poly how, ideal kbase, int * pos)
{
  int i;
  poly coeff=pOne(), base=pOne();

  for (i=1;i<=(currRing->N);i++)
  {
    if (pGetExp(how,i)>0)
    {
      pSetExp(base,i,pGetExp(monom,i));
    }
    else
    {
      pSetExp(coeff,i,pGetExp(monom,i));
    }
  }
  pSetComp(base,pGetComp(monom));
  pSetm(base);
  pSetCoeff(coeff,nCopy(pGetCoeff(monom)));
  pSetm(coeff);
  *pos = idIndexOfKBase(base,kbase);
  if (*pos<0)
    pDelete(&coeff);
  pDelete(&base);
  return coeff;
}

/*2
*returns a matrix A of coefficients with kbase*A=arg
*if all monomials in variables of how occur in kbase
*the other are deleted
*/
matrix idCoeffOfKBase(ideal arg, ideal kbase, poly how)
{
  matrix result;
  ideal tempKbase;
  poly p,q;
  intvec * convert;
  int i=IDELEMS(kbase),j=IDELEMS(arg),k,pos;
#if 0
  while ((i>0) && (kbase->m[i-1]==NULL)) i--;
  if (idIs0(arg))
    return mpNew(i,1);
  while ((j>0) && (arg->m[j-1]==NULL)) j--;
  result = mpNew(i,j);
#else
  result = mpNew(i, j);
  while ((j>0) && (arg->m[j-1]==NULL)) j--;
#endif

  tempKbase = idCreateSpecialKbase(kbase,&convert);
  for (k=0;k<j;k++)
  {
    p = arg->m[k];
    while (p!=NULL)
    {
      q = idDecompose(p,how,tempKbase,&pos);
      if (pos>=0)
      {
        MATELEM(result,(*convert)[pos],k+1) =
            pAdd(MATELEM(result,(*convert)[pos],k+1),q);
      }
      else
        pDelete(&q);
      pIter(p);
    }
  }
  idDelete(&tempKbase);
  return result;
}

static void idDeleteComps(ideal arg,int* red_comp,int del)
// red_comp is an array [0..args->rank]
{
  int i,j;
  poly p;

  for (i=IDELEMS(arg)-1;i>=0;i--)
  {
    p = arg->m[i];
    while (p!=NULL)
    {
      j = pGetComp(p);
      if (red_comp[j]!=j)
      {
        pSetComp(p,red_comp[j]);
        pSetmComp(p);
      }
      pIter(p);
    }
  }
  (arg->rank) -= del;
}

/*2
* returns the presentation of an isomorphic, minimally
* embedded  module (arg represents the quotient!)
*/
ideal idMinEmbedding(ideal arg,BOOLEAN inPlace, intvec **w)
{
  if (idIs0(arg)) return idInit(1,arg->rank);
  int i,next_gen,next_comp;
  ideal res=arg;
  if (!inPlace) res = idCopy(arg);
  res->rank=si_max(res->rank,id_RankFreeModule(res,currRing));
  int *red_comp=(int*)omAlloc((res->rank+1)*sizeof(int));
  for (i=res->rank;i>=0;i--) red_comp[i]=i;

  int del=0;
  loop
  {
    next_gen = idReadOutPivot(res,&next_comp);
    if (next_gen<0) break;
    del++;
    syGaussForOne(res,next_gen,next_comp,0,IDELEMS(res));
    for(i=next_comp+1;i<=arg->rank;i++) red_comp[i]--;
    if ((w !=NULL)&&(*w!=NULL))
    {
      for(i=next_comp;i<(*w)->length();i++) (**w)[i-1]=(**w)[i];
    }
  }

  idDeleteComps(res,red_comp,del);
  idSkipZeroes(res);
  omFree(red_comp);

  if ((w !=NULL)&&(*w!=NULL) &&(del>0))
  {
    intvec *wtmp=new intvec((*w)->length()-del);
    for(i=0;i<res->rank;i++) (*wtmp)[i]=(**w)[i];
    delete *w;
    *w=wtmp;
  }
  return res;
}

#include <polys/clapsing.h>

#ifdef HAVE_FACTORY
poly id_GCD(poly f, poly g, const ring r)
{
  ring save_r=currRing;
  rChangeCurrRing(r);
  ideal I=idInit(2,1); I->m[0]=f; I->m[1]=g;
  intvec *w = NULL;
  ideal S=idSyzygies(I,testHomog,&w);
  if (w!=NULL) delete w;
  poly gg=pTakeOutComp(&(S->m[0]),2);
  idDelete(&S);
  poly gcd_p=singclap_pdivide(f,gg,r);
  pDelete(&gg);
  rChangeCurrRing(save_r);
  return gcd_p;
}
#endif

/*2
* xx,q: arrays of length 0..rl-1
* xx[i]: SB mod q[i]
* assume: char=0
* assume: q[i]!=0
* destroys xx
*/
#ifdef HAVE_FACTORY
ideal idChineseRemainder(ideal *xx, number *q, int rl)
{
  int cnt=IDELEMS(xx[0])*xx[0]->nrows;
  ideal result=idInit(cnt,xx[0]->rank);
  result->nrows=xx[0]->nrows; // for lifting matrices
  result->ncols=xx[0]->ncols; // for lifting matrices
  int i,j;
  poly r,h,hh,res_p;
  number *x=(number *)omAlloc(rl*sizeof(number));
  for(i=cnt-1;i>=0;i--)
  {
    res_p=NULL;
    loop
    {
      r=NULL;
      for(j=rl-1;j>=0;j--)
      {
        h=xx[j]->m[i];
        if ((h!=NULL)
        &&((r==NULL)||(pLmCmp(r,h)==-1)))
          r=h;
      }
      if (r==NULL) break;
      h=pHead(r);
      for(j=rl-1;j>=0;j--)
      {
        hh=xx[j]->m[i];
        if ((hh!=NULL) && (pLmCmp(r,hh)==0))
        {
          x[j]=pGetCoeff(hh);
          hh=pLmFreeAndNext(hh);
          xx[j]->m[i]=hh;
        }
        else
          x[j]=nlInit(0, currRing);
      }
      number n=nlChineseRemainder(x,q,rl);
      for(j=rl-1;j>=0;j--)
      {
        x[j]=NULL; // nlInit(0...) takes no memory
      }
      if (nlIsZero(n)) pDelete(&h);
      else
      {
        pSetCoeff(h,n);
        //Print("new mon:");pWrite(h);
        res_p=pAdd(res_p,h);
      }
    }
    result->m[i]=res_p;
  }
  omFree(x);
  for(i=rl-1;i>=0;i--) idDelete(&(xx[i]));
  omFree(xx);
  return result;
}
#endif
/* currently unsed:
ideal idChineseRemainder(ideal *xx, intvec *iv)
{
  int rl=iv->length();
  number *q=(number *)omAlloc(rl*sizeof(number));
  int i;
  for(i=0; i<rl; i++)
  {
    q[i]=nInit((*iv)[i]);
  }
  return idChineseRemainder(xx,q,rl);
}
*/
/*
 * lift ideal with coeffs over Z (mod N) to Q via Farey
 */
ideal idFarey(ideal x, number N)
{
  int cnt=IDELEMS(x)*x->nrows;
  ideal result=idInit(cnt,x->rank);
  result->nrows=x->nrows; // for lifting matrices
  result->ncols=x->ncols; // for lifting matrices

  int i;
  for(i=cnt-1;i>=0;i--)
  {
    poly h=pCopy(x->m[i]);
    result->m[i]=h;
    while(h!=NULL)
    {
      number c=pGetCoeff(h);
      pSetCoeff0(h,nlFarey(c,N));
      nDelete(&c);
      pIter(h);
    }
    while((result->m[i]!=NULL)&&(nIsZero(pGetCoeff(result->m[i]))))
    {
      pLmDelete(&(result->m[i]));
    }
    h=result->m[i];
    while((h!=NULL) && (pNext(h)!=NULL))
    {
      if(nIsZero(pGetCoeff(pNext(h))))
      {
        pLmDelete(&pNext(h));
      }
      else pIter(h);
    }
  }
  return result;
}

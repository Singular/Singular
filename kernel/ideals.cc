/****************************************
*  Computer Algebra System SINGULAR     *
****************************************/
/* $Id$ */
/*
* ABSTRACT - all basic methods to manipulate ideals
*/

/* includes */
#include <kernel/mod2.h>

#ifndef NDEBUG
# define MYTEST 0
#else /* ifndef NDEBUG */
# define MYTEST 0
#endif /* ifndef NDEBUG */

#include <kernel/options.h>
#include <omalloc/omalloc.h>
#include <kernel/febase.h>
#include <kernel/numbers.h>
#include <kernel/longrat.h>
#include <kernel/polys.h>
#include <kernel/ring.h>
#include <kernel/kstd1.h>
#include <kernel/matpol.h>
#include <kernel/weight.h>
#include <kernel/intvec.h>
#include <kernel/syz.h>
#include <kernel/sparsmat.h>
#include <kernel/ideals.h>
#include <kernel/prCopy.h>
#include <kernel/gring.h>


omBin sip_sideal_bin = omGetSpecBin(sizeof(sip_sideal));

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
* initialise an ideal
*/
#ifdef PDEBUG
ideal idDBInit(int idsize, int rank, const char *f, int l)
#else
ideal idInit(int idsize, int rank)
#endif
{
  /*- initialise an ideal -*/
  ideal hh = (ideal )omAllocBin(sip_sideal_bin);
  hh->nrows = 1;
  hh->rank = rank;
  IDELEMS(hh) = idsize;
  if (idsize>0)
  {
    hh->m = (poly *)omAlloc0(idsize*sizeof(poly));
  }
  else
    hh->m=NULL;
  return hh;
}

#ifndef NDEBUG
// this is only for outputting an ideal within the debugger
void idShow(const ideal id, const ring lmRing, const ring tailRing, const int debugPrint)
{
  assume( debugPrint >= 0 );

  if( id == NULL )
    PrintS("(NULL)");
  else
  {
    Print("Module of rank %ld,real rank %ld and %d generators.\n",
          id->rank,idRankFreeModule(id, lmRing, tailRing),IDELEMS(id));

    int j = (id->ncols*id->nrows) - 1;
    while ((j > 0) && (id->m[j]==NULL)) j--;
    for (int i = 0; i <= j; i++)
    {
      Print("generator %d: ",i); p_DebugPrint(id->m[i], lmRing, tailRing, debugPrint);
    }
  }
}
#endif

/*2
* initialise the maximal ideal (at 0)
*/
ideal idMaxIdeal (void)
{
  int l;
  ideal hh=NULL;

  hh=idInit(pVariables,1);
  for (l=0; l<pVariables; l++)
  {
    hh->m[l] = pOne();
    pSetExp(hh->m[l],l+1,1);
    pSetm(hh->m[l]);
  }
  return hh;
}

/*2
* deletes an ideal/matrix
*/
void id_Delete (ideal * h, ring r)
{
  int j,elems;
  if (*h == NULL)
    return;
  elems=j=(*h)->nrows*(*h)->ncols;
  if (j>0)
  {
    do
    {
      p_Delete(&((*h)->m[--j]), r);
    }
    while (j>0);
    omFreeSize((ADDRESS)((*h)->m),sizeof(poly)*elems);
  }
  omFreeBin((ADDRESS)*h, sip_sideal_bin);
  *h=NULL;
}


/*2
* Shallowdeletes an ideal/matrix
*/
void id_ShallowDelete (ideal *h, ring r)
{
  int j,elems;
  if (*h == NULL)
    return;
  elems=j=(*h)->nrows*(*h)->ncols;
  if (j>0)
  {
    do
    {
      p_ShallowDelete(&((*h)->m[--j]), r);
    }
    while (j>0);
    omFreeSize((ADDRESS)((*h)->m),sizeof(poly)*elems);
  }
  omFreeBin((ADDRESS)*h, sip_sideal_bin);
  *h=NULL;
}

/*2
*gives an ideal the minimal possible size
*/
void idSkipZeroes (ideal ide)
{
  int k;
  int j = -1;
  BOOLEAN change=FALSE;
  for (k=0; k<IDELEMS(ide); k++)
  {
    if (ide->m[k] != NULL)
    {
      j++;
      if (change)
      {
        ide->m[j] = ide->m[k];
      }
    }
    else
    {
      change=TRUE;
    }
  }
  if (change)
  {
    if (j == -1)
      j = 0;
    else
    {
      for (k=j+1; k<IDELEMS(ide); k++)
        ide->m[k] = NULL;
    }
    pEnlargeSet(&(ide->m),IDELEMS(ide),j+1-IDELEMS(ide));
    IDELEMS(ide) = j+1;
  }
}

/*2
* copies the first k (>= 1) entries of the given ideal
* and returns these as a new ideal
* (Note that the copied polynomials may be zero.)
*/
ideal idCopyFirstK (const ideal ide, const int k)
{
  ideal newI = idInit(k, 0);
  for (int i = 0; i < k; i++)
    newI->m[i] = pCopy(ide->m[i]);
  return newI;
}

/*2
* ideal id = (id[i])
* result is leadcoeff(id[i]) = 1
*/
void idNorm(ideal id)
{
  for (int i=IDELEMS(id)-1; i>=0; i--)
  {
    if (id->m[i] != NULL)
    {
      pNorm(id->m[i]);
    }
  }
}

/*2
* ideal id = (id[i]), c any number
* if id[i] = c*id[j] then id[j] is deleted for j > i
*/
void idDelMultiples(ideal id)
{
  int i, j;
  int k = IDELEMS(id)-1;
  for (i=k; i>=0; i--)
  {
    if (id->m[i]!=NULL)
    {
      for (j=k; j>i; j--)
      {
        if ((id->m[j]!=NULL)
        && (pComparePolys(id->m[i], id->m[j])))
        {
          pDelete(&id->m[j]);
        }
      }
    }
  }
}

/*2
* ideal id = (id[i])
* if id[i] = id[j] then id[j] is deleted for j > i
*/
void idDelEquals(ideal id)
{
  int i, j;
  int k = IDELEMS(id)-1;
  for (i=k; i>=0; i--)
  {
    if (id->m[i]!=NULL)
    {
      for (j=k; j>i; j--)
      {
        if ((id->m[j]!=NULL)
        && (pEqualPolys(id->m[i], id->m[j])))
        {
          pDelete(&id->m[j]);
        }
      }
    }
  }
}

//
// Delete id[j], if Lm(j) == Lm(i) and j > i
//
void idDelLmEquals(ideal id)
{
  int i, j;
  int k = IDELEMS(id)-1;
  for (i=k; i>=0; i--)
  {
    if (id->m[i] != NULL)
    {
      for (j=k; j>i; j--)
      {
        if ((id->m[j] != NULL)
        && pLmEqual(id->m[i], id->m[j]))
        {
          pDelete(&id->m[j]);
        }
      }
    }
  }
}

void idDelDiv(ideal id)
{
  int i, j;
  int k = IDELEMS(id)-1;
  for (i=k; i>=0; i--)
  {
    if (id->m[i] != NULL)
    {
      for (j=k; j>i; j--)
      {
        if (id->m[j]!=NULL)
        {
          if(pDivisibleBy(id->m[i], id->m[j]))
          {
            pDelete(&id->m[j]);
          }
          else if(pDivisibleBy(id->m[j], id->m[i]))
          {
            pDelete(&id->m[i]);
            break;
          }
        }
      }
    }
  }
}

/*2
*test if the ideal has only constant polynomials
*/
BOOLEAN idIsConstant(ideal id)
{
  int k;
  for (k = IDELEMS(id)-1; k>=0; k--)
  {
    if (pIsConstantPoly(id->m[k]) == FALSE)
      return FALSE;
  }
  return TRUE;
}

/*2
* copy an ideal
*/
#ifdef PDEBUG
ideal idDBCopy(ideal h1,const char *f,int l)
{
  int i;
  ideal h2;

  idDBTest(h1,PDEBUG,f,l);
//#ifdef TEST
  if (h1 == NULL)
  {
    h2=idDBInit(1,1,f,l);
  }
  else
//#endif
  {
    h2=idDBInit(IDELEMS(h1),h1->rank,f,l);
    for (i=IDELEMS(h1)-1; i>=0; i--)
      h2->m[i] = pCopy(h1->m[i]);
  }
  return h2;
}
#endif

ideal id_Copy (ideal h1, const ring r)
{
  int i;
  ideal h2;

//#ifdef TEST
  if (h1 == NULL)
  {
    h2=idInit(1,1);
  }
  else
//#endif
  {
    h2=idInit(IDELEMS(h1),h1->rank);
    for (i=IDELEMS(h1)-1; i>=0; i--)
      h2->m[i] = p_Copy(h1->m[i],r);
  }
  return h2;
}

#ifdef PDEBUG
void idDBTest(ideal h1, int level, const char *f,const int l)
{
  int i;

  if (h1 != NULL)
  {
    // assume(IDELEMS(h1) > 0); for ideal/module, does not apply to matrix
    omCheckAddrSize(h1,sizeof(*h1));
    omdebugAddrSize(h1->m,h1->ncols*h1->nrows*sizeof(poly));
    /* to be able to test matrices: */
    for (i=(h1->ncols*h1->nrows)-1; i>=0; i--)
      _p_Test(h1->m[i], currRing, level);
    int new_rk=idRankFreeModule(h1);
    if(new_rk > h1->rank)
    {
      dReportError("wrong rank %d (should be %d) in %s:%d\n",
                   h1->rank, new_rk, f,l);
      omPrintAddrInfo(stderr, h1, " for ideal");
      h1->rank=new_rk;
    }
  }
}
#endif

/*3
* for idSort: compare a and b revlex inclusive module comp.
*/
static int pComp_RevLex(poly a, poly b,BOOLEAN nolex)
{
  if (b==NULL) return 1;
  if (a==NULL) return -1;

  if (nolex) 
  {
    int r=pLmCmp(a,b);
    if (r!=0) return r;
    number h=nSub(pGetCoeff(a),pGetCoeff(b));
    r = -1+nIsZero(h)+2*nGreaterZero(h); /* -1: <, 0:==, 1: > */
    nDelete(&h);
    return r;
  }
  int l=pVariables;
  while ((l>0) && (pGetExp(a,l)==pGetExp(b,l))) l--;
  if (l==0)
  {
    if (pGetComp(a)==pGetComp(b))
    {
      number h=nSub(pGetCoeff(a),pGetCoeff(b));
      int r = -1+nIsZero(h)+2*nGreaterZero(h); /* -1: <, 0:==, 1: > */
      nDelete(&h);
      return r;
    }
    if (pGetComp(a)>pGetComp(b)) return 1;
  }
  else if (pGetExp(a,l)>pGetExp(b,l))
    return 1;
  return -1;
}

/*2
*sorts the ideal w.r.t. the actual ringordering
*uses lex-ordering when nolex = FALSE
*/
intvec *idSort(ideal id,BOOLEAN nolex)
{
  poly p,q;
  intvec * result = new intvec(IDELEMS(id));
  int i, j, actpos=0, newpos, l;
  int diff, olddiff, lastcomp, newcomp;
  BOOLEAN notFound;

  for (i=0;i<IDELEMS(id);i++)
  {
    if (id->m[i]!=NULL)
    {
      notFound = TRUE;
      newpos = actpos / 2;
      diff = (actpos+1) / 2;
      diff = (diff+1) / 2;
      lastcomp = pComp_RevLex(id->m[i],id->m[(*result)[newpos]],nolex);
      if (lastcomp<0)
      {
        newpos -= diff;
      }
      else if (lastcomp>0)
      {
        newpos += diff;
      }
      else
      {
        notFound = FALSE;
      }
      //while ((newpos>=0) && (newpos<actpos) && (notFound))
      while (notFound && (newpos>=0) && (newpos<actpos))
      {
        newcomp = pComp_RevLex(id->m[i],id->m[(*result)[newpos]],nolex);
        olddiff = diff;
        if (diff>1)
        {
          diff = (diff+1) / 2;
          if ((newcomp==1)
          && (actpos-newpos>1)
          && (diff>1)
          && (newpos+diff>=actpos))
          {
            diff = actpos-newpos-1;
          }
          else if ((newcomp==-1)
          && (diff>1)
          && (newpos<diff))
          {
            diff = newpos;
          }
        }
        if (newcomp<0)
        {
          if ((olddiff==1) && (lastcomp>0))
            notFound = FALSE;
          else
            newpos -= diff;
        }
        else if (newcomp>0)
        {
          if ((olddiff==1) && (lastcomp<0))
          {
            notFound = FALSE;
            newpos++;
          }
          else
          {
            newpos += diff;
          }
        }
        else
        {
          notFound = FALSE;
        }
        lastcomp = newcomp;
        if (diff==0) notFound=FALSE; /*hs*/
      }
      if (newpos<0) newpos = 0;
      if (newpos>actpos) newpos = actpos;
      while ((newpos<actpos) && (pComp_RevLex(id->m[i],id->m[(*result)[newpos]],nolex)==0))
        newpos++;
      for (j=actpos;j>newpos;j--)
      {
        (*result)[j] = (*result)[j-1];
      }
      (*result)[newpos] = i;
      actpos++;
    }
  }
  for (j=0;j<actpos;j++) (*result)[j]++;
  return result;
}

/*2
* concat the lists h1 and h2 without zeros
*/
ideal idSimpleAdd (ideal h1,ideal h2)
{
  int i,j,r,l;
  ideal result;

  if (h1==NULL) return idCopy(h2);
  if (h2==NULL) return idCopy(h1);
  j = IDELEMS(h1)-1;
  while ((j >= 0) && (h1->m[j] == NULL)) j--;
  i = IDELEMS(h2)-1;
  while ((i >= 0) && (h2->m[i] == NULL)) i--;
  r = si_max(h1->rank,h2->rank);
  if (i+j==(-2))
    return idInit(1,r);
  else
    result=idInit(i+j+2,r);
  for (l=j; l>=0; l--)
  {
    result->m[l] = pCopy(h1->m[l]);
  }
  r = i+j+1;
  for (l=i; l>=0; l--, r--)
  {
    result->m[r] = pCopy(h2->m[l]);
  }
  return result;
}

/*2
* insert h2 into h1 (if h2 is not the zero polynomial)
* return TRUE iff h2 was indeed inserted
*/
BOOLEAN idInsertPoly (ideal h1, poly h2)
{
  if (h2==NULL) return FALSE;
  int j = IDELEMS(h1)-1;
  while ((j >= 0) && (h1->m[j] == NULL)) j--;
  j++;
  if (j==IDELEMS(h1))
  {
    pEnlargeSet(&(h1->m),IDELEMS(h1),16);
    IDELEMS(h1)+=16;
  }
  h1->m[j]=h2;
  return TRUE;
}

/*2
* insert h2 into h1 depending on the two boolean parameters:
* - if zeroOk is true, then h2 will also be inserted when it is zero
* - if duplicateOk is true, then h2 will also be inserted when it is
*   already present in h1
* return TRUE iff h2 was indeed inserted
*/
BOOLEAN idInsertPolyWithTests (ideal h1, const int validEntries,
  const poly h2, const bool zeroOk, const bool duplicateOk)
{
  if ((!zeroOk) && (h2 == NULL)) return FALSE;
  if (!duplicateOk)
  {
    bool h2FoundInH1 = false;
    int i = 0;
    while ((i < validEntries) && (!h2FoundInH1))
    {
      h2FoundInH1 = pEqualPolys(h1->m[i], h2);
      i++;
    }
    if (h2FoundInH1) return FALSE;
  }
  if (validEntries == IDELEMS(h1))
  {
    pEnlargeSet(&(h1->m), IDELEMS(h1), 16);
    IDELEMS(h1) += 16;
  }
  h1->m[validEntries] = h2;
  return TRUE;
}

/*2
* h1 + h2
*/
ideal idAdd (ideal h1,ideal h2)
{
  ideal result = idSimpleAdd(h1,h2);
  idCompactify(result);
  return result;
}

/*2
* h1 * h2
*/
ideal  idMult (ideal h1,ideal  h2)
{
  int i,j,k;
  ideal  hh;

  j = IDELEMS(h1);
  while ((j > 0) && (h1->m[j-1] == NULL)) j--;
  i = IDELEMS(h2);
  while ((i > 0) && (h2->m[i-1] == NULL)) i--;
  j = j * i;
  if (j == 0)
    hh = idInit(1,1);
  else
    hh=idInit(j,1);
  if (h1->rank<h2->rank)
    hh->rank = h2->rank;
  else
    hh->rank = h1->rank;
  if (j==0) return hh;
  k = 0;
  for (i=0; i<IDELEMS(h1); i++)
  {
    if (h1->m[i] != NULL)
    {
      for (j=0; j<IDELEMS(h2); j++)
      {
        if (h2->m[j] != NULL)
        {
          hh->m[k] = ppMult_qq(h1->m[i],h2->m[j]);
          k++;
        }
      }
    }
  }
  {
    idCompactify(hh);
    return hh;
  }
}

/*2
*returns true if h is the zero ideal
*/
BOOLEAN idIs0 (ideal h)
{
  int i;

  if (h == NULL) return TRUE;
  i = IDELEMS(h)-1;
  while ((i >= 0) && (h->m[i] == NULL))
  {
    i--;
  }
  if (i < 0)
    return TRUE;
  else
    return FALSE;
}

/*2
* return the maximal component number found in any polynomial in s
*/
long idRankFreeModule (ideal s, ring lmRing, ring tailRing)
{
  if (s!=NULL)
  {
    int  j=0;

    if (rRing_has_Comp(tailRing) && rRing_has_Comp(lmRing))
    {
      int  l=IDELEMS(s);
      poly *p=s->m;
      int  k;
      for (; l != 0; l--)
      {
        if (*p!=NULL)
        {
          pp_Test(*p, lmRing, tailRing);
          k = p_MaxComp(*p, lmRing, tailRing);
          if (k>j) j = k;
        }
        p++;
      }
    }
    return j;
  }
  return -1;
}

BOOLEAN idIsModule(ideal id, ring r)
{
  if (id != NULL && rRing_has_Comp(r))
  {
    int j, l = IDELEMS(id);
    for (j=0; j<l; j++)
    {
      if (id->m[j] != NULL && p_GetComp(id->m[j], r) > 0) return TRUE;
    }
  }
  return FALSE;
}


/*2
*returns true if id is homogenous with respect to the aktual weights
*/
BOOLEAN idHomIdeal (ideal id, ideal Q)
{
  int i;
  BOOLEAN     b;
  if ((id == NULL) || (IDELEMS(id) == 0)) return TRUE;
  i = 0;
  b = TRUE;
  while ((i < IDELEMS(id)) && b)
  {
    b = pIsHomogeneous(id->m[i]);
    i++;
  }
  if ((b) && (Q!=NULL) && (IDELEMS(Q)>0))
  {
    i=0;
    while ((i < IDELEMS(Q)) && b)
    {
      b = pIsHomogeneous(Q->m[i]);
      i++;
    }
  }
  return b;
}

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
  if (rHasGlobalOrdering_currRing())
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
  h3 = idMaxIdeal();
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

/*2
*the minimal index of used variables - 1
*/
int pLowVar (poly p)
{
  int k,l,lex;

  if (p == NULL) return -1;

  k = 32000;/*a very large dummy value*/
  while (p != NULL)
  {
    l = 1;
    lex = pGetExp(p,l);
    while ((l < pVariables) && (lex == 0))
    {
      l++;
      lex = pGetExp(p,l);
    }
    l--;
    if (l < k) k = l;
    pIter(p);
  }
  return k;
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
    for (j=pVariables-1; j>0; j--)
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
* verschiebt die Indizees der Modulerzeugenden um i
*/
void pShift (poly * p,int i)
{
  poly qp1 = *p,qp2 = *p;/*working pointers*/
  int     j = pMaxComp(*p),k = pMinComp(*p);

  if (j+i < 0) return ;
  while (qp1 != NULL)
  {
    if ((pGetComp(qp1)+i > 0) || ((j == -i) && (j == k)))
    {
      pAddComp(qp1,i);
      pSetmComp(qp1);
      qp2 = qp1;
      pIter(qp1);
    }
    else
    {
      if (qp2 == *p)
      {
        pIter(*p);
        pLmDelete(&qp2);
        qp2 = *p;
        qp1 = *p;
      }
      else
      {
        qp2->next = qp1->next;
        if (qp1!=NULL) pLmDelete(&qp1);
        qp1 = qp2->next;
      }
    }
  }
}

/*2
*initialized a field with r numbers between beg and end for the
*procedure idNextChoise
*/
void idInitChoise (int r,int beg,int end,BOOLEAN  *endch,int * choise)
{
  /*returns the first choise of r numbers between beg and end*/
  int i;
  for (i=0; i<r; i++)
  {
    choise[i] = 0;
  }
  if (r <= end-beg+1)
    for (i=0; i<r; i++)
    {
      choise[i] = beg+i;
    }
  if (r > end-beg+1)
    *endch = TRUE;
  else
    *endch = FALSE;
}

/*2
*returns the next choise of r numbers between beg and end
*/
void idGetNextChoise (int r,int end,BOOLEAN  *endch,int * choise)
{
  int i = r-1,j;
  while ((i >= 0) && (choise[i] == end))
  {
    i--;
    end--;
  }
  if (i == -1)
    *endch = TRUE;
  else
  {
    choise[i]++;
    for (j=i+1; j<r; j++)
    {
      choise[j] = choise[i]+j-i;
    }
    *endch = FALSE;
  }
}

/*2
*takes the field choise of d numbers between beg and end, cancels the t-th
*entree and searches for the ordinal number of that d-1 dimensional field
* w.r.t. the algorithm of construction
*/
int idGetNumberOfChoise(int t, int d, int begin, int end, int * choise)
{
  int * localchoise,i,result=0;
  BOOLEAN b=FALSE;

  if (d<=1) return 1;
  localchoise=(int*)omAlloc((d-1)*sizeof(int));
  idInitChoise(d-1,begin,end,&b,localchoise);
  while (!b)
  {
    result++;
    i = 0;
    while ((i<t) && (localchoise[i]==choise[i])) i++;
    if (i>=t)
    {
      i = t+1;
      while ((i<d) && (localchoise[i-1]==choise[i])) i++;
      if (i>=d)
      {
        omFreeSize((ADDRESS)localchoise,(d-1)*sizeof(int));
        return result;
      }
    }
    idGetNextChoise(d-1,end,&b,localchoise);
  }
  omFreeSize((ADDRESS)localchoise,(d-1)*sizeof(int));
  return 0;
}

/*2
*computes the binomial coefficient
*/
int binom (int n,int r)
{
  int i,result;

  if (r==0) return 1;
  if (n-r<r) return binom(n,n-r);
  result = n-r+1;
  for (i=2;i<=r;i++)
  {
    result *= n-r+i;
    if (result<0)
    {
      WarnS("overflow in binomials");
      return 0;
    }
    result /= i;
  }
  return result;
}

/*2
*the free module of rank i
*/
ideal idFreeModule (int i)
{
  int j;
  ideal h;

  h=idInit(i,i);
  for (j=0; j<i; j++)
  {
    h->m[j] = pOne();
    pSetComp(h->m[j],j+1);
    pSetmComp(h->m[j]);
  }
  return h;
}

/*2
* h3 := h1 intersect h2
*/
ideal idSect (ideal h1,ideal h2)
{
  int i,j,k,length;
  int flength = idRankFreeModule(h1);
  int slength = idRankFreeModule(h2);
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
    length = 1;
  }
  j = IDELEMS(first);

  ring orig_ring=currRing;
  ring syz_ring=rCurrRingAssure_SyzComp();
  rSetSyzComp(length);

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
        temp->m[k] = prCopyR(first->m[i], orig_ring);
      q = pOne();
      pSetComp(q,i+1+length);
      pSetmComp(q);
      if (flength==0) pShift(&(temp->m[k]),1);
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
        temp->m[k] = prCopyR(second->m[i], orig_ring);
      if (slength==0) pShift(&(temp->m[k]),1);
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
        p = prMoveR(temp1->m[i], syz_ring);
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
      realrki=idRankFreeModule(arg[i]);
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
  ring syz_ring=rCurrRingAssure_SyzComp();
  rSetSyzComp(syzComp);

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
            bigmat->m[i] = prCopyR(arg[j]->m[l], orig_ring);
          pShift(&(bigmat->m[i]),k*maxrk+isIdeal);
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
        p = prCopyR(tempstd->m[j], syz_ring);
      pShift(&p,-syzComp-isIdeal);
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
  k = idRankFreeModule(h1);
  h2=idCopy(h1);
  i = IDELEMS(h2)-1;
  if (k == 0)
  {
    for (j=0; j<=i; j++) pShift(&(h2->m[j]),1);
    k = 1;
  }
  if (syzcomp<k)
  {
    Warn("syzcomp too low, should be %d instead of %d",k,syzcomp);
    syzcomp = k;
    rSetSyzComp(k);
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
    int curr_syz_limit=rGetCurrSyzLimit();
    if (curr_syz_limit>0)
    for (ii=0;ii<idElemens_h1/*IDELEMS(h1)*/;ii++)
    {
      if (h1->m[ii]!=NULL)
        pShift(&h1->m[ii],curr_syz_limit);
    }
    return result;
  }
  int slength=(int)idRankFreeModule(h1);
  k=si_max(1,slength /*idRankFreeModule(h1)*/);

  assume(currRing != NULL);
  ring orig_ring=currRing;
  ring syz_ring=rCurrRingAssure_SyzComp();

  if (setSyzComp)
    rSetSyzComp(k);

  if (orig_ring != syz_ring)
  {
    s_h1=idrCopyR_NoSort(h1,orig_ring);
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
          pShift(&s_h3->m[j], -k);
        else
          pDelete(&s_h3->m[j]);
      }
    }
    idSkipZeroes(s_h3);
    s_h3->rank -= k;
    rChangeCurrRing(orig_ring);
    s_h3 = idrMoveR_NoSort(s_h3, syz_ring);
    rKill(syz_ring);
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
    ring dp_C_ring = rCurrRingAssure_dp_C();
    if (dp_C_ring != syz_ring)
      e = idrMoveR_NoSort(e, syz_ring);
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
  ring syz_ring=rCurrRingAssure_SyzComp();

  rSetSyzComp(k);

  if (orig_ring != syz_ring)
  {
    s_h1=idrCopyR_NoSort(h1,orig_ring);
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
    s_h3 = idrMoveR_NoSort(s_h3, syz_ring);
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
  int   i, j, k, t, inputIsIdeal=idRankFreeModule(h1);
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
      int curr_syz_limit=rGetCurrSyzLimit();
      if (curr_syz_limit>0)
      for (int ii=0;ii<IDELEMS(h1);ii++)
      {
        if (h1->m[ii]!=NULL)
          pShift(&h1->m[ii],curr_syz_limit);
      }
    }
    return idInit(1,h1->rank);
  }

  BITSET save_verbose=verbose;

  k=si_max(1,(int)idRankFreeModule(h1));

  if ((k==1) && (!lift3)) verbose |=Sy_bit(V_IDLIFT);

  ring orig_ring = currRing;
  ring syz_ring = rCurrRingAssure_SyzComp();
  rSetSyzComp(k);

  ideal s_h1=h1;

  if (orig_ring != syz_ring)
    s_h1 = idrCopyR_NoSort(h1,orig_ring);
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
        if (!inputIsIdeal) pShift(&(s_h3->m[j]), -1);
      }
      else
      {
        // we a syzygy here:
        if (lift3)
        {
          pShift(&s_h3->m[j], -k);
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
      q = prMoveR( s_h2->m[j], syz_ring);
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
    s_h3->m[i] = prMoveR_NoSort(s_h3->m[i], syz_ring);
  }
  if (lift3)
  {
    for (i=0; i<IDELEMS(*syz); i++)
    {
      (*syz)->m[i] = prMoveR_NoSort((*syz)->m[i], syz_ring);
    }
  }

  if (syz_ring!=orig_ring) rKill(syz_ring);
  verbose = save_verbose;
  return s_h3;
}

static void idPrepareStd(ideal s_temp, int k)
{
  int j,rk=idRankFreeModule(s_temp);
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
  int lsmod =idRankFreeModule(submod), i, j, k;
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
  k=si_max(idRankFreeModule(mod),idRankFreeModule(submod));
  if  ((k!=0) && (lsmod==0)) lsmod=1;
  k=si_max(k,(int)mod->rank);
  if (k<submod->rank) { WarnS("rk(submod) > rk(mod) ?");k=submod->rank; }

  ring orig_ring=currRing;
  ring syz_ring=rCurrRingAssure_SyzComp();
  rSetSyzComp(k);

  ideal s_mod, s_temp;
  if (orig_ring != syz_ring)
  {
    s_mod = idrCopyR_NoSort(mod,orig_ring);
    s_temp = idrCopyR_NoSort(submod,orig_ring);
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
        pShift(&(s_temp->m[j-1]),1);
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
      pShift(&(s_result->m[j]),-k);
      pNeg(s_result->m[j]);
    }
  }
  if ((lsmod==0) && (!idIs0(s_rest)))
  {
    for (j=IDELEMS(s_rest);j>0;j--)
    {
      if (s_rest->m[j-1]!=NULL)
      {
        pShift(&(s_rest->m[j-1]),-1);
        s_rest->m[j-1] = s_rest->m[j-1];
      }
    }
  }
  if(syz_ring!=orig_ring)
  {
    idDelete(&s_mod);
    rChangeCurrRing(orig_ring);
    s_result = idrMoveR_NoSort(s_result, syz_ring);
    s_rest = idrMoveR_NoSort(s_rest, syz_ring);
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
      pShift(&s_result->m[i],-comps_to_add);
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
        poly p0=pDivideM(pHead(p),pHead(Q->m[j]));
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
  int k1 = idRankFreeModule(h1);
  int k2 = idRankFreeModule(h2);
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
        pShift(&p,j*k+1);
      else
        pShift(&p,j*k);
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
        pShift(&p,1);
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
          pShift(&p,ll*k+1);
        else
          pShift(&p,ll*k);
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
  ring syz_ring=rCurrRingAssure_SyzComp();
  rSetSyzComp(kmax-1);
  if (orig_ring!=syz_ring)
  //  s_h4 = idrMoveR_NoSort(s_h4,orig_ring);
    s_h4 = idrMoveR(s_h4,orig_ring);
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
        pShift(&s_h3->m[i],-kmax);
      else
        pShift(&s_h3->m[i],-kmax+1);
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
    s_h3 = idrMoveR_NoSort(s_h3, syz_ring);
    rKill(syz_ring);
  }
  idSkipZeroes(s_h3);
  idTest(s_h3);
  return s_h3;
}

/*2
*computes recursively all monomials of a certain degree
*in every step the actvar-th entry in the exponential
*vector is incremented and the other variables are
*computed by recursive calls of makemonoms
*if the last variable is reached, the difference to the
*degree is computed directly
*vars is the number variables
*actvar is the actual variable to handle
*deg is the degree of the monomials to compute
*monomdeg is the actual degree of the monomial in consideration
*/
static void makemonoms(int vars,int actvar,int deg,int monomdeg)
{
  poly p;
  int i=0;

  if ((idpowerpoint == 0) && (actvar ==1))
  {
    idpower[idpowerpoint] = pOne();
    monomdeg = 0;
  }
  while (i<=deg)
  {
    if (deg == monomdeg)
    {
      pSetm(idpower[idpowerpoint]);
      idpowerpoint++;
      return;
    }
    if (actvar == vars)
    {
      pSetExp(idpower[idpowerpoint],actvar,deg-monomdeg);
      pSetm(idpower[idpowerpoint]);
      pTest(idpower[idpowerpoint]);
      idpowerpoint++;
      return;
    }
    else
    {
      p = pCopy(idpower[idpowerpoint]);
      makemonoms(vars,actvar+1,deg,monomdeg);
      idpower[idpowerpoint] = p;
    }
    monomdeg++;
    pSetExp(idpower[idpowerpoint],actvar,pGetExp(idpower[idpowerpoint],actvar)+1);
    pSetm(idpower[idpowerpoint]);
    pTest(idpower[idpowerpoint]);
    i++;
  }
}

/*2
*returns the deg-th power of the maximal ideal of 0
*/
ideal idMaxIdeal(int deg)
{
  if (deg < 0)
  {
    WarnS("maxideal: power must be non-negative");
  }
  if (deg < 1)
  {
    ideal I=idInit(1,1);
    I->m[0]=pOne();
    return I;
  }
  if (deg == 1)
  {
    return idMaxIdeal();
  }

  int vars = currRing->N;
  int i = binom(vars+deg-1,deg);
  if (i<=0) return idInit(1,1);
  ideal id=idInit(i,1);
  idpower = id->m;
  idpowerpoint = 0;
  makemonoms(vars,1,deg,0);
  idpower = NULL;
  idpowerpoint = 0;
  return id;
}

/*2
*computes recursively all generators of a certain degree
*of the ideal "givenideal"
*elms is the number elements in the given ideal
*actelm is the actual element to handle
*deg is the degree of the power to compute
*gendeg is the actual degree of the generator in consideration
*/
static void makepotence(int elms,int actelm,int deg,int gendeg)
{
  poly p;
  int i=0;

  if ((idpowerpoint == 0) && (actelm ==1))
  {
    idpower[idpowerpoint] = pOne();
    gendeg = 0;
  }
  while (i<=deg)
  {
    if (deg == gendeg)
    {
      idpowerpoint++;
      return;
    }
    if (actelm == elms)
    {
      p=pPower(pCopy(givenideal[actelm-1]),deg-gendeg);
      idpower[idpowerpoint]=pMult(idpower[idpowerpoint],p);
      idpowerpoint++;
      return;
    }
    else
    {
      p = pCopy(idpower[idpowerpoint]);
      makepotence(elms,actelm+1,deg,gendeg);
      idpower[idpowerpoint] = p;
    }
    gendeg++;
    idpower[idpowerpoint]=pMult(idpower[idpowerpoint],pCopy(givenideal[actelm-1]));
    i++;
  }
}

/*2
*returns the deg-th power of the ideal gid
*/
//ideal idPower(ideal gid,int deg)
//{
//  int i;
//  ideal id;
//
//  if (deg < 1) deg = 1;
//  i = binom(IDELEMS(gid)+deg-1,deg);
//  id=idInit(i,1);
//  idpower = id->m;
//  givenideal = gid->m;
//  idpowerpoint = 0;
//  makepotence(IDELEMS(gid),1,deg,0);
//  idpower = NULL;
//  givenideal = NULL;
//  idpowerpoint = 0;
//  return id;
//}
static void idNextPotence(ideal given, ideal result,
  int begin, int end, int deg, int restdeg, poly ap)
{
  poly p;
  int i;

  p = pPower(pCopy(given->m[begin]),restdeg);
  i = result->nrows;
  result->m[i] = pMult(pCopy(ap),p);
//PrintS(".");
  (result->nrows)++;
  if (result->nrows >= IDELEMS(result))
  {
    pEnlargeSet(&(result->m),IDELEMS(result),16);
    IDELEMS(result) += 16;
  }
  if (begin == end) return;
  for (i=restdeg-1;i>0;i--)
  {
    p = pPower(pCopy(given->m[begin]),i);
    p = pMult(pCopy(ap),p);
    idNextPotence(given, result, begin+1, end, deg, restdeg-i, p);
    pDelete(&p);
  }
  idNextPotence(given, result, begin+1, end, deg, restdeg, ap);
}

ideal idPower(ideal given,int exp)
{
  ideal result,temp;
  poly p1;
  int i;

  if (idIs0(given)) return idInit(1,1);
  temp = idCopy(given);
  idSkipZeroes(temp);
  i = binom(IDELEMS(temp)+exp-1,exp);
  result = idInit(i,1);
  result->nrows = 0;
//Print("ideal contains %d elements\n",i);
  p1=pOne();
  idNextPotence(temp,result,0,IDELEMS(temp)-1,exp,exp,p1);
  pDelete(&p1);
  idDelete(&temp);
  result->nrows = 1;
  idDelEquals(result);
  idSkipZeroes(result);
  return result;
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
    block1[1] = pVariables;
    if (origR->OrdSgn==1) ord[1] = ringorder_wp;
    else                  ord[1] = ringorder_ws;
    wv[1]=(int*)omAlloc0(pVariables*sizeof(int));
    double wNsqr = (double)2.0 / (double)pVariables;
    wFunctional = wFunctionalBuch;
    int  *x= (int * )omAlloc(2 * (pVariables + 1) * sizeof(int));
    int sl=IDELEMS(h1) - 1;
    wCall(h1->m, sl, x, wNsqr);
    for (sl = pVariables; sl!=0; sl--)
      wv[1][sl-1] = x[sl + pVariables + 1];
    omFreeSize((ADDRESS)x, 2 * (pVariables + 1) * sizeof(int));

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
  //pChangeRing(pVariables,currRing->OrdSgn,ord,block0,block1,wv);
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
    l=pVariables;
    while ((l>0) && (p_GetExp( hh->m[k],l,tmpR)*pGetExp(delVar,l)==0)) l--;
    if (l==0)
    {
      j++;
      if (j >= IDELEMS(h3))
      {
        pEnlargeSet(&(h3->m),IDELEMS(h3),16);
        IDELEMS(h3) += 16;
      }
      h3->m[j] = prMoveR( hh->m[k], tmpR);
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
        p = mpDetBareiss(tmp);
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
      p = mpDetBareiss(tmp);
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
  h = idMatrix2Module(mpCopy(a));
  bound = smExpBound(h,c,r,ar);
  idDelete(&h);
  tmpR=smRingChange(&origR,bound);
  b = mpNew(r,c);
  for (i=r*c-1;i>=0;i--)
  {
    if (a->m[i])
      b->m[i] = prCopyR(a->m[i],origR);
  }
  if (R!=NULL)
  {
    R = idrCopyR(R,origR);
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
  result = idrMoveR(result,tmpR);
  smKillModifiedRing(tmpR);
  idTest(result);
  return result;
}
#endif

/*2
*skips all zeroes and double elements, searches also for units
*/
void idCompactify(ideal id)
{
  int i,j;
  BOOLEAN b=FALSE;

  i = IDELEMS(id)-1;
  while ((! b) && (i>=0))
  {
    b=pIsUnit(id->m[i]);
    i--;
  }
  if (b)
  {
    for(i=IDELEMS(id)-1;i>=0;i--) pDelete(&id->m[i]);
    id->m[0]=pOne();
  }
  else
  {
    idDelMultiples(id);
  }
  idSkipZeroes(id);
}

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

/*2
* returns the ideals of initial terms
*/
ideal idHead(ideal h)
{
  ideal m = idInit(IDELEMS(h),h->rank);
  int i;

  for (i=IDELEMS(h)-1;i>=0; i--)
  {
    if (h->m[i]!=NULL) m->m[i]=pHead(h->m[i]);
  }
  return m;
}

ideal idHomogen(ideal h, int varnum)
{
  ideal m = idInit(IDELEMS(h),h->rank);
  int i;

  for (i=IDELEMS(h)-1;i>=0; i--)
  {
    m->m[i]=pHomogen(h->m[i],varnum);
  }
  return m;
}

/*------------------type conversions----------------*/
ideal idVec2Ideal(poly vec)
{
   ideal result=idInit(1,1);
   omFree((ADDRESS)result->m);
   result->m=NULL; // remove later
   pVec2Polys(vec, &(result->m), &(IDELEMS(result)));
   return result;
}

#define NEW_STUFF
#ifndef NEW_STUFF
// converts mat to module, destroys mat
ideal idMatrix2Module(matrix mat)
{
  int mc=MATCOLS(mat);
  int mr=MATROWS(mat);
  ideal result = idInit(si_max(mc,1),si_max(mr,1));
  int i,j;
  poly h;

  for(j=0;j<mc /*MATCOLS(mat)*/;j++) /* j is also index in result->m */
  {
    for (i=1;i<=mr /*MATROWS(mat)*/;i++)
    {
      h = MATELEM(mat,i,j+1);
      if (h!=NULL)
      {
        MATELEM(mat,i,j+1)=NULL;
        pSetCompP(h,i);
        result->m[j] = pAdd(result->m[j],h);
      }
    }
  }
  // obachman: need to clean this up
  idDelete((ideal*) &mat);
  return result;
}
#else

#include <kernel/sbuckets.h>

// converts mat to module, destroys mat
ideal idMatrix2Module(matrix mat)
{
  int mc=MATCOLS(mat);
  int mr=MATROWS(mat);
  ideal result = idInit(si_max(mc,1),si_max(mr,1));
  int i,j, l;
  poly h;
  poly p;
  sBucket_pt bucket = sBucketCreate(currRing);

  for(j=0;j<mc /*MATCOLS(mat)*/;j++) /* j is also index in result->m */
  {
    for (i=1;i<=mr /*MATROWS(mat)*/;i++)
    {
      h = MATELEM(mat,i,j+1);
      if (h!=NULL)
      {
        l=pLength(h);
        MATELEM(mat,i,j+1)=NULL;
        p_SetCompP(h,i, currRing);
        sBucket_Merge_p(bucket, h, l);
      }
    }
    sBucketClearMerge(bucket, &(result->m[j]), &l);
  }
  sBucketDestroy(&bucket);

  // obachman: need to clean this up
  idDelete((ideal*) &mat);
  return result;
}
#endif

/*2
* converts a module into a matrix, destroyes the input
*/
matrix idModule2Matrix(ideal mod)
{
  matrix result = mpNew(mod->rank,IDELEMS(mod));
  int i,cp;
  poly p,h;

  for(i=0;i<IDELEMS(mod);i++)
  {
    p=pReverse(mod->m[i]);
    mod->m[i]=NULL;
    while (p!=NULL)
    {
      h=p;
      pIter(p);
      pNext(h)=NULL;
//      cp = si_max(1,pGetComp(h));     // if used for ideals too
      cp = pGetComp(h);
      pSetComp(h,0);
      pSetmComp(h);
#ifdef TEST
      if (cp>mod->rank)
      {
        Print("## inv. rank %ld -> %d\n",mod->rank,cp);
        int k,l,o=mod->rank;
        mod->rank=cp;
        matrix d=mpNew(mod->rank,IDELEMS(mod));
        for (l=1; l<=o; l++)
        {
          for (k=1; k<=IDELEMS(mod); k++)
          {
            MATELEM(d,l,k)=MATELEM(result,l,k);
            MATELEM(result,l,k)=NULL;
          }
        }
        idDelete((ideal *)&result);
        result=d;
      }
#endif
      MATELEM(result,cp,i+1) = pAdd(MATELEM(result,cp,i+1),h);
    }
  }
  // obachman 10/99: added the following line, otherwise memory leack!
  idDelete(&mod);
  return result;
}

matrix idModule2formatedMatrix(ideal mod,int rows, int cols)
{
  matrix result = mpNew(rows,cols);
  int i,cp,r=idRankFreeModule(mod),c=IDELEMS(mod);
  poly p,h;

  if (r>rows) r = rows;
  if (c>cols) c = cols;
  for(i=0;i<c;i++)
  {
    p=pReverse(mod->m[i]);
    mod->m[i]=NULL;
    while (p!=NULL)
    {
      h=p;
      pIter(p);
      pNext(h)=NULL;
      cp = pGetComp(h);
      if (cp<=r)
      {
        pSetComp(h,0);
        pSetmComp(h);
        MATELEM(result,cp,i+1) = pAdd(MATELEM(result,cp,i+1),h);
      }
      else
        pDelete(&h);
    }
  }
  idDelete(&mod);
  return result;
}

/*2
* substitute the n-th variable by the monomial e in id
* destroy id
*/
ideal  idSubst(ideal id, int n, poly e)
{
  int k=MATROWS((matrix)id)*MATCOLS((matrix)id);
  ideal res=(ideal)mpNew(MATROWS((matrix)id),MATCOLS((matrix)id));

  res->rank = id->rank;
  for(k--;k>=0;k--)
  {
    res->m[k]=pSubst(id->m[k],n,e);
    id->m[k]=NULL;
  }
  idDelete(&id);
  return res;
}

BOOLEAN idHomModule(ideal m, ideal Q, intvec **w)
{
  if (w!=NULL) *w=NULL;
  if ((Q!=NULL) && (!idHomIdeal(Q,NULL))) return FALSE;
  if (idIs0(m))
  {
    if (w!=NULL) (*w)=new intvec(m->rank);
    return TRUE;
  }

  long cmax=1,order=0,ord,* diff,diffmin=32000;
  int *iscom;
  int i,j;
  poly p=NULL;
  pFDegProc d;
  if (pLexOrder && (currRing->order[0]==ringorder_lp))
     d=p_Totaldegree;
  else
     d=pFDeg;
  int length=IDELEMS(m);
  polyset P=m->m;
  polyset F=(polyset)omAlloc(length*sizeof(poly));
  for (i=length-1;i>=0;i--)
  {
    p=F[i]=P[i];
    cmax=si_max(cmax,(long)pMaxComp(p));
  }
  cmax++;
  diff = (long *)omAlloc0(cmax*sizeof(long));
  if (w!=NULL) *w=new intvec(cmax-1);
  iscom = (int *)omAlloc0(cmax*sizeof(int));
  i=0;
  while (i<=length)
  {
    if (i<length)
    {
      p=F[i];
      while ((p!=NULL) && (iscom[pGetComp(p)]==0)) pIter(p);
    }
    if ((p==NULL) && (i<length))
    {
      i++;
    }
    else
    {
      if (p==NULL) /* && (i==length) */
      {
        i=0;
        while ((i<length) && (F[i]==NULL)) i++;
        if (i>=length) break;
        p = F[i];
      }
      //if (pLexOrder && (currRing->order[0]==ringorder_lp))
      //  order=pTotaldegree(p);
      //else
      //  order = p->order;
      //  order = pFDeg(p,currRing);
      order = d(p,currRing) +diff[pGetComp(p)];
      //order += diff[pGetComp(p)];
      p = F[i];
//Print("Actual p=F[%d]: ",i);pWrite(p);
      F[i] = NULL;
      i=0;
    }
    while (p!=NULL)
    {
      if (pLexOrder && (currRing->order[0]==ringorder_lp))
        ord=pTotaldegree(p);
      else
      //  ord = p->order;
        ord = pFDeg(p,currRing);
      if (iscom[pGetComp(p)]==0)
      {
        diff[pGetComp(p)] = order-ord;
        iscom[pGetComp(p)] = 1;
/*
*PrintS("new diff: ");
*for (j=0;j<cmax;j++) Print("%d ",diff[j]);
*PrintLn();
*PrintS("new iscom: ");
*for (j=0;j<cmax;j++) Print("%d ",iscom[j]);
*PrintLn();
*Print("new set %d, order %d, ord %d, diff %d\n",pGetComp(p),order,ord,diff[pGetComp(p)]);
*/
      }
      else
      {
/*
*PrintS("new diff: ");
*for (j=0;j<cmax;j++) Print("%d ",diff[j]);
*PrintLn();
*Print("order %d, ord %d, diff %d\n",order,ord,diff[pGetComp(p)]);
*/
        if (order != (ord+diff[pGetComp(p)]))
        {
          omFreeSize((ADDRESS) iscom,cmax*sizeof(int));
          omFreeSize((ADDRESS) diff,cmax*sizeof(long));
          omFreeSize((ADDRESS) F,length*sizeof(poly));
          delete *w;*w=NULL;
          return FALSE;
        }
      }
      pIter(p);
    }
  }
  omFreeSize((ADDRESS) iscom,cmax*sizeof(int));
  omFreeSize((ADDRESS) F,length*sizeof(poly));
  for (i=1;i<cmax;i++) (**w)[i-1]=(int)(diff[i]);
  for (i=1;i<cmax;i++)
  {
    if (diff[i]<diffmin) diffmin=diff[i];
  }
  if (w!=NULL)
  {
    for (i=1;i<cmax;i++)
    {
      (**w)[i-1]=(int)(diff[i]-diffmin);
    }
  }
  omFreeSize((ADDRESS) diff,cmax*sizeof(long));
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
      int d=pFDeg(p,currRing);
      loop
      {
        pIter(p);
        if (p==NULL) break;
        if (d!=pFDeg(p,currRing))
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

ideal idJet(ideal i,int d)
{
  ideal r=idInit((i->nrows)*(i->ncols),i->rank);
  r->nrows = i-> nrows;
  r->ncols = i-> ncols;
  //r->rank = i-> rank;
  int k;
  for(k=(i->nrows)*(i->ncols)-1;k>=0; k--)
  {
    r->m[k]=ppJet(i->m[k],d);
  }
  return r;
}

ideal idJetW(ideal i,int d, intvec * iv)
{
  ideal r=idInit(IDELEMS(i),i->rank);
  if (ecartWeights!=NULL)
  {
    WerrorS("cannot compute weighted jets now");
  }
  else
  {
    short *w=iv2array(iv);
    int k;
    for(k=0; k<IDELEMS(i); k++)
    {
      r->m[k]=ppJetW(i->m[k],d,w);
    }
    omFreeSize((ADDRESS)w,(pVariables+1)*sizeof(short));
  }
  return r;
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
  rSetSyzComp(length);

  ideal s_temp;
  if (orig_ring!=syz_ring)
    s_temp=idrMoveR_NoSort(arg,orig_ring);
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
      pShift(&(temp1->m[i]),-length);
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
    flength = idRankFreeModule(h1);
  slength = idRankFreeModule(h2);
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
      if (slength==0) pShift(&(temp->m[i]),1);
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
        if (flength==0) pShift(&(temp->m[k]),1);
        k++;
      }
    }
  }

  ring orig_ring=currRing;
  ring syz_ring=rCurrRingAssure_SyzComp();
  rSetSyzComp(length);
  ideal s_temp;

  if (syz_ring != orig_ring)
  {
    s_temp = idrMoveR_NoSort(temp, orig_ring);
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
      pShift(&(s_temp1->m[i]),-length);
    }
  }
  s_temp1->rank = rk;
  idSkipZeroes(s_temp1);

  if (syz_ring!=orig_ring)
  {
    rChangeCurrRing(orig_ring);
    s_temp1 = idrMoveR_NoSort(s_temp1, syz_ring);
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

int idElem(const ideal F)
{
  int i=0,j=IDELEMS(F)-1;

  while(j>=0)
  {
    if ((F->m)[j]!=NULL) i++;
    j--;
  }
  return i;
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
    (*result)[i]=pFDeg(mod->m[i],currRing)+(*weights)[pGetComp(mod->m[i])];
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
  int i=pVariables;
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

  for (i=1;i<=pVariables;i++)
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

/*3
* searches for units in the components of the module arg and
* returns the first one
*/
static int idReadOutUnits(ideal arg,int* comp)
{
  if (idIs0(arg)) return -1;
  int i=0,j, generator=-1;
  int rk_arg=arg->rank; //idRankFreeModule(arg);
  int * componentIsUsed =(int *)omAlloc((rk_arg+1)*sizeof(int));
  poly p,q;

  while ((generator<0) && (i<IDELEMS(arg)))
  {
    memset(componentIsUsed,0,(rk_arg+1)*sizeof(int));
    p = arg->m[i];
    while (p!=NULL)
    {
      j = pGetComp(p);
      if (componentIsUsed[j]==0)
      {
        if (pLmIsConstantComp(p))
        {
          generator = i;
          componentIsUsed[j] = 1;
        }
        else
        {
          componentIsUsed[j] = -1;
        }
      }
      else if (componentIsUsed[j]>0)
      {
        (componentIsUsed[j])++;
      }
      pIter(p);
    }
    i++;
  }
  i = 0;
  *comp = -1;
  for (j=0;j<=rk_arg;j++)
  {
    if (componentIsUsed[j]>0)
    {
      if ((*comp==-1) || (componentIsUsed[j]<i))
      {
        *comp = j;
        i= componentIsUsed[j];
      }
    }
  }
  omFree(componentIsUsed);
  return generator;
}

#if 0
static void idDeleteComp(ideal arg,int red_comp)
{
  int i,j;
  poly p;

  for (i=IDELEMS(arg)-1;i>=0;i--)
  {
    p = arg->m[i];
    while (p!=NULL)
    {
      j = pGetComp(p);
      if (j>red_comp)
      {
        pSetComp(p,j-1);
        pSetm(p);
      }
      pIter(p);
    }
  }
  (arg->rank)--;
}
#endif

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
  res->rank=si_max(res->rank,idRankFreeModule(res));
  int *red_comp=(int*)omAlloc((res->rank+1)*sizeof(int));
  for (i=res->rank;i>=0;i--) red_comp[i]=i;

  int del=0;
  loop
  {
    next_gen = idReadOutUnits(res,&next_comp);
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

/*2
* transpose a module
*/
ideal idTransp(ideal a)
{
  int r = a->rank, c = IDELEMS(a);
  ideal b =  idInit(r,c);

  for (int i=c; i>0; i--)
  {
    poly p=a->m[i-1];
    while(p!=NULL)
    {
      poly h=pHead(p);
      int co=pGetComp(h)-1;
      pSetComp(h,i);
      pSetmComp(h);
      b->m[co]=pAdd(b->m[co],h);
      pIter(p);
    }
  }
  return b;
}

intvec * idQHomWeight(ideal id)
{
  poly head, tail;
  int k;
  int in=IDELEMS(id)-1, ready=0, all=0,
      coldim=pVariables, rowmax=2*coldim;
  if (in<0) return NULL;
  intvec *imat=new intvec(rowmax+1,coldim,0);

  do
  {
    head = id->m[in--];
    if (head!=NULL)
    {
      tail = pNext(head);
      while (tail!=NULL)
      {
        all++;
        for (k=1;k<=coldim;k++)
          IMATELEM(*imat,all,k) = pGetExpDiff(head,tail,k);
        if (all==rowmax)
        {
          ivTriangIntern(imat, ready, all);
          if (ready==coldim)
          {
            delete imat;
            return NULL;
          }
        }
        pIter(tail);
      }
    }
  } while (in>=0);
  if (all>ready)
  {
    ivTriangIntern(imat, ready, all);
    if (ready==coldim)
    {
      delete imat;
      return NULL;
    }
  }
  intvec *result = ivSolveKern(imat, ready);
  delete imat;
  return result;
}

BOOLEAN idIsZeroDim(ideal I)
{
  BOOLEAN *UsedAxis=(BOOLEAN *)omAlloc0(pVariables*sizeof(BOOLEAN));
  int i,n;
  poly po;
  BOOLEAN res=TRUE;
  for(i=IDELEMS(I)-1;i>=0;i--)
  {
    po=I->m[i];
    if ((po!=NULL) &&((n=pIsPurePower(po))!=0)) UsedAxis[n-1]=TRUE;
  }
  for(i=pVariables-1;i>=0;i--)
  {
    if(UsedAxis[i]==FALSE) {res=FALSE; break;} // not zero-dim.
  }
  omFreeSize(UsedAxis,pVariables*sizeof(BOOLEAN));
  return res;
}

void idNormalize(ideal I)
{
  if (rField_has_simple_inverse()) return; /* Z/p, GF(p,n), R, long R/C */
  int i;
  poly p;
  for(i=IDELEMS(I)-1;i>=0;i--)
  {
    p=I->m[i] ;
    while(p!=NULL)
    {
      nNormalize(pGetCoeff(p));
      pIter(p);
    }
  }
}

#include <kernel/clapsing.h>

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
  poly gcd_p=singclap_pdivide(f,gg);
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

/*2
* transpose a module
* NOTE: just a version of "ideal idTransp(ideal)" which works within specified ring.
*/
ideal id_Transp(ideal a, const ring rRing)
{
  int r = a->rank, c = IDELEMS(a);
  ideal b =  idInit(r,c);

  for (int i=c; i>0; i--)
  {
    poly p=a->m[i-1];
    while(p!=NULL)
    {
      poly h=p_Head(p, rRing);
      int co=p_GetComp(h, rRing)-1;
      p_SetComp(h, i, rRing);
      p_Setm(h, rRing);
      b->m[co] = p_Add_q(b->m[co], h, rRing);
      pIter(p);
    }
  }
  return b;
}



/*2
* The following is needed to compute the image of certain map used in
* the computation of cohomologies via BGG
* let M = { w_1, ..., w_k }, k = size(M) == ncols(M), n = nvars(currRing).
* assuming that nrows(M) <= m*n; the procedure computes:
* transpose(M) * transpose( var(1) I_m | ... | var(n) I_m ) :== transpose(module{f_1, ... f_k}),
* where f_i = \sum_{j=1}^{m} (w_i, v_j) gen(j),  (w_i, v_j) is a `scalar` multiplication.
* that is, if w_i = (a^1_1, ... a^1_m) | (a^2_1, ..., a^2_m) | ... | (a^n_1, ..., a^n_m) then

  (a^1_1, ... a^1_m) | (a^2_1, ..., a^2_m) | ... | (a^n_1, ..., a^n_m)
*  var_1  ... var_1  |  var_2  ...  var_2  | ... |  var_n  ...  var(n)
*  gen_1  ... gen_m  |  gen_1  ...  gen_m  | ... |  gen_1  ...  gen_m
+ =>
  f_i =

   a^1_1 * var(1) * gen(1) + ... + a^1_m * var(1) * gen(m) +
   a^2_1 * var(2) * gen(1) + ... + a^2_m * var(2) * gen(m) +
                             ...
   a^n_1 * var(n) * gen(1) + ... + a^n_m * var(n) * gen(m);

   NOTE: for every f_i we run only ONCE along w_i saving partial sums into a temporary array of polys of size m
*/
ideal id_TensorModuleMult(const int m, const ideal M, const ring rRing)
{
// #ifdef DEBU
//  WarnS("tensorModuleMult!!!!");

  assume(m > 0);
  assume(M != NULL);

  const int n = rRing->N;

  assume(M->rank <= m * n);

  const int k = IDELEMS(M);

  ideal idTemp =  idInit(k,m); // = {f_1, ..., f_k }

  for( int i = 0; i < k; i++ ) // for every w \in M
  {
    poly pTempSum = NULL;

    poly w = M->m[i];

    while(w != NULL) // for each term of w...
    {
      poly h = p_Head(w, rRing);

      const int  gen = p_GetComp(h, rRing); // 1 ...

      assume(gen > 0);
      assume(gen <= n*m);

      // TODO: write a formula with %, / instead of while!
      /*
      int c = gen;
      int v = 1;
      while(c > m)
      {
        c -= m;
        v++;
      }
      */

      int cc = gen % m;
      if( cc == 0) cc = m;
      int vv = 1 + (gen - cc) / m;

//      assume( cc == c );
//      assume( vv == v );

      //  1<= c <= m
      assume( cc > 0 );
      assume( cc <= m );

      assume( vv > 0 );
      assume( vv <= n );

      assume( (cc + (vv-1)*m) == gen );

      p_IncrExp(h, vv, rRing); // h *= var(j) && //      p_AddExp(h, vv, 1, rRing);
      p_SetComp(h, cc, rRing);

      p_Setm(h, rRing);         // addjust degree after the previous steps!

      pTempSum = p_Add_q(pTempSum, h, rRing); // it is slow since h will be usually put to the back of pTempSum!!!

      pIter(w);
    }

    idTemp->m[i] = pTempSum;
  }

  // simplify idTemp???

  ideal idResult = id_Transp(idTemp, rRing);

  id_Delete(&idTemp, rRing);

  return(idResult);
}

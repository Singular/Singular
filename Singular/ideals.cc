/****************************************
*  Computer Algebra System SINGULAR     *
****************************************/
/* $Id: ideals.cc,v 1.23 1998-04-29 07:05:25 siebert Exp $ */
/*
* ABSTRACT - all basic methods to manipulate ideals
*/

/* includes */
#include "mod2.h"
#include "tok.h"
#include "mmemory.h"
#include "febase.h"
#include "numbers.h"
#include "polys.h"
#include "ipid.h"
#include "ring.h"
#include "kstd1.h"
#include "matpol.h"
#include "weight.h"
#include "intvec.h"
#include "syz.h"
#include "ideals.h"
#include "lists.h"


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
#ifdef MDEBUG
ideal idDBInit(int idsize, int rank, char *f, int l)
#else
ideal idInit(int idsize, int rank)
#endif
{
  /*- initialise an ideal -*/
#ifdef MDEBUG
  ideal hh = (ideal )mmDBAllocBlock(sizeof(*hh),f,l);
#else
  ideal hh = (ideal )Alloc(sizeof(*hh));
#endif
  hh->nrows = 1;
  hh->rank = rank;
  IDELEMS(hh) = idsize;
  if (idsize>0)
  {
#ifdef MDEBUG
    hh->m = (poly *)mmDBAllocBlock0(idsize*sizeof(poly),f,l);
#else
    hh->m = (poly *)Alloc0(idsize*sizeof(poly));
#endif
  }
  else
    hh->m=NULL;
  return hh;
}

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
#ifdef MDEBUG
void idDBDelete (ideal* h, char *f, int l)
#else
void idDelete (ideal * h)
#endif
{
  int j,elems;
  if (*h == NULL)
    return;
  elems=j=(*h)->nrows*(*h)->ncols;
  if (j>0)
  {
    do
    {
      #ifdef MDEBUG
      pDBDelete(&((*h)->m[--j]),f,l);
      #else
      pDelete(&((*h)->m[--j]));
      #endif
    }
    while (j>0);
    #ifdef MDEBUG
    mmDBFreeBlock((ADDRESS)((*h)->m),sizeof(poly)*elems,f,l);
    #else
    Free((ADDRESS)((*h)->m),sizeof(poly)*elems);
    #endif
  }
  #ifdef MDEBUG
  mmDBFreeBlock((ADDRESS)(*h),sizeof(**h),f,l);
  #else
  Free((ADDRESS)*h,sizeof(**h));
  #endif
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
* ideal id = (id[i])
* result is leadcoeff(id[i]) = 1
*/
void idNorm(ideal id)
{
  for (int i=0; i<IDELEMS(id); i++)
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
  int i, j, t;
  int k = IDELEMS(id), l = k;
  for (i=k-2; i>=0; i--)
  {
    if (id->m[i]!=NULL)
    {
      for (j=l-1; j>i; j--)
      {
        if ((id->m[j]!=NULL)
        && (pComparePolys(id->m[i], id->m[j])))
        {
          pDelete(&id->m[j]);
          l--;
          for(t=j; t<l; t++)
          {
            id->m[t] = id->m[t+1];
          }
        }
      }
    }
  }
  if (l != k)
  {
    pEnlargeSet(&id->m, k, l-k);
    IDELEMS(id) = l;
  }
}

/*2
* ideal id = (id[i])
* if id[i] = id[j] then id[j] is deleted for j > i
*/
void idDelEquals(ideal id)
{
  int i, j, t;
  int k = IDELEMS(id), l = k;
  for (i=k-2; i>=0; i--)
  {
    for (j=l-1; j>i; j--)
    {
      if (pEqualPolys(id->m[i], id->m[j]))
      {
        pDelete(&id->m[j]);
        l--;
        for(t=j; t<l; t++)
        {
          id->m[t] = id->m[t+1];
        }
      }
    }
  }
  if (l != k)
  {
    pEnlargeSet(&id->m, k, l-k);
    IDELEMS(id) = l;
  }
}

/*2
* copy an ideal
*/
#ifdef MDEBUG
ideal idDBCopy(ideal h1,char *f,int l)
#else
ideal idCopy (ideal h1)
#endif
{
  int i;
  ideal h2;

//#ifdef TEST
  if (h1 == NULL)
  {
#ifdef MDEBUG
    h2=idDBInit(1,1,f,l);
#else
    h2=idInit(1,1);
#endif
  }
  else
//#endif
  {
#ifdef MDEBUG
    h2=idDBInit(IDELEMS(h1),h1->rank,f,l);
#else
    h2=idInit(IDELEMS(h1),h1->rank);
#endif
    for (i=IDELEMS(h1)-1; i>=0; i--)
#ifdef MDEBUG
      h2->m[i] = pDBCopy(h1->m[i],f,l);
#else
      h2->m[i] = pCopy(h1->m[i]);
#endif
  }
  return h2;
}

#ifdef PDEBUG
void idDBTest(ideal h1,char *f,int l)
{
  int i;

  if (h1 != NULL)
  {
    #ifdef MDEBUG
    mmDBTestBlock(h1,sizeof(*h1),f,l);
    #endif
    for (i=IDELEMS(h1)-1; i>=0; i--)
      pDBTest(h1->m[i],f,l);
  }
}
#endif

/*3
* for idSort: compare a and b revlex inclusive module comp.
*/
static int pComp_RevLex(poly a, poly b)
{
 int l=pVariables;
 while ((l>=0) && (pGetExp(a,l)==pGetExp(b,l))) l--;
 if (l<0)
   return 0;
 else if (pGetExp(a,l)>pGetExp(b,l))
   return 1;
 else
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

  pCompProc oldComp=pComp0;

  if (!nolex) pComp0=pComp_RevLex;

  for (i=0;i<IDELEMS(id);i++)
  {
    if (id->m[i]!=NULL)
    {
      notFound = TRUE;
      newpos = actpos / 2;
      diff = (actpos+1) / 2;
      diff = (diff+1) / 2;
      lastcomp = pComp0(id->m[i],id->m[(*result)[newpos]]);
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
      while ((newpos>=0) && (newpos<actpos) && (notFound))
      {
        newcomp = pComp0(id->m[i],id->m[(*result)[newpos]]);
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
      if (newpos>=actpos)
      {
        (*result)[actpos] = i;
      }
      else
      {
        for (j=actpos;j>newpos;j--)
        {
          (*result)[j] = (*result)[j-1];
        }
        (*result)[newpos] = i;
      }
      actpos++;
    }
  }
  for (j=0;j<actpos;j++) (*result)[j]++;
  pComp0=oldComp;
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
  r = max(h1->rank,h2->rank);
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
* h1 + h2
*/
ideal idAdd (ideal h1,ideal h2)
{
  ideal result = idSimpleAdd(h1,h2);
  ideal tmp = idCompactify(result);

  idDelete(&result);
  return tmp;
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
          hh->m[k] = pMult(pCopy(h1->m[i]),pCopy(h2->m[j]));
          k++;
        }
      }
    }
  }
  {
    ideal tmp = idCompactify(hh);
    idDelete(&hh);
    return tmp;
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
  i = IDELEMS(h);
  while ((i > 0) && (h->m[i-1] == NULL))
  {
    i--;
  }
  if (i == 0)
    return TRUE;
  else
    return FALSE;
}

/*2
* return the maximal component number found in any polynomial in s
*/
int idRankFreeModule (ideal s)
{
  if (s!=NULL)
  {
    int  j=0;
    int  l=IDELEMS(s);
    poly *p=s->m;
    int  k;

    for (; l != 0; l--)
    {
      if (*p!=NULL)
      {
        k = pMaxComp(*p);
        if (k>j) j = k;
      }
      p++;
    }
    return j;
  }
  return -1;
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
  if ((currRing->OrdSgn == 1) && (!homog))
  {
    Warn("minbase applies only to the local or homogeneous case");
    e=idCopy(h1);
    return e;
  }
  if ((currRing->OrdSgn == 1) && (homog))
  {
    lists re=min_std(h1,currQuotient,(tHomog)homog,&wth,NULL,0,2);
    h2 = (ideal)re->m[1].data;
    re->m[1].data = NULL;
    re->m[1].rtyp = NONE;
    re->Clean();
    return h2;
  }
  e=idInit(1,h1->rank);
  if (idIs0(h1))
  {
    return e;
  }
  pEnlargeSet(&(e->m),IDELEMS(e),15);
  IDELEMS(e) = 16;
  h2 = std(h1,currQuotient,isNotHomog,NULL);
  h3 = idMaxIdeal();
  h4=idMult(h2,h3);
  idDelete(&h3);
  h3=std(h4,currQuotient,isNotHomog,NULL);
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
    while ((l <= pVariables) && (lex == 0))
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

/*3
*deletes the place of t in p (t: variable with index 1)
*p is NOT in the actual ring: it has pVariables+1 variables
*/
static poly pDivByT (poly * p,int size)
{

  poly result=NULL,
       resultp=NULL , /** working pointer in result **/
       pp;
  int  i,j;

  while (*p != NULL)
  {
    i = 0;
    if (result == NULL)
    {/*the first monomial*/
      result = pInit();
      resultp = result;
      resultp->next = NULL;
    }
    else
    {
      resultp->next = pInit();
      pIter(resultp);
      resultp->next = NULL;
    }
    for (j=1; j<=pVariables; j++)
    {
      pSetExp(resultp,j,pGetExp(*p,j+1));
    }
    pSetComp(resultp,pGetComp(*p));
    pSetCoeff0(resultp,pGetCoeff(*p));
    pSetm(resultp);
    pp = (*p)->next;
    Free((ADDRESS)*p,size);
    *p = pp;
  }
  return result;
}

/*2
*dehomogenized the generators of the ideal id1 with the leading
*monomial of p replaced by n
*/
ideal idDehomogen (ideal id1,poly p,number n)
{
  int i;
  ideal result;

  if (idIs0(id1))
  {
    return idInit(1,id1->rank);
  }
  result=idInit(IDELEMS(id1),id1->rank);
  for (i=0; i<IDELEMS(id1); i++)
  {
    result->m[i] = pDehomogen(id1->m[i],p,n);
  }
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
      pSetComp(qp1,pGetComp(qp1)+i);
      qp2 = qp1;
      pIter(qp1);
    }
    else
    {
      if (qp2 == *p)
      {
        pIter(*p);
        qp2->next = NULL;
        pDelete(&qp2);
        qp2 = *p;
        qp1 = *p;
      }
      else
      {
        qp2->next = qp1->next;
        qp1->next = NULL;
        pDelete(&qp1);
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
  localchoise=(int*)Alloc((d-1)*sizeof(int));
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
        Free((ADDRESS)localchoise,(d-1)*sizeof(int));
        return result;
      }
    }
    idGetNextChoise(d-1,end,&b,localchoise);
  }
  Free((ADDRESS)localchoise,(d-1)*sizeof(int));
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
  }
  return h;
}

/*2
* h3 := h1 intersect h2
*/
ideal idSect (ideal h1,ideal h2)
{
  ideal first=h2,second=h1,temp,temp1,result;
  int i,j,k,flength,slength,length,rank=min(h1->rank,h2->rank);
  intvec *w;
  poly p,q;

  if ((idIs0(h1)) && (idIs0(h2)))  return idInit(1,rank);
  if (IDELEMS(h1)<IDELEMS(h2))
  {
    first = h1;
    second = h2;
  }
  flength = idRankFreeModule(first);
  slength = idRankFreeModule(second);
  length  = max(flength,slength);
  if (length==0)
  {
    length = 1;
  }
  temp = idInit(IDELEMS(first),1);
  j = IDELEMS(temp);
  while ((j>0) && (first->m[j-1]==NULL)) j--;
  k = 0;
  for (i=0;i<j;i++)
  {
    if (first->m[i]!=NULL)
    {
      temp->m[k] = pCopy(first->m[i]);
      q = pOne();
      pSetComp(q,i+1+length);
      if (flength==0) pShift(&(temp->m[k]),1);
      p = temp->m[k];
      while (pNext(p)) pIter(p);
      pNext(p) = q;
      k++;
    }
  }
  pEnlargeSet(&(temp->m),IDELEMS(temp),j+IDELEMS(second)-IDELEMS(temp));
  IDELEMS(temp) = j+IDELEMS(second);
  for (i=0;i<IDELEMS(second);i++)
  {
    if (second->m[i]!=NULL)
    {
      temp->m[k] = pCopy(second->m[i]);
      if (slength==0) pShift(&(temp->m[k]),1);
      k++;
    }
  }
  pSetSyzComp(length);
  temp1 = std(temp,currQuotient,testHomog,&w,NULL,length);
  if (w!=NULL) delete w;
  pSetSyzComp(0);
  idDelete(&temp);
  result = idInit(IDELEMS(temp1),rank);
  j = 0;
  for (i=0;i<IDELEMS(temp1);i++)
  {
    if ((temp1->m[i]!=NULL)
    && (pGetComp(temp1->m[i])>length))
    {
      p = temp1->m[i];
//PrintS("die Syzygie ist: ");pWrite(p);
      temp1->m[i] = NULL;
      while (p!=NULL)
      {
        q = pNext(p);
        pNext(p) = NULL;
        k = pGetComp(p)-1-length;
        pSetComp(p,0);
//PrintS("das %d-te Element: ",k);pWrite(first->m[k]);
        result->m[j] = pAdd(result->m[j],pMult(pCopy(first->m[k]),p));
        p = q;
      }
//PrintS("Generator ist: ");pWrite(result->m[j]);
      j++;
    }
  }
  idSkipZeroes(result);
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
  bigmat = idInit(j,(k+1)*maxrk);
  syzComp = k*maxrk;
  pSetSyzComp(syzComp);
  /* create unit matrices ------------------------------------------*/
  for (i=0;i<maxrk;i++)
  {
    for (j=0;j<=k;j++)
    {
      p = pOne();
      pSetComp(p,i+1+j*maxrk);
      pSetm(p);
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
          bigmat->m[i] = pCopy(arg[j]->m[l]);
          pShift(&(bigmat->m[i]),k*maxrk+isIdeal);
          i++;
        }
      }
      k++;
    }
  }
  /* std computation --------------------------------------------*/
  tempstd = std(bigmat,currQuotient,testHomog,&w,NULL,syzComp);
  if (w!=NULL) delete w;
  idDelete(&bigmat);
  pSetSyzComp(0);
  /* interprete result ----------------------------------------*/
  result = idInit(8,maxrk);
  k = 0;
  for (j=0;j<IDELEMS(tempstd);j++)
  {
    if ((tempstd->m[j]!=NULL) && (pGetComp(tempstd->m[j])>syzComp))
    {
      if (k>=IDELEMS(result))
      {
        pEnlargeSet(&(result->m),IDELEMS(result),8);
        IDELEMS(result) += 8;
      }
      p = tempstd->m[j];
      tempstd->m[j] = NULL;
      pShift(&p,-syzComp-isIdeal);
      result->m[k] = p;
      k++;
    }
  }
  /* clean up ----------------------------------------------------*/
  idDelete(&tempstd);
  idSkipZeroes(result);
  return result;
}

/*2
*computes the rank of the free module in which h1 embeds
*/
int lengthFreeModule (ideal  h1)
{
  int     i,j,k;

  if (idIs0(h1)) return 0;
  k = -1;
  for (i=0; i<IDELEMS(h1); i++)
  {
    j = pMaxComp(h1->m[i]);
    if (j>k) k = j;
  }
  return k;
}

/*2
*computes syzygies of h1,
*if quot != NULL it computes in the quotient ring modulo "quot"
*/
ideal  idPrepare (ideal  h1,ideal  quot, tHomog h,
  int* syzcomp, int *quotgen, int *quotdim, intvec **w)
{
  ideal   h2, h3;
  int     i;
  int     j,k;
  poly    p,q;
  BOOLEAN orderChanged=FALSE;

  if (idIs0(h1)) return NULL;
  k = lengthFreeModule(h1);
  if (*syzcomp<k) *syzcomp = k;
  h2 = NULL;
  h2=idCopy(h1);
  i = IDELEMS(h2)-1;
  //while ((i >= 0) && (h2->m[i] == NULL)) i--;
  if (k == 0)
  {
    for (j=0; j<=i; j++) pShift(&(h2->m[j]),1);
    *syzcomp = 1;
  }
  h2->rank = *syzcomp+i+1;
  for (j=0; j<=i; j++)
  {
    p = h2->m[j];
    q = pOne();
#ifdef DRING
    if (pDRING) { pdDFlag(q)=1; pSetm(q); }
#endif
    pSetComp(q,*syzcomp+1+j);
    if (p!=NULL)
    {
      while (pNext(p)) pIter(p);
      p->next = q;
    }
    else
      h2->m[j]=q;
  }
  if ((pOrdSgn!=1) || (h!=TRUE) || (*syzcomp>1)|| (pLexOrder))
  {
    pSetSyzComp(*syzcomp);
    orderChanged=TRUE;
  }
  else
  {
    for(j=0;(j<IDELEMS(h2) && (!orderChanged));j++)
    {
      if (h2->m[j] != NULL)
      {
        p=h2->m[j];
        while ((p!=NULL)
        && (pGetComp(p)<=*syzcomp))
        {
          if (pIsConstantComp(p))
          {
            pSetSyzComp(*syzcomp);
            orderChanged=TRUE;
            break;
          }
          pIter(p);
        }
      }
    }
  }
//  if (orderChanged) PrintS("order changed\n");
//  else PrintS("order not changed\n");
#ifdef PDEBUG
  for(j=0;j<IDELEMS(h2);j++) pTest(h2->m[j]);
#endif
  h3=std(h2,quot,h,w,NULL,*syzcomp);
  //h3->rank = h2->rank; done by std -> initBuchMora -> initS
  h3->rank-=*syzcomp;
  idDelete(&h2);
  if (orderChanged) pSetSyzComp(0);
  return h3;
}

ideal idSyzygies (ideal  h1,ideal  quot, tHomog h,intvec **w)
{
  int d;
  return idSyzygies(h1,quot,h,w,FALSE,d);
}

ideal idSyzygies (ideal  h1,ideal  quot, tHomog h,intvec **w,
                  BOOLEAN setRegularity, int &deg)
{
  ideal e, h3;
  poly  p;
  int   i, j, k=0, quotdim, quotgen,length=0,reg;
  BOOLEAN isMonomial=TRUE;

#ifdef PDEBUG
  int ii;
  for(ii=0;ii<IDELEMS(h1);ii++) pTest(h1->m[ii]);
  if (quot!=NULL)
  {
    for(ii=0;ii<IDELEMS(quot);ii++) pTest(quot->m[ii]);
  }
#endif
  if (idIs0(h1))
    return idFreeModule(IDELEMS(h1));
  h3=idPrepare(h1,quot,h,&k,&quotgen,&quotdim,w);
  if (h3==NULL)
    return idFreeModule(IDELEMS(h1));
  i = -1;
  e=idInit(16,h3->rank);
  for (j=0; j<IDELEMS(h3); j++)
  {
    if (h3->m[j] != NULL)
    {
      if (pMinComp(h3->m[j]) > k)
      {
        p = h3->m[j];
        h3->m[j]=NULL;
        pShift(&p,-k);
        if (p!=NULL)
        {
          i++;
          if (i+1 >= IDELEMS(e))
          {
            pEnlargeSet(&(e->m),IDELEMS(e),16);
            IDELEMS(e) += 16;
          }
          e->m[i] = p;
        }
      }
      else
      {
        isMonomial=isMonomial && (pNext(h3->m[j])==NULL);
        pDelete(&pNext(h3->m[j]));
      }
    }
  }
  if ((!isMonomial) && (!TEST_OPT_NOTREGULARITY) && (setRegularity) && (h==isHomog))
  {
    idSkipZeroes(h3);
    resolvente res = sySchreyerResolvente(h3,-1,&length,TRUE);
    intvec * dummy = syBetti(res,length,&reg, *w);
    deg = reg+2;
    delete dummy;
    for (j=0;j<length;j++)
    {
      if (res[j]!=NULL) idDelete(&(res[j]));
    }
    Free((ADDRESS)res,length*sizeof(ideal));
  }
  idDelete(&h3);
  idSkipZeroes(e);
  return e;
}

/*2
* computes syzygies and minimizes the input (h1)
* ONLY used in syMinRes
*/
ideal idSyzMin (ideal h1,ideal  quot, tHomog h,intvec **w,
                  BOOLEAN setRegularity, int &deg)
{
  ideal e, h3;
  poly  p;
  intvec * reord;
  int   i, l=0, j, k=0, quotdim, quotgen,length=0,reg;
  BOOLEAN isMonomial=TRUE;

  if (idIs0(h1))
    return idFreeModule(1);
//PrintS("h1 vorher\n");
//for (i=0;i<IDELEMS(h1);i++)
//{
//Print("Element %d: ",i);pWrite(h1->m[i]);
//}
  idSkipZeroes(h1);
  h3=idPrepare(h1,quot,h,&k,&quotgen,&quotdim,w);
//PrintS("h1 nachher\n");
//for (i=0;i<IDELEMS(h3);i++)
//{
//Print("Element %d: ",i);pWrite(h3->m[i]);
//}
  if (h3==NULL)
    return idFreeModule(1);
  for (i=IDELEMS(h1);i!=0;i--)
    pDelete(&(h1->m[i-1]));
  reord = new intvec(IDELEMS(h1)+1);
  i = -1;
  e=idInit(16,h3->rank);
  for (j=0; j<IDELEMS(h3); j++)
  {
    if (h3->m[j] != NULL)
    {
      p = h3->m[j];
      if (pGetComp(p) > k)
      {
        h3->m[j]=NULL;
        pShift(&p,-k);
        if (p!=NULL)
        {
          i++;
          if (i+1 >= IDELEMS(e))
          {
            pEnlargeSet(&(e->m),IDELEMS(e),16);
            IDELEMS(e) += 16;
          }
          e->m[i] = p;
        }
      }
      else
      {
        while ((pNext(p)!=NULL) && (pGetComp(pNext(p))<=k)) pIter(p);
        if (pIsConstantComp(pNext(p)))
        {
          (*reord)[pGetComp(pNext(p))-k] = l+1;
//Print("Element %d mit Comp %d: ",j,pGetComp(pNext(p))-k);
//pWrite(h3->m[j]);
          h1->m[l] = h3->m[j];
          h3->m[j] = pCopy(h1->m[l]);
          pDelete(&pNext(p));
          l++;
        }
        isMonomial=isMonomial && (pGetComp(pNext(h3->m[j]))>k);
        pDelete(&pNext(h3->m[j]));
      }
    }
  }
  if ((!isMonomial) && (!TEST_OPT_NOTREGULARITY) && (setRegularity) && (h==isHomog))
  {
    idSkipZeroes(h3);
    resolvente res = sySchreyerResolvente(h3,0,&length);
    intvec * dummy = syBetti(res,length,&reg, *w);
    deg = reg+2;
    delete dummy;
    for (j=0;j<length;j++)
    {
      if (res[j]!=NULL) idDelete(&(res[j]));
    }
    Free((ADDRESS)res,length*sizeof(ideal));
  }
  idDelete(&h3);
//PrintS("Komponententransformation: ");
//reord->show();
//PrintLn();
  for (i=IDELEMS(e);i!=0;i--)
  {
    if (e->m[i-1]!=NULL)
    {
      p = e->m[i-1];
      while (p!=NULL)
      {
        j = pGetComp(p);
        pSetComp(p,(*reord)[j]);
        pIter(p);
      }
      e->m[i-1] = pOrdPolyMerge(e->m[i-1]);
    }
  }
  idSkipZeroes(e);
  delete reord;
  return e;
}

/*
*computes a standard basis for h1 and stores the transformation matrix
* in ma
*/
ideal idLiftStd (ideal  h1,ideal  quot, matrix* ma, tHomog h)
{
  int   i, j, k=0, t, quotgen, inputIsIdeal=lengthFreeModule(h1);
  ideal e, h3;
  poly  p=NULL, q, qq;
  intvec *w=NULL;

  idDelete((ideal*)ma);
  *ma=mpNew(1,0);
  if (idIs0(h1))
    return idInit(1,h1->rank);
  h3=idPrepare(h1,quot,h,&k,&quotgen,&i,&w);
  if (w!=NULL) delete w;
  i = 0;
  for (j=0;j<IDELEMS(h3);j++)
  {
    if ((h3->m[j] != NULL) && (pMinComp(h3->m[j]) <= k))
      i++;
  }
  j = IDELEMS(h1);
  idDelete((ideal*)ma);
  *ma = mpNew(j,i);
  i = -1;
  e=idInit(16,h1->rank);
  for (j=0; j<IDELEMS(h3); j++)
  {
    if ((h3->m[j] != NULL) && (pMinComp(h3->m[j]) <= k))
    {
      q = h3->m[j];
      qq=q;
      h3->m[j]=NULL;
      while (pNext(q) != NULL)
      {
        if (pGetComp(pNext(q)) > k)
        {
          p = pNext(q);
          pNext(q) = pNext(pNext(q));
          pNext(p) = NULL;
          t=pGetComp(p);
          pSetComp(p,0);
          MATELEM(*ma,t-k,i+2) = pAdd(MATELEM(*ma,t-k,i+2),p);
        }
        else
        {
          pIter(q);
        }
      }
      if (!inputIsIdeal) pShift(&qq,-1);
      //if (quotgen != 0)
      //{
      //  q = kNF(quot,qq);
      //  pDelete(&qq);
      //}
      //else
        q = qq;
      if (q !=NULL)
      {
        i++;
        if (i+1 >= IDELEMS(e))
        {
          pEnlargeSet(&(e->m),IDELEMS(e),16);
          IDELEMS(e) += 16;
        }
        e->m[i] = q;
      }
    }
  }
  idDelete(&h3);
  idSkipZeroes(e);
  return e;
}

/*2
*computes a representation of the generators of submod with respect to those
* of mod
*/
ideal idLiftNonStB (ideal  mod, ideal submod)
{
  int   lsmod =idRankFreeModule(submod), i, j, k, quotgen;
  ideal result, h3, temp;

  if (idIs0(mod))
    return idInit(1,mod->rank);

  if  ((idRankFreeModule(mod)!=0) && (lsmod==0)) lsmod=1;
  k=lsmod;
  //idSkipZeroes(mod);
  h3=idPrepare(mod,currQuotient,(tHomog)FALSE,&k,&quotgen,&i,NULL);
  for (j=0;j<IDELEMS(h3);j++)
  {
    if ((h3->m[j] != NULL) && (pMinComp(h3->m[j]) > k))
      pDelete(&(h3->m[j]));
  }
  idSkipZeroes(h3);
  if (lsmod==0)
  {
    temp = idCopy(submod);
    for (j=IDELEMS(temp);j>0;j--)
    {
      if (temp->m[j-1]!=NULL)
        pShift(&(temp->m[j-1]),1);
    }
  }
  else
  {
    temp = submod;
  }
  pSetSyzComp(k);
  result = kNF(h3,currQuotient,temp,k);
  result->rank = h3->rank;
  idDelete(&h3);
  if (lsmod==0)
    idDelete(&temp);
  pSetSyzComp(0);
  for (j=0;j<IDELEMS(result);j++)
  {
    if (result->m[j]!=NULL)
    {
      if (pGetComp(result->m[j])<=k)
      {
        WerrorS("2nd module lies not in the first");
        idDelete(&result);
        return idInit(1,1);
      }
      else
      {
        pShift(&(result->m[j]),-k);
        pNeg(result->m[j]);
      }
    }
  }
  return result;
}

/*2
*computes a representation of the generators of submod with respect to those
* of mod which is given as standardbasis,
* uses currQuotient as the quotient ideal (if not NULL)
*/
ideal  idLift (ideal  mod,ideal submod)
{
  ideal temp, result;
  int   j,k;
  poly  p,q;
  BOOLEAN reported=FALSE;

  if (idIs0(mod)) return idInit(1,mod->rank);
  k = lengthFreeModule(mod);
  temp=idCopy(mod);
  if (k == 0)
  {
    for (j=0; j<IDELEMS(temp); j++)
    {
      if (temp->m[j]!=NULL) pSetCompP(temp->m[j],1);
    }
    k = 1;
  }
  for (j=0; j<IDELEMS(temp); j++)
  {
    if (temp->m[j]!=NULL)
    {
      p = temp->m[j];
      q = pOne();
      pGetCoeff(q)=nNeg(pGetCoeff(q));   //set q to -1
      pSetComp(q,k+1+j);
      while (pNext(p)) pIter(p);
      pNext(p) = q;
    }
  }
  result=idInit(IDELEMS(submod),submod->rank);
  pSetSyzComp(k);
  for (j=0; j<IDELEMS(submod); j++)
  {
    if (submod->m[j]!=NULL)
    {
      p = pCopy(submod->m[j]);
      if (pGetComp(p)==0) pSetCompP(p,1);
      q = kNF(temp,currQuotient,p);
      pDelete(&p);
      if (q!=NULL)
      {
        if (pMinComp(q)<=k)
        {
          if (!reported)
          {
            Warn("first module not a standardbasis");
            Warn("or second not a proper submodule");
            reported=TRUE;
          }
          pDelete(&q);
        }
        else
        {
          pShift(&q,-k);
          result->m[j] = q;
        }
      }
    }
  }
  pSetSyzComp(0);
  //idSkipZeroes(result);
  idDelete(&temp);
  return result;
}

/*2
*computes the quotient of h1,h2
*/
#ifdef OLD_QUOT
ideal idQuot (ideal  h1, ideal h2, BOOLEAN h1IsSB)
{
  int i,j = 0,l,ll,k,kkk,k1,k2,kmax;
  ideal h3,h4;
  poly     p,q = NULL;
  BOOLEAN     b = FALSE;

  k1 = lengthFreeModule(h1);
  k2 = lengthFreeModule(h2);
  k=max(k1,k2);
  if (k==0) { k = 1; b=TRUE; }
  h4 = idInit(1,1);
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
  kmax = j*k+1;
  p = pOne();
  pSetComp(p,kmax);
  pSetSyzComp(kmax-1);
  q = pAdd(q,p);
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
      p = pCopy(h4->m[i-1]);
      pShift(&p,1);
      h4->m[i] = p;
    }
  }
  kkk = IDELEMS(h4);
  i = IDELEMS(h1);
  while (h1->m[i-1] == NULL) i--;
  for (l=0; l<i; l++)
  {
    if(h1->m[l]!=NULL)
    {
      for (ll=0; ll<j; ll++)
      {
        p = pCopy(h1->m[l]);
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
  h3 = std(h4,currQuotient,(tHomog)FALSE,NULL,NULL,kmax-1);
  pSetSyzComp(0);
  idDelete(&h4);
  for (i=0;i<IDELEMS(h3);i++)
  {
    if ((h3->m[i]!=NULL) && (pGetComp(h3->m[i])>=kmax))
    {
      if (b)
        pShift(&h3->m[i],-kmax);
      else
        pShift(&h3->m[i],-kmax+1);
    }
    else
      pDelete(&h3->m[i]);
  }
  if (b)
    h3->rank = 1;
  else
    h3->rank = h1->rank;
  h4=idCompactify(h3);
  idDelete(&h3);
  return h4;
}
#else
ideal idQuot (ideal  h1, ideal h2, BOOLEAN h1IsStb)
{
  int i,j = 0,l,ll,k,kkk,k1,k2,kmax;
  intvec * weights,* weights1;
  ideal h3,h4,temph1;
  poly     p,q = NULL;
  BOOLEAN  resultIsIdeal = FALSE,addOnlyOne=TRUE;
  tHomog   hom=isNotHomog;
  BITSET old_test=test;

  k1 = lengthFreeModule(h1);
  k2 = lengthFreeModule(h2);
  if (idIs0(h2))
  {
    if (k1==0)
    {
      h3 = idInit(1,1);
      h3->m[0] = pOne();
    }
    else
      h3 = idFreeModule(h1->rank);
    return h3;
  }
  k=max(k1,k2);
  if (k==0)
  {
    k = 1;
    resultIsIdeal=TRUE;
  }
  else if ((k1>0)&&(k2>0))
  {
    resultIsIdeal=TRUE;
  }
  hom = (tHomog)idHomModule(h1,currQuotient,&weights) ;
  h4 = idInit(1,1);
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
  kmax = j*k+1;
  p = pOne();
  pSetComp(p,kmax);
  pSetSyzComp(kmax-1);
  q = pAdd(q,p);
  h4->m[0] = q;
  if (k2 == 0)
  {
    if (k > IDELEMS(h4))
    {
      pEnlargeSet(&(h4->m),IDELEMS(h4),k-IDELEMS(h4));
      IDELEMS(h4) = k;
    }
    if (k>1) addOnlyOne = FALSE;
    for (i=1; i<k; i++)
    {
      p = pCopy(h4->m[i-1]);
      pShift(&p,1);
      h4->m[i] = p;
    }
  }
  kkk = IDELEMS(h4);
  if (addOnlyOne && (!h1IsStb))
    temph1 = std(h1,currQuotient,hom,&weights,NULL);
  else
    temph1 = h1;
  idTest(temph1);
  i = IDELEMS(temph1);
  while ((i>0) && (temph1->m[i-1]==NULL)) i--;
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
  idTest(h4);
  if (addOnlyOne)
  {
    p = h4->m[0];
    for (i=0;i<IDELEMS(h4)-1;i++)
    {
      h4->m[i] = h4->m[i+1];
    }
    h4->m[IDELEMS(h4)-1] = p;
    idSkipZeroes(h4);
    test |= Sy_bit(OPT_SB_1);
  }
  idTest(h4);
  hom = (tHomog)idHomModule(h4,currQuotient,&weights1);
  if (addOnlyOne)
  {
    h3 = std(h4,currQuotient,hom,&weights1,NULL,kmax-1,IDELEMS(h4)-1);
  }
  else
  {
    h3 = std(h4,currQuotient,hom,&weights1,NULL,kmax-1);
  }
  idTest(h3);
  idDelete(&h4);
  pSetSyzComp(0);
  for (i=0;i<IDELEMS(h3);i++)
  {
    if ((h3->m[i]!=NULL) && (pGetComp(h3->m[i])>=kmax))
    {
      if (resultIsIdeal)
        pShift(&h3->m[i],-kmax);
      else
        pShift(&h3->m[i],-kmax+1);
    }
    else
      pDelete(&h3->m[i]);
  }
  if (resultIsIdeal)
    h3->rank = 1;
  else
    h3->rank = h1->rank;
  idTest(h3);
  idSkipZeroes(h3);
  //h4=idCompactify(h3);
  //idDelete(&h3);
  if ((addOnlyOne) && (!h1IsStb))
    idDelete(&temph1);
  if (weights!=NULL) delete weights;
  if (weights1!=NULL) delete weights1;
  test = old_test;
  return h3;
}
#endif

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
  for (i=restdeg-1;i>=0;i--)
  {
    p = pPower(pCopy(given->m[begin]),i);
    p = pMult(pCopy(ap),p);
    idNextPotence(given, result, begin+1, end, deg, restdeg-i, p);
    pDelete(&p);
  }
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
  idSkipZeroes(result);
  result->nrows = 1;
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
  short  **wv;
  tHomog hom;
  intvec * w;
  sip_sring tmpR;
  ring origR = currRing;

  if (delVar==NULL)
  {
    return idCopy(h1);
  }
  if (currQuotient!=NULL)
  {
    WerrorS("cannot eliminate in a qring");
    return idCopy(h1);
  }
  if (idIs0(h1)) return idInit(1,h1->rank);
  hom=(tHomog)idHomModule(h1,NULL,&w); //sets w to weight vector or NULL
  h3=idInit(16,h1->rank);
  for (k=0;; k++)
  {
    if (currRing->order[k]!=0) ordersize++;
    else break;
  }
  ord=(int*)Alloc0(ordersize*sizeof(int));
  block0=(int*)Alloc(ordersize*sizeof(int));
  block1=(int*)Alloc(ordersize*sizeof(int));
  for (k=0;; k++)
  {
    if (currRing->order[k]!=0)
    {
      block0[k+1] = currRing->block0[k];
      block1[k+1] = currRing->block1[k];
      ord[k+1] = currRing->order[k];
    }
    else
      break;
  }
  block0[0] = 1;
  block1[0] = pVariables;
  wv=(short**) Alloc0(ordersize*sizeof(short**));
  memcpy4(wv+1,currRing->wvhdl,(ordersize-1)*sizeof(short**));
  wv[0]=(short*)Alloc0((pVariables+1)*sizeof(short));
  for (j=0;j<pVariables;j++)
    if (pGetExp(delVar,j+1)!=0) wv[0][j]=1;
  ord[0] = ringorder_a;

  // fill in tmp ring to get back the data later on
  tmpR  = *origR;
  tmpR.order = ord;
  tmpR.block0 = block0;
  tmpR.block1 = block1;
  tmpR.wvhdl = wv;
  rComplete(&tmpR);

  // change into the new ring
  //pChangeRing(pVariables,currRing->OrdSgn,ord,block0,block1,wv);
  rChangeCurrRing(&tmpR, TRUE);
  currRing = &tmpR;
  h = idInit(IDELEMS(h1),1);
  // fetch data from the old ring
  for (k=0;k<IDELEMS(h1);k++) h->m[k] = pFetchCopy(origR, h1->m[k]);
  // compute std
  hh = std(h,NULL,hom,&w,hilb);
  idDelete(&h);

  // go back to the original ring
  rChangeCurrRing(origR,TRUE);
  i = IDELEMS(hh)-1;
  while ((i >= 0) && (hh->m[i] == NULL)) i--;
  j = -1;
  // fetch data from temp ring
  for (k=0; k<=i; k++)
  {
    l=pVariables;
    while ((l>0) && (pRingGetExp(&tmpR, hh->m[k],l)*pGetExp(delVar,l)==0)) l--;
    if (l==0)
    {
      j++;
      if (j >= IDELEMS(h3))
      {
        pEnlargeSet(&(h3->m),IDELEMS(h3),16);
        IDELEMS(h3) += 16;
      }
      h3->m[j] = pFetchCopy(&tmpR, hh->m[k]);
    }
  }
  idDelete(&hh);
  idSkipZeroes(h3);
  Free((ADDRESS)wv[0],(pVariables+1)*sizeof(short));
  Free((ADDRESS)wv,ordersize*sizeof(short**));
  Free((ADDRESS)ord,ordersize*sizeof(int));
  Free((ADDRESS)block0,ordersize*sizeof(int));
  Free((ADDRESS)block1,ordersize*sizeof(int));
  if (w!=NULL)
    delete w;
  return h3;
}

/*3
* produces recursively the ideal of all arxar-minors of a
*/
static void idRecMin(matrix a,int ar,poly *barDiv,ideal result,
              int * nextPlace, int rowToChose=0)
{
//Print("Level: %d\n",ar);
/*--- there is no non-zero minor coming from a------------*/
  if((ar<0) || (ar>min(a->ncols,a->nrows)-1))
  {
    return;
  }

/*--- initializing temporary structures-------------------*/
  int i,j,r=rowToChose,c,newi,newp,k;
  poly p=NULL,pp;

  if (ar==0)
  {
/*--- ar is 0, the matrix-entres are minors---------------*/
    for (i=a->nrows;i>0;i--)
    {
      for (j=a->ncols;j>0;j--)
      {
        p = MATELEM(a,i,j);
        if (p!=NULL)
        {
          if (*nextPlace>=IDELEMS(result))
          {
            pEnlargeSet(&(result->m),IDELEMS(result),16);
            IDELEMS(result) += 16;
          }
          MATELEM(a,i,j) = NULL;
          result->m[*nextPlace] = p;
          (*nextPlace)++;
        }
      }
    }
    idTest(result);
    idDelete((ideal*)&a);
    return;
  }
/*--- ar>0, we perform one step of the Bareiss algorithm--*/
  p = pCopy(*barDiv);   //we had to store barDiv for the remaining loops
  pp = pCopy(p);   //we had to store barDiv for the remaining loops
  matrix nextStep = mpOneStepBareiss(a,barDiv,&r,&c);
//Print("next row is: %d, next col: %d\n",r,c);
/*--- there is no pivot - the matrix is zero -------------*/
  if (r*c==0)
  {
    idDelete((ideal*)&a);
    return;
  }
/*--- we read out the r-1 x c-1 matrix for the next step--*/
  if ((a->nrows-1)*(a->ncols-1)>0)
  {
    matrix next = mpNew(a->nrows-1,a->ncols-1);
    for (i=a->nrows-1;i>0;i--)
    {
      for (j=a->ncols-1;j>0;j--)
      {
        MATELEM(next,i,j) = MATELEM(nextStep,i,j);
        MATELEM(nextStep,i,j) =NULL;
      }
    }
    idDelete((ideal*)&nextStep);
/*--- we call the next Step------------------------------*/
    idRecMin(next,ar-1,barDiv,result,nextPlace);
    next = NULL;
  }
/*--- now we have to take out the r-th row...------------*/
  if (((a->nrows)>1) && (rowToChose==0))
  {
    nextStep = mpNew(a->nrows-1,a->ncols);
    for (i=r-1;i>0;i--)
    {
      for (j=a->ncols;j>0;j--)
      {
        MATELEM(nextStep,i,j) = pCopy(MATELEM(a,i,j));
      }
    }
    for (i=a->nrows;i>r;i--)
    {
      for (j=a->ncols;j>0;j--)
      {
        MATELEM(nextStep,i-1,j) = pCopy(MATELEM(a,i,j));
      }
    }
/*--- and to perform the algorithm with the rest---------*/
    idRecMin(nextStep,ar,&p,result,nextPlace);
    nextStep = NULL;
  }
/*--- now we have to take out the c-th col...------------*/
  if ((a->nrows)>1)
  {
    nextStep = mpNew(a->nrows,a->ncols-1);
    for (i=a->nrows;i>0;i--)
    {
      for (j=c-1;j>0;j--)
      {
        MATELEM(nextStep,i,j) = MATELEM(a,i,j);
        MATELEM(a,i,j) = NULL;
      }
    }
    for (i=a->nrows;i>0;i--)
    {
      for (j=a->ncols;j>c;j--)
      {
        MATELEM(nextStep,i,j-1) = MATELEM(a,i,j);
        MATELEM(a,i,j) = NULL;
      }
    }
/*--- and to perform the algorithm with the rest---------*/
    idDelete((ideal*)&a);
    idRecMin(nextStep,ar,&p,result,nextPlace,r);
    nextStep = NULL;
  }
/*--- deleting temporary structures and returns----------*/
  pDelete(barDiv);
  pDelete(&p);
  pDelete(&pp);
  return;
}

/*2
* compute all ar-minors of the matrix a
* the caller of idRecMin
*/
ideal idMinors(matrix a, int ar)
{
  if((ar<=0) || (ar>min(a->ncols,a->nrows)))
  {
    Werror("%d-th minor ",ar);
    return NULL;
  }
  int i=0;
  poly barDiv=NULL;
  ideal result=idInit(16,1);
  idTest(result);

  idRecMin(mpCopy(a),ar-1,&barDiv,result,&i);
  idSkipZeroes(result);
  return result;
}

/*2
*returns TRUE if p is a unit element in the current ring
*/
BOOLEAN pIsUnit(poly p)
{
  int i;

  if (p == NULL) return FALSE;
  i = 1;
  while (i<=pVariables && pGetExp(p,i) == 0) i++;
  if (i > pVariables && (pGetComp(p) == 0))
  {
    if (currRing->OrdSgn == 1 && pNext(p) !=NULL) return FALSE;
    return TRUE;
  }
  return FALSE;
}

/*2
*skips all zeroes and double elements, searches also for units
*/
ideal idCompactify(ideal id)
{
  ideal result = NULL;
  int i,j;
  BOOLEAN b=FALSE;

  result=idCopy(id);
  i = IDELEMS(result)-1;
  while ((! b) && (i>=0))
  {
    b=pIsUnit(result->m[i]);
    i--;
  }
  if (b)
  {
    for (i=IDELEMS(result)-1;i>=0;i--)
      pDelete(&result->m[i]);
    result->m[0]=pOne();
  }
  else
  {
    for (i=1;i<IDELEMS(result);i++)
    {
      if (result->m[i]!=NULL)
      {
        for (j=0;j<i;j++)
        {
          if ((result->m[j]!=NULL)
          && (pComparePolys(result->m[i],result->m[j])))
          {
            pDelete(&(result->m[j]));
          }
        }
      }
    }
  }
  idSkipZeroes(result);
  return result;
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
   Free((ADDRESS)result->m,sizeof(poly));
   result->m=NULL; // remove later
   pVec2Polys(vec, &(result->m), &(IDELEMS(result)));
   return result;
}

ideal idMatrix2Module(matrix mat)
{
  ideal result = idInit(MATCOLS(mat),MATROWS(mat));
  int i,j;
  poly h;
#ifdef DRING
  poly p;
#endif

  for(j=0;j<MATCOLS(mat);j++) /* j is also index in result->m */
  {
    for (i=1;i<=MATROWS(mat);i++)
    {
      h = MATELEM(mat,i,j+1);
      if (h!=NULL)
      {
        MATELEM(mat,i,j+1)=NULL;
        pSetCompP(h,i);
#ifdef DRING
        pdSetDFlag(h,0);
#endif
        result->m[j] = pAdd(result->m[j],h);
      }
    }
  }
  return result;
}

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
    p=mod->m[i];
    mod->m[i]=NULL;
    while (p!=NULL)
    {
      h=p;
      pIter(p);
      pNext(h)=NULL;
//      cp = max(1,pGetComp(h));     // if used for ideals too
      cp = pGetComp(h);
      pSetComp(h,0);
#ifdef TEST
      if (cp>mod->rank)
      {
        Print("## inv. rank %d -> %d\n",mod->rank,cp);
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
    p=mod->m[i];
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
  if (idIs0(m)) return TRUE;

  int i,j,cmax=2,order=0,ord,* diff,* iscom,diffmin=32000;
  poly p=NULL;
  int length=IDELEMS(m);
  polyset P=m->m;
  polyset F=(polyset)Alloc(length*sizeof(poly));
  for (i=length-1;i>=0;i--)
  {
    p=F[i]=P[i];
    cmax=max(cmax,pMaxComp(p)+1);
  }
  diff = (int *)Alloc0(cmax*sizeof(int));
  if (w!=NULL) *w=new intvec(cmax-1);
  iscom = (int *)Alloc0(cmax*sizeof(int));
  i=0;
  while (i<=length)
  {
    if (i<length)
    {
      p=F[i];
      while ((p!=NULL) && (!iscom[pGetComp(p)])) pIter(p);
    }
    if ((p==NULL) && (i<length))
    {
      i++;
    }
    else
    {
      if (p==NULL)
      {
        i=0;
        while ((i<length) && (F[i]==NULL)) i++;
        if (i>=length) break;
        p = F[i];
      }
      if (pLexOrder)
        order=pTotaldegree(p);
      else
      //  order = p->order;
        order = pFDeg(p);
      order += diff[pGetComp(p)];
      p = F[i];
//Print("Actual p=F[%d]: ",i);pWrite(p);
      F[i] = NULL;
      i=0;
    }
    while (p!=NULL)
    {
      //if (pLexOrder)
      //  ord=pTotaldegree(p);
      //else
      //  ord = p->order;
      ord = pFDeg(p);
      if (!iscom[pGetComp(p)])
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
        if (order != ord+diff[pGetComp(p)])
        {
          Free((ADDRESS) iscom,cmax*sizeof(int));
          Free((ADDRESS) diff,cmax*sizeof(int));
          Free((ADDRESS) F,length*sizeof(poly));
          delete *w;*w=NULL;
          return FALSE;
        }
      }
      pIter(p);
    }
  }
  Free((ADDRESS) iscom,cmax*sizeof(int));
  Free((ADDRESS) F,length*sizeof(poly));
  for (i=1;i<cmax;i++) (**w)[i-1]=diff[i];
  for (i=1;i<cmax;i++)
  {
    if (diff[i]<diffmin) diffmin=diff[i];
  }
  for (i=1;i<cmax;i++)
  {
    (**w)[i-1]=diff[i]-diffmin;
  }
  Free((ADDRESS) diff,cmax*sizeof(int));
  return TRUE;
}

ideal idJet(ideal i,int d)
{
  ideal r=idInit(IDELEMS(i),i->rank);
  int k;
  for(k=0; k<IDELEMS(i); k++)
  {
    r->m[k]=pJet(i->m[k],d);
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
      r->m[k]=pJetW(i->m[k],d,w);
    }
    Free((ADDRESS)w,(pVariables+1)*sizeof(short));
  }
  return r;
}

matrix idDiff(matrix i, int k)
{
  int e=MATCOLS(i)*MATROWS(i);
  matrix r=mpNew(MATROWS(i),MATCOLS(i));
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

/*2
* represents (h1+h2)/h2=h1/(h1 intersect h2)
*/
ideal idModulo (ideal h2,ideal h1)
{
  ideal temp,temp1;
  int i,j,k,rk,flength=0,slength,length;
  intvec * w;
  poly p,q;

  if (idIs0(h2))
    return idFreeModule(max(1,h2->ncols));
  if (!idIs0(h1))
    flength = idRankFreeModule(h1);
  slength = idRankFreeModule(h2);
  length  = max(flength,slength);
  if (length==0)
  {
    length = 1;
  }
  temp = idInit(IDELEMS(h2),1);
  for (i=0;i<IDELEMS(h2);i++)
  {
    temp->m[i] = pCopy(h2->m[i]);
    q = pOne();
    pSetComp(q,i+1+length);
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
  pSetSyzComp(length);
  temp1 = std(temp,currQuotient,testHomog,&w,NULL,length);
  pSetSyzComp(0);
  idDelete(&temp);
  if (w!=NULL) delete w;
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
  idSkipZeroes(temp1);
  temp1->rank = rk;
  return temp1;
}

int idElem(ideal F)
{
  int i=0,j=0;

  while(j<IDELEMS(F))
  {
   if ((F->m)[j]!=NULL) i++;
   j++;
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
    (*result)[i]=pFDeg(mod->m[i])+(*weights)[pGetComp(mod->m[i])];
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
  while (i>=0)
  {
    while ((j>0) && (pGetExp(monom,i)<pGetExp(kbase->m[j-1],i))) j--;
    if ((j==0) || (pGetExp(monom,i)>pGetExp(kbase->m[j-1],i))) return -1;
    if (i==0)
      return j-1;
    else
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

  while ((i>0) && (kbase->m[i-1]==NULL)) i--;
  if (idIs0(arg))
    return mpNew(i,1);
  while ((j>0) && (arg->m[j-1]==NULL)) j--;
  result = mpNew(i,j);
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
      p = pIter(p);
    }
  }
  idDelete(&tempKbase);
  return result;
}

intvec * idQHomWeights(ideal id)
{
  intvec * imat=new intvec(2*pVariables,pVariables,0);
  poly actHead=NULL,wPoint=NULL;
  int actIndex,i=-1,j=1,k;
  BOOLEAN notReady=TRUE;

  while (notReady)
  {
    if (wPoint==NULL)
    {
      i++;
      while ((i<IDELEMS(id))
      && ((id->m[i]==NULL) || (pNext(id->m[i])==NULL)))
        i++;
      if (i<IDELEMS(id))
      {
        actHead = id->m[i];
        wPoint = pNext(actHead);
      }
    }
    while ((wPoint!=NULL) && (j<=2*pVariables))
    {
      for (k=1;k<=pVariables;k++)
        IMATELEM(*imat,j,k) += pGetExp(actHead,k)-pGetExp(wPoint,k);
      pIter(wPoint);
      j++;
    }
    if ((i>=IDELEMS(id)) || (j>2*pVariables))
    {
      ivTriangMat(imat,1,1);
      j = ivFirstEmptyRow(imat);
      if ((i>=IDELEMS(id)) || (j>pVariables)) notReady=FALSE;
    }
  }
  intvec *result=NULL;
  if (j<=pVariables)
  {
    result=ivSolveIntMat(imat);
  }
  //else
  //{
  //  WerrorS("not homogeneous");
  //}
  delete imat;
  return result;
}

/*2
* returns the presentation of an isomorphic, minimally
* embedded  module
*/
ideal idMinEmbedding(ideal arg)
{
  if (idIs0(arg)) return idInit(1,arg->rank);

  int i,j,k,pC;
  poly p,q;
  int rg=arg->rank;
  ideal res = idCopy(arg);
  intvec *indexMap=new intvec(rg+1);
  intvec *toKill=new intvec(rg+1);

  loop
  {
    k = 0;
    for (i=indexMap->length()-1;i>0;i--)
    {
      (*indexMap)[i] = i;
      (*toKill)[i] = 0;
    }
    for (j=IDELEMS(res)-1;j>=0;j--)
    {
      if ((res->m[j]!=NULL) && (pIsConstantComp(res->m[j])) &&
           (pNext(res->m[j])==NULL))
      {
        pC = pGetComp(res->m[j]);
        if ((*toKill)[pC]==0)
        {
          rg--;
          (*toKill)[pC] = 1;
          for (i=indexMap->length()-1;i>=pC;i--)
            (*indexMap)[i]--;
        }
        pDelete(&(res->m[j]));
        k++;
      }
    }
    idSkipZeroes(res);
    if (k==0) break;
    if (rg>0)
    {
      res->rank=rg;
      for (j=IDELEMS(res)-1;j>=0;j--)
      {
        while ((res->m[j]!=NULL) && ((*toKill)[pGetComp(res->m[j])]==1))
          pDelete1(&res->m[j]);
        p = res->m[j];
        while ((p!=NULL) && (pNext(p)!=NULL))
        {
          pSetComp(p,(*indexMap)[pGetComp(p)]);
          while ((pNext(p)!=NULL) && ((*toKill)[pGetComp(pNext(p))]==1))
            pDelete1(&pNext(p));
          pIter(p);
        }
        if (p!=NULL) pSetComp(p,(*indexMap)[pGetComp(p)]);
      }
      idSkipZeroes(res);
    }
    else
    {
      idDelete(&res);
      res=idFreeModule(1);
      break;
    }
  }
  delete toKill;
  delete indexMap;
  return res;
}


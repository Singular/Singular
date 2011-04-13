/****************************************
*  Computer Algebra System SINGULAR     *
****************************************/
/* $Id$ */
/*
* ABSTRACT - all basic methods to manipulate ideals
*/


/* includes */
#include "config.h"
#include <misc/auxiliary.h>
#include <misc/options.h>
#include <omalloc/omalloc.h>
#include <polys/monomials/p_polys.h>
#include <misc/intvec.h>

#include "simpleideals.h"

/*2
* initialise an ideal
*/
ideal idInit(int idsize, int rank)
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

#ifdef PDEBUG
// this is only for outputting an ideal within the debugger
void idShow(const ideal id, const ring lmRing, const ring tailRing, const int debugPrint)
{
  assume( debugPrint >= 0 );

  if( id == NULL )
    PrintS("(NULL)");
  else
  {
    Print("Module of rank %ld,real rank %ld and %d generators.\n",
          id->rank,id_RankFreeModule(id, lmRing, tailRing),IDELEMS(id));

    int j = (id->ncols*id->nrows) - 1;
    while ((j > 0) && (id->m[j]==NULL)) j--;
    for (int i = 0; i <= j; i++)
    {
      Print("generator %d: ",i); p_DebugPrint(id->m[i], lmRing, tailRing, debugPrint);
    }
  }
}
#endif

/* index of generator with leading term in ground ring (if any);
   otherwise -1 */
int id_PosConstant(ideal id, const ring r)
{
  int k;
  for (k = IDELEMS(id)-1; k>=0; k--)
  {
    if (p_LmIsConstantComp(id->m[k], r) == TRUE)
      return k;
  }
  return -1;
}

/*2
* initialise the maximal ideal (at 0)
*/
ideal id_MaxIdeal (const ring r)
{
  int l;
  ideal hh=NULL;

  hh=idInit(rVar(r),1);
  for (l=0; l<rVar(r); l++)
  {
    hh->m[l] = p_One(r);
    p_SetExp(hh->m[l],l+1,1,r);
    p_Setm(hh->m[l],r);
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
ideal id_CopyFirstK (const ideal ide, const int k,const ring r)
{
  ideal newI = idInit(k, 0);
  for (int i = 0; i < k; i++)
    newI->m[i] = p_Copy(ide->m[i],r);
  return newI;
}

/*2
* ideal id = (id[i])
* result is leadcoeff(id[i]) = 1
*/
void id_Norm(ideal id, const ring r)
{
  for (int i=IDELEMS(id)-1; i>=0; i--)
  {
    if (id->m[i] != NULL)
    {
      p_Norm(id->m[i],r);
    }
  }
}

/*2
* ideal id = (id[i]), c any unit
* if id[i] = c*id[j] then id[j] is deleted for j > i
*/
void id_DelMultiples(ideal id, const ring r)
{
  int i, j;
  int k = IDELEMS(id)-1;
  for (i=k; i>=0; i--)
  {
    if (id->m[i]!=NULL)
    {
      for (j=k; j>i; j--)
      {
        if (id->m[j]!=NULL)
        {
#ifdef HAVE_RINGS
          if (rField_is_Ring(r))
          {
            /* if id[j] = c*id[i] then delete id[j].
               In the below cases of a ground field, we
               check whether id[i] = c*id[j] and, if so,
               delete id[j] for historical reasons (so
               that previous output does not change) */
            if (p_ComparePolys(id->m[j], id->m[i],r)) p_Delete(&id->m[j],r);
          }
          else
          {
            if (p_ComparePolys(id->m[i], id->m[j],r)) p_Delete(&id->m[j],r);
          }
#else
          if (p_ComparePolys(id->m[i], id->m[j],r)) p_Delete(&id->m[j],r);
#endif    
        }
      }
    }
  }
}

/*2
* ideal id = (id[i])
* if id[i] = id[j] then id[j] is deleted for j > i
*/
void id_DelEquals(ideal id, const ring r)
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
        && (p_EqualPolys(id->m[i], id->m[j],r)))
        {
          p_Delete(&id->m[j],r);
        }
      }
    }
  }
}

//
// Delete id[j], if Lm(j) == Lm(i) and both LC(j), LC(i) are units and j > i
//
void id_DelLmEquals(ideal id, const ring r)
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
        && p_LmEqual(id->m[i], id->m[j],r)
#ifdef HAVE_RINGS
        && n_IsUnit(pGetCoeff(id->m[i]),r->cf) && n_IsUnit(pGetCoeff(id->m[j]),r->cf)
#endif
        )
        {
          p_Delete(&id->m[j],r);
        }
      }
    }
  }
}

//
// delete id[j], if LT(j) == coeff*mon*LT(i) and vice versa, i.e.,
// delete id[i], if LT(i) == coeff*mon*LT(j)
//
void id_DelDiv(ideal id, const ring r)
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
#ifdef HAVE_RINGS
          if (rField_is_Ring(r))
          {
            if (p_DivisibleByRingCase(id->m[i], id->m[j],r))
            {
              p_Delete(&id->m[j],r);
            }
            else if (p_DivisibleByRingCase(id->m[j], id->m[i],r))
            {
              p_Delete(&id->m[i],r);
              break;
            }
          }
          else
          {
#endif
          /* the case of a ground field: */
          if (p_DivisibleBy(id->m[i], id->m[j],r))
          {
            p_Delete(&id->m[j],r);
          }
          else if (p_DivisibleBy(id->m[j], id->m[i],r))
          {
            p_Delete(&id->m[i],r);
            break;
          }
#ifdef HAVE_RINGS
          }
#endif    
        }
      }
    }
  }
}

/*2
*test if the ideal has only constant polynomials
*/
BOOLEAN id_IsConstant(ideal id, const ring r)
{
  int k;
  for (k = IDELEMS(id)-1; k>=0; k--)
  {
    if (!p_IsConstantPoly(id->m[k],r))
      return FALSE;
  }
  return TRUE;
}

/*2
* copy an ideal
*/
#ifdef PDEBUG
ideal id_DBCopy(ideal h1,const char *f,int l, const ring r)
{
  int i;
  ideal h2;

  id_DBTest(h1,PDEBUG,f,l,r);
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
      h2->m[i] = p_Copy(h1->m[i],r);
  }
  return h2;
}
#else
ideal id_Copy(ideal h1, const ring r)
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
#endif

#ifdef PDEBUG
void id_DBTest(ideal h1, int level, const char *f,const int l, const ring r)
{
  int i;

  if (h1 != NULL)
  {
    // assume(IDELEMS(h1) > 0); for ideal/module, does not apply to matrix
    omCheckAddrSize(h1,sizeof(*h1));
    omdebugAddrSize(h1->m,h1->ncols*h1->nrows*sizeof(poly));
    /* to be able to test matrices: */
    for (i=(h1->ncols*h1->nrows)-1; i>=0; i--)
      _p_Test(h1->m[i], r, level);
    int new_rk=id_RankFreeModule(h1,r);
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
static int p_Comp_RevLex(poly a, poly b,BOOLEAN nolex, const ring R)
{
  if (b==NULL) return 1;
  if (a==NULL) return -1;

  if (nolex)
  {
    int r=p_LmCmp(a,b,R);
    if (r!=0) return r;
    number h=n_Sub(pGetCoeff(a),pGetCoeff(b),R->cf);
    r = -1+n_IsZero(h,R->cf)+2*n_GreaterZero(h,R->cf); /* -1: <, 0:==, 1: > */
    n_Delete(&h, R->cf);
    return r;
  }
  int l=rVar(R);
  while ((l>0) && (p_GetExp(a,l,R)==p_GetExp(b,l,R))) l--;
  if (l==0)
  {
    if (p_GetComp(a,R)==p_GetComp(b,R))
    {
      number h=n_Sub(pGetCoeff(a),pGetCoeff(b),R->cf);
      int r = -1+n_IsZero(h,R->cf)+2*n_GreaterZero(h,R->cf); /* -1: <, 0:==, 1: > */
      n_Delete(&h,R->cf);
      return r;
    }
    if (p_GetComp(a,R)>p_GetComp(b,R)) return 1;
  }
  else if (p_GetExp(a,l,R)>p_GetExp(b,l,R))
    return 1;
  return -1;
}

/*2
*sorts the ideal w.r.t. the actual ringordering
*uses lex-ordering when nolex = FALSE
*/
intvec *id_Sort(ideal id,BOOLEAN nolex, const ring r)
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
      lastcomp = p_Comp_RevLex(id->m[i],id->m[(*result)[newpos]],nolex,r);
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
        newcomp = p_Comp_RevLex(id->m[i],id->m[(*result)[newpos]],nolex,r);
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
      while ((newpos<actpos) && (p_Comp_RevLex(id->m[i],id->m[(*result)[newpos]],nolex,r)==0))
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

ideal idSectWithElim (ideal h1,ideal h2)
// does not destroy h1,h2
{
  if (TEST_OPT_PROT) PrintS("intersect by elimination method\n");
  assume(!idIs0(h1));
  assume(!idIs0(h2));
  assume(IDELEMS(h1)<=IDELEMS(h2));
  assume(idRankFreeModule(h1)==0);
  assume(idRankFreeModule(h2)==0);
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
  poly omt=pOne();
  pSetExp(omt,r->N,1);
  poly t=pCopy(omt);
  pSetm(omt);
  omt=pNeg(omt);
  omt=pAdd(omt,pOne());
  // compute (1-t)*h1
  h1=(ideal)mpMultP((matrix)h1,omt);
  // compute t*h2
  h2=(ideal)mpMultP((matrix)h2,pCopy(t));
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

#include "sbuckets.h"

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
* searches for the next unit in the components of the module arg and
* returns the first one;
*/
static int idReadOutPivot(ideal arg,int* comp)
{
  if (idIs0(arg)) return -1;
  int i=0,j, generator=-1;
  int rk_arg=arg->rank; //idRankFreeModule(arg);
  int * componentIsUsed =(int *)omAlloc((rk_arg+1)*sizeof(int));
  poly p;

  while ((generator<0) && (i<IDELEMS(arg)))
  {
    memset(componentIsUsed,0,(rk_arg+1)*sizeof(int));
    p = arg->m[i];
    while (p!=NULL)
    {
      j = pGetComp(p);
      if (componentIsUsed[j]==0)
      {
#ifdef HAVE_RINGS
        if (pLmIsConstantComp(p) &&
            (!rField_is_Ring(currRing) || nIsUnit(pGetCoeff(p))))
        {
#else
        if (pLmIsConstantComp(p))
        {
#endif
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
  for(i=IDELEMS(I)-1;i>=0;i--)
  {
    pNormalize(I->m[i]);
  }
}

// #include <kernel/clapsing.h>

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

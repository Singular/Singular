/****************************************
*  Computer Algebra System SINGULAR     *
****************************************/
/*
* ABSTRACT - all basic methods to manipulate ideals
*/


/* includes */



#include <misc/auxiliary.h>

#include <omalloc/omalloc.h>

#include <misc/options.h>
#include <misc/intvec.h>

// #include <coeffs/longrat.h>
#include "matpol.h"

#include "monomials/p_polys.h"
#include "weight.h"
#include "sbuckets.h"
#include "clapsing.h"

#include "simpleideals.h"

omBin sip_sideal_bin = omGetSpecBin(sizeof(sip_sideal));

static poly * idpower;
/*collects the monomials in makemonoms, must be allocated befor*/
static int idpowerpoint;
/*index of the actual monomial in idpower*/

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

/// index of generator with leading term in ground ring (if any); 
/// otherwise -1
int id_PosConstant(ideal id, const ring r)
{
  id_Test(id, r);
  const int N = IDELEMS(id) - 1; 
  const poly * m = id->m + N; 
 
  for (int k = N; k >= 0; --k, --m)
  {
    const poly p = *m;
    if (p!=NULL)
       if (p_LmIsConstantComp(p, r) == TRUE)
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
      j--;
      poly pp=((*h)->m[j]);
      if (pp!=NULL) p_Delete(&pp, r);
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

///3 for idSort: compare a and b revlex inclusive module comp.
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

// sorts the ideal w.r.t. the actual ringordering
// uses lex-ordering when nolex = FALSE
intvec *id_Sort(const ideal id, const BOOLEAN nolex, const ring r)
{
  intvec * result = new intvec(IDELEMS(id));
  int i, j, actpos=0, newpos;
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
ideal id_SimpleAdd (ideal h1,ideal h2, const ring R)
{
  int i,j,r,l;
  ideal result;

  if (h1==NULL) return id_Copy(h2,R);
  if (h2==NULL) return id_Copy(h1,R);
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
    result->m[l] = p_Copy(h1->m[l],R);
  }
  r = i+j+1;
  for (l=i; l>=0; l--, r--)
  {
    result->m[r] = p_Copy(h2->m[l],R);
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
BOOLEAN id_InsertPolyWithTests (ideal h1, const int validEntries,
  const poly h2, const bool zeroOk, const bool duplicateOk, const ring r)
{
  if ((!zeroOk) && (h2 == NULL)) return FALSE;
  if (!duplicateOk)
  {
    bool h2FoundInH1 = false;
    int i = 0;
    while ((i < validEntries) && (!h2FoundInH1))
    {
      h2FoundInH1 = p_EqualPolys(h1->m[i], h2,r);
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
ideal id_Add (ideal h1,ideal h2, const ring r)
{
  ideal result = id_SimpleAdd(h1,h2,r);
  id_Compactify(result,r);
  return result;
}

/*2
* h1 * h2
*/
ideal  id_Mult (ideal h1,ideal  h2, const ring r)
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
          hh->m[k] = pp_Mult_qq(h1->m[i],h2->m[j],r);
          k++;
        }
      }
    }
  }
  {
    id_Compactify(hh,r);
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
long id_RankFreeModule (ideal s, ring lmRing, ring tailRing)
{
  if (s!=NULL)
  {
    long  j=0;

    if (rRing_has_Comp(tailRing) && rRing_has_Comp(lmRing))
    {
      poly *p=s->m;
      for (unsigned int l=IDELEMS(s); l != 0; --l, ++p)
      {
        if (*p!=NULL)
        {
          pp_Test(*p, lmRing, tailRing);
          const long k = p_MaxComp(*p, lmRing, tailRing);
          if (k>j) j = k;
        }
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
BOOLEAN id_HomIdeal (ideal id, ideal Q, const ring r)
{
  int i;
  BOOLEAN     b;
  if ((id == NULL) || (IDELEMS(id) == 0)) return TRUE;
  i = 0;
  b = TRUE;
  while ((i < IDELEMS(id)) && b)
  {
    b = p_IsHomogeneous(id->m[i],r);
    i++;
  }
  if ((b) && (Q!=NULL) && (IDELEMS(Q)>0))
  {
    i=0;
    while ((i < IDELEMS(Q)) && b)
    {
      b = p_IsHomogeneous(Q->m[i],r);
      i++;
    }
  }
  return b;
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
ideal id_FreeModule (int i, const ring r)
{
  int j;
  ideal h;

  h=idInit(i,i);
  for (j=0; j<i; j++)
  {
    h->m[j] = p_One(r);
    p_SetComp(h->m[j],j+1,r);
    p_SetmComp(h->m[j],r);
  }
  return h;
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
static void makemonoms(int vars,int actvar,int deg,int monomdeg, const ring r)
{
  poly p;
  int i=0;

  if ((idpowerpoint == 0) && (actvar ==1))
  {
    idpower[idpowerpoint] = p_One(r);
    monomdeg = 0;
  }
  while (i<=deg)
  {
    if (deg == monomdeg)
    {
      p_Setm(idpower[idpowerpoint],r);
      idpowerpoint++;
      return;
    }
    if (actvar == vars)
    {
      p_SetExp(idpower[idpowerpoint],actvar,deg-monomdeg,r);
      p_Setm(idpower[idpowerpoint],r);
      p_Test(idpower[idpowerpoint],r);
      idpowerpoint++;
      return;
    }
    else
    {
      p = p_Copy(idpower[idpowerpoint],r);
      makemonoms(vars,actvar+1,deg,monomdeg,r);
      idpower[idpowerpoint] = p;
    }
    monomdeg++;
    p_SetExp(idpower[idpowerpoint],actvar,p_GetExp(idpower[idpowerpoint],actvar,r)+1,r);
    p_Setm(idpower[idpowerpoint],r);
    p_Test(idpower[idpowerpoint],r);
    i++;
  }
}

/*2
*returns the deg-th power of the maximal ideal of 0
*/
ideal id_MaxIdeal(int deg, const ring r)
{
  if (deg < 0)
  {
    WarnS("maxideal: power must be non-negative");
  }
  if (deg < 1)
  {
    ideal I=idInit(1,1);
    I->m[0]=p_One(r);
    return I;
  }
  if (deg == 1)
  {
    return id_MaxIdeal(r);
  }

  int vars = rVar(r);
  int i = binom(vars+deg-1,deg);
  if (i<=0) return idInit(1,1);
  ideal id=idInit(i,1);
  idpower = id->m;
  idpowerpoint = 0;
  makemonoms(vars,1,deg,0,r);
  idpower = NULL;
  idpowerpoint = 0;
  return id;
}

static void id_NextPotence(ideal given, ideal result,
  int begin, int end, int deg, int restdeg, poly ap, const ring r)
{
  poly p;
  int i;

  p = p_Power(p_Copy(given->m[begin],r),restdeg,r);
  i = result->nrows;
  result->m[i] = p_Mult_q(p_Copy(ap,r),p,r);
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
    p = p_Power(p_Copy(given->m[begin],r),i,r);
    p = p_Mult_q(p_Copy(ap,r),p,r);
    id_NextPotence(given, result, begin+1, end, deg, restdeg-i, p,r);
    p_Delete(&p,r);
  }
  id_NextPotence(given, result, begin+1, end, deg, restdeg, ap,r);
}

ideal id_Power(ideal given,int exp, const ring r)
{
  ideal result,temp;
  poly p1;
  int i;

  if (idIs0(given)) return idInit(1,1);
  temp = id_Copy(given,r);
  idSkipZeroes(temp);
  i = binom(IDELEMS(temp)+exp-1,exp);
  result = idInit(i,1);
  result->nrows = 0;
//Print("ideal contains %d elements\n",i);
  p1=p_One(r);
  id_NextPotence(temp,result,0,IDELEMS(temp)-1,exp,exp,p1,r);
  p_Delete(&p1,r);
  id_Delete(&temp,r);
  result->nrows = 1;
  id_DelEquals(result,r);
  idSkipZeroes(result);
  return result;
}

/*2
*skips all zeroes and double elements, searches also for units
*/
void id_Compactify(ideal id, const ring r)
{
  int i;
  BOOLEAN b=FALSE;

  i = IDELEMS(id)-1;
  while ((! b) && (i>=0))
  {
    b=p_IsUnit(id->m[i],r);
    i--;
  }
  if (b)
  {
    for(i=IDELEMS(id)-1;i>=0;i--) p_Delete(&id->m[i],r);
    id->m[0]=p_One(r);
  }
  else
  {
    id_DelMultiples(id,r);
  }
  idSkipZeroes(id);
}

/*2
* returns the ideals of initial terms
*/
ideal id_Head(ideal h,const ring r)
{
  ideal m = idInit(IDELEMS(h),h->rank);
  int i;

  for (i=IDELEMS(h)-1;i>=0; i--)
  {
    if (h->m[i]!=NULL) m->m[i]=p_Head(h->m[i],r);
  }
  return m;
}

ideal id_Homogen(ideal h, int varnum,const ring r)
{
  ideal m = idInit(IDELEMS(h),h->rank);
  int i;

  for (i=IDELEMS(h)-1;i>=0; i--)
  {
    m->m[i]=p_Homogen(h->m[i],varnum,r);
  }
  return m;
}

/*------------------type conversions----------------*/
ideal id_Vec2Ideal(poly vec, const ring R)
{
   ideal result=idInit(1,1);
   omFree((ADDRESS)result->m);
   result->m=NULL; // remove later
   p_Vec2Polys(vec, &(result->m), &(IDELEMS(result)),R);
   return result;
}


// converts mat to module, destroys mat
ideal id_Matrix2Module(matrix mat, const ring R)
{
  int mc=MATCOLS(mat);
  int mr=MATROWS(mat);
  ideal result = idInit(si_max(mc,1),si_max(mr,1));
  int i,j,l;
  poly h;
  sBucket_pt bucket = sBucketCreate(R);

  for(j=0;j<mc /*MATCOLS(mat)*/;j++) /* j is also index in result->m */
  {
    for (i=1;i<=mr /*MATROWS(mat)*/;i++)
    {
      h = MATELEM(mat,i,j+1);
      if (h!=NULL)
      {
        l=pLength(h);
        MATELEM(mat,i,j+1)=NULL;
        p_SetCompP(h,i, R);
        sBucket_Merge_p(bucket, h, l);
      }
    }
    sBucketClearMerge(bucket, &(result->m[j]), &l);
  }
  sBucketDestroy(&bucket);

  // obachman: need to clean this up
  id_Delete((ideal*) &mat,R);
  return result;
}

/*2
* converts a module into a matrix, destroyes the input
*/
matrix id_Module2Matrix(ideal mod, const ring R)
{
  matrix result = mpNew(mod->rank,IDELEMS(mod));
  long i; long cp;
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
      cp = si_max((long)1,p_GetComp(h, R));     // if used for ideals too
      //cp = p_GetComp(h,R);
      p_SetComp(h,0,R);
      p_SetmComp(h,R);
#ifdef TEST
      if (cp>mod->rank)
      {
        Print("## inv. rank %ld -> %ld\n",mod->rank,cp);
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
        id_Delete((ideal *)&result,R);
        result=d;
      }
#endif
      MATELEM(result,cp,i+1) = p_Add_q(MATELEM(result,cp,i+1),h,R);
    }
  }
  // obachman 10/99: added the following line, otherwise memory leack!
  id_Delete(&mod,R);
  return result;
}

matrix id_Module2formatedMatrix(ideal mod,int rows, int cols, const ring R)
{
  matrix result = mpNew(rows,cols);
  int i,cp,r=id_RankFreeModule(mod,R),c=IDELEMS(mod);
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
      cp = p_GetComp(h,R);
      if (cp<=r)
      {
        p_SetComp(h,0,R);
        p_SetmComp(h,R);
        MATELEM(result,cp,i+1) = p_Add_q(MATELEM(result,cp,i+1),h,R);
      }
      else
        p_Delete(&h,R);
    }
  }
  id_Delete(&mod,R);
  return result;
}

/*2
* substitute the n-th variable by the monomial e in id
* destroy id
*/
ideal  id_Subst(ideal id, int n, poly e, const ring r)
{
  int k=MATROWS((matrix)id)*MATCOLS((matrix)id);
  ideal res=(ideal)mpNew(MATROWS((matrix)id),MATCOLS((matrix)id));

  res->rank = id->rank;
  for(k--;k>=0;k--)
  {
    res->m[k]=p_Subst(id->m[k],n,e,r);
    id->m[k]=NULL;
  }
  id_Delete(&id,r);
  return res;
}

BOOLEAN id_HomModule(ideal m, ideal Q, intvec **w, const ring R)
{
  if (w!=NULL) *w=NULL;
  if ((Q!=NULL) && (!id_HomIdeal(Q,NULL,R))) return FALSE;
  if (idIs0(m))
  {
    if (w!=NULL) (*w)=new intvec(m->rank);
    return TRUE;
  }

  long cmax=1,order=0,ord,* diff,diffmin=32000;
  int *iscom;
  int i;
  poly p=NULL;
  pFDegProc d;
  if (R->pLexOrder && (R->order[0]==ringorder_lp))
     d=p_Totaldegree;
  else
     d=R->pFDeg;
  int length=IDELEMS(m);
  poly* P=m->m;
  poly* F=(poly*)omAlloc(length*sizeof(poly));
  for (i=length-1;i>=0;i--)
  {
    p=F[i]=P[i];
    cmax=si_max(cmax,(long)p_MaxComp(p,R));
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
      while ((p!=NULL) && (iscom[p_GetComp(p,R)]==0)) pIter(p);
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
      order = d(p,R) +diff[p_GetComp(p,R)];
      //order += diff[pGetComp(p)];
      p = F[i];
//Print("Actual p=F[%d]: ",i);pWrite(p);
      F[i] = NULL;
      i=0;
    }
    while (p!=NULL)
    {
      if (R->pLexOrder && (R->order[0]==ringorder_lp))
        ord=p_Totaldegree(p,R);
      else
      //  ord = p->order;
        ord = R->pFDeg(p,R);
      if (iscom[p_GetComp(p,R)]==0)
      {
        diff[p_GetComp(p,R)] = order-ord;
        iscom[p_GetComp(p,R)] = 1;
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
        if (order != (ord+diff[p_GetComp(p,R)]))
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

ideal id_Jet(ideal i,int d, const ring R)
{
  ideal r=idInit((i->nrows)*(i->ncols),i->rank);
  r->nrows = i-> nrows;
  r->ncols = i-> ncols;
  //r->rank = i-> rank;
  int k;
  for(k=(i->nrows)*(i->ncols)-1;k>=0; k--)
  {
    r->m[k]=pp_Jet(i->m[k],d,R);
  }
  return r;
}

ideal id_JetW(ideal i,int d, intvec * iv, const ring R)
{
  ideal r=idInit(IDELEMS(i),i->rank);
  if (ecartWeights!=NULL)
  {
    WerrorS("cannot compute weighted jets now");
  }
  else
  {
    short *w=iv2array(iv,R);
    int k;
    for(k=0; k<IDELEMS(i); k++)
    {
      r->m[k]=pp_JetW(i->m[k],d,w,R);
    }
    omFreeSize((ADDRESS)w,(rVar(R)+1)*sizeof(short));
  }
  return r;
}

/*3
* searches for the next unit in the components of the module arg and
* returns the first one;
*/
int id_ReadOutPivot(ideal arg,int* comp, const ring r)
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
      j = p_GetComp(p,r);
      if (componentIsUsed[j]==0)
      {
#ifdef HAVE_RINGS
        if (p_LmIsConstantComp(p,r) &&
            (!rField_is_Ring(r) || n_IsUnit(pGetCoeff(p),r->cf)))
        {
#else
        if (p_LmIsConstantComp(p,r))
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

intvec * id_QHomWeight(ideal id, const ring r)
{
  poly head, tail;
  int k;
  int in=IDELEMS(id)-1, ready=0, all=0,
      coldim=rVar(r), rowmax=2*coldim;
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
          IMATELEM(*imat,all,k) = p_GetExpDiff(head,tail,k,r);
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

BOOLEAN id_IsZeroDim(ideal I, const ring r)
{
  BOOLEAN *UsedAxis=(BOOLEAN *)omAlloc0(rVar(r)*sizeof(BOOLEAN));
  int i,n;
  poly po;
  BOOLEAN res=TRUE;
  for(i=IDELEMS(I)-1;i>=0;i--)
  {
    po=I->m[i];
    if ((po!=NULL) &&((n=p_IsPurePower(po,r))!=0)) UsedAxis[n-1]=TRUE;
  }
  for(i=rVar(r)-1;i>=0;i--)
  {
    if(UsedAxis[i]==FALSE) {res=FALSE; break;} // not zero-dim.
  }
  omFreeSize(UsedAxis,rVar(r)*sizeof(BOOLEAN));
  return res;
}

void id_Normalize(ideal I,const ring r) /* for ideal/matrix */
{
  if (rField_has_simple_inverse(r)) return; /* Z/p, GF(p,n), R, long R/C */
  int i;
  for(i=I->nrows*I->ncols-1;i>=0;i--)
  {
    p_Normalize(I->m[i],r);
  }
}

int id_MinDegW(ideal M,intvec *w, const ring r)
{
  int d=-1;
  for(int i=0;i<IDELEMS(M);i++)
  {
    if (M->m[i]!=NULL)
    {
      int d0=p_MinDeg(M->m[i],w,r);
      if(-1<d0&&((d0<d)||(d==-1)))
        d=d0;
    }
  }
  return d;
}

// #include <kernel/clapsing.h>

/*2
* transpose a module
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

ideal id_ChineseRemainder(ideal *xx, number *q, int rl, const ring r)
{
  int cnt=IDELEMS(xx[0])*xx[0]->nrows;
  ideal result=idInit(cnt,xx[0]->rank);
  result->nrows=xx[0]->nrows; // for lifting matrices
  result->ncols=xx[0]->ncols; // for lifting matrices
  int i,j;
  number *x=(number *)omAlloc(rl*sizeof(number));
  poly *p=(poly *)omAlloc(rl*sizeof(poly));
  for(i=cnt-1;i>=0;i--)
  {
    for(j=rl-1;j>=0;j--)
    {
      p[j]=xx[j]->m[i];
    }
    result->m[i]=p_ChineseRemainder(p,x,q,rl,r);
    for(j=rl-1;j>=0;j--)
    {
      xx[j]->m[i]=p[j];
    }
  }
  omFreeSize(p,rl*sizeof(poly));
  omFreeSize(x,rl*sizeof(number));
  for(i=rl-1;i>=0;i--) id_Delete(&(xx[i]),r);
  omFreeSize(xx,rl*sizeof(ideal));
  return result;
}

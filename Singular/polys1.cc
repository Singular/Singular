/****************************************
*  Computer Algebra System SINGULAR     *
****************************************/
/* $Id: polys1.cc,v 1.25 1999-08-23 13:15:58 Singular Exp $ */

/*
* ABSTRACT - all basic methods to manipulate polynomials:
* independent of representation
*/

/* includes */
#include <string.h>
#include "mod2.h"
#include "structs.h"
#include "tok.h"
#include "numbers.h"
#include "mmemory.h"
#include "febase.h"
#include "weight.h"
#include "intvec.h"
#include "longalg.h"
#include "ring.h"
#include "ideals.h"
#include "polys.h"
#include "ipid.h"
#ifdef HAVE_FACTORY
#include "clapsing.h"
#endif

/*-------- several access procedures to monomials -------------------- */
/*2
*test if the monomial is a constant
*/
BOOLEAN   pIsConstant(poly p)
{
  if (p!=NULL)
  {
    int i;
    for (i=pVariables;i;i--)
    {
      if (pGetExp(p,i)!=0) return FALSE;
    }
    if (pGetComp(p)) return FALSE;
  }
  return TRUE;
}

/*2
*test if the monomial is a constant as a vector component
*/
BOOLEAN   pIsConstantComp(poly p)
{
  int i;

  for (i=pVariables;i>0;i--)
  {
    if (pGetExp(p,i)!=0) return FALSE;
  }
  return TRUE;
}

/*2
*test if a monomial /head term is a pure power
*/
int pIsPurePower(poly p)
{
  pTest(p);
  int i,k=0;

  for (i=pVariables;i>0;i--)
  {
    if (pGetExp(p,i)!=0)
    {
      if(k!=0) return 0;
      k=i;
    }
  }
  return k;
}

/*-----------------------------------------------------------*/
/*
* the module weights for std
*/
static pFDegProc pOldFDeg;
static intvec * pModW;
static BOOLEAN pOldLexOrder;

static int pModDeg(poly p)
{
  return pOldFDeg(p)+(*pModW)[pGetComp(p)-1];
}

void pSetModDeg(intvec *w)
{
  if (w!=NULL)
  {
    pModW = w;
    pOldFDeg = pFDeg;
    pOldLexOrder = pLexOrder;
    pFDeg = pModDeg;
    pLexOrder = TRUE;
  }
  else
  {
    pModW = NULL;
    pFDeg = pOldFDeg;
    pLexOrder = pOldLexOrder;
  }
}

/*-------------operations on polynomials:------------*/
/*2
* add p1 and p2, p1 and p2 are destroyed
*/
poly pAdd(poly p1, poly p2)
{
  static poly a1, p, a2, a;
  int c;
  number t;

  if (p1==NULL) return p2;
  if (p2==NULL) return p1;
  a1 = p1;
  a2 = p2;
  a = p  = pInit();
  nNew(&(p->coef));
  loop
  {
    /* a1 and a2 are non-NULL, so we may use pComp0 instead of pComp */
    c = pComp0(a1, a2);
    if (c == 1)
    {
      a = pNext(a) = a1;
      pIter(a1);
      if (a1==NULL)
      {
        pNext(a) = a2;
        break;
      }
    }
    else if (c == -1)
    {
      a = pNext(a) = a2;
      pIter(a2);
      if (a2==NULL)
      {
        pNext(a) = a1;
        break;
      }
    }
    else
    {
      t=nAdd(a1->coef,a2->coef);
      pDelete1(&a2);
      if (nIsZero(t))
      {
        nDelete(&t);
        pDelete1(&a1);
      }
      else
      {
        pSetCoeff(a1,t);
        a = pNext(a) = a1;
        pIter(a1);
      }
      if (a1==NULL)
      {
        pNext(a) = a2;
        break;
      }
      else if (a2==NULL)
      {
        pNext(a) = a1;
        break;
      }
    }
  }
  pDelete1(&p);
  return p;
}

/*2
* multiply a polynomial by -1
*/
poly pNeg(poly p)
{
  poly h = p;
  while (h!=NULL)
  {
    pGetCoeff(h)=nNeg(pGetCoeff(h));
    pIter(h);
  }
  return p;
}

/*2
* subtract p2 from p1, p1 and p2 are destroyed
* do not put attention on speed: the procedure is only used in the interpreter
*/
poly pSub(poly p1, poly p2)
{
  return pAdd(p1, pNeg(p2));
}

/*3
*  create binomial coef.
*/
static number* pnBin(int exp)
{
  int e, i, h;
  number x, y, *bin=NULL;

  x = nInit(exp);
  if (nIsZero(x))
  {
    nDelete(&x);
    return bin;
  }
  h = (exp >> 1) + 1;
  bin = (number *)Alloc0(h*sizeof(number));
  bin[1] = x;
  if (exp < 4)
    return bin;
  i = exp - 1;
  for (e=2; e<h; e++)
  {
//    if(!nIsZero(bin[e-1]))
//    {
      x = nInit(i);
      i--;
      y = nMult(x,bin[e-1]);
      nDelete(&x);
      x = nInit(e);
      bin[e] = nIntDiv(y,x);
      nDelete(&x);
      nDelete(&y);
//    }
//    else
//    {
//      bin[e] = nInit(binom(exp,e));
//    }
  }
  return bin;
}

static void pnFreeBin(number *bin, int exp)
{
  int e, h = (exp >> 1) + 1;

  if (bin[1] != NULL)
  {
    for (e=1; e<h; e++)
      nDelete(&(bin[e]));
  }
  Free((ADDRESS)bin, h*sizeof(number));
}

/*3
* compute for a monomial m
* the power m^exp, exp > 1
* destroys p
*/
static poly pMonPower(poly p, int exp)
{
  int i;

  if(!nIsOne(pGetCoeff(p)))
  {
    number x, y;
    y = pGetCoeff(p);
    nPower(y,exp,&x);
    nDelete(&y);
    pSetCoeff0(p,x);
  }
  for (i=pVariables; i!=0; i--)
  {
    pMultExp(p,i, exp);
  }
  #ifdef TEST_MAC_ORDER
  if (bNoAdd)
    pSetm(p);
  else
  #endif
    p->Order *= exp;
  return p;
}

/*3
* compute for monomials p*q
* destroys p, keeps q
*/
static void pMonMult(poly p, poly q)
{
  number x, y;
  int i;

  y = pGetCoeff(p);
  x = nMult(y,pGetCoeff(q));
  nDelete(&y);
  pSetCoeff0(p,x);
  for (i=pVariables; i!=0; i--)
  {
    pAddExp(p,i, pGetExp(q,i));
  }
  p->Order += q->Order;
}

/*3
* compute for monomials p*q
* keeps p, q
*/
static poly pMonMultC(poly p, poly q)
{
  number x;
  int i;
  poly r = pInit();

  x = nMult(pGetCoeff(p),pGetCoeff(q));
  pSetCoeff0(r,x);
  pSetComp(r, pGetComp(p));
  for (i=pVariables; i!=0; i--)
  {
    pSetExp(r,i, pGetExp(p,i) + pGetExp(q,i));
  }
  r->Order = p->Order + q->Order;
  return r;
}

/*
*  compute for a poly p = head+tail, tail is monomial
*          (head + tail)^exp, exp > 1
*          with binomial coef.
*/
static poly pTwoMonPower(poly p, int exp)
{
  int eh, e, al;
  poly *a;
  poly tail, b, res, h;
  number x;
  number *bin = pnBin(exp);

  tail = pNext(p);
  if (bin == NULL)
  {
    pMonPower(p,exp);
    pMonPower(tail,exp);
#ifdef PDEBUG
    pTest(p);
#endif
    return p;
  }
  eh = exp >> 1;
  al = (exp + 1) * sizeof(poly);
  a = (poly *)Alloc(al);
  a[1] = p;
  for (e=1; e<exp; e++)
  {
    a[e+1] = pMonMultC(a[e],p);
  }
  res = a[exp];
  b = pHead(tail);
  for (e=exp-1; e>eh; e--)
  {
    h = a[e];
    x = nMult(bin[exp-e],pGetCoeff(h));
    pSetCoeff(h,x);
    pMonMult(h,b);
    res = pNext(res) = h;
    pMonMult(b,tail);
  }
  for (e=eh; e!=0; e--)
  {
    h = a[e];
    x = nMult(bin[e],pGetCoeff(h));
    pSetCoeff(h,x);
    pMonMult(h,b);
    res = pNext(res) = h;
    pMonMult(b,tail);
  }
  pDelete1(&tail);
  pNext(res) = b;
  pNext(b) = NULL;
  res = a[exp];
  Free((ADDRESS)a, al);
  pnFreeBin(bin, exp);
//  tail=res;
// while((tail!=NULL)&&(pNext(tail)!=NULL))
// {
//   if(nIsZero(pGetCoeff(pNext(tail))))
//   {
//     pDelete1(&pNext(tail));
//   }
//   else
//     pIter(tail);
// }
#ifdef PDEBUG
  pTest(res);
#endif
  return res;
}

static poly pPow(poly p, int i)
{
  poly rc = pCopy(p);
  i -= 2;
  do
  {
    rc = pMult(rc,pCopy(p));
    i--;
  }
  while (i != 0);
  return pMult(rc,p);
}

/*2
* returns the i-th power of p
* p will be destroyed
*/
poly pPower(poly p, int i)
{
  if (i==0)
    return pOne();

  poly rc=NULL;

  if(p!=NULL)
  {
    if (i > EXPONENT_MAX)
    {
      Werror("exponent is too large, max. is %d",EXPONENT_MAX);
      return NULL;
    }
    switch (i)
    {
      case 0:
      {
        rc=pOne();
#ifdef DRING
        if ((pDRING) && (pdDFlag(p)==1))
        {
          pdSetDFlag(rc,1);
        }
#endif
        pDelete(&p);
        break;
      }
      case 1:
        rc=p;
        break;
      case 2:
        rc=pMult(pCopy(p),p);
        break;
      default:
        if (i < 0)
        {
#ifdef DRING
          if (pDRING)
          {
            int j,t;
            rc=p;
            while(p!=NULL)
            {
              for(j=1;j<=pdK;j++)
              {
                if(pGetExp(p,pdY(j))!=0)
                {
                  pDelete(&rc);
                  return NULL;
                }
              }
              for(j=1;j<=pdN;j++)
              {
                t=pGetExp(p,pdX(j));
                pSetExp(p,pdX(j),pGetExp(p,pdIX(j)));
                pSetExp(p,pdIX(j),t);
              }
              pIter(p);
            }
            return pPower(rc,-i);
          }
          else
#endif
          {
            pDelete(&p);
            return NULL;
          }
        }
#ifdef SDRING
        if(pSDRING)
        {
          return pPow(p,i);
        }
        else
#endif
        {
          rc = pNext(p);
          if (rc == NULL)
            return pMonPower(p,i);
          /* else: binom */
          int char_p=rChar(currRing);
          if (pNext(rc) != NULL)
            return pPow(p,i);
          if ((char_p==0) || (i<=char_p))
            return pTwoMonPower(p,i);
          poly p_p=pTwoMonPower(pCopy(p),char_p);
          return pMult(pPower(p,i-char_p),p_p);
        }
      /*end default:*/
    }
  }
  return rc;
}

/*2
* returns the partial differentiate of a by the k-th variable
* does not destroy the input
*/
poly pDiff(poly a, int k)
{
  poly res, f, last;
  number t;

  last = res = NULL;
  while (a!=NULL)
  {
    if (pGetExp(a,k)!=0)
    {
      f = pNew();
      pCopy2(f,a);
      pNext(f)=NULL;
      t = nInit(pGetExp(a,k));
      pSetCoeff0(f,nMult(t,pGetCoeff(a)));
      nDelete(&t);
      if (nIsZero(pGetCoeff(f)))
        pDelete1(&f);
      else
      {
        pDecrExp(f,k);
        pSetm(f);
        if (res==NULL)
        {
          res=last=f;
        }
        else
        {
          pNext(last)=f;
          last=f;
        }
        //res = pAdd(res, f);
      }
    }
    pIter(a);
  }
  return res;
}

static poly pDiffOpM(poly a, poly b,BOOLEAN multiply)
{
  int i,j,s;
  number n,h,hh;
  poly p=pOne();
  n=nMult(pGetCoeff(a),pGetCoeff(b));
  for(i=pVariables;i>0;i--)
  {
    s=pGetExp(b,i);
    if (s<pGetExp(a,i))
    {
      nDelete(&n);
      pDelete1(&p);
      return NULL;
    }
    if (multiply)
    {
      for(j=pGetExp(a,i); j>0;j--)
      {
        h = nInit(s);
        hh=nMult(n,h);
        nDelete(&h);
        nDelete(&n);
        n=hh;
        s--;
      }
      pSetExp(p,i,s);
    }
    else
    {
      pSetExp(p,i,s-pGetExp(a,i));
    }
  }
  pSetm(p);
  /*if (multiply)*/ pSetCoeff(p,n);
  return p;
}

poly pDiffOp(poly a, poly b,BOOLEAN multiply)
{
  poly result=NULL;
  poly h;
  for(;a!=NULL;pIter(a))
  {
    for(h=b;h!=NULL;pIter(h))
    {
      result=pAdd(result,pDiffOpM(a,h,multiply));
    }
  }
  return result;
}

/*2
* returns the length of a (numbers of monomials)
*/
int pLength(poly a)
{
  int l = 0;

  while (a!=NULL)
  {
    pIter(a);
    l++;
  }
  return l;
}


void pSplit(poly p, poly *h)
{
  *h=pNext(p);
  pNext(p)=NULL;
}

/*2
* returns maximal column number in the modul element a (or 0)
*/
int pMaxComp(poly p)
{
  int result,i;

  if(p==NULL) return 0;
  result = pGetComp(p);
  while (pNext(p)!=NULL)
  {
    pIter(p);
    i = pGetComp(p);
    if (i>result) result = i;
  }
  return result;
}

/*2
* returns minimal column number in the modul element a (or 0)
*/
int pMinComp(poly p)
{
  int result,i;

  if(p==NULL) return 0;
  result = pGetComp(p);
  while (pNext(p)!=NULL)
  {
    pIter(p);
    i = pGetComp(p);
    if (i<result) result = i;
  }
  return result;
}

/*2
* returns TRUE, if all monoms have the same component
*/
BOOLEAN pOneComp(poly p)
{
  if(p!=NULL)
  {
    int i = pGetComp(p);
    while (pNext(p)!=NULL)
    {
      pIter(p);
      if(i != pGetComp(p)) return FALSE;
    }
  }
  return TRUE;
}

/*2
* multiplies the polynomial a by the column generator with number i
*/
void pSetCompP(poly p, int i)
{

  if ((p!=NULL) && (pGetComp(p)==0))
  {
    do
    {
      pSetComp(p, (Exponent_t)i);
      pIter(p);
    } while (p!=NULL);
  }
}

/*2
* handle memory request for sets of polynomials (ideals)
* l is the length of *p, increment is the difference (may be negative)
*/
void pEnlargeSet(polyset *p, int l, int increment)
{
  int i;
  polyset h;

  h=(polyset)ReAlloc((poly*)*p,l*sizeof(poly),(l+increment)*sizeof(poly));
  if (increment>0)
  {
    //for (i=l; i<l+increment; i++)
    //  h[i]=NULL;
    memset(&(h[l]),0,increment*sizeof(poly));
  }
  *p=h;
}

/*2
* returns a polynomial representing the integer i
*/
poly pISet(int i)
{
  poly rc = NULL;
  if (i!=0)
  {
    rc = pInit();
    pSetCoeff0(rc,nInit(i));
    if (nIsZero(pGetCoeff(rc)))
      pDelete1(&rc);
#ifdef TEST_MAC_ORDER
    else if (bNoAdd)
      pSetm(rc);
#endif
  }
  return rc;
}

void pContent(poly ph)
{
  number h,d;
  poly p;

  if(pNext(ph)==NULL)
  {
    pSetCoeff(ph,nInit(1));
  }
  else
  {
#ifdef PDEBUG
    if (!pTest(ph)) return;
#endif
    nNormalize(pGetCoeff(ph));
    if(!nGreaterZero(pGetCoeff(ph))) ph = pNeg(ph);
    h=nCopy(pGetCoeff(ph));
    p = pNext(ph);
    while (p!=NULL)
    {
      nNormalize(pGetCoeff(p));
      d=nGcd(h,pGetCoeff(p));
      nDelete(&h);
      h = d;
      if(nIsOne(h))
      {
        break;
      }
      pIter(p);
    }
    p = ph;
    //number tmp;
    if(!nIsOne(h))
    {
      while (p!=NULL)
      {
        //d = nDiv(pGetCoeff(p),h);
        //tmp = nIntDiv(pGetCoeff(p),h);
        //if (!nEqual(d,tmp))
        //{
        //  StringSetS("** div0:");nWrite(pGetCoeff(p));StringAppendS("/");
        //  nWrite(h);StringAppendS("=");nWrite(d);StringAppendS(" int:");
        //  nWrite(tmp);Print(StringAppendS("\n"));
        //}
        //nDelete(&tmp);
        d = nIntDiv(pGetCoeff(p),h);
        pSetCoeff(p,d);
        pIter(p);
      }
    }
    nDelete(&h);
#ifdef HAVE_FACTORY
    if ( (nGetChar() == 1) || (nGetChar() < 0) ) /* Q[a],Q(a),Zp[a],Z/p(a) */
    {
      singclap_divide_content(ph);
      if(!nGreaterZero(pGetCoeff(ph))) ph = pNeg(ph);
    }
#endif
  }
  pTest(ph);
}

//void pContent(poly ph)
//{
//  number h,d;
//  poly p;
//
//  p = ph;
//  if(pNext(p)==NULL)
//  {
//    pSetCoeff(p,nInit(1));
//  }
//  else
//  {
//#ifdef PDEBUG
//    if (!pTest(p)) return;
//#endif
//    nNormalize(pGetCoeff(p));
//    if(!nGreaterZero(pGetCoeff(ph)))
//    {
//      ph = pNeg(ph);
//      nNormalize(pGetCoeff(p));
//    }
//    h=pGetCoeff(p);
//    pIter(p);
//    while (p!=NULL)
//    {
//      nNormalize(pGetCoeff(p));
//      if (nGreater(h,pGetCoeff(p))) h=pGetCoeff(p);
//      pIter(p);
//    }
//    h=nCopy(h);
//    p=ph;
//    while (p!=NULL)
//    {
//      d=nGcd(h,pGetCoeff(p));
//      nDelete(&h);
//      h = d;
//      if(nIsOne(h))
//      {
//        break;
//      }
//      pIter(p);
//    }
//    p = ph;
//    //number tmp;
//    if(!nIsOne(h))
//    {
//      while (p!=NULL)
//      {
//        d = nIntDiv(pGetCoeff(p),h);
//        pSetCoeff(p,d);
//        pIter(p);
//      }
//    }
//    nDelete(&h);
//#ifdef HAVE_FACTORY
//    if ( (nGetChar() == 1) || (nGetChar() < 0) ) /* Q[a],Q(a),Zp[a],Z/p(a) */
//    {
//      pTest(ph);
//      singclap_divide_content(ph);
//      pTest(ph);
//    }
//#endif
//  }
//}

void pCleardenom(poly ph)
{
  number d, h;
  poly p;

  p = ph;
  if(pNext(p)==NULL)
  {
    pSetCoeff(p,nInit(1));
  }
  else
  {
    h = nInit(1);
    while (p!=NULL)
    {
      nNormalize(pGetCoeff(p));
      d=nLcm(h,pGetCoeff(p));
      nDelete(&h);
      h=d;
      pIter(p);
    }
    /* contains the 1/lcm of all denominators */
    if(!nIsOne(h))
    {
      p = ph;
      while (p!=NULL)
      {
        /* should be:
        * number hh;
        * nGetDenom(p->coef,&hh);
        * nMult(&h,&hh,&d);
        * nNormalize(d);
        * nDelete(&hh);
        * nMult(d,p->coef,&hh);
        * nDelete(&d);
        * nDelete(&(p->coef));
        * p->coef =hh;
        */
        d=nMult(h,pGetCoeff(p));
        nNormalize(d);
        pSetCoeff(p,d);
        pIter(p);
      }
      nDelete(&h);
      if (nGetChar()==1)
      {
        h = nInit(1);
        p=ph;
        while (p!=NULL)
        {
          d=nLcm(h,pGetCoeff(p));
          nDelete(&h);
          h=d;
          pIter(p);
        }
        /* contains the 1/lcm of all denominators */
        if(!nIsOne(h))
        {
          p = ph;
          while (p!=NULL)
          {
            /* should be:
            * number hh;
            * nGetDenom(p->coef,&hh);
            * nMult(&h,&hh,&d);
            * nNormalize(d);
            * nDelete(&hh);
            * nMult(d,p->coef,&hh);
            * nDelete(&d);
            * nDelete(&(p->coef));
            * p->coef =hh;
            */
            d=nMult(h,pGetCoeff(p));
            nNormalize(d);
            pSetCoeff(p,d);
            pIter(p);
          }
          nDelete(&h);
        }
      }
    }
    pContent(ph);
  }
}

/*2
*tests if p is homogeneous with respect to the actual weigths
*/
BOOLEAN pIsHomogeneous (poly p)
{
  poly qp=p;
  int o;

  if ((p == NULL) || (pNext(p) == NULL)) return TRUE;
  pFDegProc d=(pLexOrder ? pTotaldegree : pFDeg );
  o = d(p);
  do
  {
    if (d(qp) != o) return FALSE;
    pIter(qp);
  }
  while (qp != NULL);
  return TRUE;
}

// orders monoms of poly using merge sort (ususally faster than
// insertion sort). ASSUMES that pSetm was performed on monoms
// (i.e. that Order field is set correctly)
poly pOrdPolyMerge(poly p)
{
  poly qq,pp,result=NULL;

  if (p == NULL) return NULL;

  loop
  {
    qq = p;
    loop
    {
      if (pNext(p) == NULL) return pAdd(result, qq);
      if (pComp(p,pNext(p)) != 1)
      {
        pp = p;
        pIter(p);
        pNext(pp) = NULL;
        result = pAdd(result, qq);
        break;
      }
      pIter(p);
    }
  }
}

// orders monoms of poly using insertion sort, performs pSetm on each
// monom (i.e. sets Order field)
poly pOrdPolyInsertSetm(poly p)
{
  poly qq,result = NULL;

#if 0
  while (p != NULL)
  {
    qq = p;
    pIter(p);
    qq->next = NULL;
    pSetm(qq);
    result = pAdd(result,qq);
    pTest(result);
  }
#else
  while (p != NULL)
  {
    qq = p;
    pIter(p);
    qq->next = result;
    result = qq;
    pSetm(qq);
  }
  p = result;
  result = NULL;
  while (p != NULL)
  {
    qq = p;
    pIter(p);
    qq->next = NULL;
    result = pAdd(result, qq);
  }
  pTest(result);
#endif
  return result;
}

/*2
*returns a re-ordered copy of a polynomial, with permutation of the variables
*/
poly pPermPoly (poly p, int * perm, ring oldRing,
   int *par_perm, int OldPar)
{
  int OldpVariables = oldRing->N;
  poly result = NULL;
  poly result_last = NULL;
  poly aq=NULL; /* the map coefficient */
  poly qq; /* the mapped monomial */

  while (p != NULL)
  {
    if (OldPar==0)
    {
      qq = pInit();
      pGetCoeff(qq)=nMap(pGetCoeff(p));
    }
    else
    {
      qq=pOne();
      aq=naPermNumber(pGetCoeff(p),par_perm,OldPar);
      pTest(aq);
    }
    pSetComp(qq, pRingGetComp(oldRing,p));
    if (nIsZero(pGetCoeff(qq)))
    {
      pDelete1(&qq);
    }
    else
    {
      int i;
      for(i=1; i<=OldpVariables; i++)
      {
        if (pRingGetExp(oldRing,p,i)!=0)
        {
          if (perm==NULL)
          {
            pSetExp(qq,i, pRingGetExp(oldRing,p,i));
          }
          else if (perm[i]>0)
            pAddExp(qq,perm[i], pRingGetExp(oldRing, p,i));
          else if (perm[i]<0)
          {
            lnumber c=(lnumber)pGetCoeff(qq);
            c->z->e[-perm[i]-1]+=pRingGetExp(oldRing, p,i);
          }
          else
          {
            /* this variable maps to 0 !*/
            pDelete1(&qq);
            break;
          }
        }
      }
    }
    pIter(p);
#if 1
    if (qq!=NULL)
    {
      pSetm(qq);
      pTest(qq);
      pTest(aq);
      if (aq!=NULL) qq=pMult(aq,qq);
      aq = qq;
      while (pNext(aq) != NULL) pIter(aq);
      if (result_last==NULL)
      {
        result=qq;
      }
      else
      {
        pNext(result_last)=qq;
      }
      result_last=aq;
      aq = NULL;
    }
    else if (aq!=NULL)
    {
      pDelete(&aq);
    }
  }
  result=pOrdPolyMerge(result);
#else
  //  if (qq!=NULL)
  //  {
  //    pSetm(qq);
  //    pTest(qq);
  //    pTest(aq);
  //    if (aq!=NULL) qq=pMult(aq,qq);
  //    aq = qq;
  //    while (pNext(aq) != NULL) pIter(aq);
  //    pNext(aq) = result;
  //    aq = NULL;
  //    result = qq;
  //  }
  //  else if (aq!=NULL)
  //  {
  //    pDelete(&aq);
  //  }
  //}
  //p = result;
  //result = NULL;
  //while (p != NULL)
  //{
  //  qq = p;
  //  pIter(p);
  //  qq->next = NULL;
  //  result = pAdd(result, qq);
  //}
#endif
  pTest(result);
  return result;
}

poly pJet(poly p, int m)
{
  poly r=NULL;
  poly t=NULL;

  while (p!=NULL)
  {
    if (pTotaldegree(p)<=m)
    {
      if (r==NULL)
        r=pHead(p);
      else
      if (t==NULL)
      {
        pNext(r)=pHead(p);
        t=pNext(r);
      }
      else
      {
        pNext(t)=pHead(p);
        pIter(t);
      }
    }
    pIter(p);
  }
  return r;
}

poly pJetW(poly p, int m, short *w)
{
  poly r=NULL;
  poly t=NULL;
  short *wsave=ecartWeights;

  ecartWeights=w;

  while (p!=NULL)
  {
    if (totaldegreeWecart(p)<=m)
    {
      if (r==NULL)
        r=pHead(p);
      else
      if (t==NULL)
      {
        pNext(r)=pHead(p);
        t=pNext(r);
      }
      else
      {
        pNext(t)=pHead(p);
        pIter(t);
      }
    }
    pIter(p);
  }
  ecartWeights=wsave;
  return r;
}

int pDegW(poly p, short *w)
{
  int r=0;
  short *wsave=ecartWeights;

  ecartWeights=w;

  while (p!=NULL)
  {
    r=max(r, totaldegreeWecart(p));
    pIter(p);
  }
  ecartWeights=wsave;
  return r;
}

/*-----------type conversions ----------------------------*/
/*2
* input: a set of polys (len elements: p[0]..p[len-1])
* output: a vector
* p will not be changed
*/
poly  pPolys2Vec(polyset p, int len)
{
  poly v=NULL;
  poly h;
  int i;

  for (i=len-1; i>=0; i--)
  {
    if (p[i])
    {
      h=pCopy(p[i]);
      pSetCompP(h,i+1);
      v=pAdd(v,h);
    }
  }
 return v;
}

/*2
* convert a vector to a set of polys,
* allocates the polyset, (entries 0..(*len)-1)
* the vector will not be changed
*/
void  pVec2Polys(poly v, polyset *p, int *len)
{
  poly h;
  int k;

  *len=pMaxComp(v);
  if (*len==0) *len=1;
  *p=(polyset)Alloc0((*len)*sizeof(poly));
  while (v!=NULL)
  {
    h=pHead(v);
    k=pGetComp(h);
    pSetComp(h,0);
    (*p)[k-1]=pAdd((*p)[k-1],h);
    pIter(v);
  }
}

int pVar(poly m)
{
  if (m==NULL) return 0;
  if (pNext(m)!=NULL) return 0;
  int i,e=0;
  for (i=pVariables; i>0; i--)
  {
    if (pGetExp(m,i)==1)
    {
      if (e==0) e=i;
      else return 0;
    }
  }
  return e;
}

/*----------utilities for syzygies--------------*/
//BOOLEAN   pVectorHasUnitM(poly p, int * k)
//{
//  while (p!=NULL)
//  {
//    if (pIsConstantComp(p))
//    {
//      *k = pGetComp(p);
//      return TRUE;
//    }
//    else pIter(p);
//  }
//  return FALSE;
//}

BOOLEAN   pVectorHasUnitB(poly p, int * k)
{
  poly q=p,qq;
  int i;

  while (q!=NULL)
  {
    if (pIsConstantComp(q))
    {
      i = pGetComp(q);
      qq = p;
      while ((qq != q) && (pGetComp(qq) != i)) pIter(qq);
      if (qq == q)
      {
        *k = i;
        return TRUE;
      }
      else
        pIter(q);
    }
    else pIter(q);
  }
  return FALSE;
}

void   pVectorHasUnit(poly p, int * k, int * len)
{
  poly q=p,qq;
  int i,j=0;

  *len = 0;
  while (q!=NULL)
  {
    if (pIsConstantComp(q))
    {
      i = pGetComp(q);
      qq = p;
      while ((qq != q) && (pGetComp(qq) != i)) pIter(qq);
      if (qq == q)
      {
       j = 0;
       while (qq!=NULL)
       {
         if (pGetComp(qq)==i) j++;
        pIter(qq);
       }
       if ((*len == 0) || (j<*len))
       {
         *len = j;
         *k = i;
       }
      }
    }
    pIter(q);
  }
}

/*2
* returns TRUE if p1 = p2
*/
BOOLEAN pEqualPolys(poly p1,poly p2)
{
  while ((p1 != NULL) && (p2 != NULL))
  {
    /* p1 and p2 are non-NULL, so we may use pComp0 instead of pComp */
    if (! pEqual(p1, p2))
    {
       return FALSE;
    }
    if (nEqual(pGetCoeff(p1),pGetCoeff(p2)) == FALSE)
    {
      return FALSE;
    }
    pIter(p1);
    pIter(p2);
  }
  return (p1==p2);
}

/*2
*returns TRUE if p1 is a skalar multiple of p2
*assume p1 != NULL and p2 != NULL
*/
BOOLEAN pComparePolys(poly p1,poly p2)
{
  number n,nn;
  int i;

  if (pLength(p1) != pLength(p2))
    return FALSE;
  n=nDiv(pGetCoeff(p1),pGetCoeff(p2));
  while ((p1 != NULL) /*&& (p2 != NULL)*/)
  {
    if ( ! pLmEqual(p1, p2))
    {
        nDelete(&n);
        return FALSE;
    }
    if (!nEqual(pGetCoeff(p1),nn=nMult(pGetCoeff(p2),n)))
    {
      nDelete(&n);
      nDelete(&nn);
      return FALSE;
    }
    nDelete(&nn);
    pIter(p1);
    pIter(p2);
  }
  nDelete(&n);
  return TRUE;
}

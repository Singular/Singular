/****************************************
*  Computer Algebra System SINGULAR     *
****************************************/
/* $Id$ */

/*
* ABSTRACT - all basic methods to manipulate polynomials:
* independent of representation
*/

/* includes */
#include <string.h>
#include <kernel/mod2.h>
#include <kernel/options.h>
#include <kernel/numbers.h>
#include <kernel/ffields.h>
#include <omalloc.h>
#include <kernel/febase.h>
#include <kernel/weight.h>
#include <kernel/intvec.h>
#include <kernel/longalg.h>
#include <kernel/ring.h>
#include <kernel/ideals.h>
#include <kernel/polys.h>
//#include "ipid.h"
#ifdef HAVE_FACTORY
#include <kernel/clapsing.h>
#endif

#ifdef HAVE_RATGRING
#include <kernel/ratgring.h>
#endif

/*-------- several access procedures to monomials -------------------- */
/*
* the module weights for std
*/
static pFDegProc pOldFDeg;
static pLDegProc pOldLDeg;
static intvec * pModW;
static BOOLEAN pOldLexOrder;

static long pModDeg(poly p, ring r = currRing)
{
  long d=pOldFDeg(p, r);
  int c=p_GetComp(p, r);
  if ((c>0) && (pModW->range(c-1))) d+= (*pModW)[c-1];
  return d;
  //return pOldFDeg(p, r)+(*pModW)[p_GetComp(p, r)-1];
}

void pSetModDeg(intvec *w)
{
  if (w!=NULL)
  {
    pModW = w;
    pOldFDeg = pFDeg;
    pOldLDeg = pLDeg;
    pOldLexOrder = pLexOrder;
    pSetDegProcs(pModDeg);
    pLexOrder = TRUE;
  }
  else
  {
    pModW = NULL;
    pRestoreDegProcs(pOldFDeg, pOldLDeg);
    pLexOrder = pOldLexOrder;
  }
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
  bin = (number *)omAlloc0(h*sizeof(number));
  bin[1] = x;
  if (exp < 4)
    return bin;
  i = exp - 1;
  for (e=2; e<h; e++)
  {
      x = nInit(i);
      i--;
      y = nMult(x,bin[e-1]);
      nDelete(&x);
      x = nInit(e);
      bin[e] = nIntDiv(y,x);
      nDelete(&x);
      nDelete(&y);
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
  omFreeSize((ADDRESS)bin, h*sizeof(number));
}

/*3
* compute for a monomial m
* the power m^exp, exp > 1
* destroys p
*/
static poly p_MonPower(poly p, int exp, const ring r)
{
  int i;

  if(!n_IsOne(pGetCoeff(p),r))
  {
    number x, y;
    y = pGetCoeff(p);
    n_Power(y,exp,&x,r);
    n_Delete(&y,r);
    pSetCoeff0(p,x);
  }
  for (i=rVar(r); i!=0; i--)
  {
    p_MultExp(p,i, exp,r);
  }
  p_Setm(p,r);
  return p;
}

/*3
* compute for monomials p*q
* destroys p, keeps q
*/
static void p_MonMult(poly p, poly q, const ring r)
{
  number x, y;
  int i;

  y = pGetCoeff(p);
  x = n_Mult(y,pGetCoeff(q),r);
  n_Delete(&y,r);
  pSetCoeff0(p,x);
  //for (i=pVariables; i!=0; i--)
  //{
  //  pAddExp(p,i, pGetExp(q,i));
  //}
  //p->Order += q->Order;
  p_ExpVectorAdd(p,q,r);
}

/*3
* compute for monomials p*q
* keeps p, q
*/
static poly p_MonMultC(poly p, poly q, const ring rr)
{
  number x;
  int i;
  poly r = p_Init(rr);

  x = n_Mult(pGetCoeff(p),pGetCoeff(q),rr);
  pSetCoeff0(r,x);
  p_ExpVectorSum(r,p, q, rr);
  return r;
}

/*
*  compute for a poly p = head+tail, tail is monomial
*          (head + tail)^exp, exp > 1
*          with binomial coef.
*/
static poly p_TwoMonPower(poly p, int exp, const ring r)
{
  int eh, e;
  long al;
  poly *a;
  poly tail, b, res, h;
  number x;
  number *bin = pnBin(exp);

  tail = pNext(p);
  if (bin == NULL)
  {
    p_MonPower(p,exp,r);
    p_MonPower(tail,exp,r);
#ifdef PDEBUG
    p_Test(p,r);
#endif
    return p;
  }
  eh = exp >> 1;
  al = (exp + 1) * sizeof(poly);
  a = (poly *)omAlloc(al);
  a[1] = p;
  for (e=1; e<exp; e++)
  {
    a[e+1] = p_MonMultC(a[e],p,r);
  }
  res = a[exp];
  b = p_Head(tail,r);
  for (e=exp-1; e>eh; e--)
  {
    h = a[e];
    x = n_Mult(bin[exp-e],pGetCoeff(h),r);
    p_SetCoeff(h,x,r);
    p_MonMult(h,b,r);
    res = pNext(res) = h;
    p_MonMult(b,tail,r);
  }
  for (e=eh; e!=0; e--)
  {
    h = a[e];
    x = n_Mult(bin[e],pGetCoeff(h),r);
    p_SetCoeff(h,x,r);
    p_MonMult(h,b,r);
    res = pNext(res) = h;
    p_MonMult(b,tail,r);
  }
  p_LmDelete(&tail,r);
  pNext(res) = b;
  pNext(b) = NULL;
  res = a[exp];
  omFreeSize((ADDRESS)a, al);
  pnFreeBin(bin, exp);
//  tail=res;
// while((tail!=NULL)&&(pNext(tail)!=NULL))
// {
//   if(nIsZero(pGetCoeff(pNext(tail))))
//   {
//     pLmDelete(&pNext(tail));
//   }
//   else
//     pIter(tail);
// }
#ifdef PDEBUG
  p_Test(res,r);
#endif
  return res;
}

static poly p_Pow(poly p, int i, const ring r)
{
  poly rc = p_Copy(p,r);
  i -= 2;
  do
  {
    rc = p_Mult_q(rc,p_Copy(p,r),r);
    p_Normalize(rc,r);
    i--;
  }
  while (i != 0);
  return p_Mult_q(rc,p,r);
}

/*2
* returns the i-th power of p
* p will be destroyed
*/
poly p_Power(poly p, int i, const ring r)
{
  poly rc=NULL;

  if (i==0)
  {
    p_Delete(&p,r);
    return p_One(r);
  }

  if(p!=NULL)
  {
    if ( (i > 0) && ((unsigned long ) i > (r->bitmask)))
    {
      Werror("exponent %d is too large, max. is %ld",i,r->bitmask);
      return NULL;
    }
    switch (i)
    {
// cannot happen, see above
//      case 0:
//      {
//        rc=pOne();
//        pDelete(&p);
//        break;
//      }
      case 1:
        rc=p;
        break;
      case 2:
        rc=p_Mult_q(p_Copy(p,r),p,r);
        break;
      default:
        if (i < 0)
        {
          p_Delete(&p,r);
          return NULL;
        }
        else
        {
#ifdef HAVE_PLURAL
          if (rIsPluralRing(r)) /* in the NC case nothing helps :-( */
          {
            int j=i;
            rc = p_Copy(p,r);
            while (j>1)
            {
              rc = p_Mult_q(p_Copy(p,r),rc,r);
              j--;
            }
            p_Delete(&p,r);
            return rc;
          }
#endif
          rc = pNext(p);
          if (rc == NULL)
            return p_MonPower(p,i,r);
          /* else: binom ?*/
          int char_p=rChar(r);
          if ((pNext(rc) != NULL)
#ifdef HAVE_RINGS
             || rField_is_Ring(r)
#endif
             )
            return p_Pow(p,i,r);
          if ((char_p==0) || (i<=char_p))
            return p_TwoMonPower(p,i,r);
          poly p_p=p_TwoMonPower(p_Copy(p,r),char_p,r);
          return p_Mult_q(p_Power(p,i-char_p,r),p_p,r);
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
      f = pLmInit(a);
      t = nInit(pGetExp(a,k));
      pSetCoeff0(f,nMult(t,pGetCoeff(a)));
      nDelete(&t);
      if (nIsZero(pGetCoeff(f)))
        pLmDelete(&f);
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
      pLmDelete(&p);
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
  if (nIsZero(n))  p=pLmDeleteAndNext(p); // return NULL as p is a monomial
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


void pSplit(poly p, poly *h)
{
  *h=pNext(p);
  pNext(p)=NULL;
}



int pMaxCompProc(poly p)
{
  return pMaxComp(p);
}

/*2
* handle memory request for sets of polynomials (ideals)
* l is the length of *p, increment is the difference (may be negative)
*/
void pEnlargeSet(polyset *p, int l, int increment)
{
  int i;
  polyset h;

  h=(polyset)omReallocSize((poly*)*p,l*sizeof(poly),(l+increment)*sizeof(poly));
  if (increment>0)
  {
    //for (i=l; i<l+increment; i++)
    //  h[i]=NULL;
    memset(&(h[l]),0,increment*sizeof(poly));
  }
  *p=h;
}

number pInitContent(poly ph);
number pInitContent_a(poly ph);

void p_Content(poly ph, const ring r)
{
#ifdef HAVE_RINGS
  if (rField_is_Ring(r))
  {
    if ((ph!=NULL) && rField_has_Units(r))
    {
      number k = nGetUnit(pGetCoeff(ph));
      if (!nIsOne(k))
      {
        number tmpGMP = k;
        k = nInvers(k);
        nDelete(&tmpGMP);
        poly h = pNext(ph);
        pSetCoeff(ph, nMult(pGetCoeff(ph), k));
        while (h != NULL)
        {
          pSetCoeff(h, nMult(pGetCoeff(h), k));
          pIter(h);
        }
      }
      nDelete(&k);
    }
    return;
  }
#endif
  number h,d;
  poly p;

  if(TEST_OPT_CONTENTSB) return;
  if(pNext(ph)==NULL)
  {
    pSetCoeff(ph,nInit(1));
  }
  else
  {
    nNormalize(pGetCoeff(ph));
    if(!nGreaterZero(pGetCoeff(ph))) ph = pNeg(ph);
    if (rField_is_Q())
    {
      h=pInitContent(ph);
      p=ph;
    }
    else if ((rField_is_Extension(r))
    && ((rPar(r)>1)||(r->minpoly==NULL)))
    {
      h=pInitContent_a(ph);
      p=ph;
    }
    else
    {
      h=nCopy(pGetCoeff(ph));
      p = pNext(ph);
    }
    while (p!=NULL)
    {
      nNormalize(pGetCoeff(p));
      d=nGcd(h,pGetCoeff(p),r);
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
    if (rField_is_Q_a(r))
    {
      number hzz = nlInit(1, r);
      h = nlInit(1, r);
      p=ph;
      while (p!=NULL)
      { // each monom: coeff in Q_a
        lnumber c_n_n=(lnumber)pGetCoeff(p);
        napoly c_n=c_n_n->z;
        while (c_n!=NULL)
        { // each monom: coeff in Q
          d=nlLcm(hzz,pGetCoeff(c_n),r->algring);
          n_Delete(&hzz,r->algring);
          hzz=d;
          pIter(c_n);
        }
        c_n=c_n_n->n;
        while (c_n!=NULL)
        { // each monom: coeff in Q
          d=nlLcm(h,pGetCoeff(c_n),r->algring);
          n_Delete(&h,r->algring);
          h=d;
          pIter(c_n);
        }
        pIter(p);
      }
      /* hzz contains the 1/lcm of all denominators in c_n_n->z*/
      /* h contains the 1/lcm of all denominators in c_n_n->n*/
      number htmp=nlInvers(h);
      number hzztmp=nlInvers(hzz);
      number hh=nlMult(hzz,h);
      nlDelete(&hzz,r->algring);
      nlDelete(&h,r->algring);
      number hg=nlGcd(hzztmp,htmp,r->algring);
      nlDelete(&hzztmp,r->algring);
      nlDelete(&htmp,r->algring);
      h=nlMult(hh,hg);
      nlDelete(&hg,r->algring);
      nlDelete(&hh,r->algring);
      nlNormalize(h);
      if(!nlIsOne(h))
      {
        p=ph;
        while (p!=NULL)
        { // each monom: coeff in Q_a
          lnumber c_n_n=(lnumber)pGetCoeff(p);
          napoly c_n=c_n_n->z;
          while (c_n!=NULL)
          { // each monom: coeff in Q
            d=nlMult(h,pGetCoeff(c_n));
            nlNormalize(d);
            nlDelete(&pGetCoeff(c_n),r->algring);
            pGetCoeff(c_n)=d;
            pIter(c_n);
          }
          c_n=c_n_n->n;
          while (c_n!=NULL)
          { // each monom: coeff in Q
            d=nlMult(h,pGetCoeff(c_n));
            nlNormalize(d);
            nlDelete(&pGetCoeff(c_n),r->algring);
            pGetCoeff(c_n)=d;
            pIter(c_n);
          }
          pIter(p);
        }
      }
      nlDelete(&h,r->algring);
    }
  }
}
void pSimpleContent(poly ph,int smax)
{
  if(TEST_OPT_CONTENTSB) return;
  if (ph==NULL) return;
  if (pNext(ph)==NULL)
  {
    pSetCoeff(ph,nInit(1));
    return;
  }
  if ((pNext(pNext(ph))==NULL)||(!rField_is_Q()))
  {
    return;
  }
  number d=pInitContent(ph);
  if (nlSize(d)<=smax)
  {
    //if (TEST_OPT_PROT) PrintS("G");
    return;
  }
  poly p=ph;
  number h=d;
  if (smax==1) smax=2;
  while (p!=NULL)
  {
#if 0
    d=nlGcd(h,pGetCoeff(p),currRing);
    nlDelete(&h,currRing);
    h = d;
#else
    nlInpGcd(h,pGetCoeff(p),currRing);
#endif
    if(nlSize(h)<smax)
    {
      //if (TEST_OPT_PROT) PrintS("g");
      return;
    }
    pIter(p);
  }
  p = ph;
  if (!nlGreaterZero(pGetCoeff(p))) h=nlNeg(h);
  if(nlIsOne(h)) return;
  //if (TEST_OPT_PROT) PrintS("c");
  while (p!=NULL)
  {
#if 1
    d = nlIntDiv(pGetCoeff(p),h);
    pSetCoeff(p,d);
#else
    nlInpIntDiv(pGetCoeff(p),h,currRing);
#endif
    pIter(p);
  }
  nlDelete(&h,currRing);
}

number pInitContent(poly ph)
// only for coefficients in Q
#if 0
{
  assume(!TEST_OPT_CONTENTSB);
  assume(ph!=NULL);
  assume(pNext(ph)!=NULL);
  assume(rField_is_Q());
  if (pNext(pNext(ph))==NULL)
  {
    return nlGetNom(pGetCoeff(pNext(ph)),currRing);
  }
  poly p=ph;
  number n1=nlGetNom(pGetCoeff(p),currRing);
  pIter(p);
  number n2=nlGetNom(pGetCoeff(p),currRing);
  pIter(p);
  number d;
  number t;
  loop
  {
    nlNormalize(pGetCoeff(p));
    t=nlGetNom(pGetCoeff(p),currRing);
    if (nlGreaterZero(t))
      d=nlAdd(n1,t);
    else
      d=nlSub(n1,t);
    nlDelete(&t,currRing);
    nlDelete(&n1,currRing);
    n1=d;
    pIter(p);
    if (p==NULL) break;
    nlNormalize(pGetCoeff(p));
    t=nlGetNom(pGetCoeff(p),currRing);
    if (nlGreaterZero(t))
      d=nlAdd(n2,t);
    else
      d=nlSub(n2,t);
    nlDelete(&t,currRing);
    nlDelete(&n2,currRing);
    n2=d;
    pIter(p);
    if (p==NULL) break;
  }
  d=nlGcd(n1,n2,currRing);
  nlDelete(&n1,currRing);
  nlDelete(&n2,currRing);
  return d;
}
#else
{
  number d=pGetCoeff(ph);
  if(SR_HDL(d)&SR_INT) return d;
  int s=mpz_size1(d->z);
  int s2=-1;
  number d2;
  loop
  {
    pIter(ph);
    if(ph==NULL)
    {
      if (s2==-1) return nlCopy(d);
      break;
    }
    if (SR_HDL(pGetCoeff(ph))&SR_INT)
    {
      s2=s;
      d2=d;
      s=0;
      d=pGetCoeff(ph);
      if (s2==0) break;
    }
    else
    if (mpz_size1((pGetCoeff(ph)->z))<=s)
    {
      s2=s;
      d2=d;
      d=pGetCoeff(ph);
      s=mpz_size1(d->z);
    }
  }
  return nlGcd(d,d2,currRing);
}
#endif

number pInitContent_a(poly ph)
// only for coefficients in K(a) anf K(a,...)
{
  number d=pGetCoeff(ph);
  int s=naParDeg(d);
  if (s /* naParDeg(d)*/ <=1) return naCopy(d);
  int s2=-1;
  number d2;
  int ss;
  loop
  {
    pIter(ph);
    if(ph==NULL)
    {
      if (s2==-1) return naCopy(d);
      break;
    }
    if ((ss=naParDeg(pGetCoeff(ph)))<s)
    {
      s2=s;
      d2=d;
      s=ss;
      d=pGetCoeff(ph);
      if (s2<=1) break;
    }
  }
  return naGcd(d,d2,currRing);
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
#if 0
void p_Content(poly ph, ring r)
{
  number h,d;
  poly p;

  if(pNext(ph)==NULL)
  {
    pSetCoeff(ph,n_Init(1,r));
  }
  else
  {
    n_Normalize(pGetCoeff(ph),r);
    if(!n_GreaterZero(pGetCoeff(ph),r)) ph = p_Neg(ph,r);
    h=n_Copy(pGetCoeff(ph),r);
    p = pNext(ph);
    while (p!=NULL)
    {
      n_Normalize(pGetCoeff(p),r);
      d=n_Gcd(h,pGetCoeff(p),r);
      n_Delete(&h,r);
      h = d;
      if(n_IsOne(h,r))
      {
        break;
      }
      pIter(p);
    }
    p = ph;
    //number tmp;
    if(!n_IsOne(h,r))
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
        d = n_IntDiv(pGetCoeff(p),h,r);
        p_SetCoeff(p,d,r);
        pIter(p);
      }
    }
    n_Delete(&h,r);
#ifdef HAVE_FACTORY
    //if ( (n_GetChar(r) == 1) || (n_GetChar(r) < 0) ) /* Q[a],Q(a),Zp[a],Z/p(a) */
    //{
    //  singclap_divide_content(ph);
    //  if(!n_GreaterZero(pGetCoeff(ph),r)) ph = p_Neg(ph,r);
    //}
#endif
  }
}
#endif

poly p_Cleardenom(poly ph, const ring r)
{
  poly start=ph;
  number d, h;
  poly p;

#ifdef HAVE_RINGS
  if (rField_is_Ring(r))
  {
    p_Content(ph,r);
    return start;
  }
#endif
  if (rField_is_Zp(r) && TEST_OPT_INTSTRATEGY) return start;
  p = ph;
  if(pNext(p)==NULL)
  {
    if (TEST_OPT_CONTENTSB)
    {
      number n=nGetDenom(pGetCoeff(p));
      if (!nIsOne(n))
      {
        number nn=nMult(pGetCoeff(p),n);
        nNormalize(nn);
        pSetCoeff(p,nn);
      }
      nDelete(&n);
    }
    else
      pSetCoeff(p,nInit(1));
  }
  else
  {
    h = nInit(1);
    while (p!=NULL)
    {
      nNormalize(pGetCoeff(p));
      d=nLcm(h,pGetCoeff(p),currRing);
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
        loop
        {
          h = nInit(1);
          p=ph;
          while (p!=NULL)
          {
            d=nLcm(h,pGetCoeff(p),currRing);
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
          else
          {
            nDelete(&h);
            break;
          }
        }
      }
    }
    if (h!=NULL) nDelete(&h);
  
    p_Content(ph,r);
#ifdef HAVE_RATGRING
    if (rIsRatGRing(r))
    {
      /* quick unit detection in the rational case is done in gr_nc_bba */
      pContentRat(ph);
      start=ph;
    }
#endif
  }
  return start;
}

void p_Cleardenom_n(poly ph,const ring r,number &c)
{
  number d, h;
  poly p;

  p = ph;
  if(pNext(p)==NULL)
  {
    c=nInvers(pGetCoeff(p));
    pSetCoeff(p,nInit(1));
  }
  else
  {
    h = nInit(1);
    while (p!=NULL)
    {
      nNormalize(pGetCoeff(p));
      d=nLcm(h,pGetCoeff(p),r);
      nDelete(&h);
      h=d;
      pIter(p);
    }
    c=h;
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
      if (nGetChar()==1)
      {
        loop
        {
          h = nInit(1);
          p=ph;
          while (p!=NULL)
          {
            d=nLcm(h,pGetCoeff(p),r);
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
            number t=nMult(c,h);
            nDelete(&c);
            c=t;
          }
          else
          {
            break;
          }
          nDelete(&h);
        }
      }
    }
  }
}

number p_GetAllDenom(poly ph, const ring r)
{
  number d=n_Init(1,r);
  poly p = ph;

  while (p!=NULL)
  {
    number h=n_GetDenom(pGetCoeff(p),r);
    if (!n_IsOne(h,r))
    {
      number dd=n_Mult(d,h,r);
      n_Delete(&d,r);
      d=dd;
    }
    n_Delete(&h,r);
    pIter(p);
  }
  return d;
}

/*2
*tests if p is homogeneous with respect to the actual weigths
*/
BOOLEAN pIsHomogeneous (poly p)
{
  poly qp=p;
  int o;

  if ((p == NULL) || (pNext(p) == NULL)) return TRUE;
  pFDegProc d;
  if (pLexOrder && (currRing->order[0]==ringorder_lp))
    d=p_Totaldegree;
  else 
    d=pFDeg;
  o = d(p,currRing);
  do
  {
    if (d(qp,currRing) != o) return FALSE;
    pIter(qp);
  }
  while (qp != NULL);
  return TRUE;
}

// orders monoms of poly using merge sort (ususally faster than
// insertion sort). ASSUMES that pSetm was performed on monoms
poly pOrdPolyMerge(poly p)
{
  poly qq,pp,result=NULL;

  if (p == NULL) return NULL;

  loop
  {
    qq = p;
    loop
    {
      if (pNext(p) == NULL)
      {
        result=pAdd(result, qq);
        pTest(result);
        return result;
      }
      if (pLmCmp(p,pNext(p)) != 1)
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

// orders monoms of poly using insertion sort, performs pSetm on each monom
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
poly pPermPoly (poly p, int * perm, const ring oldRing, nMapFunc nMap,
   int *par_perm, int OldPar)
{
  int OldpVariables = oldRing->N;
  poly result = NULL;
  poly result_last = NULL;
  poly aq=NULL; /* the map coefficient */
  poly qq; /* the mapped monomial */

  while (p != NULL)
  {
    if ((OldPar==0)||(rField_is_GF(oldRing)))
    {
      qq = pInit();
      number n=nMap(pGetCoeff(p));
      if ((currRing->minpoly!=NULL)
      && ((rField_is_Zp_a()) || (rField_is_Q_a())))
      {
        nNormalize(n);
      }
      pGetCoeff(qq)=n;
    // coef may be zero:  pTest(qq);
    }
    else
    {
      qq=pOne();
      aq=naPermNumber(pGetCoeff(p),par_perm,OldPar, oldRing);
      if ((currRing->minpoly!=NULL)
      && ((rField_is_Zp_a()) || (rField_is_Q_a())))
      {
        poly tmp=aq;
        while (tmp!=NULL)
        {
          number n=pGetCoeff(tmp);
          nNormalize(n);
          pGetCoeff(tmp)=n;
          pIter(tmp);
        }
      }
      pTest(aq);
    }
    if (rRing_has_Comp(currRing)) pSetComp(qq, p_GetComp(p,oldRing));
    if (nIsZero(pGetCoeff(qq)))
    {
      pLmDelete(&qq);
    }
    else
    {
      int i;
      int mapped_to_par=0;
      for(i=1; i<=OldpVariables; i++)
      {
        int e=p_GetExp(p,i,oldRing);
        if (e!=0)
        {
          if (perm==NULL)
          {
            pSetExp(qq,i, e);
          }
          else if (perm[i]>0)
            pAddExp(qq,perm[i], e/*p_GetExp( p,i,oldRing)*/);
          else if (perm[i]<0)
          {
            if (rField_is_GF())
            {
              number c=pGetCoeff(qq);
              number ee=nfPar(1);
              number eee;nfPower(ee,e,&eee); //nfDelete(ee,currRing);
              ee=nfMult(c,eee);
              //nfDelete(c,currRing);nfDelete(eee,currRing);
              pSetCoeff0(qq,ee);
            }
            else
            {
              lnumber c=(lnumber)pGetCoeff(qq);
              if (c->z->next==NULL)
                napAddExp(c->z,-perm[i],e/*p_GetExp( p,i,oldRing)*/);
              else /* more difficult: we have really to multiply: */
              {
                lnumber mmc=(lnumber)naInit(1,currRing);
                napSetExp(mmc->z,-perm[i],e/*p_GetExp( p,i,oldRing)*/);
                napSetm(mmc->z);
                pGetCoeff(qq)=naMult((number)c,(number)mmc);
                nDelete((number *)&c);
                nDelete((number *)&mmc); 
              }
              mapped_to_par=1;
            }
          }
          else
          {
            /* this variable maps to 0 !*/
            pLmDelete(&qq);
            break;
          }
        }
      }
      if (mapped_to_par
      && (currRing->minpoly!=NULL))
      {
        number n=pGetCoeff(qq);
        nNormalize(n);
        pGetCoeff(qq)=n;
      }
    }
    pIter(p);
#if 1
    if (qq!=NULL)
    {
      pSetm(qq);
      pTest(aq);
      pTest(qq);
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
  result=pSortAdd(result);
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

#if 0
/*2
*returns a re-ordered copy of a polynomial, with permutation of the variables
*/
poly p_PermPoly (poly p, int * perm, ring oldRing,
   int *par_perm, int OldPar, ring newRing)
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
      number n=newRing->cf->nMap(pGetCoeff(p));
      if ((newRing->minpoly!=NULL)
      && ((rField_is_Zp_a(newRing)) || (rField_is_Q_a(newRing))))
      {
        newRing->cf->nNormalize(n);
      }
      pGetCoeff(qq)=n;
    // coef may be zero:  pTest(qq);
    }
    else
    {
      qq=p_ISet(1, newRing);
      aq=naPermNumber(pGetCoeff(p),par_perm,OldPar, oldRing);
      if ((newRing->minpoly!=NULL)
      && ((rField_is_Zp_a(newRing)) || (rField_is_Q_a(newRing))))
      {
        poly tmp=aq;
        while (tmp!=NULL)
        {
          number n=pGetCoeff(tmp);
          newRing->cf->nNormalize(n);
          pGetCoeff(tmp)=n;
          pIter(tmp);
        }
      }
      //pTest(aq);
    }
    p_SetComp(qq, p_GetComp(p,oldRing), newRing);
    if (newRing->cf->nIsZero(pGetCoeff(qq)))
    {
      p_DeleteLm(&qq, newRing);
    }
    else
    {
      int i;
      int mapped_to_par=0;
      for(i=1; i<=OldpVariables; i++)
      {
        int e=p_GetExp(p,i,oldRing);
        if (e!=0)
        {
          if (perm==NULL)
          {
            p_SetExp(qq,i, e, newRing);
          }
          else if (perm[i]>0)
            p_AddExp(qq,perm[i], e/*p_GetExp( p,i,oldRing)*/, newRing);
          else if (perm[i]<0)
          {
            lnumber c=(lnumber)pGetCoeff(qq);
            napAddExp(c->z,-perm[i],e/*p_GetExp( p,i,oldRing)*/);
            mapped_to_par=1;
          }
          else
          {
            /* this variable maps to 0 !*/
            p_DeleteLm(&qq, newRing);
            break;
          }
        }
      }
      if (mapped_to_par
      && (newRing->minpoly!=NULL))
      {
        number n=pGetCoeff(qq);
        newRing->cf->nNormalize(n);
        pGetCoeff(qq)=n;
      }
    }
    pIter(p);
    if (qq!=NULL)
    {
      p_Setm(qq, newRing);
      //pTest(aq);
      //pTest(qq);
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
      p_Delete(&aq, newRing);
    }
  }
  result=pOrdPolyMerge(result);
  //pTest(result);
  return result;
}
#endif

poly ppJet(poly p, int m)
{
  poly r=NULL;
  poly t=NULL;

  while (p!=NULL)
  {
    if (p_Totaldegree(p,currRing)<=m)
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

poly pJet(poly p, int m)
{
  poly t=NULL;

  while((p!=NULL) && (p_Totaldegree(p,currRing)>m)) pLmDelete(&p);
  if (p==NULL) return NULL;
  poly r=p;
  while (pNext(p)!=NULL)
  {
    if (p_Totaldegree(pNext(p),currRing)>m)
    {
      pLmDelete(&pNext(p));
    }
    else
      pIter(p);
  }
  return r;
}

poly ppJetW(poly p, int m, short *w)
{
  poly r=NULL;
  poly t=NULL;
  while (p!=NULL)
  {
    if (totaldegreeWecart_IV(p,currRing,w)<=m)
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
  poly t=NULL;
  while((p!=NULL) && (totaldegreeWecart_IV(p,currRing,w)>m)) pLmDelete(&p);
  if (p==NULL) return NULL;
  poly r=p;
  while (pNext(p)!=NULL)
  {
    if (totaldegreeWecart_IV(pNext(p),currRing,w)>m)
    {
      pLmDelete(&pNext(p));
    }
    else
      pIter(p);
  }
  return r;
}

int pMinDeg(poly p,intvec *w)
{
  if(p==NULL)
    return -1;
  int d=-1;
  while(p!=NULL)
  {
    int d0=0;
    for(int j=0;j<pVariables;j++)
      if(w==NULL||j>=w->length())
        d0+=pGetExp(p,j+1);
      else
        d0+=(*w)[j]*pGetExp(p,j+1);
    if(d0<d||d==-1)
      d=d0;
    pIter(p);
  }
  return d;
}

poly pSeries(int n,poly p,poly u, intvec *w)
{
  short *ww=iv2array(w);
  if(p!=NULL)
  {
    if(u==NULL)
      p=pJetW(p,n,ww);
    else
      p=pJetW(pMult(p,pInvers(n-pMinDeg(p,w),u,w)),n,ww);
  }
  omFreeSize((ADDRESS)ww,(pVariables+1)*sizeof(short));
  return p;
}

poly pInvers(int n,poly u,intvec *w)
{
  short *ww=iv2array(w);
  if(n<0)
    return NULL;
  number u0=nInvers(pGetCoeff(u));
  poly v=pNSet(u0);
  if(n==0)
    return v;
  poly u1=pJetW(pSub(pOne(),pMult_nn(u,u0)),n,ww);
  if(u1==NULL)
    return v;
  poly v1=pMult_nn(pCopy(u1),u0);
  v=pAdd(v,pCopy(v1));
  for(int i=n/pMinDeg(u1,w);i>1;i--)
  {
    v1=pJetW(pMult(v1,pCopy(u1)),n,ww);
    v=pAdd(v,pCopy(v1));
  }
  pDelete(&u1);
  pDelete(&v1);
  omFreeSize((ADDRESS)ww,(pVariables+1)*sizeof(short));
  return v;
}

long pDegW(poly p, const short *w)
{
  long r=-LONG_MAX;

  while (p!=NULL)
  {
    long t=totaldegreeWecart_IV(p,currRing,w);
    if (t>r) r=t;
    pIter(p);
  }
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
  *p=(polyset)omAlloc0((*len)*sizeof(poly));
  while (v!=NULL)
  {
    h=pHead(v);
    k=pGetComp(h);
    pSetComp(h,0);
    (*p)[k-1]=pAdd((*p)[k-1],h);
    pIter(v);
  }
}

int p_Var(poly m,const ring r)
{
  if (m==NULL) return 0;
  if (pNext(m)!=NULL) return 0;
  int i,e=0;
  for (i=r->N; i>0; i--)
  {
    int exp=p_GetExp(m,i,r);
    if (exp==1)
    {
      if (e==0) e=i;
      else return 0;
    }
    else if (exp!=0)
    {
      return 0;
    }
  }
  return e;
}

/*----------utilities for syzygies--------------*/
//BOOLEAN   pVectorHasUnitM(poly p, int * k)
//{
//  while (p!=NULL)
//  {
//    if (pLmIsConstantComp(p))
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
    if (pLmIsConstantComp(q))
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
    if (pLmIsConstantComp(q))
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
BOOLEAN p_EqualPolys(poly p1,poly p2, const ring r)
{
  while ((p1 != NULL) && (p2 != NULL))
  {
    if (! p_LmEqual(p1, p2,r))
      return FALSE;
    if (! n_Equal(p_GetCoeff(p1,r), p_GetCoeff(p2,r),r ))
      return FALSE;
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
  pAssume(p1 != NULL && p2 != NULL);

  if (!pLmEqual(p1,p2)) //compare leading mons
      return FALSE;
  if ((pNext(p1)==NULL) && (pNext(p2)!=NULL))
     return FALSE;
  if ((pNext(p2)==NULL) && (pNext(p1)!=NULL))
     return FALSE;
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

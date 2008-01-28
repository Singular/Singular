/****************************************
*  Computer Algebra System SINGULAR     *
****************************************/
/* $Id: polys0.cc,v 1.3 2008-01-28 13:44:59 Singular Exp $ */

/*
* ABSTRACT - all basic methods to convert polynomials to strings
*/

/* includes */
#include "mod2.h"
#include "structs.h"
#include "numbers.h"
#include "ring.h"
#include "p_polys.h"
#include "febase.h"

/*2
* writes a monomial (p),
* uses form x*gen(.) if ko != coloumn number of p
*/
static void writemon(poly p, int ko, ring r)
{
  BOOLEAN wroteCoef=FALSE,writeGen=FALSE;

  if (pGetCoeff(p)!=NULL)
    n_Normalize(pGetCoeff(p),r);

  if (((p_GetComp(p,r) == (short)ko)
    &&(p_LmIsConstantComp(p, r)))
  || ((!n_IsOne(pGetCoeff(p),r))
    && (!n_IsMOne(pGetCoeff(p),r))
  )
  )
  {
    n_Write(p->coef,r);
    wroteCoef=(rShortOut(r) == FALSE ||(r->parameter!=NULL));
    writeGen=TRUE;
  }
  else if (n_IsMOne(pGetCoeff(p),r))
  {
    if (n_GreaterZero(pGetCoeff(p),r))
    {
      n_Write(p->coef,r);
      wroteCoef=(rShortOut(r) == FALSE ||(r->parameter!=NULL));
      writeGen=TRUE;
    }
    else
      StringAppendS("-");
  }

  int i;
  for (i=0; i<r->N; i++)
  {
    {
      Exponent_t ee = p_GetExp(p,i+1,r);
      if (ee!=0)
      {
        if (wroteCoef)
          StringAppendS("*");
        //else
          wroteCoef=(rShortOut(r) == FALSE);
        writeGen=TRUE;
        StringAppendS(rRingVar(i, r));
        if (ee != 1)
        {
          if (rShortOut(r)==0) StringAppendS("^");
          StringAppend("%d", ee);
        }
      }
    }
  }
  //StringAppend("{%d}",p->Order);
  if (p_GetComp(p, r) != (Exponent_t)ko)
  {
    if (writeGen) StringAppendS("*");
    StringAppend("gen(%d)", p_GetComp(p, r));
  }
}

char* p_String0(poly p, ring lmRing, ring tailRing)
{
  if (p == NULL)
  {
    return StringAppendS("0");
  }
  if ((p_GetComp(p, lmRing) == 0) || (!lmRing->VectorOut))
  {
    writemon(p,0, lmRing);
    p = pNext(p);
    while (p!=NULL)
    {
      if ((p->coef==NULL)||n_GreaterZero(p->coef,tailRing))
        StringAppendS("+");
      writemon(p,0, tailRing);
      p = pNext(p);
    }
    return StringAppendS("");
  }

  Exponent_t k = 1;
  StringAppendS("[");
  loop
  {
    while (k < p_GetComp(p,lmRing))
    {
      StringAppendS("0,");
      k++;
    }
    writemon(p,k,lmRing);
    pIter(p);
    while ((p!=NULL) && (k == p_GetComp(p, tailRing)))
    {
      if (n_GreaterZero(p->coef,tailRing)) StringAppendS("+");
      writemon(p,k,tailRing);
      pIter(p);
    }
    if (p == NULL) break;
    StringAppendS(",");
    k++;
  }
  return StringAppendS("]");
}

char* p_String(poly p, ring lmRing, ring tailRing)
{
  StringSetS("");
  return p_String0(p, lmRing, tailRing);
}

/*2
* writes a polynomial p to stdout
*/
void p_Write0(poly p, ring lmRing, ring tailRing)
{
  PrintS(p_String(p, lmRing, tailRing));
}

/*2
* writes a polynomial p to stdout followed by \n
*/
void p_Write(poly p, ring lmRing, ring tailRing)
{
  p_Write0(p, lmRing, tailRing);
  PrintLn();
}

/*2
*the standard debugging output:
*print the first two monomials of the poly (wrp) or only the lead term (wrp0),
*possibly followed by the string "+..."
*/
void p_wrp0(poly p, ring ri)
{
  poly r;

  if (p==NULL) PrintS("NULL");
  else if (pNext(p)==NULL) p_Write0(p, ri);
  else
  {
    r = pNext(p);
    pNext(p) = NULL;
    p_Write0(p, ri);
    if (r!=NULL)
    {
      PrintS("+...");
      pNext(p) = r;
    }
  }
}
//#if !defined(__OPTIMIZE__) || defined(KDEBUG): used in list1
#if 1
void p_wrp(poly p, ring lmRing, ring tailRing)
{
  poly r;

  if (p==NULL) PrintS("NULL");
  else if (pNext(p)==NULL) p_Write0(p, lmRing);
  else
  {
    r = pNext(pNext(p));
    pNext(pNext(p)) = NULL;
    p_Write0(p, lmRing, tailRing);
    if (r!=NULL)
    {
      PrintS("+...");
      pNext(pNext(p)) = r;
    }
  }
}
#else
// this is for use with buckets
void p_wrp(poly p, ring lmRing, ring tailRing)
{
  poly r;

  if (p==NULL) PrintS("NULL");
  else if (pNext(p)==NULL) p_Write0(p, lmRing);
  else
  {
    r = pNext(p);
    pNext(p) = NULL;
    p_Write0(p, lmRing, tailRing);
    pNext(p) = r;
  }
}
#endif
//#endif

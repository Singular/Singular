/****************************************
*  Computer Algebra System SINGULAR     *
****************************************/
/* $Id: polys0.cc,v 1.15 2000-10-19 15:00:20 obachman Exp $ */

/*
* ABSTRACT - all basic methods to convert polynomials to strings
*/

/* includes */
#include "mod2.h"
#include "tok.h"
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
    nNormalize(pGetCoeff(p));

  if (((p_GetComp(p,r) == (short)ko)
    &&(p_LmIsConstantComp(p, r)))
  || ((!nIsOne(pGetCoeff(p)))
    && (!nIsMOne(pGetCoeff(p)))
  )
  )
  {
    nWrite(p->coef);
    wroteCoef=(rShortOut(r) == FALSE ||(r->parameter!=NULL));
    writeGen=TRUE;
  }
  else if (nIsMOne(pGetCoeff(p)))
  {
    if (nGreaterZero(pGetCoeff(p)))
    {
      nWrite(p->coef);
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

char* pString0(poly p, ring r)
{
  if (p == NULL)
  {
    return StringAppendS("0");
  }
  if ((p_GetComp(p, r) == 0) || (!r->VectorOut))
  {
    writemon(p,0, r);
    p = pNext(p);
    while (p!=NULL)
    {
      if ((p->coef==NULL)||nGreaterZero(p->coef))
        StringAppendS("+");
      writemon(p,0, r);
      p = pNext(p);
    }
    return StringAppendS("");
  }

  Exponent_t k = 1;
  StringAppendS("[");
  loop
  {
    while (k < p_GetComp(p,r))
    {
      StringAppendS("0,");
      k++;
    }
    writemon(p,k,r);
    pIter(p);
    while ((p!=NULL) && (k == p_GetComp(p, r)))
    {
      if (nGreaterZero(p->coef)) StringAppendS("+");
      writemon(p,k,r);
      pIter(p);
    }
    if (p == NULL) break;
    StringAppendS(",");
    k++;
  }
  return StringAppendS("]");
}

char* pString(poly p, ring r)
{
  StringSetS("");
  return pString0(p, r);
}

/*2
* writes a polynomial p to stdout
*/
void pWrite0(poly p, ring r)
{
  PrintS(pString(p, r));
}

/*2
* writes a polynomial p to stdout followed by \n
*/
void pWrite(poly p, ring r)
{
  pWrite0(p, r);
  PrintLn();
}

/*2
*the standard debugging output:
*print the first two monomials of the poly (wrp) or only the lead ter (wrp0),
*possibly followed by the string "+..."
*/
void wrp0(poly p, ring ri)
{
  poly r;

  if (p==NULL) PrintS("NULL");
  else if (pNext(p)==NULL) pWrite0(p, ri);
  else
  {
    r = pNext(p);
    pNext(p) = NULL;
    pWrite0(p, ri);
    if (r!=NULL)
    {
      PrintS("+...");
      pNext(p) = r;
    }
  }
}
void wrp(poly p, ring ri)
{
  poly r;

  if (p==NULL) PrintS("NULL");
  else if (pNext(p)==NULL) pWrite0(p, ri);
  else
  {
    r = pNext(pNext(p));
    pNext(pNext(p)) = NULL;
    pWrite0(p, ri);
    if (r!=NULL)
    {
      PrintS("+...");
      pNext(pNext(p)) = r;
    }
  }
}

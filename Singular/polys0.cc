/****************************************
*  Computer Algebra System SINGULAR     *
****************************************/
/* $Id: polys0.cc,v 1.12 1999-09-29 10:59:37 obachman Exp $ */

/*
* ABSTRACT - all basic methods to convert polynomials to strings
*/

/* includes */
#include "mod2.h"
#include "tok.h"
#include "structs.h"
#include "febase.h"
#include "numbers.h"
#include "ring.h"
//#include "ipid.h"
#include "polys.h"

BOOLEAN pVectorOut=TRUE;
/*2
* writes a monomial (p),
* uses form x*gen(.) if ko != coloumn number of p
*/
static void writemon(poly p, int ko)
{
  BOOLEAN wroteCoef=FALSE,writeGen=FALSE;

  if (pGetCoeff(p)!=NULL)
    nNormalize(pGetCoeff(p));

  if (((pGetComp(p) == (short)ko)
    &&(pIsConstantComp(p)))
  || ((!nIsOne(pGetCoeff(p)))
    && (!nIsMOne(pGetCoeff(p)))
  )
  )
  {
    nWrite(p->coef);
    wroteCoef=((pShortOut==0)||(currRing->parameter!=NULL));
    writeGen=TRUE;
  }
  else if (nIsMOne(pGetCoeff(p)))
  {
    if (nGreaterZero(pGetCoeff(p)))
    {
      nWrite(p->coef);
      wroteCoef=((pShortOut==0)||(currRing->parameter!=NULL));
      writeGen=TRUE;
    }
    else
      StringAppendS("-");
  }

  int i;
  for (i=0; i<pVariables; i++)
  {
    {
      Exponent_t ee = pGetExp(p,i+1);
      if (ee!=0)
      {
        if (wroteCoef)
          StringAppendS("*");
        //else
          wroteCoef=(pShortOut==0);
        writeGen=TRUE;
        StringAppendS(RingVar(i));
        if (ee != 1)
        {
          if (pShortOut==0) StringAppendS("^");
          StringAppend("%d", ee);
        }
      }
    }
  }
  //StringAppend("{%d}",p->Order);
  if (pGetComp(p) != (Exponent_t)ko)
  {
    if (writeGen) StringAppendS("*");
    StringAppend("gen(%d)", pGetComp(p));
  }
}

char* pString0(poly p)
{
  if (p == NULL)
  {
    return StringAppendS("0");
  }
  if ((pGetComp(p) == 0) || (!pVectorOut))
  {
    writemon(p,0);
    p = pNext(p);
    while (p!=NULL)
    {
      if ((p->coef==NULL)||nGreaterZero(p->coef))
        StringAppendS("+");
      writemon(p,0);
      p = pNext(p);
    }
    return StringAppendS("");
  }

  Exponent_t k = 1;
  StringAppendS("[");
  loop
  {
    while (k < pGetComp(p))
    {
      StringAppendS("0,");
      k++;
    }
    writemon(p,k);
    pIter(p);
    while ((p!=NULL) && (k == pGetComp(p)))
    {
      if (nGreaterZero(p->coef)) StringAppendS("+");
      writemon(p,k);
      pIter(p);
    }
    if (p == NULL) break;
    StringAppendS(",");
    k++;
  }
  return StringAppendS("]");
}

char* pString(poly p)
{
  StringSetS("");
  return pString0(p);
}

/*2
* writes a polynomial p to stdout
*/
void pWrite0(poly p)
{
  PrintS(pString(p));
}

/*2
* writes a polynomial p to stdout followed by \n
*/
void pWrite(poly p)
{
  pWrite0(p);
  PrintLn();
}

/*2
*the standard debugging output:
*print the first two monomials of the poly,
*possibly followed by the string "+..."
*/
void wrp(poly p)
{
  poly r;

  if (p==NULL) PrintS("NULL");
  else if (pNext(p)==NULL) pWrite0(p);
  else
  {
    r = pNext(pNext(p));
    pNext(pNext(p)) = NULL;
    pWrite0(p);
    if (r!=NULL)
    {
      PrintS("+...");
      pNext(pNext(p)) = r;
    }
  }
}

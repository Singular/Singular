/****************************************
*  Computer Algebra System SINGULAR     *
****************************************/
static char rcsid[] = "$Id: polys0.cc,v 1.2 1997-03-24 14:25:38 Singular Exp $";
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
#include "ipid.h"
#include "polys.h"

int pVectorOut=TRUE;
/*2
* writes a monomial (p),
* uses form x*gen(.) if ko != coloumn number of p
*/
static void writemon(poly p, int ko)
{
  int i;
  BOOLEAN wroteCoef=FALSE,writeGen=FALSE;

  if ((pIsConstantComp(p))
  || (!nIsOne(pGetCoeff(p)))
#ifdef DRING
  || (pDRING && pdIsConstantComp(p))
#endif
  )
  {
//    if (nIsMOne(pGetCoeff(p))  StringAppendS("-");
//    else
    {
      nWrite(p->coef);
      wroteCoef=((pShortOut==0)||(currRing->parameter!=NULL));
      writeGen=TRUE;
    }
  }
  for (i=0; i<pVariables; i++)
  {
#ifdef DRING
    if ((!pDRING)||(i!=2*pdN))
#endif
    {
      short ee = p->exp[i+1];
      if (ee!=0)
      {
        if (wroteCoef)
          StringAppendS("*");
        //else
          wroteCoef=(pShortOut==0);
        writeGen=TRUE;
#ifdef DRING
        if((pDRING)&&(pdN<=i)&&(i<2*pdN)&&(pdDFlag(p)==0))
        {
          StringAppendS(RingVar(i-pdN));
          ee=-ee;
        }
        else
          StringAppendS(RingVar(i));
#else
        StringAppendS(RingVar(i));
#endif
        if (ee != 1)
        {
          if (pShortOut==0) StringAppendS("^");
          StringAppend("%d", ee);
        }
      }
    }
  }
  if (p->exp[0] != (short)ko)
  {
    if (writeGen) StringAppendS("*");
    StringAppend("gen(%d)", p->exp[0]);
  }
}

char* pString0(poly p)
{
  if (p == NULL)
  {
    return StringAppendS("0");
  }
  if ((p->exp[0] == 0) || (!pVectorOut))
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

  short k = 1;
  StringAppendS("[");
  loop
  {
    while (k < p->exp[0])
    {
      StringAppendS("0,");
      k++;
    }
    writemon(p,k);
    p = pNext(p);
    while ((p!=NULL) && (k == p->exp[0]))
    {
      if (nGreaterZero(p->coef)) StringAppendS("+");
      writemon(p,k);
      p = pNext(p);
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
  Print(pString(p));
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

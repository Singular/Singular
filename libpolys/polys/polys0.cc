/****************************************
*  Computer Algebra System SINGULAR     *
****************************************/

/*
* ABSTRACT - all basic methods to convert polynomials to strings
*/

/* includes */

#include "misc/auxiliary.h"

#include "coeffs/numbers.h"
#include "polys/monomials/ring.h"
#include "polys/monomials/p_polys.h"
#ifdef HAVE_SHIFTBBA
#include "polys/shiftop.h"
#endif

/*2
* writes a monomial (p),
* uses form x*gen(.) if ko != coloumn number of p
*/
static void writemon(poly p, int ko, const ring r)
{
  assume(r != NULL);
  const coeffs C = r->cf;
  assume(C != NULL);

  BOOLEAN wroteCoef=FALSE,writeGen=FALSE;
  const BOOLEAN bNotShortOut = (rShortOut(r) == FALSE);

  if (((p_GetComp(p,r) == ko)
    &&(p_LmIsConstantComp(p, r)))
  || ((!n_IsOne(pGetCoeff(p),C))
    && (!n_IsMOne(pGetCoeff(p),C))
  )
  )
  {
    if( bNotShortOut )
      n_WriteLong(pGetCoeff(p),C);
    else
      n_WriteShort(pGetCoeff(p),C);

    wroteCoef=(bNotShortOut)
    || (rParameter(r)!=NULL)
    || rField_is_R(r) || (rField_is_long_R(r)) || (rField_is_long_C(r));
    writeGen=TRUE;
  }
  else if (n_IsMOne(pGetCoeff(p),C))
  {
    if (n_GreaterZero(pGetCoeff(p),C))
    {
      if( bNotShortOut )
        n_WriteLong(pGetCoeff(p),C);
      else
        n_WriteShort(pGetCoeff(p),C);

      wroteCoef=(bNotShortOut)
      || (rParameter(r)!=NULL)
      || rField_is_R(r) || (rField_is_long_R(r)) || (rField_is_long_C(r));
      writeGen=TRUE;
    }
    else
      StringAppendS("-");
  }

  int i;
  {
    for (i=0; i<rVar(r); i++)
    {
      {
        long ee = p_GetExp(p,i+1,r);
        if (ee!=0L)
        {
          if (wroteCoef)
            StringAppendS("*");
          //else
          wroteCoef=(bNotShortOut);
          writeGen=TRUE;
          StringAppendS(rRingVar(i, r));
          if (ee != 1L)
          {
            if (bNotShortOut) StringAppendS("^");
            StringAppend("%ld", ee);
          }
        }
      }
    }
  }
  //StringAppend("{%d}",p->Order);
  if (p_GetComp(p, r) != (long)ko)
  {
    if (writeGen) StringAppendS("*");
    StringAppend("gen(%d)", p_GetComp(p, r));
  }
}

/*2
* writes a monomial (p),
* uses form x*gen(.) if ko != coloumn number of p
*/
#ifdef HAVE_SHIFTBBA
static void writemonLP(poly p, int ko, const ring r)
{
  assume(r != NULL);
  const coeffs C = r->cf;
  assume(C != NULL);

  BOOLEAN wroteCoef=FALSE,writeGen=FALSE;

  if (((p_GetComp(p,r) == ko)
    &&(p_LmIsConstantComp(p, r)))
  || ((!n_IsOne(pGetCoeff(p),C))
    && (!n_IsMOne(pGetCoeff(p),C))
  )
  )
  {
    n_WriteLong(pGetCoeff(p),C);

    wroteCoef=TRUE;
    writeGen=TRUE;
  }
  else if (n_IsMOne(pGetCoeff(p),C))
  {
    if (n_GreaterZero(pGetCoeff(p),C))
    {
      n_WriteLong(pGetCoeff(p),C);

      wroteCoef=TRUE;
      writeGen=TRUE;
    }
    else
      StringAppendS("-");
  }

  int i;
  {
    int lV = r->isLPring;
    int lastVar = p_mLastVblock(p, r) * lV;
    BOOLEAN wroteBlock = FALSE;
    for (i=0; i<rVar(r); i++)
    {
      {
        long ee = p_GetExp(p,i+1,r);
        BOOLEAN endOfBlock = ((i+1) % lV) == 0;
        BOOLEAN writeEmptyBlock = ee==0L && endOfBlock && !wroteBlock && i < lastVar;
        if (ee!=0L || writeEmptyBlock)
        {
          if (wroteBlock)
            StringAppendS("&");
          else if (wroteCoef)
            StringAppendS("*");
          //else
          wroteCoef=TRUE; //(bNotShortOut);
          writeGen=TRUE;
          if (writeEmptyBlock)
            StringAppendS("_");
          else
          {
            StringAppendS(rRingVar(i, r));
            if (ee != 1L)
            {
              StringAppend("^%ld", ee);
            }
            wroteBlock = TRUE;
          }
        }
        if (endOfBlock)
          wroteBlock = FALSE;
      }
    }
  }
  //StringAppend("{%d}",p->Order);
  if (p_GetComp(p, r) != (long)ko)
  {
    if (writeGen) StringAppendS("*");
    StringAppend("gen(%d)", p_GetComp(p, r));
  }
}
#endif

/// if possible print p in a short way...
void p_String0Short(const poly p, ring lmRing, ring tailRing)
{
  // NOTE: the following (non-thread-safe!) UGLYNESS
  // (changing naRing->ShortOut for a while) is due to Hans!
  // Just think of other ring using the VERY SAME naRing and possible
  // side-effects...
  const BOOLEAN bLMShortOut = rShortOut(lmRing);
  const BOOLEAN bTAILShortOut = rShortOut(tailRing);

  lmRing->ShortOut = rCanShortOut(lmRing);
  tailRing->ShortOut = rCanShortOut(tailRing);

  p_String0(p, lmRing, tailRing);

  lmRing->ShortOut = bLMShortOut;
  tailRing->ShortOut = bTAILShortOut;
}

/// print p in a long way...
void p_String0Long(const poly p, ring lmRing, ring tailRing)
{
  // NOTE: the following (non-thread-safe!) UGLYNESS
  // (changing naRing->ShortOut for a while) is due to Hans!
  // Just think of other ring using the VERY SAME naRing and possible
  // side-effects...
  // but this is not a problem: i/o is not thread-safe anyway.
  const BOOLEAN bLMShortOut = rShortOut(lmRing);
  const BOOLEAN bTAILShortOut = rShortOut(tailRing);

  lmRing->ShortOut = FALSE;
  tailRing->ShortOut = FALSE;

  p_String0(p, lmRing, tailRing);

  lmRing->ShortOut = bLMShortOut;
  tailRing->ShortOut = bTAILShortOut;
}


void p_String0(poly p, ring lmRing, ring tailRing)
{
  if (p == NULL)
  {
    StringAppendS("0");
    return;
  }
  p_Normalize(p,lmRing);
  if ((n_GetChar(lmRing->cf) == 0)
  && (nCoeff_is_transExt(lmRing->cf)))
    p_Normalize(p,lmRing); /* Manual/absfact.tst */
#ifdef HAVE_SHIFTBBA
  if(lmRing->isLPring)
  {
    if ((p_GetComp(p, lmRing) == 0) || (!lmRing->VectorOut))
    {
      writemonLP(p,0, lmRing);
      p = pNext(p);
      while (p!=NULL)
      {
        assume((p->coef==NULL)||(!n_IsZero(p->coef,tailRing->cf)));
        if ((p->coef==NULL)||n_GreaterZero(p->coef,tailRing->cf))
          StringAppendS("+");
        writemonLP(p,0, tailRing);
        p = pNext(p);
      }
      return;
    }
  }
  else
#endif
  {
    if ((p_GetComp(p, lmRing) == 0) || (!lmRing->VectorOut))
    {
      writemon(p,0, lmRing);
      p = pNext(p);
      while (p!=NULL)
      {
        assume((p->coef==NULL)||(!n_IsZero(p->coef,tailRing->cf)));
        if ((p->coef==NULL)||n_GreaterZero(p->coef,tailRing->cf))
          StringAppendS("+");
        writemon(p,0, tailRing);
        p = pNext(p);
      }
      return;
    }
  }

  long k = 1;
  StringAppendS("[");
#ifdef HAVE_SHIFTBBA
  if(lmRing->isLPring)
  {
    loop
    {
      while (k < p_GetComp(p,lmRing))
      {
        StringAppendS("0,");
        k++;
      }
      writemonLP(p,k,lmRing);
      pIter(p);
      while ((p!=NULL) && (k == p_GetComp(p, tailRing)))
      {
        if (n_GreaterZero(p->coef,tailRing->cf)) StringAppendS("+");
        writemonLP(p,k,tailRing);
        pIter(p);
      }
      if (p == NULL) break;
      StringAppendS(",");
      k++;
    }
  }
  else
#endif
  {
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
        if (n_GreaterZero(p->coef,tailRing->cf)) StringAppendS("+");
        writemon(p,k,tailRing);
        pIter(p);
      }
      if (p == NULL) break;
      StringAppendS(",");
      k++;
    }
  }
  StringAppendS("]");
}

char* p_String(poly p, ring lmRing, ring tailRing)
{
  StringSetS("");
  p_String0(p, lmRing, tailRing);
  return StringEndS();
}

/*2
* writes a polynomial p to stdout
*/
void p_Write0(poly p, ring lmRing, ring tailRing)
{
  char *s=p_String(p, lmRing, tailRing);
  PrintS(s);
  omFree(s);
}

/*2
* writes a polynomial p to stdout followed by \n
*/
void p_Write(poly p, ring lmRing, ring tailRing)
{
  p_Write0(p, lmRing, tailRing);
  PrintLn();
}

#if !defined(__OPTIMIZE__) || defined(KDEBUG)
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
#endif
void p_wrp(poly p, ring lmRing, ring tailRing)
{
  poly r;

  if (p==NULL) PrintS("NULL");
  else if (pNext(p)==NULL) p_Write0(p, lmRing);
  else
  {
    r = pNext(pNext(p));
    pNext(pNext(p)) = NULL;
    p_Write0(p, tailRing);
    if (r!=NULL)
    {
      PrintS("+...");
      pNext(pNext(p)) = r;
    }
  }
}

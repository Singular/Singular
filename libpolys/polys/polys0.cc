/****************************************
*  Computer Algebra System SINGULAR     *
****************************************/

/*
* ABSTRACT - all basic methods to convert polynomials to strings
*/

/* includes */

#include <misc/auxiliary.h>

// #include <polys/structs.h>
#include <coeffs/numbers.h>
#include <polys/monomials/ring.h>
#include <polys/monomials/p_polys.h>
// #include <???/febase.h>

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

  if (pGetCoeff(p)!=NULL)
    n_Normalize(pGetCoeff(p),C);

  if (((p_GetComp(p,r) == (short)ko)
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
  //StringAppend("{%d}",p->Order);
  if (p_GetComp(p, r) != (long)ko)
  {
    if (writeGen) StringAppendS("*");
    StringAppend("gen(%d)", p_GetComp(p, r));
  }
}

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

  long k = 1;
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
      if (n_GreaterZero(p->coef,tailRing->cf)) StringAppendS("+");
      writemon(p,k,tailRing);
      pIter(p);
    }
    if (p == NULL) break;
    StringAppendS(",");
    k++;
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
    p_Write0(p, lmRing, tailRing);
    if (r!=NULL)
    {
      PrintS("+...");
      pNext(pNext(p)) = r;
    }
  }
}

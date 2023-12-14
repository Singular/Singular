// emacs edit mode for this file is -*- C++ -*-

/****************************************
*  Computer Algebra System SINGULAR     *
****************************************/
/*
* ABSTRACT - The FGLM-Algorithm
*   Implementation of number-vectors for the fglm algorithm.
*   (See fglm.cc). Based on a letter-envelope implementation, mainly
*   written to be used by the fglm algorithm. Hence they are
*   specialized for this purpose.
*/




#include "kernel/mod2.h"

#include "kernel/structs.h"
#include "coeffs/numbers.h"
#include "fglm.h"
#include "fglmvec.h"

#define PROT(msg)
#define STICKYPROT(msg) if (BTEST1(OPT_PROT)) Print(msg)
#define PROT2(msg,arg)
#define STICKYPROT2(msg,arg) if (BTEST1(OPT_PROT)) Print(msg,arg)
#define fglmASSERT(ignore1,ignore2)

class fglmVectorRep
{
private:
  int ref_count;
  int N;
  number *elems;
public:
    fglmVectorRep ():ref_count (1), N (0), elems (0)
  {
  }
  fglmVectorRep (int n, number * e):ref_count (1), N (n), elems (e)
  {
  }
  fglmVectorRep (int n):ref_count (1), N (n)
  {
    fglmASSERT (N >= 0, "illegal Vector representation");
    if(N == 0)
      elems = 0;
    else
    {
      elems = (number *) omAlloc (N * sizeof (number));
      for(int i = N - 1; i >= 0; i--)
        elems[i] = nInit (0);
    }
  }
  ~fglmVectorRep ()
  {
    if(N > 0)
    {
      for(int i = N - 1; i >= 0; i--)
        nDelete (elems + i);
      omFreeSize ((ADDRESS) elems, N * sizeof (number));
    }
  }

  fglmVectorRep *clone () const
  {
    if(N > 0)
    {
      number *elems_clone;
        elems_clone = (number *) omAlloc (N * sizeof (number));
      for(int i = N - 1; i >= 0; i--)
          elems_clone[i] = nCopy (elems[i]);
        return new fglmVectorRep (N, elems_clone);
    }
    else
        return new fglmVectorRep (N, 0);
  }
  BOOLEAN deleteObject ()
  {
    return --ref_count == 0;
  }
  fglmVectorRep *copyObject ()
  {
    ref_count++;
    return this;
  }
  int refcount () const
  {
    return ref_count;
  }
  BOOLEAN isUnique () const
  {
    return ref_count == 1;
  }

  int size () const
  {
    return N;
  }
  int isZero () const
  {
    int k;
    for(k = N; k > 0; k--)
    {
      if(!nIsZero (getconstelem (k)))
          return 0;
    }
    return 1;
  }
  int numNonZeroElems () const
  {
    int num = 0;
    int k;
    for(k = N; k > 0; k--)
    {
      if(!nIsZero (getconstelem (k)))
          num++;
    }
    return num;
  }
  void setelem (int i, number n)
  {
    fglmASSERT (0 < i && i <= N, "setelem: wrong index");
    nDelete (elems + i - 1);
    elems[i - 1] = n;
  }
  number ejectelem (int i, number n)
  {
    fglmASSERT (isUnique (), "should only be called if unique!");
    number temp = elems[i - 1];
    elems[i - 1] = n;
    return temp;
  }
  number & getelem (int i)
  {
    fglmASSERT (0 < i && i <= N, "getelem: wrong index");
    return elems[i - 1];
  }
  number getconstelem (int i) const
  {
    fglmASSERT (0 < i && i <= N, "getconstelem: wrong index");
    return elems[i - 1];
  }
  friend class fglmVector;
};


///--------------------------------------------------------------------------------
/// Implementation of class fglmVector
///--------------------------------------------------------------------------------

fglmVector::fglmVector (fglmVectorRep * r):rep (r)
{
}

fglmVector::fglmVector ():rep (new fglmVectorRep ())
{
}

fglmVector::fglmVector (int size):rep (new fglmVectorRep (size))
{
}

fglmVector::fglmVector (int size, int basis):rep (new fglmVectorRep (size))
{
  rep->setelem (basis, nInit (1));
}

fglmVector::fglmVector (const fglmVector & v)
{
  rep = v.rep->copyObject ();
}

fglmVector::~fglmVector ()
{
  if(rep->deleteObject ())
    delete rep;
}

#ifndef HAVE_EXPLICIT_CONSTR
void fglmVector::mac_constr (const fglmVector & v)
{
  rep = v.rep->copyObject ();
}

void fglmVector::mac_constr_i (int size)
{
  rep = new fglmVectorRep (size);
}

void fglmVector::clearelems ()
{
  if(rep->deleteObject ())
    delete rep;
}
#endif

void fglmVector::makeUnique ()
{
  if(rep->refcount () != 1)
  {
    rep->deleteObject ();
    rep = rep->clone ();
  }
}

int fglmVector::size () const
{
  return rep->size ();
}

int fglmVector::numNonZeroElems () const
{
  return rep->numNonZeroElems ();
}

void
  fglmVector::nihilate (const number fac1, const number fac2,
                        const fglmVector v)
{
  int i;
  int vsize = v.size ();
  number term1, term2;
  fglmASSERT (vsize <= rep->size (), "v has to be smaller or equal");
  if(rep->isUnique ())
  {
    for(i = vsize; i > 0; i--)
    {
      term1 = nMult (fac1, rep->getconstelem (i));
      term2 = nMult (fac2, v.rep->getconstelem (i));
      rep->setelem (i, nSub (term1, term2));
      nDelete (&term1);
      nDelete (&term2);
    }
    for(i = rep->size (); i > vsize; i--)
    {
      rep->setelem (i, nMult (fac1, rep->getconstelem (i)));
    }
  }
  else
  {
    number *newelems;
    newelems = (number *) omAlloc (rep->size () * sizeof (number));
    for(i = vsize; i > 0; i--)
    {
      term1 = nMult (fac1, rep->getconstelem (i));
      term2 = nMult (fac2, v.rep->getconstelem (i));
      newelems[i - 1] = nSub (term1, term2);
      nDelete (&term1);
      nDelete (&term2);
    }
    for(i = rep->size (); i > vsize; i--)
    {
      newelems[i - 1] = nMult (fac1, rep->getconstelem (i));
    }
    rep->deleteObject ();
    rep = new fglmVectorRep (rep->size (), newelems);
  }
}

fglmVector & fglmVector::operator = (const fglmVector & v)
{
  if(this != &v)
  {
    if(rep->deleteObject ())
      delete rep;
    rep = v.rep->copyObject ();
  }
  return *this;
}

int fglmVector::operator == (const fglmVector & v)
{
  if(rep->size () == v.rep->size ())
  {
    if(rep == v.rep)
      return 1;
    else
    {
      int i;
      for(i = rep->size (); i > 0; i--)
        if(!nEqual (rep->getconstelem (i), v.rep->getconstelem (i)))
          return 0;
      return 1;
    }
  }
  return 0;
}

int fglmVector::operator != (const fglmVector & v)
{
  return !(*this == v);
}

int fglmVector::isZero ()
{
  return rep->isZero ();
}

int fglmVector::elemIsZero (int i)
{
  return nIsZero (rep->getconstelem (i));
}

fglmVector & fglmVector::operator += (const fglmVector & v)
{
  fglmASSERT (size () == v.size (), "incompatible vectors");
  // ACHTUNG : Das Verhalten hier mit gcd genau ueberpruefen!
  int i;
  if(rep->isUnique ())
  {
    for(i = rep->size (); i > 0; i--)
      n_InpAdd (rep->getelem(i), v.rep->getconstelem (i), currRing->cf);
  }
  else
  {
    int n = rep->size ();
    number *newelems;
    newelems = (number *) omAlloc (n * sizeof (number));
    for(i = n; i > 0; i--)
      newelems[i - 1] = nAdd (rep->getconstelem (i), v.rep->getconstelem (i));
    rep->deleteObject ();
    rep = new fglmVectorRep (n, newelems);
  }
  return *this;
}

fglmVector & fglmVector::operator -= (const fglmVector & v)
{
  fglmASSERT (size () == v.size (), "incompatible vectors");
  int i;
  if(rep->isUnique ())
  {
    for(i = rep->size (); i > 0; i--)
      rep->setelem (i, nSub (rep->getconstelem (i), v.rep->getconstelem (i)));
  }
  else
  {
    int n = rep->size ();
    number *newelems;
    newelems = (number *) omAlloc (n * sizeof (number));
    for(i = n; i > 0; i--)
      newelems[i - 1] = nSub (rep->getconstelem (i), v.rep->getconstelem (i));
    rep->deleteObject ();
    rep = new fglmVectorRep (n, newelems);
  }
  return *this;
}

fglmVector & fglmVector::operator *= (const number & n)
{
  int s = rep->size ();
  int i;
  if(!rep->isUnique ())
  {
    number *temp;
    temp = (number *) omAlloc (s * sizeof (number));
    for(i = s; i > 0; i--)
      temp[i - 1] = nMult (rep->getconstelem (i), n);
    rep->deleteObject ();
    rep = new fglmVectorRep (s, temp);
  }
  else
  {
    for(i = s; i > 0; i--)
      nInpMult (rep->getelem (i), n);
  }
  return *this;
}

fglmVector & fglmVector::operator /= (const number & n)
{
  int s = rep->size ();
  int i;
  if(!rep->isUnique ())
  {
    number *temp;
    temp = (number *) omAlloc (s * sizeof (number));
    for(i = s; i > 0; i--)
    {
      temp[i - 1] = nDiv (rep->getconstelem (i), n);
      nNormalize (temp[i - 1]);
    }
    rep->deleteObject ();
    rep = new fglmVectorRep (s, temp);
  }
  else
  {
    for(i = s; i > 0; i--)
    {
      rep->setelem (i, nDiv (rep->getconstelem (i), n));
      nNormalize (rep->getelem (i));
    }
  }
  return *this;
}

fglmVector operator - (const fglmVector & v)
{
  fglmVector temp (v.size ());
  int i;
  number n;
  for(i = v.size (); i > 0; i--)
  {
    n = nCopy (v.getconstelem (i));
    n = nInpNeg (n);
    temp.setelem (i, n);
  }
  return temp;
}

fglmVector operator + (const fglmVector & lhs, const fglmVector & rhs)
{
  fglmVector temp = lhs;
  temp += rhs;
  return temp;
}

fglmVector operator - (const fglmVector & lhs, const fglmVector & rhs)
{
  fglmVector temp = lhs;
  temp -= rhs;
  return temp;
}

fglmVector operator * (const fglmVector & v, const number n)
{
  fglmVector temp = v;
  temp *= n;
  return temp;
}

fglmVector operator * (const number n, const fglmVector & v)
{
  fglmVector temp = v;
  temp *= n;
  return temp;
}

number & fglmVector::getelem (int i)
{
  makeUnique ();
  return rep->getelem (i);
}

number fglmVector::getconstelem (int i) const
{
  return rep->getconstelem (i);
}

void fglmVector::setelem (int i, number & n)
{
  makeUnique ();
  rep->setelem (i, n);
  n = n_Init (0, currRing->cf);
}

number fglmVector::gcd () const
{
  int i = rep->size ();
  BOOLEAN found = FALSE;
  BOOLEAN gcdIsOne = FALSE;
  number theGcd;
  number current;
  while(i > 0 && !found)
  {
    current = rep->getconstelem (i);
    if(!nIsZero (current))
    {
      theGcd = nCopy (current);
      found = TRUE;
      if(!nGreaterZero (theGcd))
      {
        theGcd = nInpNeg (theGcd);
      }
      if(nIsOne (theGcd))
        gcdIsOne = TRUE;
    }
    i--;
  }
  if(found)
  {
    if (getCoeffType(currRing->cf)!=n_Zp)
    {
      while(i > 0 && !gcdIsOne)
      {
        current = rep->getconstelem (i);
        if(!nIsZero (current))
        {
          number temp = n_SubringGcd (theGcd, current, currRing->cf);
          nDelete (&theGcd);
          theGcd = temp;
          if(nIsOne (theGcd))
            gcdIsOne = TRUE;
        }
        i--;
      }
    }
  }
  else
    theGcd = nInit (0);
  return theGcd;
}

number fglmVector::clearDenom ()
{
  number theLcm = nInit (1);
  if (getCoeffType(currRing->cf)==n_Zp)
  {
    return theLcm;
  }
  BOOLEAN isZero = TRUE;
  int i;
  for(i = size (); i > 0; i--)
  {
    if(!nIsZero (rep->getconstelem (i)))
    {
      isZero = FALSE;
      number temp = n_NormalizeHelper (theLcm, rep->getconstelem (i), currRing->cf);
      nDelete (&theLcm);
      theLcm = temp;
    }
  }
  if(isZero)
  {
    nDelete (&theLcm);
    theLcm = nInit (0);
  }
  else
  {
    if(!nIsOne (theLcm))
    {
      *this *= theLcm;
      for(i = size (); i > 0; i--)
      {
        nNormalize (rep->getelem (i));
      }
    }
  }
  return theLcm;
}

// ----------------------------------------------------------------------------
// Local Variables: ***
// compile-command: "make Singular" ***
// page-delimiter: "^\\(\\|//!\\)" ***
// fold-internal-margins: nil ***
// End: ***

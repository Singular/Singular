/*****************************************************************************\
 * Computer Algebra System SINGULAR
\*****************************************************************************/
/** @file cfCharSetsUtil.cc
 *
 * This file provides utility functions to compute characteristic sets
 *
 * @note some of the code is code from libfac or derived from code from libfac.
 * Libfac is written by M. Messollen. See also COPYING for license information
 * and README for general information on characteristic sets.
 *
 * @authors Martin Lee
 *
 **/
/*****************************************************************************/

#include "config.h"

#include "canonicalform.h"
#include "cf_algorithm.h"
#include "cfCharSetsUtil.h"

#define __ARRAY_INIT__ -1

// the maximal degree of polys in PS wrt. variable x
int
degpsmax (const CFList & PS, const Variable & x,
          Intarray & A, Intarray & C)
{
  int varlevel= level(x);
  if (A[varlevel] != __ARRAY_INIT__)
    return A[varlevel];
  int max= 0, temp, count= 0;

  for (CFListIterator i= PS; i.hasItem(); i++)
  {
    temp= degree (i.getItem(), x);
    if (temp > max)
    {
      max= temp;
      count = 0;
    }
    if (temp == max)
      count += max;  // we count the number of polys
  }
  A[varlevel]= max;
  C[varlevel]= count;
  return max;
}

// the minimal non-zero degree of polys in PS wrt. x
// returns 0 if variable x doesn't occure in any of the polys
int
degpsmin (const CFList & PS, const Variable & x, Intarray & A, Intarray & B,
          Intarray & C, Intarray & D)
{
  int varlevel= level(x);
  if (B[varlevel] != __ARRAY_INIT__ )
    return B[varlevel];
  int min= degpsmax (PS, x, A, C), temp, count= 0;

  if (min == 0)
  {
    B[varlevel]= min;
    D[varlevel]= min;
    return min;
  }
  else
  {
    for (CFListIterator i= PS; i.hasItem(); i++)
    {
      temp= degree (i.getItem(), x);
      if (temp < min && temp != 0)
      {
        min= temp;
        count= 0;
      }
      if (temp == min)
        count += min; // we count the number of polys
    }
  }
  B[varlevel]= min;
  D[varlevel]= count;
  return min;
}

// the minimal total degree of lcoeffs of polys in PS wrt. x
// for those polys having degree degpsmin in x.
// F will be assigned the minimal number of terms of those lcoeffs
int
Tdeg (const CFList & PS, const Variable & x, Intarray & A, Intarray & B,
      Intarray & C, Intarray & D, Intarray & E, Intarray & F)
{
  int k= degpsmin (PS, x, A, B, C, D), varlevel= level(x), min= 0;

  if (E[varlevel] != __ARRAY_INIT__)
    return E [varlevel];
  if (k == 0)
  {
    E[varlevel]= 0;
    F[varlevel]= 0;
  }
  else
  {
    int nopslc= 0;
    CFList LCdegList;
    CanonicalForm elem;
    CFListIterator i;

    for (i= PS; i.hasItem(); i++)
    {
      elem= i.getItem();
      if (degree (elem, x) == k)
        LCdegList.append (LC (elem, x));
    }

    if (LCdegList.length() > 0)
    {
      CFList TermList;
      int newmin, newnopslc;

      min= totaldegree (LCdegList.getFirst());
      TermList= get_Terms (LCdegList.getFirst());
      nopslc= TermList.length();
      for (i= LCdegList; i.hasItem(); i++)
      {
        elem= i.getItem();
        newmin= totaldegree(elem);
        TermList= get_Terms(elem);
        newnopslc= TermList.length();
        if (newmin < min)
          min= newmin;
        if (newnopslc < nopslc)
          nopslc= newnopslc;
      }

    }
    E[varlevel]= min;
    F[varlevel]= nopslc;
  }
  return min;
}

// The number of the poly in which Variable x first occures
int
nr_of_poly( const CFList & PS, const Variable & x, Intarray & G)
{
  int min= 0, varlevel= level(x);
  if (G[varlevel] != __ARRAY_INIT__)
    return G[varlevel];
  for (CFListIterator i= PS; i.hasItem(); i++)
  {
    min += 1;
    if (degree (i.getItem(), x) > 0)
      break;
  }
  G[varlevel]= min;
  return min;
}

int
degord (const Variable & x, const Variable & y, const CFList & PS,
        Intarray & A, Intarray & B, Intarray & C, Intarray & D,
        Intarray & E, Intarray & F, Intarray & G)
{
  int xlevel= level(x), ylevel= level(y);

  if      (degpsmax(PS,y,A,C) < degpsmax(PS,x,A,C))         return 1;
  else if (degpsmax(PS,x,A,C) < degpsmax(PS,y,A,C) )        return 0;
  else if (C[ylevel] < C[xlevel])                           return 1;
  else if (C[xlevel] < C[ylevel])                           return 0;
  else if (degpsmin(PS,x,A,B,C,D) < degpsmin(PS,y,A,B,C,D)) return 1;
  else if (degpsmin(PS,y,A,B,C,D) < degpsmin(PS,x,A,B,C,D)) return 0;
  else if (D[ylevel] < D[xlevel])                           return 1;
  else if (D[xlevel] < D[ylevel])                           return 0;
  else if (Tdeg(PS,y,A,B,C,D,E,F) < Tdeg(PS,x,A,B,C,D,E,F)) return 1;
  else if (Tdeg(PS,x,A,B,C,D,E,F) < Tdeg(PS,y,A,B,C,D,E,F)) return 0;
  else if (F[ylevel] < F[xlevel])                           return 1;
  else if (F[xlevel] < F[ylevel])                           return 0;
  else if (nr_of_poly(PS,x,G) <= nr_of_poly(PS,y,G))        return 1;
  else return 0;
}

// determine the highest variable of all involved Variables in PS
// NOTE:
//    this doesn't give always the correct answer:
//    If a variable is assigned the highest level in the definition of the
//    original ring, but doesn't occure in any of the
//    polynomials, get_max_var returns the variable with a level lower than
//    the highest level.
//    Is there a workaround?
// But for the redefinition of the ring this doesn't matter due to the
// implementation of neworder().

Variable
get_max_var (const CFList & PS)
{
  Variable x= PS.getFirst().mvar(), y;
  for (CFListIterator i= PS; i.hasItem(); i++)
  {
    y= i.getItem().mvar();
    if (y > x)
      x= y;
  }
  return x;
}


// determine if variable x is in one and only one of the polynomials in PS
// first criterion for neworder
CFList
only_in_one (const CFList & PS, const Variable & x)
{
  CFList output;

  for (CFListIterator i= PS; i.hasItem(); i++ )
  {
    if (degree (i.getItem(), x) >= 1)
      output.insert (i.getItem());
    if (output.length() >= 2)
      break;
  }
  return output;
}

// initialize all Arrays (of same range) with __ARRAY_INIT__
void
initArray (const int highest_level, Intarray & A, Intarray & B, Intarray & C,
           Intarray & D, Intarray & E, Intarray & F, Intarray & G)
{
  for (int i=1 ; i <= highest_level; i ++)
  {
    A[i]= __ARRAY_INIT__;
    B[i]= __ARRAY_INIT__;
    C[i]= __ARRAY_INIT__;
    D[i]= __ARRAY_INIT__;
    E[i]= __ARRAY_INIT__;
    F[i]= __ARRAY_INIT__;
    G[i]= __ARRAY_INIT__;
  }
}

// now for the second criterion; a little more complex
//
// idea: set up seven arrays of lenth highest_level
//       (of which some entries may be empty, because of only_in_one!)
//   A saves maxdegree of Variable x in A(level(x))
//   B saves mindegree of Variable x in B(level(x))
//   C saves the number of polys in PS which have degree A(level(x)) in
//     D(level(x))
//   D saves the number of polys in PS which have degree B(level(x)) in
//     D(level(x))
//   E saves the minimal total degree of lcoeffs of polys wrt x in E(level(x))
//   F saves the minimal number of terms of lcoeffs of E(level(x)) in F(~)
//   G saves nr of poly Variable x has first deg <> 0

#define __INIT_GAP__ 3
Varlist
reorderb (const Varlist & difference, const CFList & PS,
          const int highest_level)
{
  Intarray A(1, highest_level), B(1, highest_level), C(1, highest_level),
           D(1, highest_level), E(1, highest_level), F(1, highest_level),
           G(1, highest_level);
  initArray (highest_level, A, B, C, D, E, F, G);
  int i= 0, j, n= difference.length(), gap= 1 + __INIT_GAP__;
  Variable temp;
  Array<Variable> v(0, n);
  VarlistIterator J;

  for (J= difference; J.hasItem(); J++ )
  {
    v[i]= J.getItem();
    i++;
  }

  while (gap <= n)
    gap = __INIT_GAP__ * gap + 1;
  gap /= __INIT_GAP__;

  while (gap > 0)
  {
    for (i= gap; i <= n - 1; i++)
    {
      temp= v[i];
      for (j= i - gap; j >=0 ; j -= gap)
      {
        if (degord (v[j], temp, PS, A, B, C, D, E, F, G))
          break;
        v[j + gap]= v[j];
      }
      v[j + gap]= temp;
    }
    gap /= __INIT_GAP__;
  }
  Varlist output;
  for (i= 0; i <= n - 1; i++)
    output.append (v[i]);
  return output;
}

/// swapvar a whole list of CanonicalForms
CFList
swapvar (const CFList & PS, const Variable & x, const Variable & y)
{
  CFList ps;

  for (CFListIterator i= PS; i.hasItem(); i++)
    ps.append (swapvar (i.getItem(), x, y));
  return ps;
}

CFFList
swapvar (const CFFList & PS, const Variable & x, const Variable & y)
{
  CFFList ps;

  for (CFFListIterator i= PS; i.hasItem(); i++)
    ps.append (CFFactor (swapvar (i.getItem().factor(), x, y),
                         i.getItem().exp()));
  return ps;
}

bool
lowerRank (const CanonicalForm & F, const CanonicalForm & G, int & ind)
{
  int degF, degG, levelF, levelG;

  levelF= F.level();
  levelG= G.level();
  if (F.inCoeffDomain())
  {
    if (G.inCoeffDomain())
      ind= 1;
    return true;
  }
  else if (G.inCoeffDomain())
    return false;
  else if (levelF < levelG)
    return true;
  else if (levelF == levelG)
  {
    degF= degree(F);
    degG= degree(G);
    if (degF < degG)
      return true;
    else if (degF == degG)
      return lowerRank (LC (F), LC (G), ind);
    else
      return false;
  }
  return false;
}


CanonicalForm
lowestRank (const CFList & L)
{
  CFListIterator i= L;
  CanonicalForm f;
  int ind= 0;
  if (!i.hasItem())
    return f;

  f= i.getItem();
  i++;

  while (i.hasItem())
  {
    if (lowerRank (i.getItem(), f, ind))
    {
      if (ind)
      {
        if (size (i.getItem()) < size (f))
          f= i.getItem();
        ind= 0;
      }
      else
        f= i.getItem();
    }
    i++;
  }
  return f;
}

int minLevel (const CFList& L)
{
  if (L.isEmpty())
    return 0;
  int min= size (L.getFirst());
  return min;
}

/// sort in descending order of length of elements
void
sortListCFList (ListCFList& list)
{
  int l= 1;
  int k= 1;
  CFList buf;
  ListCFListIterator m;
  for (ListCFListIterator i= list; l <= list.length(); i++, l++)
  {
    for (ListCFListIterator j= list; k <= list.length() - l; k++)
    {
      m= j;
      m++;
      if ((j.getItem().length() < m.getItem().length()) ||
          (j.getItem().length() == m.getItem().length() &&
           minLevel (j.getItem()) > minLevel (m.getItem())))
      {
        buf= m.getItem();
        m.getItem()= j.getItem();
        j.getItem()= buf;
        j++;
        j.getItem()= m.getItem();
      }
      else
        j++;
    }
    k= 1;
  }
}


/// sort in descending order of level of elements
void
sortCFListByLevel (CFList& list)
{
  int l= 1;
  int k= 1;
  CanonicalForm buf;
  CFListIterator m;
  for (CFListIterator i= list; l <= list.length(); i++, l++)
  {
    for (CFListIterator j= list; k <= list.length() - l; k++)
    {
      m= j;
      m++;
      if ((size (j.getItem()) < size (m.getItem())) ||
          ((size (j.getItem()) == size (m.getItem()))
            && (j.getItem().level() < m.getItem().level())))
      {
        buf= m.getItem();
        m.getItem()= j.getItem();
        j.getItem()= buf;
        j++;
        j.getItem()= m.getItem();
      }
      else
        j++;
    }
    k= 1;
  }
}


/* basic operations on lists */
/// is PS a subset of Cset ?
bool
isSubset (const CFList &PS, const CFList& Cset)
{
  for (CFListIterator i= PS; i.hasItem(); i++)
  {
    if (!find (Cset, i.getItem()))
      return 0;
  }
  return 1;
}

/// Union of a and b stored in b
void
inplaceUnion (const ListCFList& a, ListCFList& b)
{
  if (a.isEmpty())
    return;
  if (b.isEmpty())
  {
    b= a;
    return;
  }

  ListCFListIterator i;
  CFList elem;

  for (i= a; i.hasItem(); i++)
  {
    elem= i.getItem();
    if ((!elem.isEmpty()) && (!find (b, elem)))
      b.insert(elem);
  }
}

ListCFList
adjoin (const CFList& is, const CFList& qs, const ListCFList& qh)
{
  ListCFList iss, qhi;
  ListCFListIterator j;
  CFList iscopy, itt;
  CFListIterator i;
  int ind, length;

  for (i= is; i.hasItem(); i++)
  {
    if (i.getItem().level() > 0)
      iscopy= Union (CFList (i.getItem()), iscopy);
  }
  if (iscopy.isEmpty())
    return iss;

  qhi= Difference (qh, qs);
  length= qhi.length();

  for (i= iscopy; i.hasItem(); i++)
  {
    itt= Union (qs, CFList (i.getItem()));
    ind= 0;
    if (length > 0)
    {
      for (j= qhi; j.hasItem(); j++)
      {
        if (isSubset (j.getItem(), itt))
          ind= 1;
      }
    }
    if (ind == 0)
      iss.append (itt);
  }
  return iss;
}

ListCFList
adjoinb (const CFList & is, const CFList & qs, const ListCFList & qh,
         const CFList & cs)
{
  ListCFList iss, qhi;
  ListCFListIterator j;
  CFList iscopy, itt;
  CFListIterator i;
  int ind, length;

  for (i= is ; i.hasItem(); i++)
  {
    if (i.getItem().level() > 0)
      iscopy= Union (CFList (i.getItem()), iscopy);
  }
  if (iscopy.isEmpty())
    return iss;
  qhi= Difference (qh, qs);
  length= qhi.length();
  for (i= iscopy; i.hasItem(); i++)
  {
    itt= Union (Union (qs, CFList (i.getItem())), cs);
    ind= 0;
    if (length > 0)
    {
      for (j= qhi; j.hasItem(); j++)
      {
        if (isSubset (j.getItem(), itt))
          ind= 1;
      }
    }
    if (ind == 0)
      iss.append(itt);
  }
  return iss;
}

void
select (const ListCFList& ppi, int length, ListCFList& ppi1, ListCFList& ppi2)
{
  CFList elem;
  for (ListCFListIterator i= ppi; i.hasItem(); i++)
  {
    elem= i.getItem();
    if (!elem.isEmpty())
    {
      if (length <= elem.length())
        ppi2.append(elem);
      else
        ppi1.append(elem);
    }
  }
}

/* end basic operations on lists */

/// normalize a poly, i.e. in char 0 clear denominators, remove integer content
/// in char p divide by leading coeff
CanonicalForm normalize (const CanonicalForm& F)
{
  if (F.isZero())
    return F;
  if (getCharacteristic() == 0)
  {
    CanonicalForm G;
    bool isRat= isOn (SW_RATIONAL);
    if (!isRat)
      On (SW_RATIONAL);
    G= F;
    G *= bCommonDen (G);
    Off (SW_RATIONAL);
    G /= icontent (G);
    if (isRat)
      On (SW_RATIONAL);
    if (lc(G) < 0)
      G= -G;
    return G;
  }

  return F/lc (F);
}

/// pseudo remainder of F by G with certain factors of LC (g) cancelled
CanonicalForm
Prem (const CanonicalForm& F, const CanonicalForm& G)
{
  CanonicalForm f, g, l, test, lu, lv, t, retvalue;
  int degF, degG, levelF, levelG;
  bool reord;
  Variable v, vg= G.mvar();

  if ( (levelF= F.level()) < (levelG= G.level()))
    return F;
  else
  {
    if ( levelF == levelG )
    {
      f= F;
      g= G;
      reord= false;
      v= F.mvar();
    }
    else
    {
      v= Variable (levelF + 1);
      f= swapvar (F, vg, v);
      g= swapvar (G, vg, v);
      reord= true;
    }
    degG= degree (g, v );
    degF= degree (f, v );
    if (degG <= degF)
    {
      l= LC (g);
      g= g - l*power (v, degG);
    }
    else
      l= 1;
    while ((degG <= degF) && (!f.isZero()))
    {
      test= gcd (l, LC(f));
      lu= l / test;
      lv= LC(f) / test;
      t= g*lv*power (v, degF - degG);

      if (degF == 0)
        f= 0;
      else
        f= f - LC (f)*power (v, degF);

      f= f*lu - t;
      degF= degree (f, v);
    }

    if (reord)
      retvalue= swapvar (f, vg, v);
    else
      retvalue= f;

    return retvalue;
  }
}

/// pseudo remainder of f by L with faster test for remainder being zero
CanonicalForm
Premb (const CanonicalForm &f, const CFList &L)
{
  CanonicalForm rem= f;
  CFList l= L;
  l.removeFirst();
  CFListIterator i= l;

  for (i.lastItem(); i.hasItem(); i--)
    rem= normalize (Prem (rem, i.getItem()));

  CanonicalForm tmp= L.getFirst()/content (L.getFirst());

  bool isRat= isOn (SW_RATIONAL);
  int ch=getCharacteristic();
  if (ch == 0 && !isRat)
    On (SW_RATIONAL);
  if (fdivides (tmp, rem))
  {
    if (ch == 0 && !isRat)
      Off (SW_RATIONAL);
    return 0;
  }

  if (ch == 0 && !isRat)
    Off (SW_RATIONAL);

  rem= normalize (Prem (rem, L.getFirst()));

  return rem;
}

/// pseudo remainder of f by L
CanonicalForm
Prem (const CanonicalForm &f, const CFList &L)
{
  CanonicalForm rem= f;
  CFListIterator i= L;

  for (i.lastItem(); i.hasItem(); i--)
    rem= normalize (Prem (rem, i.getItem()));

  return rem;
}

CFList uniGcd (const CFList& L)
{
  CFList tmp;
  CanonicalForm g;
  CFListIterator i;
  for (i= L; i.hasItem(); i++)
  {
    if (i.getItem().isUnivariate() && i.getItem().level() == 1)
      tmp.append (i.getItem());
  }
  if (tmp.length() <= 2)
    return L;
  i= tmp;
  g= i.getItem();
  i++;
  g= gcd (g,i.getItem());
  i++;
  for (; i.hasItem(); i++)
    g= gcd (g, i.getItem());
  return Union (Difference (L, tmp), CFList (g));
}

CFList initials (const CFList& L)
{
  CFList result;
  for (CFListIterator iter= L; iter.hasItem(); iter++)
  {
    if (!LC(iter.getItem()).inCoeffDomain())
      result.append (LC (iter.getItem()));
  }
  return result;
}

CFList
factorsOfInitials(const CFList & L)
{
  CFList result;
  CFFList factors;
  CanonicalForm tmp;

  for (CFListIterator i= L; i.hasItem(); i++)
  {
    factors= factorize (LC (i.getItem()));
    for (CFFListIterator j= factors; j.hasItem(); j++)
    {
      tmp= j.getItem().factor();
      if (!tmp.inCoeffDomain())
        result= Union (result, CFList (normalize (tmp)));
    }
  }

  return result;
}

void
removeContent (CanonicalForm& F, CanonicalForm& cF)
{
  if (size (F) == 1)
  {
    CanonicalForm tmp= F;
    F= F.mvar();
    cF= tmp/F;
    if (!cF.inCoeffDomain())
      cF= normalize (cF);
    else
      cF= 0;
    F= normalize (F);

    return;
  }

  cF= content (F);

  if (cF.inCoeffDomain())
    cF= 0;
  else
  {
    cF= normalize (cF);
    F /= cF;
    F= normalize (F);
  }
}

CFList
factorPSet (const CFList& PS)
{
  CFList result;
  CFFList factors;
  CFFListIterator j;

  for (CFListIterator i= PS; i. hasItem(); i++)
  {
    factors= factorize (i.getItem());
    if (factors.getFirst().factor().inCoeffDomain())
      factors.removeFirst();
    for (j= factors; j.hasItem(); j++ )
      result= Union (result, CFList (normalize (j.getItem().factor())));
  }
  return result;
}

void
removeFactors (CanonicalForm& r, StoreFactors& StoredFactors,
               CFList& removedFactors)
{
  CanonicalForm quot;
  CFList testlist;
  int n= level(r);
  bool divides;
  CFListIterator j;

  for (int i=1; i<= n; i++)
    testlist.append (CanonicalForm (Variable (i)));

  // remove already removed factors
  for (j= StoredFactors.FS1; j.hasItem(); j++)
  {
    while (fdivides (j.getItem(), r, quot))
    {
      r= quot;
    }
  }

  for (j= StoredFactors.FS2; j.hasItem(); j++)
  {
    divides= false;
    if (j.getItem() != r)
    {
      while (fdivides (j.getItem(), r, quot))
      {
        divides= true;
        r= quot;
      }
      if (divides)
        removedFactors= Union (removedFactors, CFList (j.getItem()));
    }
  }
  r= normalize (r);

  // remove variables
  for (j= testlist; j.hasItem() && !r.isOne(); j++)
  {
    divides= false;
    if (j.getItem() != r)
    {
      while (fdivides (j.getItem(), r, quot))
      {
        divides= true;
        r= quot;
      }
      if (divides)
        removedFactors= Union (removedFactors, CFList (j.getItem()));
     }
  }
  r= normalize (r);
}

CFList
removeContent (const CFList & PS, StoreFactors & StoredFactors)
{
  CFListIterator i= PS;
  if ((!i.hasItem()) || (PS.getFirst().level() == 0 ))
    return PS;

  CFList output;
  CanonicalForm cc,elem;

  for (; i.hasItem(); i++)
  {
    elem= i.getItem();
    cc= content (elem, elem.mvar());
    if (cc.level() > 0 )
    {
      output.append (normalize (elem / cc));
      StoredFactors.FS1 = Union (CFList (normalize (cc)), StoredFactors.FS1);
    }
    else
      output.append(normalize (elem));
  }
  return output;
}

bool
contractsub (const CFList& cs1, const CFList& cs2)
{
  CFListIterator i;

  CanonicalForm r;
  for (i= cs1; i.hasItem(); i++)
  {
    if (Prem (i.getItem(), cs2) != 0)
      return false;
  }

  CFList is= factorsOfInitials (cs1);

  for (i= is; i.hasItem(); i++)
  {
    if (Prem (i.getItem(), cs2) == 0)
      return false;
  }
  return true;
}

ListCFList
contract (const ListCFList& cs)
{
  ListCFList mem, ts;
  CFList iitem, jitem;

  if (cs.length() < 2)
    return cs;

  int l= cs.length();
  int ii= 1;
  ListCFListIterator j;
  for (ListCFListIterator i= cs; i.hasItem() && ii < l; i++, ii++)
  {
    iitem= i.getItem();
    if (!find (mem, iitem))
    {
      j= i;
      j++;
      for (; j.hasItem(); j++)
      {
        jitem= j.getItem();
        if (!find (mem, jitem))
        {
          if (contractsub (iitem, jitem))
          {
            ts.append (jitem);
            mem.append (jitem);
          }
          else
          {
            if (contractsub (jitem, iitem))
              ts.append (iitem); // no mem.append (item) because we assume cs does not contain duplicate entries
          }
        }
      }
    }
  }
  return Difference (cs,ts);
}


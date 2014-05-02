/*****************************************************************************\
 * Computer Algebra System SINGULAR
\*****************************************************************************/
/** @file cfCharSets.cc
 *
 * This file provides functions to compute characteristic sets
 * 
 * ABSTRACT: Descriptions can be found in Wang "On the Parallelization of
 * characteristic-set based algorithms" or Greuel/Pfister "A Singular
 * Introduction to Commutative Algebra".
 *
 * @authors Martin Lee, Michael Messollen
 *
 **/
/*****************************************************************************/

#include <ctime>

#include "cfCharSets.h"
#include "canonicalform.h"
#include "cf_algorithm.h"
#include "facAlgFunc.h"

static bool
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

static
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

// sort in descending order of length of elements
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
      if (j.getItem().length() < m.getItem().length())
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


// sort in descending order of level of elements
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

static bool
member (const CanonicalForm& f, const CFList& F)
{
  for (CFListIterator i= F; i.hasItem(); i++)
  {
    if (i.getItem().mapinto() == f.mapinto())
      return 1;
  }
  return 0;
}

// are list A and B the same?
static bool
same (const CFList& A, const CFList& B)
{
  if (A.length() != B.length())
    return 0;

  CFListIterator i;

  for (i= A; i.hasItem(); i++)
  {
    if (!member (i.getItem(), B))
      return 0;
  }
  for (i= B; i.hasItem(); i++)
  {
    if (!member (i.getItem(), A))
      return 0;
  }
  return 1;
}


// is List cs contained in List of lists pi?
static bool
member (const CFList& cs, const ListCFList& pi)
{
  if (pi.isEmpty())
    return 0;

  ListCFListIterator i;

  for (i= pi; i.hasItem(); i++)
  {
    if (i.getItem().length() != cs.length())
      continue;
    if (same (cs, i.getItem()))
      return 1;
  }
  return 0;
}

// is PS a subset of Cset ?
static bool
subset (const CFList &PS, const CFList& Cset)
{
  for (CFListIterator i= PS; i.hasItem(); i++)
  {
    if (!member (i.getItem(), Cset))
      return 0;
  }
  return 1;
}

// Union of two List of Lists
static ListCFList
MyUnion (const ListCFList& a, const ListCFList& b)
{
  if (a.isEmpty())
    return b;
  if (b.isEmpty())
    return a;

  ListCFList output;
  ListCFListIterator i;
  CFList elem;

  for (i= a; i.hasItem(); i++)
  {
    elem= i.getItem();
    if ((!elem.isEmpty()) && (!member (elem, output)))
      output.append(elem);
  }

  for (i= b; i.hasItem(); i++)
  {
    elem= i.getItem();
    if ((!elem.isEmpty()) && (!member (elem, output)))
      output.append(elem);
  }
  return output;
}

// Union of a and b stored in b
void
MyUnion2 (const ListCFList& a, ListCFList& b)
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
    if ((!elem.isEmpty()) && (!member (elem, b)))
      b.insert(elem);
  }
}

//if list b is member of the list of lists remove b and return the rest
static ListCFList
MyDifference (const ListCFList& a, const CFList& b)
{
  ListCFList output;
  ListCFListIterator i;
  CFList elem;

  for (i= a; i.hasItem(); i++)
  {
    elem= i.getItem();
    if ((!elem.isEmpty()) && (!same (elem, b)))
      output.append (elem);
  }
  return output;
}

// remove all elements of b from list of lists a and return the rest
static ListCFList
Minus( const ListCFList& a, const ListCFList& b)
{
  ListCFList output= a;

  for (ListCFListIterator i=b; i.hasItem(); i++)
    output = MyDifference (output, i.getItem());

  return output;
}

static ListCFList
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

  qhi= MyDifference (qh, qs);
  length= qhi.length();

  for (i= iscopy; i.hasItem(); i++)
  {
    itt= Union (qs, CFList (i.getItem()));
    ind= 0;
    if (length > 0)
    {
      for (j= qhi; j.hasItem(); j++)
      {
        if (subset (j.getItem(), itt))
          ind= 1;
      }
    }
    if (ind == 0)
      iss.append (itt);
  }
  return iss;
}

static ListCFList
adjoinb (const CFList & is, const CFList & qs, const ListCFList & qh ,const CFList & cs)
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
  qhi= MyDifference (qh, qs);
  length= qhi.length();
  for (i= iscopy; i.hasItem(); i++)
  {
    itt= Union (Union (qs, CFList (i.getItem())), cs);
    ind= 0;
    if (length > 0)
    {
      for (j= qhi; j.hasItem(); j++)
      {
        if (subset (j.getItem(), itt))
          ind= 1;
      }
    }
    if (ind == 0)
      iss.append(itt);
  }
  return iss;
}

static void
select (const ListCFList& ppi, int length, ListCFList& ppi1, ListCFList& ppi2)
{
  CFList elem;
  for (ListCFListIterator i=ppi; i.hasItem(); i++)
  {
    elem = i.getItem();
    if (!elem.isEmpty())
    {
      if (length <= elem.length())
        ppi2.append(elem);
      else
        ppi1.append(elem);
    }
  }
}

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
    G= F/lc(F);
    G *= bCommonDen (G);
    if (!isRat)
      Off (SW_RATIONAL);
    return G;
  }

  return F/lc (F);
}

static
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

      t= power (v, degF - degG)*g*lv;

      if (degF == 0)
        f= 0;
      else
        f= f - LC (f)*power (v, degF);

      f= lu*f - t;
      degF= degree (f, v);
    }

    if (reord)
      retvalue= swapvar (f, vg, v);
    else
      retvalue= f;

    return retvalue;
  }
}

static
CanonicalForm
Prem( const CanonicalForm &f, const CFList &L)
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


CFList
factorsOfInitials(const CFList & L)
{
  CFList result;
  CFFList factors;
  CanonicalForm tmp;

  for (CFListIterator i= L; i.hasItem(); i++)
  {
    factors= factorize (LC(i.getItem()));
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

    return;
  }

  cF= content (F);

  if (cF.inCoeffDomain())
    cF= 0;
  else
    cF= normalize (cF);
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

  for (j= StoredFactors.FS1; j.hasItem(); j++)
  {
    while (fdivides (j.getItem(), r, quot))
    {
      if (!quot.inCoeffDomain())
        r= quot;
      else
        break;
    }
  }

  // remove already removed factors
  for (j= StoredFactors.FS2; j.hasItem(); j++)
  {
    divides= false;
    while (fdivides (j.getItem(), r, quot))
    {
      if (!quot.inCoeffDomain())
      {
        divides= true;
        r= quot;
      }
      else
        break;
    }
    if (divides)
      removedFactors= Union (removedFactors, CFList (j.getItem()));
  }
  r= normalize (r);

  // remove variables
  for (j= testlist; j.hasItem() && !r.isOne(); j++)
  {
    while (fdivides (j.getItem(), r, quot))
    {
      if (!quot.inCoeffDomain())
        r= quot;
      else
        break;
      removedFactors= Union (removedFactors, CFList (j.getItem()));
    }
  }
}


static bool
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

static ListCFList
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
    if (!member (iitem, mem))
    {
      j= i;
      j++;
      for (; j.hasItem(); j++)
      {
        jitem= j.getItem();
        if (!member (jitem, mem))
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
  return Minus (cs,ts);
}


CFList
basicSet (const CFList &PS)
{
  CFList QS= PS, BS, RS;
  CanonicalForm b;
  int cb, degb;

  if (PS.length() < 2)
    return PS;

  CFListIterator i;

  while (!QS.isEmpty())
  {
    b= lowestRank (QS);
    cb= b.level();

    BS= Union(CFList (b), BS);

    if (cb <= 0)
      return CFList();
    else
    {
      degb= degree (b);
      RS= CFList();
      for (i= QS; i.hasItem(); i++)
      {
        if (degree (i.getItem(), cb) < degb)
          RS= Union (CFList (i.getItem()), RS);
      }
      QS= RS;
    }
  }

  return BS;
}

CFList
charSet (const CFList &PS)
{
  CFList QS= PS, RS= PS, CSet, tmp;
  CFListIterator i;
  CanonicalForm r;

  while (!RS.isEmpty())
  {
    CSet= basicSet (QS);

    RS= CFList();
    if (CSet.length() > 0 && CSet.getFirst().level() > 0)
    {
      tmp= Difference (QS, CSet);
      for (i= tmp; i.hasItem(); i++)
      {
        r= Prem (i.getItem(), CSet);
        if (r != 0)
          RS= Union (RS, CFList (r));
      }
      QS= Union (QS, RS);
    }
  }

  return CSet;
}

/// medial set
CFList
charSetN (const CFList &PS)
{
  CFList QS= PS, RS= PS, CSet, tmp;
  CFListIterator i;
  CanonicalForm r;

  while (!RS.isEmpty())
  {
    QS= uniGcd (QS);
    CSet= basicSet (QS);

    RS= CFList();
    if (CSet.length() > 0 && CSet.getFirst().level() > 0)
    {
      tmp= Difference (QS, CSet);
      for (i= tmp; i.hasItem(); i++)
      {
        r= Prem (i.getItem(), CSet);
        if (!r.isZero())
          RS= Union (RS, CFList (r));
      }
      QS= Union (CSet, RS);
    }
  }

  return CSet;
}

/// compute a characteristic set via medial set
CFList
charSetViaCharSetN (const CFList& PS)
{
  CFList result= charSetN (PS);
  if (result.isEmpty() || result.getFirst().inCoeffDomain())
    return CFList(1);
  CanonicalForm r;
  CFList RS;
  for (CFListIterator i= PS; i.hasItem(); i++)
  {
    r= Prem (i.getItem(), result);
    if (!r.isZero())
      RS= Union (RS, CFList (r));
  }
  if (RS.isEmpty())
    return result;
  return charSetViaCharSetN (Union (PS,Union (RS, result)));
}

/*CFList
pruneList (const CFList& L)
{
  printf ("L.length()= %d\n", L.length());
  if (L.length() <= 1)
    return L;
  CanonicalForm f, h, g;
  CFList gcds, result;
  CFListIterator i= L, j;

  int l= L.length();
  int n= 0;
  int m= 0;
  for (; (i.hasItem() && n < l); i++, n++)
  {
    f= i.getItem();
    for (j= L; (j.hasItem() && m < l); j++, m++)
    {
      h= j.getItem();
      g= gcd (f, h);
      if (!g.inCoeffDomain())
      {
        g= normalize (g);
        gcds= Union (gcds, CFList (g));
        f /= g;
        h /= g;
        out_cf ("g= ", g, "\n");
        out_cf ("h= ", h, "\n");
        out_cf ("f= ", f, "\n");
        if (!f.inCoeffDomain())
          result= Union (result, CFList (f));
        if (!h.inCoeffDomain())
          result= Union (result, CFList (h));
      }
    }
  }
  printf ("gcds= ");
  out_cflist (gcds);
  printf ("\n");
  printf ("result= ");
  out_cflist (result);
  printf ("\n");
  result= Union (result, gcds);
  return result;
}*/

/// modified medial set
CFList
modCharSet (const CFList& L, StoreFactors& StoredFactors)
{
  CFList QS= L, RS= L, CSet, tmp, contents, initial, removedFactors;
  CFListIterator i;
  CanonicalForm r, cF;
  bool noRemainder= true;
  StoreFactors StoredFactors2, StoredFactors3;

  QS= uniGcd (L);

  while (!RS.isEmpty())
  {
    noRemainder= true;
    CSet= basicSet (QS);

    initial= factorsOfInitials (CSet);

    StoredFactors2.FS1= StoredFactors.FS1;
    StoredFactors2.FS2= Union (StoredFactors2.FS2, initial);

    RS= CFList();

    if (CSet.length() > 0 && CSet.getFirst().level() > 0)
    {
      tmp= Difference (QS, CSet);

      for (i= tmp; i.hasItem(); i++)
      {
        r= Prem (i.getItem(), CSet);
        if (!r.isZero())
        {
          noRemainder= false;
          removeContent (r, cF);

          if (!cF.isZero())
            contents= Union (contents, factorPSet (CFList(cF))); //factorPSet maybe too much it should suffice to do a squarefree factorization instead

          removeFactors (r, StoredFactors2, removedFactors);
          StoredFactors2.FS1= Union (StoredFactors2.FS1, removedFactors);
          StoredFactors2.FS2= Difference (StoredFactors2.FS2, removedFactors);

          removedFactors= CFList();

          RS= Union (RS, CFList (r));
        }
      }

      if (!noRemainder)
      {
        StoredFactors.FS1= Union (StoredFactors2.FS1, contents);
        StoredFactors.FS2= StoredFactors2.FS2;
      }
      else
        StoredFactors= StoredFactors2;

      QS= Union (CSet, RS);

      contents= CFList();
      removedFactors= CFList();
    }
    else
      StoredFactors= StoredFactors2;
  }

  return CSet;
}

/// characteristic set via modified medial set
CFList
charSetViaModCharSet (const CFList& PS, StoreFactors& StoredFactors)
{
  CFList result= modCharSet (PS, StoredFactors);
  if (result.isEmpty() || result.getFirst().inCoeffDomain())
    return CFList(1);
  CanonicalForm r;
  CFList RS;
  CFList tmp= Difference (PS, result);
  for (CFListIterator i= tmp; i.hasItem(); i++)
  {
    r= Prem (i.getItem(), result);
    if (!r.isZero())
      RS= Union (RS, CFList (r));
  }
  if (RS.isEmpty())
    return result;

  return charSetViaModCharSet (Union (PS, Union (RS, result)), StoredFactors);
}

CFList
charSetViaModCharSet (const CFList& PS)
{
  StoreFactors tmp;
  return charSetViaModCharSet (PS, tmp);
}

CFList
modCharSet (const CFList& PS)
{
  StoreFactors tmp;
  return modCharSet (PS, tmp);
}

ListCFList
charSeries (const CFList& L)
{
  ListCFList tmp, result, tmp2, ppi1, ppi2, qqi, ppi, alreadyConsidered;
  CFList l, charset, ini;

  int count= 0;
  int highestLevel= 1;
  CFListIterator iter;

  StoreFactors StoredFactors;

  l= L;

  for (iter= l; iter.hasItem(); iter++)
  {
    iter.getItem()= normalize (iter.getItem());
    if (highestLevel < iter.getItem().level())
      highestLevel= iter.getItem().level();
  }

  tmp= ListCFList (l);

  while (!tmp.isEmpty())
  {
    sortListCFList (tmp);

    l= tmp.getFirst();

    tmp= MyDifference (tmp, l);

    select (ppi, l.length(), ppi1, ppi2);

    MyUnion2 (ppi2, qqi);

    if (count > 0)
      ppi= MyUnion (ListCFList (l), ppi1);
    else
      ppi= ListCFList();

    if (l.length() - 3 < highestLevel)
      charset= charSetViaModCharSet (l, StoredFactors);
    else
      charset= charSetViaCharSetN (l);

    if (charset.length() > 0 && charset.getFirst().level() > 0)
    {
      result= MyUnion (result, ListCFList (charset));
      ini= factorsOfInitials (charset);

      ini= Union (ini, factorPSet (StoredFactors.FS1));
      sortCFListByLevel (ini);
    }
    else
    {
      ini= factorPSet (StoredFactors.FS1);
      sortCFListByLevel (ini);
    }

    tmp2= adjoin (ini, l, qqi);
    tmp= MyUnion (tmp, tmp2);

    StoredFactors.FS1= CFList();
    StoredFactors.FS2= CFList();

    ppi1= ListCFList();
    ppi2= ListCFList();

    count++;
  }

  //TODO need to remove superflous components

  return result;
}

static bool
irreducible (const CFList & AS)
{
// AS is given by AS = { A1, A2, .. Ar }, d_i = degree(Ai)

// 1) we test: if d_i > 1, d_j =1 for all j<>i, then AS is irreducible.
  bool deg1= true;
  for (CFListIterator i= AS ; i.hasItem(); i++)
  {
    if (degree (i.getItem()) > 1)
    {
      if (deg1)
        deg1= false;
      else
        return false; // found 2nd poly with deg > 1
    }
  }
  return true;
}

static CFList
irredAS (CFList & AS, int & indexRed, CanonicalForm & reducible)
{
  CFFList qs;
  CFList ts, as;
  CanonicalForm elem;
  bool ind= true;
  int nr= 0, success= -1;
  CFListIterator i;

  indexRed= 0;
  for (i= AS; i.hasItem(); i++ )
  {
    nr += 1;
    if (degree(i.getItem()) > 1)
    {
      qs= factorize (i.getItem());
      if (qs.getFirst().factor().inCoeffDomain())
        qs.removeFirst();
    }
    else
      qs= CFFList (CFFactor(i.getItem(),1));

    if ((qs.length() >= 2 ) || (qs.getFirst().exp() > 1))
    {
      indexRed= nr;
      ind= false;
      reducible= i.getItem();
      break;
    }
  }

  if (ind)
  {
    if (irreducible (AS))  // as quasilinear? => irreducible!
      indexRed= 0;
    else
    {
      i= AS;
      for (nr= 1; nr< AS.length(); nr++)
      {
        as.append (i.getItem());
        i++;
        if (degree (i.getItem()) > 1)
        {  // search for a non linear elem
          qs= newfactoras (i.getItem(), as, success);
          if (qs.length() > 1 || qs.getFirst().exp() > 1)
          { //found elem is reducible
            reducible= i.getItem();
            indexRed= nr + 1;
            break;
          }
        }
      }
    }
  }
  for (CFFListIterator k= qs; k.hasItem(); k++)
    ts.append (k.getItem().factor());
  return ts;
}

ListCFList
irrCharSeries (const CFList & PS)
{
  CanonicalForm reducible, reducible2;
  CFList qs, cs, factorset, is, ts;
  ListCFList pi, ppi, qqi, qsi, iss, qhi= ListCFList(PS);

  StoreFactors StoredFactors;

  int nr_of_iteration= 0, indexRed, highestlevel= 0;

  for (CFListIterator iter= PS; iter.hasItem(); iter++)
  {
    if (level (iter.getItem()) > highestlevel)
      highestlevel= level(iter.getItem());
  }

  while (!qhi.isEmpty())
  {
    sortListCFList (qhi);

    qs= qhi.getFirst();

    ListCFList ppi1,ppi2;
    select (ppi, qs.length(), ppi1, ppi2);

    MyUnion2 (ppi2, qqi);

    if (nr_of_iteration == 0)
    {
      nr_of_iteration += 1;
      ppi= ListCFList();
    }
    else
    {
      nr_of_iteration += 1;
      ppi= MyUnion (ListCFList(qs), ppi1);
    }

    cs= charSetViaModCharSet (qs, StoredFactors);

    //cs = removeContent (cs, StoredFactors); //do I really need it
    factorset= StoredFactors.FS2;

    if (cs.getFirst().level() > 0)
    {
      ts= irredAS (cs, indexRed, reducible);

      if (indexRed <= 0) // irreducible
      {
        if (!subset (cs,qs))
          cs= charSetViaCharSetN (Union (qs,cs));
        if (!member (cs, pi))
        {
          pi= MyUnion (pi, ListCFList (cs));
          if (cs.getFirst().level() > 0)
          {
            ts= irredAS (cs, indexRed, reducible);

            if (indexRed <= 0) //irreducible
            {
              qsi= MyUnion (qsi, ListCFList(cs));
              if (cs.length() == highestlevel)
                is= factorPSet (factorset);
              else
                is= Union (factorsOfInitials (cs), factorPSet (factorset));
              iss= adjoin (is, qs, qqi);
            }
          }
          else
            iss= adjoin (factorPSet (factorset), qs, qqi);
        }
        else
          iss= adjoin (factorPSet (factorset), qs, qqi);
      }

      if (indexRed > 0)
      {
        is= factorPSet (factorset);
        if (indexRed > 1)
        {
          CFList cst;
          for (CFListIterator i=cs ; i.hasItem(); i++)
          {
            if (i.getItem() == reducible)
              break;
            else 
              cst.append(i.getItem());
          }
          is= Union (factorsOfInitials (cst), is);
          iss= MyUnion (adjoin (is, qs, qqi), adjoinb (ts, qs, qqi, cst));
        }
        else
          iss= adjoin (Union (is, ts), qs, qqi);
      }
    }
    else
      iss= adjoin (factorPSet (factorset), qs, qqi);
    if (qhi.length() > 1)
    {
      qhi.removeFirst();
      qhi= MyUnion (qhi, iss);
    }
    else
      qhi= iss;
  }
  if (!qsi.isEmpty())
    return contract (qsi);
  return ListCFList() ;
}


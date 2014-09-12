/*****************************************************************************\
 * Computer Algebra System SINGULAR
\*****************************************************************************/
/** @file cfCharSets.cc
 *
 * This file provides functions to compute characteristic sets
 *
 * @note some of the code is code from libfac or derived from code from libfac.
 * Libfac is written by M. Messollen. See also COPYING for license information
 * and README for general information on characteristic sets.
 *
 * ABSTRACT: Descriptions can be found in Wang "On the Parallelization of
 * characteristic-set based algorithms" or Greuel/Pfister "A Singular
 * Introduction to Commutative Algebra".
 *
 * @authors Martin Lee
 *
 **/
/*****************************************************************************/

#include "config.h"

#include "timing.h"

#include "canonicalform.h"
#include "cfCharSets.h"
#include "cfCharSetsUtil.h"
#include "cf_algorithm.h"
#include "facAlgFunc.h"

TIMING_DEFINE_PRINT(neworder_time)

// set up a new orderd list of Variables.
// we try to reorder the variables heuristically optimal.
Varlist
neworder (const CFList & PolyList)
{
  CFList PS= PolyList, PS1=PolyList;
  Varlist oldorder, reorder, difference;

  TIMING_START (neworder_time);

  int highest_level= level (get_max_var (PS));

  // set up oldorder and first criterion: only_in_one
  for (int i= highest_level; i>=1; i--)
  {
    oldorder.insert (Variable (i));
    CFList is_one= only_in_one (PS1, Variable (i)); 
    if (is_one.length() == 1)
    {
      reorder.insert (Variable (i));
      PS1= Difference (PS1, is_one);
    }
    else if (is_one.length() == 0)
    {
      reorder.append (Variable (i)); // assigne it the highest level
      PS1= Difference (PS1, is_one); 
    }
  }
  difference= Difference (oldorder, reorder);

  // rearrange the ordering of the variables!
  difference= reorderb (difference, PS, highest_level);
  reorder= Union (reorder, difference);
  TIMING_END(neworder_time);

  TIMING_PRINT(neworder_time, "\ntime used for neworder   : ");

  return Union (reorder, Difference (oldorder, reorder));
}

// the same as above, only returning a list of CanonicalForms
CFList
newordercf (const CFList & PolyList)
{
  Varlist reorder= neworder (PolyList);
  CFList output;

  for (VarlistIterator i=reorder; i.hasItem(); i++)
    output.append (CanonicalForm (i.getItem()));

  return output;
}

// the same as above, only returning a list of ints
IntList
neworderint (const CFList & PolyList)
{
  Varlist reorder= neworder (PolyList);
  IntList output;

  for (VarlistIterator i= reorder; i.hasItem(); i++)
    output.append (level (i.getItem()));

  return output;
}

// a library function: we reorganize the global variable ordering
CFList
reorder (const Varlist & betterorder, const CFList & PS)
{
  int i= 1, n= betterorder.length();
  Intarray v (1, n);
  CFList ps= PS;

  //initalize:
  for (VarlistIterator j= betterorder; j.hasItem(); j++)
  {
    v[i]= level (j.getItem());
    i++;
  }
  // reorder:
  for (i= 1; i <= n; i++)
    ps= swapvar (ps, Variable (v[i]), Variable (n + i));
  return ps;
}

CFFList
reorder (const Varlist & betterorder, const CFFList & PS)
{
  int i= 1, n= betterorder.length();
  Intarray v (1, n);
  CFFList ps= PS;

  //initalize:
  for (VarlistIterator j= betterorder; j.hasItem(); j++)
  {
    v[i]= level (j.getItem());
    i++;
  }

  // reorder:
  for (i= 1; i <= n; i++)
    ps= swapvar (ps, Variable (v[i]), Variable (n + i));
  return ps;
}

ListCFList
reorder (const Varlist & betterorder, const ListCFList & Q)
{
  ListCFList Q1;

  for (ListCFListIterator i= Q; i.hasItem(); i++)
    Q1.append (reorder (betterorder, i.getItem()));
  return Q1;
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
  CFList L;
  CFFList sqrfFactors;
  CanonicalForm sqrf;
  CFFListIterator iter2;
  for (CFListIterator iter= PS; iter.hasItem(); iter++)
  {
    sqrf= 1;
    sqrfFactors= sqrFree (iter.getItem());
    for (iter2= sqrfFactors; iter2.hasItem(); iter2++)
      sqrf *= iter2.getItem().factor();
    L= Union (L, CFList (normalize (sqrf)));
  }

  CFList result= charSetN (L);

  if (result.isEmpty() || result.getFirst().inCoeffDomain())
    return CFList(1);

  CanonicalForm r;
  CFList RS;
  CFList tmp= Difference (L, result);

  for (CFListIterator i= tmp; i.hasItem(); i++)
  {
    r= Premb (i.getItem(), result);
    if (!r.isZero())
      RS= Union (RS, CFList (r));
  }
  if (RS.isEmpty())
    return result;

  return charSetViaCharSetN (Union (L, Union (RS, result)));
}

/// modified medial set
CFList
modCharSet (const CFList& L, StoreFactors& StoredFactors, bool removeContents)
{
  CFList QS, RS= L, CSet, tmp, contents, initial, removedFactors;
  CFListIterator i;
  CanonicalForm r, cF;
  bool noRemainder= true;
  StoreFactors StoredFactors2;

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
          if (removeContents)
          {
            removeContent (r, cF);

            if (!cF.isZero())
              contents= Union (contents, factorPSet (CFList(cF))); //factorPSet maybe too much it should suffice to do a squarefree factorization instead
          }

          removeFactors (r, StoredFactors2, removedFactors);
          StoredFactors2.FS1= Union (StoredFactors2.FS1, removedFactors);
          StoredFactors2.FS2= Difference (StoredFactors2.FS2, removedFactors);

          removedFactors= CFList();

          RS= Union (RS, CFList (r));
        }
      }

      if (removeContents && !noRemainder)
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
charSetViaModCharSet (const CFList& PS, StoreFactors& StoredFactors,
                      bool removeContents)
{
  CFList L;
  CFFList sqrfFactors;
  CanonicalForm sqrf;
  CFFListIterator iter2;
  for (CFListIterator iter= PS; iter.hasItem(); iter++)
  {
    sqrf= 1;
    sqrfFactors= sqrFree (iter.getItem());
    for (iter2= sqrfFactors; iter2.hasItem(); iter2++)
      sqrf *= iter2.getItem().factor();
    L= Union (L, CFList (normalize (sqrf)));
  }

  L= uniGcd (L);

  CFList result= modCharSet (L, StoredFactors, removeContents);

  if (result.isEmpty() || result.getFirst().inCoeffDomain())
    return CFList(1);

  CanonicalForm r;
  CFList RS;
  CFList tmp= Difference (L, result);

  for (CFListIterator i= tmp; i.hasItem(); i++)
  {
    r= Premb (i.getItem(), result);
    if (!r.isZero())
      RS= Union (RS, CFList (r));
  }
  if (RS.isEmpty())
    return result;

  return charSetViaModCharSet (Union (L, Union (RS, result)), StoredFactors,
                               removeContents);
}

CFList
charSetViaModCharSet (const CFList& PS, bool removeContents)
{
  StoreFactors tmp;
  return charSetViaModCharSet (PS, tmp, removeContents);
}

CFList
modCharSet (const CFList& PS, bool removeContents)
{
  StoreFactors tmp;
  return modCharSet (PS, tmp, removeContents);
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

    tmp= Difference (tmp, l);

    select (ppi, l.length(), ppi1, ppi2);

    inplaceUnion (ppi2, qqi);

    if (count > 0)
      ppi= Union (ppi1, ListCFList (l));
    else
      ppi= ListCFList();

    if (l.length() - 3 < highestLevel)
      charset= charSetViaModCharSet (l, StoredFactors);
    else
      charset= charSetViaCharSetN (l);

    if (charset.length() > 0 && charset.getFirst().level() > 0)
    {
      result= Union (ListCFList (charset), result);
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
    tmp= Union (tmp2, tmp);

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
  int nr= 0;
  CFListIterator i;

  indexRed= 0;
  for (i= AS; i.hasItem(); i++ )
  {
    nr += 1;
    if (degree (i.getItem()) > 1)
    {
      qs= factorize (i.getItem());
      if (qs.getFirst().factor().inCoeffDomain())
        qs.removeFirst();
    }
    else
      qs= CFFList (CFFactor (normalize (i.getItem()), 1));

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
          qs= facAlgFunc2 (i.getItem(), as);
          if (qs.getFirst().factor().inCoeffDomain())
            qs.removeFirst();
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
    ts.append (normalize (k.getItem().factor()));
  return ts;
}

ListCFList
irrCharSeries (const CFList & PS)
{
  CanonicalForm reducible, reducible2;
  CFList qs, cs, factorset, is, ts, L;
  CanonicalForm sqrf;
  CFFList sqrfFactors;
  CFFListIterator iter2;
  for (CFListIterator iter= PS; iter.hasItem(); iter++)
  {
    sqrf= 1;
    sqrfFactors= sqrFree (iter.getItem());
    if (sqrfFactors.getFirst().factor().inCoeffDomain())
      sqrfFactors.removeFirst();
    for (iter2= sqrfFactors; iter2.hasItem(); iter2++)
      sqrf *= iter2.getItem().factor();
    sqrf= normalize (sqrf);
    L= Union (CFList (sqrf), L);
  }

  ListCFList pi, ppi, qqi, qsi, iss, qhi= ListCFList(L);

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

    inplaceUnion (ppi2, qqi);

    if (nr_of_iteration == 0)
    {
      nr_of_iteration += 1;
      ppi= ListCFList();
    }
    else
    {
      nr_of_iteration += 1;
      ppi= Union (ppi1, ListCFList (qs));
    }

    StoreFactors StoredFactors;
    if (qs.length() - 3 < highestlevel)
      cs= modCharSet (qs, StoredFactors, false);
    else
      cs= charSetN (qs);
    cs= removeContent (cs, StoredFactors);

    factorset= StoredFactors.FS1;

    if (!cs.isEmpty() && cs.getFirst().level() > 0)
    {
      ts= irredAS (cs, indexRed, reducible);

      if (indexRed <= 0) // irreducible
      {
        if (!isSubset (cs,qs))
          cs= charSetViaCharSetN (Union (qs,cs));
        if (!find (pi, cs))
        {
          pi= Union (ListCFList (cs), pi);
          if (cs.getFirst().level() > 0)
          {
            ts= irredAS (cs, indexRed, reducible);

            if (indexRed <= 0) //irreducible
            {
              qsi= Union (ListCFList(cs), qsi);
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
          for (CFListIterator i= cs ; i.hasItem(); i++)
          {
            if (i.getItem() == reducible)
              break;
            else 
              cst.append (i.getItem());
          }
          is= Union (factorsOfInitials (cst), is);
          iss= Union (adjoinb (ts, qs, qqi, cst), adjoin (is, qs, qqi));
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
      qhi= Union (iss, qhi);
    }
    else
      qhi= iss;
  }
  if (!qsi.isEmpty())
    return contract (qsi);
  return ListCFList(CFList (1)) ;
}


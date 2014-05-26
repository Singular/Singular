/*****************************************************************************\
 * Computer Algebra System SINGULAR
\*****************************************************************************/
/** @file cfCharSetsUtil.h
 *
 * This file provides utility functions to compute characteristic sets
 *
 * @note some of the code is code from libfac or derived from code from libfac.
 * Libfac is written by M. Messollen. See also COPYING for license information
 * and README for general information on characteristic sets.
 *
 * ABSTRACT: Descriptions can be found in Wang "On the Parallelization of
 * characteristic-set based algorithms" or Greuel/Pfister "A Singular
 * Introduction to Commutative Algebra".
 *
 * @author Martin Lee
 *
 **/
/*****************************************************************************/

#ifndef CF_CHARSETS_UTIL
#define CF_CHARSETS_UTIL

/*BEGINPUBLIC*/
class StoreFactors
{
  public:
    CFList FS1;
    CFList FS2;
    inline StoreFactors& operator= (const StoreFactors& value)
    {
      if ( this != &value )
      {
        FS1 = value.FS1;
        FS2 = value.FS2;
      }
      return *this;
    }
};
/*ENDPUBLIC*/

Variable
get_max_var (const CFList & PS);

CFList
only_in_one (const CFList & PS, const Variable & x);

Varlist
reorderb (const Varlist & difference, const CFList & PS,
          const int highest_level);

CFList
swapvar (const CFList & PS, const Variable & x, const Variable & y);

CFFList
swapvar (const CFFList & PS, const Variable & x, const Variable & y);

CanonicalForm
lowestRank (const CFList & L);

CFList initials (const CFList& L);

void
sortListCFList (ListCFList& list);

void
sortCFListByLevel (CFList& list);

CanonicalForm
Prem (const CanonicalForm& F, const CanonicalForm& G);

CanonicalForm
Premb (const CanonicalForm &f, const CFList &L);

CanonicalForm
Prem (const CanonicalForm &f, const CFList &L);

CFList uniGcd (const CFList& L);

CFList
factorsOfInitials(const CFList & L);

void
removeContent (CanonicalForm& F, CanonicalForm& cF);

CFList
factorPSet (const CFList& PS);

void
removeFactors (CanonicalForm& r, StoreFactors& StoredFactors,
               CFList& removedFactors);

CFList
removeContent (const CFList & PS, StoreFactors & StoredFactors);

ListCFList
contract (const ListCFList& cs);

bool
isMember (const CFList& cs, const ListCFList& pi);

bool
isSubset (const CFList &PS, const CFList& Cset);

ListCFList
minus (const ListCFList& a, const CFList& b);

ListCFList
adjoin (const CFList& is, const CFList& qs, const ListCFList& qh);

ListCFList
adjoinb (const CFList & is, const CFList & qs, const ListCFList & qh,
         const CFList & cs);

ListCFList
MyUnion (const ListCFList& a, const ListCFList& b);

void
inplaceUnion (const ListCFList& a, ListCFList& b);

void
select (const ListCFList& ppi, int length, ListCFList& ppi1, ListCFList& ppi2);

CanonicalForm normalize (const CanonicalForm& F);

#endif

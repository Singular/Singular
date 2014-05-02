/*****************************************************************************\
 * Computer Algebra System SINGULAR
\*****************************************************************************/
/** @file cfCharSets.h
 *
 * This file provides functions to compute characteristic sets
 * 
 * ABSTRACT: Descriptions can be found in Wang "On the Parallelization of
 * characteristic-set based algorithms" or Greuel/Pfister "A Singular
 * Introduction to Commutative Algebra".
 *
 * @author Martin Lee
 *
 **/
/*****************************************************************************/

#ifndef CF_CHARSETS
#define CF_CHARSETS

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif /* HAVE_CONFIG_H */

#include "canonicalform.h"

CanonicalForm normalize (const CanonicalForm& F);

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

/// basic set in the sense of Wang a.k.a. minimal ascending set in the sense of
/// Greuel/Pfister
CFList
basicSet (const CFList& PS);

/// characteristic set
CFList
charSet (const CFList& PS);

/// modified medial set
CFList
modCharSet (const CFList& PS, StoreFactors& StoredFactors);

/// wrapper for modCharSet
CFList
modCharSet (const CFList& PS);

/// modified characteristic set, i.e. a characteristic set with certain
/// factors removed
CFList
charSetViaModCharSet (const CFList& PS);

/// characteristic series
ListCFList
charSeries (const CFList& L);

/// irreducible characteristic series
ListCFList
irrCharSeries (const CFList & PS);

// the next three give you a heuristically optimal reorderd list of the
// variables. For internal and external (e.g. Singular/Macaulay2) library use.
// This is really experimental!
// See the comments in reorder.cc.
// 
// this gives you a heuristically optimal ordering for the ring variables
// if you use the irreducible characteristic series.
Varlist  neworder( const CFList & PolyList );
// the same as neworder(...) only returning a list of CanonicalForm 's
// (i.e. the variables as CanonicalForms)
CFList   newordercf(const CFList & PolyList );
// the same as neworder(...) only returning a list of int 's (i.e. the levels)
IntList  neworderint(const CFList & PolyList );

// for library internal use only:
// next function reorders the variables in PS:
// a code segment to use:
// ...
// #include <tmpl_inst.h> // for typedef's
// CFList PS= <setup-your-list-of-CanonicalForms>;
// Varlist betterorder= neworder(PS);
// PS= reorder(betterorder,PS); // reorder variables in PS from oldorder 
//                                 to betterorder
// ListCFList Q= IrrCharSeries( PS );
// Q= reorder(betterorder,Q);   // revert ordering to oldorder
// 
CFList reorder( const Varlist & betterorder, const CFList & PS);
CFFList reorder( const Varlist & betterorder, const CFFList & PS);
ListCFList reorder(const Varlist & betterorder, const ListCFList & Q);
/*ENDPUBLIC*/

#endif

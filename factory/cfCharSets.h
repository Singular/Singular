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

/*ENDPUBLIC*/

#endif

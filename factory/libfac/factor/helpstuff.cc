////////////////////////////////////////////////////////////
// emacs edit mode for this file is -*- C++ -*-
////////////////////////////////////////////////////////////
// FACTORY - Includes
#include <factory.h>
// Factor - Includes
#include "tmpl_inst.h"
// some CC's need this:
#include "helpstuff.h"

bool
mydivremt ( const CanonicalForm& f, const CanonicalForm& g, CanonicalForm& a, CanonicalForm& b )
{
  bool retvalue;
  CanonicalForm aa,bb;
  retvalue = divremt(f,g,a,bb);
  aa= f-g*a;
  if ( aa==bb ) { b=bb; }
  else { b=aa; }
  return retvalue;
}

// Now some procedures used in SqrFree and in Factor
///////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////
// We have to include a version of <CFFList>.append(CFFactor)//
// and Union( CFFList, CFFList)                              //
// because we have to look for multiplicities in SqrFree.    //
// e.g.: SqrFree( f^3 ) with char <> 3                       //
///////////////////////////////////////////////////////////////
CFFList
myappend( const CFFList & Inputlist, const CFFactor & TheFactor)
{
  CFFList Outputlist ;
  CFFactor copy;
  CFFListIterator i;
  int exp=0;

  for ( i=Inputlist ; i.hasItem() ; i++ )
  {
    copy = i.getItem();
    if ( copy.factor() == TheFactor.factor() )
      exp += copy.exp();
    else
      Outputlist.append(copy);
  }
  Outputlist.append( CFFactor(TheFactor.factor(), exp + TheFactor.exp()));
  return Outputlist;
}

CFFList
myUnion(const CFFList & Inputlist1,const CFFList & Inputlist2)
{
  CFFList Outputlist;
  CFFListIterator i;

  for ( i=Inputlist1 ; i.hasItem() ; i++ )
    Outputlist = myappend(Outputlist, i.getItem() );
  for ( i=Inputlist2 ; i.hasItem() ; i++ )
    Outputlist = myappend(Outputlist, i.getItem() );

  return Outputlist;
}


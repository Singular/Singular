////////////////////////////////////////////////////////////
// emacs edit mode for this file is -*- C++ -*-
// $Id$
////////////////////////////////////////////////////////////
#ifndef HELPSTUFF_H
#define HELPSTUFF_H
bool mydivremt ( const CanonicalForm&, const CanonicalForm&, CanonicalForm&, CanonicalForm& );
///////////////////////////////////////////////////////////////
// Now some procedures used in MVMultiHensel and in Truefactors
///////////////////////////////////////////////////////////////
CanonicalForm mod_power( const CanonicalForm & f, int k, int levelU);
int subvardegree( const CanonicalForm & F, int levelF );
CanonicalForm change_poly( const CanonicalForm & f , const SFormList & Substitutionlist, int directionback );

///////////////////////////////////////////////////////////////
// Now some procedures used in SqrFree and in Factor
///////////////////////////////////////////////////////////////
CFFList myappend( const CFFList & Inputlist, const CFFactor & TheFactor) ;
CFFList myUnion(const CFFList & Inputlist1,const CFFList & Inputlist2);
int Powerup( const int base , const int exp=1);
inline int min ( const int a, const int b ){
  return (a<=b ? a:b);
}
inline int max ( const int a, const int b ){
  return (a>b ? a:b);
}

#endif /* HELPSTUFF_H */


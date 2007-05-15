/** Copyright 1996 Michael Messollen. All rights reserved. */
////////////////////////////////////////////////////////////
// emacs edit mode for this file is -*- C++ -*-
// $Id: helpstuff.h,v 1.4 2007-05-15 14:46:49 Singular Exp $
////////////////////////////////////////////////////////////
#ifndef HELPSTUFF_H
#define HELPSTUFF_H
bool mydivremt ( const CanonicalForm&, const CanonicalForm&, CanonicalForm&, CanonicalForm& );
void mydivrem( const CanonicalForm& f, const CanonicalForm& g, CanonicalForm& a, CanonicalForm& b );
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

///////////////////////////////////////////////////////////////////////////////
/*
$Log: not supported by cvs2svn $
Revision 1.3  1997/09/12 07:19:57  Singular
* hannes/michael: libfac-0.3.0

Revision 1.2  1997/04/25 22:23:49  michael
Version for libfac-0.2.1

*/

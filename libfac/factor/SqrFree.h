////////////////////////////////////////////////////////////
// emacs edit mode for this file is -*- C++ -*-
// $Id: SqrFree.h,v 1.8 2008-06-10 14:49:15 Singular Exp $
///////////////////////////////////////////////////////////////////////////////
#ifndef SQRFREE_H
#define SQRFREE_H

/*BEGINPUBLIC*/
// CFFList SqrFree( const CanonicalForm & f ) ;
CFFList SqrFreeMV( const CanonicalForm & f , const CanonicalForm & mipo=0) ;
/*ENDPUBLIC*/
int     SqrFreeTest( const CanonicalForm & r, int opt=1) ;
CFFList SqrFree( const CanonicalForm & f ) ;
#endif /* SQRFREE_H */

////////////////////////////////////////////////////////////
/*
$Log: not supported by cvs2svn $
Revision 1.7  2008/03/17 17:44:16  Singular
*hannes: fact.tst

Revision 1.4  2002/08/19 11:11:34  Singular
* hannes/pfister: alg_gcd etc.

Revision 1.3  1997/09/12 07:19:51  Singular
* hannes/michael: libfac-0.3.0

Revision 1.2  1997/04/25 22:13:03  michael
Version for libfac-0.2.1

*/

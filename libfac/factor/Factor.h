////////////////////////////////////////////////////////////
// emacs edit mode for this file is -*- C++ -*-
// $Id: Factor.h,v 1.7 2008-06-10 14:49:15 Singular Exp $
///////////////////////////////////////////////////////////////////////////////
#ifndef FACTOR_H
#define FACTOR_H
CanonicalForm generate_mipo( int degree_of_Extension , const Variable & Extension );
CFFList Factorized( const CanonicalForm & F, const CanonicalForm & mipo, int Mainvar=0);
/*BEGINPUBLIC*/
CFFList Factorize( const CanonicalForm & F, int is_SqrFree=0 ) ;
CFFList Factorize( const CanonicalForm & F, const CanonicalForm & mi, int is_SqrFree=0 ) ;
CFFList Factorize2(CanonicalForm F, const CanonicalForm & minpoly );
/*ENDPUBLIC*/

#endif /* FACTOR_H */

////////////////////////////////////////////////////////////
/*
$Log: not supported by cvs2svn $
Revision 1.6  2008/05/31 17:20:10  Singular
hannes: minor irras changes

Revision 1.5  2002/08/19 11:11:33  Singular
* hannes/pfister: alg_gcd etc.

Revision 1.4  2002/07/30 15:05:38  Singular
*hannes: added Factorize for alg. ext.

Revision 1.3  1997/09/12 07:19:47  Singular
* hannes/michael: libfac-0.3.0

Revision 1.3  1997/04/25 22:11:35  michael
version for libfac-0.2.1

*/

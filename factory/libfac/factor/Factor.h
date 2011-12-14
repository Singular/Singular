////////////////////////////////////////////////////////////
// emacs edit mode for this file is -*- C++ -*-
// $Id$
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


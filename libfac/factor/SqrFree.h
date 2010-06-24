////////////////////////////////////////////////////////////
// emacs edit mode for this file is -*- C++ -*-
// $Id$
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

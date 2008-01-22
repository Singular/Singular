/* Copyright 1996 Michael Messollen. All rights reserved. */
////////////////////////////////////////////////////////////
// emacs edit mode for this file is -*- C++ -*-
// $Id: SqrFree.h,v 1.5 2008-01-22 09:51:37 Singular Exp $
///////////////////////////////////////////////////////////////////////////////
#ifndef SQRFREE_H
#define SQRFREE_H

/*BEGINPUBLIC*/
// CFFList SqrFree( const CanonicalForm & f ) ;
/*ENDPUBLIC*/
//CFFList InternalSqrFree( const CanonicalForm & f , const CanonicalForm & mipo=0) ;
int     SqrFreeTest( const CanonicalForm & r, int opt=1) ;
CFFList SqrFree( const CanonicalForm & f ) ;
#endif /* SQRFREE_H */

////////////////////////////////////////////////////////////
/*
$Log: not supported by cvs2svn $
Revision 1.4  2002/08/19 11:11:34  Singular
* hannes/pfister: alg_gcd etc.

Revision 1.3  1997/09/12 07:19:51  Singular
* hannes/michael: libfac-0.3.0

Revision 1.2  1997/04/25 22:13:03  michael
Version for libfac-0.2.1

*/

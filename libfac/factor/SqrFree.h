/* Copyright 1996 Michael Messollen. All rights reserved. */
////////////////////////////////////////////////////////////
// emacs edit mode for this file is -*- C++ -*-
// $Id: SqrFree.h,v 1.3 1997-09-12 07:19:51 Singular Exp $
///////////////////////////////////////////////////////////////////////////////
#ifndef SQRFREE_H
#define SQRFREE_H

/*BEGINPUBLIC*/
// CFFList SqrFree( const CanonicalForm & f ) ;
/*ENDPUBLIC*/
CFFList InternalSqrFree( const CanonicalForm & f ) ;
int     SqrFreeTest( const CanonicalForm & r, int opt=1) ;
CFFList SqrFree( const CanonicalForm & f ) ;
#endif /* SQRFREE_H */

////////////////////////////////////////////////////////////
/*
$Log: not supported by cvs2svn $
Revision 1.2  1997/04/25 22:13:03  michael
Version for libfac-0.2.1

*/

/* Copyright 1996 Michael Messollen. All rights reserved. */
////////////////////////////////////////////////////////////
// emacs edit mode for this file is -*- C++ -*-
// $Id: SqrFree.h,v 1.1.1.1 1997-05-02 17:00:46 Singular Exp $
///////////////////////////////////////////////////////////////////////////////
#ifndef SQRFREE_H
#define SQRFREE_H

/*BEGINPUBLIC*/
// int SqrFreeTest( const CanonicalForm & r, int opt=1) ;
// CFFList SqrFree( const CanonicalForm & f ) ;
CFFList InternalSqrFree( const CanonicalForm & f ) ;
/*ENDPUBLIC*/
int SqrFreeTest( const CanonicalForm & r, int opt=1) ;
CFFList SqrFree( const CanonicalForm & f ) ;
#endif /* SQRFREE_H */

////////////////////////////////////////////////////////////
/*
$Log: not supported by cvs2svn $
Revision 1.2  1997/04/25 22:13:03  michael
Version for libfac-0.2.1

*/

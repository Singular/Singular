/* Copyright 1996 Michael Messollen. All rights reserved. */
////////////////////////////////////////////////////////////
// emacs edit mode for this file is -*- C++ -*-
// $Id: Factor.h,v 1.1.1.1 1997-05-02 17:00:46 Singular Exp $
///////////////////////////////////////////////////////////////////////////////
#ifndef FACTOR_H
#define FACTOR_H
CFFList Factorized( const CanonicalForm & F, const Variable & alpha, int Mainvar=0);
/*BEGINPUBLIC*/
CFFList Factorize( const CanonicalForm & F, int is_SqrFree=0 ) ;
/*ENDPUBLIC*/

#endif /* FACTOR_H */

////////////////////////////////////////////////////////////
/*
$Log: not supported by cvs2svn $
Revision 1.3  1997/04/25 22:11:35  michael
version for libfac-0.2.1

*/

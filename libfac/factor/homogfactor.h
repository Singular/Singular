/* Copyright 1997 Michael Messollen. All rights reserved. */
///////////////////////////////////////////////////////////////////////////////
// emacs edit mode for this file is -*- C++ -*-
// $Id: homogfactor.h,v 1.2 1997-06-09 15:56:10 Singular Exp $
///////////////////////////////////////////////////////////////////////////////
#ifndef HFACTOR_H
#define HFACTOR_H
CFList get_Terms( const CanonicalForm & f );
bool is_homogeneous( const CanonicalForm & f);
CFFList  HomogFactor( const CanonicalForm & g, const Variable  & minpoly, const int Mainvar );
#endif /* HFACTOR_H */

////////////////////////////////////////////////////////////
/*
$Log: not supported by cvs2svn $
Revision 1.2  1997/04/25 22:14:46  michael
Version for libfac-0.2.1

*/

/* Copyright 1996 Michael Messollen. All rights reserved. */
////////////////////////////////////////////////////////////
// emacs edit mode for this file is -*- C++ -*-
// $Id: MVMultiHensel.h,v 1.2 1997-06-09 15:56:00 Singular Exp $
/////////////////////////////////////////////////////////////
#ifndef MULTIHENSEL_H
#define MULTIHENSEL_H
// recursive version:
CFFList multihensel( const CanonicalForm & mF, const CFFList & Factorlist, const SFormList & Substitutionlist);
// this is the real one (quasiparallel):
CFFList MultiHensel( const CanonicalForm & mF, const CFFList & Factorlist, const SFormList & Substitutionlist);

#endif /* MULTIHENSEL_H */

////////////////////////////////////////////////////////////
/*
$Log: not supported by cvs2svn $
Revision 1.2  1997/04/25 22:31:40  michael
Version for libfac-0.2.1

*/

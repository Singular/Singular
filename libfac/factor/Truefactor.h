////////////////////////////////////////////////////////////
// emacs edit mode for this file is -*- C++ -*-
// $Id: Truefactor.h,v 1.4 2008-06-10 14:49:15 Singular Exp $
///////////////////////////////////////////////////////////////////////////////
#ifndef TRUEFACTOR_H
#define TRUEFACTOR_H

CFFList Truefactors( const CanonicalForm Ua, int levelU, const SFormList & SubstitutionList, const CFFList & PiList);
CFFList TakeNorms(const CFFList & PiList);
#endif /* TRUEFACTOR_H */

///////////////////////////////////////////////////////////////////////////////
/*
$Log: not supported by cvs2svn $
Revision 1.3  1997/09/12 07:19:53  Singular
* hannes/michael: libfac-0.3.0

Revision 1.2  1997/04/25 22:25:50  michael
Version for libfac-0.2.1

*/

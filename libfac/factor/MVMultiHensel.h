////////////////////////////////////////////////////////////
// emacs edit mode for this file is -*- C++ -*-
// $Id: MVMultiHensel.h,v 1.5 2008-06-10 14:49:15 Singular Exp $
/////////////////////////////////////////////////////////////
#ifndef MULTIHENSEL_H
#define MULTIHENSEL_H
// recursive version:
CFFList multihensel( const CanonicalForm & mF, const CFFList & Factorlist,
                     const SFormList & Substitutionlist,
                     const CanonicalForm &alpha);
// this is the real one (quasiparallel):
CFFList MultiHensel( const CanonicalForm & mF, const CFFList & Factorlist,
                     const SFormList & Substitutionlist,
                     const CanonicalForm &alpha);

#endif /* MULTIHENSEL_H */

////////////////////////////////////////////////////////////
/*
$Log: not supported by cvs2svn $
Revision 1.4  2007/05/21 16:40:12  Singular
*hannes: Factorize2

Revision 1.3  1997/09/12 07:19:48  Singular
* hannes/michael: libfac-0.3.0

Revision 1.2  1997/04/25 22:31:40  michael
Version for libfac-0.2.1

*/

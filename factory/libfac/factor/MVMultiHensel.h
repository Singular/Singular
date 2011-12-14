////////////////////////////////////////////////////////////
// emacs edit mode for this file is -*- C++ -*-
// $Id$
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

// emacs edit mode for this file is -*- C++ -*-
// $Id: cf_defs.h,v 1.2 1996-07-08 08:16:06 stobbe Exp $

#ifndef INCL_CF_DEFS_H
#define incl_cf_defs_H

/*
$Log: not supported by cvs2svn $
Revision 1.1  1996/07/02 11:18:57  stobbe
"new switch SW_USE_EZGCD.
"

Revision 1.0  1996/05/17 10:59:38  stobbe
Initial revision

*/

#include "conf.h"

/*BEGINPUBLIC*/

#define LEVELBASE -1000000
#define LEVELTRANS -500000
#define LEVELQUOT 1000000
#define LEVELEXPR 1000001

#define UndefinedDomain 32000
#define PrimePowerDomain 5
#define GaloisFieldDomain 4
#define FiniteFieldDomain 3
#define RationalDomain 2
#define IntegerDomain 1

#define SW_RATIONAL 0
#define SW_QUOTIENT 1
#define SW_SYMMETRIC_FF 2
#define SW_BERLEKAMP 3
#define SW_FAC_USE_BIG_PRIMES 4
#define SW_FAC_QUADRATICLIFT 5
#define SW_USE_EZGCD 6

/*ENDPUBLIC*/

#endif /* INCL_CF_DEFS_H */

/* emacs edit mode for this file is -*- C++ -*- */
/* $Id: cf_defs.h,v 1.5 1997-06-19 12:27:23 schmidt Exp $ */

#ifndef INCL_CF_DEFS_H
#define INCL_CF_DEFS_H

#include <config.h>

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
#define SW_USE_SPARSEMOD 7

/*ENDPUBLIC*/

#endif /* ! INCL_CF_DEFS_H */

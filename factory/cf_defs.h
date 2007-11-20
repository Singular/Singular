/* emacs edit mode for this file is -*- C++ -*- */
/* $Id: cf_defs.h,v 1.13 2007-11-20 10:08:02 Singular Exp $ */

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

//{{{ constants
//{{{ docu
//
// - factory switches.
//
//}}}
const int SW_RATIONAL = 0;
const int SW_QUOTIENT = 1;
const int SW_SYMMETRIC_FF = 2;
const int SW_BERLEKAMP = 3;
const int SW_FAC_USE_BIG_PRIMES = 4;
const int SW_FAC_QUADRATICLIFT = 5;
const int SW_USE_EZGCD = 6;
const int SW_USE_EZGCD_P = 7;
const int SW_USE_SPARSEMOD = 8;
const int SW_USE_NTL=9;
const int SW_USE_NTL_GCD_0=10;
const int SW_USE_NTL_GCD_P=11;
const int SW_USE_NTL_SORT=12;
const int SW_USE_CHINREM_GCD=13;
const int SW_USE_GCD_P=14;
//}}}

/*ENDPUBLIC*/

#endif /* ! INCL_CF_DEFS_H */

/* emacs edit mode for this file is -*- C++ -*- */

/**
 * @file cf_defs.h
 *
 * factory switches.
**/

#ifndef INCL_CF_DEFS_H
#define INCL_CF_DEFS_H

// #include "config.h"

/*BEGINPUBLIC*/

#define LEVELBASE -1000000
#define LEVELTRANS -500000
#define LEVELQUOT 1000000
#define LEVELEXPR 1000001

#define UndefinedDomain 32000
#define GaloisFieldDomain 4
#define FiniteFieldDomain 3
#define RationalDomain 2
#define IntegerDomain 1

/// set to 1 for computations over Q
const int SW_RATIONAL = 0;
/// set to 1 for symmetric representation over F_q
const int SW_SYMMETRIC_FF = 1;
/// set to 1 to use EZGCD over Z
const int SW_USE_EZGCD = 2;
/// set to 1 to use EZGCD over F_q
const int SW_USE_EZGCD_P = 3;
/// set to 1 to sort factors in a factorization
const int SW_USE_NTL_SORT=4;
/// set to 1 to use modular gcd over Z
const int SW_USE_CHINREM_GCD=5;
/// set to 1 to use Encarnacion GCD over Q(a)
const int SW_USE_QGCD=6;
/// set to 1 to use modular GCD over F_q
const int SW_USE_FF_MOD_GCD=7;

/*ENDPUBLIC*/

#endif /* ! INCL_CF_DEFS_H */

/* emacs edit mode for this file is -*- C++ -*- */

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

//{{{ constants
//{{{ docu
//
// - factory switches.
//
//}}}
const int SW_RATIONAL = 0;
const int SW_SYMMETRIC_FF = 1;
const int SW_BERLEKAMP = 2;
const int SW_FAC_QUADRATICLIFT = 3;
const int SW_USE_EZGCD = 4;
const int SW_USE_EZGCD_P = 5;
const int SW_USE_NTL=6;
const int SW_USE_NTL_GCD_0=7;
const int SW_USE_NTL_GCD_P=8;
const int SW_USE_NTL_SORT=9;
const int SW_USE_CHINREM_GCD=10;
const int SW_USE_QGCD=11;
const int SW_USE_FF_MOD_GCD=12;
//}}}

/*ENDPUBLIC*/

#endif /* ! INCL_CF_DEFS_H */

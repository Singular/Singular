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
const int SW_USE_EZGCD = 2;
const int SW_USE_EZGCD_P = 3;
const int SW_USE_NTL_SORT=4;
const int SW_USE_CHINREM_GCD=5;
const int SW_USE_QGCD=6;
const int SW_USE_FF_MOD_GCD=7;
//}}}

/*ENDPUBLIC*/

#endif /* ! INCL_CF_DEFS_H */

/* emacs edit mode for this file is -*- C++ -*- */
/* $Id$ */

#ifndef INCL_NTL_UTIL_H
#define INCL_NTL_UTIL_H

//{{{ docu
//
// ntl_util.h - header to ntl_util.cc.
//
//}}}

#include <factory.h>

#include <ZZ.h>
#include <ZZ_p.h>
#include <ZZX.h>
#include <zz_pX.h>

void operator << ( ZZ &, const CanonicalForm & );
void operator << ( CanonicalForm &, const ZZ & );
void operator << ( ZZX &, const CanonicalForm & );
void operator << ( CanonicalForm &, const ZZX & );
void operator << ( zz_p &, const CanonicalForm & );
void operator << ( CanonicalForm &, const zz_p & );
void operator << ( zz_pX &, const CanonicalForm & );
void operator << ( CanonicalForm &, const zz_pX & );

void setCharacteristicNTL ( int );

#endif /* ! INCL_NTL_UTIL_H */

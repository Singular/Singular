/**
 * @file cfUnivarGcd.h
 *
 * univariate Gcd over finite fields and Z, extended GCD over finite fields
 * and Q
 *
 * @note if NTL or FLINT are available they are used to compute the (ext)Gcd
**/

#ifndef CF_UNIVAR_GCD_H
#define CF_UNIVAR_GCD_H

#ifdef HAVE_NTL
#include <NTL/ZZX.h>
#include "NTLconvert.h"
bool isPurePoly(const CanonicalForm & );
#ifndef HAVE_FLINT
CanonicalForm gcd_univar_ntl0( const CanonicalForm &, const CanonicalForm & );
CanonicalForm gcd_univar_ntlp( const CanonicalForm &, const CanonicalForm & );
#endif
#endif

#ifdef HAVE_FLINT
#include "FLINTconvert.h"
CanonicalForm gcd_univar_flint0 (const CanonicalForm &, const CanonicalForm &);
CanonicalForm gcd_univar_flintp (const CanonicalForm &, const CanonicalForm &);
#endif

#ifndef HAVE_NTL
CanonicalForm gcd_poly_univar0( const CanonicalForm & F, const CanonicalForm & G, bool primitive );
#endif

/*BEGINPUBLIC*/
CanonicalForm FACTORY_PUBLIC
extgcd ( const CanonicalForm & f, const CanonicalForm & g, CanonicalForm & a, CanonicalForm & b );
/*ENDPUBLIC*/

#endif

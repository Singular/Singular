/**
 * @file cfGcdAlgExt.h
 *
 * GCD over Q(a)
 *
 * ABSTRACT: Implementation of Encarnacion's GCD algorithm over number fields,
 * see M.J. Encarnacion "Computing GCDs of polynomials over number fields",
 * extended to the multivariate case.
 *
 * @sa cfNTLzzpEXGCD.h
**/

#ifndef CF_GCD_ALGEXT_H
#define CF_GCD_ALGEXT_H

// #include "config.h"

#include "canonicalform.h"
#include "variable.h"

/// gcd over Q(a)
CanonicalForm QGCD( const CanonicalForm &, const CanonicalForm & );

#ifndef HAVE_NTL
void tryDivrem (const CanonicalForm&, const CanonicalForm&, CanonicalForm&,
                CanonicalForm&, CanonicalForm&, const CanonicalForm&,
                bool&);
void tryEuclid( const CanonicalForm &, const CanonicalForm &, const CanonicalForm &, CanonicalForm &, bool & );
void tryExtgcd( const CanonicalForm & F, const CanonicalForm & G, const CanonicalForm& M, CanonicalForm & result, CanonicalForm & s, CanonicalForm & t, bool & fail );
#endif
void tryInvert( const CanonicalForm &, const CanonicalForm &, CanonicalForm &, bool & );

/// modular gcd over F_p[x]/(M) for not necessarily irreducible M.
/// If a zero divisor is encountered fail is set to true.
void tryBrownGCD( const CanonicalForm & F, const CanonicalForm & G, const CanonicalForm & M, CanonicalForm & result, bool & fail, bool topLevel= true );

bool isLess(int *a, int *b, int lower, int upper);
bool isEqual(int *a, int *b, int lower, int upper);
CanonicalForm firstLC(const CanonicalForm & f);

#endif


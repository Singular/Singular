#ifndef CF_GCD_ALGEXT_H
#define CF_GCD_ALGEXT_H

// #include "config.h"

#include "canonicalform.h"
#include "variable.h"

CanonicalForm QGCD( const CanonicalForm &, const CanonicalForm & );
#ifndef HAVE_NTL
void tryDivrem (const CanonicalForm&, const CanonicalForm&, CanonicalForm&,
                CanonicalForm&, CanonicalForm&, const CanonicalForm&,
                bool&);
void tryEuclid( const CanonicalForm &, const CanonicalForm &, const CanonicalForm &, CanonicalForm &, bool & );
void tryExtgcd( const CanonicalForm & F, const CanonicalForm & G, const CanonicalForm& M, CanonicalForm & result, CanonicalForm & s, CanonicalForm & t, bool & fail );
#endif
void tryInvert( const CanonicalForm &, const CanonicalForm &, CanonicalForm &, bool & );
/*BEGINPUBLIC*/
bool hasFirstAlgVar( const CanonicalForm &, Variable & );
/*ENDPUBLIC*/
void tryBrownGCD( const CanonicalForm & F, const CanonicalForm & G, const CanonicalForm & M, CanonicalForm & result, bool & fail, bool topLevel= true );
int * leadDeg(const CanonicalForm & f, int *degs);
bool isLess(int *a, int *b, int lower, int upper);
bool isEqual(int *a, int *b, int lower, int upper);
CanonicalForm firstLC(const CanonicalForm & f);

#endif


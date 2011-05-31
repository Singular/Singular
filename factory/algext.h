#ifndef ALGEXT_H
#define ALGEXT_H

#include <config.h>

#include "canonicalform.h"
#include "variable.h"

CanonicalForm QGCD( const CanonicalForm &, const CanonicalForm & );
void tryEuclid( const CanonicalForm &, const CanonicalForm &, const CanonicalForm &, CanonicalForm &, bool & );
void tryInvert( const CanonicalForm &, const CanonicalForm &, CanonicalForm &, bool & );
bool hasFirstAlgVar( const CanonicalForm &, Variable & );
void tryBrownGCD( const CanonicalForm & F, const CanonicalForm & G, const CanonicalForm & M, CanonicalForm & result, bool & fail );
void tryCRA( const CanonicalForm & x1, const CanonicalForm & q1, const CanonicalForm & x2, const CanonicalForm & q2, CanonicalForm & xnew, CanonicalForm & qnew, bool & fail );
void tryExtgcd( const CanonicalForm & F, const CanonicalForm & G, CanonicalForm & result, CanonicalForm & s, CanonicalForm & t, bool & fail );
int * leadDeg(const CanonicalForm & f, int *degs);
bool isLess(int *a, int *b, int lower, int upper);
bool isEqual(int *a, int *b, int lower, int upper);
CanonicalForm firstLC(const CanonicalForm & f);

#endif


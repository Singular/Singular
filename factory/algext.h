#include <config.h>

#include "canonicalform.h"
#include "variable.h"

CanonicalForm QGCD( const CanonicalForm &, const CanonicalForm & );
CanonicalForm univarQGCD( const CanonicalForm & F, const CanonicalForm & G );
//void tryEuclid( const CanonicalForm &, const CanonicalForm &, const CanonicalForm, CanonicalForm &, bool & );
void tryInvert( const CanonicalForm &, const CanonicalForm &, CanonicalForm &, bool & );
bool hasFirstAlgVar( const CanonicalForm &, Variable & );
void tryBrownGCD( const CanonicalForm & F, const CanonicalForm & G, const CanonicalForm & M, CanonicalForm & result, bool & fail );
void tryCRA( const CanonicalForm & x1, const CanonicalForm & q1, const CanonicalForm & x2, const CanonicalForm & q2, CanonicalForm & xnew, CanonicalForm & qnew, bool & fail );
void tryExtgcd( const CanonicalForm & F, const CanonicalForm & G, CanonicalForm & result, CanonicalForm & s, CanonicalForm & t, bool & fail );




#include <config.h>

#include "canonicalform.h"
#include "variable.h"

CanonicalForm QGCD( const CanonicalForm &, const CanonicalForm & );
void tryEuclid( const CanonicalForm &, const CanonicalForm &, const CanonicalForm, CanonicalForm &, bool & );
void tryInvert( const CanonicalForm &, const CanonicalForm &, CanonicalForm &, bool & );
bool hasFirstAlgVar( const CanonicalForm &, Variable & );
////////////////////////////////////////////////////////////
// emacs edit mode for this file is -*- C++ -*-
////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////

#ifndef FAC_ALG_FUNC_H
#define FAC_ALG_FUNC_H

#include "canonicalform.h"

// missing class: IntGenerator:
class IntGenerator : public CFGenerator
{
private:
    int current;
public:
    IntGenerator() : current(0) {}
    ~IntGenerator() {}
    bool hasItems() const;
    void reset() { current = 0; }
    CanonicalForm item() const;
    void next();
    void operator++ () { next(); }
    void operator++ ( int ) { next(); }
};

CanonicalForm alg_gcd(const CanonicalForm &, const CanonicalForm &, const CFList &);
/*BEGINPUBLIC*/
CFFList newfactoras( const CanonicalForm & f, const CFList & as, int &success);
CFFList newcfactor(const CanonicalForm & f, const CFList & as, int & success );
/*ENDPUBLIC*/

#endif /* INCL_ALGFACTOR_H */

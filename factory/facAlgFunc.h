/*****************************************************************************\
 * Computer Algebra System SINGULAR
\*****************************************************************************/
/** @file facAlgFunc.h
 *
 * Factorization over algebraic function fields
 *
 * @note some of the code is code from libfac or derived from code from libfac.
 * Libfac is written by M. Messollen. See also COPYING for license information
 * and README for general information on characteristic sets.
 *
 * @author Martin Lee
 *
 **/
/*****************************************************************************/

#ifndef FAC_ALG_FUNC_H
#define FAC_ALG_FUNC_H

#include "canonicalform.h"
#include "cf_generator.h"

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

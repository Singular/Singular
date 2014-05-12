/*****************************************************************************\
 * Computer Algebra System SINGULAR
\*****************************************************************************/
/** @file facAlgFuncUtil.h
 *
 * Utility functions for factorization over algebraic function fields
 *
 * @note some of the code is code from libfac or derived from code from libfac.
 * Libfac is written by M. Messollen. See also COPYING for license information
 * and README for general information on characteristic sets.
 *
 * @author Martin Lee
 *
 **/
/*****************************************************************************/


#ifndef FAC_ALG_FUNC_UTIL_H
#define FAC_ALG_FUNC_UTIL_H

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

CFFList
append (const CFFList & Inputlist, const CFFactor & TheFactor);

CFFList
merge (const CFFList & Inputlist1, const CFFList & Inputlist2);

Varlist
varsInAs (const Varlist & uord, const CFList & As);

int hasVar (const CanonicalForm &f, const Variable &v);

int hasAlgVar(const CanonicalForm &f);

CanonicalForm
generateMipo (int degOfExt);

CanonicalForm alg_lc (const CanonicalForm & f);

CanonicalForm alg_LC (const CanonicalForm& f, int lev);

void deflateDegree (const CanonicalForm & F, int & pExp, int n);

CanonicalForm deflatePoly (const CanonicalForm & F, int exps, int n);

CanonicalForm inflatePoly (const CanonicalForm & F, int exps, int n);

void
multiplicity (CFFList& factors, const CanonicalForm& F, const CFList& as);

CanonicalForm
backSubst (const CanonicalForm& F, const CFList& a, const CFList& b);

CanonicalForm
subst (const CanonicalForm& f, const CFList& a, const CFList& b,
       const CanonicalForm& Rstar, bool isFunctionField);

CanonicalForm
divide (const CanonicalForm & ff, const CanonicalForm & f, const CFList & as);

CanonicalForm
QuasiInverse (const CanonicalForm& f, const CanonicalForm& g,
              const Variable& x);

CanonicalForm
evaluate (const CanonicalForm& f, const CanonicalForm& g,
          const CanonicalForm& h, const CanonicalForm& powH,
          const Variable& v);

int
getDegOfExt (IntList & degreelist, int n);

bool
isInseparable (const CFList & Astar);

#endif


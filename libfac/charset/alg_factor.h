////////////////////////////////////////////////////////////
// emacs edit mode for this file is -*- C++ -*-
////////////////////////////////////////////////////////////
// $Id: alg_factor.h,v 1.4 2008-11-06 14:05:51 Singular Exp $
////////////////////////////////////////////////////////////

#ifndef INCL_NEW_ALGFACTOR_H
#define INCL_NEW_ALGFACTOR_H

#include <factory.h>
#include <tmpl_inst.h>  // for typedef's

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

CFFList newfactoras( const CanonicalForm & f, const CFList & as, int &success);
CFFList newcfactor(const CanonicalForm & f, const CFList & as, int success );
/*BEGINPUBLIC*/
/*ENDPUBLIC*/

#endif /* INCL_ALGFACTOR_H */

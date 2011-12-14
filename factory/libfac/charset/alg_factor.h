////////////////////////////////////////////////////////////
// emacs edit mode for this file is -*- C++ -*-
////////////////////////////////////////////////////////////
// $Id$
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
/*BEGINPUBLIC*/
CFFList newcfactor(const CanonicalForm & f, const CFList & as, int & success );
/*ENDPUBLIC*/

#endif /* INCL_ALGFACTOR_H */

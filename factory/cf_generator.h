/* emacs edit mode for this file is -*- C++ -*- */
/* $Id: cf_generator.h,v 1.3 2004-12-10 10:14:46 Singular Exp $ */

#ifndef INCL_CF_GENERATOR_H
#define INCL_CF_GENERATOR_H

#include <config.h>

#include "canonicalform.h"

/*BEGINPUBLIC*/

class CFGenerator
{
public:
    CFGenerator() {}
    virtual ~CFGenerator() {}
    virtual bool hasItems() const { return false; }
    virtual void reset() {};
    virtual CanonicalForm item() const { return 0; }
    virtual void next() {};
};

class FFGenerator : public CFGenerator
{
private:
    int current;
public:
    FFGenerator() : current(0) {}
    ~FFGenerator() {}
    bool hasItems() const;
    void reset() { current = 0; }
    CanonicalForm item() const;
    void next();
    void operator++ () { next(); }
    void operator++ ( int ) { next(); }
};

class GFGenerator : public CFGenerator
{
private:
    int current;
public:
    GFGenerator();
    ~GFGenerator() {}
    bool hasItems() const;
    void reset();
    CanonicalForm item() const;
    void next();
    void operator++ () { next(); }
    void operator++ ( int ) { next(); }
};

class AlgExtGenerator //??? : public CFGenerator
{
private:
    Variable algext;
    FFGenerator **gensf;
    GFGenerator **gensg;
    int n;
    bool nomoreitems;
    AlgExtGenerator();
    AlgExtGenerator( const AlgExtGenerator & );
    AlgExtGenerator& operator= ( const AlgExtGenerator & );
public:
    AlgExtGenerator( const Variable & a );
    ~AlgExtGenerator();

    bool hasItems() const { return ! nomoreitems; }
    void reset();
    CanonicalForm item() const;
    void next();
    void operator++ () { next(); }
    void operator++ ( int ) { next(); }
};

class CFGenFactory
{
public:
    static CFGenerator* generate();
};

/*ENDPUBLIC*/

#endif /* ! INCL_CF_GENERATOR_H */

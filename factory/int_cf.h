/* emacs edit mode for this file is -*- C++ -*- */
/* $Id: int_cf.h,v 1.12 2006-05-15 09:03:05 Singular Exp $ */

#ifndef INCL_INT_CF_H
#define INCL_INT_CF_H

#include <config.h>

#ifndef NOSTREAMIO
#ifdef HAVE_IOSTREAM
#include <iostream>
#define OSTREAM std::ostream
#elif defined(HAVE_IOSTREAM_H)
#include <iostream.h>
#define OSTREAM ostream
#endif
#endif /* NOSTREAMIO */

#include "assert.h"

#include "cf_defs.h"
#include "variable.h"

class CanonicalForm;

class InternalCF {
private:
    int refCount;
protected:
    int getRefCount() { return refCount; };
    void incRefCount() { refCount++; };
    int decRefCount() { return --refCount; };
public:
    InternalCF() { refCount = 1; };
    InternalCF( const InternalCF& )
    {
	ASSERT( 0, "ups there is something wrong in your code");
    };
    virtual ~InternalCF() {};
    int deleteObject() { return decRefCount() == 0; }
    InternalCF* copyObject() { incRefCount(); return this; }
    virtual InternalCF* deepCopyObject() const PVIRT_INTCF("deepCopyObject");
    virtual const char * const classname() const PVIRT_CHARCC("classname");
    virtual InternalCF* genZero() PVIRT_INTCF("genZero");
    virtual InternalCF* genOne() PVIRT_INTCF("genOne");
    virtual int level() const { return LEVELBASE; }
    virtual int levelcoeff() const { return UndefinedDomain; }
    virtual int type() const { return UndefinedDomain; }
    virtual Variable variable() const { return Variable(); }
#ifndef NOSTREAMIO
    virtual void print( OSTREAM&, char* ) PVIRT_VOID("print");
#endif /* NOSTREAMIO */
    virtual bool inBaseDomain() const { return true; }
    virtual bool inExtension() const { return false; }
    virtual bool inCoeffDomain() const { return true; }
    virtual bool inPolyDomain() const { return false; }
    virtual bool inQuotDomain() const { return false; }
    virtual bool isZero() const;
    virtual bool isOne() const;
    virtual bool isUnivariate() const { return false; }
    virtual int intval() const;
    virtual int intmod( int ) const { return 0; }
    virtual int sign() const PVIRT_INT("sign");

    virtual InternalCF* num();
    virtual InternalCF* den();

    virtual InternalCF* neg() PVIRT_INTCF("neg");
    virtual InternalCF* invert(); // semantically const, changes refCount
    virtual int comparesame ( InternalCF * ) PVIRT_INT("comparesame");
    virtual int comparecoeff ( InternalCF * ) PVIRT_INT("comparecoeff");

    virtual InternalCF* addsame( InternalCF* ) PVIRT_INTCF("addsame");
    virtual InternalCF* subsame( InternalCF* ) PVIRT_INTCF("subsame");
    virtual InternalCF* mulsame( InternalCF* ) PVIRT_INTCF("mulsame");
    virtual InternalCF* dividesame( InternalCF* ) PVIRT_INTCF("dividesame");
    virtual InternalCF* modulosame( InternalCF* ) PVIRT_INTCF("modulosame");
    virtual InternalCF* divsame( InternalCF* ) PVIRT_INTCF("divsame");
    virtual InternalCF* modsame( InternalCF* ) PVIRT_INTCF("modsame");
    virtual void divremsame( InternalCF*, InternalCF*&, InternalCF*& ) PVIRT_VOID("divremsame");
    virtual bool divremsamet( InternalCF*, InternalCF*&, InternalCF*& ) PVIRT_BOOL("divremsamet");

    virtual InternalCF* addcoeff( InternalCF* ) PVIRT_INTCF("addcoeff");
    virtual InternalCF* subcoeff( InternalCF*, bool ) PVIRT_INTCF("subcoeff");
    virtual InternalCF* mulcoeff( InternalCF* ) PVIRT_INTCF("mulcoeff");
    virtual InternalCF* dividecoeff( InternalCF*, bool ) PVIRT_INTCF("dividecoeff");
    virtual InternalCF* modulocoeff( InternalCF*, bool ) PVIRT_INTCF("dividecoeff");
    virtual InternalCF* divcoeff( InternalCF*, bool ) PVIRT_INTCF("divcoeff");
    virtual InternalCF* modcoeff( InternalCF*, bool ) PVIRT_INTCF("modcoeff");
    virtual void divremcoeff( InternalCF*, InternalCF*&, InternalCF*&, bool ) PVIRT_VOID("divremcoeff");
    virtual bool divremcoefft( InternalCF*, InternalCF*&, InternalCF*&, bool ) PVIRT_BOOL("divremcoefft");

    virtual InternalCF * bgcdsame ( const InternalCF * const ) const;
    virtual InternalCF * bgcdcoeff ( const InternalCF * const ); // semantically const, changes refcount

    virtual InternalCF * bextgcdsame ( InternalCF *, CanonicalForm &, CanonicalForm & ); // semantically const, changes refcount
    virtual InternalCF * bextgcdcoeff ( InternalCF *, CanonicalForm &, CanonicalForm & ); // semantically const, changes refcount

    virtual InternalCF* sqrt();
    virtual int ilog2();
    virtual CanonicalForm lc();
    virtual CanonicalForm Lc();
    virtual CanonicalForm LC();
    virtual CanonicalForm coeff( int i );
    virtual int degree();
    virtual int taildegree();
    virtual CanonicalForm tailcoeff();
};

#endif /* ! INCL_INT_CF_H */

// emacs edit mode for this file is -*- C++ -*-
// $Id: int_cf.h,v 1.0 1996-05-17 10:59:41 stobbe Exp $

#ifndef INCL_INTERNALCF_H
#define INCL_INTERNALCF_H

/*
$Log: not supported by cvs2svn $
*/

#include <iostream.h>
#include "cf_defs.h"
#include "variable.h"

class CanonicalForm;

#ifdef NDEBUG
#define PVIRT_VOID(msg) = 0
#define PVIRT_INTCF(msg) = 0
#define PVIRT_BOOL(msg) = 0
#define PVIRT_INT(msg) = 0
#define PVIRT_CHARCC(msg) = 0
#else
#define PVIRT_VOID(msg) { cerr << "pure method(" << msg << ") called" << endl; }
#define PVIRT_INTCF(msg) { cerr << "pure method(" << msg << ") called" << endl; return 0; }
#define PVIRT_BOOL(msg) { cerr << "pure method(" << msg << ") called" << endl; return false; }
#define PVIRT_INT(msg) { cerr << "pure method(" << msg << ") called" << endl; return 0; }
#define PVIRT_CHARCC(msg) { cerr << "pure method(" << msg << ") called" << endl; return 0; }
#endif

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
	cerr << "ups there is something wrong in your code" << endl; 
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
    virtual void print( ostream&, char* ) PVIRT_VOID("print");
    virtual bool inBaseDomain() const { return true; }
    virtual bool inExtension() const { return false; }
    virtual bool inCoeffDomain() const { return true; }
    virtual bool inPolyDomain() const { return false; }
    virtual bool inQuotDomain() const { return false; }
    virtual bool isZero() const PVIRT_BOOL("isZero");
    virtual bool isOne() const PVIRT_BOOL("isOne");
    virtual bool isUnivariate() const { return false; }
    virtual int intval() const;
    virtual int intmod( int ) const { return 0; }
    virtual int sign() const;

    virtual InternalCF* num();
    virtual InternalCF* den();

    virtual InternalCF* neg() PVIRT_INTCF("neg");
    virtual InternalCF* invert(); // semantically const, changes refCount const
    virtual int comparesame ( InternalCF* ) PVIRT_INT("comparesame");

    virtual InternalCF* addsame( InternalCF* ) PVIRT_INTCF("addsame");
    virtual InternalCF* subsame( InternalCF* ) PVIRT_INTCF("subsame");
    virtual InternalCF* mulsame( InternalCF* ) PVIRT_INTCF("mulsame");
    virtual InternalCF* dividesame( InternalCF* ) PVIRT_INTCF("dividesame");
    virtual InternalCF* modulosame( InternalCF* ) PVIRT_INTCF("modulosame");
    virtual InternalCF* divsame( InternalCF* ) PVIRT_INTCF("divsame");
    virtual InternalCF* modsame( InternalCF* ) PVIRT_INTCF("modsame");
    virtual void divremsame( InternalCF*, InternalCF*&, InternalCF*& ) PVIRT_VOID("divremsame");
    virtual bool divremsamet( InternalCF*, InternalCF*&, InternalCF*& ) PVIRT_BOOL("divremsamet");

    virtual int comparecoeff ( InternalCF* );

    virtual InternalCF* addcoeff( InternalCF* ) PVIRT_INTCF("addcoeff");
    virtual InternalCF* subcoeff( InternalCF*, bool ) PVIRT_INTCF("subcoeff");
    virtual InternalCF* mulcoeff( InternalCF* ) PVIRT_INTCF("mulcoeff");
    virtual InternalCF* dividecoeff( InternalCF*, bool ) PVIRT_INTCF("dividecoeff");
    virtual InternalCF* modulocoeff( InternalCF*, bool ) PVIRT_INTCF("dividecoeff");
    virtual InternalCF* divcoeff( InternalCF*, bool ) PVIRT_INTCF("divcoeff");
    virtual InternalCF* modcoeff( InternalCF*, bool ) PVIRT_INTCF("modcoeff");
    virtual void divremcoeff( InternalCF*, InternalCF*&, InternalCF*&, bool ) PVIRT_VOID("divremcoeff");
    virtual bool divremcoefft( InternalCF*, InternalCF*&, InternalCF*&, bool ) PVIRT_BOOL("divremcoefft");

    virtual InternalCF* sqrt();
    virtual CanonicalForm lc();
    virtual CanonicalForm LC();
    virtual CanonicalForm coeff( int i );
    virtual int degree();
    virtual int taildegree();
    virtual CanonicalForm tailcoeff();
};

#define OBJDEL( x ) if ( (x)->deleteObject() ) delete (x)

#endif /* INCL_INTERNALCF_H */

// emacs edit mode for this file is -*- C++ -*-
// $Id: int_poly.h,v 1.0 1996-05-17 10:59:41 stobbe Exp $

#ifndef INCL_INTERNALPOLY_H
#define INCL_INTERNALPOLY_H

/*
$Log: not supported by cvs2svn $
*/

#include "cf_defs.h"
#include "int_cf.h"
#include "variable.h"
#include "canonicalform.h"


class term {
private:
    term * next;
    CanonicalForm coeff;
    int exp;
public:
    term() : next(0), coeff(0), exp(0) {}
    term( term * n, const CanonicalForm & c, int e ) : next(n), coeff(c), exp(e) {}
    friend class InternalPoly;
    friend class CFIterator;
};

typedef term * termList;


class InternalPoly : public InternalCF {
private:
    termList firstTerm, lastTerm;
    Variable var;
    InternalPoly( termList, termList, const Variable & );

    static termList copyTermList ( termList, termList&, bool negate = false );
    static termList deepCopyTermList ( termList, termList& );
    static void freeTermList ( termList );
    static void negateTermList ( termList );
    static termList addTermList ( termList, termList, termList&, bool negate );
    static void mulTermList ( termList, const CanonicalForm& , const int );
    static termList divTermList ( termList, const CanonicalForm&, termList& );
    static termList modTermList ( termList, const CanonicalForm&, termList& );
    static void appendTermList ( termList&, termList&, const CanonicalForm&, const int );
    static termList mulAddTermList ( termList theList, termList aList, const CanonicalForm & c, const int exp, termList & lastTerm, bool negate );
    static termList reduceTermList ( termList first, termList redterms, termList & last );
public:
    InternalPoly();
    InternalPoly( const Variable & v, const int e, const CanonicalForm& c );
    InternalPoly( const InternalPoly& );
    ~InternalPoly();
    InternalCF* deepCopyObject() const;
    const char * const classname() const { return "InternalPoly"; }
    int level() const { return var.level(); }
    Variable variable() const { return var; }
    int degree();
    CanonicalForm lc();
    CanonicalForm LC();
    int taildegree();
    CanonicalForm tailcoeff();
    CanonicalForm coeff( int i );
    void print( ostream&, char* );
    bool isZero() const { return false; }
    bool isOne() const { return false; }
    bool inBaseDomain() const { return false; }
    bool inExtension() const { return var.level() < 0; }
    bool inCoeffDomain() const { return var.level() < 0; }
    bool inPolyDomain() const { return var.level() > 0; }
    bool inQuotDomain() const { return false; }
    InternalCF* genZero();
    InternalCF* genOne();

    bool isUnivariate() const;

    InternalCF* neg();
    InternalCF* invert();

    int comparesame ( InternalCF* );

    InternalCF* addsame( InternalCF* );
    InternalCF* subsame( InternalCF* );
    InternalCF* mulsame( InternalCF* ); 
    InternalCF* dividesame( InternalCF* );
    InternalCF* modulosame( InternalCF* );
    InternalCF* divsame( InternalCF* );
    InternalCF* modsame( InternalCF* );
    void divremsame( InternalCF*, InternalCF*&, InternalCF*& );
    bool divremsamet( InternalCF*, InternalCF*&, InternalCF*& );

    int comparecoeff ( InternalCF* );

    InternalCF* addcoeff( InternalCF* );
    InternalCF* subcoeff( InternalCF*, bool );
    InternalCF* mulcoeff( InternalCF* ); 
    InternalCF* dividecoeff( InternalCF*, bool ); 
    InternalCF* modulocoeff( InternalCF*, bool ); 
    InternalCF* divcoeff( InternalCF*, bool ); 
    InternalCF* modcoeff( InternalCF*, bool );
    void divremcoeff( InternalCF*, InternalCF*&, InternalCF*&, bool );
    bool divremcoefft( InternalCF*, InternalCF*&, InternalCF*&, bool );

    int sign() const;

    friend class CFIterator;
};


#endif /* INCL_INTERNALPOLY_H */



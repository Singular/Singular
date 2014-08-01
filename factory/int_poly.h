/* emacs edit mode for this file is -*- C++ -*- */

#ifndef INCL_INT_POLY_H
#define INCL_INT_POLY_H

/**
 * @file int_poly.h
 *
 * Factory's internal polynomials
**/

// #include "config.h"

#ifndef NOSTREAMIO
#ifdef HAVE_IOSTREAM
#include <iostream>
#define OSTREAM std::ostream
#elif defined(HAVE_IOSTREAM_H)
#include <iostream.h>
#define OSTREAM ostream
#endif
#endif /* NOSTREAMIO */

#include "cf_defs.h"
#include "int_cf.h"
#include "variable.h"
#include "canonicalform.h"

#ifdef HAVE_OMALLOC
#ifndef OM_NDEBUG
#define OM_NDEBUG
#endif
#  include <omalloc/omalloc.h>
#endif

class term {
private:
    term * next;
    CanonicalForm coeff;
    int exp;
#ifdef HAVE_OMALLOC
  static const omBin term_bin;
#endif
public:
    term() : next(0), coeff(0), exp(0) {}
    term( term * n, const CanonicalForm & c, int e ) : next(n), coeff(c), exp(e) {}
    friend class InternalPoly;
    friend class CFIterator;
#ifdef HAVE_OMALLOC
  void* operator new(size_t)
    {
      void* addr;
      omTypeAllocBin(void*, addr, term_bin);
      return addr;
    }
  void operator delete(void* addr, size_t)
    {
      omFreeBin(addr, term_bin);
    }
#endif
};

typedef term * termList;


/**
 * factory's class for polynomials
 *
 * polynomials are represented as a linked list termList, factory
 * uses a sparse distributive representation of polynomials, i.e. each poly
 * is viewed as a univariate poly in its main variable CanonicalForm::mvar()
 * over a (polynomial) ring
**/
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
    static termList divideTermList ( termList, const CanonicalForm&, termList& );
    static termList divTermList ( termList, const CanonicalForm&, termList& );
    static termList tryDivTermList ( termList, const CanonicalForm&, termList&, const CanonicalForm&, bool& );
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
    const char * classname() const { return "InternalPoly"; }
    int level() const { return var.level(); }
    Variable variable() const { return var; }
    int degree();
    CanonicalForm lc();
    CanonicalForm Lc();
    CanonicalForm LC();
    int taildegree();
    CanonicalForm tailcoeff();
    CanonicalForm coeff( int i );
#ifndef NOSTREAMIO
    void print( OSTREAM&, char* );
#endif /* NOSTREAMIO */
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
    InternalCF* tryInvert( const CanonicalForm&, bool& );
    int comparesame ( InternalCF* );

    InternalCF* addsame( InternalCF* );
    InternalCF* subsame( InternalCF* );
    InternalCF* mulsame( InternalCF* );
    InternalCF* tryMulsame ( InternalCF*, const CanonicalForm&);
    InternalCF* dividesame( InternalCF* );
    InternalCF* modulosame( InternalCF* );
    InternalCF* divsame( InternalCF* );
    InternalCF* tryDivsame ( InternalCF*, const CanonicalForm&, bool& );
    InternalCF* modsame( InternalCF* );
    void divremsame( InternalCF*, InternalCF*&, InternalCF*& );
    bool divremsamet( InternalCF*, InternalCF*&, InternalCF*& );
    bool tryDivremsamet( InternalCF*, InternalCF*&, InternalCF*&, const CanonicalForm&, bool& );

    int comparecoeff ( InternalCF* );

    InternalCF* addcoeff( InternalCF* );
    InternalCF* subcoeff( InternalCF*, bool );
    InternalCF* mulcoeff( InternalCF* );
    InternalCF* dividecoeff( InternalCF*, bool );
    InternalCF* tryDividecoeff ( InternalCF*, bool, const CanonicalForm&, bool& );
    InternalCF* modulocoeff( InternalCF*, bool );
    InternalCF* divcoeff( InternalCF*, bool );
    InternalCF* tryDivcoeff ( InternalCF*, bool, const CanonicalForm&, bool& );
    InternalCF* modcoeff( InternalCF*, bool );
    void divremcoeff( InternalCF*, InternalCF*&, InternalCF*&, bool );
    bool divremcoefft( InternalCF*, InternalCF*&, InternalCF*&, bool );
    bool tryDivremcoefft ( InternalCF*, InternalCF*&, InternalCF*&, bool, const CanonicalForm&, bool& );

    int sign() const;

#ifdef HAVE_OMALLOC
  static const omBin InternalPoly_bin;
  void* operator new(size_t)
    {
      void* addr;
      omTypeAllocBin(void*, addr, InternalPoly_bin);
      return addr;
    }
  void operator delete(void* addr, size_t)
    {
      omFreeBin(addr, InternalPoly_bin);
    }
#endif
    friend class CFIterator;
};

#endif /* ! INCL_INT_POLY_H */

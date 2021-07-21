/* emacs edit mode for this file is -*- C++ -*- */

/**
 * @file canonicalform.h
 *
 * Header for factory's main class CanonicalForm
**/
#ifndef INCL_CANONICALFORM_H
#define INCL_CANONICALFORM_H

// #include "config.h"

#ifndef NOSTREAMIO
# ifdef HAVE_IOSTREAM
#  include <iostream>
#  define OSTREAM std::ostream
#  define ISTREAM std::istream
# elif defined(HAVE_IOSTREAM_H)
#  include <iostream.h>
#  define OSTREAM ostream
#  define ISTREAM istream
# endif
#endif /* NOSTREAMIO */

#include <stdint.h>

#include "cf_defs.h"
#include "variable.h"
#include "factory/cf_gmp.h"
#include "factory/templates/ftmpl_list.h"
#include "factory/templates/ftmpl_array.h"
#include "factory/templates/ftmpl_afactor.h"
#include "factory/templates/ftmpl_factor.h"
#include "factory/templates/ftmpl_matrix.h"
#include "si_log2.h"
#ifdef HAVE_OMALLOC
#ifndef XMEMORY_H
#include "omalloc/omallocClass.h"
#endif
#endif

/*BEGINPUBLIC*/

#undef CF_INLINE
#define CF_INLINE
#undef CF_NO_INLINE
#define CF_NO_INLINE

/*ENDPUBLIC*/

#ifdef CF_USE_INLINE
#ifndef __CYGWIN__
#undef CF_INLINE
#define CF_INLINE inline
#endif
#else
#undef CF_INLINE
#define CF_INLINE
#endif

/*BEGINPUBLIC*/

class InternalCF;

inline int is_imm ( const InternalCF * const ptr )
{
    // returns 0 if ptr is not immediate
    return ( ((int)((intptr_t)ptr)) & 3 );
}


/**
 * factory's main class
 *
 * a CanonicalForm can represent a polynomial over or a constant in F_p,
 * F_p(alpha), GF (F_p[t]/(Conway polynomial)), Z, or Q
 *
 * @sa int_poly.h, variable.h, ffops.h, gfops.h, imm.h, int_int.h, int_rat.h
**/
class FACTORY_PUBLIC CanonicalForm
#ifdef HAVE_OMALLOC
#ifndef XMEMORY_H
       : public omallocClass
#endif
#endif
{
private:
    InternalCF *value;
public:
    // constructors, destructors, selectors
    CF_INLINE CanonicalForm();
    CF_INLINE CanonicalForm( const CanonicalForm& );
    CF_INLINE CanonicalForm( InternalCF* );
    CF_INLINE CanonicalForm( const int );
    CF_INLINE CanonicalForm( const long );
    CF_INLINE CanonicalForm( const Variable & );
    CF_INLINE CanonicalForm( const Variable &, int );
    CanonicalForm( const char *, const int base=10 ); // use with caution - does only handle integers !!!

    CF_NO_INLINE ~CanonicalForm();

    InternalCF* getval() const; // use with caution !!!

    CanonicalForm deepCopy() const;

    void mpzval(mpz_t val) const;
    // predicates
    CF_NO_INLINE bool isOne() const;
    CF_NO_INLINE bool isZero() const;
    inline bool isImm() const { return is_imm( value ); };

    bool inZ() const;
    bool inQ() const;
    bool inFF() const;
    bool inGF() const;
    bool inBaseDomain() const;
    bool inExtension() const;
    bool inCoeffDomain() const;
    bool inPolyDomain() const;
    bool inQuotDomain() const;

    bool isFFinGF() const;
    bool isUnivariate() const;
    bool isHomogeneous() const;

    // conversion functions
    long intval() const;
    CanonicalForm mapinto () const;

    CanonicalForm lc () const;
    CanonicalForm Lc () const;
    CanonicalForm LC () const;
    CanonicalForm LC ( const Variable & v ) const;

    int degree () const;
    int degree ( const Variable & v ) const;

    CanonicalForm tailcoeff () const;
    CanonicalForm tailcoeff ( const Variable & v ) const;
    int taildegree () const;

    int level () const;
    Variable mvar () const;

    CanonicalForm num () const;
    CanonicalForm den () const;

    // assignment operators
    CF_NO_INLINE CanonicalForm& operator = ( const CanonicalForm& );
    CF_NO_INLINE CanonicalForm& operator = ( const long );

    CanonicalForm& operator += ( const CanonicalForm& );
    CanonicalForm& operator -= ( const CanonicalForm& );
    CanonicalForm& operator *= ( const CanonicalForm& );
    CanonicalForm& operator /= ( const CanonicalForm& );
    CanonicalForm& operator %= ( const CanonicalForm& );
    CanonicalForm& div ( const CanonicalForm& );
    CanonicalForm& tryDiv (const CanonicalForm&, const CanonicalForm&, bool& );
    CanonicalForm& mod ( const CanonicalForm& );

    // evaluation operators
    CanonicalForm operator () ( const CanonicalForm & f ) const;
    CanonicalForm operator () ( const CanonicalForm & f, const Variable & v ) const;

    CanonicalForm operator [] ( int i ) const;

    CanonicalForm deriv() const;
    CanonicalForm deriv( const Variable & x ) const;

    int sign() const;
    CanonicalForm sqrt() const;
    int ilog2() const;

    // comparison operators
    friend FACTORY_PUBLIC bool operator == ( const CanonicalForm&, const CanonicalForm& );
    friend FACTORY_PUBLIC bool operator != ( const CanonicalForm&, const CanonicalForm& );
    friend FACTORY_PUBLIC bool operator > ( const CanonicalForm&, const CanonicalForm& );
    friend FACTORY_PUBLIC bool operator < ( const CanonicalForm&, const CanonicalForm& );

    // arithmetic operators
    friend CF_NO_INLINE FACTORY_PUBLIC CanonicalForm operator - ( const CanonicalForm& );

    friend FACTORY_PUBLIC void divrem ( const CanonicalForm&, const CanonicalForm&, CanonicalForm&, CanonicalForm& );
    friend FACTORY_PUBLIC bool divremt ( const CanonicalForm&, const CanonicalForm&, CanonicalForm&, CanonicalForm& );
    friend FACTORY_PUBLIC bool tryDivremt ( const CanonicalForm&, const CanonicalForm&, CanonicalForm&, CanonicalForm&, const CanonicalForm&, bool& );

    friend FACTORY_PUBLIC CanonicalForm bgcd ( const CanonicalForm &, const CanonicalForm & );
    friend FACTORY_PUBLIC CanonicalForm bextgcd ( const CanonicalForm &, const CanonicalForm &, CanonicalForm &, CanonicalForm & );

    // input/output
#ifndef NOSTREAMIO
   void print( OSTREAM&, char * ) const;
   void print( OSTREAM& ) const;
   friend FACTORY_PUBLIC OSTREAM& operator << ( OSTREAM&, const CanonicalForm& );
   friend FACTORY_PUBLIC ISTREAM& operator >> ( ISTREAM&, CanonicalForm& );
#endif /* NOSTREAMIO */

    // obsolete methods
    CanonicalForm genZero() const;
    CanonicalForm genOne() const;

    friend class CFIterator;
};

CF_INLINE CanonicalForm
operator + ( const CanonicalForm&, const CanonicalForm& );

CF_NO_INLINE FACTORY_PUBLIC CanonicalForm
operator - ( const CanonicalForm&, const CanonicalForm& );

CF_INLINE CanonicalForm
operator * ( const CanonicalForm&, const CanonicalForm& );

CF_NO_INLINE FACTORY_PUBLIC CanonicalForm
operator / ( const CanonicalForm&, const CanonicalForm& );

CF_NO_INLINE FACTORY_PUBLIC CanonicalForm
operator % ( const CanonicalForm&, const CanonicalForm& );

CF_NO_INLINE FACTORY_PUBLIC CanonicalForm
div ( const CanonicalForm&, const CanonicalForm& );

CF_NO_INLINE FACTORY_PUBLIC CanonicalForm
mod ( const CanonicalForm&, const CanonicalForm& );

/*ENDPUBLIC*/

#ifdef CF_USE_INLINE
#ifndef __CYGWIN__
#include "cf_inline.cc"
#endif
#endif

/*BEGINPUBLIC*/

//{{{ function declarations from canonicalform.cc
CanonicalForm FACTORY_PUBLIC blcm ( const CanonicalForm & f, const CanonicalForm & g );

CanonicalForm FACTORY_PUBLIC power ( const CanonicalForm & f, int n );

CanonicalForm FACTORY_PUBLIC power ( const Variable & v, int n );
//}}}

//{{{ function declarations from cf_gcd.cc
CanonicalForm FACTORY_PUBLIC gcd ( const CanonicalForm&, const CanonicalForm& );

CanonicalForm FACTORY_PUBLIC gcd_poly ( const CanonicalForm & f, const CanonicalForm & g );

CanonicalForm FACTORY_PUBLIC lcm ( const CanonicalForm&, const CanonicalForm& );

CanonicalForm FACTORY_PUBLIC pp ( const CanonicalForm& );

CanonicalForm FACTORY_PUBLIC content ( const CanonicalForm& );

CanonicalForm FACTORY_PUBLIC content ( const CanonicalForm&, const Variable& );

CanonicalForm FACTORY_PUBLIC icontent ( const CanonicalForm & f );

CanonicalForm FACTORY_PUBLIC vcontent ( const CanonicalForm & f, const Variable & x );
//}}}

//{{{ function declarations from cf_ops.cc
CanonicalForm FACTORY_PUBLIC swapvar ( const CanonicalForm &, const Variable &, const Variable & );

CanonicalForm FACTORY_PUBLIC replacevar ( const CanonicalForm &, const Variable &, const Variable & );

int getNumVars( const CanonicalForm & f );

CanonicalForm getVars( const CanonicalForm & f );

CanonicalForm apply ( const CanonicalForm & f, void (*mf)( CanonicalForm &, int & ) );

CanonicalForm mapdomain ( const CanonicalForm & f, CanonicalForm (*mf)( const CanonicalForm & ) );

int * degrees ( const CanonicalForm & f, int * degs = 0 );

int totaldegree ( const CanonicalForm & f );

int totaldegree ( const CanonicalForm & f, const Variable & v1, const Variable & v2 );

int size ( const CanonicalForm & f, const Variable & v );

int size ( const CanonicalForm & f );

int size_maxexp ( const CanonicalForm & f, int& maxexp );

CanonicalForm reduce ( const CanonicalForm& f, const CanonicalForm & M);

bool hasFirstAlgVar( const CanonicalForm & f, Variable & a);

CanonicalForm leftShift (const CanonicalForm& F, int n);
//}}}

//{{{ inline functions corresponding to CanonicalForm methods
//{{{ docu
//
// - inline functions corresponding to CanonicalForm methods.
//
// These function exist for convenience only and because it is
// more beautiful to write 'degree( f )' than 'f.degree()'.
//
//}}}
inline CanonicalForm
lc ( const CanonicalForm & f ) { return f.lc(); }

inline CanonicalForm
Lc ( const CanonicalForm & f ) { return f.Lc(); }

inline CanonicalForm
LC ( const CanonicalForm & f ) { return f.LC(); }

inline CanonicalForm
LC ( const CanonicalForm & f, const Variable & v ) { return f.LC( v ); }

inline int
degree ( const CanonicalForm & f ) { return f.degree(); }

inline int
degree ( const CanonicalForm & f, const Variable & v ) { return f.degree( v ); }

inline int
taildegree ( const CanonicalForm & f ) { return f.taildegree(); }

inline CanonicalForm
tailcoeff ( const CanonicalForm & f ) { return f.tailcoeff(); }

inline CanonicalForm
tailcoeff (const CanonicalForm& f, const Variable& v) { return f.tailcoeff(v); }

inline int
level ( const CanonicalForm & f ) { return f.level(); }

inline Variable
mvar ( const CanonicalForm & f ) { return f.mvar(); }

inline CanonicalForm
num ( const CanonicalForm & f ) { return f.num(); }

inline CanonicalForm
den ( const CanonicalForm & f ) { return f.den(); }

inline int
sign ( const CanonicalForm & a ) { return a.sign(); }

inline CanonicalForm
deriv ( const CanonicalForm & f, const Variable & x ) { return f.deriv( x ); }

inline CanonicalForm
sqrt ( const CanonicalForm & a ) { return a.sqrt(); }

inline int
ilog2 ( const CanonicalForm & a ) { return a.ilog2(); }

inline CanonicalForm
mapinto ( const CanonicalForm & f ) { return f.mapinto(); }
//}}}

//{{{ inline functions
inline CanonicalForm
head ( const CanonicalForm & f )
{
    if ( f.level() > 0 )
        return power( f.mvar(), f.degree() ) * f.LC();
    else
        return f;
}

inline int
headdegree ( const CanonicalForm & f ) { return totaldegree( head( f ) ); }


//}}}

//{{{ other function declarations
void FACTORY_PUBLIC setCharacteristic( int c ); // -> Fp && Q
void setCharacteristic( int c, int n ); // -> PrimePower
void setCharacteristic( int c, int n, char name ); // -> GF(q)

int FACTORY_PUBLIC getCharacteristic();
int getGFDegree();
CanonicalForm getGFGenerator();

void FACTORY_PUBLIC On( int );
void FACTORY_PUBLIC Off( int );
bool FACTORY_PUBLIC isOn( int );
//}}}

//{{{ type definitions
typedef AFactor<CanonicalForm> CFAFactor;
typedef List <CFAFactor> CFAFList;
typedef ListIterator<CFAFactor> CFAFListIterator;
typedef Factor<CanonicalForm> CFFactor;
typedef List<CFFactor> CFFList;
typedef ListIterator<CFFactor> CFFListIterator;
typedef List<CanonicalForm> CFList;
typedef ListIterator<CanonicalForm> CFListIterator;
typedef Array<CanonicalForm> CFArray;
typedef Matrix<CanonicalForm> CFMatrix;
typedef List<CFList> ListCFList;
typedef ListIterator<CFList> ListCFListIterator ;
typedef List<int> IntList;
typedef ListIterator<int> IntListIterator;
typedef List<Variable> Varlist;
typedef ListIterator<Variable> VarlistIterator;
typedef Array<int> Intarray;
//}}}

/*ENDPUBLIC*/
#endif /* ! INCL_CANONICALFORM_H */

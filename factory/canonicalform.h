/* emacs edit mode for this file is -*- C++ -*- */
/* $Id: canonicalform.h,v 1.12 1997-08-29 11:30:01 schmidt Exp $ */

#ifndef INCL_CANONICALFORM_H
#define INCL_CANONICALFORM_H

#include <config.h>

#ifndef NOSTREAMIO
#include <iostream.h>
#endif /* NOSTREAMIO */

#include "cf_defs.h"
#include "variable.h"
#ifdef macintosh
#include <::templates:ftmpl_list.h>
#include <::templates:ftmpl_array.h>
#include <::templates:ftmpl_factor.h>
#include <::templates:ftmpl_matrix.h>
#else
#include "templates/ftmpl_list.h"
#include "templates/ftmpl_array.h"
#include "templates/ftmpl_factor.h"
#include "templates/ftmpl_matrix.h"
#endif

/*BEGINPUBLIC*/

int initCanonicalForm( void );

static int cf_is_initialized_now = initCanonicalForm();

class InternalCF;

class CanonicalForm {
private:
    InternalCF *value;
public:
    CanonicalForm();
    CanonicalForm( const CanonicalForm& );
    CanonicalForm( InternalCF* );
    CanonicalForm( const int );
    CanonicalForm( const Variable & );
    CanonicalForm( const Variable &, int );
    CanonicalForm( const char * ); // use with caution - does only handle integers !!!

    ~CanonicalForm();

    InternalCF* getval() const; // use with caution !!!

    int sign() const;

    bool isOne() const;
    bool isZero() const;
    bool isImm() const;

    int intval() const;

    CanonicalForm lc() const;
    CanonicalForm LC() const;
    CanonicalForm LC( const Variable & v ) const;

    int degree() const;
    int degree( const Variable & v ) const;

    CanonicalForm tailcoeff() const;
    int taildegree() const;

    int level() const;
    Variable mvar() const;

    CanonicalForm num() const;
    CanonicalForm den() const;

    CanonicalForm deepCopy() const;

    bool isFFinGF() const;
    bool hasGcd() const { return false; }
    CanonicalForm gcd( const CanonicalForm & f ) const;
    CanonicalForm deriv() const;
    CanonicalForm deriv( const Variable & x ) const;
    CanonicalForm sqrt() const;
    int ilog2() const;
    static CanonicalForm genCoeff( int what, int i = 0 );

    CanonicalForm genZero() const;
    CanonicalForm genOne() const;

    bool isUnivariate() const;

    bool inZ() const;
    bool inQ() const;
    bool inFF() const;
    bool inGF() const;
    bool inPP() const;
    bool inBaseDomain() const;
    bool inExtension() const;
    bool inCoeffDomain() const;
    bool inPolyDomain() const;
    bool inQuotDomain() const;

    CanonicalForm mapinto () const;

    CanonicalForm operator () ( const CanonicalForm & f ) const;
    CanonicalForm operator () ( const CanonicalForm & f, const Variable & v ) const;
    CanonicalForm operator [] ( int i ) const;
    CanonicalForm& operator = ( const CanonicalForm& );
    CanonicalForm& operator = ( const int );

    CanonicalForm& operator += ( const CanonicalForm& );
    CanonicalForm& operator -= ( const CanonicalForm& );
    CanonicalForm& operator *= ( const CanonicalForm& );
    CanonicalForm& operator /= ( const CanonicalForm& );
    CanonicalForm& operator %= ( const CanonicalForm& );
    CanonicalForm& div ( const CanonicalForm& );
    CanonicalForm& mod ( const CanonicalForm& );

    friend CanonicalForm operator - ( const CanonicalForm& );

    friend CanonicalForm operator + ( const CanonicalForm&, const CanonicalForm& );
    friend CanonicalForm operator - ( const CanonicalForm&, const CanonicalForm& );
    friend CanonicalForm operator * ( const CanonicalForm&, const CanonicalForm& );
    friend CanonicalForm operator / ( const CanonicalForm&, const CanonicalForm& );
    friend CanonicalForm operator % ( const CanonicalForm&, const CanonicalForm& );

    friend CanonicalForm div ( const CanonicalForm&, const CanonicalForm& );
    friend CanonicalForm mod ( const CanonicalForm&, const CanonicalForm& );

    friend void divrem ( const CanonicalForm&, const CanonicalForm&, CanonicalForm&, CanonicalForm& );
    friend bool divremt ( const CanonicalForm&, const CanonicalForm&, CanonicalForm&, CanonicalForm& );

#ifndef NOSTREAMIO
    void print( ostream&, char * ) const;
    friend ostream& operator << ( ostream&, const CanonicalForm& );
    friend istream& operator >> ( istream&, CanonicalForm& );
#endif /* NOSTREAMIO */

    friend bool operator == ( const CanonicalForm&, const CanonicalForm& );
    friend bool operator != ( const CanonicalForm&, const CanonicalForm& );
    friend bool operator > ( const CanonicalForm&, const CanonicalForm& );
    friend bool operator < ( const CanonicalForm&, const CanonicalForm& );

    friend class CFIterator;
};

// some useful functions

inline CanonicalForm
lc ( const CanonicalForm & f ) { return f.lc(); }

inline CanonicalForm
LC ( const CanonicalForm & f ) { return f.LC(); }

inline CanonicalForm
LC ( const CanonicalForm & f, const Variable & v ) { return f.LC( v ); }

inline int
degree ( const CanonicalForm & f ) { return f.degree(); }

inline int
degree ( const CanonicalForm & f, const Variable & v ) { return f.degree( v ); }

int totaldegree ( const CanonicalForm & f );

int totaldegree ( const CanonicalForm & f, const Variable & v1, const Variable & v2 );

inline int
taildegree ( const CanonicalForm & f ) { return f.taildegree(); }

inline CanonicalForm
tailcoeff ( const CanonicalForm & f ) { return f.tailcoeff(); }

CanonicalForm power ( const CanonicalForm & f, int n );

CanonicalForm power ( const Variable & v, int n );

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

inline int
level ( const CanonicalForm & f ) { return f.level(); }

inline CanonicalForm
mvar ( const CanonicalForm & f ) { return f.mvar(); }

inline CanonicalForm
num ( const CanonicalForm & f ) { return f.num(); }

inline CanonicalForm
den ( const CanonicalForm & f ) { return f.den(); }

inline CanonicalForm
mapinto ( const CanonicalForm & f ) { return f.mapinto(); }

CanonicalForm swapvar ( const CanonicalForm&, const Variable&, const Variable& );

CanonicalForm resultant( const CanonicalForm & f, const CanonicalForm& g, const Variable & x );

CanonicalForm gcd ( const CanonicalForm&, const CanonicalForm& );

CanonicalForm extgcd ( const CanonicalForm&, const CanonicalForm&, CanonicalForm&, CanonicalForm& );

CanonicalForm iextgcd ( const CanonicalForm&, const CanonicalForm&, CanonicalForm&, CanonicalForm& );

CanonicalForm lcm ( const CanonicalForm&, const CanonicalForm& );

CanonicalForm common_den ( const CanonicalForm& );

CanonicalForm pp ( const CanonicalForm& );

CanonicalForm content ( const CanonicalForm& );

CanonicalForm content ( const CanonicalForm&, const Variable& );

CanonicalForm icontent ( const CanonicalForm & f );

CanonicalForm apply ( const CanonicalForm & f, void (*mf)( CanonicalForm &, int & ) );

CanonicalForm mapdomain ( const CanonicalForm & f, CanonicalForm (*mf)( const CanonicalForm & ) );

bool divides ( const CanonicalForm & f, const CanonicalForm & g );

inline CanonicalForm
deriv ( const CanonicalForm & f, const Variable & x )
{
    return f.deriv(x);
}

inline CanonicalForm
abs ( const CanonicalForm & f )
{
    if ( f < 0 )
	return -f;
    else
	return f;
}

//{{{ inline CanonicalForm sqrt ( const CanonicalForm & a )
//{{{ docu
//
// sqrt() - inline function version of method sqrt().
//
//}}}
inline CanonicalForm
sqrt ( const CanonicalForm & a )
{
    return a.sqrt();
}
//}}}

//{{{ inline int ilog2 ( const CanonicalForm & a )
//{{{ docu
//
// ilog2() - inline function version of method ilog().
//
//}}}
inline int
ilog2 ( const CanonicalForm & a )
{
    return a.ilog2();
}
//}}}

CanonicalForm psr ( const CanonicalForm & f, const CanonicalForm & g, const Variable & x  );

CanonicalForm psq ( const CanonicalForm & f, const CanonicalForm & g, const Variable & x );

void psqr ( const CanonicalForm & f, const CanonicalForm & g, CanonicalForm & q, CanonicalForm & r, const Variable & x );

int getNumVars( const CanonicalForm & f );
CanonicalForm getVars( const CanonicalForm & f );

int * degrees ( const CanonicalForm & f, int * degs = 0 );

//CanonicalForm norm( const CanonicalForm & );

//CanonicalForm igcd( const CanonicalForm & );

//CanonicalForm ff2z( const CanonicalForm & );
//CanonicalForm z2ff( const CanonicalForm & );

void setCharacteristic( int c ); // -> Fp && Q
void setCharacteristic( int c, int n ); // -> PrimePower
void setCharacteristic( int c, int n, char name ); // -> GF(q)

int getCharacteristic();
int getGFDegree();

CanonicalForm getGFGenerator();

// misc template stuff for inst file

typedef Factor<CanonicalForm> CFFactor;
typedef List<CFFactor> CFFList;
typedef ListIterator<CFFactor> CFFListIterator;
typedef List<CanonicalForm> CFList;
typedef ListIterator<CanonicalForm> CFListIterator;
typedef Array<CanonicalForm> CFArray;
typedef Matrix<CanonicalForm> CFMatrix;

void On( int );
void Off( int );
bool isOn( int );

/*ENDPUBLIC*/

#endif /* ! INCL_CANONICALFORM_H */

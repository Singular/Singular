/* emacs edit mode for this file is -*- C++ -*- */
/* $Id: factory.h,v 1.1 1997-11-27 21:34:34 stobbe Exp $ */

#ifndef INCL_FACTORY_H
#define INCL_FACTORY_H

//{{{ docu
//
// factory.template - template to generate `factory.h'.
//
// `factory.h' is the user interface to Factory.  Created
// automatically by `makeheader', it collects all important
// declarations from all important Factory header files into one
// overall header file leaving out all boring Factory internal
// stuff.  See `./bin/makeheader' for an explanation of the syntax
// of this file.
//
// Note: In this file the order of "includes" matters (since this
// are not real includes)!  In general, files at the end depend
// on files at the beginning.
//
// ---RS---
// This is the specialized version of factory.h for Windows NT
// ---RS---
//
//}}}

#include <factoryconf.h>

#ifndef NOSTREAMIO
#include <iostream.h>
#endif /* ! NOSTREAMIO */

#ifdef SINGULAR
extern "C" {
#include <gmp.h>
}
#endif /* SINGULAR */

#ifdef macintosh
#include <::templates:ftmpl_array.h>
#include <::templates:ftmpl_factor.h>
#include <::templates:ftmpl_list.h>
#include <::templates:ftmpl_matrix.h>
#else
#include <templates/ftmpl_array.h>
#include <templates/ftmpl_factor.h>
#include <templates/ftmpl_list.h>
#include <templates/ftmpl_matrix.h>
#endif

extern const char factoryVersion[];
extern const char factoryConfiguration[];

int cf_getPrime( int i );

int cf_getNumPrimes();

int cf_getSmallPrime( int i );

int cf_getNumSmallPrimes();

int cf_getBigPrime( int i );

int cf_getNumBigPrimes();

#define LEVELBASE -1000000
#define LEVELTRANS -500000
#define LEVELQUOT 1000000
#define LEVELEXPR 1000001

#define UndefinedDomain 32000
#define PrimePowerDomain 5
#define GaloisFieldDomain 4
#define FiniteFieldDomain 3
#define RationalDomain 2
#define IntegerDomain 1

//{{{ constants
//{{{ docu
//
// - factory switches.
//
//}}}
const int SW_RATIONAL = 0;
const int SW_QUOTIENT = 1;
const int SW_SYMMETRIC_FF = 2;
const int SW_BERLEKAMP = 3;
const int SW_FAC_USE_BIG_PRIMES = 4;
const int SW_FAC_QUADRATICLIFT = 5;
const int SW_USE_EZGCD = 6;
const int SW_USE_SPARSEMOD = 7;
//}}}



class CanonicalForm;

class Variable
{
private:
    int _level;
    Variable( int l, bool flag );
public:
    Variable() : _level(LEVELBASE) {}
    Variable( int l );
    Variable( char name );
    Variable( int l, char name );
    Variable( const Variable & v ) : _level(v._level) {}
    ~Variable() {};
    Variable& operator= ( const Variable & v )
    {
	_level = v._level;
	return *this;
    }
    int level() const { return _level; }
    char name() const;
    static Variable highest() { return Variable( LEVELQUOT-1 ); }
    Variable next() const { return Variable( _level+1 ); }
    friend bool operator == ( const Variable & lhs, const Variable & rhs )
    {
	return lhs._level == rhs._level;
    }
    friend bool operator != ( const Variable & lhs, const Variable & rhs )
    {
	return lhs._level != rhs._level;
    }
    friend bool operator > ( const Variable & lhs, const Variable & rhs )
    {
	return lhs._level > rhs._level;
    }
    friend bool operator < ( const Variable & lhs, const Variable & rhs )
    {
	return lhs._level < rhs._level;
    }
    friend bool operator >= ( const Variable & lhs, const Variable & rhs )
    {
	return lhs._level >= rhs._level;
    }
    friend bool operator <= ( const Variable & lhs, const Variable & rhs )
    {
	return lhs._level <= rhs._level;
    }
#ifndef NOSTREAMIO
    friend ostream & operator << ( ostream & os, const Variable & v );
#endif /* NOSTREAMIO */
    friend Variable rootOf( const CanonicalForm &, char name = '@' );
};

inline int level( const Variable & v ) { return v.level(); }
inline char name( const Variable & v ) { return v.name(); }

CanonicalForm getMipo( const Variable & alpha, const Variable & x );

char getDefaultVarName();
char getDefaultExtName();



int initCanonicalForm( void );

static int cf_is_initialized_now = initCanonicalForm();

class InternalCF;

//{{{ class CanonicalForm
class CanonicalForm
{
private:
    InternalCF *value;
public:
    // constructors, destructors, selectors
    CanonicalForm();
    CanonicalForm( const CanonicalForm& );
    CanonicalForm( InternalCF* );
    CanonicalForm( const int );
    CanonicalForm( const Variable & );
    CanonicalForm( const Variable &, int );
    CanonicalForm( const char * ); // use with caution - does only handle integers !!!

    ~CanonicalForm();

    InternalCF* getval() const; // use with caution !!!

    CanonicalForm deepCopy() const;

    // predicates
    bool isOne() const;
    bool isZero() const;
    bool isImm() const;

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

    bool isFFinGF() const;
    bool isUnivariate() const;

    // conversion functions
    int intval() const;
    CanonicalForm mapinto () const;

    CanonicalForm lc () const;
    CanonicalForm Lc () const;
    CanonicalForm LC () const;
    CanonicalForm LC ( const Variable & v ) const;

    int degree () const;
    int degree ( const Variable & v ) const;

    CanonicalForm tailcoeff () const;
    int taildegree () const;

    int level () const;
    Variable mvar () const;

    CanonicalForm num () const;
    CanonicalForm den () const;

    // assignment operators
    CanonicalForm& operator = ( const CanonicalForm& );
    CanonicalForm& operator = ( const int );

    CanonicalForm& operator += ( const CanonicalForm& );
    CanonicalForm& operator -= ( const CanonicalForm& );
    CanonicalForm& operator *= ( const CanonicalForm& );
    CanonicalForm& operator /= ( const CanonicalForm& );
    CanonicalForm& operator %= ( const CanonicalForm& );
    CanonicalForm& div ( const CanonicalForm& );
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
    bool hasGcd() const { return false; }
    CanonicalForm gcd( const CanonicalForm & f ) const;

    // comparison operators
    friend bool operator == ( const CanonicalForm&, const CanonicalForm& );
    friend bool operator != ( const CanonicalForm&, const CanonicalForm& );
    friend bool operator > ( const CanonicalForm&, const CanonicalForm& );
    friend bool operator < ( const CanonicalForm&, const CanonicalForm& );

    // arithmetic operators
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

    // obsolete methods
    static CanonicalForm genCoeff( int what, int i = 0 );
    CanonicalForm genZero() const;
    CanonicalForm genOne() const;

    friend class CFIterator;
};
//}}}

// some useful functions

//{{{ function declarations from canonicalform.cc
CanonicalForm power ( const CanonicalForm & f, int n );

CanonicalForm power ( const Variable & v, int n );

bool divides ( const CanonicalForm & f, const CanonicalForm & g );
//}}}

//{{{ function declarations from cf_gcd.cc
CanonicalForm gcd ( const CanonicalForm&, const CanonicalForm& );

CanonicalForm extgcd ( const CanonicalForm&, const CanonicalForm&, CanonicalForm&, CanonicalForm& );

CanonicalForm iextgcd ( const CanonicalForm&, const CanonicalForm&, CanonicalForm&, CanonicalForm& );

CanonicalForm lcm ( const CanonicalForm&, const CanonicalForm& );

CanonicalForm pp ( const CanonicalForm& );

CanonicalForm content ( const CanonicalForm& );

CanonicalForm content ( const CanonicalForm&, const Variable& );

CanonicalForm icontent ( const CanonicalForm & f );
//}}}

//{{{ function declarations from cf_ops.cc
CanonicalForm swapvar ( const CanonicalForm &, const Variable &, const Variable & );

CanonicalForm replacevar ( const CanonicalForm &, const Variable &, const Variable & );

int getNumVars( const CanonicalForm & f );

CanonicalForm getVars( const CanonicalForm & f );

CanonicalForm apply ( const CanonicalForm & f, void (*mf)( CanonicalForm &, int & ) );

CanonicalForm mapdomain ( const CanonicalForm & f, CanonicalForm (*mf)( const CanonicalForm & ) );

int * degrees ( const CanonicalForm & f, int * degs = 0 );

int totaldegree ( const CanonicalForm & f );

int totaldegree ( const CanonicalForm & f, const Variable & v1, const Variable & v2 );

int size ( const CanonicalForm & f, const Variable & v );

int size ( const CanonicalForm & f );
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

inline int
level ( const CanonicalForm & f ) { return f.level(); }

inline CanonicalForm
mvar ( const CanonicalForm & f ) { return f.mvar(); }

inline CanonicalForm
num ( const CanonicalForm & f ) { return f.num(); }

inline CanonicalForm
den ( const CanonicalForm & f ) { return f.den(); }

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

inline CanonicalForm
abs ( const CanonicalForm & f )
{
    if ( f < 0 )
	return -f;
    else
	return f;
}

inline int
headdegree ( const CanonicalForm & f ) { return totaldegree( head( f ) ); }
//}}}

//{{{ other function declarations
void setCharacteristic( int c ); // -> Fp && Q
void setCharacteristic( int c, int n ); // -> PrimePower
void setCharacteristic( int c, int n, char name ); // -> GF(q)

int getCharacteristic();
int getGFDegree();
CanonicalForm getGFGenerator();

void On( int );
void Off( int );
bool isOn( int );
//}}}

//{{{ type definitions
typedef Factor<CanonicalForm> CFFactor;
typedef List<CFFactor> CFFList;
typedef ListIterator<CFFactor> CFFListIterator;
typedef List<CanonicalForm> CFList;
typedef ListIterator<CanonicalForm> CFListIterator;
typedef Array<CanonicalForm> CFArray;
typedef Matrix<CanonicalForm> CFMatrix;
//}}}


CanonicalForm binomialpower ( const Variable&, const CanonicalForm&, int );

//{{{ function declarations from cf_algorithm.cc
CanonicalForm psr ( const CanonicalForm & f, const CanonicalForm & g, const Variable & x );

CanonicalForm psq ( const CanonicalForm & f, const CanonicalForm & g, const Variable & x );

void psqr ( const CanonicalForm & f, const CanonicalForm & g, CanonicalForm & q, CanonicalForm & r, const Variable & x );

CanonicalForm common_den ( const CanonicalForm & f );
//}}}

//{{{ function declarations from cf_chinese.cc
void chineseRemainder ( const CanonicalForm & x1, const CanonicalForm & q1, const CanonicalForm & x2, const CanonicalForm & q2, CanonicalForm & xnew, CanonicalForm & qnew );

void chineseRemainder ( const CFArray & x, const CFArray & q, CanonicalForm & xnew, CanonicalForm & qnew );
//}}}

//{{{ function declarations from cf_factor.cc
CFFList factorize ( const CanonicalForm & f, bool issqrfree = false );

CFFList factorize ( const CanonicalForm & f, const Variable & alpha );

CFFList sqrFree ( const CanonicalForm & f, bool sort = false );

bool isSqrFree ( const CanonicalForm & f );
//}}}

//{{{ function declarations from cf_linsys.cc
bool linearSystemSolve ( CFMatrix & M );

CanonicalForm determinant ( const CFMatrix & M, int n );
//}}}

//{{{ function declarations from cf_resultant.cc
CFArray subResChain ( const CanonicalForm & f, const CanonicalForm & g, const Variable & x );

CanonicalForm resultant ( const CanonicalForm & f, const CanonicalForm & g, const Variable & x );
//}}}



class Evaluation
{
protected:
    CFArray values;
public:
    Evaluation() : values() {}
    Evaluation( int min, int max ) : values( min, max ) {}
    Evaluation( const Evaluation & e ) : values( e.values ) {}
    virtual ~Evaluation() {}
    Evaluation& operator= ( const Evaluation & e );
    int min() const { return values.min(); }
    int max() const { return values.max(); }
    CanonicalForm operator[] ( int i ) const { return values[i]; }
    CanonicalForm operator[] ( const Variable & v ) const { return operator[](v.level()); }
    CanonicalForm operator() ( const CanonicalForm& f ) const;
    CanonicalForm operator() ( const CanonicalForm & f, int i, int j ) const;
    virtual void nextpoint();
#ifndef NOSTREAMIO
    friend ostream& operator<< ( ostream& s, const Evaluation &e );
#endif /* NOSTREAMIO */
};



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

class AlgExtGenerator : public CFGenerator
{
private:
    Variable algext;
    CFGenerator **gens;
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


class term;
typedef term * termList;

class CFIterator {
private:
    CanonicalForm data;
    termList cursor;
    bool ispoly, hasterms;
public:
    CFIterator ();
    CFIterator ( const CFIterator& );
    CFIterator ( const CanonicalForm& );
    CFIterator ( const CanonicalForm&, const Variable& );

    ~CFIterator ();

    CFIterator& operator= ( const CFIterator& );
    CFIterator& operator= ( const CanonicalForm& );

    CFIterator& operator++ ();
    CFIterator& operator++ ( int );
    int hasTerms () const;
    CanonicalForm coeff () const;
    int exp () const;
};




class CFRandom {
public:
    virtual ~CFRandom() {}
    virtual CanonicalForm generate() const { return 0; }
    virtual CFRandom * clone() const { return new CFRandom(); }
};

class GFRandom : public CFRandom
{
public:
    GFRandom() {};
    ~GFRandom() {}
    CanonicalForm generate() const;
    CFRandom * clone() const;
};

class FFRandom : public CFRandom
{
public:
    FFRandom() {}
    ~FFRandom() {}
    CanonicalForm generate() const;
    CFRandom * clone() const;
};

class IntRandom : public CFRandom
{
private:
    int max;
public:
    IntRandom();
    IntRandom( int m );
    ~IntRandom();
    CanonicalForm generate() const;
    CFRandom * clone() const;
};

class AlgExtRandomF : public CFRandom {
private:
    Variable algext;
    CFRandom * gen;
    int n;
    AlgExtRandomF();
    AlgExtRandomF( const AlgExtRandomF & );
    AlgExtRandomF( const Variable & v, CFRandom * g, int nn );
    AlgExtRandomF& operator= ( const AlgExtRandomF & );
public:
    AlgExtRandomF( const Variable & v );
    AlgExtRandomF( const Variable & v1, const Variable & v2 );
    ~AlgExtRandomF();
    CanonicalForm generate() const;
    CFRandom * clone() const;
};

class CFRandomFactory {
public:
    static CFRandom * generate();
};

int factoryrandom( int n );

void factoryseed( int s );



CanonicalForm find_irreducible ( int deg, CFRandom & gen, const Variable & x );



class modpk
{
private:
    CanonicalForm pk;
    CanonicalForm pkhalf;
    int p;
    int k;
public:
    modpk();
    modpk( int q, int l );
    modpk( const modpk & m );
    modpk& operator= ( const modpk& m );
    ~modpk() {}
    int getp() const { return p; }
    int getk() const { return k; }
    CanonicalForm inverse( const CanonicalForm & f, bool symmetric = true ) const;
    CanonicalForm getpk() const { return pk; }
    CanonicalForm operator() ( const CanonicalForm & f, bool symmetric = true ) const;
};


CanonicalForm replaceLc( const CanonicalForm & f, const CanonicalForm & c );

CanonicalForm remainder( const CanonicalForm & f, const CanonicalForm & g, const modpk & pk );

void divremainder( const CanonicalForm & f, const CanonicalForm & g, CanonicalForm & quot, CanonicalForm & rem, const modpk & pk );

CanonicalForm maxCoeff( const CanonicalForm & f );

bool Hensel ( const CanonicalForm & U, CFArray & G, const CFArray & lcG, const Evaluation & A, const modpk & bound, const Variable & x );

/* some special array functions */

CanonicalForm sum ( const CFArray & a, int f, int l );

CanonicalForm prod ( const CFArray & a, int f, int l );

CanonicalForm sum ( const CFArray & a );

CanonicalForm prod ( const CFArray & a );

CanonicalForm crossprod ( const CFArray & a, const CFArray & b );


//{{{ class MapPair
//{{{ docu
//
// class MapPair - stores one mapping pair (Variable -> CanonicalForm).
//
// This class is only used to store such pairs.  It has no
// methods to transform a CanonicalForm as the class CFMap has.
//
// V, S: the pair (V -> S)
//
//}}}
//{{{ inline method docu
//
// Variable var () const
// CanonicalForm subst () const
//
// var(), subst() - selectors, return V and P, resp.
//
//}}}
class MapPair
{
private:
    Variable V;
    CanonicalForm S;
public:
    MapPair ( const Variable & v, const CanonicalForm & s ) : V(v), S(s) {}
    MapPair () : V(), S(1) {}
    MapPair ( const MapPair & p ) : V(p.V), S(p.S) {}
    ~MapPair () {}
    MapPair & operator = ( const MapPair & p );
    Variable var () const { return V; }
    CanonicalForm subst () const { return S; }
#ifndef NOSTREAMIO
    friend ostream & operator << ( ostream & s, const MapPair & p );
#endif /* NOSTREAMIO */
};
//}}}

typedef List<MapPair> MPList;
typedef ListIterator<MapPair> MPListIterator;

//{{{ class CFMap
//{{{ docu
//
// class CFMap - class to map canonical forms.
//
// Use an object of class CFMap to insert 'values' into canonical
// form.  Such a mapping is defined by a list of MapPairs (V -> S)
// describing which canonical form S to insert for variable V.
// Hereby, the substituted canonical forms are not subject to
// further substitutions.
//
// P: list of MapPairs, sorted by level in descending order
//
//}}}
class CFMap
{
private:
  MPList P;
public:
  CFMap () {}
  CFMap ( const CanonicalForm & s ) : P( MapPair( Variable(), s ) ) {}
  CFMap ( const Variable & v ) : P( MapPair( v, 1 ) ) {}
  CFMap ( const Variable & v, const CanonicalForm & s ) : P( MapPair( v, s ) ) {}
  ~CFMap () {}
  CFMap ( const CFList & L );
  CFMap ( const CFMap & m ) : P( m.P ) {}
  CFMap & operator = ( const CFMap & m );
  void newpair ( const Variable & v, const CanonicalForm & s );
  CanonicalForm operator () ( const CanonicalForm & f ) const;
#ifndef NOSTREAMIO
  friend ostream & operator << ( ostream & s, const CFMap & m );
#endif /* NOSTREAMIO */
};
//}}}

CanonicalForm compress ( const CanonicalForm & f, CFMap & m );
void compress ( const CFArray & a, CFMap & M, CFMap & N );
void compress ( const CanonicalForm & f, const CanonicalForm & g, CFMap & M, CFMap & N );




class REvaluation : public Evaluation
{
private:
    CFRandom * gen;
public:
    REvaluation() : Evaluation(), gen(0) {}
    REvaluation( int min, int max, const CFRandom & sample ) : Evaluation( min, max ), gen( sample.clone() ) {}
    REvaluation( const REvaluation & e );
    ~REvaluation();
    REvaluation& operator= ( const REvaluation & e );
    void nextpoint();
};


#ifdef SINGULAR


int gf_gf2ff ( int a );

bool gf_isff ( int a );



MP_INT gmp_numerator ( const CanonicalForm & f );

MP_INT gmp_denominator ( const CanonicalForm & f );

CanonicalForm make_cf ( const MP_INT & n );

CanonicalForm make_cf ( const MP_INT & n, const MP_INT & d, bool normalize );

#endif /* SINGULAR */

#endif /* ! INCL_FACTORY_H */

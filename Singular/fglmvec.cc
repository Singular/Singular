// emacs edit mode for this file is -*- C++ -*-
// $Id: fglmvec.cc,v 1.1.1.1 1997-03-19 13:18:45 obachman Exp $

#include <assert.h>
#include <counted.h>
#include <fglmvec.h>
#include "fglm.h"
#include "numbers.h"

class fglmVectorRep : public CountedObject
{
private:
    int N;
    number * elems;
public:
    fglmVectorRep() : N(0), elems(0) {}
    fglmVectorRep( int n, number * e ) : N(n), elems(e) {}
    fglmVectorRep( int n ) : N(n) 
    {
	ASSERT( N >= 0, "illegal Vector representation" );
	if ( N == 0 ) 
	    elems= 0;
	else {
	    elems= (number *)Alloc( N*sizeof( number ) );
	    for ( int i= N-1; i >= 0; i-- )
		elems[i]= nInit( 0 );
	}
    }
    ~fglmVectorRep()
    {
	for ( int i= N-1; i>=0; i-- )
	    nDelete( elems + i );
	Free( (ADDRESS)elems, N*sizeof( number ) );
    }

    fglmVectorRep* clone() const
    {
	if ( N > 0 ) {
	    number * elems_clone;
	    elems_clone= (number *)Alloc( N*sizeof( number ) );
	    for ( int i= N-1; i >= 0; i-- )
		elems_clone[i] = nCopy( elems[i] );
	    return new fglmVectorRep( N, elems_clone );
	} else
	    return new fglmVectorRep( N, 0 );
    }
    int size() const { return N; }
    int numNonZeroElems() const 
    {
	int num = 0;
	for ( int k = N; k > 0; k-- )
	    if ( ! nIsZero( getconstelem(k) ) ) num++;
	return num;
    }
    void setelem( int i, number n )
    {
	nDelete( elems + i-1 );
	elems[i-1]= n;
    }
    number & getelem( int i )
    {
	return elems[i-1];
    }
    const number getconstelem( int i) const
    {
	return elems[i-1];
    }
    void print( ostream & s ) const
    {
	if ( N == 0 )
	    s << "( )";
	else {
	    s << "( ";
	    StringSet( "" );
	    nWrite( elems[0] );
	    s << StringAppend( "" );
	    for ( int i = 1; i < N; i++ ) {
		s << ", ";
		StringSet( "" );
		nWrite( elems[i] );
		s << StringAppend( "" );
	    }
	    s << " )";
	}
	s << "[" << refcount() << "]";
    }
    friend class fglmVector;
    friend class subFglmVector;
};


///--------------------------------------------------------------------------------
/// Implementation of class fglmVector
///--------------------------------------------------------------------------------

fglmVector::fglmVector( fglmVectorRep * r ) : rep(r) {}

fglmVector::fglmVector() : rep( new fglmVectorRep() ) {}

fglmVector::fglmVector( int size ) : rep( new fglmVectorRep( size ) ) {}

fglmVector::fglmVector( int size, int basis ) : rep( new fglmVectorRep( size ) )
{
    rep->setelem( basis, nInit( 1 ) );
}

fglmVector::fglmVector( const fglmVector & v )
{
    rep= (fglmVectorRep*)v.rep->copyObject();
}

fglmVector::~fglmVector()
{
    if ( rep->deleteObject() )
	delete rep;
}

void 
fglmVector::makeUnique()
{
    if ( rep->refcount() != 1 ) {
	rep->deleteObject();
	rep= rep->clone();
    }
}

int 
fglmVector::size() const
{
    return rep->size();
}

int
fglmVector::numNonZeroElems() const
{
    return rep->numNonZeroElems();
}

fglmVector & 
fglmVector::operator = ( const fglmVector & v )
{
    if ( this != &v ) {
	if ( rep->deleteObject() )
	    delete rep;
	rep = (fglmVectorRep*)v.rep->copyObject();
    }
    return *this;
}

subFglmVector 
fglmVector::operator () ( int min, int max )
{
    ASSERT( min > 0 && max >= min && max <= rep->size(), "illegal index" );
    makeUnique();
    return subFglmVector( min, max, rep );
}

const subFglmVector 
fglmVector::operator () ( int min, int max ) const
{
    ASSERT( min > 0 && max >= min && max <= rep->size(), "illegal index" );
    return subFglmVector( min, max, rep );
}

int
fglmVector::operator == ( const fglmVector & v )
{
    if ( rep->size() == v.rep->size() ) {
	if ( rep == v.rep ) return 1;
	else {
	    int i;
	    for ( i= rep->size(); i > 0; i-- )
		if ( ! nEqual( rep->getconstelem( i ), v.rep->getconstelem( i ) ) )
		    return 0;
	    return 1;
	}
    }
    return 0;
}

int
fglmVector::operator != ( const fglmVector & v )
{
    return !( *this == v );
}

int 
fglmVector::isZero()
{
    int i;
    for ( i= rep->size(); i > 0; i-- )
	if ( ! nIsZero( rep->getconstelem( i ) ) )
	    return 0;
    return 1;
}

fglmVector &
fglmVector::operator += ( const fglmVector & v )
{
    ASSERT( size() == v.size(), "incompatible vectors" );
    // ACHTUNG : Das Verhalten hier mit gcd genau ueberpruefen!
    int i;
    if ( rep->isUnique() ) {
	for ( i= rep->size(); i > 0; i-- ) 
	    rep->setelem( i, nAdd( rep->getconstelem( i ), v.rep->getconstelem( i ) ) );
    }
    else 
    {
	int n = rep->size();
	number* newelems;
	newelems= (number *)Alloc( n*sizeof( number ) );
	for ( i= n; i > 0; i-- )
	    newelems[i-1]= nAdd( rep->getconstelem( i ), v.rep->getconstelem( i ) );
	rep->deleteObject();
	rep= new fglmVectorRep( n, newelems );
    }
    return *this;
}

fglmVector &
fglmVector::operator -= ( const fglmVector & v )
{
    ASSERT( size() == v.size(), "incompatible vectors" );
    int i;
    if ( rep->isUnique() ) {
	for ( i= rep->size(); i > 0; i-- ) 
	    rep->setelem( i, nSub( rep->getconstelem( i ), v.rep->getconstelem( i ) ) );
    }
    else 
    {
	int n = rep->size();
	number* newelems;
	newelems= (number *)Alloc( n*sizeof( number ) );
	for ( i= n; i > 0; i-- )
	    newelems[i-1]= nSub( rep->getconstelem( i ), v.rep->getconstelem( i ) );
	rep->deleteObject();
	rep= new fglmVectorRep( n, newelems );
    }
    return *this;
}

fglmVector &
fglmVector::operator *= ( const number & n )
{
    int s= rep->size();
    int i;
    if ( ! rep->isUnique() ) {
	number * temp;
	temp= (number *)Alloc( s*sizeof( number ) );
	for ( i= s; i > 0; i-- ) 
	    temp[i-1]= nMult( rep->getconstelem( i ), n );
	rep->deleteObject();
	rep= new fglmVectorRep( s, temp );
    }
    else 
    {
	for (i= s; i > 0; i-- ) 
	    rep->setelem( i, nMult( rep->getconstelem( i ), n ) );
    }
    return *this;
}

fglmVector &
fglmVector::operator /= ( const number & n )
{
    int s= rep->size();
    int i;
    if ( ! rep->isUnique() ) {
	number * temp;
	temp= (number *)Alloc( s*sizeof( number ) );
	for ( i= s; i > 0; i-- ) {
	    temp[i-1]= nDiv( rep->getconstelem( i ), n );
	    nNormalize( temp[i-1] );
	}
	rep->deleteObject();
	rep= new fglmVectorRep( s, temp );
    }
    else 
    {
	for (i= s; i > 0; i-- ) {
	    rep->setelem( i, nDiv( rep->getconstelem( i ), n ) );
	    nNormalize( rep->getelem( i ) );
	}
    }
    return *this;
}

fglmVector &
fglmVector::operator *= ( const Mat & M ) 
{
    return *this;
}

fglmVector 
operator - ( const fglmVector & v ) 
{
    fglmVector temp( v.size() );
    int i;
    number n ;
    for ( i= v.size(); i > 0; i-- ) {
	n= nCopy( v.getconstelem( i ) );
	n= nNeg( n );
	temp.setelem( i, n );
    }
    return temp;
}

fglmVector 
operator + ( const fglmVector & lhs, const fglmVector & rhs ) 
{
    fglmVector temp= lhs;
    temp+= rhs;
    return temp;
}

fglmVector 
operator - ( const fglmVector & lhs, const fglmVector & rhs )
{
    fglmVector temp= lhs;
    temp-= rhs;
    return temp;
}

fglmVector 
operator * ( const fglmVector & v, const number n )
{
    fglmVector temp= v;
    temp*= n;
    return temp;
}

fglmVector
operator * ( const number n, const fglmVector & v )
{
    fglmVector temp= v;
    temp*= n;
    return temp;
}

fglmVector
operator * ( const Mat & M, const fglmVector & v )
{
    fglmVector temp= v;
    temp*= M;
    return temp;
}

number &
fglmVector::getelem( int i )
{
    makeUnique();
    return rep->getelem( i );
}

const number 
fglmVector::getconstelem( int i ) const
{
    return rep->getconstelem( i );
}

void
fglmVector::setelem( int i, number & n )
{
    makeUnique();
    rep->setelem( i, n );
    n= nNULL;
}

number 
fglmVector::gcd() const
{ 
    int i= rep->size();
    bool found = false;
    bool gcdIsOne = false;
    number theGcd;
    number current;
    //. einen Eintrag ungleich Null finden:
    while( i > 0 && ! found ) {
	current= rep->getconstelem( i );
	if ( ! nIsZero( current ) ) {
	    theGcd= nCopy( current );
	    found= true;
	    //. positiv machen und ueberpruefen, ob gcd=1:
	    if ( ! nGreaterZero( theGcd ) ) {
		theGcd= nNeg( theGcd );
	    }
	    if ( nIsOne( theGcd ) ) gcdIsOne= true;
	}
	i--;  //. i auf jeden Fall runterzaehlen.
    }
    if ( found ) {
	while ( i > 0 && ! gcdIsOne ) {
	    current= rep->getconstelem( i );
	    if ( ! nIsZero( current ) ) {
		// Hier gehe ich davon aus, dass nGcd seine Argumente nicht loescht!
		number temp= nGcd( theGcd, current );
		nDelete( &theGcd );
		theGcd= temp;
		if ( nIsOne( theGcd ) ) gcdIsOne= true;
	    }
	    i--;
	}
    }
    else 
	theGcd= nInit( 0 );
    return theGcd;
}

number
fglmVector::clearDenom() 
{
    number theLcm = nInit( 1 );
    number current;
    bool isZero = true;
    int i;
    for ( i= size(); i > 0; i-- ) {
//	current= rep->getconstelem( i );
//	if ( ! nIsZero( current ) ) {
	if ( ! nIsZero( rep->getconstelem(i) ) ) {
	    isZero= false;
//	    number temp= nLcm( theLcm, current );
	    number temp= nLcm( theLcm, rep->getconstelem( i ) );
	    nDelete( &theLcm );
	    theLcm= temp;
	}
    }
    if ( isZero ) {
	nDelete( &theLcm );
	theLcm= nInit( 0 );
    }
    else {
	if ( ! nIsOne( theLcm ) ) {
	    *this *= theLcm;
	    for ( i= size(); i > 0; i-- ) {
		nNormalize( rep->getelem( i ) );
	    }
	}
    }
    return theLcm;
}

void 
fglmVector::print( ostream & s ) const
{
    rep->print( s );
}

#ifdef FGLM_DEBUG
void
fglmVector::test() const
{
    int i;
    BOOLEAN dummy;
    for ( i= 1; i <= size(); i++ ) {
	dummy= nTest( rep->getconstelem( i ) );
    }
}
#endif

///--------------------------------------------------------------------------------
/// Implementation of class subFglmVector
///--------------------------------------------------------------------------------

subFglmVector::subFglmVector( int min, int max, fglmVectorRep * repr ) : minelem(min), maxelem(max), rep(repr) {}

subFglmVector::subFglmVector( const subFglmVector & v ) : minelem(v.minelem), maxelem(v.maxelem), rep(v.rep) {}

subFglmVector & 
subFglmVector::operator = ( const subFglmVector & v )
{
    if ( this != &v ) {
	ASSERT( maxelem-minelem == v.maxelem-v.minelem, "incompatible subFglmVectors" );
	int n = maxelem - minelem;
	int i;
	if ( rep == v.rep ) {
	    // --> copy blocks of the same fglmVector
	    if ( minelem < v.minelem )
		for ( i = 0; i <= n; i++ ) 
		    rep->setelem( minelem + i, nCopy( rep->getconstelem( v.minelem + i ) ) );
	    else if ( minelem > v.minelem )
		for ( i = n; i >= 0; i-- ) 
		    rep->setelem( minelem + i, nCopy( rep->getconstelem( v.minelem + i ) ) );
	}    
	else {
	    for ( i = 0; i <= n; i++ ) 
		rep->setelem( minelem + i , nCopy( v.rep->getconstelem( v.minelem + i ) ) );
	}
    }
    return *this;
}

subFglmVector & 
subFglmVector::operator = ( const fglmVector & v )
{
    number newelem;
    ASSERT( maxelem-minelem+1 == v.size(), "incompatible subFglmVectors" );
    if ( rep != v.rep ) {
	int n = v.size();
	int i;
	for ( i = 0; i < n; i++ ) 
	    rep->setelem( minelem + i, nCopy( v.rep->getconstelem( 1 + i ) ) );
    }
    return *this;
}

// T & 
// subFglmVector::operator[] ( int i )
// {
//     ASSERT( minelem <= i && i <= maxelem, "illegal index" );
//     return rep->elem( i );
// }

// const number & 
// subFglmVector::operator [] ( int i ) const
// {
//     return rep->getelem( i );
// }    

subFglmVector &
subFglmVector::operator += ( const fglmVector & v )
{
    ASSERT( maxelem-minelem+1 == v.size(), "incompatible Vectors" );
    ASSERT( rep->isUnique(), "this should never happen" );
    int i;
    for ( i= v.size()-1; i >= 0; i-- )
	rep->setelem( minelem + i, nAdd( rep->getconstelem( minelem + i ), v.rep->getconstelem( 1 + i ) ) );
    return *this;
}

subFglmVector &
subFglmVector::operator += ( const subFglmVector & v )
{
    ASSERT( maxelem-minelem == v.maxelem-v.minelem, "incompatible SubVectors" );
    ASSERT( rep->isUnique(), "this should never happen" );
    int i;
    int n= maxelem-minelem;
    if ( rep == v.rep ) {
	if ( minelem >= v.minelem )
	    for ( i= n; i >= 0; i-- ) 
		rep->setelem( minelem + i, nAdd( rep->getconstelem( minelem + i ), rep->getconstelem( v.minelem + i ) ) );
	else 
	    for ( i= 0; i<= n; i++ ) 
		rep->setelem( minelem + i, nAdd( rep->getconstelem( minelem + i ), rep->getconstelem( v.minelem + i ) ) );
    }
    else
	for ( i= maxelem-minelem; i >= 0; i-- )
	    rep->setelem( minelem + i, nAdd( rep->getconstelem( minelem + i ), v.rep->getconstelem( v.minelem + i ) ) );
    return *this;
}

subFglmVector &
subFglmVector::operator -= ( const fglmVector & v )
{
    ASSERT( maxelem-minelem+1 == v.size(), "incompaitble Vectors" );
    ASSERT( rep->isUnique(), "this should never happen" );
    int i;
    for ( i= v.size()-1; i >= 0; i-- ) 
	rep->setelem( minelem + i, nSub( rep->getconstelem( minelem + i ), v.rep->getconstelem( 1 + i ) ) );
    return *this;
}

subFglmVector &
subFglmVector::operator -= ( const subFglmVector & v )
{
    ASSERT( maxelem-minelem == v.maxelem-v.minelem, "incompatible SubVectors" );
    ASSERT( rep->isUnique(), "this should never happen" );
    int i;
    int n = maxelem - minelem;
    if ( rep == v.rep ) {
	if ( minelem == v.minelem )
	    for ( i= n; i >= 0; i-- ) rep->setelem( minelem + i, nInit(0) );
	else if ( minelem > v.minelem )
	    for ( i= n; i >= 0; i-- ) 
		rep->setelem( minelem + i, nSub( rep->getconstelem( minelem + i ), rep->getconstelem( v.minelem + i ) ) );
	else 
	    for ( i= 0; i<= n; i++ ) 
		rep->setelem( minelem + i, nSub( rep->getconstelem( minelem + i ), rep->getconstelem( v.minelem + i ) ) );
    }
    else
	for ( i= n; i >= 0; i-- )
	    rep->setelem( minelem + i, nSub( rep->getconstelem( minelem + i ), v.rep->getconstelem( minelem + i ) ) );
    return *this;
}

subFglmVector&
subFglmVector::operator *= ( const number & n )
{
    ASSERT( rep->isUnique(), "this should never happen" );
    int i;
    for ( i= minelem; i <= maxelem; i++ )
	rep->setelem( i, nMult( rep->getconstelem( i ), n ) );
    return *this;
}

subFglmVector::operator fglmVector() const
{
    if ( minelem == 1 && maxelem == rep->size() )
	return fglmVector( (fglmVectorRep*)rep->copyObject() );
    else {
	int n = maxelem-minelem+1;
	int i;
	number * newelems;
	newelems= (number *)Alloc( n*sizeof( number ) );

	for ( i= 0; i < n; i++ )
	    newelems[i] = nCopy( rep->getconstelem( minelem + i ) );
	return fglmVector( new fglmVectorRep( n, newelems ) );
    }
}



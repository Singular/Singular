/// BigInt.h

/// A BigInt-class with not much more functions than needed by the
/// LLL-algorithm. Wrapper class for the GNU MP library.

#ifndef BIG_INT_H
#define BIG_INT_H

#include <stdlib.h>
#include <float.h>
#include <math.h>
#include <ctype.h>
#include <string.h>
#include <stddef.h>
#include <gmp.h>

class BigInt
{
    mpz_t value;

    public:

    BigInt( );
    BigInt( int );
    BigInt( const BigInt& );
    ~BigInt( );

    BigInt& operator = ( int );
    BigInt& operator = ( const BigInt& );

    operator bool( );
    operator int( );
    operator short( );

    BigInt  operator - ( );
    BigInt& operator += ( const BigInt& );
    BigInt& operator -= ( const BigInt& );
    BigInt& operator *= ( const BigInt& );
    BigInt& operator /= ( const BigInt& );
    BigInt& operator ++ ( );
    BigInt  operator ++ ( int );
    BigInt& operator -- ( );
    BigInt  operator -- ( int );

    friend BigInt operator - ( const BigInt& );

    friend bool operator <  ( const BigInt&, const BigInt& );
    friend bool operator <= ( const BigInt&, const BigInt& );
    friend bool operator >  ( const BigInt&, const BigInt& );
    friend bool operator >= ( const BigInt&, const BigInt& );
    friend bool operator == ( const BigInt&, const BigInt& );
    friend bool operator != ( const BigInt&, const BigInt& );
    friend bool operator <  ( const int&, const BigInt& );
    friend bool operator <= ( const int&, const BigInt& );
    friend bool operator >  ( const int&, const BigInt& );
    friend bool operator >= ( const int&, const BigInt& );
    friend bool operator == ( const int&, const BigInt& );
    friend bool operator != ( const int&, const BigInt& );
    friend bool operator <  ( const BigInt&, const int& );
    friend bool operator <= ( const BigInt&, const int& );
    friend bool operator >  ( const BigInt&, const int& );
    friend bool operator >= ( const BigInt&, const int& );
    friend bool operator == ( const BigInt&, const int& );
    friend bool operator != ( const BigInt&, const int& );

    friend int    sgn ( const BigInt& );
    friend BigInt abs ( const BigInt& );
};

BigInt operator + ( const BigInt&, const BigInt& );
BigInt operator - ( const BigInt&, const BigInt& );
BigInt operator * ( const BigInt&, const BigInt& );
BigInt operator / ( const BigInt&, const BigInt& );
BigInt operator + ( const int&, const BigInt& );
BigInt operator - ( const int&, const BigInt& );
BigInt operator * ( const int&, const BigInt& );
BigInt operator / ( const int&, const BigInt& );
BigInt operator + ( const BigInt&, const int& );
BigInt operator - ( const BigInt&, const int& );
BigInt operator * ( const BigInt&, const int& );
BigInt operator / ( const BigInt&, const int& );

#endif  /// BIG_INT_H

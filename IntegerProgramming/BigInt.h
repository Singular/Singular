// BigInt.h

// A BigInt-class with not much more functions than needed by the 
// LLL-algorithm. Wrapper class for the GNU MP library.

#ifndef BIG_INT_H
#define BIG_INT_H

#include <stdlib.h>
#include <float.h>
#include <math.h>
#include <ctype.h>
#include <string.h>
#include <si_gmp.h>

class BigInt
{
    mpz_t value;

    public:

    BigInt( );
    BigInt( long int );
    BigInt( unsigned long int );
    BigInt( int );
    BigInt( unsigned int );
    BigInt( short int );
    BigInt( unsigned short int );
    BigInt( char );
    BigInt( unsigned char );
    BigInt( const BigInt& );
    ~BigInt( );

    BigInt& operator = ( long int );
    BigInt& operator = ( unsigned long int );
    BigInt& operator = ( int );
    BigInt& operator = ( unsigned int );
    BigInt& operator = ( short int );
    BigInt& operator = ( unsigned short int );
    BigInt& operator = ( char );
    BigInt& operator = ( unsigned char );
    BigInt& operator = ( const BigInt& );

    operator bool( );
    operator long int( );
    operator unsigned long int( );
    operator int( );
    operator unsigned int( );
    operator short int( );
    operator unsigned short int( );
    operator char( );
    operator unsigned char( );

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
    friend bool operator <  ( const long int&, const BigInt& );
    friend bool operator <= ( const long int&, const BigInt& );
    friend bool operator >  ( const long int&, const BigInt& );
    friend bool operator >= ( const long int&, const BigInt& );
    friend bool operator == ( const long int&, const BigInt& );
    friend bool operator != ( const long int&, const BigInt& );
    friend bool operator <  ( const BigInt&, const long int& );
    friend bool operator <= ( const BigInt&, const long int& );
    friend bool operator >  ( const BigInt&, const long int& );
    friend bool operator >= ( const BigInt&, const long int& );
    friend bool operator == ( const BigInt&, const long int& );
    friend bool operator != ( const BigInt&, const long int& );
  
    friend int    sgn ( const BigInt& );
    friend BigInt abs ( const BigInt& );
};

BigInt operator + ( const BigInt&, const BigInt& );
BigInt operator - ( const BigInt&, const BigInt& );
BigInt operator * ( const BigInt&, const BigInt& );
BigInt operator / ( const BigInt&, const BigInt& );
BigInt operator + ( const long int&, const BigInt& );
BigInt operator - ( const long int&, const BigInt& );
BigInt operator * ( const long int&, const BigInt& );
BigInt operator / ( const long int&, const BigInt& );
BigInt operator + ( const BigInt&, const long int& );
BigInt operator - ( const BigInt&, const long int& );
BigInt operator * ( const BigInt&, const long int& );
BigInt operator / ( const BigInt&, const long int& );

#endif  // BIG_INT_H

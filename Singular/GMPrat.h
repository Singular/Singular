// ----------------------------------------------------------------------------
//  GMPrat.h
//  begin of file
//  originally written by Gerd Sussner, sussner@mi.uni-erlangen.de
//  copied by Stephan Endrass, endrass@mathematik.uni-mainz.de
//  23.7.99
// ----------------------------------------------------------------------------

#ifndef  GMPRAT_H
#define  GMPRAT_H

#include "gmp.h"

class Rational
{
    struct rep
    {
        mpq_t rat;
        int   n;
        rep() { n=1; }
    };

    rep             *p;
    static Rational save;

    void disconnect();

    public:

    Rational( );
    Rational( long int );
    Rational( unsigned long int );
    Rational( int );
    Rational( unsigned int );
    Rational( double );
    Rational( float );
    Rational( char* );
    Rational( const Rational& );
    Rational( long int,unsigned long int );
    Rational( unsigned long int,unsigned long int );
    Rational( int, unsigned int );
    Rational( unsigned int,unsigned int );
    Rational( const Rational&,const Rational& );
    Rational( long int,long int );
    Rational( int,int );
    Rational( char*,char* );
    ~Rational( );

    Rational& operator = ( int );
    Rational& operator = ( double );
    Rational& operator = ( float );
    Rational& operator = ( char *s );
    Rational& operator = ( const Rational& );

    unsigned int length( ) const;
    Rational get_num( );
    Rational get_den( );
    int      get_num_si( );
    int      get_den_si( );
    operator bool( );
    operator long int( );
    operator unsigned long int( );
    operator int( );
    operator unsigned int( );
    operator short int( );
    operator unsigned short int( );
    operator char( );
    operator unsigned char( );
    operator double( );
    operator float( );

    Rational  operator - ( );
    Rational  operator ~ ( );
    Rational& operator += ( const Rational& );
    Rational& operator -= ( const Rational& );
    Rational& operator *= ( const Rational& );
    Rational& operator /= ( const Rational& );
    Rational& operator ++ ( );
    Rational  operator ++ ( int );
    Rational& operator -- ( );
    Rational  operator -- ( int );

    friend Rational operator - ( const Rational& );

    friend bool operator <  ( const Rational&,const Rational& );
    friend bool operator <= ( const Rational&,const Rational& );
    friend bool operator >  ( const Rational&,const Rational& );
    friend bool operator >= ( const Rational&,const Rational& );
    friend bool operator == ( const Rational&,const Rational& );
    friend bool operator != ( const Rational&,const Rational& );

    #ifdef GMPRAT_PRINT
    friend ostream& operator<<( ostream&,const Rational& );
    #endif

    friend int      sgn ( const Rational& );
    friend Rational abs ( const Rational& );
    friend Rational pow ( const Rational&,int );

    double   complexity( ) const;

    friend Rational gcd ( const Rational&, const Rational& );
    friend Rational lcm ( const Rational&, const Rational& );
    friend Rational gcd ( Rational*, int );
    friend Rational lcm ( Rational*, int );
};

Rational operator + ( const Rational&, const Rational& );
Rational operator - ( const Rational&, const Rational& );
Rational operator * ( const Rational&, const Rational& );
Rational operator / ( const Rational&, const Rational& );

#endif /* GMPRAT_H */

// ----------------------------------------------------------------------------
//  GMPrat.h
//  end of file
// ----------------------------------------------------------------------------


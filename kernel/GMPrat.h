// ----------------------------------------------------------------------------
//  GMPrat.h
//  begin of file
//  originally written by Gerd Sussner, sussner@mi.uni-erlangen.de
//  copied by Stephan Endrass, endrass@mathematik.uni-mainz.de
//  23.7.99
// ----------------------------------------------------------------------------

#ifndef  GMPRAT_H
#define  GMPRAT_H

#include <kernel/si_gmp.h>

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
    Rational( int );
    Rational( const Rational& );
    Rational( const Rational&,const Rational& );
    Rational( int,int );
    ~Rational( );

    Rational& operator = ( int );
    Rational& operator = ( char *s );
    Rational& operator = ( const Rational& );

    unsigned int length( ) const;
    Rational get_num( );
    Rational get_den( );
    int      get_num_si( );
    int      get_den_si( );
    operator int( );

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


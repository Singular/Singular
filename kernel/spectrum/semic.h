// ----------------------------------------------------------------------------
//  semic.h
//  begin of file
//  Stephan Endrass, endrass@mathematik.uni-mainz.de
//  23.7.99
// ----------------------------------------------------------------------------

// ----------------------------------------------------------------------------
//  here we define a class  spectrum  to test semicontinuity
// ----------------------------------------------------------------------------

#ifndef SEMIC_H
#define SEMIC_H

#ifdef HAVE_SPECTRUM


#include <kernel/spectrum/GMPrat.h>

typedef enum
{
    OPEN,
    LEFTOPEN,
    RIGHTOPEN,
    CLOSED

} interval_status;

/*typedef enum
{
    semicOK,
    semicMulNegative,

    semicListTooShort,
    semicListTooLong,

    semicListFirstElementWrongType,
    semicListSecondElementWrongType,
    semicListThirdElementWrongType,
    semicListFourthElementWrongType,
    semicListFifthElementWrongType,
    semicListSixthElementWrongType,

    semicListNNegative,
    semicListWrongNumberOfNumerators,
    semicListWrongNumberOfDenominators,
    semicListWrongNumberOfMultiplicities,

    semicListMuNegative,
    semicListPgNegative,
    semicListNumNegative,
    semicListDenNegative,
    semicListMulNegative,

    semicListNotSymmetric,
    semicListNotMonotonous,

    semicListMilnorWrong,
    semicListPGWrong

} semicState; */ //TODO move to Singular

class spectrum
{
public:

    int         mu;      // milnor number
    int         pg;      // geometrical genus
    int         n;       // # of spectrum numbers
    Rational    *s;      // spectrum numbers
    int         *w;      // multiplicities

//    spectrum( );
//    spectrum( lists );

    ///  Zero constructor
    spectrum( )
    {
       copy_zero( );
    }

    spectrum( const spectrum& );

    ~spectrum( );

    spectrum operator = ( const spectrum& );
//    spectrum operator = ( lists );

    friend spectrum   operator + ( const spectrum&,const spectrum& );
    friend spectrum   operator * ( int,const spectrum& );

    #ifdef SEMIC_PRINT
    friend ostream &  operator << ( ostream&,const spectrum& );
    #endif /*SEMIC_PRINT*/

    void    copy_new    ( int );
//    void    copy_delete ( void );

///  Delete the memory of a spectrum
inline void copy_delete( void )
{
    if( s != (Rational*)NULL && n > 0 ) delete [] s;
    if( w != (int*)NULL      && n > 0 ) delete [] w;
    copy_zero( );
}

///  Initialize with zero
inline void copy_zero( void )
{
    mu = 0;
    pg = 0;
    n  = 0;
    s  = (Rational*)NULL;
    w  = (int*)NULL;
}

///  Initialize shallow from another spectrum
inline void copy_shallow( spectrum &spec )
{
    mu = spec.mu;
    pg = spec.pg;
    n  = spec.n;
    s  = spec.s;
    w  = spec.w;
}

    void    copy_deep   ( const spectrum& );
//    void    copy_deep   ( lists );

//    lists   thelist     ( void );

    int     add_subspectrum    ( spectrum&,int );
    int     next_number        ( Rational* );
    int     next_interval      ( Rational*,Rational* );
    int     numbers_in_interval( Rational&,Rational&,interval_status );
    int     mult_spectrum      ( spectrum& );
    int     mult_spectrumh     ( spectrum& );

    //    int     set_milnor         ( void );
    //   int     set_geometric_genus( void );
};

#endif /*HAVE_SPECTRUM*/
// ----------------------------------------------------------------------------
//  semic.h
//  end of file
// ----------------------------------------------------------------------------
#endif

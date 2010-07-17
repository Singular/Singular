// ----------------------------------------------------------------------------
//  npolygon.h
//  begin of file
//  Stephan Endrass, endrass@mathematik.uni-mainz.de
//  23.7.99
// ----------------------------------------------------------------------------

#ifndef NPOLYGON_H
#define NPOLYGON_H

#include <kernel/GMPrat.h>

// ----------------------------------------------------------------------------
//  Class representing a linear form QQ^N-->QQ
// ----------------------------------------------------------------------------

class linearForm
{

private:

    Rational    *c;                   // the coefficients
    int         N;                    // number of coefficients

public:

    linearForm( );
    linearForm( const linearForm& );
    ~linearForm( );

    linearForm & operator = ( const linearForm& );

    friend  int     operator == ( const linearForm&,const linearForm& );

    void        copy_new     ( int );
    void        copy_delete  ( void );
    void        copy_zero    ( void );
    void        copy_shallow ( linearForm& );
    void        copy_deep    ( const linearForm& );

    Rational    weight       ( poly ) const;
    Rational    weight_shift ( poly ) const;
    Rational    weight1      ( poly ) const;
    Rational    weight_shift1( poly ) const;

    Rational    pweight      ( poly ) const;

    int         positive     ( void );

    #ifdef  NPOLYGON_PRINT
        friend ostream & operator << ( ostream&,const linearForm& );
    #endif

    friend class newtonPolygon;
};

// ----------------------------------------------------------------------------
//  Class representing a Newton polygon
// ----------------------------------------------------------------------------

class newtonPolygon
{

private:

    linearForm  *l;                   // the linear forms
    int         N;                    // number of linear forms

public:

    newtonPolygon( );
    newtonPolygon( const newtonPolygon& );
    newtonPolygon( poly );
    ~newtonPolygon( );

    newtonPolygon & operator = ( const newtonPolygon& );


    void        copy_new    ( int );
    void        copy_delete ( void );
    void        copy_zero   ( void );
    void        copy_shallow( newtonPolygon& );
    void        copy_deep   ( const newtonPolygon& );

    void        add_linearForm( const linearForm& );

    Rational    weight       ( poly ) const;
    Rational    weight_shift ( poly ) const;
    Rational    weight1      ( poly ) const;
    Rational    weight_shift1( poly ) const;

    //int         is_sqh     ( void ) const;
    //Rational*   sqh_weights( void ) const;
    //int         sqh_N      ( void ) const;

    #ifdef  NPOLYGON_PRINT
        friend ostream & operator << ( ostream&,const newtonPolygon&  );
    #endif
};

// ---------------------------------------
//  inline functions for class linearForm
// ---------------------------------------

// ----------------------------------------------------------------------------
//  Initialize with zero
// ----------------------------------------------------------------------------

inline  void    linearForm::copy_zero( void )
{
    c = (Rational*)NULL;
    N = 0;
}

// ----------------------------------------------------------------------------
//  Initialize shallow from another linear form
// ----------------------------------------------------------------------------

inline  void    linearForm::copy_shallow( linearForm &l )
{
    c = l.c;
    N = l.N;
}


// ----------------------------------------------------------------------------
//  Zero constructor
// ----------------------------------------------------------------------------

inline  linearForm::linearForm( )
{
    copy_zero( );
}


// ------------------------------------------
//  inline functions for class newtonPolygon
// ------------------------------------------

// ----------------------------------------------------------------------------
//  Initialize with zero
// ----------------------------------------------------------------------------

inline  void    newtonPolygon::copy_zero( void )
{
    l = (linearForm*)NULL;
    N = 0;
}

// ----------------------------------------------------------------------------
//  Initialize shallow from another Newton polygon
// ----------------------------------------------------------------------------

inline  void    newtonPolygon::copy_shallow( newtonPolygon &np )
{
    l = np.l;
    N = np.N;
}


// ----------------------------------------------------------------------------
//  Zero constructor
// ----------------------------------------------------------------------------

inline newtonPolygon::newtonPolygon( )
{
    copy_zero( );
}

#endif /* NPOLYGON_H */

// ----------------------------------------------------------------------------
//  npolygon.h
//  end of file
// ----------------------------------------------------------------------------

// ----------------------------------------------------------------------------
//  multicnt.h
//  begin of file
//  Stephan Endrass, endrass@mathematik.uni-mainz.de
//  23.7.99
// ----------------------------------------------------------------------------

// ----------------------------------------------------------------------------
//  Description: the class multiCnt is a general multi purpose counter.
//  The counter holds an array  cnt  of  N  integers.
//  The integer  last_inc  holds the index of the last incremented entry.
// ----------------------------------------------------------------------------

#ifndef MULTICNT_H
#define MULTICNT_H

class multiCnt
{
public:

    int *cnt;
    int N;
    int last_inc;

    multiCnt( );
    multiCnt( int );
    multiCnt( int,int );
    multiCnt( int,int* );
    multiCnt( const multiCnt& );

    void    copy_zero   ( void );
    void    copy_new    ( int );
    void    copy_delete ( void );
    void    copy_shallow( multiCnt& );
    void    copy_deep   ( const multiCnt& );

    void    set( int );

    void    inc      ( void );
    void    inc_carry( void );
    int     inc      ( int );
    //void    dec      ( void );
    //void    dec_carry( void );
    //int     dec      ( int );
};

// ----------------------------------------------------------------------------
//  zero init
// ----------------------------------------------------------------------------

inline  void    multiCnt::copy_zero( void )
{
    cnt      = (int*)NULL;
    N        = 0;
    last_inc = 0;
}

// ----------------------------------------------------------------------------
//  copy a counter by reference
// ----------------------------------------------------------------------------

inline  void multiCnt::copy_shallow( multiCnt &C )
{
    cnt      = C.cnt;
    N        = C.N;
    last_inc = C.last_inc;
}

// ----------------------------------------------------------------------------
//  zero constructor
// ----------------------------------------------------------------------------

inline multiCnt::multiCnt( )
{
    copy_zero( );
}

#endif /* MULTICNT_H */

// ----------------------------------------------------------------------------
//  multicnt.h
//  end of file
// ----------------------------------------------------------------------------

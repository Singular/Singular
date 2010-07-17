// ----------------------------------------------------------------------------
//  multicnt.cc
//  begin of file
//  Stephan Endrass, endrass@mathematik.uni-mainz.de
//  23.7.99
// ----------------------------------------------------------------------------

#define MULTICNT_CC

#include <kernel/mod2.h>

#ifdef HAVE_SPECTRUM

#include <stdlib.h>

#ifdef  MULTICNT_PRINT
#include <iostream.h>
#ifndef  MULTICNT_IOSTREAM
#include <stdio.h>
#endif
#endif

#include <kernel/multicnt.h>

// ----------------------------------------------------------------------------
//  allocate counter memory
// ----------------------------------------------------------------------------

void    multiCnt::copy_new( int n )
{
    if( n > 0 )
    {
        cnt = new int[n];

        #ifndef NDEBUG
        if( cnt == (int*)NULL )
        {
            #ifdef MULTICNT_PRINT
            #ifdef MULTICNT_IOSTREAM
                cerr << "multiCnt::copy_new(" << n << ")" << endl;
                cerr << "    returned ZERO!!!" << endl;
                cerr << "    exit..." << endl;
            #else
                fprintf( stderr,"multiCnt::copy_new( %d )\n",n );
                fprintf( stderr,"    returned ZERO!!!\n" );
                fprintf( stderr,"    exit...\n" );
            #endif
            #endif

            exit( 1 );
        }
        #endif
    }
    else if( n == 0 )
    {
        cnt = (int*)NULL;
    }
    else
    {
        #ifdef MULTICNT_PRINT
        #ifdef MULTICNT_IOSTREAM
            cerr << "multiCnt::copy_new(" << n << ")" << endl;
            cerr << "    exit..." << endl;
        #else
            fprintf( stderr,"multiCnt::copy_new( %d )\n",n );
            fprintf( stderr,"    exit...\n" );
        #endif
        #endif

        exit( 1 );
    }
}

// ----------------------------------------------------------------------------
//  delete counter memory
// ----------------------------------------------------------------------------

void    multiCnt::copy_delete( void )
{
    if( N>0 && cnt!=(int*)NULL ) delete [] cnt;
    copy_zero( );
}

// ----------------------------------------------------------------------------
//  copy a counter
// ----------------------------------------------------------------------------

void multiCnt::copy_deep( const multiCnt &C )
{
    copy_new( C.N );

    last_inc = C.last_inc;
    N        = C.N;

    for( int i=0; i<N; i++ )
    {
        cnt[i] = C.cnt[i];
    }
}

// ----------------------------------------------------------------------------
//  set all counter entries to c
// ----------------------------------------------------------------------------

void multiCnt::set( int c )
{
    for( int i=0; i<N; i++ ) cnt[i]=c;
}


// ----------------------------------------------------------------------------
//  n entries zero init constructor
// ----------------------------------------------------------------------------

multiCnt::multiCnt( int n ) :
    last_inc( 0 )
{
    copy_new( n );
    N = n;
    set( 0 );
}

// ----------------------------------------------------------------------------
//  n entries c init constructor
// ----------------------------------------------------------------------------

multiCnt::multiCnt( int n,int c ) :
    last_inc( 0 )
{
    copy_new( n );
    N = n;
    set( c );
}

// ----------------------------------------------------------------------------
//  n entries c* init constructor
// ----------------------------------------------------------------------------

multiCnt::multiCnt( int n,int *c ) :
    last_inc( 0 )
{
    copy_new( n );
    N = n;
    for( int i=0; i<N; i++ ) cnt[i] = c[i];
}

// ----------------------------------------------------------------------------
//  increment the counter
// ----------------------------------------------------------------------------

void multiCnt::inc( void )
{
    cnt[0]++;
    last_inc=0;
}

// ----------------------------------------------------------------------------
//  decrement the counter
// ----------------------------------------------------------------------------

/*
void multiCnt::dec( void )
{
    cnt[0]--;
    last_inc=0;
}
*/

// ----------------------------------------------------------------------------
//  increment the counter and carry over
// ----------------------------------------------------------------------------

void multiCnt::inc_carry( void )
{
    for( int i=0; i<=last_inc; i++ ) cnt[i] = 0;
    last_inc++;
    cnt[last_inc]++;
}

// ----------------------------------------------------------------------------
//  decrement the counter and carry over
// ----------------------------------------------------------------------------

/*
void multiCnt::dec_carry( void )
{
    for( int i=0; i<=last_inc; i++ ) cnt[i] = 0;
    last_inc++;
    cnt[last_inc]--;
}
*/

// ----------------------------------------------------------------------------
//  increment the counter and carry over automatic
// ----------------------------------------------------------------------------

int  multiCnt::inc( int carry )
{
    if( carry==FALSE )
    {
        inc( );
    }
    else
    {
        if( last_inc==N-1 )
        {
            return  FALSE;
        }

        inc_carry( );
    }

    return  TRUE;
}

// ----------------------------------------------------------------------------
//  decrement the counter and carry over automatic
// ----------------------------------------------------------------------------

/*
int  multiCnt::dec( int carry )
{
    if( carry==FALSE )
    {
        dec( );
    }
    else
    {
        if( last_inc==N-1 )
        {
            return  FALSE;
        }

        dec_carry( );
    }

    return  TRUE;
}
*/

#endif /* HAVE_SPECTRUM */
// ----------------------------------------------------------------------------
//  multicnt.cc
//  end of file
// ----------------------------------------------------------------------------

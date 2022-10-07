// ----------------------------------------------------------------------------
//  semic.cc
//  begin of file
//  Stephan Endrass, endrass@mathematik.uni-mainz.de
//  23.7.99
// ----------------------------------------------------------------------------

#define SEMIC_CC




#include "kernel/mod2.h"

#ifdef HAVE_SPECTRUM

#ifdef  SEMIC_PRINT
#ifndef SEMIC_IOSTREAM
#include <stdio.h>
#else
#include <iostream.h>
#endif
#endif

#include <string.h>

#include "misc/intvec.h"
#include "misc/mylimits.h"
#include "kernel/spectrum/GMPrat.h"
#include "kernel/spectrum/semic.h"

// ----------------------------------------------------------------------------
//  Copy constructor for  spectrum
// ----------------------------------------------------------------------------

spectrum::spectrum( const spectrum &spec )
{
    copy_deep( spec );
}

// ----------------------------------------------------------------------------
//  Destructor for  spectrum
// ----------------------------------------------------------------------------

spectrum::~spectrum( )
{
    copy_delete( );
}

// ----------------------------------------------------------------------------
//  Allocate memory for a spectrum of  k  numbers
// ----------------------------------------------------------------------------

void spectrum::copy_new( int k )
{
    if( k > 0 )
    {
        s = new Rational[k];
        w = new int[k];

        #ifndef SING_NDEBUG
        if( s == (Rational*)NULL || w == (int*)NULL )
        {
            #ifdef SEMIC_PRINT
            #ifdef SEMIC_IOSTREAM
                cerr << "spectrum::copy_new(" << k << ")" << endl;
                cerr << "    returned ZERO!!!" << endl;
                cerr << "    exit..." << endl;
            #else
                fprintf( stderr,"spectrum::copy_new( %d )\n",k );
                fprintf( stderr,"    returned ZERO!!!\n" );
                fprintf( stderr,"    exit...\n" );
            #endif
            #endif
        }
        #endif
    }
    else if( k == 0 )
    {
        s = (Rational*)NULL;
        w = (int*)NULL;
    }
    else if( k < 0 )
    {
        #ifdef SEMIC_PRINT
        #ifdef SEMIC_IOSTREAM
                cerr << "spectrum::copy_new(" << k << ")";
                cerr << ": k < 0 ..." << endl;
        #else
                fprintf( stderr,"spectrum::copy_new( %d )",k );
                fprintf( stderr,": k < 0 ...\n" );
        #endif
        #endif

        exit( 1 );
    }
}

// ----------------------------------------------------------------------------
//  Initialize a  spectrum  deep from another  spectrum
// ----------------------------------------------------------------------------

void spectrum::copy_deep( const spectrum &spec )
{
    mu = spec.mu;
    pg = spec.pg;
    n  = spec.n;

    copy_new( n );

    for( int i=0; i<n; i++ )
    {
        s[i] = spec.s[i];
        w[i] = spec.w[i];
    }
}

// ----------------------------------------------------------------------------
//  operator  =  for  spectrum
// ----------------------------------------------------------------------------

spectrum spectrum::operator = ( const spectrum &spec )
{
    copy_delete( );
    copy_deep( spec );

    return *this;
}

// ----------------------------------------------------------------------------
//  add the two spectra  s1  and  s2  and return their sum
// ----------------------------------------------------------------------------

spectrum  operator + ( const spectrum &s1,const spectrum &s2 )
{
    int i1=0, i2=0, i3=0;

    spectrum result;

    do
    {
        if( i1 >= s1.n )
        {
            i2++;
        }
        else if( i2 >= s2.n )
        {
            i1++;
        }
        else if( s1.s[i1] < s2.s[i2] )
        {
            i1++;
        }
        else if( s1.s[i1] == s2.s[i2] )
        {
            i1++;
            i2++;
        }
        else
        {
            i2++;
        }
        i3++;
    }
    while( i1 < s1.n || i2 < s2.n );

    result.copy_new( i3 );
    result.n = i3;

    i1 = i2 = i3 = 0;

    do
    {
        if( i1 >= s1.n )
        {
            result.s[i3] = s2.s[i2];
            result.w[i3] = s2.w[i2];
            i2++;
        }
        else if( i2 >= s2.n )
        {
            result.s[i3] = s1.s[i1];
            result.w[i3] = s1.w[i1];
            i1++;
        }
        else if( s1.s[i1] < s2.s[i2] )
        {
            result.s[i3] = s1.s[i1];
            result.w[i3] = s1.w[i1];
            i1++;
          }
        else if( s1.s[i1] == s2.s[i2] )
        {
            result.s[i3] = s1.s[i1];
            result.w[i3] = s1.w[i1] + s2.w[i2];
            i1++;
            i2++;
        }
        else
        {
            result.s[i3] = s2.s[i2];
            result.w[i3] = s2.w[i2];
            i2++;
        }
        i3++;
    }
    while( i1 < s1.n || i2 < s2.n );

    result.mu = s1.mu + s2.mu;
    result.pg = s1.pg + s2.pg;

    return  result;
}

// ----------------------------------------------------------------------------
//  multiply the multiplicities of the spectrum numbers of  a  with  m
// ----------------------------------------------------------------------------

spectrum operator * ( int k,const spectrum &spec )
{
    if( k == 0 )
    {
        spectrum result;

        return  result;
    }
    else
    {
        spectrum result( spec );

        result.mu *= k;
        result.pg *= k;

        for( int i=0; i<result.n; i++ )
        {
            result.w[i] *= k;
        }

        return  result;
    }
}

// ----------------------------------------------------------------------------
//  Print a  spectrum
// ----------------------------------------------------------------------------

#ifdef SEMIC_PRINT

ostream & operator << ( ostream &s,const spectrum &spec )
{
    for( int i=0; i<spec.n; i++ )
    {
        if( i>0 )
        {
            #ifdef SEMIC_STDOUT
                s << "+";
            #else
                fprintf( stdout,"+" );
            #endif
        }

        #ifdef SEMIC_STDOUT
            s << spec.w[i] << "*t^";
        #else
            fprintf( stdout,"%d*t^",spec.w[i] );
        #endif

        s << spec.s[i];
    }

    return s;
}
#endif

// ----------------------------------------------------------------------------
//  Add a subspectrum with multiplicity  k  (faster than '+')
// ----------------------------------------------------------------------------

int    spectrum::add_subspectrum( spectrum &a,int k )
{
    int i,j;
    for( i=0, j=0; i<n; i++ )
    {
        if( s[i] == a.s[j] )
        {
            w[i] += k*a.w[j];
            j++;
        }
    }

    return ( j == a.n ? TRUE : FALSE );
}

// ----------------------------------------------------------------------------
//  set  *alpha  to the next spectrum number strictly bigger than  *alpha
//  returns: TRUE, if such a spectrum number exists
//           FALSE otherwise
// ----------------------------------------------------------------------------

int    spectrum::next_number( Rational *alpha )
{
    int i=0;

    while( i < n && *alpha >= s[i]  )
    {
        i++;
    }

    if( i < n )
    {
        *alpha = s[i];
        return TRUE;
    }
    else
    {
        return FALSE;
    }
}

// ----------------------------------------------------------------------------
//  find the next interval on the real line of same length as
//  [*alpha1,*alpha2]  having a spectrum number as interval border
// ----------------------------------------------------------------------------

int     spectrum::next_interval( Rational *alpha1,Rational *alpha2 )
{
    Rational zero( 0,1 );
    Rational a1 = *alpha1;
    Rational a2 = *alpha2;
    Rational d  = *alpha2 - *alpha1;

    int    e1 = this->next_number( &a1 );
    int    e2 = this->next_number( &a2 );

    if( e1 || e2 )
    {
        Rational d1 = a1 - *alpha1;
        Rational d2 = a2 - *alpha2;

        if( d1 < d2 || d2 == zero )
        {
            *alpha1 = a1;
            *alpha2 = a1 + d;
        }
        else
        {
            *alpha1 = a2 - d;
            *alpha2 = a2;
        }
        return  TRUE;
    }
    else
    {
        return  FALSE;
    }
}

// ----------------------------------------------------------------------------
//  compute the number of spectrum numbers in the interval  [*alpha1,*alpha2]
// ----------------------------------------------------------------------------

int     spectrum::numbers_in_interval( Rational &alpha1,
                Rational &alpha2,interval_status status )
{
    int count = 0;

    for( int i=0; i<n; i++ )
    {
        if( ( ( status == OPEN   || status == LEFTOPEN  ) &&
              s[i] >  alpha1 ) ||
            ( ( status == CLOSED || status == RIGHTOPEN ) &&
              s[i] >= alpha1 ) )
        {
              if( ( ( status == OPEN   || status == RIGHTOPEN  ) &&
                  s[i] <  alpha2 ) ||
                ( ( status == CLOSED || status == LEFTOPEN ) &&
                  s[i] <= alpha2 ) )
            {
                count += w[i];
            }
            else
            {
                break;
            }
        }
    }

    return count;
}

// ----------------------------------------------------------------------------
//  find the maximal integer  k  such that  k*t is semicontinous
//  for the spectrum
// ----------------------------------------------------------------------------

int     spectrum::mult_spectrum( spectrum &t )
{
    spectrum u = *this + t;

    Rational alpha1 = -2;
    Rational alpha2 = -1;

    int      mult=MAX_INT_VAL,nthis,nt;

    while( u.next_interval( &alpha1,&alpha2 ) )
    {
        nt    = t.numbers_in_interval( alpha1,alpha2,LEFTOPEN );
        nthis = this->numbers_in_interval( alpha1,alpha2,LEFTOPEN );

        if( nt != 0 )
        {
            mult = (nthis/nt < mult ? nthis/nt: mult );
        }

    }

    return  mult;
}

// ----------------------------------------------------------------------------
//  find the maximal integer  k  such that  k*t is semicontinous
//  for the spectrum (in the homogeneous sense)
// ----------------------------------------------------------------------------

int     spectrum::mult_spectrumh( spectrum &t )
{
    spectrum u = *this + t;

    Rational alpha1 = -2;
    Rational alpha2 = -1;

    int      mult=MAX_INT_VAL,nthis,nt;

    while( u.next_interval( &alpha1,&alpha2 ) )
    {
        nt    = t.numbers_in_interval( alpha1,alpha2,LEFTOPEN );
        nthis = this->numbers_in_interval( alpha1,alpha2,LEFTOPEN );

        if( nt != 0 )
        {
            mult = (nthis/nt < mult ? nthis/nt: mult );
        }

        nt    = t.numbers_in_interval( alpha1,alpha2,OPEN );
        nthis = this->numbers_in_interval( alpha1,alpha2,OPEN );

        if( nt != 0 )
        {
            mult = (nthis/nt < mult ? nthis/nt: mult );
        }
    }

    return  mult;
}

// ----------------------------------------------------------------------------
//  Set the Milnor number
// ----------------------------------------------------------------------------

/*
int spectrum::set_milnor( void )
{
   mu = 0;

   for( int i=0; i<n; i++ )
   {
      mu += w[i];
   }

   return  mu;
}

// ----------------------------------------------------------------------------
//  Set the geometrical genus
// ----------------------------------------------------------------------------

int spectrum::set_geometric_genus( void )
{
   pg = 0;

   for( int i=0; i<n && s[i]<=1; i++ )
   {
      pg += w[i];
   }
   return  pg;
}
*/

#endif /* HAVE_SPECTRUM */
// ----------------------------------------------------------------------------
//  semic.cc
//  end of file
// ----------------------------------------------------------------------------

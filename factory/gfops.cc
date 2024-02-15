/* emacs edit mode for this file is -*- C++ -*- */


#include "config.h"


#ifdef HAVE_CSTDIO
#include <cstdio>
#include <cstdlib>
#else
#include <stdio.h>
#include <stdlib.h>
#endif
#include <string.h>

#include "cf_assert.h"

#include "cf_defs.h"
#include "gf_tabutil.h"
#include "cf_util.h"
#include "canonicalform.h"
#include "variable.h"
#include "gfops.h"

#ifdef SINGULAR
#include "singext.h"
#endif


const int gf_maxtable = 63001;
const int gf_maxbuffer = 200;

const int gf_primes_len = 42;
#ifndef NOASSERT
STATIC_VAR unsigned short gf_primes [] =
{
      2,   3,   5,   7,  11,  13,  17,  19,
     23,  29,  31,  37,  41,  43,  47,  53,
     59,  61,  67,  71,  73,  79,  83,  89,
     97, 101, 103, 107, 109, 113, 127, 131,
    137, 139, 149, 151, 157, 163, 167, 173,
    179, 181, 191, 193, 197, 199, 223, 211,
    227, 229, 233, 239, 241, 251
};
#endif

VAR int gf_q = 0;
VAR int gf_p = 0;
VAR int gf_n = 0;
VAR int gf_q1 = 0;
VAR int gf_m1 = 0;
VAR char gf_name = 'Z';

VAR unsigned short * gf_table = 0;

INST_VAR CanonicalForm gf_mipo=0L;

static CanonicalForm intVec2CF ( int degree, int * coeffs, int level )
{
    int i;
    CanonicalForm result;
    for ( i = 0; i <= degree; i++ )
    {
        result += CanonicalForm( coeffs[ i ] ) * power( Variable( level ), degree - i );
    }
    return result;
}

STATIC_VAR char *gftable_dir;
extern "C" {
  void set_gftable_dir(char *d){
    gftable_dir = d;
  }
}

static void gf_get_table ( int p, int n )
{
    char buffer[gf_maxbuffer];
    int q = ipower( p, n );

    // do not read the table a second time
    if ( gf_q == q )
    {
        return;
    }

    if ( gf_table == 0 )
        gf_table = new unsigned short[gf_maxtable];

    // try to open file
    char *gffilename;
    FILE * inputfile;
    if (gftable_dir)
    {
      snprintf( buffer, "gftables/%d", q);
      gffilename = (char *)malloc(strlen(gftable_dir) + strlen(buffer) + 1);
      STICKYASSERT(gffilename,"out of memory");
      strcpy(gffilename,gftable_dir);
      strcat(gffilename,buffer);
      inputfile = fopen( gffilename, "r" );
    }
    else
    {
      sprintf( buffer, gf_maxbuffer, "gftables/%d", q );
      gffilename = buffer;
#ifndef SINGULAR
      inputfile = fopen( buffer, "r" );
#else
      inputfile = feFopen( buffer, "r" );
#endif
    }
    if (!inputfile)
    {
      fprintf(stderr,"can not open GF(q) addition table: %s\n",gffilename);
      STICKYASSERT(inputfile, "can not open GF(q) table");
    }

    // read ID
    char * bufptr;
    char * success;
    success = fgets( buffer, gf_maxbuffer, inputfile );
    STICKYASSERT( success, "illegal table (reading ID)" );
    STICKYASSERT( strcmp( buffer, "@@ factory GF(q) table @@\n" ) == 0, "illegal table" );
    // read p and n from file
    int pFile, nFile;
    success = fgets( buffer, gf_maxbuffer, inputfile );
    STICKYASSERT( success, "illegal table (reading p and n)" );
    sscanf( buffer, "%d %d", &pFile, &nFile );
    STICKYASSERT( p == pFile && n == nFile, "illegal table" );
    // skip (sic!) factory-representation of mipo
    // and terminating "; "
    bufptr = (char *)strchr( buffer, ';' ) + 2;
    // read simple representation of mipo
    int i, degree;
    sscanf( bufptr, "%d", &degree );
    bufptr = (char *)strchr( bufptr, ' ' ) + 1;
    int * mipo = NEW_ARRAY(int,degree+1);
    for ( i = 0; i <= degree; i++ )
    {
        sscanf( bufptr, "%d", mipo + i );
        bufptr = (char *)strchr( bufptr, ' ' ) + 1;
    }

    gf_p = p; gf_n = n;
    gf_q = q; gf_q1 = q-1;
    gf_mipo = intVec2CF( degree, mipo, 1 );
    DELETE_ARRAY(mipo);

    // now for the table
    int k, digs = gf_tab_numdigits62( gf_q );
    i = 1;
    while ( i < gf_q )
    {
        success = fgets( buffer, gf_maxbuffer, inputfile );
        STICKYASSERT( strlen( buffer ) - 1 == (size_t)digs * 30, "illegal table" );
        bufptr = buffer;
        k = 0;
        while ( i < gf_q && k < 30 )
        {
            gf_table[i] = convertback62( bufptr, digs );
            bufptr += digs;
            if ( gf_table[i] == gf_q )
            {
                if ( i == gf_q1 )
                    gf_m1 = 0;
                else
                    gf_m1 = i;
            }
            i++; k++;
        }
    }
    gf_table[0] = gf_table[gf_q1];
    gf_table[gf_q] = 0;

    (void)fclose( inputfile );
}

#ifndef NOASSERT
static bool gf_valid_combination ( int p, int n )
{
    int i = 0;
    while ( i < gf_primes_len && gf_primes[i] != p ) i++;
    if ( i == gf_primes_len )
        return false;
    else
    {
        i = n;
        int a = 1;
        while ( a < gf_maxtable && i > 0 )
        {
            a *= p;
            i--;
        }
        if ( i > 0 || a > gf_maxtable )
            return false;
        else
            return true;
    }
}
#endif

void gf_setcharacteristic ( int p, int n, char name )
{
    ASSERT( gf_valid_combination( p, n ), "illegal immediate GF(q)" );
    gf_name = name;
    gf_get_table( p, n );
}

long gf_gf2ff ( long a )
{
    if ( gf_iszero( a ) )
        return 0;
    else
    {
        // starting from z^0=1, step through the table
        // counting the steps until we hit z^a or z^0
        // again.  since we are working in char(p), the
        // latter is guaranteed to be fulfilled.
        long i = 0, ff = 1;
        do
        {
            if ( i == a )
                return ff;
            ff++;
            i = gf_table[i];
        } while ( i != 0 );
        return -1;
    }
}

int gf_gf2ff ( int a )
{
    if ( gf_iszero( a ) )
        return 0;
    else
    {
        // starting from z^0=1, step through the table
        // counting the steps until we hit z^a or z^0
        // again.  since we are working in char(p), the
        // latter is guaranteed to be fulfilled.
        int i = 0, ff = 1;
        do
        {
            if ( i == a )
                return ff;
            ff++;
            i = gf_table[i];
        } while ( i != 0 );
        return -1;
    }
}

bool gf_isff ( long a )
{
    if ( gf_iszero( a ) )
        return true;
    else
    {
        // z^a in GF(p) iff (z^a)^p-1=1
        return gf_isone( gf_power( a, gf_p - 1 ) );
    }
}

bool gf_isff ( int a )
{
    if ( gf_iszero( a ) )
        return true;
    else
    {
        // z^a in GF(p) iff (z^a)^p-1=1
        return gf_isone( gf_power( a, gf_p - 1 ) );
    }
}

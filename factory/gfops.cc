// emacs edit mode for this file is -*- C++ -*-
// $Id: gfops.cc,v 1.0 1996-05-17 10:59:46 stobbe Exp $

/*
$Log: not supported by cvs2svn $
*/

#include <iostream.h>
#include <fstream.h>
#include <strstream.h>

#include "assert.h"
#include "cf_defs.h"
#include "gfops.h"
#include "gf_tabutil.h"
#include "cf_util.h"
#include "canonicalform.h"


#define MAXTABLE 32767

int gf_q = 0;
int gf_p = 0;
int gf_n = 0;
int gf_q1 = 0;
int gf_m1 = 0;
char gf_name = 'Z';

unsigned short * gf_table = 0;

const int gf_primes_len = 42;

static unsigned short gf_primes [] =
{
      2,   3,   5,   7,  11,  13,  17,  19,
     23,  29,  31,  37,  41,  43,  47,  53,
     59,  61,  67,  71,  73,  79,  83,  89,
     97, 101, 103, 107, 109, 113, 127, 131,
    137, 139, 149, 151, 157, 163, 167, 173,
    179, 181
};

CanonicalForm gf_mipo = 0;

static void gf_get_table ( int p, int n )
{
    if ( gf_table == 0 )
	gf_table = new unsigned short[MAXTABLE];
    ostrstream fname;
    fname << "gftables/gftable." << p << "." << n << '\0';
    char * fn = fname.str();
    ifstream inputfile( fn, ios::in );
    STICKYASSERT( inputfile, "can not open GF(q) table" );
    delete [] fn;
    int i, k, pp, nn, digs;
    gf_q = ipower( p, n );
    gf_p = p;
    gf_q1 = gf_q-1;
    digs = gf_tab_numdigits62( gf_q );
    char * buffer = new char[100];
    char * bufptr;
    inputfile.getline( buffer, 99, '\n' );
    STICKYASSERT( strcmp( buffer, "@@ factory GF(q) table @@" ) == 0, "illegal table" );
    inputfile >> pp;
    inputfile >> nn;
    STICKYASSERT( p == pp && n == nn, "illegal table" );
    inputfile >> gf_mipo;
    inputfile.getline( buffer, 99, '\n' );
    i = 1;
    while ( i < gf_q ) {
	inputfile.getline( buffer, 99, '\n' );
	STICKYASSERT( strlen( buffer ) == (size_t)digs * 30, "illegal table" );
	bufptr = buffer;
	k = 0;
	while ( i < gf_q && k < 30 ) {
	    gf_table[i] = convertback62( bufptr, digs );
	    bufptr += digs;
            if ( gf_table[i] == gf_q )
		if ( i == gf_q1 )
		    gf_m1 = 0;
		else
		    gf_m1 = i;
	    i++; k++;
	}
    }
    gf_table[0] = gf_table[gf_q1];
}

static bool gf_valid_combination ( int p, int n )
{
    int i = 0;
    while ( i < gf_primes_len && gf_primes[i] != p ) i++;
    if ( i == gf_primes_len )
	return false;
    else {
	int m = MAXTABLE;
	i = n;
	int a = 1;
	while ( a < m && i > 0 ) {
	    a *= p;
	    i--;
	}
	if ( i > 0 || a > m )
	    return false;
	else
	    return true;
    }
}
    
void gf_setcharacteristic ( int p, int n, char name )
{
    ASSERT( gf_valid_combination( p, n ), "illegal immediate GF(q)" );
    gf_name = name;
    gf_get_table( p, n );
}

int gf_gf2ff ( int a )
{
    if ( gf_iszero( a ) )
	return 0;
    else {
	int i = 0, ff = 1;
	do {
	    if ( i == a )
		return ff;
	    ff++;
	    i = gf_table[i];
	} while ( i != 0 );
	return -1;
    }
}

bool gf_isff ( int a )
{
    if ( gf_iszero( a ) )
	return true;
    else {
	int i = 0;
	do {
	    if ( i == a )
		return true;
	    i = gf_table[i];
	} while ( i != 0 );
	return false;
    }
}

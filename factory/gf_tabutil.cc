/* emacs edit mode for this file is -*- C++ -*- */
/* $Id$ */

#include <config.h>

#include "assert.h"

#include "cf_defs.h"
#include "gf_tabutil.h"

int gf_tab_numdigits62 ( int q )
{
    if ( q < 62 )
        return 1;
    else  if ( q < 62*62 )
        return 2;
    else
        return 3;
}

char conv62 ( int i )
{
    if ( i < 10 )
        return '0' + char(i);
    else  if ( i < 36 )
        return 'A' + char(i-10);
    else
        return 'a' + char(i-36);
}

void convert62 ( int i, int n, char * p )
{
    for ( int j = n-1; j >= 0; j-- ) {
        p[j] = conv62( i % 62 );
        i /= 62;
    }
}

int convback62 ( char c )
{
    if ( c >= '0' && c <= '9' )
        return int(c) - int('0');
    else  if ( c >= 'A' && c <= 'Z' )
        return int(c) - int('A') + 10;
    else
        return int(c) - int('a') + 36;
}

int convertback62 ( char * p, int n )
{
    int r = 0;
    for ( int j = 0; j < n; j++ )
        r = r * 62 + convback62( p[j] );
    return r;
}

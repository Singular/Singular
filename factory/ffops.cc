/* emacs edit mode for this file is -*- C++ -*- */
/* $Id: ffops.cc,v 1.7 1998-03-11 09:40:31 schmidt Exp $ */

#include <config.h>

#include <string.h>

#include "assert.h"

#include "cf_defs.h"
#include "ffops.h"

int ff_prime = 31991;
int ff_halfprime = 31991 / 2;
bool ff_big = false;
short * ff_invtab = new short [32767];

void ff_setprime ( const int p )
{
    if ( p != ff_prime ) {
	ff_prime = p;
	ff_halfprime = ff_prime / 2;
	if ( ! ff_big )
	    memset( ff_invtab, 0, ff_prime*sizeof(short) );
    }
}

int ff_newinv ( const int a )
{
    int u, r0 = a, r1 = ff_prime, q0 = 1, q1 = 0;
    while ( ( r0 > 0 ) && ( r1 > 0 ) ) {
	u = r0 / r1;
	r0 = r0 % r1;
	q0 = u*q1 + q0;
	if ( r0 > 0 ) {
	    u = r1 / r0;
	    r1 = r1 % r0;
	    q1 = u*q0 + q1;
	}
    }
    if ( r0 == 0 )
	return (ff_invtab[a] = -q1);
    else
	return (ff_invtab[a] = q0);
}

int ff_biginv ( const int a )
{
    INT64 u, r0 = a, r1 = ff_prime, q0 = 1, q1 = 0;
    while ( ( r0 > 0 ) && ( r1 > 0 ) ) {
	u = r0 / r1;
	r0 = r0 % r1;
	q0 = u*q1 + q0;
	if ( r0 > 0 ) {
	    u = r1 / r0;
	    r1 = r1 % r0;
	    q1 = u*q0 + q1;
	}
    }
    if ( r0 == 0 )
	return -(int)q1;
    else
	return (int)q0;
}

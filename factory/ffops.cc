// emacs edit mode for this file is -*- C++ -*-
// $Id: ffops.cc,v 1.3 1997-04-08 16:04:39 schmidt Exp $

/*
$Log: not supported by cvs2svn $
Revision 1.2  1996/06/18 12:05:42  stobbe
"ff_biginv: changed long's to long long int.
"

Revision 1.1  1996/06/18 06:57:00  stobbe
"Now the functionality to handle prime numbers that are bigger than 2^15.
This results in various changes and some new functions.
"

Revision 1.0  1996/05/17 10:59:46  stobbe
Initial revision

*/

#include <config.h>

#include "assert.h"

#include "cf_defs.h"
#include "ffops.h"

int ff_prime = 31991;
int ff_halfprime = 31991 / 2;
bool ff_big = false;
short * ff_invtab = new short [32767];

void ff_setprime ( const int p )
{
    ff_prime = p;
    ff_halfprime = ff_prime / 2;
    if ( ! ff_big )
	for ( int i = 0; i < ff_prime; i++ ) ff_invtab[i] = 0;
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
    long long int u, r0 = a, r1 = ff_prime, q0 = 1, q1 = 0;
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

// emacs edit mode for this file is -*- C++ -*-
// $Id: ffops.cc,v 1.1 1996-06-18 06:57:00 stobbe Exp $

/*
$Log: not supported by cvs2svn $
Revision 1.0  1996/05/17 10:59:46  stobbe
Initial revision

*/

#include "assert.h"
#include "cf_defs.h"
#include "ffops.h"

int ff_prime = 31991;
int ff_halfprime = 31991 / 2;
bool ff_big = false;
short * ff_invtab = new short [31991];

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
	return -q1;
    else
	return q0;
}

/* emacs edit mode for this file is -*- C++ -*- */
/* $Id: gfops.h,v 1.7 2006-05-15 08:17:53 Singular Exp $ */

#ifndef INCL_GFOPS_H
#define INCL_GFOPS_H

#include <config.h>

#ifndef NOSTREAMIO
#ifdef HAVE_IOSTREAM
#include <iostream>
#elif defined(HAVE_IOSTREAM_H)
#include <iostream.h>
#endif
#endif /* NOSTREAMIO */

#include "assert.h"

#include "cf_defs.h"
#include "canonicalform.h"

extern int gf_q;
extern int gf_p;
extern int gf_n;
extern int gf_q1;
extern int gf_m1;
extern char gf_name;

extern unsigned short * gf_table;

extern CanonicalForm gf_mipo;

//{{{ predicates
inline bool gf_iszero ( int a )
{
    return gf_q == a;
}

inline bool gf_isone ( int a )
{
    return 0 == a;
}
//}}}

//{{{ conversion functions
inline int gf_int2gf ( int i )
{
    while ( i < 0 )
	i += gf_p;
    while ( i >= gf_p )
	i -= gf_p;
    if ( i == 0 )
	return gf_q;
    int c = 0;
    while ( i > 1 ) {
	c = gf_table[c];
	i--;
    }
    return c;
}
//}}}

//{{{ zero and one
inline int gf_zero()
{
    return gf_q;
}

inline int gf_one()
{
    return 0;
}
//}}}

//{{{ inline int gf_sign ( int a )
// docu: see imm_sign()
inline
int gf_sign ( int a )
{
    if ( gf_iszero( a ) )
	return 0;
    else
	return 1;
}
//}}}

//{{{ arithmetic operators
inline int gf_neg ( int a )
{
    // -z^a=z^a*(-1)=z^a*gf_m1;
    if ( a == gf_q ) return a;
    int i = a + gf_m1;
    if ( i >= gf_q1 )
	i -= gf_q1;
    return i;
}

inline int gf_add ( int a, int b )
{
    // z^a+z^b=z^b*(z^(a-b)+1), if a>=b;
    //        =z^a*(z^(b-a)+1), if a<b;
    if ( a == gf_q ) return b;
    if ( b == gf_q ) return a;
    int zb, zab, r;
    if ( a >= b ) {
	zb = b;
	zab = a - b;
    }
    else {
	zb = a;
	zab = b - a;
    }
    if ( gf_table[zab] == gf_q )
	r = gf_q; /*if z^(a-b)+1 =0*/
    else {
	r= zb + gf_table[zab];
	if ( r >= gf_q1 )
	    r -= gf_q1;
    }
    return r;
}

inline int gf_sub ( int a, int b )
{
    return gf_add( a, gf_neg( b ) );
}

inline int gf_mul ( int a, int b )
{
    if ( a == gf_q || b == gf_q )
	return gf_q;
    else {
	int i = a + b;
	if ( i >= gf_q1 ) i -= gf_q1;
	return i;
    }
}

inline int gf_div ( int a, int b )
{
    ASSERT( b != gf_q, "divide by zero" );
    if ( a == gf_q )
	return gf_q;
    else {
	int s = a - b;
	if (s < 0)
	    s += gf_q1;
	return s;
    }
}

inline int gf_inv ( int a )
{
    ASSERT( a != gf_q, "divide by zero" );
    return gf_q1 - a;
}
//}}}

//{{{ input/output
#ifndef NOSTREAMIO
inline void gf_print ( ostream & os, int a )
{
    if ( a == gf_q )
	os << "0";
    else  if ( a == 0 )
	os << "1";
    else  if ( a == 1 )
	os << gf_name;
    else
	os << gf_name << "^" << a;
}
#endif /* NOSTREAMIO */
//}}}

//{{{ exponentation
inline int gf_power ( int a, int n )
{
    if ( n == 0 )
	return 0;
    else if ( n == 1 )
	return a;
    else
	return gf_mul( a, gf_power( a, n-1 ) );
}
//}}}

void gf_setcharacteristic ( int p, int n, char name );

// Singular needs this
/*BEGINPUBLIC*/

int gf_gf2ff ( int a );

bool gf_isff ( int a );

/*ENDPUBLIC*/

#endif /* ! INCL_GFOPS_H */

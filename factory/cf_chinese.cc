/* emacs edit mode for this file is -*- C++ -*- */
/* $Id: cf_chinese.cc,v 1.4 1997-07-19 08:13:16 schmidt Exp $ */

//{{{ docu
//
// cf_chinese.cc - algorithms for chinese remaindering.
//
// Used by: cf_gcd.cc, cf_linsys.cc, sm_util.cc
//
//}}}

#include <config.h>

#include "assert.h"

#include "cf_defs.h"
#include "canonicalform.h"

//{{{ void chineseRemainder( const CanonicalForm x1, const CanonicalForm q1, const CanonicalForm x2, const CanonicalForm q2, CanonicalForm & xnew, CanonicalForm & qnew )
//{{{ docu
//
// chineseRemainder - (simple) integer chinese remaindering.
//
// Calculate xnew such that xnew=x1 (mod q1) and xnew=x2
// (mod q2) and qnew = q1*q2.  q1 and q2 should be integers,
// pairwise prime, x1 and x2 should be integers.
//
// Note: be sure you are calculating in Z, and not in Q!
//
//}}}
void chineseRemainder( const CanonicalForm x1, const CanonicalForm q1, const CanonicalForm x2, const CanonicalForm q2, CanonicalForm & xnew, CanonicalForm & qnew )
{
    CanonicalForm a1, a2;
    (void)iextgcd( q1, q2, a1, a2 );
    qnew = q1 * q2;
    // xnew = mod( q1*a1*x2 + q2*a2*x1, qnew);
    xnew = ( q1*a1*x2 + q2*a2*x1 ) % qnew;
}
//}}}

//{{{ void chineseRemainder( const CFArray & x, const CFArray & q, CanonicalForm & xnew, CanonicalForm & qnew )
//{{{ docu
//
// chineseRemainder - integer chinese remaindering.
//
// Calculate xnew such that xnew=x[i] (mod q[i]).  q[i] should be
// integers, pairwise prime. x[i] should be integers.
//
// Note: be sure you are calculating in Z, and not in Q!
//
//}}}
void chineseRemainder( const CFArray & x, const CFArray & q, CanonicalForm & xnew, CanonicalForm & qnew )
{
    ASSERT( x.min() == q.min() && x.size() == q.size(), "incompatible arrays" );
    CFArray X(x), Q(q);
    int i, j, n = x.size();
    // we use a divide-and-conquer algorithm to reduce the number
    // of remainderings
    while ( n != 1 ) {
	i = j = x.min();
	while ( i < x.min() + n - 1 ) {
	    chineseRemainder( X[i], Q[i], X[i+1], Q[i+1], X[j], Q[j] );
	    i += 2;
	    j++;
	}
	if ( n & 1 ) {
	    X[j] = X[i];
	    Q[j] = Q[i];
	}
	n = ( n + 1) / 2;
    }
    xnew = X[x.min()];
    qnew = Q[q.min()];
}
//}}}

// emacs edit mode for this file is -*- C++ -*-
// $Id: cf_chinese.cc,v 1.1 1996-12-05 18:24:52 schmidt Exp $

/*
$Log: not supported by cvs2svn $
Revision 1.0  1996/05/17 10:59:43  stobbe
Initial revision

*/

#include "assert.h"
#include "cf_defs.h"
#include "canonicalform.h"


void chineseRemainder( const CanonicalForm x1, const CanonicalForm q1, const CanonicalForm x2, const CanonicalForm q2, CanonicalForm & xnew, CanonicalForm & qnew )
{
    CanonicalForm a1, a2;
    (void)iextgcd( q1, q2, a1, a2 );
    qnew = q1 * q2;
    xnew = ( q1*a1*x2 + q2*a2*x1 ) % qnew;
}

void chineseRemainder( const CFArray & x, const CFArray & q, CanonicalForm & xnew, CanonicalForm & qnew )
{
    ASSERT( x.min() == q.min() && x.size() == q.size(), "incompatible arrays" );
    CFArray X(x), Q(q);
    int i, j, n = x.size();
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








// emacs edit mode for this file is -*- C++ -*-
// $Id: cf_chinese.cc,v 1.0 1996-05-17 10:59:43 stobbe Exp $

/*
$Log: not supported by cvs2svn $
*/

#include "assert.h"
#include "cf_defs.h"
#include "canonicalform.h"


void chineseRemainder( const CanonicalForm & x1, const CanonicalForm & q1, const CanonicalForm & x2, const CanonicalForm & q2, CanonicalForm & xnew, CanonicalForm & qnew )
{
    CanonicalForm a1, a2;
    (void)iextgcd( q1, q2, a1, a2 );
    qnew = q1 * q2;
    xnew = ( q1*a1*x2 + q2*a2*x1 ) % qnew;
}

void chineseRemainder( const CFArray & x, const CFArray & q, CanonicalForm & xnew, CanonicalForm & qnew )
{
    CanonicalForm qq, xx;
    int i, m = x.min();
    qnew = q[m];
    xnew = x[m];
    m = x.max();
    for ( i = x.min()+1; i <= m; i++ ) {
	qq = qnew; xx = xnew;
	chineseRemainder( x[i], q[i], xx, qq, xnew, qnew );
    }
}








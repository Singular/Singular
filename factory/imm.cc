/* emacs edit mode for this file is -*- C++ -*- */
/* $Id: imm.cc,v 1.3 1998-05-11 10:38:23 schmidt Exp $ */

//{{{ docu
//
// imm.cc - some special implementations of immediate functions
//   for some special platforms.
//
//}}}

#include <config.h>

#include <stuff_64.h>

#include "imm.h"

//{{{ InternalCF * imm_mul (InternalCF * lhs, InternalCF * rhs )
//{{{ docu
//
// imm_mul() - multiplication of two immediate integers.
//
// Special implementation for MetroWerks.
//
// This function has been written by Wilfred Pohl
// <pohl@mathematik.uni-kl.de>.
//
//}}}
InternalCF *
imm_mul (InternalCF * lhs, InternalCF * rhs )
{
    int li = imm2int( lhs ), ri = imm2int( rhs );    
    Int_64 pr;
    
    if (li<0)
    {
	if (ri<0)
	    mul_64(&pr, -li, -ri);
	else
	    mul_64(&pr, -li, ri);
    }
    else
    {
	if (ri<0)
	    mul_64(&pr, li, -ri);
	else
	    mul_64(&pr, li, ri);
    }

    if (pr.hi || (pr.lo>MAXIMMEDIATE))
    {
	InternalCF * res = CFFactory::basic( IntegerDomain, li, true );
	return res->mulcoeff( rhs );
    }
    else
    {
	int res = pr.lo;
	if ((li>0) == (ri>0))
	    return int2imm(res);
	else
	    return int2imm(-res);
    }
}
//}}}

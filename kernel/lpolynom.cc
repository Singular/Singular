/****************************************
*  Computer Algebra System SINGULAR     *
****************************************/
/* $Id: lpolynom.cc,v 1.2 2008-08-07 13:18:36 Singular Exp $ */
/*
* ABSTRACT: lpolynomial definition 
*/
#include "mod2.h"

#ifdef HAVE_F5
#include "kutil.h"
#include "structs.h"
#include "omalloc.h"
#include "polys.h"
#include "p_polys.h"
#include "ideals.h"
#include "febase.h"
#include "kstd1.h"
#include "khstd.h"
#include "kbuckets.h"
#include "weight.h"
#include "intvec.h"
#include "pInline1.h"
#include "f5gb.h"
#include "lpolynom.h"


/*2
* all functions working on the class lpoly for labeled polynomials
*/

void lpoly::setPoly(poly p)
{
        polynomial = p;
}

void lpoly::setTerm(poly t)
{
        term = t;
}

void lpoly::setIndex(long i)
{
        index = i;
}

poly lpoly::getPoly()
{
        return polynomial;
}

poly lpoly::getTerm()
{
        return term;
}

long lpoly::getIndex()
{
        return index;
}


#endif

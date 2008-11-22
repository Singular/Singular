/****************************************
*  Computer Algebra System SINGULAR     *
****************************************/
/* $Id: lpolynomial.cc,v 1.1 2008-11-22 20:46:20 ederc Exp $ */
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
#include "lpolynomial.h"


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


void lpoly::setDel(bool b)
{
        del = b;
}

void lpoly::setNext(lpoly* l)
{
        next = l;
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

bool lpoly::getDel()
{
        return del;
}

lpoly *lpoly::getNext()
{
        return next;
}

#endif

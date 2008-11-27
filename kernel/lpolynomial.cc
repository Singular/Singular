/****************************************
*  Computer Algebra System SINGULAR     *
****************************************/
/* $Id: lpolynomial.cc,v 1.2 2008-11-27 17:18:45 ederc Exp $ */
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
#include "lplist.h"
/*
================================================================
all functions working on the class LPoly for labeled polynomials
================================================================
*/
void LPoly::setPoly(poly p) 
{
    polynomial = p;
}

void LPoly::setTerm(poly t) {
    term = t;
}

void LPoly::setIndex(long i) {
    index = i;
}


void LPoly::setDel(bool b) {
    del = b;
}

void LPoly::setNext(LPoly* l) {
    next = l;
}

poly LPoly::getPoly() {
    return polynomial;
}

poly LPoly::getTerm() {
    return term;
}

long LPoly::getIndex() {
    return index;
}

bool LPoly::getDel() {
    return del;
}

LPoly* LPoly::getNext() {
    return next;
}

/*
=====================================================
comparing two labeled polynomials by their signatures
=====================================================
*/
int LPoly::compare(const LPoly& lp) {
    if(index > lp.index) {
        return 1;
    }
    if(index < lp.index) {
        return -1;
    }
    return 0;
}

void LPoly::get() {
    Print("Label: ");
    pWrite(getTerm());
    Print("  %d\n",getIndex());
}
#endif

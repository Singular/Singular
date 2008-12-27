/****************************************
*  Computer Algebra System SINGULAR     *
****************************************/
/* $Id: lpolynomial.cc,v 1.4 2008-12-27 13:50:06 ederc Exp $ */
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
#include "lists.h"
/*
================================================================
all functions working on the class LPoly for labeled polynomials
================================================================
*/
LPoly::LPoly(poly* t,long* i,poly* p) {
    set(t,i,p);
}
void LPoly::setPoly(poly* p)  {
    polynomial = *p;
}

void LPoly::setTerm(poly* t) {
    term = *t;
}

void LPoly::setIndex(long* i) {
    index = *i;
}


void LPoly::setDel(bool b) {
    del = b;
}

poly* LPoly::getPoly() {
    return &polynomial;
}

poly* LPoly::getTerm() {
    return &term;
}

long* LPoly::getIndex() {
    return &index;
}

bool LPoly::getDel() const {
    return del;
}

void LPoly::set(poly* t, long* i, poly* p) {
    this->setTerm(t);
    this->setIndex(i);
    this->setPoly(p);
}

LPoly* LPoly::get() {
    return this;
}
#endif

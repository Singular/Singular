/****************************************
*  Computer Algebra System SINGULAR     *
****************************************/
/* $Id: lpolynomial.cc,v 1.5 2009-01-15 17:44:24 ederc Exp $ */
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
LPoly::LPoly(poly* t,int* i,poly* p) {
    set(t,i,p);
}

void LPoly::setPoly(poly* p)  {
    polynomial = *p;
}

void LPoly::setTerm(poly* t) {
    term = *t;
}

void LPoly::setIndex(int* i) {
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

int* LPoly::getIndex() {
    return &index;
}

bool LPoly::getDel() const {
    return del;
}

void LPoly::set(poly* t, int* i, poly* p) {
    this->setTerm(t);
    this->setIndex(i);
    this->setPoly(p);
}

LPoly* LPoly::get() {
    return this;
}

/*
====================================
functions working on the class CPair
====================================
*/
CPair::CPair(int degree, poly term1, LPoly* lpoly1, poly term2, LPoly* lpoly2) {
   deg  =   degree;
   t1   =   term1;
   lp1  =   lpoly1;
   t2   =   term2;
   lp2  =   lpoly2;
}

int CPair::getDeg() {
    return deg;
}

poly CPair::getT1() {
    return t1;
}

poly CPair::getT2() {
    return t2;
}

poly CPair::getLp1Poly() {
    return *(lp1->getPoly());
}

poly CPair::getLp2Poly() {
    return *(lp2->getPoly());
}

poly CPair::getLp1Term() {
    return *(lp1->getTerm());
}

poly CPair::getLp2Term() {
    return *(lp2->getTerm());
}

int CPair::getLp1Index() {
    return *(lp1->getIndex());
}


int CPair::getLp2Index() {
    return *(lp2->getIndex());
}


#endif

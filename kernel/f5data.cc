/****************************************
*  Computer Algebra System SINGULAR     *
****************************************/
/* $Id: f5data.cc,v 1.4 2009-02-08 19:17:54 ederc Exp $ */
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
#include "f5data.h"
#include "f5lists.h"
/*
================================================================
all functions working on the class LPoly for labeled polynomials
================================================================
*/
LPoly::LPoly(poly t,int i,poly p) {
    set(t,i,p);
}

void LPoly::setPoly(poly p)  {
    polynomial = p;
}

void LPoly::setTerm(poly t) {
    term = t;
}

void LPoly::setIndex(int i) {
    index = i;
}

poly LPoly::getPoly() {
    return polynomial;
}

poly LPoly::getTerm() {
    return term;
}

int LPoly::getIndex() {
    return index;
}

void LPoly::set(poly t, int i, poly p) {
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
CPair::CPair(long degree, poly term1, LPoly* lpoly1, poly term2, LPoly* lpoly2) {
   deg              =   degree;
   t1               =   term1;
   lp1              =   lpoly1;
   t2               =   term2;
   lp2              =   lpoly2;
   lastRuleTested   =   NULL;
}

long CPair::getDeg() {
    return deg;
}

poly CPair::getT1() {
    return t1;
}

poly* CPair::getAdT1() {
    return &t1;
}

poly* CPair::getAdT2() {
    return &t2;
}

poly CPair::getT2() {
    return t2;
}

LPoly* CPair::getAdLp1() {
    return lp1;
}

LPoly* CPair::getAdLp2() {
    return lp2;
}

poly CPair::getLp1Poly() {
    return lp1->getPoly();
}

poly CPair::getLp2Poly() {
    return lp2->getPoly();
}

poly CPair::getLp1Term() {
    return lp1->getTerm();
}

poly CPair::getLp2Term() {
    return lp2->getTerm();
}

int CPair::getLp1Index() {
    return lp1->getIndex();
}

int CPair::getLp2Index() {
    return lp2->getIndex();
}

Rule* CPair::getLastRuleTested() {
    return lastRuleTested;
}

/*
===================================
functions working on the class Rule
===================================
*/
Rule::Rule(int i, poly t, LPoly* l) {
    index   =   i;
    term    =   t;
    origin  =   l;
}

int Rule::getIndex() {
    return index;
}

poly Rule::getTerm() {
    return term;
}

LPoly* Rule::getOrigin() {
    return origin;
}

void Rule::setOrigin(LPoly* l) {
    origin  =   l;
}
#endif

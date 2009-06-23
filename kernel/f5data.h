//! \file f5data.cc
/****************************************
*  Computer Algebra System SINGULAR     *
****************************************/
/* $Id: f5data.h,v 1.12 2009-06-23 10:08:15 ederc Exp $ */
/*
* ABSTRACT: labeled polynomial interface
*/
#ifndef F5DATA_HEADER
#define F5DATA_HEADER
#ifdef HAVE_F5
/*!
=========================================================
=========================================================
classes for labeled polynomials/pairs/S-polynomials in F5
=========================================================
=========================================================
*/
class LPoly;
class CPair;
class Rule;


/*!
============================
class of labeled polynomials
============================
*/
class LPoly {
    private:
        poly    term;           //term of signature
        int     index;          //index of signature
        poly    polynomial;     //standard polynomial data
        Rule*   _rule;
        bool    del;
    public:
        inline          LPoly(poly t, int i, poly p, Rule* r=NULL);
        inline  void    setPoly(poly p);
        inline  poly    getPoly();
        inline  void    setTerm(poly t);
        inline  poly    getTerm();
        inline  void    setIndex(int i);
        inline  int     getIndex();
        inline  void    setRule(Rule* r);
        inline  Rule*   getRule();
        inline  void    setDel(bool d);
        inline  bool    getDel();
        inline  void    set(poly t, int i, poly p, Rule* r);
        inline  LPoly*  get();
};

LPoly::LPoly(poly t,int i,poly p, Rule* r) {
    set(t,i,p,r);
    del =   0;
}

void LPoly::setPoly(poly p)  {
    //poly _p     =   pInit();
    //_p          =   pCopy(p);
    polynomial = p;
}

void LPoly::setTerm(poly t) {
    //poly _t     =   pInit();
    //_t          =   pCopy(t);
    term = t;
}

void LPoly::setIndex(int i) {
    index = i;
}

void LPoly::setRule(Rule* r) {
    _rule   =   r;
}

void LPoly::setDel(bool d) {
    del =   d;
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

Rule* LPoly::getRule() {
    return _rule;
}

bool LPoly::getDel() {
    return del;
}

void LPoly::set(poly t, int i, poly p, Rule* r) {
    this->setTerm(t);
    this->setIndex(i);
    this->setPoly(p);
    this->setRule(r);
}

LPoly* LPoly::get() {
    return this;
}


/*!
===================================
structure of labeled critical pairs
===================================
*/
class CPair {
    private:
        long    deg;            // total degree of the critical pair
        poly    t1;             // first term for label
        LPoly*  lp1;            // first labeled poly
        poly    t2;             // second term for label
        LPoly*  lp2;            // second labeled poly
        Rule*   testedRule;     // already tested by rules up to lastRuleTested
    public:
        inline          CPair(long degree, poly term1, LPoly* lpoly1, poly term2, LPoly* lpoly2, Rule* r = NULL);
        inline  long    getDeg();
        inline  poly    getT1();
        inline  poly*   getAdT1();
        inline  LPoly*  getAdLp1();
        inline  poly    getLp1Poly();
        inline  poly    getLp1Term();
        inline  int     getLp1Index();
        inline  poly    getT2();
        inline  poly*   getAdT2();
        inline  LPoly*  getAdLp2();
        inline  poly    getLp2Poly();
        inline  poly    getLp2Term();
        inline  int     getLp2Index();
        inline  Rule*   getTestedRule();
        inline  void    setTestedRule(Rule* r);
};

CPair::CPair(long degree, poly term1, LPoly* lpoly1, poly term2, LPoly* lpoly2, Rule* r) {
   deg              =   degree;
   t1               =   term1;
   lp1              =   lpoly1;
   t2               =   term2;
   lp2              =   lpoly2;
   testedRule       =   r;
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

Rule* CPair::getTestedRule() {
    return testedRule;
}

void CPair::setTestedRule(Rule* r) {
    testedRule      =   r;
}


/*!
========================================================
structure of rules(i.e. already computed / known labels)
========================================================
*/
class Rule {
    private:
        int     index;      // index of the labeled polynomial the rule comes from 
        poly    term;       // term of the labeled polynomial the rule comes from
    public:
        inline          Rule(int i, poly term);
        inline          ~Rule();
        inline  int     getIndex();
        inline  poly    getTerm();
};

Rule::Rule(int i, poly t) {
    index   =   i;
    term    =   t;
}

Rule::~Rule() {
    //pDelete(&term);
}

int Rule::getIndex() {
    return index;
}

poly Rule::getTerm() {
    return term;
}
#endif
#endif

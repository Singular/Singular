//! \file f5data.cc
/****************************************
*  Computer Algebra System SINGULAR     *
****************************************/
/* $Id$ */
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
class LPolyOld;
class CPairOld;
class RuleOld;


/*!
============================
class of labeled polynomials
============================
*/
class LPolyOld {
    private:
        poly    term;           //term of signature
        int     index;          //index of signature
        poly    polynomial;     //standard polynomial data
        RuleOld*   _RuleOld;
        bool    del;
    public:
        inline          LPolyOld(poly t, int i, poly p, RuleOld* r=NULL);
        inline  void    setPoly(poly p);
        inline  poly    getPoly();
        inline  void    setTerm(poly t);
        inline  poly    getTerm();
        inline  void    setIndex(int i);
        inline  int     getIndex();
        inline  void    setRuleOld(RuleOld* r);
        inline  RuleOld*   getRuleOld();
        inline  void    setDel(bool d);
        inline  bool    getDel();
        inline  void    set(poly t, int i, poly p, RuleOld* r);
        inline  LPolyOld*  get();
};

LPolyOld::LPolyOld(poly t,int i,poly p, RuleOld* r) {
    set(t,i,p,r);
    del =   0;
}

void LPolyOld::setPoly(poly p)  {
    //poly _p     =   pInit();
    //_p          =   pCopy(p);
    polynomial = p;
}

void LPolyOld::setTerm(poly t) {
    //poly _t     =   pInit();
    //_t          =   pCopy(t);
    term = t;
}

void LPolyOld::setIndex(int i) {
    index = i;
}

void LPolyOld::setRuleOld(RuleOld* r) {
    _RuleOld   =   r;
}

void LPolyOld::setDel(bool d) {
    del =   d;
}

poly LPolyOld::getPoly() {
    return polynomial;
}

poly LPolyOld::getTerm() {
    return term;
}

int LPolyOld::getIndex() {
    return index;
}

RuleOld* LPolyOld::getRuleOld() {
    return _RuleOld;
}

bool LPolyOld::getDel() {
    return del;
}

void LPolyOld::set(poly t, int i, poly p, RuleOld* r) {
    this->setTerm(t);
    this->setIndex(i);
    this->setPoly(p);
    this->setRuleOld(r);
}

LPolyOld* LPolyOld::get() {
    return this;
}


/*!
===================================
structure of labeled critical pairs
===================================
*/
class CPairOld {
    private:
        long    deg;            // total degree of the critical pair
        poly    t1;             // first term for label
        LPolyOld*  lp1;            // first labeled poly
        poly    t2;             // second term for label
        LPolyOld*  lp2;            // second labeled poly
        RuleOld*   testedRuleOld;     // already tested by RuleOlds up to lastRuleOldTested
        bool  del;
    public:
        inline          CPairOld(long degree, poly term1, LPolyOld* LPolyOld1, poly term2, LPolyOld* LPolyOld2, bool useless, RuleOld* r = NULL);
        inline  long    getDeg();
        inline  poly    getT1();
        inline  poly*   getAdT1();
        inline  LPolyOld*  getAdLp1();
        inline  poly    getLp1Poly();
        inline  poly    getLp1Term();
        inline  int     getLp1Index();
        inline  poly    getT2();
        inline  poly*   getAdT2();
        inline  LPolyOld*  getAdLp2();
        inline  poly    getLp2Poly();
        inline  poly    getLp2Term();
        inline  int     getLp2Index();
        inline  bool    getDel();
        inline  RuleOld*   getTestedRuleOld();
        inline  void    setTestedRuleOld(RuleOld* r);
};

CPairOld::CPairOld(long degree, poly term1, LPolyOld* LPolyOld1, poly term2, LPolyOld* LPolyOld2, bool useless, RuleOld* r) {
   deg              =   degree;
   t1               =   term1;
   lp1              =   LPolyOld1;
   t2               =   term2;
   lp2              =   LPolyOld2;
   testedRuleOld       =   r;
   del              =   useless;
}

long CPairOld::getDeg() {
    return deg;
}

poly CPairOld::getT1() {
    return t1;
}

poly* CPairOld::getAdT1() {
    return &t1;
}

poly* CPairOld::getAdT2() {
    return &t2;
}

poly CPairOld::getT2() {
    return t2;
}

LPolyOld* CPairOld::getAdLp1() {
    return lp1;
}

LPolyOld* CPairOld::getAdLp2() {
    return lp2;
}

poly CPairOld::getLp1Poly() {
    return lp1->getPoly();
}

poly CPairOld::getLp2Poly() {
    return lp2->getPoly();
}

poly CPairOld::getLp1Term() {
    return lp1->getTerm();
}

poly CPairOld::getLp2Term() {
    return lp2->getTerm();
}

int CPairOld::getLp1Index() {
    return lp1->getIndex();
}

int CPairOld::getLp2Index() {
    return lp2->getIndex();
}

bool CPairOld::getDel() {
  return del;
}

RuleOld* CPairOld::getTestedRuleOld() {
    return testedRuleOld;
}

void CPairOld::setTestedRuleOld(RuleOld* r) {
    testedRuleOld      =   r;
}


/*!
========================================================
structure of RuleOlds(i.e. already computed / known labels)
========================================================
*/
class RuleOld {
    private:
        int     index;      // index of the labeled polynomial the RuleOld comes from 
        poly    term;       // term of the labeled polynomial the RuleOld comes from
    public:
        inline          RuleOld(int i, poly term);
        inline          ~RuleOld();
        inline  int     getIndex();
        inline  poly    getTerm();
};

RuleOld::RuleOld(int i, poly t) {
    index   =   i;
    term    =   t;
}

RuleOld::~RuleOld() {
    //pDelete(&term);
}

int RuleOld::getIndex() {
    return index;
}

poly RuleOld::getTerm() {
    return term;
}
#endif
#endif

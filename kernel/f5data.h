/****************************************
*  Computer Algebra System SINGULAR     *
****************************************/
/* $Id: f5data.h,v 1.5 2009-02-11 21:24:07 ederc Exp $ */
/*
* ABSTRACT: labeled polynomial interface
*/
#ifndef F5DATA_HEADER
#define F5DATA_HEADER
#ifdef HAVE_F5
/*
=========================================================
=========================================================
classes for labeled polynomials/pairs/S-polynomials in F5
=========================================================
=========================================================
*/
class LPoly;
class CPair;
class Rule;


/*
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
    public:
                LPoly(poly t, int i, poly p, Rule* r=NULL);
        void    setPoly(poly p);
        poly    getPoly();
        void    setTerm(poly t);
        poly    getTerm();
        void    setIndex(int i);
        int     getIndex();
        void    setRule(Rule* r);
        Rule*   getRule();
        void    set(poly t, int i, poly p, Rule* r);
        LPoly*  get();
};

/*
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
        Rule*   lastRuleTested; // already tested by rules up to lastRuleTested
    public:
                CPair(long degree, poly term1, LPoly* lpoly1, poly term2, LPoly* lpoly2);
        long    getDeg();
        poly    getT1();
        poly*   getAdT1();
        LPoly*  getAdLp1();
        poly    getLp1Poly();
        poly    getLp1Term();
        int     getLp1Index();
        poly    getT2();
        poly*   getAdT2();
        LPoly*  getAdLp2();
        poly    getLp2Poly();
        poly    getLp2Term();
        int     getLp2Index();
        Rule*   getLastRuleTested();
};


/*
========================================================
structure of rules(i.e. already computed / known labels)
========================================================
*/
class Rule {
    private:
        int     index;      // index of the labeled polynomial the rule comes from 
        poly    term;       // term of the labeled polynomial the rule comes from
    public:
                Rule(int i, poly term);
        int     getIndex();
        poly    getTerm();
};
#endif
#endif

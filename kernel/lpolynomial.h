/****************************************
*  Computer Algebra System SINGULAR     *
****************************************/
/* $Id: lpolynomial.h,v 1.6 2009-01-25 17:13:06 ederc Exp $ */
/*
* ABSTRACT: labeled polynomial interface
*/
#ifndef LPOLYNOMIAL_HEADER
#define LPOLYNOMIAL_HEADER

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
        poly    term; //term of signature
        int     index; //index of signature
        poly    polynomial; //standard polynomial data
        bool    del; //for deletion in TopReduction Subalgorithm
    public:
                LPoly(poly*t,int* i,poly* p); 
        void    setPoly(poly* p);
        poly*   getPoly();
        void    setTerm(poly* t);
        poly*   getTerm();
        void    setIndex(int* i);
        int*    getIndex();
        void    setDel(bool b);
        bool    getDel() const;
        void    set(poly* t, int* i, poly* p);
        LPoly*  get();
};


/*
===================================
structure of labeled critical pairs
===================================
*/
class CPair {
    private:
        long    deg;    // total degree of the critical pair
        poly    t1;     // first term for label
        LPoly*  lp1;     // first labeled poly
        poly    t2;     // second term for label
        LPoly*  lp2;     // second labeled poly
    public:
                CPair(long degree, poly term1, LPoly* lpoly1, poly term2, LPoly* lpoly2);
        long    getDeg();
        poly    getT1();
        poly    getLp1Poly();
        poly    getLp1Term();
        int     getLp1Index();
        poly    getT2();
        poly    getLp2Poly();
        poly    getLp2Term();
        int     getLp2Index();
};


/*
========================================================
structure of rules(i.e. already computed / known labels)
========================================================
*/
class Rule {
    private:
        int*     index;    // index of the labeled polynomial the rule comes from 
        poly*    term;     // term of the labeled polynomial the rule comes from
    public:
                Rule(int* i, poly* term);
        int     getIndex();
        poly    getTerm();
};
#endif
#endif

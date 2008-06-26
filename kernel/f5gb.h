/****************************************
*  Computer Algebra System SINGULAR     *
****************************************/
/* $Id: f5gb.h,v 1.6 2008-06-26 16:05:21 ederc Exp $ */
/*
* ABSTRACT: f5gb interface
*/
#ifndef F5_HEADER
#define F5_HEADER
#include "mod2.h"

#ifdef HAVE_F5


// structure of the rules, i.e. index and term
struct rule{
        long index;
        poly term;
};


// class of a labeled polynomial 
class lpoly {
        private:
                poly term;
                long index;
                poly polynomial;

        public:
                void setPoly(poly p);
                poly getPoly();
                void setTerm(poly t);
                poly getTerm();
                void setIndex(long i);
                long getIndex();
};




// sort polynomials in ideal i by decreasing total degree
void qsort_degree(poly* left, poly* right);


// computes incrementally gbs of subsets of the input 
// gb{f_m} -> gb{f_m,f_(m-1)} -> gb{f_m,...,f_1}  
lpoly* f5_inc(lpoly* lp, lpoly* g_prev);


// main function of our f5 implementation
ideal F5main(ideal i, ring r);
#endif
#endif


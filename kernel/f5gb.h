/****************************************
*  Computer Algebra System SINGULAR     *
****************************************/
/* $Id: f5gb.h,v 1.5 2008-06-01 15:15:00 ederc Exp $ */
/*
* ABSTRACT: f5gb interface
*/
#ifndef F5_HEADER
#define F5_HEADER
#include "mod2.h"

#ifdef HAVE_F5


class lpoly {
        private:
                poly* t_ptr;
                long* i_ptr;
                poly* p_ptr;

        public:
                void setPoly(poly* p);
                poly* getPoly();
                void setTerm(poly* t);
                poly* getTerm();
                void setIndex(long* i);
                long* getIndex();
};




// sort polynomials in ideal i by decreasing total degree
void qsort_degree(poly* left, poly* right);

// main function of our f5 implementation
ideal F5main(ideal i, ring r);
#endif
#endif


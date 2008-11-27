/****************************************
*  Computer Algebra System SINGULAR     *
****************************************/
/* $Id: lpolynomial.h,v 1.2 2008-11-27 17:19:22 ederc Exp $ */
/*
* ABSTRACT: labeled polynomial interface
*/
#ifndef LPOLYNOMIAL_HEADER
#define LPOLYNOMIAL_HEADER

#ifdef HAVE_F5


// class of a labeled polynomial 
class LPoly
{
        private:
                poly    term; //term of signature
                long    index; //index of signature
                poly    polynomial; //standard polynomial data
                bool    del; //for deletion in TopReduction Subalgorithm
                LPoly   *next; //pointer on the next labeled polynomial in the list
        public:
                void    setPoly(poly p);
                poly    getPoly();
                void    setTerm(poly t);
                poly    getTerm();
                void    setIndex(long i);
                long    getIndex();
                void    setDel(bool b);
                bool    getDel();
                void    setNext(LPoly *l);
                LPoly   *getNext();
                int     compare(const LPoly& lp);
                void    get();
};


// structure of the critical pairs, just the addresses of the corresponding generator labeled polynomials
struct critpair
{
        LPoly       *cp1;   // first  component
        LPoly       *cp2;   // second component
        critpair    *next;  // next critical pair
};


#endif
#endif



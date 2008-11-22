/****************************************
*  Computer Algebra System SINGULAR     *
****************************************/
/* $Id: lpolynomial.h,v 1.1 2008-11-22 20:47:27 ederc Exp $ */
/*
* ABSTRACT: f5gb interface
*/
#ifndef LPOLYNOMIAL_HEADER
#define LPOLYNOMIAL_HEADER

#ifdef HAVE_F5


// class of a labeled polynomial 
class lpoly
{
        private:
                poly    term; //term of signature
                long    index; //index of signature
                poly    polynomial; //standard polynomial data
                bool    del; //for deletion in TopReduction Subalgorithm
                lpoly   *next; //pointer on the next labeled polynomial in the list
        public:
                void    setPoly(poly p);
                poly    getPoly();
                void    setTerm(poly t);
                poly    getTerm();
                void    setIndex(long i);
                long    getIndex();
                void    setDel(bool b);
                bool    getDel();
                void    setNext(lpoly *l);
                lpoly   *getNext();
};


// structure of the critical pairs, just the addresses of the corresponding generator labeled polynomials
struct critpair
{
        lpoly       *cp1;   // first  component
        lpoly       *cp2;   // second component
        critpair    *next;  // next critical pair
};


#endif
#endif



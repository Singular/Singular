/****************************************
*  Computer Algebra System SINGULAR     *
****************************************/
/* $Id: lpolynomial.h,v 1.3 2008-12-26 13:52:15 ederc Exp $ */
/*
* ABSTRACT: labeled polynomial interface
*/
#ifndef LPOLYNOMIAL_HEADER
#define LPOLYNOMIAL_HEADER

#ifdef HAVE_F5


/*
=============================
class of a labeled polynomial
=============================
*/
class LPoly
{
        private:
                poly    term; //term of signature
                long    index; //index of signature
                poly    polynomial; //standard polynomial data
                bool    del; //for deletion in TopReduction Subalgorithm
        public:
                void    setPoly(poly* p);
                poly    getPoly() const;
                void    setTerm(poly* t);
                poly    getTerm() const;
                void    setIndex(long* i);
                long    getIndex() const;
                void    setDel(bool b);
                bool    getDel() const;
                int     compare(const LPoly& lp) const;
                void    set(poly* t, long* i, poly* p);
                LPoly*  get();
};


/*
===============================
structure of the critical pairs
===============================
*/
struct CPair
{
        LPoly*  cp1;   // first  component
        LPoly*  cp2;   // second component
};


#endif
#endif

/****************************************
*  Computer Algebra System SINGULAR     *
****************************************/
/* $Id: lpolynom.h,v 1.2 2008-08-07 13:18:36 Singular Exp $ */
/*
* ABSTRACT: f5gb interface
*/
#ifndef LPOLYNOM_HEADER
#define LPOLYNOM_HEADER

#ifdef HAVE_F5


// class of a labeled polynomial 
class lpoly
{
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

#endif
#endif



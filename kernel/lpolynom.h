/****************************************
*  Computer Algebra System SINGULAR     *
****************************************/
/* $Id: lpolynom.h,v 1.1 2008-08-05 20:44:08 ederc Exp $ */
/*
* ABSTRACT: f5gb interface
*/
#ifndef LPOLYNOM_HEADER
#define LPOLYNOM_HEADER
#include "mod2.h"

#ifdef HAVE_F5


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

#endif
#endif



/* emacs edit mode for this file is -*- C++ -*- */
/* $Id: fac_iterfor.h,v 1.5 2006-05-15 08:17:52 Singular Exp $ */

#ifndef INCL_FAC_ITERFOR_H
#define INCL_FAC_ITERFOR_H

#include <config.h>

#ifndef NOSTREAMIO
#ifdef HAVE_IOSTREAM
#include <iostream>
#elif defined(HAVE_IOSTREAM_H)
#include <iostream.h>
#endif
#endif /* NOSTREAMIO */

class IteratedFor
{
private:
    int MAX;
    int FROM;
    int TO;
    int N;
    bool last;
    int * index;
    int * imax;
    void fill ( int from, int n );
public:
    IteratedFor( int from, int to, int max );
    IteratedFor( const IteratedFor & );
    ~IteratedFor();
    IteratedFor& operator= ( const IteratedFor & );
    int from() const { return FROM; }
    int to() const { return TO; }
    int n() const { return N; };
    int max() const { return MAX; }
    void nextiteration();
    void operator++ () { nextiteration(); }
    void operator++ ( int ) { nextiteration(); }
    bool iterations_left() const { return ! last; }
    int operator[] ( int ) const;
};

#ifndef NOSTREAMIO
ostream& operator<< ( ostream &, const IteratedFor & );
#endif /* NOSTREAMIO */

#endif /* ! INCL_FAC_ITERFOR_H */

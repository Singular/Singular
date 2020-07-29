/* emacs edit mode for this file is -*- C++ -*- */

#ifndef INCL_FAC_ITERFOR_H
#define INCL_FAC_ITERFOR_H

// #include "config.h"

#ifndef NOSTREAMIO
#ifdef HAVE_IOSTREAM
#include <iostream>
#define OSTREAM std::ostream
#elif defined(HAVE_IOSTREAM_H)
#include <iostream.h>
#define OSTREAM ostream
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
OSTREAM& operator<< ( OSTREAM &, const IteratedFor & );
#endif /* NOSTREAMIO */

#endif /* ! INCL_FAC_ITERFOR_H */

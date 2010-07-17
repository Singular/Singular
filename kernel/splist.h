// ----------------------------------------------------------------------------
//  splist.h
//  begin of file
//  Stephan Endrass, endrass@mathematik.uni-mainz.de
//  23.7.99
// ----------------------------------------------------------------------------

// ----------------------------------------------------------------------------
//  Description: the class  spectrumPolyList  is to hold monomials
//  and their normal forms with respect to a given standard basis
//  and a given ordering. On this list we run a standard basis
//  conversion algorithm to compute  the spectrum
// ----------------------------------------------------------------------------

#ifndef SPLIST_H
#define SPLIST_H

#include <kernel/npolygon.h>

enum    spectrumState
{
    spectrumOK,
    spectrumZero,
    spectrumBadPoly,
    spectrumNoSingularity,
    spectrumNotIsolated,
    spectrumDegenerate,
    spectrumWrongRing,
    spectrumNoHC,
    spectrumUnspecErr
};

// ----------------------------------------------------------------------------

class spectrumPolyNode
{
public:

    spectrumPolyNode    *next;
    poly                mon;
    Rational            weight;
    poly                nf;

    spectrumPolyNode( );
    spectrumPolyNode( spectrumPolyNode*,poly,const Rational&,poly );
    ~spectrumPolyNode( );

    void    copy_zero   ( void );
    void    copy_shallow( spectrumPolyNode*,poly,const Rational&,poly );
    void    copy_shallow( spectrumPolyNode& );
};

// ----------------------------------------------------------------------------

class spectrumPolyList
{
public:

    spectrumPolyNode       *root;
    int             N;
    newtonPolygon   *np;

    spectrumPolyList( );
    spectrumPolyList( newtonPolygon* );
    ~spectrumPolyList( );

    void    copy_zero   ( void );
    void    copy_shallow( spectrumPolyNode*,int,newtonPolygon* );
    void    copy_shallow( spectrumPolyList& );

    void    insert_node( poly,poly );
    void    delete_node( spectrumPolyNode** );

    void    delete_monomial( poly );

    spectrumState  spectrum( lists*,int );

    #ifdef SPLIST_PRINT
        friend ostream & operator << ( ostream&,const spectrumPolyList& );
    #endif
};

#endif /* SPLIST_H */

// ----------------------------------------------------------------------------
//  splist.h
//  end of file
// ----------------------------------------------------------------------------

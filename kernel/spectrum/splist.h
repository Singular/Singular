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

#include <kernel/spectrum/npolygon.h>

/*enum    spectrumState
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
};*/ //TODO move to Singular

// ----------------------------------------------------------------------------

class spectrumPolyNode
{
public:

    spectrumPolyNode    *next;
    poly                mon;
    Rational            weight;
    poly                nf;
    ring                r;

    spectrumPolyNode( );
    spectrumPolyNode( spectrumPolyNode*,poly,const Rational&,poly, const ring);
    ~spectrumPolyNode( );

    void    copy_zero   ( void );
    void    copy_shallow( spectrumPolyNode*,poly,const Rational&,poly, const ring);
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

    void    insert_node( poly,poly, const ring );
    void    delete_node( spectrumPolyNode** );

    void    delete_monomial( poly, const ring );

    //spectrumState  spectrum( lists*,int );

    #ifdef SPLIST_PRINT
        friend ostream & operator << ( ostream&,const spectrumPolyList& );
    #endif
};

#endif /* SPLIST_H */

// ----------------------------------------------------------------------------
//  splist.h
//  end of file
// ----------------------------------------------------------------------------

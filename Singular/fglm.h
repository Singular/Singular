// emacs edit mode for this file is -*- C++ -*-
// $Id: fglm.h,v 1.1.1.1 1997-03-19 13:18:54 obachman Exp $
//=============================================
//          Header-File for fglm.cc 
//=============================================

#ifndef FGLM_H
#define FGLM_H
#include "polys.h"
#include "fglmvec.h"
#include <iostream.h>

class idealFunctionals;

struct divInfo;

class listElem
{
    int insertions;
    int numVars;
    poly monom;
    divInfo * divisors; //. runs from divisors[0]..divisors[insertions-1] (max numVars-1)
public:
    listElem();
    listElem( poly p, int basis, int var );
    void cleanup();
    poly & getMonom();
    BOOLEAN isBasisOrEdge() const;
    void newDivisor( int basis, int var );
    void updateFunctionals( int basis, idealFunctionals & l ) const;
    void updateFunctionals( const fglmVector v, idealFunctionals & l ) const;
};
ostream & operator << ( ostream &, const listElem & );

class fglmElem
{
private:
    int insertions;  // decreases during calcuation
    poly monom;
    fglmVector v;   // monom = poly(v) * var;
    int var;
public:
    fglmElem( poly & m, fglmVector mv, int v );

    void cleanup();
    BOOLEAN isBasisOrEdge() const;
    poly & getMonom();
    fglmVector getVector();
    int getVar();
    int getInsertions();
    void newDivisor();
};
ostream & operator << ( ostream &, const fglmElem & );

ideal
fglmProc( leftv first, leftv secod );
#endif






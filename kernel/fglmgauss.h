// emacs edit mode for this file is -*- C++ -*-
// $Id$

/****************************************
*  Computer Algebra System SINGULAR     *
****************************************/
/*
* ABSTRACT - header file for the gauss - reducer
*            (used by fglm)
*/
#ifndef FGLMGAUSS_H
#define FGLMGAUSS_H

#include <kernel/structs.h>
#include <kernel/fglmvec.h>

class gaussElem;

class gaussReducer
{
private:
    gaussElem * elems;
    BOOLEAN * isPivot;
    int * perm;
    fglmVector v;
    fglmVector p;
    number pdenom;
    int size;
    int max;
public:
    gaussReducer( int dimen );
    ~gaussReducer();

    // reduce returns TRUE, if v reduces to 0, FALSE otherwise;
    BOOLEAN reduce( fglmVector v );

    // if a vector does not reduce to zero, then it can be stored as a new gauss
    // vector.
    // Has to be called after reduce!
    void store();

    // if a vector reduces to zero, then one can get the corresponding fglmVector
    // of the linear dependence
    // Has to be called after reduce!
    fglmVector getDependence();
};

#endif

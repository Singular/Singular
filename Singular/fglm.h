// emacs edit mode for this file is -*- C++ -*-
// $Id: fglm.h,v 1.6 1997-10-18 11:01:57 Singular Exp $

/****************************************
*  Computer Algebra System SINGULAR     *
****************************************/
/* 
* ABSTRACT - The FGLM-Algorithm
*   The main header file for the fglm algorithm          
*   (See fglm.cc for details)
*/

#ifndef FGLM_H
#define FGLM_H

#include "polys.h"
#include "fglmvec.h"

#define PROT(msg)
#define STICKYPROT(msg) if (BTEST1(OPT_PROT)) Print(msg)
#define PROT2(msg,arg)
#define STICKYPROT2(msg,arg) if (BTEST1(OPT_PROT)) Print(msg,arg)
#define fglmASSERT(ignore1,ignore2)

// internal Version: 1.10.1.4
// Some data types needed by the fglm algorithm. claptmpl.cc has to know them.
class fglmSelem
{
public:
    int numVars;
    poly monom;
    int * divisors;
    fglmSelem( poly p, int var );

    void cleanup();
    BOOLEAN isBasisOrEdge() const { return ( (divisors[0] == numVars) ? TRUE : FALSE ); }
    void newDivisor( int var ) { divisors[ ++divisors[0] ]= var; }
};

class fglmDelem
{
public:
    int insertions;  
    poly monom;
    fglmVector v;   
    int var;
    fglmDelem( poly & m, fglmVector mv, int v );

    void cleanup();
    BOOLEAN isBasisOrEdge() const { return ( (insertions == 0) ? TRUE : FALSE ); }
    void newDivisor() { insertions--; }
};

// fglmzero(...):
// The fglm algorithm for 0-dimensional ideals. ( fglmzero is defined in fglmzero.cc )
// Calculates the reduced groebner basis of sourceIdeal in destRing.
// The sourceIdeal has to be a reduced, 0-dimensional groebner basis in sourceRing.
// Warning: There is no check, if the ideal is really 0-dimensional and minimal.
// If it is minimal but not reduced, then it returns FALSE, otherwise TRUE.
// Warning: There is no check, if the rings are compatible for fglm (see 
// fglm.cc for functions to check this)
// if switchBack==TRUE, then the procedure sets the ring as currentRing which was
// current when it was called ( When called there may be currRing != sourceRing ).
// if switchBack==FALSE, then currRing==destRing at the end.
// if deleteIdeal==TRUE then sourceIdeal is deleted (in any case, even if the
// procedure fails)
// if deleteIdeal==FALSE, then nothing happens to sourceIdeal
BOOLEAN
fglmzero( idhdl sourceRingHdl, ideal & sourceIdeal, idhdl destRingHdl, ideal & destideal, BOOLEAN switchBack = TRUE, BOOLEAN deleteIdeal = FALSE );

// fglmproc(...):
// The procedure which has to be called from the interpreter.
// first is the sourceRing, second is the given ideal in sourceRing.
// Returns the groebnerbasis of the sourceIdeal in the currentRing.
// Checks, if the ideal is really a reduced groebner basis of a 
// 0-dimensional Ideal. Returns TRUE if an error occoured.
BOOLEAN
fglmProc( leftv result, leftv first, leftv second );

// FindUnivariatePolys (test)
ideal
FindUnivariateWrapper( ideal source );

// homogeneous FGLM
ideal
fglmhomProc(leftv first, leftv second);
#endif

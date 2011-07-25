// ----------------------------------------------------------------------------
//  spectrum.h
//  begin of file
//  Stephan Endrass, endrass@mathematik.uni-mainz.de
//  23.7.99
// ----------------------------------------------------------------------------

#ifndef SPECTRUM_H
#define SPECTRUM_H
#include <kernel/npolygon.h>
#include <kernel/splist.h>

/*BOOLEAN    spectrumProc ( leftv,leftv );
BOOLEAN    spectrumfProc( leftv,leftv );
BOOLEAN    spaddProc    ( leftv,leftv,leftv );
BOOLEAN    spmulProc    ( leftv,leftv,leftv );
BOOLEAN    semicProc   ( leftv,leftv,leftv );
BOOLEAN    semicProc3   ( leftv,leftv,leftv,leftv );*/ //TODO move to kernel

BOOLEAN    hasTermOfDegree( poly h, int d, const ring r );
int        hasOne( ideal J, const ring r );
BOOLEAN    hasAxis( ideal J,int k, const ring r );
poly       computeWC( const newtonPolygon &np,Rational max_weight, const ring r );
void       computeNF( ideal stdJ,poly hc,poly wc,spectrumPolyList *NF, const ring r );
//void       spectrumPrintError(spectrumState state);
BOOLEAN    ringIsLocal( const ring r);

BOOLEAN inline hasConstTerm( poly h, const ring r )
{ return  hasTermOfDegree(h,0,r); }
BOOLEAN inline hasLinearTerm( poly h, const ring r )
{ return  hasTermOfDegree(h,1,r); }


#endif

// ----------------------------------------------------------------------------
//  spectrum.h
//  end of file
// ----------------------------------------------------------------------------

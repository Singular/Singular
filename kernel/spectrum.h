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

BOOLEAN    spectrumProc ( leftv,leftv );
BOOLEAN    spectrumfProc( leftv,leftv );
BOOLEAN    spaddProc    ( leftv,leftv,leftv );
BOOLEAN    spmulProc    ( leftv,leftv,leftv );
BOOLEAN    semicProc   ( leftv,leftv,leftv );
BOOLEAN    semicProc3   ( leftv,leftv,leftv,leftv );
BOOLEAN    hasTermOfDegree( poly h, int d );
int        hasOne( ideal J );
BOOLEAN    hasAxis( ideal J,int k );
poly       computeWC( const newtonPolygon &np,Rational max_weight );
void       computeNF( ideal stdJ,poly hc,poly wc,spectrumPolyList *NF );
void       spectrumPrintError(spectrumState state);
BOOLEAN    ringIsLocal( void );

BOOLEAN inline hasConstTerm( poly h )
{ return  hasTermOfDegree(h,0); }
BOOLEAN inline hasLinearTerm( poly h )
{ return  hasTermOfDegree(h,1); }


#endif

// ----------------------------------------------------------------------------
//  spectrum.h
//  end of file
// ----------------------------------------------------------------------------

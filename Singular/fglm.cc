// emacs edit mode for this file is -*- C++ -*-
// $Id: fglm.cc,v 1.5 1997-03-27 10:32:36 Singular Exp $ 

/****************************************
*  Computer Algebra System SINGULAR     *
****************************************/
/* 
* ABSTRACT - The FGLM-Algorithm
*   Calculate a reduced groebner basis for one ordering, given a 
*   reduced groebner basis for another ordering.
*   In this file the input is checked. Furthermore we decide, if
*   the input is 0-dimensional ( then fglmzero.cc is used ) or
*   if the input is homogeneous ( then fglmhom.cc is used. Yet 
*   not implemented ).
*/

#ifndef NOSTREAMIO
#include <iostream.h>
#endif
#include "mod2.h"
#include "tok.h"
#include "structs.h"
#include "polys.h"
#include "ideals.h"
#include "ring.h"
#include "ipid.h"
#include "ipshell.h"
#include "febase.h"
#include "maps.h"
#include "mmemory.h"
#include "kstd1.h"   
#include "fglm.h"

//     enumeration to handle the various errors to occour.
enum FglmState{ 
    FglmOk, 
    FglmHasOne, 
    FglmNoIdeal,
    FglmNotReduced,
    FglmNotZeroDim, 
    FglmIncompatibleRings
};

// Has to be called, if currQuotient != NULL. ( i.e. qring-case )
// Then a new ideal is build, consisting of the generators of sourceIdeal
// and the generators of currQuotient, which are completely reduced by
// the sourceIdeal. This means: If sourceIdeal is reduced, then the new
// ideal will be reduced as well.
// Assumes that currRing == sourceRing
ideal fglmUpdatesource( const ideal sourceIdeal ) 
{
    int k, l, offset;
    BOOLEAN found;
    ideal newSource= idInit( IDELEMS( sourceIdeal ) + IDELEMS( currQuotient ), 1 );
    for ( k= IDELEMS( sourceIdeal )-1; k >=0; k-- )
	(newSource->m)[k]= pCopy( (sourceIdeal->m)[k] );
    offset= IDELEMS( sourceIdeal );
    for ( l= IDELEMS( currQuotient )-1; l >= 0; l-- ) {
	found= FALSE;
	for ( k= IDELEMS( sourceIdeal )-1; (k >= 0) && (found == FALSE); k-- )
	    if ( pDivisibleBy( (sourceIdeal->m)[k], (currQuotient->m)[l] ) )
		found= TRUE;
	if ( ! found ) {
	    (newSource->m)[offset]= pCopy( (currQuotient->m)[l] );
	    offset++;
	}
    }
    idSkipZeroes( newSource );
    return newSource;
}

// Has to be called, if currQuotient != NULL, i.e. in qring-case.
// Gets rid of the elements of result which are contained in 
// currQuotient and skips Zeroes.
// Assumes that currRing == destRing
void
fglmUpdateresult( ideal & result ) 
{
    int k, l;
    BOOLEAN found;
    for ( k= IDELEMS( result )-1; k >=0; k-- ) {
	found= FALSE;
	for ( l= IDELEMS( currQuotient )-1; (l >= 0) && ( found == FALSE ); l-- )
	    if ( pDivisibleBy( (currQuotient->m)[l], (result->m)[k] ) )
		found= TRUE;
	if ( found ) pDelete( &(currQuotient->m)[l] );
    }
    idSkipZeroes( result );
}

// Checks if the two rings sringHdl and dringHdl are compatible enough to
// be used for the fglm. This means:
//  1) Same Characteristic, 2) globalOrderings in both rings, 
//  3) Same number of variables, 4) same number of parameters
//  5) variables in one ring are permutated variables of the other one
//  6) parameters in one ring are permutated parameters of the other one
//  7) either both rings are rings or both rings are qrings
//  8) if they are qrings, the quotientIdeals of both must coincide.
// vperm must be a vector of length pVariables+1, initialized by 0.
// If both rings are compatible, it stores the permutation of the 
// variables if mapped from sringHdl to dringHdl. 
// if the rings are compatible, it returns FglmOk.
// Should be called with currRing= IDRING( sringHdl );
FglmState 
fglmConsistency( idhdl sringHdl, idhdl dringHdl, int * vperm ) 
{
    int k;
    FglmState state= FglmOk;
    ring dring = IDRING( dringHdl );
    ring sring = IDRING( sringHdl );
    
    if ( sring->ch != dring->ch ) {
	WerrorS( "rings must have same characteristic" );
	state= FglmIncompatibleRings;
    }
    if ( (sring->OrdSgn != 1) || (dring->OrdSgn != 1) ) {
	WerrorS( "only works for global orderings" );
	state= FglmIncompatibleRings;
    }
    if ( sring->N != dring->N ) {
	WerrorS( "rings must have same number of variables" );
	state= FglmIncompatibleRings;
    }
    if ( sring->P != dring->P ) {
	WerrorS( "rings must have same number of parameters" );
	state= FglmIncompatibleRings;
    }
    if ( state != FglmOk ) return state;
    // now the rings have the same number of variables resp. parameters.
    // check if the names of the variables resp. parameters do agree:
    int nvar = sring->N;
    int npar = sring->P;
    int * pperm;
    if ( npar > 0 ) 
	pperm= (int *)Alloc0( (npar+1)*sizeof( int ) );
    else
	pperm= NULL;
    maFindPerm( sring->names, nvar, sring->parameter, npar, dring->names, nvar, dring->parameter, npar, vperm, pperm );
    for ( k= nvar; (k > 0) && (state == FglmOk); k-- )
	if ( vperm[k] <= 0 ) {
	    WerrorS( "variable names do not agree" );
	    state= FglmIncompatibleRings;
	}
    for ( k= npar-1; (k >= 0) && (state == FglmOk); k-- )
	if ( pperm[k] >= 0 ) {
	    WerrorS( "paramater names do not agree" );
	    state= FglmIncompatibleRings;
	}
    Free( (ADDRESS)pperm, (npar+1)*sizeof( int ) );
    if ( state != FglmOk ) return state;
    // check if both rings are qrings or not
    if ( sring->qideal != NULL ) {
	if ( dring->qideal == NULL ) {
	    Werror( "%s is a qring, current ring not", sringHdl->id );
	    return FglmIncompatibleRings;
	}
	// both rings are qrings, now check if both quotients define the same ideal.
	// check if sring->qideal is contained in dring->qideal:
	rSetHdl( dringHdl, TRUE );
	nSetMap( sring->ch, sring->parameter, npar, sring->minpoly );
	ideal sqind = idInit( IDELEMS( sring->qideal ), 1 );
	for ( k= IDELEMS( sring->qideal )-1; k >= 0; k-- )
	    (sqind->m)[k]= pPermPoly( (sring->qideal->m)[k], vperm, nvar );
	ideal sqindred = kNF( dring->qideal, NULL, sqind );
	if ( ! idIs0( sqindred ) ) {
	    WerrorS( "the quotients do not agree" );
	    state= FglmIncompatibleRings;
	}
	idDelete( & sqind );
	idDelete( & sqindred );
	rSetHdl( sringHdl, TRUE );
	if ( state != FglmOk ) return state;
	// check if dring->qideal is contained in sring->qideal:
	int * dsvperm = (int *)Alloc0( (nvar+1)*sizeof( int ) );
	maFindPerm( dring->names, nvar, NULL, 0, sring->names, nvar, NULL, 0, dsvperm, NULL );
	nSetMap( dring->ch, dring->parameter, npar, dring->minpoly );
	ideal dqins = idInit( IDELEMS( dring->qideal ), 1 );
	for ( k= IDELEMS( dring->qideal )-1; k >= 0; k-- ) 
	    (dqins->m)[k]= pPermPoly( (dring->qideal->m)[k], dsvperm, nvar );
	ideal dqinsred = kNF( sring->qideal, NULL, dqins );
	if ( ! idIs0( dqinsred ) ) {
	    WerrorS( "the quotients do not agree" );
	    state= FglmIncompatibleRings;
	}
	idDelete( & dqins );
	idDelete( & dqinsred );
	Free( (ADDRESS)dsvperm, (nvar+1)*sizeof( int ) );
	if ( state != FglmOk ) return state;
    }  
    else {
	if ( dring->qideal != NULL ) {
	    Werror( "current ring is a qring, %s not", sringHdl->id );
	    return FglmIncompatibleRings;
	}
    }
    return FglmOk;
}

//     Checks if the ideal "theIdeal" is zero-dimensional and minimal. It does
//      not check, if it is reduced. 
//     returns FglmOk if we can use theIdeal for CalculateFunctionals (this 
//                     function reports an error if theIdeal is not reduced, 
//                     so this need not to be tested here)
//             FglmNotReduced if theIdeal is not minimal
//             FglmNotZeroDim if it is not zero-dimensional
//             FglmHasOne if 1 belongs to theIdeal
FglmState 
fglmIdealcheck( const ideal theIdeal )
{
    FglmState state = FglmOk;
    int power;
    int k; 
    BOOLEAN * purePowers = (BOOLEAN *)Alloc( pVariables*sizeof( BOOLEAN ) );
    for ( k= pVariables-1; k >= 0; k-- ) 
	purePowers[k]= FALSE;

    for ( k= IDELEMS( theIdeal ) - 1; (state == FglmOk) && (k >= 0); k-- ) {
	poly p = (theIdeal->m)[k];
	if ( pIsConstant( p ) ) state= FglmHasOne;
	else if ( (power= pIsPurePower( p )) > 0 ) {
	    fglmASSERT( 0 < power && power <= pVariables, "illegal power" );
	    if ( purePowers[power-1] == TRUE  ) state= FglmNotReduced;
	    else purePowers[power-1]= TRUE; 
	}
	for ( int l = IDELEMS( theIdeal ) - 1; state == FglmOk && l >= 0; l-- ) 
	    if ( (k != l) && pDivisibleBy( p, (theIdeal->m)[l] ) )
		state= FglmNotReduced;
    }
    if ( state == FglmOk ) {
	for ( k= pVariables-1 ; (state == FglmOk) && (k >= 0); k-- ) 
	    if ( purePowers[k] == FALSE ) state= FglmNotZeroDim;
    }
    Free( (ADDRESS)purePowers, pVariables*sizeof( BOOLEAN ) );
    return state;
}

//     the main function for the fglm-Algorithm. 
//     Checks the input-data, calls CalculateFunctionals, handles change
//     of ring-vars and finaly calls GroebnerViaFunctionals.
//     returns the new groebnerbasis or 0 if an error occoured.
BOOLEAN
fglmProc( leftv result, leftv first, leftv second ) 
{
    FglmState state = FglmOk;
    //. array for the permutations of vars in both rings:
    //. counts from perm[1]..perm[pvariables]
    int * vperm = (int *)Alloc0( (pVariables+1)*sizeof( int ) );

    idhdl destRingHdl = currRingHdl;
    ring destRing = currRing;
    ideal destIdeal = NULL;
    idhdl sourceRingHdl = (idhdl)first->data;
    rSetHdl( sourceRingHdl, TRUE );
    ring sourceRing = currRing;
    state= fglmConsistency( sourceRingHdl, destRingHdl, vperm );

    if ( state == FglmOk ) {
	idhdl ih = currRing->idroot->get( second->Name(), myynest );
	if ( (ih != NULL) && (IDTYP(ih)==IDEAL_CMD) ) {
	    ideal sourceIdeal;
	    if ( currQuotient != NULL ) 
		sourceIdeal= fglmUpdatesource( IDIDEAL( ih ) );
	    else
		sourceIdeal = IDIDEAL( ih );
	    state= fglmIdealcheck( sourceIdeal );
	    if ( state == FglmOk ) { 
		// Now the settings are compatible with FGLM
		assumeStdFlag( (leftv)ih );
		if ( fglmzero( sourceRingHdl, sourceIdeal, destRingHdl, destIdeal, FALSE, (currQuotient != NULL) ) == FALSE )
		    state= FglmNotReduced;
	    }
	} else state= FglmNoIdeal;
    }
    if ( currRingHdl != destRingHdl )
	rSetHdl( destRingHdl, TRUE );
    switch (state) {
	case FglmOk:
	    if ( currQuotient != NULL ) fglmUpdateresult( destIdeal );
	    break;
	case FglmHasOne:
	    destIdeal= idInit(1,1);
	    (destIdeal->m)[0]= pOne();
	    state= FglmOk;
	    break;
	case FglmIncompatibleRings:
	    Werror( "ring %s and current ring are incompatible", first->Name() );
	    destIdeal= idInit(0,0);
	    break;
	case FglmNoIdeal:
	    Werror( "Can't find ideal %s in ring %s", second->Name(), first->Name() );
	    destIdeal= idInit(0,0);
	    break;
	case FglmNotZeroDim:
	    Werror( "The ideal %s has to be 0-dimensional", second->Name() );
	    destIdeal= idInit(0,0);
	    break;
	case FglmNotReduced:
	    Werror( "The ideal %s has to be reduced", second->Name() );
	    destIdeal= idInit(0,0);
	    break;
	default:
	    destIdeal= idInit(1,1);
    }
    Free( (ADDRESS)vperm, (pVariables+1)*sizeof(int) );

    result->rtyp = IDEAL_CMD;
    result->data= (void *)destIdeal;
    setFlag( result, FLAG_STD );
    if ( state == FglmOk )
	return FALSE;
    else
	return TRUE;
}

// ----------------------------------------------------------------------------
// Local Variables: ***
// compile-command: "make Singular" ***
// page-delimiter: "^\\(\\|//!\\)" ***
// fold-internal-margins: nil ***
// End: ***

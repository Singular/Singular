// emacs edit mode for this file is -*- C++ -*-
// $Id: fglmhom.cc,v 1.2 1997-10-06 12:19:11 obachman Exp $

/****************************************
*  Computer Algebra System SINGULAR     *
****************************************/
/* 
* ABSTRACT - The FGLM-Algorithm extended for homogeneous ideals.
*   Calculates via the hilbert-function a groebner basis.
*/

#ifndef NOSTREAMIO
#include <iostream.h>
#endif

#include "mod2.h"
#ifdef HAVE_FGLM
#include "tok.h"
#include "structs.h"
#include "subexpr.h"  
#include "polys.h"
#include "ideals.h"
#include "ring.h"
#include "ipid.h"
#include "ipshell.h"
#include "febase.h"
#include "maps.h"
#include "mmemory.h"
#include "fglm.h"
#include "fglmvec.h"
#include "intvec.h"
#include "stairc.h"  // -> hHStdSeries, hFirstSeries usw.
#include <templates/list.h>

#define PROT(msg)
#define STICKYPROT(msg) if (BTEST1(OPT_PROT)) Print(msg)
#define PROT2(msg,arg)
#define STICKYPROT2(msg,arg) if (BTEST1(OPT_PROT)) Print(msg,arg)
#define fglmASSERT(ignore1,ignore2)


struct homogElem
{
    poly mon;
//    fglmVector v;
    int basis;
    BOOLEAN inDest;
    homogElem( poly m, int b, BOOLEAN ind ) : mon(m), 
//v(), 
	basis(b), inDest(ind) {}
};

struct doublepoly
{
    poly sm;
    poly dm;
};

struct homogData 
{
    doublepoly * sourceHeads;
    int numSourceHeads;
    ideal destIdeal;
    int numDestPolys;
    homogElem * monlist;
    int numMonoms;
    int basisSize;
//     homogData() : sourceHeads(NULL), numSourceHeads(0), monlist(NULL), 
// 	numMonoms(0), basisSize(0) {}
};

int
hfglmNextdegree( intvec * source, ideal current, int & deg ) 
{
    int numelems;
    intvec * newhilb = hHstdSeries( current, NULL, currQuotient );

    loop 
    {
	if ( deg < newhilb->length() ) 
	{
	    if ( deg < source->length() )
		numelems= (*newhilb)[deg]-(*source)[deg];
	    else
		numelems= (*newhilb)[deg];
	}
	else
	{
	    if (deg < source->length())
		numelems= -(*source)[deg];
	    else
	    {
		deg= 0;
		return 0;
	    }
	}
	if (numelems != 0) 
	    return numelems;
	deg++;
    }  
    delete newhilb;
}

void 
generateMonoms( poly m, int var, int deg, homogData * dat )
{
    if ( var == pVariables ) {
	BOOLEAN inSource = FALSE;
	BOOLEAN inDest = FALSE;
	poly mon = pCopy( m );
	pSetExp( mon, var, deg );
	pSetm( mon );
	int i;
	for ( i= dat->numSourceHeads - 1; (i >= 0) && (inSource==FALSE); i-- ) {
	    if ( pDivisibleBy( dat->sourceHeads[i].dm, mon ) ) {
		inSource= TRUE;
	    }
	}
	for ( i= dat->numDestPolys - 1; (i >= 0) && (inDest==FALSE); i-- ) {
	    if ( pDivisibleBy( (dat->destIdeal->m)[i], mon ) ) {
		inDest= TRUE;
	    }
	}
	if ( (!inSource) || (!inDest) ) {
	    int basis = 0;
	    if ( !inSource ) basis= ++(dat->basisSize);
	    dat->monlist[dat->numMonoms]= homogElem( mon, basis, inDest );
	    dat->numMonoms++;
	    PROT2( " %s", pString(mon) );
	    if ( inSource ) PROT( "(s)" );
	    if ( inDest ) PROT( "(d)" );
	}
	else {
	    PROT2( "{%s}", pString(mon) );
	    pDelete( & mon );
	}
	return;
    }
    else {
	poly newm = pCopy( m );
	while ( deg >= 0 ) {
	    generateMonoms( newm, var+1, deg, dat );
	    pGetExp( newm, var )++;
	    pSetm( newm );
	    deg--;
	}
	pDelete( & newm );
    }
    return;
}

BOOLEAN
fglmhomog( idhdl sourceRingHdl, ideal sourceIdeal, idhdl destRingHdl, ideal & destIdeal )
//homogfglm( idhdl sRingHdl, ideal sIdeal, idhdl dRingHdl, int * varperm, int * parperm )
{
//-> oldversion
//     int deg = 0;
//     int numelems = 0;
//     int groebnerBS = 16;
//     List<hdestElem> monlist;
//     rSetHdl( sourceRingHdl, TRUE );
//     intvec * sourcehilb = hHstdSeries( sourceIdeal, NULL, currQuotient );
//     rSetHdl( destRingHdl, TRUE );
//     ideal destIdeal = idInit( groebnerBS, 1 );
//     numelems= hfglmNextdegree( sourcehilb, destIdeal, deg );
//     Print( "**Next Step: numelems %i in degree %i\n", numelems, deg );

//     monomsOfDegree( deg, monlist );
//     Print( "number: %i\n", monlist.length() );
//     return destIdeal;
//<-
#define groebnerBS 16
    int numGBelems;
    int deg = 0;

    homogData dat;

    // get the hilbert series and the leading monomials of the sourceIdeal:
    rSetHdl( sourceRingHdl, TRUE );
    ring sourceRing = currRing;

    intvec * hilb = hHstdSeries( sourceIdeal, NULL, currQuotient ); 
    int s;
    dat.sourceHeads= (doublepoly *)Alloc( IDELEMS( sourceIdeal ) * sizeof( doublepoly ) );
    for ( s= IDELEMS( sourceIdeal ) - 1; s >= 0; s-- ) {
	dat.sourceHeads[s].sm= pHead( (sourceIdeal->m)[s] );
    }
    dat.numSourceHeads= IDELEMS( sourceIdeal );
    rSetHdl( destRingHdl, TRUE );
    ring destRing = currRing;

    // Map the sourceHeads to the destRing
    int * vperm = (int *)Alloc( (sourceRing->N + 1)*sizeof(int) );
    maFindPerm( sourceRing->names, sourceRing->N, NULL, 0, currRing->names, currRing->N, NULL, 0, vperm, NULL );
    nSetMap( sourceRing->ch, sourceRing->parameter, sourceRing->P, sourceRing->minpoly );
    for ( s= IDELEMS( sourceIdeal ) - 1; s >= 0; s-- ) {
	dat.sourceHeads[s].dm= pPermPoly( dat.sourceHeads[s].sm, vperm, sourceRing->N, NULL, 0 );
    }
    PROT( "sourceHeads: " );
    for ( s= dat.numSourceHeads - 1; s >= 0; s-- ) {
	PROT2( "%s ", pString(dat.sourceHeads[s].dm) );
    }
    PROT( "\n" );

    dat.destIdeal= idInit( 0, 1 );
    dat.numDestPolys= 0;
//     dat.destIdeal = idInit( groebnerBS, 1 );

    if ( (numGBelems= hfglmNextdegree( hilb, dat.destIdeal, deg )) != 0 ) {
	int num = 0;  // the number of monoms of degree deg
	PROT2( "[deg= %i >", deg );
	dat.monlist= (homogElem *)Alloc( 100*sizeof( homogElem ) );
	dat.numMonoms= 0;
	poly start= pOne();
	generateMonoms( start, 1, deg, &dat );
	pDelete( & start );
	Free( (ADDRESS)dat.monlist, 100*sizeof( homogElem ) );
	PROT( " < " );
    }
//     while ( (numGBelems= nextdegree( hilb, dIdeal, deg )) != 0 ) {
// 	int num = 0;  // the number of monoms of degree deg
// 	PROT2( "[deg= %i", deg );
// 	PROT( " >" );
// 	poly start= pOne();
// 	generateMonoms( pOne, 1, deg, &dat );
// 	pDelete( & start );
//     }
    
	// Calculate 
// 	hsourceElem * sourcelist = degmon( deg, num, dIdeal );
	// check if monoms are already in dIdeal:
// 	int k;
// 	for ( k= num; k > 0; k-- ) {
// 	    sourcelist[k].inIdeal= isinIdeal( sourcelist[k].m, dIdeal );
// 	}
// 	getVectorRep( sRingHdl, sIdeal, varperm, parperm, sourcelist );
// 	k= 1;
// 	while ( (k <= num) && (numGBelems > 0) ) {
	    

// 	Free( (ADDRESS)hsourceElem, num );
//     destIdeal= idInit(0,0);
    PROT( "\n" );
    return TRUE;
//    return idInit(0,0);
}





// //-> enum HomogFglmState
// //     enumeration to handle the various errors to occour.
// enum HomogFglmState{ 
//     HomogFglmOk, 
//     HomogFglmHasOne, 
//     HomogFglmNoIdeal,
//     HomogFglmNotReduced,
//     HomogFglmNotZeroDim, 
//     HomogFglmIncompatibleRings
// };
// //<-

// //-> BOOLEAN homogfglmProc( leftv result, leftv first, leftv second )
// //     the main function for the fglm-Algorithm. 
// //     Checks the input-data, calls CalculateFunctionals, handles change
// //     of ring-vars and finaly calls GroebnerViaFunctionals.
// //     returns the new groebnerbasis or 0 if an error occoured.
// BOOLEAN
// homogfglmProc( leftv result, leftv first, leftv second ) 
// {
//     HomogFglmState state = HomogFglmOk;
//     idhdl destRingHdl = currRingHdl;
//     ring destRing = currRing;
//     int destChar= nGetChar();
//     rSetHdl( (idhdl)first->data, TRUE );
//     idhdl sourceRingHdl = currRingHdl;
//     ideal sourceIdeal;

//     idhdl ih = currRing->idroot->get( second->name, myynest );
//     if ( ih == NULL ) state= HomogFglmNoIdeal;
    
//     //. check if rings are compatible: (i.e. same Characteristic and same var-names)
//     if ( nGetChar() != destChar ) state= HomogFglmIncompatibleRings;
//     else {
// 	if ( currRing->N == destRing->N ) {
// // 	    maFindPerm( currRing->names, pVariables, NULL, destRing->names, pVariables, NULL, perm, NULL );
// // 	    for ( int k = 1; state == HomogFglmOk && k <= pVariables; k++ ) 
// // 		if ( perm[k] <= 0 ) state= HomogFglmIncompatibleRings;
// 	} else state= HomogFglmIncompatibleRings;
//     }
//     ideal destIdeal;
//     //. In den Zielring schalten und die neue Basis ausrechnen:
//     rSetHdl( destRingHdl, TRUE );
//     switch (state) {
// 	case HomogFglmOk:
// 	    sourceIdeal= IDIDEAL( ih );
// 	    assumeStdFlag( (leftv)ih );
// 	    destIdeal= homogfglm( sourceRingHdl, sourceIdeal, destRingHdl, NULL, NULL );
// 	    break;
// 	case HomogFglmHasOne:
// 	    destIdeal= idInit(1,1);
// 	    (destIdeal->m)[0]= pOne();
// 	    break;
// 	case HomogFglmIncompatibleRings:
// 	    Werror( "ring %s and current ring are incompatible", first->name );
// 	    destIdeal= idInit(0,0);
// 	    break;
// 	case HomogFglmNoIdeal:
// 	    Werror( "Can't find ideal %s in ring %s", second->name, first->name );
// 	    destIdeal= idInit(0,0);
// 	    break;
// 	case HomogFglmNotZeroDim:
// 	    Werror( "The ideal %s has to be 0-dimensional", second->name );
// 	    destIdeal= idInit(0,0);
// 	    break;
// 	case HomogFglmNotReduced:
// 	    Werror( "The ideal %s has to be reduced", second->name );
// 	    destIdeal= idInit(0,0);
// 	    break;
// 	default:
// 	    destIdeal= idInit(1,1);
//     }
//     result->rtyp = IDEAL_CMD;
//     result->data= (void *)destIdeal;
//     setFlag( result, FLAG_STD );
//     if ( state == HomogFglmOk )
// 	return FALSE;
//     else
// 	return TRUE;
// }
// //<-
#endif

// Questions:
// Muss ich einen intvec freigeben?
// ----------------------------------------------------------------------------
// Local Variables: ***
// compile-command: "make Singular" ***
// page-delimiter: "^\\(\\|//!\\)" ***
// fold-internal-margins: nil ***
// End: ***

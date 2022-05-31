// emacs edit mode for this file is -*- C++ -*-

/****************************************
*  Computer Algebra System SINGULAR     *
****************************************/
/*
* ABSTRACT - The FGLM-Algorithm extended for homogeneous ideals.
*   Calculates via the hilbert-function a groebner basis.
*/





#include "kernel/mod2.h"
#if 0
#include "factoryconf.h"
#ifndef NOSTREAMIO
#include "iostream.h"
#endif
// #include "Singular/tok.h"
#include "kernel/structs.h"
#include "kernel/subexpr.h"
#include "kernel/polys.h"
#include "kernel/ideals.h"
#include "polys/monomials/ring.h"
#include "kernel/ipid.h"
#include "kernel/ipshell.h"
#include "polys/monomials/maps.h"
#include "fglm.h"
#include "fglmvec.h"
#include "fglmgauss.h"
#include "misc/intvec.h"
#include "kernel/GBEngine/kstd1.h"
#include "kernel/combinatorics/stairc.h"
#include "factory/templates/ftmpl_list.h"

// obachman: Got rid off those "redefiende messages by includeing fglm.h
#include "fglm.h"
#if 0
#define PROT(msg) if (BTEST1(OPT_PROT)) Print(msg)
#define STICKYPROT(msg) if (BTEST1(OPT_PROT)) Print(msg)
#define PROT2(msg,arg) if (BTEST1(OPT_PROT)) Print(msg,arg)
#define STICKYPROT2(msg,arg) if (BTEST1(OPT_PROT)) Print(msg,arg)
#define fglmASSERT(ignore1,ignore2)
#endif

struct doublepoly
{
    poly sm;
    poly dm;
};

class homogElem
{
public:
    doublepoly mon;
    fglmVector v;
    fglmVector dv;
    int basis;
    int destbasis;
    BOOLEAN inDest;
    homogElem() : v(), dv(), basis(0), destbasis(0), inDest(FALSE) {}
    homogElem( poly m, int b, BOOLEAN ind ) :
        basis(b), inDest(ind)
    {
        mon.dm= m;
        mon.sm= NULL;
    }
#ifndef HAVE_EXPLICIT_CONSTR
    void initialize( poly m, int b, BOOLEAN ind )
    {
        basis = b;
        inDest = ind;
        mon.dm = m;
        mon.sm = NULL;
    }
    void initialize( const homogElem h )
    {
        basis = h.basis;
        inDest = h.inDest;
        mon.dm = h.mon.dm;
        mon.sm = h.mon.sm;
    }
#endif
};

struct homogData
{
    ideal sourceIdeal;
    doublepoly * sourceHeads;
    int numSourceHeads;
    ideal destIdeal;
    int numDestPolys;
    homogElem * monlist;
    int monlistmax;
    int monlistblock;
    int numMonoms;
    int basisSize;
    int overall;  // nur zum testen.
    int numberofdestbasismonoms;
//     homogData() : sourceHeads(NULL), numSourceHeads(0), monlist(NULL),
//         numMonoms(0), basisSize(0) {}
};

int
hfglmNextdegree( intvec * source, ideal current, int & deg )
{
    int numelems;
    intvec * newhilb = hFirstSeries( current, NULL, currRing->qideal );

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
    if ( var == (currRing->N) ) {
        BOOLEAN inSource = FALSE;
        BOOLEAN inDest = FALSE;
        poly mon = pCopy( m );
        pSetExp( mon, var, deg );
        pSetm( mon );
        ++dat->overall;
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
            if ( !inSource )
                basis= ++(dat->basisSize);
            if ( !inDest )
                ++dat->numberofdestbasismonoms;
            if ( dat->numMonoms == dat->monlistmax ) {
                int k;
#ifdef HAVE_EXPLICIT_CONSTR
                // Expand array using Singulars ReAlloc function
                dat->monlist=
                    (homogElem * )omReallocSize( dat->monlist,
                                           (dat->monlistmax)*sizeof( homogElem ),
                                           (dat->monlistmax+dat->monlistblock) * sizeof( homogElem ) );
                for ( k= dat->monlistmax; k < (dat->monlistmax+dat->monlistblock); k++ )
                    dat->monlist[k].homogElem();
#else
                // Expand array by generating new one and copying
                int newsize = dat->monlistmax  + dat->monlistblock;
                homogElem * tempelem = new homogElem[ newsize ];
                // Copy old elements
                for ( k= dat->monlistmax - 1; k >= 0; k-- )
                    tempelem[k].initialize( dat->monlist[k] );
                delete [] homogElem;
                homogElem = tempelem;
#endif
                dat->monlistmax+= dat->monlistblock;
            }
#ifdef HAVE_EXPLICIT_CONSTR
            dat->monlist[dat->numMonoms]= homogElem( mon, basis, inDest );
#else
            dat->monlist[dat->numMonoms].initialize( mon, basis, inDest );
#endif
            dat->numMonoms++;
            if ( inSource && ! inDest ) PROT( "\\" );
            if ( ! inSource && inDest ) PROT( "/" );
            if ( ! inSource && ! inDest ) PROT( "." );
        }
        else {
            pDelete( & mon );
        }
        return;
    }
    else {
        poly newm = pCopy( m );
        while ( deg >= 0 ) {
            generateMonoms( newm, var+1, deg, dat );
            pIncrExp( newm, var );
            pSetm( newm );
            deg--;
        }
        pDelete( & newm );
    }
    return;
}

void
mapMonoms( ring oldRing, homogData & dat )
{
    int * vperm = (int *)omAlloc( (currRing->N + 1)*sizeof(int) );
    maFindPerm( oldRing->names, oldRing->N, NULL, 0, currRing->names, currRing->N, NULL, 0, vperm, NULL );
    //nSetMap( oldRing->ch, oldRing->parameter, oldRing->P, oldRing->minpoly );
    nSetMap( oldRing );
    int s;
    for ( s= dat.numMonoms - 1; s >= 0; s-- ) {
//        dat.monlist[s].mon.sm= pPermPoly( dat.monlist[s].mon.dm, vperm, currRing->N, NULL, 0 );
      // obachman: changed the folowing to reflect the new calling interface of
      // pPermPoly -- Tim please check whether this is correct!
        dat.monlist[s].mon.sm= pPermPoly( dat.monlist[s].mon.dm, vperm, oldRing, NULL, 0 );
    }
}

void
getVectorRep( homogData & dat )
{
    // Calculate the NormalForms
    int s;
    for ( s= 0;  s < dat.numMonoms; s++ ) {
        if ( dat.monlist[s].inDest == FALSE ) {
            fglmVector v;
            if ( dat.monlist[s].basis == 0 ) {
                v= fglmVector( dat.basisSize );
                // now the monom is in L(source)
                PROT( "(" );
                poly nf = kNF( dat.sourceIdeal, NULL, dat.monlist[s].mon.sm );
                PROT( ")" );
                poly temp = nf;
                while (temp != NULL ) {
                    int t;
                    for ( t= dat.numMonoms - 1; t >= 0; t-- ) {
                        if ( dat.monlist[t].basis > 0 ) {
                            if ( pLmEqual( dat.monlist[t].mon.sm, temp ) ) {
                                number coeff= nCopy( pGetCoeff( temp ) );
                                v.setelem( dat.monlist[t].basis, coeff );
                            }
                        }
                    }
                    pIter(temp);
                }
                pDelete( & nf );
            }
            else {
                PROT( "." );
                v= fglmVector( dat.basisSize, dat.monlist[s].basis );
            }
            dat.monlist[s].v= v;
        }
    }
}

void
remapVectors( ring oldring, homogData & dat )
{
    //nSetMap( oldring->ch, oldring->parameter, oldring->P, oldring->minpoly );
    nSetMap( oldring );
    int s;
    for ( s= dat.numMonoms - 1; s >= 0; s-- ) {
        if ( dat.monlist[s].inDest == FALSE ) {
            int k;
            fglmVector newv( dat.basisSize );
            for ( k= dat.basisSize; k > 0; k-- ){
                number newnum= nMap( dat.monlist[s].v.getelem( k ) );
                newv.setelem( k, newnum );
            }
            dat.monlist[s].dv= newv;
        }
    }
}

void
gaussreduce( homogData & dat, int maxnum, int BS )
{
    int s;
    int found= 0;

    int destbasisSize = 0;
    gaussReducer gauss( dat.basisSize );

    for ( s= 0; (s < dat.numMonoms) && (found < maxnum); s++ ) {
        if ( dat.monlist[s].inDest == FALSE ) {
            if ( gauss.reduce( dat.monlist[s].dv ) == FALSE ) {
                destbasisSize++;
                dat.monlist[s].destbasis= destbasisSize;
                gauss.store();
                PROT( "." );
            }
            else {
                fglmVector p= gauss.getDependence();
                poly result = pCopy( dat.monlist[s].mon.dm );
                pSetCoeff( result, nCopy( p.getconstelem( p.size() ) ) );
                int l = 0;
                int k;
                for ( k= 1; k < p.size(); k++ ) {
                    if ( ! p.elemIsZero( k ) ) {
                        while ( dat.monlist[l].destbasis != k )
                            l++;
                        poly temp = pCopy( dat.monlist[l].mon.dm );
                        pSetCoeff( temp, nCopy( p.getconstelem( k ) ) );
                        result= pAdd( result, temp );
                    }
                }
                if ( ! nGreaterZero( pGetCoeff( result ) ) ) result= pNeg( result );
//                PROT2( "(%s)", pString( result ) );
                PROT( "+" );
                found++;
                (dat.destIdeal->m)[dat.numDestPolys]= result;
                dat.numDestPolys++;
                if ( IDELEMS(dat.destIdeal) == dat.numDestPolys ) {
                    pEnlargeSet( & dat.destIdeal->m, IDELEMS( dat.destIdeal ), BS );
                    IDELEMS( dat.destIdeal )+= BS;
                }

            }

        }
    }
    PROT2( "(%i", s );
    PROT2( "/%i)", dat.numberofdestbasismonoms );
}


BOOLEAN
fglmhomog( ring sourceRing, ideal sourceIdeal, ring destRing, ideal & destIdeal )
{
#define groebnerBS 16
    int numGBelems;
    int deg = 0;

    homogData dat;

    // get the hilbert series and the leading monomials of the sourceIdeal:
    rChangeCurrRing( sourceRing );

    intvec * hilb = hFirstSeries( sourceIdeal, NULL, currRing->qideal );
    int s;
    dat.sourceIdeal= sourceIdeal;
    dat.sourceHeads= (doublepoly *)omAlloc( IDELEMS( sourceIdeal ) * sizeof( doublepoly ) );
    for ( s= IDELEMS( sourceIdeal ) - 1; s >= 0; s-- )
    {
        dat.sourceHeads[s].sm= pHead( (sourceIdeal->m)[s] );
    }
    dat.numSourceHeads= IDELEMS( sourceIdeal );
    rChangeCurrRing( destRing );

    // Map the sourceHeads to the destRing
    int * vperm = (int *)omAlloc( (sourceRing->N + 1)*sizeof(int) );
    maFindPerm( sourceRing->names, sourceRing->N, NULL, 0, currRing->names,
                currRing->N, NULL, 0, vperm, NULL, currRing->ch);
    //nSetMap( sourceRing->ch, sourceRing->parameter, sourceRing->P, sourceRing->minpoly );
    nSetMap( sourceRing );
    for ( s= IDELEMS( sourceIdeal ) - 1; s >= 0; s-- )
    {
        dat.sourceHeads[s].dm= pPermPoly( dat.sourceHeads[s].sm, vperm, sourceRing, NULL, 0 );
    }

    dat.destIdeal= idInit( groebnerBS, 1 );
    dat.numDestPolys= 0;

    while ( (numGBelems= hfglmNextdegree( hilb, dat.destIdeal, deg )) != 0 )
    {
        int num = 0;  // the number of monoms of degree deg
        PROT2( "deg= %i ", deg );
        PROT2( "num= %i\ngen>", numGBelems );
        dat.monlistblock= 512;
        dat.monlistmax= dat.monlistblock;
#ifdef HAVE_EXPLICIT_CONSTR
        dat.monlist= (homogElem *)omAlloc( dat.monlistmax*sizeof( homogElem ) );
        int j;
        for ( j= dat.monlistmax - 1; j >= 0; j-- ) dat.monlist[j].homogElem();
#else
        dat.monlist = new homogElem[ dat.monlistmax ];
#endif
        dat.numMonoms= 0;
        dat.basisSize= 0;
        dat.overall= 0;
        dat.numberofdestbasismonoms= 0;

        poly start= pOne();
        generateMonoms( start, 1, deg, &dat );
        pDelete( & start );

        PROT2( "(%i/", dat.basisSize );
        PROT2( "%i)\nvec>", dat.overall );
        // switch to sourceRing and map monoms
        rChangeCurrRing( sourceRing );
        mapMonoms( destRing, dat );
        getVectorRep( dat );

        // switch to destination Ring and remap the vectors
        rChangeCurrRing( destRing );
        remapVectors( sourceRing, dat );

        PROT( "<\nred>" );
        // now do gaussian reduction
        gaussreduce( dat, numGBelems, groebnerBS );

#ifdef HAVE_EXPLICIT_CONSTR
        omFreeSize( (ADDRESS)dat.monlist, dat.monlistmax*sizeof( homogElem ) );
#else
        delete [] dat.monlist;
#endif
        PROT( "<\n" );
    }
    PROT( "\n" );
    destIdeal= dat.destIdeal;
    idSkipZeroes( destIdeal );
    return TRUE;
}

/* ideal fglmhomProc(leftv first, leftv second) // TODO: Move to Singular/
{
    idhdl dest= currRingHdl;
    ideal result;
    // in den durch das erste Argument angegeben Ring schalten:
    rSetHdl( (idhdl)first->data, TRUE );
    idhdl ih= currRing->idroot->get( second->name, myynest );
    ASSERT( ih!=NULL, "Error: Can't find ideal in ring");
    rSetHdl( dest, TRUE );

    ideal i= IDIDEAL(ih);
    fglmhomog( IDRING((idhdl)first->data), i, IDRING(dest), result );

    return( result );
} */

#endif

// Questions:
// Muss ich einen intvec freigeben?
// ----------------------------------------------------------------------------
// Local Variables: ***
// compile-command: "make Singular" ***
// page-delimiter: "^\\(\\|//!\\)" ***
// fold-internal-margins: nil ***
// End: ***

// emacs edit mode for this file is -*- C++ -*-

/****************************************
*  Computer Algebra System SINGULAR     *
****************************************/
/*
* ABSTRACT - class gaussReducer. Used in fglmzero.cc and fglmhom.cc
*  to find linear dependecies of fglmVectors.
*/




#include "kernel/mod2.h"

#include "kernel/structs.h"
#include "coeffs/numbers.h"
#include "polys/monomials/ring.h"

#include "fglmvec.h"
#include "fglmgauss.h"

class gaussElem
{
public:
    fglmVector v;
    fglmVector p;
    number pdenom;
    number fac;
    gaussElem( const fglmVector newv, const fglmVector newp, number & newpdenom, number & newfac ) : v( newv ), p( newp ), pdenom( newpdenom ), fac( newfac )
    {
        newpdenom= NULL;
        newfac= NULL;
    }

#ifndef HAVE_EXPLICIT_CONSTR
  gaussElem() : v(), p(), pdenom(NULL), fac(NULL) {}

  void mac_gaussElem( const fglmVector newv, const fglmVector newp, number & newpdenom, number & newfac )
    {
    v= newv;
    p= newp;
    pdenom=newpdenom;
    fac=newfac;
        newpdenom= NULL;
        newfac= NULL;
    }
#endif

    ~gaussElem()
    {
      if (pdenom!=NULL) nDelete( & pdenom );
      if (fac!=NULL)    nDelete( & fac );
    }
};

gaussReducer::gaussReducer( int dimen )
{
    int k;
    size= 0;
    max= dimen;
#ifndef HAVE_EXPLICIT_CONSTR
    elems= new gaussElem[ max+1 ];
#else
    elems= (gaussElem *)omAlloc( (max+1)*sizeof( gaussElem ) );
#endif
    isPivot= (BOOLEAN *)omAlloc( (max+1)*sizeof( BOOLEAN ) );
    for ( k= max; k > 0; k-- )
            isPivot[k]= FALSE;
    perm= (int *)omAlloc( (max+1)*sizeof( int ) );
}

gaussReducer::~gaussReducer()
{
#ifndef HAVE_EXPLICIT_CONSTR
    delete [] elems;
#else
    int k;
    for ( k= size; k > 0; k-- )
        elems[k].~gaussElem();
    omFreeSize( (ADDRESS)elems, (max+1)*sizeof( gaussElem ) );
#endif

    omFreeSize( (ADDRESS)isPivot, (max+1)*sizeof( BOOLEAN ) );
    omFreeSize( (ADDRESS)perm, (max+1)*sizeof( int ) );
}

BOOLEAN
gaussReducer::reduce( fglmVector thev )
{
    number fac1, fac2;
    number temp;
    // Hier ueberlegen, ob thev als referenz, oder wie wann was kopiert usw.
    v= thev;
    p= fglmVector( size + 1, size + 1 );
    // fglmASSERT( pdenom == NULL );
    pdenom= nInit( 1 );
    number vdenom = v.clearDenom();
    if ( ! nIsOne( vdenom ) && ! nIsZero( vdenom ) ) {
        p.setelem( p.size(), vdenom );
    }
    else {
        nDelete( & vdenom );
    }
    number gcd = v.gcd();
    if ( ! nIsOne( gcd ) && ! nIsZero( gcd ) ) {
        v /= gcd;
        number temp= nMult( pdenom, gcd );
        nDelete( & pdenom );
        pdenom= temp;
    }
    nDelete( & gcd );

    int k;
    for ( k= 1; k <= size; k++ ) {
        if ( ! v.elemIsZero( perm[k] ) ) {
            fac1= elems[k].fac;
            fac2= nCopy( v.getconstelem( perm[k] ) );
            v.nihilate( fac1, fac2, elems[k].v );
            fac1= nMult( fac1, elems[k].pdenom );
            temp= nMult( fac2, pdenom );
            nDelete( & fac2 );
            fac2= temp;
            p.nihilate( fac1, fac2, elems[k].p );
            temp= nMult( pdenom, elems[k].pdenom );
            nDelete( & pdenom );
            pdenom= temp;

            nDelete( & fac1 );
            nDelete( & fac2 );
            number gcd = v.gcd();
            if ( ! nIsOne( gcd ) && ! nIsZero( gcd ) )
            {
                v/= gcd;
                number temp = nMult( pdenom, gcd );
                nDelete( & pdenom );
                pdenom= temp;
            }
            nDelete( & gcd );
            gcd= p.gcd();
            temp= n_SubringGcd( pdenom, gcd, currRing->cf );
            nDelete( & gcd );
            gcd= temp;
            if ( ! nIsZero( gcd ) && ! nIsOne( gcd ) )
            {
                p/= gcd;
                temp= nDiv( pdenom, gcd );
                nDelete( & pdenom );
                pdenom= temp;
                nNormalize( pdenom );
            }
            nDelete( & gcd );
        }
    }
    return ( v.isZero() );
}

void
gaussReducer::store()
{
    // fglmASSERT( size < max );
    // number fac;
    // find the pivot-element in v:

    size++;
    int k= 1;
    while ( nIsZero(v.getconstelem(k)) || isPivot[k] ) {
        k++;
    }
    // fglmASSERT( k <= dimen, "Error(1) in fglmDdata::pivot-search");
    number pivot= v.getconstelem( k );
    int pivotcol = k;
    k++;
    while ( k <= max ) {
        if ( ! nIsZero( v.getconstelem(k) ) && ! isPivot[k] ) {
            if ( nGreater( v.getconstelem( k ), pivot ) ) {
                pivot= v.getconstelem( k );
                pivotcol= k;
            }
        }
        k++;
    }
    // fglmASSERT( ! nIsZero( pivot ), "Error(2) fglmDdata::Pivotelement ist Null" );
    isPivot[ pivotcol ]= TRUE;
    perm[size]= pivotcol;

    pivot= nCopy( v.getconstelem( pivotcol ) );
#ifndef HAVE_EXPLICIT_CONSTR
    elems[size].mac_gaussElem( v, p, pdenom, pivot );
#else
    elems[size].gaussElem( v, p, pdenom, pivot );
#endif
}

fglmVector
gaussReducer::getDependence()
{
    nDelete( & pdenom );
    // hier kann p noch gekuerzt werden, je nach Charakteristik
    fglmVector result = p;
    p= fglmVector();
    return ( result );
}
// ================================================================================

// ----------------------------------------------------------------------------
// Local Variables: ***
// compile-command: "make Singular" ***
// page-delimiter: "^\\(\\|//!\\)" ***
// fold-internal-margins: nil ***
// End: ***

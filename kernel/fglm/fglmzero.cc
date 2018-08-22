// emacs edit mode for this file is -*- C++ -*-

/****************************************
*  Computer Algebra System SINGULAR     *
****************************************/
/*
* ABSTRACT - The FGLM-Algorithm
*   Implementation of the fglm algorithm for 0-dimensional ideals,
*   based on an idea by Faugere/Gianni/Lazard and Mora.
*   The procedure CalculateFunctionals calculates the functionals
*   which define the given ideal in the source ring. They build the
*   input for GroebnerViaFunctionals, which defines the reduced
*   groebner basis for the ideal in the destination ring.
*/

/* Changes:
 * o FindUnivariatePolys added
 */




#include "kernel/mod2.h"


// assumes, that NOSTREAMIO is set in factoryconf.h, which is included
// by templates/list.h.

#include "factory/factory.h"

#include "factory/templates/ftmpl_list.h"
#include "factory/templates/ftmpl_list.cc"

#include "misc/options.h"
#include "misc/intvec.h"

#include "polys/monomials/maps.h"
#include "polys/monomials/ring.h"

#include "kernel/polys.h"
#include "kernel/ideals.h"
#include "kernel/GBEngine/kstd1.h"

#include "fglm.h"
#include "fglmvec.h"
#include "fglmgauss.h"

#define PROT(msg)
#define STICKYPROT(msg) if (BTEST1(OPT_PROT)) Print(msg)
#define PROT2(msg,arg)
#define STICKYPROT2(msg,arg) if (BTEST1(OPT_PROT)) Print(msg,arg)
#define fglmASSERT(ignore1,ignore2)



// internal Version: 1.3.1.12
// ============================================================
//!      The idealFunctionals
// ============================================================

struct matElem
{
    int row;
    number elem;
};

struct matHeader
{
    int size;
    BOOLEAN owner;
    matElem * elems;
};

class idealFunctionals
{
private:
    int _block;
    int _max;
    int _size;
    int _nfunc;
    int * currentSize;
    matHeader ** func;
    matHeader * grow( int var );
public:
    idealFunctionals( int blockSize, int numFuncs );
    ~idealFunctionals();

    int dimen() const { fglmASSERT( _size>0, "called too early"); return _size; }
    void endofConstruction();
    void map( ring source );
    void insertCols( int * divisors, int to );
    void insertCols( int * divisors, const fglmVector to );
    fglmVector addCols( const int var, int basisSize, const fglmVector v ) const;
    fglmVector multiply( const fglmVector v, int var ) const;
};


idealFunctionals::idealFunctionals( int blockSize, int numFuncs )
{
    int k;
    _block= blockSize;
    _max= _block;
    _size= 0;
    _nfunc= numFuncs;

    currentSize= (int *)omAlloc0( _nfunc*sizeof( int ) );
    //for ( k= _nfunc-1; k >= 0; k-- )
    //    currentSize[k]= 0;

    func= (matHeader **)omAlloc( _nfunc*sizeof( matHeader * ) );
    for ( k= _nfunc-1; k >= 0; k-- )
        func[k]= (matHeader *)omAlloc( _max*sizeof( matHeader ) );
}

idealFunctionals::~idealFunctionals()
{
    int k;
    int l;
    int row;
    matHeader * colp;
    matElem * elemp;
    for ( k= _nfunc-1; k >= 0; k-- ) {
        for ( l= _size-1, colp= func[k]; l >= 0; l--, colp++ ) {
            if ( ( colp->owner == TRUE ) && ( colp->size > 0 ) ) {
                for ( row= colp->size-1, elemp= colp->elems; row >= 0; row--, elemp++ )
                    nDelete( & elemp->elem );
                omFreeSize( (ADDRESS)colp->elems, colp->size*sizeof( matElem ) );
            }
        }
        omFreeSize( (ADDRESS)func[k], _max*sizeof( matHeader ) );
    }
    omFreeSize( (ADDRESS)func, _nfunc*sizeof( matHeader * ) );
    omFreeSize( (ADDRESS)currentSize, _nfunc*sizeof( int ) );
}

void
idealFunctionals::endofConstruction()
{
    _size= currentSize[0];
}

void
idealFunctionals::map( ring source )
{
    // maps from ring source to currentRing.
    int var, col, row;
    matHeader * colp;
    matElem * elemp;
    number newelem;

    int * perm = (int *)omAlloc0( (_nfunc+1)*sizeof( int ) );
    maFindPerm( source->names, source->N, NULL, 0, currRing->names,
                currRing->N, NULL, 0, perm, NULL , currRing->cf->type);
    nMapFunc nMap=n_SetMap( source->cf, currRing->cf);

    matHeader ** temp = (matHeader **)omAlloc( _nfunc*sizeof( matHeader * ));
    for ( var= 0; var < _nfunc; var ++ ) {
        for ( col= 0, colp= func[var]; col < _size; col++, colp++ ) {
            if ( colp->owner == TRUE ) {
                for ( row= colp->size-1, elemp= colp->elems; row >= 0;
                  row--, elemp++ )
                {
                    newelem= nMap( elemp->elem, source->cf, currRing->cf );
                    nDelete( & elemp->elem );
                    elemp->elem= newelem;
                }
            }
        }
        temp[ perm[var+1]-1 ]= func[var];
    }
    omFreeSize( (ADDRESS)func, _nfunc*sizeof( matHeader * ) );
    omFreeSize( (ADDRESS)perm, (_nfunc+1)*sizeof( int ) );
    func= temp;
}

matHeader *
idealFunctionals::grow( int var )
{
    if ( currentSize[var-1] == _max ) {
        int k;
        for ( k= _nfunc; k > 0; k-- )
            func[k-1]= (matHeader *)omReallocSize( func[k-1], _max*sizeof( matHeader ), (_max + _block)*sizeof( matHeader ) );
        _max+= _block;
    }
    currentSize[var-1]++;
    return func[var-1] + currentSize[var-1] - 1;
}

void
idealFunctionals::insertCols( int * divisors, int to )
{
    fglmASSERT( 0 < divisors[0] && divisors[0] <= _nfunc, "wrong number of divisors" );
    int k;
    BOOLEAN owner = TRUE;
    matElem * elems = (matElem *)omAlloc( sizeof( matElem ) );
    elems->row= to;
    elems->elem= nInit( 1 );
    for ( k= divisors[0]; k > 0; k-- ) {
        fglmASSERT( 0 < divisors[k] && divisors[k] <= _nfunc, "wrong divisor" );
        matHeader * colp = grow( divisors[k] );
        colp->size= 1;
        colp->elems= elems;
        colp->owner= owner;
        owner= FALSE;
    }
}


void
idealFunctionals::insertCols( int * divisors, const fglmVector to )
{
    // divisors runs from divisors[0]..divisors[size-1]
    fglmASSERT( 0 < divisors[0] && divisors[0] <= _nfunc, "wrong number of divisors" );
    int k, l;
    int numElems= to.numNonZeroElems();
    matElem * elems;
    matElem * elemp;
    BOOLEAN owner = TRUE;
    if ( numElems > 0 ) {
        elems= (matElem *)omAlloc( numElems * sizeof( matElem ) );
        for ( k= 1, l= 1, elemp= elems; k <= numElems; k++, elemp++ ) {
            while ( nIsZero( to.getconstelem(l) ) ) l++;
            elemp->row= l;
            elemp->elem= nCopy( to.getconstelem( l ) );
            l++; // hochzaehlen, damit wir nicht noch einmal die gleiche Stelle testen
        }
    }
    else
        elems= NULL;
    for ( k= divisors[0]; k > 0; k-- ) {
        fglmASSERT( 0 < divisors[k] && divisors[k] <= _nfunc, "wrong divisor" );
        matHeader * colp = grow( divisors[k] );
        colp->size= numElems;
        colp->elems= elems;
        colp->owner= owner;
        owner= FALSE;
    }
}

fglmVector
idealFunctionals::addCols( const int var, int basisSize, const fglmVector v ) const
{
    fglmVector result( basisSize );
    matHeader * colp;
    matElem * elemp;
    number factor, temp;
    int k, l;
    int vsize = v.size();

    fglmASSERT( currentSize[var-1]+1 >= vsize, "wrong v.size()" );
    for ( k= 1, colp= func[var-1]; k <= vsize; k++, colp++ ) {
        factor= v.getconstelem( k );
        if ( ! nIsZero( factor ) ) {
            for ( l= colp->size-1, elemp= colp->elems; l >= 0; l--, elemp++ ) {
                temp= nMult( factor, elemp->elem );
                number newelem= nAdd( result.getconstelem( elemp->row ), temp );
                nDelete( & temp );
                nNormalize( newelem );
                result.setelem( elemp->row, newelem );
            }
        }
    }
    return result;
}

fglmVector
idealFunctionals::multiply( const fglmVector v, int var ) const
{
    fglmASSERT( v.size() == _size, "multiply: v has wrong size");
    fglmVector result( _size );
    matHeader * colp;
    matElem * elemp;
    number factor, temp;
    int k, l;
    for ( k= 1, colp= func[var-1]; k <= _size; k++, colp++ ) {
        factor= v.getconstelem( k );
        if ( ! nIsZero( factor ) ) {
            for ( l= colp->size-1, elemp= colp->elems; l >= 0; l--, elemp++ ) {
                temp= nMult( factor, elemp->elem );
                number newelem= nAdd( result.getconstelem( elemp->row ), temp );
                nDelete( & temp );
                nNormalize( newelem );
                result.setelem( elemp->row, newelem );
            }
        }
    }
    return result;
}

// ============================================================
//!      The old basis
// ============================================================

//     Contains the data for a border-monomial, i.e. the monom itself
//      ( as a Singular-poly ) and its normal-form, described by an
//      fglmVector. The size of the Vector depends on the current size of
//      the basis when the normalForm was computed.
//     monom gets deleted when borderElem comes out of scope.
class borderElem
{
public:
    poly monom;
    fglmVector nf;
    borderElem() : monom(NULL), nf() {}
    borderElem( poly p, fglmVector n ) : monom( p ), nf( n ) {}
    ~borderElem() { if (monom!=NULL) pLmDelete(&monom); }
#ifndef HAVE_EXPLICIT_CONSTR
    void insertElem( poly p, fglmVector n )
    {
        monom= p;
        nf= n;
    }
#endif
};

//     This class contains the relevant data for the 'candidates'
//     The declaration of class fglmSelem is found in fglm.h

fglmSelem::fglmSelem( poly p, int var ) : monom( p ), numVars( 0 )
{
    for ( int k = (currRing->N); k > 0; k-- )
        if ( pGetExp( monom, k ) > 0 )
            numVars++;
    divisors= (int *)omAlloc( (numVars+1)*sizeof( int ) );
    divisors[0]= 0;
    newDivisor( var );
}

void
fglmSelem::cleanup()
{
    omFreeSize( (ADDRESS)divisors, (numVars+1)*sizeof( int ) );
}

//     The data-structure for the Functional-Finding-Algorithm.
class fglmSdata
{
private:
    ideal theIdeal;
    int idelems;
    int* varpermutation;

    int basisBS;
    int basisMax;
    int basisSize;
    polyset basis;  //. rem: runs from basis[1]..basis[dimen]

    int borderBS;
    int borderMax;
    int borderSize;
    borderElem * border;  //. rem: runs from border[1]..border[dimen]

    List<fglmSelem> nlist;
    BOOLEAN _state;
public:
    fglmSdata( const ideal thisIdeal );
    ~fglmSdata();

    BOOLEAN state() const { return _state; };
    int getBasisSize() const { return basisSize; };
    int newBasisElem( poly & p );
    void newBorderElem( poly & m, fglmVector v );
    BOOLEAN candidatesLeft() const { return ( nlist.isEmpty() ? FALSE : TRUE ); }
    fglmSelem nextCandidate();
    void updateCandidates();
    int getEdgeNumber( const poly m ) const;
    poly getSpanPoly( int number ) const { return pCopy( (theIdeal->m)[number-1] ); }
    fglmVector getVectorRep( const poly m );
    fglmVector getBorderDiv( const poly m, int & var ) const;
};

fglmSdata::fglmSdata( const ideal thisIdeal )
{
    // An dieser Stelle kann die BlockSize ( =BS ) noch sinnvoller berechnet
    // werden, jenachdem wie das Ideal aussieht.
    theIdeal= thisIdeal;
    idelems= IDELEMS( theIdeal );
    varpermutation = (int*)omAlloc( ((currRing->N)+1)*sizeof(int) );
    // Sort ring variables by increasing values (because of weighted orderings)
    ideal perm = idMaxIdeal(1);
    intvec *iv = idSort(perm,TRUE);
    idDelete(&perm);
    for(int i = (currRing->N); i > 0; i--) varpermutation[(currRing->N)+1-i] = (*iv)[i-1];
    delete iv;

    basisBS= 100;
    basisMax= basisBS;
    basisSize= 0;
    basis= (polyset)omAlloc( basisMax*sizeof( poly ) );

    borderBS= 100;
    borderMax= borderBS;
    borderSize= 0;
#ifndef HAVE_EXPLICIT_CONSTR
    border= new borderElem[ borderMax ];
#else
    border= (borderElem *)omAlloc( borderMax*sizeof( borderElem ) );
#endif
    // rem: the constructors are called in newBorderElem().
    _state= TRUE;
}

fglmSdata::~fglmSdata()
{
    omFreeSize( (ADDRESS)varpermutation, ((currRing->N)+1)*sizeof(int) );
    for ( int k = basisSize; k > 0; k-- )
        pLmDelete( basis + k );  //. rem: basis runs from basis[1]..basis[basisSize]
    omFreeSize( (ADDRESS)basis, basisMax*sizeof( poly ) );
#ifndef HAVE_EXPLICIT_CONSTR
    delete [] border;
#else
    for ( int l = borderSize; l > 0; l-- )
        // rem: the polys of borderElem are deleted via ~borderElem()
        border[l].~borderElem();
    omFreeSize( (ADDRESS)border, borderMax*sizeof( borderElem ) );
#endif
}

//     Inserts poly p without copying into basis, reAllocs Memory if necessary,
//      increases basisSize and returns the new basisSize.
//     Remember: The elements of basis are deleted via pDelete in ~fglmSdata!
//     Sets m= NULL to indicate that now basis is ow(e?)ing the poly.
int
fglmSdata::newBasisElem( poly & m )
{
    basisSize++;
    if ( basisSize == basisMax ) {
        basis= (polyset)omReallocSize( basis, basisMax*sizeof( poly ), (basisMax + basisBS)*sizeof( poly ) );
        basisMax+= basisBS;
    }
    basis[basisSize]= m;
    m= NULL;
    return basisSize;
}

//     Inserts poly p and fglmvector v without copying into border, reAllocs Memory
//      if necessary, and increases borderSize.
//     Remember: The poly of border is deleted via ~borderElem in ~fglmSdata!
//     Sets m= NULL to indicate that now border is ow(e?)ing the poly.
void
fglmSdata::newBorderElem( poly & m, fglmVector v )
{
    borderSize++;
    if ( borderSize == borderMax ) {
#ifndef HAVE_EXPLICIT_CONSTR
        borderElem * tempborder = new borderElem[ borderMax+borderBS ];
        for ( int k = 0; k < borderMax; k++ ) {
            tempborder[k]= border[k];
            border[k].insertElem( NULL, fglmVector() );
        }
        delete [] border;
        border= tempborder;
#else
        border= (borderElem *)omReallocSize( border, borderMax*sizeof( borderElem ), (borderMax + borderBS)*sizeof( borderElem ) );
#endif
        borderMax+= borderBS;
    }
#ifndef HAVE_EXPLICIT_CONSTR
    border[borderSize].insertElem( m, v );
#else
    border[borderSize].borderElem( m, v );
#endif
    m= NULL;
}

fglmSelem
fglmSdata::nextCandidate()
{
    fglmSelem result = nlist.getFirst();
    nlist.removeFirst();
    return result;
}

//     Multiplies basis[basisSize] with all ringvariables and inserts the new monomials
//      into the list of candidates, according to the given order. If a monomial already
//      exists, then "insertions" and "divisors" are updated.
//     Assumes that ringvar(varperm[k]) < ringvar(varperm[l]) for k > l
void
fglmSdata::updateCandidates()
{
    ListIterator<fglmSelem> list = nlist;
    fglmASSERT( basisSize > 0 && basisSize < basisMax, "Error(1) in fglmSdata::updateCandidates - wrong bassSize" );
    poly m = basis[basisSize];
    poly newmonom = NULL;
    int k = (currRing->N);
    BOOLEAN done = FALSE;
    int state = 0;
    while ( k >= 1 )
    {
        newmonom = pCopy( m );
        pIncrExp( newmonom, varpermutation[k] );
        pSetm( newmonom );
        done= FALSE;
        while ( list.hasItem() && (!done) )
        {
            if ( (state= pCmp( list.getItem().monom, newmonom )) < 0 )
                list++;
            else done= TRUE;
        }
        if ( !done )
        {
            nlist.append( fglmSelem( newmonom, varpermutation[k] ) );
            break;
        }
        if ( state == 0 )
        {
            list.getItem().newDivisor( varpermutation[k] );
            pLmDelete(&newmonom);
        }
        else
        {
            list.insert( fglmSelem( newmonom, varpermutation[k] ) );
        }
        k--;
    }
    while ( --k >= 1 )
    {
        newmonom= pCopy( m ); // HIER
        pIncrExp( newmonom, varpermutation[k] );
        pSetm( newmonom );
        nlist.append( fglmSelem( newmonom, varpermutation[k] ) );
    }
}

//     if p == pHead( (theIdeal->m)[k] ) return k, 0 otherwise
//     (Assumes that pLmEqual just checks the leading monomials without
//      coefficients.)
int
fglmSdata::getEdgeNumber( const poly m ) const
{
    for ( int k = idelems; k > 0; k-- )
        if ( pLmEqual( m, (theIdeal->m)[k-1] ) )
            return k;
    return 0;
}

//     Returns the fglmVector v, s.t.
//        p = v[1]*basis(1) + .. + v[basisSize]*basis(basisSize)
//     So the size of v depends on the current size of the basis.
//     Assumes that such an representation exists, i.e. all monoms in p have to be
//      smaller than basis[basisSize] and that basis[k] < basis[l] for k < l.
fglmVector
fglmSdata::getVectorRep( const poly p )
{
    fglmVector temp( basisSize );
    poly m = p;
    int num = basisSize;
    while ( m != NULL ) {
        int comp = pCmp( m, basis[num] );
        if ( comp == 0 ) {
            fglmASSERT( num > 0, "Error(1) in fglmSdata::getVectorRep" );
            number newelem = nCopy( pGetCoeff( m ) );
            temp.setelem( num, newelem );
            num--;
            pIter( m );
        }
        else {
            if ( comp < 0 ) {
                num--;
            }
            else {
                // This is the place where we can detect if the sourceIdeal
                // is not reduced. In this case m is not in basis[]. Since basis[]
                // is ordered this is the case, if and only if basis[i]<m
                // and basis[j]>m for all j>i
                _state= FALSE;
                return temp;
            }
        }
    }
    return temp;
}

//     Searches through the border for a monomoial bm which devides m and returns
//      its normalform in vector representation.
//     var contains the number of the variable v, s.t. bm = m * v
fglmVector
fglmSdata::getBorderDiv( const poly m, int & var ) const
{
//     int num2 = borderSize;
//     while ( num2 > 0 ) {
//         poly temp = border[num2].monom;
//         if ( pDivisibleBy( temp, m ) ) {
//             poly divisor = pDivideM( m, temp );
//             int var = pIsPurePower( divisor );
//             if ( (var != 0) && (pGetCoeff( divisor, var ) == 1) ) {
//                 Print( "poly %s divides poly %s", pString( temp ), pString( m ) );
//             }
//         }
//         num2--;
//     }
    int num = borderSize;
    while ( num > 0 ) {
        poly temp = border[num].monom;
        if ( pDivisibleBy( temp, m ) ) {
            var = (currRing->N);
            while ( var > 0 ) {
                if ( (pGetExp( m, var ) - pGetExp( temp, var )) == 1 )
                    return border[num].nf;
                var--;
            }
        }
        num--;
    }
    return fglmVector();
}

void
internalCalculateFunctionals( const ideal /*& theIdeal*/, idealFunctionals & l,
                              fglmSdata & data )
{

    // insert pOne() into basis and update the workingList:
    poly one = pOne();
    data.newBasisElem( one );
    data.updateCandidates();

    STICKYPROT(".");
    while ( data.candidatesLeft() == TRUE ) {
        fglmSelem candidate = data.nextCandidate();
        if ( candidate.isBasisOrEdge() == TRUE ) {
            int edge = data.getEdgeNumber( candidate.monom );
            if ( edge != 0 )
            {
                // now candidate is an edge, i.e. we know its normalform:
                // NF(p) = - ( tail(p)/LC(p) )
                poly nf = data.getSpanPoly( edge );
                pNorm( nf );
                pLmDelete(&nf);  //. deletes the leadingmonomial
                nf= pNeg( nf );
                fglmVector nfv = data.getVectorRep( nf );
                l.insertCols( candidate.divisors, nfv );
                data.newBorderElem( candidate.monom, nfv );
                pDelete( &nf );
                STICKYPROT( "+" );
            }
            else
            {
                int basis= data.newBasisElem( candidate.monom );
                data.updateCandidates();
                l.insertCols( candidate.divisors, basis );
                STICKYPROT( "." );
            }
        }
        else {
            int var = 0;
            fglmVector temp = data.getBorderDiv( candidate.monom, var );
            fglmASSERT( var > 0, "this should never happen" );
            fglmVector nfv = l.addCols( var, data.getBasisSize(), temp );
            data.newBorderElem( candidate.monom, nfv );
            l.insertCols( candidate.divisors, nfv );
            STICKYPROT( "-" );
        }
        candidate.cleanup();
    } //. while ( data.candidatesLeft() == TRUE )
    l.endofConstruction();
    STICKYPROT2( "\nvdim= %i\n", data.getBasisSize() );
    return;
}

//     Calculates the defining Functionals for the ideal "theIdeal" and
//     returns them in "l".
//     The ideal has to be zero-dimensional and reduced and has to be a
//     real subset of the polynomal ring.
//     In any case it has to be zero-dimensional and minimal (check this
//      via fglmIdealcheck). Any minimal but not reduced ideal is detected.
//      In this case it returns FglmNotReduced.
//     If the base domain is Q, the leading coefficients of the polys
//     have to be in Z.
//     returns TRUE if the result is valid, FALSE if theIdeal
//      is not reduced.
static BOOLEAN
CalculateFunctionals( const ideal & theIdeal, idealFunctionals & l )
{
    fglmSdata data( theIdeal );
    internalCalculateFunctionals( theIdeal, l, data );
    return ( data.state() );
}

static BOOLEAN
CalculateFunctionals( const ideal & theIdeal, idealFunctionals & l,
                      poly & p, fglmVector & v )
{
    fglmSdata data( theIdeal );
    internalCalculateFunctionals( theIdeal, l, data );
    //    STICKYPROT("Calculating vector rep\n");
    v = data.getVectorRep( p );
    // if ( v.isZero() )
    //   STICKYPROT("vectorrep is 0\n");
    return ( data.state() );
}

// ============================================================
//!      The new basis
// ============================================================

//     The declaration of class fglmDelem is found in fglm.h

fglmDelem::fglmDelem( poly & m, fglmVector mv, int v ) : v( mv ), insertions( 0 ), var( v )
{
    monom= m;
    m= NULL;
    for ( int k = (currRing->N); k > 0; k-- )
        if ( pGetExp( monom, k ) > 0 )
            insertions++;
    // Wir gehen davon aus, dass ein fglmDelem direkt bei der Erzeugung
    // auch in eine Liste eingefuegt wird. Daher wird hier automatisch
    // newDivisor aufgerufen ( v teilt ja m )
    newDivisor();
}

void
fglmDelem::cleanup()
{
    if ( monom != NULL )
    {
        pLmDelete(&monom);
    }
}

class oldGaussElem
{
public:
    fglmVector v;
    fglmVector p;
    number pdenom;
    number fac;

#ifndef HAVE_EXPLICIT_CONSTR
    oldGaussElem() : v(), p(), pdenom( NULL ), fac( NULL ) {}
#endif
    oldGaussElem( const fglmVector newv, const fglmVector newp, number & newpdenom, number & newfac ) : v( newv ), p( newp ), pdenom( newpdenom ), fac( newfac )
    {
        newpdenom= NULL;
        newfac= NULL;
    }
    ~oldGaussElem();
#ifndef HAVE_EXPLICIT_CONSTR
    void insertElem( const fglmVector newv, const fglmVector newp, number & newpdenom, number & newfac )
    {
        v= newv;
        p= newp;
        pdenom= newpdenom;
        fac= newfac;
        newpdenom= NULL;
        newfac= NULL;
    }
#endif
};

oldGaussElem::~oldGaussElem()
{
  if (fac!=NULL)    nDelete( & fac );
  if (pdenom!=NULL) nDelete( & pdenom );
}


class fglmDdata
{
private:
    int dimen;
    oldGaussElem * gauss;
    BOOLEAN * isPivot;  // [1]..[dimen]
    int * perm;  // [1]..[dimen]
    int basisSize;  //. the CURRENT basisSize, i.e. basisSize <= dimen
    polyset basis;  // [1]..[dimen]. The monoms of the new Vectorspace-basis

    int* varpermutation;

    int groebnerBS;
    int groebnerSize;
    ideal destId;

    List<fglmDelem> nlist;
public:
    fglmDdata( int dimension );
    ~fglmDdata();

    int getBasisSize() const { return basisSize; }
    BOOLEAN candidatesLeft() const { return ( nlist.isEmpty() ? FALSE : TRUE ); }
    fglmDelem nextCandidate();
    void newBasisElem( poly & m, fglmVector v, fglmVector p, number & denom );
    void updateCandidates( poly m, const fglmVector v );
    void newGroebnerPoly( fglmVector & v, poly & p );
    void gaussreduce( fglmVector & v, fglmVector & p, number & denom );
    ideal buildIdeal()
    {
        idSkipZeroes( destId );
        return destId;
    }
};

fglmDdata::fglmDdata( int dimension )
{
    int k;
    dimen= dimension;
    basisSize= 0;
    //. All arrays run from [1]..[dimen], thus omAlloc( dimen + 1 )!
#ifndef HAVE_EXPLICIT_CONSTR
    gauss= new oldGaussElem[ dimen+1 ];
#else
    gauss= (oldGaussElem *)omAlloc( (dimen+1)*sizeof( oldGaussElem ) );
#endif
    isPivot= (BOOLEAN *)omAlloc( (dimen+1)*sizeof( BOOLEAN ) );
    for ( k= dimen; k > 0; k-- ) isPivot[k]= FALSE;
    perm= (int *)omAlloc( (dimen+1)*sizeof( int ) );
    basis= (polyset)omAlloc( (dimen+1)*sizeof( poly ) );
    varpermutation = (int*)omAlloc( ((currRing->N)+1)*sizeof(int) );
    // Sort ring variables by increasing values (because of weighted orderings)
    ideal perm_id = idMaxIdeal(1);
    intvec *iv = idSort(perm_id,TRUE);
    idDelete(&perm_id);
    for(int i = (currRing->N); i > 0; i--) varpermutation[(currRing->N)+1-i] = (*iv)[i-1];
    delete iv;

    groebnerBS= 16;
    groebnerSize= 0;
    destId= idInit( groebnerBS, 1 );
}

fglmDdata::~fglmDdata()
{
  // STICKYPROT2("dimen= %i", dimen);
  // STICKYPROT2("basisSize= %i", basisSize);
  //    fglmASSERT( dimen == basisSize, "Es wurden nicht alle BasisElemente gefunden!" );
    int k;
#ifndef HAVE_EXPLICIT_CONSTR
    delete [] gauss;
#else
    // use basisSize instead of dimen because of fglmquot!
    for ( k= basisSize; k > 0; k-- )
        gauss[k].~oldGaussElem();
    omFreeSize( (ADDRESS)gauss, (dimen+1)*sizeof( oldGaussElem ) );
#endif
    omFreeSize( (ADDRESS)isPivot, (dimen+1)*sizeof( BOOLEAN ) );
    omFreeSize( (ADDRESS)perm, (dimen+1)*sizeof( int ) );
    // use basisSize instead of dimen because of fglmquot!
    //. Remember: There is no poly in basis[0], thus k > 0
    for ( k= basisSize; k > 0; k-- )
        pLmDelete( basis[k]);
    omFreeSize( (ADDRESS)basis, (dimen+1)*sizeof( poly ) );
    omFreeSize( (ADDRESS)varpermutation, ((currRing->N)+1)*sizeof(int) );
}

fglmDelem
fglmDdata::nextCandidate()
{
    fglmDelem result = nlist.getFirst();
    nlist.removeFirst();
    return result;
}

void
fglmDdata::newBasisElem( poly & m, fglmVector v, fglmVector p, number & denom )
{
// inserts m as a new basis monom. m is NOT copied but directly inserted.
// returns m=NULL to indicate, that now basis is oweing m.
    basisSize++;
    basis[basisSize]= m;
    m= NULL;
    int k= 1;
    while ( nIsZero(v.getconstelem(k)) || isPivot[k] ) {
        k++;
    }
    fglmASSERT( k <= dimen, "Error(1) in fglmDdata::pivot-search");
    number pivot= v.getconstelem( k );
    int pivotcol = k;
    k++;
    while ( k <= dimen ) {
        if ( ! nIsZero( v.getconstelem(k) ) && ! isPivot[k] ) {
            if ( nGreater( v.getconstelem( k ), pivot ) ) {
                pivot= v.getconstelem( k );
                pivotcol= k;
            }
        }
        k++;
    }
    fglmASSERT( ! nIsZero( pivot ), "Error(2) fglmDdata::Pivotelement ist Null" );
    isPivot[ pivotcol ]= TRUE;
    perm[basisSize]= pivotcol;

    pivot= nCopy( v.getconstelem( pivotcol ) );
#ifndef HAVE_EXPLICIT_CONSTR
    gauss[basisSize].insertElem( v, p, denom, pivot );
#else
    gauss[basisSize].oldGaussElem( v, p, denom, pivot );
#endif
}

void
fglmDdata::updateCandidates( poly m, const fglmVector v )
{
    ListIterator<fglmDelem> list = nlist;
    poly newmonom = NULL;
    int k = (currRing->N);
    BOOLEAN done = FALSE;
    int state = 0;
    while ( k >= 1 )
    {
        newmonom = pCopy( m );
        pIncrExp( newmonom, varpermutation[k] );
        pSetm( newmonom );
        done= FALSE;
        while ( list.hasItem() && (!done) )
        {
            if ( (state= pCmp( list.getItem().monom, newmonom )) < 0 )
                list++;
            else done= TRUE;
        }
        if ( !done )
        {
            nlist.append( fglmDelem( newmonom, v, k ) );
            break;
        }
        if ( state == 0 )
        {
            list.getItem().newDivisor();
            pLmDelete( & newmonom );
        }
        else
        {
            list.insert( fglmDelem( newmonom, v, k ) );
        }
        k--;
    }
    while ( --k >= 1 )
    {
        newmonom= pCopy( m );
        pIncrExp( newmonom, varpermutation[k] );
        pSetm( newmonom );
        nlist.append( fglmDelem( newmonom, v, k ) );
    }
}

void
fglmDdata::newGroebnerPoly( fglmVector & p, poly & m )
// Inserts gp = p[1]*basis(1)+..+p[basisSize]*basis(basisSize)+p[basisSize+1]*m as
//  a new groebner polynomial for the ideal.
// All elements (monomials and coefficients) of gp are copied, instead of m.
// Assumes that p.length() == basisSize+1.
{
    //. Baue das Polynom von oben nach unten:
    fglmASSERT( p.size() == basisSize+1, "GP::newGroebnerPoly: p has wrong size" );
    int k;
    poly result = m;
    poly temp = result;
    m= NULL;
    if ( nGetChar() > 0 ) {
        number lead = nCopy( p.getconstelem( basisSize+1 ) );
        p /= lead;
        nDelete( & lead );
    }
    if ( nGetChar() == 0 ) {
        number gcd= p.gcd();
        fglmASSERT( ! nIsZero( gcd ), "FATAL: gcd and thus p is zero" );
        if ( ! nIsOne( gcd ) )
            p /= gcd;
        nDelete( & gcd );
    }
    pSetCoeff( result, nCopy( p.getconstelem( basisSize+1 ) ) );
    for ( k= basisSize; k > 0; k-- ) {
        if ( ! nIsZero( p.getconstelem( k ) ) ) {
            temp->next= pCopy( basis[k] );
            pIter( temp );
            pSetCoeff( temp, nCopy( p.getconstelem( k ) ) );
        }
    }
    pSetm( result );
    if ( ! nGreaterZero( pGetCoeff( result ) ) ) result= pNeg( result );
    if ( groebnerSize == IDELEMS( destId ) ) {
        pEnlargeSet( & destId->m, IDELEMS( destId ), groebnerBS );
        IDELEMS( destId )+= groebnerBS;
    }
    (destId->m)[groebnerSize]= result;
    groebnerSize++;
}

void
fglmDdata::gaussreduce( fglmVector & v, fglmVector & p, number & pdenom )
{
    int k;
    number fac1, fac2;
    number temp;
    fglmASSERT( pdenom == NULL, "pdenom in gaussreduce should be NULL" );
    pdenom= nInit( 1 );
    number vdenom = v.clearDenom();
    if ( ! nIsZero( vdenom ) && ! nIsOne( vdenom ) ) {
        p.setelem( p.size(), vdenom );
    }
    else {
        nDelete( &vdenom );
    }
    number gcd = v.gcd();
    if ( ! nIsZero( gcd ) && ! nIsOne( gcd ) ) {
        v /= gcd;
        number temp= nMult( pdenom, gcd );
        nDelete( &pdenom );
        pdenom= temp;
    }
    nDelete( & gcd );

    for ( k= 1; k <= basisSize; k++ ) {

        if ( ! v.elemIsZero( perm[k] ) ) {
            fac1= gauss[k].fac;
            fac2= nCopy( v.getconstelem( perm[k] ) );
            v.nihilate( fac1, fac2, gauss[k].v );
            fac1= nMult( fac1, gauss[k].pdenom );
            temp= nMult( fac2, pdenom );
            nDelete( &fac2 );
            fac2= temp;
              p.nihilate( fac1, fac2, gauss[k].p );
            temp= nMult( pdenom, gauss[k].pdenom );
            nDelete( &pdenom );
            pdenom= temp;

            nDelete( & fac1 );
            nDelete( & fac2 );
            number gcd = v.gcd();
            if ( ! nIsZero( gcd ) && ! nIsOne( gcd ) )
            {
                v /= gcd;
                number temp= nMult( pdenom, gcd );
                nDelete( &pdenom );
                pdenom= temp;
            }
            nDelete( & gcd );
            gcd= p.gcd();
            temp= n_SubringGcd( pdenom, gcd, currRing->cf );
            nDelete( &gcd );
            gcd= temp;
            if ( ! nIsZero( gcd ) && ! nIsOne( gcd ) )
            {
                p /= gcd;
                temp= nDiv( pdenom, gcd );
                nDelete( & pdenom );
                pdenom= temp;
                nNormalize( pdenom );
            }
            nDelete( & gcd );
        }
    }
}

static ideal
GroebnerViaFunctionals( const idealFunctionals & l,
                        fglmVector iv = fglmVector() )
// If iv is zero, calculates the groebnerBasis for the ideal which is
// defined by l.
// If iv is not zero, then the groebnerBasis if i:p is calculated where
// i is defined by l and iv is the vector-representation of nf(p) wrt. i
// The dimension of l has to be finite.
// The result is in reduced form.
{
    fglmDdata data( l.dimen() );

    // insert pOne() and update workinglist according to iv:
    fglmVector initv;
    if ( iv.isZero() ) {
      // STICKYPROT("initv is zero\n");
      initv = fglmVector( l.dimen(), 1 );
    }
    else {
      // STICKYPROT("initv is not zero\n");
      initv = iv;
    }

    poly one = pOne();
    data.updateCandidates( one, initv );
    number nOne = nInit( 1 );
    data.newBasisElem( one, initv, fglmVector( 1, 1 ), nOne );
    STICKYPROT( "." );
    while ( data.candidatesLeft() == TRUE ) {
        fglmDelem candidate = data.nextCandidate();
        if ( candidate.isBasisOrEdge() == TRUE ) {
            // Now we have the chance to find a new groebner polynomial

            // v is the vector-representation of candidate.monom
            // some elements of v are zeroed in data.gaussreduce(). Which
            // ones and how this was done is stored in p.
            // originalV containes the unchanged v, which is later inserted
            // into the working list (via data.updateCandidates().
            fglmVector v = l.multiply( candidate.v, candidate.var );
            fglmVector originalV = v;
            fglmVector p( data.getBasisSize()+1, data.getBasisSize()+1 );
            number pdenom = NULL;
            data.gaussreduce( v, p, pdenom );
            if ( v.isZero() ) {
                // Now v is linear dependend to the already found basis elements.
                // This means that v (rsp. candidate.monom) is the leading
                // monomial of the next groebner-basis polynomial.
                data.newGroebnerPoly( p, candidate.monom );
                nDelete( & pdenom );
                STICKYPROT( "+" );
            }
            else {
                // no linear dependence could be found, so v ( rsp. monom )
                // is a basis monomial. We store the zeroed version ( i.e. v
                // and not originalV ) as well as p, the denomiator and all
                // the other stuff.
                // erst updateCandidates, dann newBasisELem!!!
                data.updateCandidates( candidate.monom, originalV );
                data.newBasisElem( candidate.monom, v, p, pdenom );
                STICKYPROT( "." );
            }
        }
        else {
            STICKYPROT( "-" );
            candidate.cleanup();
        }
    }  //. while data.candidatesLeft()
    STICKYPROT( "\n" );
    return ( data.buildIdeal() );
}
//<-

static ideal
FindUnivariatePolys( const idealFunctionals & l )
{
  fglmVector v;
  fglmVector p;
  ideal destIdeal = idInit( (currRing->N), 1 );

  int i;
  BOOLEAN isZero;
  int *varpermutation = (int*)omAlloc( ((currRing->N)+1)*sizeof(int) );
  ideal perm = idMaxIdeal(1);
  intvec *iv = idSort(perm,TRUE);
  idDelete(&perm);
  for(i = (currRing->N); i > 0; i--) varpermutation[(currRing->N)+1-i] = (*iv)[i-1];
  delete iv;

  for (i= 1; i <= (currRing->N); i++ )
  {
    // main loop
    STICKYPROT2( "(%i)", i /*varpermutation[i]*/);
    gaussReducer gauss( l.dimen() );
    isZero= FALSE;
    v= fglmVector( l.dimen(), 1 );
    while ( !isZero )
    {
      if ( (isZero= gauss.reduce( v )))
      {
        STICKYPROT( "+" );
        p= gauss.getDependence();
        number gcd= p.gcd();
        if ( ! nIsOne( gcd ) )
        {
          p /= gcd;
        }
        nDelete( & gcd );
        int k;
        poly temp = NULL;
        poly result=NULL;
        for ( k= p.size(); k > 0; k-- )
        {
          number n = nCopy( p.getconstelem( k ) );
          if ( ! nIsZero( n ) )
          {
            if ( temp == NULL )
            {
              result= pOne();
              temp= result;
            }
            else
            {
              temp->next= pOne();
              pIter( temp );
            }
            pSetCoeff( temp, n );
            pSetExp( temp, i /*varpermutation[i]*/, k-1 );
            pSetm( temp );
          }
        }
        if ( ! nGreaterZero( pGetCoeff( result ) ) ) result= pNeg( result );
        (destIdeal->m)[i-1]= result;
      }
      else
      {
        STICKYPROT( "." );
        gauss.store();
        v= l.multiply( v, i /*varpermutation[i]*/ );
      }
    }
  }
  STICKYPROT( "\n" );
  omFreeSize( (ADDRESS)varpermutation, ((currRing->N)+1)*sizeof(int) );
  return destIdeal;
}

// for a descritption of the parameters see fglm.h
BOOLEAN
fglmzero( ring sourceRing, ideal & sourceIdeal, ring destRing, ideal & destIdeal, BOOLEAN switchBack, BOOLEAN deleteIdeal )
{
    ring initialRing = currRing;
    BOOLEAN fglmok;

    if ( currRing != sourceRing )
    {
        rChangeCurrRing( sourceRing );
    }
    idealFunctionals L( 100, rVar(currRing) );
    fglmok = CalculateFunctionals( sourceIdeal, L );
    if ( deleteIdeal == TRUE )
        idDelete( & sourceIdeal );
    rChangeCurrRing( destRing );
    if ( fglmok == TRUE )
    {
        L.map( sourceRing );
        destIdeal= GroebnerViaFunctionals( L );
    }
    if ( (switchBack) && (currRing != initialRing) )
      rChangeCurrRing( initialRing );
    return fglmok;
}

BOOLEAN
fglmquot( ideal sourceIdeal, poly quot, ideal & destIdeal)
{
    BOOLEAN fglmok;
    fglmVector v;

    idealFunctionals L( 100, (currRing->N) );
    // STICKYPROT("calculating normal form\n");
    // poly p = kNF( sourceIdeal, currRing->qideal, quot );
    // STICKYPROT("calculating functionals\n");
    fglmok = CalculateFunctionals( sourceIdeal, L, quot, v );
    if ( fglmok == TRUE ) {
      // STICKYPROT("calculating groebner basis\n");
        destIdeal= GroebnerViaFunctionals( L, v );
    }
    return fglmok;
}

BOOLEAN
FindUnivariateWrapper( ideal source, ideal & destIdeal )
{
    BOOLEAN fglmok;

    idealFunctionals L( 100, (currRing->N) );
    fglmok = CalculateFunctionals( source, L );
    if ( fglmok == TRUE ) {
        destIdeal= FindUnivariatePolys( L );
        return TRUE;
    }
    else
        return FALSE;
}

// ----------------------------------------------------------------------------
// Local Variables: ***
// compile-command: "make Singular" ***
// page-delimiter: "^\\(\\|//!\\)" ***
// fold-internal-margins: nil ***
// End: ***

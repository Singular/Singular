// emacs edit mode for this file is -*- C++ -*-
// $Id: fglm.cc,v 1.1.1.1 1997-03-19 13:18:45 obachman Exp $ 
//=============================================
//             FGLM - Algorithm 
//=============================================

// if system allows use of <iostream.h>:
// #define FGLM_IOSTREAM 
// #define FGLM_DEBUG  
// show in any case, without option(prot), what the algorithm finds:
// #define FGLM_PROT
// in gaussreduce show, which elements are nihilated and which ones not:
// #define FGLM_GAUSSPROT  
// perform all test s.a. nTest, pTest, continuity-tests and so on:
// #define FGLM_TEST  
#undef FGLM_IOSTREAM
#undef FGLM_DEBUG
#undef FGLM_PROT
#undef FGLM_GAUSSPROT
#undef FGLM_TEST

//-> the include-stuff
#ifdef FGLM_IOSTREAM
#include <iostream.h>
#endif
#include "mod2.h"
#ifdef HAVE_FGLM
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
#include "fglm.h"
#include "fglmvec.h"
#include "fglmvec.cc"
#include <templates/list.h>
//<-

// ============================================================
//!      The idealFunctionals 
// ============================================================

//-> class idealFunctionals
//     Functional to represent a 0-dimensional Ideal.
//       Der Ring, der aktiv ist, wenn die Klasse initiiert wird, bestimmt,
//       wieviele Matrizen es gibt (naemlich pVariables Stueck). 
//     ---> Hier noch naehre Spezifikationen hin, was die so machen

struct matElem
{
    int row;
    number elem;
};

struct matHeader
{
    int num;
    int size;
    matElem * elems;
#ifdef FGLM_TEST
    int addusage;
    int multusage;
#endif
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
    matHeader * grow( int var );  // inline
public:
    idealFunctionals( int blockSize, int numFuncs );
    ~idealFunctionals();

    int dimen() const { ASSERT( _size>0, "called to early"); return _size; } 
    void endofConstruction();
    void reorder( int * perm );
    void insertCol( int var, int from, int to );
    void insertCol( int var, int from, const fglmVector to );
    fglmVector addCols( const int var, int basisSize, const fglmVector v ) const;
    fglmVector multiply( const fglmVector v, int var ) const;
#ifdef FGLM_PROT
    void print() const;
    void debugprint() const;
    void statistic() const;
#endif
};

idealFunctionals::idealFunctionals( int blockSize, int numFuncs ) 
{
    int k;
    _block= blockSize;
    _max= _block;
    _size= 0;
    _nfunc= numFuncs;

    currentSize= (int *)Alloc( _nfunc*sizeof( int ) );
    for ( k= _nfunc-1; k >= 0; k-- ) 
	currentSize[k]= 0;

    func= (matHeader **)Alloc( _nfunc*sizeof( matHeader * ) );
    for ( k= _nfunc-1; k >= 0; k-- ) 
	func[k]= (matHeader *)Alloc( _max*sizeof( matHeader ) );
}

idealFunctionals::~idealFunctionals()
{
    int k;
    int l;
    int row;
    matHeader * colp;
    matElem * elemp;
    // hier stand frueher ein ASSERT( size>0 ) das ist rausgebaut, da ein Functional
    // auch geloscht werden kann, wenn es nicht aufgebaut worden ist.
    for ( k= _nfunc-1; k >= 0; k-- ) {
	// beachte: in der aeusseren Schleife _size,
	// bei Free aber _max. Das ist richtig so.
	for ( l= _size-1, colp= func[k]; l >= 0; l--, colp++ ) {
	    if (colp->size > 0 ) {
		for ( row= colp->size-1, elemp= colp->elems; row >= 0; row--, elemp++ )
		    nDelete( & elemp->elem );
		Free( (ADDRESS)colp->elems, colp->size*sizeof( matElem ) );
	    }
	}
	Free( (ADDRESS)func[k], _max*sizeof( matHeader ) );
    }
    Free( (ADDRESS)func, _nfunc*sizeof( matHeader * ) );
    Free( (ADDRESS)currentSize, _nfunc*sizeof( int ) );
}

void
idealFunctionals::endofConstruction() 
{
    _size= currentSize[0];
#ifdef FGLM_TEST
    int var, col, row;
    matHeader * colp;
    matElem * elemp;
    for ( var= 0; var < _nfunc; var++ ) {
	ASSERT( currentSize[var] == _size, "illegal number of columns" );
	for ( col= 0, colp= func[var]; 
	      col < _size; 
	      col++, colp++ ) {
	    for ( row= colp->size-1, elemp= colp->elems; 
		  row >= 0; 
		  row--, elemp++ ) {
		nTest( elemp->elem );
	    }
	}
    }
#endif
    // hier wurde frueher currentSize freigegeben.
}

void
idealFunctionals::reorder( int * perm ) 
{
    // simulates a change of the variables.
    // assumes that perm has size _nfunc+1, i.e it runs
    // from perm[1]..perm[_nfunc]

    matHeader ** temp = (matHeader **)Alloc( _nfunc*sizeof( matHeader * ));
    for ( int k = _nfunc; k > 0; k-- ) {
	temp[perm[k]-1]= func[k-1];
    }
    Free( (ADDRESS)func, _nfunc*sizeof( matHeader * ) );
    func= temp;
}

matHeader *
idealFunctionals::grow( int var ) 
{
    if ( currentSize[var-1] == _max ) {
	int k;
#ifdef FGLM_PROT
	Print( "(f+)" );
#endif
	for ( k= _nfunc; k > 0; k-- )
	    func[k-1]= (matHeader *)ReAlloc( func[k-1], _max*sizeof( matHeader ), (_max + _block)*sizeof( matHeader ) );
	_max+= _block;
    }
    currentSize[var-1]++;
    return func[var-1] + currentSize[var-1] - 1;   // -1, da wir von 0 ab zaehlen
}

void
idealFunctionals::insertCol( int var, int from, int to )
{
    ASSERT( 0 < var && var <= _nfunc, "illegal variable");
    matHeader * colp = grow( var );
    colp->num= from;
    colp->size= 1;
    colp->elems= (matElem *)Alloc( sizeof( matElem ) );
#ifdef FGLM_TEST
    colp->addusage= 0;
    colp->multusage= 0;
#endif
    colp->elems->row= to;
    colp->elems->elem= nInit( 1 );
}
    
void
idealFunctionals::insertCol( int var, int from, const fglmVector to )
{
    ASSERT( 0 < var && var <= _nfunc, "illegal variable");
    int k, l;
    int numElems= to.numNonZeroElems();
    matHeader * colp = grow( var );
    matElem * elemp;
    colp->num= from;
    colp->size= numElems;
#ifdef FGLM_TEST
    colp->addusage= 0;
    colp->multusage= 0;
#endif
    if ( numElems > 0 ) {
	colp->elems= (matElem *)Alloc( numElems*sizeof( matElem ) );
	for ( k= 1, l= 1, elemp= colp->elems; k <= numElems; k++, elemp++ ) {
	    while ( nIsZero( to.getconstelem(l) ) ) l++;
	    elemp->row= l;
	    elemp->elem= nCopy( to.getconstelem( l ) );
	    l++; // hochzaehlen, damit wir nicht noch einmal die gleiche Stelle testen
	}
    }
    else colp->elems= NULL;
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

    for ( k= currentSize[var-1]-1, colp= func[var-1]; k >= 0; k--, colp++ ) {
	if ( colp->num <= vsize ) {
	    factor= v.getconstelem( colp->num );
	    if ( ! nIsZero( factor ) ) {
#ifdef FGLM_TEST
		colp->addusage++;
#endif
		for ( l= colp->size-1, elemp= colp->elems; l >= 0; l--, elemp++ ) {
		    temp= nMult( factor, elemp->elem );
		    number newelem= nAdd( result.getconstelem( elemp->row ), temp );
		    nDelete( & temp );
		    nNormalize( newelem );
		    result.setelem( elemp->row, newelem );
		}    
	    }
	}
    }
    return result;
}

fglmVector
idealFunctionals::multiply( const fglmVector v, int var ) const
{
    ASSERT( v.size() == _size, "multiply: v has wrong size");
    fglmVector result( _size );
    matHeader * colp;
    matElem * elemp;
    number factor, temp;
    int k, l;

    for ( k= _size-1, colp= func[var-1]; k >= 0; k--, colp++ ) {
	factor= v.getconstelem( colp->num );
	if ( ! nIsZero( factor ) ) {
#ifdef FGLM_TEST
	    colp->multusage++;
#endif
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

#ifdef FGLM_PROT
void
idealFunctionals::print() const
{
    // nach der Hammermethode programmiert
    int f;
    int row;
    int col;
    int s1, s2;
    matHeader * head;
    BOOLEAN found = FALSE;
    ASSERT( _size > 0, "called too early" );
    for ( f= 0; f < _nfunc; f++ ) {
	Print( "var %i :\n", f+1 );
	for ( row= 0; row < _size; row++ ) {
	    for ( col= 0; col < _size; col++ ) {
		head= func[f];
		found= FALSE;
		for ( s1= 0; s1 < _size; s1++ ) {
		    if ( head[s1].num == col+1 ) {
			for ( s2= 0; s2 < head[s1].size; s2++ ) {
			    if ( head[s1].elems[s2].row == row+1 ) {
				StringSet("");
				nWrite( head[s1].elems[s2].elem );
				Print( StringAppend( " " ) );
				found= TRUE;
			    }
			}
		    }
		}
		if ( found == FALSE ) Print( "0 " );
	    }
	    Print( "\n" );
	}
	Print( "\n" );
    }
}

void 
idealFunctionals::debugprint() const 
{
    int var, col, row;
    matHeader * colp;
    matElem * elemp;
    
    ASSERT( _size > 0, "called too early" );
    Print( "\n size = %i", _size );
    for ( var= 0; var < _nfunc; var++ ) {
	Print( "\nvar %i : \n", var+1 );
	for ( col= 0, colp= func[var]; 
	      col < _size; 
	      col++, colp++ ) {
	    Print( " col %i: [%i] ", colp->num, colp->size );
	    for ( row= colp->size-1, elemp= colp->elems; 
		  row >= 0; 
		  row--, elemp++ ) {
		Print( "(%i,", elemp->row );
		StringSet("");
		nWrite( elemp->elem );
		Print( StringAppend( ") " ) );
	    }
	    Print( "\n" );
	}
    }
}

void
idealFunctionals::statistic() const
{
    int k;
    int col;
    int sum;
#ifdef FGLM_TEST
    int addsum;
    int multsum;
    int neverused;
#endif
    matHeader * colp;
    ASSERT( _size > 0, "called too early" );
    Print( "Functional-Statistic: \n" );
    for ( k= 1; k <= _nfunc; k++ ) {
	Print( "var %i: ", k );
	sum= 0; 
#ifdef FGLM_TEST
	addsum= 0; multsum= 0; neverused= 0;
#endif
// 	cout << "("; cout.flush();
	Print( "(" );
	for ( col= 0, colp= func[ k-1 ];
	      col < _size;
	      col++, colp++ ) {
	    sum+= colp->size;
#ifdef FGLM_TEST
	    if ( colp->addusage > 0 ) addsum++;
	    if ( colp->multusage > 0 ) multsum++;
	    if ( (colp->addusage == 0) && (colp->multusage == 0) ) neverused++;
	    if ( _size < 100 )
		Print( " %i/%i", colp->addusage, colp->multusage );
#endif
// 	    cout << int ( (double(colp->size)/double(_size))*100 ) << "%, ";
	}
	Print( " )\n" );
// 	cout << ") summe: "; cout.flush();
#ifdef FGLM_TEST
	Print( "Auslastung: %i%% addcols: %i%% mult: %i%% neverused: %i%%\n", int(100*double(sum)/(_size*_size)), int(100*double(addsum)/_size), int(100*double(multsum)/_size), int(100*double(neverused)/_size) );
#else
	Print( "Auslastung: %i%%\n", int(100*double(sum)/(_size*_size)) );
#endif
    }
}

#endif

//<-

// ============================================================
//!      The old basis 
// ============================================================

//-> class borderElem
//     Contains the data for a border-monomial, i.e. the monom itself
//      ( as a Singular-poly ) and its normal-form, described by an
//      fglmVector. The size of the Vector depends on the current size of
//      the basis when the normalForm was computed.
//     monom gets deleted when borderElem comes out of scope.
class borderElem 
{
private:
    poly monom;
    fglmVector nf;
public:
    borderElem() : monom(NULL), nf() {}
    borderElem( poly p, fglmVector n ) : monom( p ), nf( n ) {}
    ~borderElem() { pDelete1( &monom ); }

    poly & getMonom() { return monom; }
    fglmVector & getNF() { return nf; }
};
//<-

//-> struct divInfo
//     Contains the number of a basis-monom and the number of a
//     ring-variable, s.t. monom=basis*var.
//     Used in class listElem.divisors
struct divInfo
{
    int basis;
    int var;
    divInfo() : basis( 0 ), var( 0 ) {}
    divInfo( int b, int v ) : basis( b ), var( v ) {}
};
//<-

//-> class listElem (Definitions)
//     This class contains the relevant data for the 'candidates'
//     The declaration of class listElem is found in fglm.h

listElem::listElem() : insertions( 0 ), numVars( 0 ), monom( NULL ), divisors( 0 ) {}

listElem::listElem( poly p, int basis, int var ) : insertions( 0 ), numVars( 0 ), monom( p ) 
{
    for ( int k = pVariables; k > 0; k-- ) 
	if ( pGetExp( monom, k ) > 0 ) 
	    numVars++;
    divisors= (divInfo *)Alloc( numVars*sizeof( divInfo ) );
    newDivisor( basis, var );
}

void 
listElem::cleanup()
{
#ifdef FGLM_TEST
    if ( monom != NULL ) Print( "listElem.cleanup: monom!=0\n" );
// ACHTUNG:: Soll das pdelete1 hier wirklich im ifdef stehen?
    pDelete1( &monom );
#endif
    Free( (ADDRESS)divisors, numVars*sizeof( divInfo ) );
}

poly & 
listElem::getMonom() 
{ 
    return monom; 
}

BOOLEAN 
listElem::isBasisOrEdge() const
{
    return ( (insertions == numVars) ? TRUE : FALSE );
}

void 
listElem::newDivisor( int basis, int var ) 
{
    //. rem: divisors[] counts from zero, i.e. divisors[0]..divisors[insertions-1]
    divisors[insertions]= divInfo( basis, var );
    insertions++;
}

void 
listElem::updateFunctionals( int basis, idealFunctionals & l ) const 
{
    for ( int k = insertions; k > 0; k-- ) {
	l.insertCol( divisors[k-1].var, divisors[k-1].basis, basis );
    }
}

void 
listElem::updateFunctionals( const fglmVector v, idealFunctionals & l ) const 
{
    for ( int k = insertions; k > 0; k-- ) {
	l.insertCol( divisors[k-1].var, divisors[k-1].basis, v );
    }
}

ostream &
operator << ( ostream & os, const listElem & l )
{
    return os;
}
//<-

//-> class FP
//     The data-structure for the Functional-Finding-Algorithm.
class FP
{
private:
    ideal theIdeal;
    int idelems;

    int basisBS;
    int basisMax;
    int basisSize;
    poly * basis;  //. rem: runs from basis[1]..basis[dimen]

    int borderBS;
    int borderMax;
    int borderSize;
    borderElem * border;  //. rem: runs from border[1]..border[dimen]

    List<listElem> nlist;
public:
    FP( const ideal thisIdeal );
    ~FP();

    int getBasisSize() const { return basisSize; };
    int newBasisElem( poly & p );
    void newBorderElem( poly & m, fglmVector v );
    BOOLEAN candidatesLeft() const;
    listElem nextCandidate();
    void updateCandidates();
    int getEdgeNumber( const poly m ) const;
    poly getSpanPoly( int number ) const;
    fglmVector getVectorRep( const poly m ) const;
    fglmVector getBorderDiv( const poly m, int & var ) const;
#ifdef FGLM_PROT
    void printnlist() const;
    void printBasis() const;
    void printBorder() const;
#endif
};

//-> class FP (Method-definition)
FP::FP( const ideal thisIdeal ) 
{
    // An dieser Stelle kann die BlockSize ( =BS ) noch sinnvoller berechnet
    // werden, jenachdem wie das Ideal aussieht.
    theIdeal= thisIdeal;
    idelems= IDELEMS( theIdeal );

    basisBS= 100;
    basisMax= basisBS;
    basisSize= 0;
    basis= (poly *)Alloc( basisMax*sizeof( poly ) );  

    borderBS= 100;
    borderMax= borderBS;
    borderSize= 0;
    border= (borderElem *)Alloc( borderMax*sizeof( borderElem ) );
    // rem: the constructors are called in newBorderElem().
}

FP::~FP()
{
    for ( int k = basisSize; k > 0; k-- )
	pDelete1( basis + k );  //. rem: basis runs from basis[1]..basis[basisSize]
    Free( (ADDRESS)basis, basisMax*sizeof( poly ) );
    // HIER STAND int k
    for ( int l = borderSize; l > 0; l-- )
	// rem: the polys of borderElem are deleted via ~borderElem()
	border[l].~borderElem();
    Free( (ADDRESS)border, borderMax*sizeof( borderElem ) );
}

//-> FP::newBasisElem( poly & m )
//     Inserts poly p without copying into basis, reAllocs Memory if necessary, 
//      increases basisSize and returns the new basisSize.
//     Remember: The elements of basis are deleted via pDelete in ~FP!
//     Sets m= NULL to indicate that now basis is ow(e?)ing the poly.
int
FP::newBasisElem( poly & m ) 
{
    basisSize++;
    if ( basisSize == basisMax ) {
#ifdef FGLM_PROT
	Print( "(ba+)" ); 
#endif
	basis= (poly *)ReAlloc( basis, basisMax*sizeof( poly ), (basisMax + basisBS)*sizeof( poly ) );
	basisMax+= basisBS;
    }
    basis[basisSize]= m;
    m= NULL;
#ifdef FGLM_TEST
    for ( int k = 1; k <= basisSize; k++ ) 
	pTest( basis[k] );
#endif
    return basisSize;
}
//<-

//-> void FP::newBorderElem( poly & m, fglmVector v )
//     Inserts poly p and fglmvector v without copying into border, reAllocs Memory 
//      if necessary, and increases borderSize.
//     Remember: The poly of border is deleted via ~borderElem in ~FP!
//     Sets m= NULl to indicate that now border is ow(e?)ing the poly.
void
FP::newBorderElem( poly & m, fglmVector v )
{
    borderSize++;
    if ( borderSize == borderMax ) {
#ifdef FGLM_PROT
	Print( "(bo+)" );
#endif
	border= (borderElem *)ReAlloc( border, borderMax*sizeof( borderElem ), (borderMax + borderBS)*sizeof( borderElem ) );
	borderMax+= borderBS;
    }
    // WARNING: The constructor is called explicit!
    border[borderSize].borderElem( m, v );
    m= NULL;
}
//<-

BOOLEAN
FP::candidatesLeft() const
{
    return ( nlist.isEmpty() ? FALSE : TRUE );
}

listElem
FP::nextCandidate()
{
    listElem result = nlist.getFirst();
    nlist.removeFirst();
    return result;
}

//-> FP::updateCandidates()
//     Multiplies basis[basisSize] with all ringvariables and inserts the new monomials 
//      into the list of candidates, according to the given order. If a monomial already
//      exists, then "insertions" and "divisors" are updated. 
//     Assumes that ringvar(k) < ringvar(l) for k < l ???? STIMMT DAS ????
void
FP::updateCandidates()
{
    ListIterator<listElem> list = nlist;
    ASSERT( basisSize > 0 && basisSize< basisMax, "Error(1) in FP::updateCandidates - wrong basisSize");
    poly m = basis[basisSize];
    poly newmonom = NULL;
    int k = pVariables;
    BOOLEAN done = FALSE;
    int state = 0;
    while ( k >= 1 ) {
	newmonom = pCopy( m );  // HIER
	pGetExp( newmonom, k )++;
	pSetm( newmonom );
	done= FALSE;
	while ( list.hasItem() && (done == FALSE) ) {
	    if ( (state= pComp( list.getItem().getMonom(), newmonom )) < 0 )
		list++;
	    else done= TRUE;
	}
	if ( done == FALSE ) {
	    nlist.append( listElem( newmonom, basisSize, k ) );
	    break;
	}
	if ( state == 0 ) {
	    list.getItem().newDivisor( basisSize, k );
	    pDelete1( &newmonom );
	}
	else {
	    list.insert( listElem( newmonom, basisSize, k ) );
	    // ACHTUNG: list++ rausgebaut ( siehe Buch Seite 56 )
//	    list++;
	}
	k--;
    }
    while ( --k >= 1 ) {
	newmonom= pCopy( m ); // HIER
	pGetExp( newmonom, k )++;
	pSetm( newmonom );
	nlist.append( listElem( newmonom, basisSize, k ) );
    }
#ifdef  FGLM_TEST
    if ( nlist.length() >= 2 ) {
	list= nlist;
	ListIterator<listElem> next= nlist;
	next++;
	while ( next.hasItem() ) {
	    if ( pComp( list.getItem().getMonom(), next.getItem().getMonom() ) > 0 ) {
		Print( "ERROR: FP::nlist ist nicht sortiert (basisSize=%i)" , basisSize );
	    }
	    list++; next++;
	}
	list= nlist;
	while ( list.hasItem() ) {
	    next= list;
	    next++;
	    while ( next.hasItem() ) {
		if ( pComp( list.getItem().getMonom(), next.getItem().getMonom() ) == 0 ) {
		    Print( "ERROR: GF::nlist das Monom %s existiert doppelt (basisSize=%i)", pString( list.getItem().getMonom() ), basisSize );
		}
		next++;
	    }
	    list++;
	}
    }
#endif
}
//<-

//-> int FP::getEdgeNumber( const poly m ) const
//     if p == pHead( (theIdeal->m)[k] ) return k, 0 otherwise
//     (Assumes that pEqual just checks the leading monomials without
//      coefficients.)
int
FP::getEdgeNumber( const poly m ) const
{
    for ( int k = idelems; k > 0; k-- ) 
	if ( pEqual( m, (theIdeal->m)[k-1] ) )
	    return k;
    return 0;
}
//<-

//-> poly FP::getSpanPoly( int number ) const
poly
FP::getSpanPoly( int number ) const
{
    return pCopy( (theIdeal->m)[number-1] );
}
//<-

//-> fglmVector FP::getVectorRep( const poly p ) const
//     Returns the fglmVector v, s.t. 
//        p = v[1]*basis(1) + .. + v[basisSize]*basis(basisSize)
//     So the size of v depends on the current size of the basis.
//     Assumes that such an representation exists, i.e. all monoms in p have to be
//      smaller than basis[basisSize] and that basis[k] < basis[l] for k < l.
fglmVector
FP::getVectorRep( const poly p ) const
{
    fglmVector temp( basisSize );
    poly m = p;
    int num = basisSize;
    while ( m != NULL ) {
	int comp = pComp( m, basis[num] );
	if ( comp == 0 ) {
	    ASSERT( num > 0, "Error(1) in FP::getVectorRep" );
	    number newelem = nCopy( pGetCoeff( m ) );
	    temp.setelem( num, newelem );
	    num--;
	    m= pIter( m );
	}
        else if ( comp < 0 ) {
	    num--;
	}
#ifdef FGLM_TEST
	else Print( "Error(2) in FP::getVectorRep!\n" );
#endif
    }
    return temp;
}
//<-

//-> FP::getBorderDiv(const poly m, int & var ) const
//     Searches through the border for a monomoial bm which devides m and returns
//      its normalform in vector representation.
//     var contains the number of the variable v, s.t. bm = m * v 
fglmVector
FP::getBorderDiv( const poly m, int & var ) const
{
    int num = borderSize;
    while ( num > 0 ) {
	poly temp = border[num].getMonom();
	if ( pDivisibleBy( temp, m ) ) {
	    //. Bemerkung: Wir suchen eine Variable, s.d. bm=m*v, d.h. ohne
	    //. Exponenten. Da wir border aber von oben nach unten durchsuchen,
	    //. ( und in border alle Elemente des Randes, die kleiner als
	    //.  border[borderSize] sind, enthalten sind)
	    //. wissen wir, dass, sobald ein monom teilt, es auch eine solche
	    //. Variable geben muss. Es ist hier also keine weitere Ueberpfruefung
	    //. notwendig.
	    var = pVariables;
	    while ( var > 0 ) {
		if ( (pGetExp( m, var ) - pGetExp( temp, var )) == 1 ) 
		    return border[num].getNF();
		var--;
	    }
#ifdef FGLM_TEST
	    Print( "Error(2) in FP::getBorderDiv\n" );
	    break;
#endif
	}
	num--;
    }
#ifdef FGLM_TEST
    Print( "Error(1) in FP::getBorderDiv\n" );
#endif
    return fglmVector();
}
//<-

#ifdef FGLM_PROT
void
FP::printnlist() const
{
    Print( "size= %i\n", nlist.length() );
    Print( "<" );
    ListIterator<listElem> list= nlist;
    while ( list.hasItem() ) {
	Print( "%s ",pString( list.getItem().getMonom() ) );
	list++;
    }
    Print( " >\n" );
}


void
FP::printBasis() const
{
    Print( "Basis: ( " );
    for ( int k = 1; k < basisSize; k++ ) {
	Print( "%s, ", 	pString( basis[k] ) );
    }
    Print( "%s )", pString( basis[basisSize] ) );
}

void
FP::printBorder() const
{
    Print( "Border: ( ");
    for ( int k = 1; k < borderSize; k++ ) {
	Print( "%s, ", pString( border[k].getMonom() ) );
    }
    Print( "%s )", pString( border[borderSize].getMonom() ) );
}
#endif
//<-
//<-

//-> void CalculateFunctionals( const ideal & theIdeal, idealFunctionals & l )
//     Calculates the defining Functionals for the ideal "theIdeal" and 
//     returns them in "l".
//     The ideal has to be zero-dimensional and reduced and has to be a
//     real subset of the polynomal ring.
//     If the base domain is Q, the leading coefficients of the polys
//     have to be in Z.
void
CalculateFunctionals( const ideal & theIdeal, idealFunctionals & l )
{
    FP data( theIdeal );

    // insert pOne() into basis and update the workingList:
    poly one = pOne();
    data.newBasisElem( one );
    data.updateCandidates();

#ifdef FGLM_PROT
    Print( "." );
#else
    if (BTEST1(OPT_PROT)) Print(".");
#endif

    while ( data.candidatesLeft() == TRUE ) {
	listElem candidate = data.nextCandidate();
	if ( candidate.isBasisOrEdge() == TRUE ) {
	    int edge = data.getEdgeNumber( candidate.getMonom() );
	    if ( edge != 0 ) {
		// now candidate is an edge, i.e. we know its normalform:
		// NF(p) = - ( tail(p)/LC(p) )
		poly nf = data.getSpanPoly( edge );
		pNorm( nf );
		pDelete1( &nf );  //. deletes the leadingmonomial
		nf= pNeg( nf );
		fglmVector nfv = data.getVectorRep( nf );
		data.newBorderElem( candidate.getMonom(), nfv );
		candidate.updateFunctionals( nfv, l );
		pDelete( &nf );
//		if (BTEST1(OPT_PROT)) Print("+");
#ifdef FGLM_PROT
		Print( "+" );
#else
		if (BTEST1(OPT_PROT)) Print("+");
#endif
	    }
	    else {
		int basis = data.newBasisElem( candidate.getMonom() );
		//. jetzt ist basisSize hochgezaehlt.
#ifdef FGLM_PROT
	    Print( "<" );
#endif
		data.updateCandidates();
#ifdef FGLM_PROT
	    Print( ">" );
#endif
		candidate.updateFunctionals( basis, l );
#ifdef FGLM_PROT
		Print( "." );
#else
		if (BTEST1(OPT_PROT)) Print(".");
#endif
	    }
	}
	else {
	    int var = 0;
	    fglmVector temp = data.getBorderDiv( candidate.getMonom(), var );
	    ASSERT( var > 0, "this should never happen" );
	    fglmVector nfv = l.addCols( var, data.getBasisSize(), temp );
	    data.newBorderElem( candidate.getMonom(), nfv );
	    candidate.updateFunctionals( nfv, l );
#ifdef FGLM_PROT
	    Print( "-" );
#else
	    if (BTEST1(OPT_PROT)) Print("-");
#endif
	}
	candidate.cleanup();
    } //. while ( data.candidatesLeft() == TRUE )
    l.endofConstruction();
#ifdef FGLM_PROT
    Print( "\n" );
    if ( data.getBasisSize() <= 10 ) {
	data.printBasis(); Print( "\n" );
	data.printBorder(); Print( "\n" );
	Print( "matrices:\n" );
	l.print();
	l.debugprint();
    }
#endif

#ifdef FGLM_PROT
    Print( "\nvdim= %i\n", data.getBasisSize() );
#else
    if (BTEST1(OPT_PROT)) Print( "\nvdim= %i\n", data.getBasisSize() );
#endif
}
//<- calculateFunctionals

// ============================================================
//!      The new basis
// ============================================================

//-> class fglmElem ( Method-definitions )
//     The declaration of class fglmElem is found in fglm.h

fglmElem::fglmElem( poly & m, fglmVector mv, int v ) : insertions( 0 ), v( mv ), var( v ) 
{
    monom= m; 
    m= NULL;
    for ( int k = pVariables; k > 0; k-- )
	if ( pGetExp( monom, k ) > 0 )
	    insertions++;
    // Wir gehen davon aus, dass ein fglmElem direkt bei der Erzeugung
    // auch in eine Liste eingefuegt wird. Daher wird hier automatisch
    // newDivisor aufgerufen ( v teilt ja m )
    newDivisor();
}

void
fglmElem::cleanup()
{
    if ( monom != NULL ) {
// #ifdef FGLM_DEBUG
// 	cout << "[cleanup:deleting monom]"; cout.flush();
// #endif
	pDelete1( &monom );
    }
}

BOOLEAN 
fglmElem::isBasisOrEdge() const 
{
    return ( (insertions == 0) ? TRUE : FALSE );
}

poly &
fglmElem::getMonom()
{ 
    return monom; 
}

fglmVector
fglmElem::getVector() 
{
    return v;
}

int
fglmElem::getVar()
{
    return var;
}

int
fglmElem::getInsertions()
{
    return insertions;
}

void
fglmElem::newDivisor()
{
    insertions--;
}

ostream &
operator << ( ostream & os, const fglmElem & l )
{
    return os;
}
//<-

//-> class gaussElem
class gaussElem
{
public:
    fglmVector v;
    fglmVector p;
    number pdenom;
    number fac;

    gaussElem( const fglmVector newv, const fglmVector newp, number & newpdenom, number & newfac ) : v( newv ), p( newp ) 
    {
	pdenom= newpdenom;
	newpdenom= NULL;
	fac = newfac;
	newfac= NULL;
    }
    ~gaussElem();
};

gaussElem::~gaussElem()
{
    // Die Destruktoren fuer v und triseq werden automatisch aufgerufen.
// #ifdef FGLM_IOSTREAM
//     cout << "(~gE)"; cout.flush();
// #endif
    nDelete( & fac );
    nDelete( & pdenom );
}

//<-

//-> class GF
class GF 
{
private:
    int dimen;
    gaussElem * gauss;
    BOOLEAN * isPivot;  // [1]..[dimen]
    int * perm;  // [1]..[dimen]
    int basisSize;  //. the CURRENT basisSize, i.e. basisSize <= dimen
    poly * basis;  // [1]..[dimen]. The monoms of the new Vectorspace-basis
    
    int groebnerBS;
    int groebnerMax;
    int groebnerSize;
    poly * groebner;  //. The Array in which to store the new groebnerpolys

    List<fglmElem> nlist;
public:
    GF( int dimension );
    ~GF();

#ifdef FGLM_PROT
    void print();
    void printnlist();
#endif
    
    int getBasisSize() const { return basisSize; }
    BOOLEAN candidatesLeft() const;
    fglmElem nextCandidate();
    void newBasisElem( poly & m, fglmVector v, fglmVector p, number & denom );
    void updateCandidates( poly m, const fglmVector v );
    void newGroebnerPoly( fglmVector & v, poly & p );
    void gaussreduce( fglmVector & v, fglmVector & p, number & denom );
    ideal buildIdeal() const;
};

GF::GF( int dimension )
{
    dimen= dimension;
    basisSize= 0;
    //. All arrays run from [1]..[dimen], thus Alloc( dimen + 1 )!
    gauss= (gaussElem *)Alloc( (dimen+1)*sizeof( gaussElem ) );
    isPivot= (BOOLEAN *)Alloc( (dimen+1)*sizeof( BOOLEAN ) );
    for ( int k = dimen; k > 0; k-- ) isPivot[k]= FALSE;
    perm= (int *)Alloc( (dimen+1)*sizeof( int ) );
    basis= (poly *)Alloc( (dimen+1)*sizeof( poly ) );
    //. Hier kann groebnerBS verbessert werden?
    groebnerBS= 10;
    groebnerMax= groebnerBS;
    groebnerSize= 0;
    groebner= (poly *)Alloc( groebnerMax*sizeof( poly ) );  //. Ab 0 indiziert!
}

GF::~GF()
{
    // ACHTUNG: Evtl muss fuer gaussElem ein destr. aufgreufen werden!
    ASSERT( dimen == basisSize, "Es wurden nicht alle BasisElemente gefunden!" );
    int k;
    for ( k= dimen; k > 0; k-- ) 
	gauss[k].~gaussElem();
    Free( (ADDRESS)gauss, (dimen+1)*sizeof( gaussElem ) );
    Free( (ADDRESS)isPivot, (dimen+1)*sizeof( BOOLEAN ) );
    Free( (ADDRESS)perm, (dimen+1)*sizeof( int ) );
    //. Remember: There is no poly in basis[0], thus k > 0
    for ( k= dimen; k > 0; k-- )
	pDelete1( basis + k );
    Free( (ADDRESS)basis, (dimen+1)*sizeof( poly ) );
    //. Die polynome aus groebner werden nicht geloescht, da sie in das ideal kommen.
    Free( (ADDRESS)groebner, groebnerMax*sizeof( poly ) );
}

#ifdef FGLM_PROT
void
GF::print() 
{
    int k;
    Print( "dimen= %i\n", dimen );
    Print( "basisSize= %i\n", basisSize );
    Print( "perm=" );
    for ( k= 1; k <= basisSize; k++ ) Print( " %i", perm[k] );
    Print( "\n" );
    Print( "isPivot=" );
    for ( k= 1; k <= dimen; k++ ) Print( " %i",isPivot[k] );
    Print( "\n" );
    Print( "basis:" );
    for ( k= 1; k <= basisSize; k++ )  Print( " %s", pString( basis[k] ) );
    Print( "\n" );
    Print( "groebnerSize= %i\n", groebnerSize );
    Print( "groebner=" );
    for ( k= 0; k < groebnerSize; k++ ) Print(" %s", pString( groebner[k] ) );
    Print( "\n" );
    Print( "gauss:\n" );
    for ( k= 1; k <= basisSize; k++ ) {
#ifdef FGLM_IOSTREAM
	cout << "[" << k << "] v= " << gauss[k].v << endl;
	cout << "[" << k << "] p= " << gauss[k].p << endl;
	cout << "[" << k << "] pdenom= ";
	StringSet( "" );
	nWrite( gauss[k].pdenom );
	cout << StringAppend( "" ) << endl;
	cout << "[" << k << "] fac= ";
	StringSet( "" );
	nWrite( gauss[k].fac );
	cout << StringAppend( "" ) << endl;
	cout << endl;
#endif
    }
}

void
GF::printnlist()
{
    Print( "size= %i\n", nlist.length() );
    Print( "<" );
    ListIterator<fglmElem> list= nlist;
    while ( list.hasItem() ) {
	Print( "(%s,%i)",pString( list.getItem().getMonom() ), list.getItem().getInsertions() );
	list++;
    }
    Print( " >\n" );
}
#endif

BOOLEAN
GF::candidatesLeft() const 
{
    return ( nlist.isEmpty() ? FALSE : TRUE );
}

fglmElem
GF::nextCandidate() 
{
    fglmElem result = nlist.getFirst();
    nlist.removeFirst();
    return result;
}

void 
GF::newBasisElem( poly & m, fglmVector v, fglmVector p, number & denom )
{
// inserts m as a new basis monom. m is NOT copied but directly inserted.
// returns m=NULL to indicate, that now basis is oweing m.
    basisSize++;
    basis[basisSize]= m;
    m= NULL;
    // Aus v das pivot-Element heraussuchen:
    int k= 1;
    while ( nIsZero(v.getconstelem(k)) || isPivot[k] ) {
	k++;
    }
    ASSERT( k <= dimen, "Error(1) in GF::pivot-search");
    // ACHTUNG hier nCopy eingesetzt.
    number pivot= v.getconstelem( k );
    int pivotcol = k;
    // hier ein k++ eingebaut
    k++;
    while ( k <= dimen ) {
	if ( ! nIsZero( v.getconstelem(k) ) && ! isPivot[k] ) {
	    if ( nGreater( v.getconstelem( k ), pivot ) ) {
		// ACHTUNG hier nCopy eingesetzt.
		pivot= v.getconstelem( k );
		pivotcol= k;
	    }
	}
	k++;
    }
#ifdef FGLM_IOSTREAM
    if ( nIsZero( pivot ) ) cout << " Error(2) GF::Pivotelement ist Null" << endl; cout.flush();
#endif
    isPivot[ pivotcol ]= TRUE;
    perm[basisSize]= pivotcol;
    
    // das hier ist noch nicht so gut:
    pivot= nCopy( v.getconstelem( pivotcol ) );
    gauss[basisSize].gaussElem( v, p, denom, pivot );
}

void
GF::updateCandidates( poly m, const fglmVector v )
{
    ListIterator<fglmElem> list = nlist;
    poly newmonom = NULL;
    int k = pVariables;
    BOOLEAN done = FALSE;
    int state = 0;
    while ( k >= 1 ) {
	newmonom = pCopy( m );
	pGetExp( newmonom, k )++;
	pSetm( newmonom );
	done= FALSE;
	while ( list.hasItem() && (done == FALSE) ) {
	    if ( (state= pComp( list.getItem().getMonom(), newmonom )) < 0 )
		list++;
	    else done= TRUE;
	}
	if ( done == FALSE ) {
	    nlist.append( fglmElem( newmonom, v, k ) );
	    break;
	}
	if ( state == 0 ) {
	    list.getItem().newDivisor();
	    pDelete1( & newmonom );
	}
	else {
	    list.insert( fglmElem( newmonom, v, k ) );
	    // list++ ausgebaut: nach list.insert zeigt list immer noch auf
	    // das gleiche Element, das jetzt aber einen Platz weiter hinten 
	    // steht. (siehe Buch Seite 56 und FP::updateCandidates()
//	    list++;
	}
	k--;
    }
    while ( --k >= 1 ) {
	newmonom= pCopy( m );
	pGetExp( newmonom, k )++;
	pSetm( newmonom );
	nlist.append( fglmElem( newmonom, v, k ) );
    }
#ifdef  FGLM_TEST
    if ( nlist.length() >= 2 ) {
	list= nlist;
	ListIterator<fglmElem> next= nlist;
	next++;
	while ( next.hasItem() ) {
	    if ( pComp( list.getItem().getMonom(), next.getItem().getMonom() ) > 0 ) {
		Print( "ERROR: GF::nlist ist nicht sortiert (basisSize=%i)", basisSize );
	    }
	    list++; next++;
	}
	list= nlist;
	while ( list.hasItem() ) {
	    next= list;
	    next++;
	    while ( next.hasItem() ) {
		if ( pComp( list.getItem().getMonom(), next.getItem().getMonom() ) == 0 ) {
		    Print( "ERROR: GF::nlist das Monom %s existiert doppelt (basisSize=%i)", pString( list.getItem().getMonom() ), basisSize );
		}
		next++;
	    }
	    list++;
	}
    }
#endif
}

void 
GF::newGroebnerPoly( fglmVector & p, poly & m ) 
// Inserts gp = p[1]*basis(1)+..+p[basisSize]*basis(basisSize)+p[basisSize+1]*m as
//  a new groebner polynomial for the ideal.
// All elements (monomials and coefficients) of gp are copied.
// Assumes that p.length() == basisSize+1.
{
    //. Baue das Polynom von oben nach unten:
#ifdef FGLM_IOSTREAM
    ASSERT( p.size() == basisSize+1, "GP::newGroebnerPoly: p has wrong size" );
#endif
    poly result = m;
    m= NULL;
    // p auf Normalform bringen, d.h. im Fall von Characteristic p:
    //   p( basisSize+1 ) = 1
    // im Fall von Characteristic 0: 
    //   p = p / ggt(p)
    // Achtung: Bislang nur fuer Characteristic p!!!
//     number temp = nCopy( p.getconstelem( basisSize+1 ) );
//     p /= temp; 
    number ggt= p.gcd();
    if ( ! nIsZero( ggt ) && ! nIsOne( ggt ) ) 
	p /= ggt;
    nDelete( & ggt );
    pSetCoeff( result, nCopy( p.getconstelem( basisSize+1 ) ) );
    for ( int k = basisSize; k > 0; k-- ) {
	if ( ! nIsZero( p.getconstelem( k ) ) ) {
	    poly temp = pCopy( basis[k] );
	    pSetCoeff( temp, nCopy( p.getconstelem( k ) ) );
	    result= pAdd( result, temp );
	}
    }
    if ( groebnerSize == groebnerMax ) {
#ifdef FGLM_PROT
	Print( "(gr+)" );
#endif
	groebner= (poly *)ReAlloc( groebner, groebnerMax*sizeof( poly ), (groebnerMax + groebnerBS)*sizeof( poly ) );
	groebnerMax+= groebnerBS;
    }
    groebner[groebnerSize]= result;
    groebnerSize++;
}

void
GF::gaussreduce( fglmVector & v, fglmVector & p, number & pdenom ) 
{
// IDEE: Die Aenderungen an p erst ausfuehren, wenn nicht Null heraus-
// gekommen ist!? Bringt das was?
    int k;
    number fac1, fac2;
    number temp;
    ASSERT( pdenom == NULL, "pdenom in gaussreduce should be NULL" );
    pdenom= nInit( 1 );
    number vdenom = v.clearDenom();
    if ( ! nIsOne( vdenom ) && ! nIsZero( vdenom ) ) {
	p.setelem( p.size(), vdenom );
    }
    else {
	nDelete( &vdenom );
    }
    // ggt aus v rausziehen:
    number gcd = v.gcd();
    if ( ! nIsOne( gcd ) && ! nIsZero( gcd ) ) {
	v /= gcd;
	number temp= nMult( pdenom, gcd );
	nDelete( &pdenom );
	pdenom= temp;
    }
    nDelete( & gcd );

    for ( k= 1; k <= basisSize; k++ ) {
	fac2= nCopy( v.getconstelem( perm[k] ) );
	if ( ! nIsZero( fac2 ) ) {
#ifdef FGLM_GAUSSPROT
	    Print( "x" );
#endif
	    fac1= nCopy( gauss[k].fac );
	    v= fac1 * v - fac2 * gauss[k].v;
	    temp= nMult( fac1, gauss[k].pdenom );
	    nDelete( &fac1 );
	    fac1= temp;
	    temp= nMult( fac2, pdenom );
	    nDelete( &fac2 );
	    fac2= temp;
	    int s = gauss[k].p.size();
	    p *= fac1;
	    p(1,s) = p(1,s) - fac2 * gauss[k].p;
	    temp= nMult( pdenom, gauss[k].pdenom );
	    nDelete( &pdenom );
	    pdenom= temp;

	    nDelete( & fac1 );
	    nDelete( & fac2 );
	    // ggt aus v rausziehen:
	    number gcd = v.gcd();
	    if ( ! nIsOne( gcd ) && ! nIsZero( gcd ) ) {
		v /= gcd;
		number temp= nMult( pdenom, gcd );
		nDelete( &pdenom );
		pdenom= temp;
	    }
	    nDelete( & gcd );
	    // p kuerzen:
	    gcd= p.gcd();
	    temp= nGcd( pdenom, gcd );
	    nDelete( &gcd );
	    gcd= temp;
	    if ( ! nIsZero( gcd ) && ! nIsOne( gcd ) ) {
		p /= gcd;
		temp= nDiv( pdenom, gcd );
		nDelete( & pdenom );
		pdenom= temp;
		nNormalize( pdenom );
	    }
	    nDelete( & gcd );
	}
	else { 
#ifdef FGLM_GAUSSPROT
	    Print( "_" );
#endif
	    nDelete( & fac2 );
	}
    }
// #ifdef FGLM_GAUSS_IO
//     cout << endl << "(ende gauss-reduce)" << endl; cout.flush();
// #endif
}

ideal
GF::buildIdeal() const 
{
    ideal result = idInit( groebnerSize, 1 );
    for ( int k = 0; k < groebnerSize; k++ ) 
	result->m[k]= groebner[k];
    return result;
}
//<-

//-> ideal GroebnerViaFunctionals( const idealFunctionals & l )
ideal
GroebnerViaFunctionals( const idealFunctionals & l )
// Calculates the groebnerBasis for the ideal which is defined by l.
// The dimension of l has to be finite.
// The result is in reduced form. 
{
    GF data( l.dimen() );
    
    // insert pOne() and update workinglist:
    poly one = pOne();
    data.updateCandidates( one, fglmVector(l.dimen(), 1) );
    number nOne = nInit( 1 );
    data.newBasisElem( one, fglmVector( l.dimen(), 1 ), fglmVector( 1, 1 ), nOne );
    ASSERT( one == NULL , "warum ist one nicht NULL" );
#ifdef FGLM_PROT
    Print( "." );
#else
    if (BTEST1(OPT_PROT)) Print(".");
#endif
    while ( data.candidatesLeft() == TRUE ) {
	fglmElem candidate = data.nextCandidate();
	if ( candidate.isBasisOrEdge() == TRUE ) {
	    // Now we have the chance to find a new groebner polynomial
	    
	    // v is the vector-representation of candidate.getMonom()
	    // some elements of v are zeroed in data.gaussreduce(). Which
	    // ones and how this was done is stored in p.
	    // originalV containes the unchanged v, which is later inserted
	    // into the working list (via data.updateCandidates().
#ifdef FGLM_PROT
	    Print( "[" );
#endif
	    fglmVector v = l.multiply( candidate.getVector(), candidate.getVar() );
#ifdef FGLM_PROT 
	    Print( "]" );
#endif
	    fglmVector originalV = v;
	    fglmVector p( data.getBasisSize()+1, data.getBasisSize()+1 );
#ifdef FGLM_DEBUG
	    v.test();
	    p.test();
#endif

	    number pdenom = NULL;
#ifdef FGLM_PROT
	    Print( "(" );
#endif
	    data.gaussreduce( v, p, pdenom );
#ifdef FGLM_PROT
	    Print( ")" );
#endif
#ifdef FGLM_DEBUG
	    v.test();
	    p.test();
#endif
	    if ( v.isZero() ) {
		// Now v is linear dependend to the already found basis elements.
		// This means that v (rsp. candidate.getMonom()) is the leading
		// monomial of the next groebner-basis polynomial.
		data.newGroebnerPoly( p, candidate.getMonom() );
		nDelete( & pdenom );
#ifdef FGLM_PROT
		Print( "*" );
#else
		if (BTEST1(OPT_PROT)) Print("*");
#endif
	    }
	    else {
		// no linear dependence could be found, so v ( rsp. getMonom() )
		// is a basis monomial. We store the zeroed version ( i.e. v
		// and not originalV ) as well as p, the denomiator and all
		// the other stuff.
		// erst updateCandidates, dann newBasisELem!!!
#ifdef FGLM_PROT
	    Print( "<" );
#endif
		data.updateCandidates( candidate.getMonom(), originalV );
#ifdef FGLM_PROT
	    Print( ">" );
#endif
		data.newBasisElem( candidate.getMonom(), v, p, pdenom );
#ifdef FGLM_PROT
		Print( "." );
#else
		if (BTEST1(OPT_PROT)) Print(".");
#endif
	    }
	}
	else {
#ifdef FGLM_PROT
	    Print( "-" );
#else
	    if (BTEST1(OPT_PROT)) Print ( "-" );
#endif
	    candidate.cleanup();
	}
    }  //. while data.candidatesLeft()
#ifdef FGLM_PROT
    Print( "\n" );
    l.statistic();
#else
    if ( BTEST1( OPT_PROT ) ) Print( "\n" );
#endif
    return ( data.buildIdeal() );
}
//<-

// ============================================================
//!      the surrounding procedure
// ============================================================

//-> enum FglmState
//     enumeration to handle the various errors to occour.
enum FglmState{ 
    FglmOk, 
    FglmHasOne, 
    FglmNoIdeal,
    FglmNotReduced,
    FglmNotZeroDim, 
    FglmIncompatibleRings
};
//<-

//-> FglmState CheckIdealForFglm( const ideal & theIdeal )
//     Checks if the ideal "theIdeal" is 0-dimensional and reduced.
//     Returns FglmOk, FglmNotReduced or FglmNotZeroDim.
FglmState 
CheckIdealForFglm( const ideal & theIdeal )
{
    FglmState state = FglmOk;
    int power;
    int k;  // HIER DAS WAR WEG
    BOOLEAN * purePowers = (BOOLEAN *)Alloc( pVariables*sizeof( BOOLEAN ) );
    // HIER STAND int k
    // Ausserdem faelschlicherweise k = IDELEMS( theIdeal )
    for ( k = pVariables-1; k >= 0; k-- ) 
	purePowers[k]= FALSE;

    // HIER STAND int k
    for ( k = IDELEMS( theIdeal ) - 1; state == FglmOk && k >= 0; k-- ) {
	poly p = (theIdeal->m)[k];
	if ( pIsConstant( p ) ) state= FglmHasOne;
	else if ( (power= pIsPurePower( p )) > 0 ) {
	    ASSERT( 0 < power && power <= pVariables, "illegal power" );
	    if ( purePowers[power-1] == TRUE  ) state= FglmNotReduced;
	    else purePowers[power-1]= TRUE; 
	}
	for ( int l = IDELEMS( theIdeal ) - 1; state == FglmOk && l >= 0; l-- ) 
	    if ( (k != l) && pDivisibleBy( p, (theIdeal->m)[l] ) )
		state= FglmNotReduced;
    }
    if ( state == FglmOk ) {
	for ( k = pVariables-1 ; state == FglmOk && k >= 0; k-- ) 
	    if ( purePowers[k] == FALSE ) state= FglmNotZeroDim;
    }
    Free( (ADDRESS)purePowers, pVariables*sizeof( BOOLEAN ) );
    return state;
}
//<-

//-> ideal fglmProc( leftv first, leftv second )
//     the main function for the fglm-Algorithm. 
//     Checks the input-data, calls CalculateFunctionals, handles change
//     of ring-vars and finaly calls GroebnerViaFunctionals.
//     returns the new groebnerbasis or 0 if an error occoured.
ideal
fglmProc( leftv first, leftv second ) 
{
    FglmState state = FglmOk;
    //. array for the permutations of vars in both rings:
    //. counts from perm[1]..perm[pvariables]
    int * perm = (int *)Alloc0( (pVariables+1)*sizeof( int ) );
    // Warning: die 100 darf hier natuerlich nicht stehenblieben!
    idealFunctionals L( 100, pVariables );
    idhdl destRingHdl = currRingHdl;
    ring destRing = currRing;
    //. Switch to source ring:
    rSetHdl( (idhdl)first->data, TRUE );
    //. check if rings are compatible:
    if ( currRing->N == destRing->N ) {
	maFindPerm( currRing->names, pVariables, NULL, destRing->names, pVariables, NULL, perm, NULL );
	for ( int k = 1; state == FglmOk && k <= pVariables; k++ ) 
	    if ( perm[k] <= 0 ) state= FglmIncompatibleRings;
    } else state= FglmIncompatibleRings;
    //. Calculate the defining functionals:
    if ( state == FglmOk ) {
	idhdl ih = currRing->idroot->get( second->name, myynest );
	if ( ih != NULL ) {
	    ideal sourceIdeal = IDIDEAL( ih );
	    state= CheckIdealForFglm( sourceIdeal );
	    if ( state == FglmOk ) { 
		if ( ! hasFlag( ih, FLAG_STD ) ) {
		    Warn("%s is no standardbasis", ih->id );
		}
		CalculateFunctionals( sourceIdeal, L );
	    }
	} else state= FglmNoIdeal;
    }
    // Die Variablenvertauschungen hier berechnen 
    L.reorder( perm );
    Free( (ADDRESS)perm, (pVariables+1)*sizeof(int) );
    //. In den Zielring schalten und die neue Basis ausrechnen:
    rSetHdl( destRingHdl, TRUE );
    ideal destIdeal;
    switch (state) {
	case FglmOk:
	    destIdeal= GroebnerViaFunctionals( L );
	    break;
	case FglmHasOne:
	    destIdeal= idInit(1,1);
	    (destIdeal->m)[0]= pOne();
	    break;
	case FglmIncompatibleRings:
	    Werror( "ring %s and current ring are incompatible", first->name );
	    destIdeal= idInit(0,0);
	    break;
	case FglmNoIdeal:
	    // Fehlermeldung
	    Werror( "Can't find ideal %s in ring %s", second->name, first->name );
	    destIdeal= idInit(0,0);
	    break;
	case FglmNotZeroDim:
	    Werror( "The ideal %s has to be 0-dimensional", second->name );
	    destIdeal= idInit(0,0);
	    break;
	case FglmNotReduced:
	    Werror( "The ideal %s has to be reduced", second->name );
	    destIdeal= idInit(0,0);
	    break;
	default:
#ifdef FGLM_IOSTREAM
	    cout << "(default)" << endl; cout.flush();
#endif
	    destIdeal= idInit(1,1);
    }
    return destIdeal;
}
//<-

// ----------------------------------------------------------------------------
//  Lokale Variablen fuer EMACS
// hier kann man ein folded-file: t einbauen. Dann wird das file aber immer
// gefaltet, wenn man es laedt. Das ist dumm wenn man mit dem Debugger arbeitet.
//
// Local Variables: ***
// compile-command: "make Singular" ***
// page-delimiter: "^\\(\\|//!\\)" ***
// fold-internal-margins: nil ***
// End: ***

#endif

// ----------------------------------------------------------------------------
//  splist.cc
//  begin of file
//  Stephan Endrass, endrass@mathematik.uni-mainz.de
//  23.7.99
// ----------------------------------------------------------------------------

#include "mod2.h"

#ifdef HAVE_SPECTRUM

#ifdef  SPLIST_PRINT
#include <iostream.h>
#ifndef SPLIST_IOSTREAM
#include <stdio.h>
#endif
#endif

#include "tok.h"
#include "GMPrat.h"
#include "numbers.h"
#include "polys.h"
#include "npolygon.h"
#include "splist.h"
#include "intvec.h"
#include "lists.h"

// ------------------------
//  class spectrumPolyNode
// ------------------------

// ----------------------------------------------------------------------------
//  Initialize a  spectrumPolyNode  with zero
// ----------------------------------------------------------------------------

void    spectrumPolyNode::copy_zero( void )
{
    next   = (spectrumPolyNode*)NULL;
    mon    = (poly)NULL;
    weight = (Rational)0;
    nf     = (poly)NULL;
}

// ----------------------------------------------------------------------------
//  Inizialize a  spectrumPolyNode  shallow from data
// ----------------------------------------------------------------------------

void    spectrumPolyNode::copy_shallow(
                spectrumPolyNode *pnode,poly m,const Rational &w,poly f )
{
    next   = pnode;
    mon    = m;
    weight = w;
    nf     = f;
}

// ----------------------------------------------------------------------------
//  Inizialize a  spectrumPolyNode  shallow from another  spectrumPolyNode
// ----------------------------------------------------------------------------

void    spectrumPolyNode::copy_shallow( spectrumPolyNode &node )
{
    next   = node.next;
    mon    = node.mon;
    weight = node.weight;
    nf     = node.nf;
}

// ----------------------------------------------------------------------------
//  Zero constructor for  spectrumPolyNode
// ----------------------------------------------------------------------------

spectrumPolyNode::spectrumPolyNode( )
{
    copy_zero( );
}

// ----------------------------------------------------------------------------
//  Data constructor for  spectrumPolyNode  is shallow
// ----------------------------------------------------------------------------

spectrumPolyNode::spectrumPolyNode(
        spectrumPolyNode *pnode,poly m,const Rational &w,poly f )
{
    copy_shallow( pnode,m,w,f );
}

// ----------------------------------------------------------------------------
//  Destructor is empty since we construct our objects shallow
// ----------------------------------------------------------------------------

spectrumPolyNode::~spectrumPolyNode( )
{
    if( mon!=(poly)NULL ) pDelete( &mon );
    if( nf !=(poly)NULL ) pDelete( &nf );
    copy_zero( );
}

// ------------------------
//  class spectrumPolyList
// ------------------------

// ----------------------------------------------------------------------------
//  Initialize a  spectrumPolyList  with zero
// ----------------------------------------------------------------------------

void    spectrumPolyList::copy_zero( void )
{
    root = (spectrumPolyNode*)NULL;
    N    = 0;
    np   = (newtonPolygon*)NULL;
}

// ----------------------------------------------------------------------------
//  Inizialize a  spectrumPolyList  shallow from data
// ----------------------------------------------------------------------------

void    spectrumPolyList::copy_shallow(
                spectrumPolyNode *node,int k,newtonPolygon *npolygon )
{
    root = node;
    N    = k;
    np   = npolygon;
}

// ----------------------------------------------------------------------------
//  Inizialize a  spectrumPolyList  shallow from another  spectrumPolyList
// ----------------------------------------------------------------------------

void    spectrumPolyList::copy_shallow( spectrumPolyList &splist )
{
    copy_shallow( splist.root,splist.N,splist.np );
}

// ----------------------------------------------------------------------------
//  Zero constructor for  spectrumPolyList
// ----------------------------------------------------------------------------

spectrumPolyList::spectrumPolyList( )
{
    copy_zero( );
}

// ----------------------------------------------------------------------------
//  Data constructor for  spectrumPolyList
// ----------------------------------------------------------------------------

spectrumPolyList::spectrumPolyList( newtonPolygon *npolygon )
{
    copy_shallow( (spectrumPolyNode*)NULL,0,npolygon );
}

// ----------------------------------------------------------------------------
//  Destuctor for  spectrumPolyList
// ----------------------------------------------------------------------------

spectrumPolyList::~spectrumPolyList( )
{
    spectrumPolyNode *node;

    while( root!=(spectrumPolyNode*)NULL )
    {
        node = root->next;
        delete root;
        root = node;
    }

    copy_zero( );
}

// ----------------------------------------------------------------------------
//  Insert a new node into a  spectrumPolyList  at position k
//      If the list is sorted, then
//      the new node ist inserted such that the list remains sorted.
// ----------------------------------------------------------------------------

void    spectrumPolyList::insert_node( poly m,poly f )
{
    #ifdef SPLIST_DEBUG
        if( np==(newtonPolygon*)NULL )
        {
            #ifdef SPLIST_PRINT
            #ifdef SPLIST_IOSTREAM
                cerr << "void    spectrumPolyList::insert_node( poly f )" << endl;
                cerr << "    no Newton polygon" << endl;
                cerr << "    exiting..." << endl;
            #else
                fprintf( stderr,"void    spectrumPolyList::insert_node( poly f )\n" );
                fprintf( stderr,"    no Newton polygon\n" );
                fprintf( stderr,"    exiting...\n" );
            #endif
            #endif

            exit( 1 );
        }
    #endif

    spectrumPolyNode    *newnode = new spectrumPolyNode(
        (spectrumPolyNode*)NULL,m,np->weight_shift( m ),f );

    if( N==0 ||
              root->weight>newnode->weight ||
            ( root->weight==newnode->weight &&
              pCmp( root->mon,newnode->mon )<0 ) )
    {
        // ----------------------
        //  insert at position 0
        // ----------------------

        newnode->next = root;
        root          = newnode;
    }
    else if( N==1 )
    {
        // ---------------
        //  insert at end
        // ---------------

        root->next    = newnode;
    }
    else
    {
        // ----------------------------
        //  insert according to weight
        // ----------------------------

        spectrumPolyNode *actual = root;
        spectrumPolyNode *next   = root->next;

        while( next!=(spectrumPolyNode*)NULL &&
               ( newnode->weight>next->weight ||
               ( newnode->weight==next->weight &&
                 pCmp( newnode->mon,next->mon )<0 ) ) )
        {
            actual = next;
            next   = next->next;
        }

        actual->next = newnode;
        newnode->next = next;
    }
    N++;
}

// ----------------------------------------------------------------------------
//  Delete a node from a  spectrumPolyList
// ----------------------------------------------------------------------------

void    spectrumPolyList::delete_node( spectrumPolyNode **node )
{
    spectrumPolyNode *foo = *node;
    *node = (*node)->next;
    delete foo;
    N--;
}

// ----------------------------------------------------------------------------
//  Delete all nodes where   node->mon  is a multiple of  m
//  In every node delete all instances of  m  in  node->nf
// ----------------------------------------------------------------------------

void    spectrumPolyList::delete_monomial( poly m )
{
    spectrumPolyNode **node = &root;
    poly              *f;

    m = pCopy( m );

    while( *node!=(spectrumPolyNode*)NULL )
    {
        if( pCmp( m,(*node)->mon )>=0 && pLmDivisibleByNoComp( m,(*node)->mon ) )
        {
            delete_node( node );
        }
        else if( (*node)->nf!=(poly)NULL )
        {
            f = &((*node)->nf);

            while( *f!=(poly)NULL )
            {
                if( pCmp( m,*f )>=0 && pLmDivisibleByNoComp( m,*f ) )
                {
                    pDeleteLm(f);
                }
                else
                    {
                    f = &(pNext( *f ));
                }
            }

            if( (*node)->nf==(poly)NULL )
            {
                delete_node( node );
            }
            else
            {
                node = &((*node)->next);
            }
        }
        else
        {
            node = &((*node)->next);
        }
    }
    pDelete( &m );
}

// ----------------------------------------------------------------------------
//  Compute the spectrum of a  spectrumPolyList
// ----------------------------------------------------------------------------

spectrumState   spectrumPolyList::spectrum( lists *L,int fast )
{
    spectrumPolyNode  **node = &root;
    spectrumPolyNode  *search;

    poly              f,tmp;
    int               found,cmp;

    Rational smax( ( fast==0 ? 0 : pVariables ),
                   ( fast==2 ? 2 : 1 ) );

    Rational weight_prev( 0,1 );

    int     mu = 0;          // the milnor number
    int     pg = 0;          // the geometrical genus
    int     n  = 0;          // number of different spectral numbers
    int     z  = 0;          // number of spectral number equal to smax

    int     k = 0;

    while( (*node)!=(spectrumPolyNode*)NULL &&
           ( fast==0 || (*node)->weight<=smax ) )
    {
        // ---------------------------------------
        //  determine the first normal form which
        //  contains the monomial  node->mon
        // ---------------------------------------

        found  = FALSE;
        search = *node;

        while( search!=(spectrumPolyNode*)NULL && found==FALSE )
        {
            if( search->nf!=(poly)NULL )
            {
                f = search->nf;

                do
                {
                    // --------------------------------
                    //  look for  (*node)->mon  in   f
                    // --------------------------------

                    cmp = pCmp( (*node)->mon,f );

                    if( cmp<0 )
                    {
                        f = pNext( f );
                    }
                    else if( cmp==0 )
                    {
                        // -----------------------------
                        //  we have found a normal form
                        // -----------------------------

                        found = TRUE;

                        //  normalize coefficient

                        number inv = nInvers( pGetCoeff( f ) );
                        pMult_nn( search->nf,inv );
                        nDelete( &inv );

                        //  exchange  normal forms

                        tmp         = (*node)->nf;
                        (*node)->nf = search->nf;
                        search->nf  = tmp;
                    }
                }
                while( cmp<0 && f!=(poly)NULL );
            }
            search = search->next;
        }

        if( found==FALSE )
        {
            // ------------------------------------------------
            //  the weight of  node->mon  is a spectrum number
            // ------------------------------------------------

            mu++;

            if( (*node)->weight<=(Rational)1 )              pg++;
            if( (*node)->weight==smax )           z++;
            if( (*node)->weight>weight_prev )     n++;

            weight_prev = (*node)->weight;
            node = &((*node)->next);
        }
        else
        {
            // -----------------------------------------------
            //  determine all other normal form which contain
            //  the monomial  node->mon
            //  replace for  node->mon  its normal form
            // -----------------------------------------------

            while( search!=(spectrumPolyNode*)NULL )
            {
                    if( search->nf!=(poly)NULL )
                {
                    f = search->nf;

                    do
                    {
                        // --------------------------------
                        //  look for  (*node)->mon  in   f
                        // --------------------------------

                        cmp = pCmp( (*node)->mon,f );

                        if( cmp<0 )
                        {
                            f = pNext( f );
                        }
                        else if( cmp==0 )
                        {
                            search->nf = pSub( search->nf,
                                ppMult_nn( (*node)->nf,pGetCoeff( f ) ) );
                            pNorm( search->nf );
                        }
                    }
                    while( cmp<0 && f!=(poly)NULL );
                }
                search = search->next;
            }
            delete_node( node );
        }

    }

    // --------------------------------------------------------
    //  fast computation exploits the symmetry of the spectrum
    // --------------------------------------------------------

    if( fast==2 )
    {
        mu = 2*mu - z;
        n  = ( z > 0 ? 2*n - 1 : 2*n );
    }

    // --------------------------------------------------------
    //  compute the spectrum numbers with their multiplicities
    // --------------------------------------------------------

    intvec            *nom  = new intvec( n );
    intvec            *den  = new intvec( n );
    intvec            *mult = new intvec( n );

    int count         = 0;
    int multiplicity  = 1;

    for( search=root; search!=(spectrumPolyNode*)NULL &&
                     ( fast==0 || search->weight<=smax );
                     search=search->next )
    {
        if( search->next==(spectrumPolyNode*)NULL ||
            search->weight<search->next->weight )
        {
            (*nom) [count] = search->weight.get_num_si( );
            (*den) [count] = search->weight.get_den_si( );
            (*mult)[count] = multiplicity;

            multiplicity=1;
            count++;
        }
        else
        {
            multiplicity++;
        }
    }

    // --------------------------------------------------------
    //  fast computation exploits the symmetry of the spectrum
    // --------------------------------------------------------

    if( fast==2 )
    {
        int n1,n2;
        for( n1=0, n2=n-1; n1<n2; n1++, n2-- )
        {
            (*nom) [n2] = pVariables*(*den)[n1]-(*nom)[n1];
            (*den) [n2] = (*den)[n1];
            (*mult)[n2] = (*mult)[n1];
        }
    }

    // -----------------------------------
    //  test if the spectrum is symmetric
    // -----------------------------------

    if( fast==0 || fast==1 )
    {
        int symmetric=TRUE;

        for( int n1=0, n2=n-1 ; n1<n2 && symmetric==TRUE; n1++, n2-- )
        {
            if( (*mult)[n1]!=(*mult)[n2] ||
                (*den) [n1]!= (*den)[n2] ||
                (*nom)[n1]+(*nom)[n2]!=pVariables*(*den) [n1] )
            {
                symmetric = FALSE;
            }
        }

        if( symmetric==FALSE )
        {
            // ---------------------------------------------
            //  the spectrum is not symmetric => degenerate
            //  principal part
            // ---------------------------------------------

            *L = (lists)omAllocBin( slists_bin);
            (*L)->Init( 1 );
            (*L)->m[0].rtyp = INT_CMD;    //  milnor number
            (*L)->m[0].data = (void*)mu;

            return spectrumDegenerate;
        }
    }

    *L = (lists)omAllocBin( slists_bin);

    (*L)->Init( 6 );

    (*L)->m[0].rtyp = INT_CMD;    //  milnor number
    (*L)->m[1].rtyp = INT_CMD;    //  geometrical genus
    (*L)->m[2].rtyp = INT_CMD;    //  number of spectrum values
    (*L)->m[3].rtyp = INTVEC_CMD; //  nominators
    (*L)->m[4].rtyp = INTVEC_CMD; //  denomiantors
    (*L)->m[5].rtyp = INTVEC_CMD; //  multiplicities

    (*L)->m[0].data = (void*)mu;
    (*L)->m[1].data = (void*)pg;
    (*L)->m[2].data = (void*)n;
    (*L)->m[3].data = (void*)nom;
    (*L)->m[4].data = (void*)den;
    (*L)->m[5].data = (void*)mult;

    return  spectrumOK;
}

// ----------------------------------------------------------------------------
//  Print a  spectrumPolyList
// ----------------------------------------------------------------------------

#ifdef SPLIST_PRINT
ostream & operator << ( ostream &s,const spectrumPolyList &l )
{
    #ifdef SPLIST_IOSTREAM
        s << "elements: " << l.N << endl;
        s << "{";
    #else
        fprintf( stdout,"elements: %d\n",l.N );
        fprintf( stdout,"{" );
    #endif

    if( l.root!=(spectrumPolyNode*)NULL )
    {
        #ifdef SPLIST_IOSTREAM
            s << "(";
            pWrite0( l.root->mon );
            s << "=>";
            pWrite0( l.root->nf );
            cout << "," << l.root->weight << ")" << endl;
        #else
            fprintf( stdout,"(" );
            pWrite0( l.root->mon );
            fprintf( stdout,"=>" );
            pWrite0( l.root->nf );
            fprintf( stdout,"," );
            cout << l.root->weight;
            fprintf( stdout,")\n" );
        #endif

        spectrumPolyNode *node = l.root->next;

        while( node!=(spectrumPolyNode*)NULL )
        {
            #ifdef SPLIST_IOSTREAM
                s << ",(";
                pWrite0( node->mon );
                s << "=>";
                pWrite0( node->nf );
                cout << "," << node->weight << ")" << endl;
            #else
                fprintf( stdout,",(" );
                pWrite0( node->mon );
                fprintf( stdout,"=>" );
                pWrite0( node->nf );
                fprintf( stdout,"," );
                cout << node->weight;
                fprintf( stdout,")\n" );
            #endif

            node = node->next;
        }
    }
    #ifdef SPLIST_IOSTREAM
        s << "}";
    #else
        fprintf( stdout,"}" );
    #endif

    return  s;
}
#endif

#endif /* HAVE_SPECTRUM */
// ----------------------------------------------------------------------------
//  splist.cc
//  end of file
// ----------------------------------------------------------------------------


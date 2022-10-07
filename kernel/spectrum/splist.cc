// ----------------------------------------------------------------------------
//  splist.cc
//  begin of file
//  Stephan Endrass, endrass@mathematik.uni-mainz.de
//  23.7.99
// ----------------------------------------------------------------------------

#define SPLIST_CC




#include "kernel/mod2.h"

#ifdef HAVE_SPECTRUM

#ifdef  SPLIST_PRINT
#include <iostream.h>
#ifndef SPLIST_IOSTREAM
#include <stdio.h>
#endif
#endif

#include "kernel/structs.h"
#include "kernel/spectrum/GMPrat.h"
#include "coeffs/numbers.h"
#include "polys/monomials/p_polys.h"
#include "kernel/spectrum/npolygon.h"
#include "kernel/spectrum/splist.h"
#include "misc/intvec.h"

// ------------------------
//  class spectrumPolyNode
// ------------------------

// ----------------------------------------------------------------------------
//  Initialize a  spectrumPolyNode  with zero
// ----------------------------------------------------------------------------

void    spectrumPolyNode::copy_zero( void )
{
    next   = (spectrumPolyNode*)NULL;
    mon    = NULL;
    weight = (Rational)0;
    nf     = NULL;
    r      = NULL;
}

// ----------------------------------------------------------------------------
//  Initialize a  spectrumPolyNode  shallow from data
// ----------------------------------------------------------------------------

void    spectrumPolyNode::copy_shallow(
         spectrumPolyNode *pnode,poly m,const Rational &w,poly f, const ring R )
{
    next   = pnode;
    mon    = m;
    weight = w;
    nf     = f;
    r      = R;
}

// ----------------------------------------------------------------------------
//  Initialize a  spectrumPolyNode  shallow from another  spectrumPolyNode
// ----------------------------------------------------------------------------

void    spectrumPolyNode::copy_shallow( spectrumPolyNode &node )
{
    next   = node.next;
    mon    = node.mon;
    weight = node.weight;
    nf     = node.nf;
    r      = node.r;
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
        spectrumPolyNode *pnode,poly m,const Rational &w,poly f, const ring R )
{
    copy_shallow( pnode,m,w,f,R );
}

// ----------------------------------------------------------------------------
//  Destructor is empty since we construct our objects shallow
// ----------------------------------------------------------------------------

spectrumPolyNode::~spectrumPolyNode()
{
    if( mon!=NULL ) p_Delete( &mon, r );
    if( nf !=NULL ) p_Delete( &nf,r );
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
//  Initialize a  spectrumPolyList  shallow from data
// ----------------------------------------------------------------------------

void    spectrumPolyList::copy_shallow(
                spectrumPolyNode *node,int k,newtonPolygon *npolygon )
{
    root = node;
    N    = k;
    np   = npolygon;
}

// ----------------------------------------------------------------------------
//  Initialize a  spectrumPolyList  shallow from another  spectrumPolyList
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
//  Destructor for  spectrumPolyList
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
//      the new node is inserted such that the list remains sorted.
// ----------------------------------------------------------------------------

void    spectrumPolyList::insert_node( poly m,poly f, const ring R )
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
        (spectrumPolyNode*)NULL,m,np->weight_shift( m,R ),f, R );

    if( N==0 ||
              root->weight>newnode->weight ||
            ( root->weight==newnode->weight &&
              p_Cmp( root->mon,newnode->mon,R )<0 ) )
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
                 p_Cmp( newnode->mon,next->mon, R )<0 ) ) )
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

void    spectrumPolyList::delete_monomial( poly m, const ring R )
{
    spectrumPolyNode **node = &root;
    poly              *f;

    m = p_Copy( m,R );

    while( *node!=(spectrumPolyNode*)NULL )
    {
        if( p_Cmp( m,(*node)->mon,R )>=0 && p_LmDivisibleByNoComp( m,(*node)->mon, R ))
        {
            delete_node( node );
        }
        else if( (*node)->nf!=NULL )
        {
            f = &((*node)->nf);

            while( *f!=NULL )
            {
                if( p_Cmp( m,*f,R )>=0 && p_LmDivisibleByNoComp( m,*f,R ) )
                {
                    p_LmDelete(f,R);
                }
                else
                {
                    f = &(pNext( *f ));
                }
            }

            if( (*node)->nf==NULL )
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
    p_Delete( &m,R );
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


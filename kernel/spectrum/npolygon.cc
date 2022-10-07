// ----------------------------------------------------------------------------
//  npolygon.cc
//  begin of file
//  Stephan Endrass, endrass@mathematik.uni-mainz.de
//  23.7.99
// ----------------------------------------------------------------------------

#define  NPOLYGON_CC




#include "kernel/mod2.h"

#ifdef HAVE_SPECTRUM

#ifdef   NPOLYGON_PRINT
#include <iostream.h>
#ifndef   NPOLYGON_IOSTREAM
#include <stdio.h>
#endif
#endif

#include "polys/monomials/p_polys.h"
#include "polys/monomials/ring.h"

#include "kernel/spectrum/GMPrat.h"
#include "kernel/spectrum/kmatrix.h"
#include "kernel/spectrum/npolygon.h"
#include "kernel/structs.h"

// ----------------------------------------------------------------------------
//  Allocate memory for a linear form of  k  terms
// ----------------------------------------------------------------------------

void    linearForm::copy_new( int k )
{
  if( k > 0 )
  {
    c = new Rational[k];

    #ifndef NBDEBUG
    if( c == (Rational*)NULL )
    {
      #ifdef  NPOLYGON_PRINT
      #ifdef  NPOLYGON_IOSTREAM
        cerr <<
        "void linearForm::copy_new( int k ): no memory left ...\n" ;
      #else
        fprintf( stderr,
        "void linearForm::copy_new( int k ): no memory left ...\n");
      #endif
      #endif
      HALT();
    }
    #endif
  }
  else if( k == 0 )
  {
    c = (Rational*)NULL;
  }
  else if( k < 0 )
  {
    #ifdef  NPOLYGON_PRINT
    #ifdef  NPOLYGON_IOSTREAM
      cerr <<
      "void linearForm::copy_new( int k ): k < 0 ...\n";
    #else
      fprintf( stderr,
      "void linearForm::copy_new( int k ): k < 0 ...\n" );
    #endif
    #endif

    HALT();
  }
}

// ----------------------------------------------------------------------------
//  Delete the memory of a linear form
// ----------------------------------------------------------------------------

void    linearForm::copy_delete( void )
{
  if( c != (Rational*)NULL && N > 0 )
    delete [] c;
  copy_zero( );
}

// ----------------------------------------------------------------------------
//  Initialize deep from another linear form
// ----------------------------------------------------------------------------

void    linearForm::copy_deep( const linearForm &l )
{
  copy_new( l.N );
  for( int i=l.N-1; i>=0; i-- )
  {
    c[i] = l.c[i];
  }
  N = l.N;
}

// ----------------------------------------------------------------------------
//  Copy constructor
// ----------------------------------------------------------------------------

linearForm::linearForm( const linearForm &l )
{
  copy_deep( l );
}

// ----------------------------------------------------------------------------
//  Destructor
// ----------------------------------------------------------------------------

linearForm::~linearForm( )
{
  copy_delete( );
}

// ----------------------------------------------------------------------------
//  Define `=` to be a deep copy
// ----------------------------------------------------------------------------

linearForm & linearForm::operator = ( const linearForm &l )
{
  copy_delete( );
  copy_deep( l );

  return *this;
}

// ----------------------------------------------------------------------------
//  ostream for linear form
// ----------------------------------------------------------------------------

#ifdef  NPOLYGON_PRINT

ostream & operator << ( ostream &s,const linearForm &l )
{
  for( int i=0; i<l.N; i++ )
  {
    if( l.c[i] != (Rational)0 )
    {
      if( i > 0 && l.c[i] >= (Rational)0 )
      {
        #ifdef NPOLYGON_IOSTREAM
          s << "+";
        #else
          fprintf( stdout,"+" );
        #endif
      }

      s << l.c[i];

      #ifdef NPOLYGON_IOSTREAM
        s << "*x" << i+1;
      #else
        fprintf( stdout,"*x%d",i+1 );
      #endif
    }
  }
  return s;
}

#endif

// ----------------------------------------------------------------------------
//  Equality of linear forms
// ----------------------------------------------------------------------------

int     operator == ( const linearForm &l1,const linearForm &l2 )
{
  if( l1.N!=l2.N )
    return  FALSE;
  for( int i=l1.N-1; i >=0 ; i-- )
  {
    if( l1.c[i]!=l2.c[i] )
      return  FALSE;
  }
  return  TRUE;
}


// ----------------------------------------------------------------------------
//  Newton weight of a monomial
// ----------------------------------------------------------------------------

Rational linearForm::weight( poly m, const ring r ) const
{
  Rational ret=(Rational)0;

  for( int i=0,j=1; i<N; i++,j++ )
  {
    ret += c[i]*(Rational)p_GetExp( m,j,r );
  }

  return ret;
}

// ----------------------------------------------------------------------------
//  Newton weight of a polynomial
// ----------------------------------------------------------------------------

Rational linearForm::pweight( poly m, const ring r ) const
{
  if( m==(poly)NULL )
    return  (Rational)0;

  Rational    ret = weight( m, r );
  Rational    tmp;

  for( m=pNext(m); m!=(poly)NULL; pIter(m) )
  {
    tmp = weight( m, r );
    if( tmp<ret )
    {
      ret = tmp;
    }
  }

  return ret;
}

// ----------------------------------------------------------------------------
//  Newton weight of a monomial shifted by the product of the variables
// ----------------------------------------------------------------------------

Rational linearForm::weight_shift( poly m, const ring r ) const
{
  Rational ret=(Rational)0;

  for( int i=0,j=1; i<N; i++,j++ )
  {
    ret += c[i]*(Rational)( p_GetExp( m,j,r ) + 1 );
  }

  return ret;
}

// ----------------------------------------------------------------------------
//  Newton weight of a monomial (forgetting the first variable)
// ----------------------------------------------------------------------------

Rational linearForm::weight1( poly m, const ring r ) const
{
  Rational ret=(Rational)0;

  for( int i=0,j=2; i<N; i++,j++ )
  {
    ret += c[i]*(Rational)p_GetExp( m,j,r );
  }

  return ret;
}

// ----------------------------------------------------------------------------
//  Newton weight of a monomial shifted by the product of the variables
//  (forgetting the first variable)
// ----------------------------------------------------------------------------

Rational linearForm::weight_shift1( poly m, const ring r ) const
{
  Rational ret=(Rational)0;

  for( int i=0,j=2; i<N; i++,j++ )
  {
    ret += c[i]*(Rational)( p_GetExp( m,j,r ) + 1 );
  }

  return ret;
}


// ----------------------------------------------------------------------------
//  Test if all coefficients of a linear form are positive
// ----------------------------------------------------------------------------

int linearForm::positive( void )
{
  for( int i=0; i<N; i++ )
  {
    if( c[i] <= (Rational)0 )
    {
      return FALSE;
    }
  }
  return  TRUE;
}


// ----------------------------------------------------------------------------
//  Allocate memory for a Newton polygon of  k  linear forms
// ----------------------------------------------------------------------------

void    newtonPolygon::copy_new( int k )
{
  if( k > 0 )
  {
    l = new linearForm[k];

    #ifndef SING_NDEBUG
      if( l == (linearForm*)NULL )
      {
        #ifdef  NPOLYGON_PRINT
          #ifdef  NPOLYGON_IOSTREAM
            cerr <<
            "void newtonPolygon::copy_new( int k ): no memory left ...\n";
          #else
            fprintf( stderr,
            "void newtonPolygon::copy_new( int k ): no memory left ...\n" );
          #endif
        #endif

        HALT();
      }
    #endif
  }
  else if( k == 0 )
  {
    l = (linearForm*)NULL;
  }
  else if( k < 0 )
  {
    #ifdef  NPOLYGON_PRINT
      #ifdef  NPOLYGON_IOSTREAM
        cerr << "void newtonPolygon::copy_new( int k ): k < 0 ...\n";
      #else
        fprintf( stderr,
        "void newtonPolygon::copy_new( int k ): k < 0 ...\n" );
      #endif
    #endif

    HALT();
  }
}

// ----------------------------------------------------------------------------
//  Delete the memory of a Newton polygon
// ----------------------------------------------------------------------------

void    newtonPolygon::copy_delete( void )
{
  if( l != (linearForm*)NULL && N > 0 )
    delete [] l;
  copy_zero( );
}

// ----------------------------------------------------------------------------
//  Initialize deep from another Newton polygon
// ----------------------------------------------------------------------------

void    newtonPolygon::copy_deep( const newtonPolygon &np )
{
  copy_new( np.N );
  for( int i=0; i<np.N; i++ )
  {
    l[i] = np.l[i];
  }
  N = np.N;
}

// ----------------------------------------------------------------------------
//  Copy constructor
// ----------------------------------------------------------------------------

newtonPolygon::newtonPolygon( const newtonPolygon &np )
{
  copy_deep( np );
}

// ----------------------------------------------------------------------------
//  Destructor
// ----------------------------------------------------------------------------

newtonPolygon::~newtonPolygon( )
{
  copy_delete( );
}

// ----------------------------------------------------------------------------
//  We define `=` to be a deep copy
// ----------------------------------------------------------------------------

newtonPolygon & newtonPolygon::operator = ( const newtonPolygon &np )
{
  copy_delete( );
  copy_deep( np );

  return *this;
}

// ----------------------------------------------------------------------------
//  Initialize a Newton polygon from a polynomial
// ----------------------------------------------------------------------------

newtonPolygon::newtonPolygon( poly f, const ring s )
{
  copy_zero( );

  int *r=new int[s->N];
  poly *m=new poly[s->N];


  KMatrix<Rational> mat(s->N,s->N+1 );

  int i,j,stop=FALSE;
  linearForm sol;

  // ---------------
  //  init counters
  // ---------------

  for( i=0; i<s->N; i++ )
  {
    r[i] = i;
  }

  m[0] = f;

  for( i=1; i<s->N; i++ )
  {
    m[i] = pNext(m[i-1]);
  }

  // -----------------------------
  //  find faces (= linear forms)
  // -----------------------------

  do
  {
    // ---------------------------------------------------
    //  test if monomials p.m[r[0]]m,...,p.m[r[p.vars-1]]
    //  are linearely independent
    // ---------------------------------------------------

    for( i=0; i<s->N; i++ )
    {
      for( j=0; j<s->N; j++ )
      {
        //    mat.set( i,j,pGetExp( m[r[i]],j+1 ) );
        mat.set( i,j,p_GetExp( m[i],j+1,s ) );
      }
      mat.set( i,j,1 );
    }

    if( mat.solve( &(sol.c),&(sol.N) ) == s->N )
    {
      // ---------------------------------
      //  check if linearForm is positive
      //  check if linearForm is extremal
      // ---------------------------------

      if( sol.positive( ) && sol.pweight( f,s ) >= (Rational)1 )
      {
        // ----------------------------------
        //  this is a face or the polyhedron
        // ----------------------------------

        add_linearForm( sol );
        sol.c = (Rational*)NULL;
        sol.N = 0;
      }
    }

    // --------------------
    //  increment counters
    // --------------------

    for( i=1; r[i-1] + 1 == r[i] && i < s->N; i++ );

    for( j=0; j<i-1; j++ )
    {
      r[j]=j;
    }

    if( i>1 )
    {
      m[0]=f;
      for( j=1; j<i-1; j++ )
      {
        m[j]=pNext(m[j-1]);
      }
    }
    r[i-1]++;
    m[i-1]=pNext(m[i-1]);

    if( m[s->N-1] == (poly)NULL )
    {
      stop = TRUE;
    }
  } while( stop == FALSE );
}

#ifdef  NPOLYGON_PRINT

ostream & operator << ( ostream &s,const newtonPolygon &a )
{
  #ifdef NPOLYGON_IOSTREAM
    s << "Newton polygon:" << endl;
  #else
    fprintf( stdout,"Newton polygon:\n" );
  #endif

  for( int i=0; i<a.N; i++ )
  {
    s << a.l[i];

    #ifdef NPOLYGON_IOSTREAM
      s << endl;
    #else
      fprintf( stdout,"\n" );
    #endif
  }

  return s;
}

#endif

// ----------------------------------------------------------------------------
//  Add a face to the newton polygon
// ----------------------------------------------------------------------------

void    newtonPolygon::add_linearForm( const linearForm &l0 )
{
  int           i;
  newtonPolygon np;

  // -------------------------------------
  //  test if linear form is already here
  // -------------------------------------

  for( i=0; i<N; i++ )
  {
    if( l0==l[i] )
    {
      return;
    }
  }

  np.copy_new( N+1 );
  np.N = N+1;

  for( i=0; i<N; i++ )
  {
    np.l[i].copy_shallow( l[i] );
    l[i].copy_zero( );
  }

  np.l[N] = l0;

  copy_delete( );
  copy_shallow( np );
  np.copy_zero( );

  return;
}

// ----------------------------------------------------------------------------
//  Newton weight of a monomial
// ----------------------------------------------------------------------------

Rational  newtonPolygon::weight( poly m, const ring r ) const
{
  Rational ret = l[0].weight( m,r );
  Rational tmp;

  for( int i=1; i<N; i++ )
  {
    tmp = l[i].weight( m,r );

    if( tmp < ret )
    {
      ret = tmp;
    }
  }
  return ret;
}

// ----------------------------------------------------------------------------
//  Newton weight of a monomial shifted by the product of the variables
// ----------------------------------------------------------------------------

Rational  newtonPolygon::weight_shift( poly m, const ring r ) const
{
  Rational ret = l[0].weight_shift( m, r );
  Rational tmp;

  for( int i=1; i<N; i++ )
  {
    tmp = l[i].weight_shift( m, r );

    if( tmp < ret )
    {
      ret = tmp;
    }
  }
  return ret;
}

// ----------------------------------------------------------------------------
//  Newton weight of a monomial (forgetting the first variable)
// ----------------------------------------------------------------------------

Rational  newtonPolygon::weight1( poly m, const ring r ) const
{
  Rational ret = l[0].weight1( m, r );
  Rational tmp;

  for( int i=1; i<N; i++ )
  {
    tmp = l[i].weight1( m, r );

    if( tmp < ret )
    {
      ret = tmp;
    }
  }
  return ret;
}

// ----------------------------------------------------------------------------
//  Newton weight of a monomial shifted by the product of the variables
//  (forgetting the first variable)
// ----------------------------------------------------------------------------

Rational  newtonPolygon::weight_shift1( poly m, const ring r ) const
{
  Rational ret = l[0].weight_shift1( m, r );
  Rational tmp;

  for( int i=1; i<N; i++ )
  {
    tmp = l[i].weight_shift1( m, r );

    if( tmp < ret )
    {
      ret = tmp;
    }
  }
  return ret;
}

/*
// ----------------------------------------------------------------------------
//  Check if the polynomial belonging to the Newton polygon
//  is semiquasihomogeneous (sqh)
// ----------------------------------------------------------------------------

int     newtonPolygon::is_sqh( void ) const
{
  return ( N==1 ? TRUE : FALSE );
}

// ----------------------------------------------------------------------------
//  If the polynomial belonging to the Newton polygon is sqh,
//  return its weights vector
// ----------------------------------------------------------------------------

Rational*   newtonPolygon::sqh_weights( void ) const
{
  return ( N==1 ? l[0].c : (Rational*)NULL );
}

int    newtonPolygon::sqh_N( void ) const
{
  return ( N==1 ? l[0].N : 0 );
}
*/

#endif /* HAVE_SPECTRUM */
// ----------------------------------------------------------------------------
//  npolygon.cc
//  end of file
// ----------------------------------------------------------------------------


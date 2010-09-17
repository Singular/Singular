// ----------------------------------------------------------------------------
//  spectrum.cc
//  begin of file
//  Stephan Endrass, endrass@mathematik.uni-mainz.de
//  23.7.99
// ----------------------------------------------------------------------------

#define SPECTRUM_CC

#include <kernel/mod2.h>

#ifdef HAVE_SPECTRUM

#ifdef  SPECTRUM_PRINT
#include <iostream.h>
#ifndef  SPECTRUM_IOSTREAM
#include <stdio.h>
#endif
#endif

#include <omalloc/mylimits.h>

#include <kernel/numbers.h>
#include <kernel/polys.h>
#include <kernel/ideals.h>
#include <kernel/kstd1.h>
#include <kernel/stairc.h>
#include <kernel/intvec.h>
#include <kernel/ring.h>

#include <kernel/multicnt.h>
#include <kernel/GMPrat.h>
#include <kernel/kmatrix.h>
#include <kernel/npolygon.h>
#include <kernel/splist.h>
#include <kernel/semic.h>

// ----------------------------------------------------------------------------
//  test if the polynomial  h  has a term of total degree d
// ----------------------------------------------------------------------------

BOOLEAN hasTermOfDegree( poly h, int d )
{
  do
  {
    if( pTotaldegree( h )== d )
      return  TRUE;
    pIter(h);
  }
  while( h!=NULL );

  return  FALSE;
}

// ----------------------------------------------------------------------------
//  test if the polynomial  h  has a constant term
// ----------------------------------------------------------------------------

static BOOLEAN inline hasConstTerm( poly h )
{
  return  hasTermOfDegree(h,0);
}

// ----------------------------------------------------------------------------
//  test if the polynomial  h  has a linear term
// ----------------------------------------------------------------------------

static BOOLEAN inline hasLinearTerm( poly h )
{
  return  hasTermOfDegree(h,1);
}

// ----------------------------------------------------------------------------
//  test if the ideal  J  has a lead monomial on the axis number  k
// ----------------------------------------------------------------------------

BOOLEAN hasAxis( ideal J,int k )
{
  int i;

  for( i=0; i<IDELEMS(J); i++ )
  {
    if (pIsPurePower( J->m[i] ) == k) return TRUE;
  }
  return  FALSE;
}

// ----------------------------------------------------------------------------
//  test if the ideal  J  has  1  as a lead monomial
// ----------------------------------------------------------------------------

int     hasOne( ideal J )
{
  int i;

  for( i=0; i<IDELEMS(J); i++ )
  {
    if (pIsConstant(J->m[i])) return TRUE;
  }
  return  FALSE;
}
// ----------------------------------------------------------------------------
//  test if  m  is a multiple of one of the monomials of  f
// ----------------------------------------------------------------------------

int     isMultiple( poly f,poly m )
{
  while( f!=NULL )
  {
    // ---------------------------------------------------
    //  for a local order  f|m  is only possible if  f>=m
    // ---------------------------------------------------

    if( pLmCmp( f,m )>=0 )
    {
      if( pLmDivisibleByNoComp( f,m ) )
      {
        return  TRUE;
      }
      else
      {
        pIter( f );
      }
    }
    else
    {
      return FALSE;
    }
  }

  return  FALSE;
}

// ----------------------------------------------------------------------------
//  compute the minimal monomial of minimmal  weight>=max_weight
// ----------------------------------------------------------------------------

poly    computeWC( const newtonPolygon &np,Rational max_weight )
{
  poly m  = pOne();
  poly wc = NULL;
  int  mdegree;

  for( int i=1; i<=pVariables; i++ )
  {
    mdegree = 1;
    pSetExp( m,i,mdegree );
    // pSetm( m );
    // np.weight_shift does not need pSetm( m ), postpone it

    while(  np.weight_shift( m )<max_weight )
    {
      mdegree++;
      pSetExp( m,i,mdegree );
      // pSetm( m );
      // np.weight_shift does not need pSetm( m ), postpone it
    }
    pSetm( m );

    if( i==1 || pCmp( m,wc )<0 )
    {
      pDelete( &wc );
      wc = pHead( m );
    }

    pSetExp( m,i,0 );
  }

  pDelete( &m );

  return  wc;
}

// ----------------------------------------------------------------------------
//  deletes all monomials of  f  which are less than  hc
// ----------------------------------------------------------------------------

static inline  poly    normalFormHC( poly f,poly hc )
{
  poly    *ptr = &f;

  while( (*ptr)!=NULL )
  {
    if( pLmCmp( *ptr,hc )>=0 )
    {
      ptr = &(pNext( *ptr ));
    }
    else
    {
      pDelete( ptr );
      return  f;
    }
  }

  return f;
}

// ----------------------------------------------------------------------------
//  deletes all monomials of  f  which are multiples of monomials of  Z
// ----------------------------------------------------------------------------

static inline  poly    normalFormZ( poly f,poly Z )
{
  poly    *ptr = &f;

  while( (*ptr)!=NULL )
  {
    if( !isMultiple( Z,*ptr ) )
    {
      ptr = &(pNext( *ptr ));
    }
    else
    {
      pLmDelete(ptr);
    }
  }

  return f;
}

// ?? HS:
// Looks for the shortest polynomial f in stdJ which is divisible
// by the monimial m, return its index in stdJ
// ----------------------------------------------------------------------------
//  Looks for the first polynomial f in stdJ which satisfies m=LT(f)*eta
//  for a monomial eta. The return eta*f-m and cancel all monomials
//  which are smaller than the highest corner hc
// ----------------------------------------------------------------------------

static inline  int     isLeadMonomial( poly m,ideal stdJ )
{
  int     length = INT_MAX;
  int     result = -1;

  for( int i=0; i<IDELEMS(stdJ); i++ )
  {
    if( pCmp( stdJ->m[i],m )>=0 && pDivisibleBy( stdJ->m[i],m ) )
    {
      int     tmp = pLength( stdJ->m[i] );

      if( tmp < length )
      {
        length = tmp;
        result = i;
      }
    }
  }

  return  result;
}

// ----------------------------------------------------------------------------
//  set the exponent of a monomial t an integer array
// ----------------------------------------------------------------------------

static void    setExp( poly m,int *r )
{
  for( int i=pVariables; i>0; i-- )
  {
    pSetExp( m,i,r[i-1] );
  }
  pSetm( m );
}

// ----------------------------------------------------------------------------
//  check if the ordering is a reverse wellordering, i.e. every monomial
//  is smaller than only finitely monomials
// ----------------------------------------------------------------------------

static BOOLEAN isWell( void )
{
  int b = rBlocks( currRing );

  if( b==3 &&
      ( currRing->order[0] == ringorder_ds ||
        currRing->order[0] == ringorder_Ds ||
        currRing->order[0] == ringorder_ws ||
        currRing->order[0] == ringorder_Ws ) )
  {
    return  TRUE;
  }
  else if( b>=3
  && (( currRing->order[0] ==ringorder_a
        && currRing->block1[0]==pVariables  )
    || (currRing->order[0]==ringorder_M
        && currRing->block1[0]==pVariables*pVariables )))
  {
    for( int i=pVariables-1; i>=0; i-- )
    {
      if( currRing->wvhdl[0][i]>=0 )
      {
        return  FALSE;
      }
    }
    return  TRUE;
  }

  return  FALSE;
}

// ----------------------------------------------------------------------------
//  compute all monomials not in  stdJ  and their normal forms
// ----------------------------------------------------------------------------

void    computeNF( ideal stdJ,poly hc,poly wc,spectrumPolyList *NF )
{
  int         carry,k;
  multiCnt    C( pVariables,0 );
  poly        Z = NULL;

  int         well = isWell( );

  do
  {
    poly    m = pOne();
    setExp( m,C.cnt );

    carry = FALSE;

    k = isLeadMonomial( m,stdJ );

    if( k < 0 )
    {
      // ---------------------------
      //  m  is not a lead monomial
      // ---------------------------

      NF->insert_node( m,NULL );
    }
    else if( isMultiple( Z,m ) )
    {
      // ------------------------------------
      //  m  is trivially in the ideal  stdJ
      // ------------------------------------

      pDelete( &m );
      carry = TRUE;
    }
    else if( pCmp( m,hc ) < 0 || pCmp( m,wc ) < 0 )
    {
      // -------------------
      //  we do not need  m
      // -------------------

      pDelete( &m );
      carry = TRUE;
    }
    else
    {
      // --------------------------
      //  compute lazy normal form
      // --------------------------

      poly    multiplicant = pDivide( m,stdJ->m[k] );
      pGetCoeff( multiplicant ) = nInit(1);

      poly    nf = pMult_mm( pCopy( stdJ->m[k] ), multiplicant );

      pDelete( &multiplicant );

      nf = normalFormHC( nf,hc );

      if( pNext( nf )==NULL )
      {
        // ----------------------------------
        //  normal form of  m  is  m  itself
        // ----------------------------------

        pDelete( &nf );
        NF->delete_monomial( m );
        Z = pAdd( Z,m );
        carry = TRUE;
      }
      else
      {
        nf = normalFormZ( nf,Z );

        if( pNext( nf )==NULL )
        {
          // ----------------------------------
          //  normal form of  m  is  m  itself
          // ----------------------------------

          pDelete( &nf );
          NF->delete_monomial( m );
          Z = pAdd( Z,m );
          carry = TRUE;
        }
        else
        {
          // ------------------------------------
          //  normal form of  m  is a polynomial
          // ------------------------------------

          pNorm( nf );
          if( well==TRUE )
          {
            NF->insert_node( m,nf );
          }
          else
          {
            poly    nfhard = kNF( stdJ,(ideal)NULL,pNext( nf ),0,0 );
            nfhard = normalFormHC( nfhard,hc );
            nfhard = normalFormZ ( nfhard,Z );

            if( nfhard==NULL )
            {
              NF->delete_monomial( m );
              Z = pAdd( Z,m );
              carry = TRUE;
            }
            else
            {
              pDelete( &pNext( nf ) );
              pNext( nf ) = nfhard;
              NF->insert_node( m,nf );
            }
          }
        }
      }
    }
  }
  while( C.inc( carry ) );

  // delete single monomials

  BOOLEAN  not_finished;

  do
  {
    not_finished = FALSE;

    spectrumPolyNode  *node = NF->root;

    while( node!=(spectrumPolyNode*)NULL )
    {
      if( node->nf!=NULL && pNext( node->nf )==NULL )
      {
        NF->delete_monomial( node->nf );
        not_finished = TRUE;
        node = (spectrumPolyNode*)NULL;
      }
      else
      {
        node = node->next;
      }
    }
  } while( not_finished );

  pDelete( &Z );
}

// ----------------------------------------------------------------------------
//  check if  currRing is local
// ----------------------------------------------------------------------------

BOOLEAN ringIsLocal( void )
{
  poly    m   = pOne();
  poly    one = pOne();
  BOOLEAN res=TRUE;

  for( int i=pVariables; i>0; i-- )
  {
    pSetExp( m,i,1 );
    pSetm( m );

    if( pCmp( m,one )>0 )
    {
      res=FALSE;
      break;
    }
    pSetExp( m,i,0 );
  }

  pDelete( &m );
  pDelete( &one );

  return  res;
}

// ----------------------------------------------------------------------------
// print error message corresponding to spectrumState state:
// ----------------------------------------------------------------------------
void spectrumPrintError(spectrumState state)
{
  switch( state )
  {
    case spectrumZero:
      WerrorS( "polynomial is zero" );
      break;
    case spectrumBadPoly:
      WerrorS( "polynomial has constant term" );
      break;
    case spectrumNoSingularity:
      WerrorS( "not a singularity" );
      break;
    case spectrumNotIsolated:
      WerrorS( "the singularity is not isolated" );
      break;
    case spectrumNoHC:
      WerrorS( "highest corner cannot be computed" );
      break;
    case spectrumDegenerate:
      WerrorS( "principal part is degenerate" );
      break;
    case spectrumOK:
      break;

    default:
      WerrorS( "unknown error occurred" );
      break;
  }
}
#endif /* HAVE_SPECTRUM */
// ----------------------------------------------------------------------------
//  spectrum.cc
//  end of file
// ----------------------------------------------------------------------------

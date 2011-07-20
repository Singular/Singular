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

#include <coeffs/numbers.h>
#include <polys/monomials/p_polys.h>
#include <polys/simpleideals.h>
#include <misc/intvec.h>
#include <polys/monomials/ring.h>
//extern ring currRing;

#include <kernel/kstd1.h>
#include <kernel/stairc.h>
#include <kernel/multicnt.h>
#include <kernel/GMPrat.h>
#include <kernel/kmatrix.h>
#include <kernel/npolygon.h>
#include <kernel/splist.h>
#include <kernel/semic.h>

// ----------------------------------------------------------------------------
//  test if the polynomial  h  has a term of total degree d
// ----------------------------------------------------------------------------

BOOLEAN hasTermOfDegree( poly h, int d, const ring r )
{
  do
  {
    if( p_Totaldegree( h,r )== d )
      return  TRUE;
    pIter(h);
  }
  while( h!=NULL );

  return  FALSE;
}

// ----------------------------------------------------------------------------
//  test if the polynomial  h  has a constant term
// ----------------------------------------------------------------------------

static BOOLEAN inline hasConstTerm( poly h, const ring r )
{
  return  hasTermOfDegree(h,0,r);
}

// ----------------------------------------------------------------------------
//  test if the polynomial  h  has a linear term
// ----------------------------------------------------------------------------

static BOOLEAN inline hasLinearTerm( poly h, const ring r )
{
  return  hasTermOfDegree(h,1,r);
}

// ----------------------------------------------------------------------------
//  test if the ideal  J  has a lead monomial on the axis number  k
// ----------------------------------------------------------------------------

BOOLEAN hasAxis( ideal J,int k, const ring r )
{
  int i;

  for( i=0; i<IDELEMS(J); i++ )
  {
    if (p_IsPurePower( J->m[i],r ) == k) return TRUE;
  }
  return  FALSE;
}

// ----------------------------------------------------------------------------
//  test if the ideal  J  has  1  as a lead monomial
// ----------------------------------------------------------------------------

int     hasOne( ideal J, const ring r )
{
  int i;

  for( i=0; i<IDELEMS(J); i++ )
  {
    if (p_IsConstant(J->m[i],r)) return TRUE;
  }
  return  FALSE;
}
// ----------------------------------------------------------------------------
//  test if  m  is a multiple of one of the monomials of  f
// ----------------------------------------------------------------------------

int     isMultiple( poly f,poly m, const ring r )
{
  while( f!=NULL )
  {
    // ---------------------------------------------------
    //  for a local order  f|m  is only possible if  f>=m
    // ---------------------------------------------------

    if( p_LmCmp( f,m,r )>=0 )
    {
      if( p_LmDivisibleByNoComp( f,m,r ) )
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

poly    computeWC( const newtonPolygon &np,Rational max_weight, const ring r )
{
  poly m  = p_One(r);
  poly wc = NULL;
  int  mdegree;

  for( int i=1; i<=r->N; i++ )
  {
    mdegree = 1;
    p_SetExp( m,i,mdegree,r );
    // pSetm( m );
    // np.weight_shift does not need pSetm( m ), postpone it

    while(  np.weight_shift( m,r )<max_weight )
    {
      mdegree++;
      p_SetExp( m,i,mdegree,r );
      // pSetm( m );
      // np.weight_shift does not need pSetm( m ), postpone it
    }
    p_Setm( m,r );

    if( i==1 || p_Cmp( m,wc,r )<0 )
    {
      p_Delete( &wc,r );
      wc = p_Head( m,r );
    }

    p_SetExp( m,i,0,r );
  }

  p_Delete( &m,r );

  return  wc;
}

// ----------------------------------------------------------------------------
//  deletes all monomials of  f  which are less than  hc
// ----------------------------------------------------------------------------

static inline  poly    normalFormHC( poly f,poly hc, const ring r )
{
  poly    *ptr = &f;

  while( (*ptr)!=NULL )
  {
    if( p_LmCmp( *ptr,hc,r )>=0 )
    {
      ptr = &(pNext( *ptr ));
    }
    else
    {
      p_Delete( ptr,r );
      return  f;
    }
  }

  return f;
}

// ----------------------------------------------------------------------------
//  deletes all monomials of  f  which are multiples of monomials of  Z
// ----------------------------------------------------------------------------

static inline  poly    normalFormZ( poly f,poly Z, const ring r )
{
  poly    *ptr = &f;

  while( (*ptr)!=NULL )
  {
    if( !isMultiple( Z,*ptr,r ) )
    {
      ptr = &(pNext( *ptr ));
    }
    else
    {
      p_LmDelete(ptr,r);
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

static inline  int     isLeadMonomial( poly m,ideal stdJ, const ring r )
{
  int     length = INT_MAX;
  int     result = -1;

  for( int i=0; i<IDELEMS(stdJ); i++ )
  {
    if( p_Cmp( stdJ->m[i],m,r )>=0 && p_DivisibleBy( stdJ->m[i],m,r ) )
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

static void    setExp( poly m,int *r, const ring s )
{
  for( int i=s->N; i>0; i-- )
  {
    p_SetExp( m,i,r[i-1],s );
  }
  p_Setm( m,s );
}

// ----------------------------------------------------------------------------
//  check if the ordering is a reverse wellordering, i.e. every monomial
//  is smaller than only finitely monomials
// ----------------------------------------------------------------------------

static BOOLEAN isWell( const ring r )
{
  int b = rBlocks( r );

  if( b==3 &&
      ( r->order[0] == ringorder_ds ||
        r->order[0] == ringorder_Ds ||
        r->order[0] == ringorder_ws ||
        r->order[0] == ringorder_Ws ) )
  {
    return  TRUE;
  }
  else if( b>=3
  && (( r->order[0] ==ringorder_a
        && r->block1[0]==r->N )
    || (r->order[0]==ringorder_M
        && r->block1[0]==r->N*r->N )))
  {
    for( int i=r->N-1; i>=0; i-- )
    {
      if( r->wvhdl[0][i]>=0 )
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

void    computeNF( ideal stdJ,poly hc,poly wc,spectrumPolyList *NF, const ring r )
{
  int         carry,k;
  multiCnt    C( r->N,0 );
  poly        Z = NULL;

  int         well = isWell(r);

  do
  {
    poly    m = p_One(r);
    setExp( m,C.cnt,r );

    carry = FALSE;

    k = isLeadMonomial( m,stdJ,r );

    if( k < 0 )
    {
      // ---------------------------
      //  m  is not a lead monomial
      // ---------------------------

      NF->insert_node( m,NULL,r );
    }
    else if( isMultiple( Z,m,r ) )
    {
      // ------------------------------------
      //  m  is trivially in the ideal  stdJ
      // ------------------------------------

      p_Delete( &m,r );
      carry = TRUE;
    }
    else if( p_Cmp( m,hc,r ) < 0 || p_Cmp( m,wc,r ) < 0 )
    {
      // -------------------
      //  we do not need  m
      // -------------------

      p_Delete( &m,r );
      carry = TRUE;
    }
    else
    {
      // --------------------------
      //  compute lazy normal form
      // --------------------------

      poly    multiplicant = p_Divide( m,stdJ->m[k],r );
      pGetCoeff( multiplicant ) = n_Init(1,r->cf);

      poly    nf = p_Mult_mm( p_Copy( stdJ->m[k],r ), multiplicant,r );

      p_Delete( &multiplicant,r );

      nf = normalFormHC( nf,hc,r );

      if( pNext( nf )==NULL )
      {
        // ----------------------------------
        //  normal form of  m  is  m  itself
        // ----------------------------------

        p_Delete( &nf,r );
        NF->delete_monomial( m,r );
        Z = p_Add_q( Z,m,r );
        carry = TRUE;
      }
      else
      {
        nf = normalFormZ( nf,Z,r );

        if( pNext( nf )==NULL )
        {
          // ----------------------------------
          //  normal form of  m  is  m  itself
          // ----------------------------------

          p_Delete( &nf,r );
          NF->delete_monomial( m,r );
          Z = p_Add_q( Z,m,r );
          carry = TRUE;
        }
        else
        {
          // ------------------------------------
          //  normal form of  m  is a polynomial
          // ------------------------------------

          p_Norm( nf,r );
          if( well==TRUE )
          {
            NF->insert_node( m,nf,r );
          }
          else
          {
            poly    nfhard = kNF( stdJ,(ideal)NULL,pNext( nf ),0,0 );
            nfhard = normalFormHC( nfhard,hc,r );
            nfhard = normalFormZ ( nfhard,Z,r );

            if( nfhard==NULL )
            {
              NF->delete_monomial( m,r );
              Z = p_Add_q( Z,m,r );
              carry = TRUE;
            }
            else
            {
              p_Delete( &pNext( nf ),r );
              pNext( nf ) = nfhard;
              NF->insert_node( m,nf,r );
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
        NF->delete_monomial( node->nf,r );
        not_finished = TRUE;
        node = (spectrumPolyNode*)NULL;
      }
      else
      {
        node = node->next;
      }
    }
  } while( not_finished );

  p_Delete( &Z,r );
}

// ----------------------------------------------------------------------------
//  check if  currRing is local
// ----------------------------------------------------------------------------

BOOLEAN ringIsLocal( const ring r )
{
  poly    m   = p_One(r);
  poly    one = p_One(r);
  BOOLEAN res=TRUE;

  for( int i=r->N; i>0; i-- )
  {
    p_SetExp( m,i,1,r );
    p_Setm( m,r );

    if( p_Cmp( m,one,r )>0 )
    {
      res=FALSE;
      break;
    }
    p_SetExp( m,i,0,r );
  }

  p_Delete( &m,r );
  p_Delete( &one,r );

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

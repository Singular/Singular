// ----------------------------------------------------------------------------
//  spectrum.cc
//  begin of file
//  Stephan Endrass, endrass@mathematik.uni-mainz.de
//  23.7.99
// ----------------------------------------------------------------------------

#define SPECTRUM_CC

#include "mod2.h"

#ifdef HAVE_SPECTRUM

#ifdef  SPECTRUM_PRINT
#include <iostream.h>
#ifndef  SPECTRUM_IOSTREAM
#include <stdio.h>
#endif
#endif

#include <limits.h>

#include "numbers.h"
#include "polys.h"
#include "ipid.h"
#include "ideals.h"
#include "kstd1.h"
#include "stairc.h"
#include "lists.h"
#include "intvec.h"
#include "ring.h"

#include "multicnt.h"
#include "GMPrat.h"
#include "kmatrix.h"
#include "npolygon.h"
#include "splist.h"
#include "semic.h"

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
  while( h!=(poly)NULL );

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
  while( f!=(poly)NULL )
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
  poly wc = (poly)NULL;
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

  while( (*ptr)!=(poly)NULL )
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

  while( (*ptr)!=(poly)NULL )
  {
    if( !isMultiple( Z,*ptr ) )
    {
      ptr = &(pNext( *ptr ));
    }
    else
    {
      pDeleteLm(ptr);
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

static void    computeNF( ideal stdJ,poly hc,poly wc,spectrumPolyList *NF )
{
  int         carry,k;
  multiCnt    C( pVariables,0 );
  poly        Z = (poly)NULL;

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

      NF->insert_node( m,(poly)NULL );
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

      if( pNext( nf )==(poly)NULL )
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

        if( pNext( nf )==(poly)NULL )
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

            if( nfhard==(poly)NULL )
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
      if( node->nf!=(poly)NULL && pNext( node->nf )==(poly)NULL )
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
//  this is the main spectrum computation function
// ----------------------------------------------------------------------------

static spectrumState   spectrumCompute( poly h,lists *L,int fast )
{
  int i,j;

  #ifdef SPECTRUM_DEBUG
  #ifdef SPECTRUM_PRINT
  #ifdef SPECTRUM_IOSTREAM
    cout << "spectrumCompute\n";
    if( fast==0 ) cout << "    no optimization" << endl;
    if( fast==1 ) cout << "    weight optimization" << endl;
    if( fast==2 ) cout << "    symmetry optimization" << endl;
  #else
    fprintf( stdout,"spectrumCompute\n" );
    if( fast==0 ) fprintf( stdout,"    no optimization\n" );
    if( fast==1 ) fprintf( stdout,"    weight optimization\n" );
    if( fast==2 ) fprintf( stdout,"    symmetry optimization\n" );
  #endif
  #endif
  #endif

  // ----------------------
  //  check if  h  is zero
  // ----------------------

  if( h==(poly)NULL )
  {
    return  spectrumZero;
  }

  // ----------------------------------
  //  check if  h  has a constant term
  // ----------------------------------

  if( hasConstTerm( h ) )
  {
    return  spectrumBadPoly;
  }

  // --------------------------------
  //  check if  h  has a linear term
  // --------------------------------

  if( hasLinearTerm( h ) )
  {
    *L = (lists)omAllocBin( slists_bin);
    (*L)->Init( 1 );
    (*L)->m[0].rtyp = INT_CMD;    //  milnor number
    /* (*L)->m[0].data = (void*)0;a  -- done by Init */

    return  spectrumNoSingularity;
  }

  // ----------------------------------
  //  compute the jacobi ideal of  (h)
  // ----------------------------------

  ideal J = NULL;
  J = idInit( pVariables,1 );

  #ifdef SPECTRUM_DEBUG
  #ifdef SPECTRUM_PRINT
  #ifdef SPECTRUM_IOSTREAM
    cout << "\n   computing the Jacobi ideal...\n";
  #else
    fprintf( stdout,"\n   computing the Jacobi ideal...\n" );
  #endif
  #endif
  #endif

  for( i=0; i<pVariables; i++ )
  {
    J->m[i] = pDiff( h,i+1); //j );

    #ifdef SPECTRUM_DEBUG
    #ifdef SPECTRUM_PRINT
    #ifdef SPECTRUM_IOSTREAM
      cout << "        ";
    #else
      fprintf( stdout,"        " );
    #endif
      pWrite( J->m[i] );
    #endif
    #endif
  }

  // --------------------------------------------
  //  compute a standard basis  stdJ  of  jac(h)
  // --------------------------------------------

  #ifdef SPECTRUM_DEBUG
  #ifdef SPECTRUM_PRINT
  #ifdef SPECTRUM_IOSTREAM
    cout << endl;
    cout << "    computing a standard basis..." << endl;
  #else
    fprintf( stdout,"\n" );
    fprintf( stdout,"    computing a standard basis...\n" );
  #endif
  #endif
  #endif

  ideal stdJ = kStd(J,currQuotient,isNotHomog,NULL);
  idSkipZeroes( stdJ );

  #ifdef SPECTRUM_DEBUG
  #ifdef SPECTRUM_PRINT
    for( i=0; i<IDELEMS(stdJ); i++ )
    {
      #ifdef SPECTRUM_IOSTREAM
        cout << "        ";
      #else
        fprintf( stdout,"        " );
      #endif

      pWrite( stdJ->m[i] );
    }
  #endif
  #endif

  idDelete( &J );

  // ------------------------------------------
  //  check if the  h  has a singularity
  // ------------------------------------------

  if( hasOne( stdJ ) )
  {
    // -------------------------------
    //  h is smooth in the origin
    //  return only the Milnor number
    // -------------------------------

    *L = (lists)omAllocBin( slists_bin);
    (*L)->Init( 1 );
    (*L)->m[0].rtyp = INT_CMD;    //  milnor number
    /* (*L)->m[0].data = (void*)0;a  -- done by Init */

    return  spectrumNoSingularity;
  }

  // ------------------------------------------
  //  check if the singularity  h  is isolated
  // ------------------------------------------

  for( i=pVariables; i>0; i-- )
  {
    if( hasAxis( stdJ,i )==FALSE )
    {
      return  spectrumNotIsolated;
    }
  }

  // ------------------------------------------
  //  compute the highest corner  hc  of  stdJ
  // ------------------------------------------

  #ifdef SPECTRUM_DEBUG
  #ifdef SPECTRUM_PRINT
  #ifdef SPECTRUM_IOSTREAM
    cout << "\n    computing the highest corner...\n";
  #else
    fprintf( stdout,"\n    computing the highest corner...\n" );
  #endif
  #endif
  #endif

  poly hc = (poly)NULL;

  scComputeHC( stdJ,currQuotient, 0,hc );

  if( hc!=(poly)NULL )
  {
    pGetCoeff(hc) = nInit(1);

    for( i=pVariables; i>0; i-- )
    {
      if( pGetExp( hc,i )>0 ) pDecrExp( hc,i );
    }
    pSetm( hc );
  }
  else
  {
    return  spectrumNoHC;
  }

  #ifdef SPECTRUM_DEBUG
  #ifdef SPECTRUM_PRINT
  #ifdef SPECTRUM_IOSTREAM
    cout << "       ";
  #else
    fprintf( stdout,"       " );
  #endif
    pWrite( hc );
  #endif
  #endif

  // ----------------------------------------
  //  compute the Newton polygon  nph  of  h
  // ----------------------------------------

  #ifdef SPECTRUM_DEBUG
  #ifdef SPECTRUM_PRINT
  #ifdef SPECTRUM_IOSTREAM
    cout << "\n    computing the newton polygon...\n";
  #else
    fprintf( stdout,"\n    computing the newton polygon...\n" );
  #endif
  #endif
  #endif

  newtonPolygon nph( h );

  #ifdef SPECTRUM_DEBUG
  #ifdef SPECTRUM_PRINT
    cout << nph;
  #endif
  #endif

  // -----------------------------------------------
  //  compute the weight corner  wc  of  (stdj,nph)
  // -----------------------------------------------

  #ifdef SPECTRUM_DEBUG
  #ifdef SPECTRUM_PRINT
  #ifdef SPECTRUM_IOSTREAM
    cout << "\n    computing the weight corner...\n";
  #else
    fprintf( stdout,"\n    computing the weight corner...\n" );
  #endif
  #endif
  #endif

  poly    wc = ( fast==0 ? pCopy( hc ) :
               ( fast==1 ? computeWC( nph,(Rational)pVariables ) :
              /* fast==2 */computeWC( nph,((Rational)pVariables)/(Rational)2 ) ) );

  #ifdef SPECTRUM_DEBUG
  #ifdef SPECTRUM_PRINT
  #ifdef SPECTRUM_IOSTREAM
    cout << "        ";
  #else
    fprintf( stdout,"        " );
  #endif
    pWrite( wc );
  #endif
  #endif

  // -------------
  //  compute  NF
  // -------------

  #ifdef SPECTRUM_DEBUG
  #ifdef SPECTRUM_PRINT
  #ifdef SPECTRUM_IOSTREAM
    cout << "\n    computing NF...\n" << endl;
  #else
    fprintf( stdout,"\n    computing NF...\n" );
  #endif
  #endif
  #endif

  spectrumPolyList NF( &nph );

  computeNF( stdJ,hc,wc,&NF );

  #ifdef SPECTRUM_DEBUG
  #ifdef SPECTRUM_PRINT
    cout << NF;
  #ifdef SPECTRUM_IOSTREAM
    cout << endl;
  #else
    fprintf( stdout,"\n" );
  #endif
  #endif
  #endif

  // ----------------------------
  //  compute the spectrum of  h
  // ----------------------------

  return  NF.spectrum( L,fast );
}

// ----------------------------------------------------------------------------
//  check if  currRing is local
// ----------------------------------------------------------------------------

static BOOLEAN ringIsLocal( void )
{
  poly    m   = pOne();
  poly    one = pOne();

  for( int i=pVariables; i>0; i-- )
  {
    pSetExp( m,i,1 );
    pSetm( m );

    if( pCmp( m,one )>0 )
    {
      return  FALSE;
    }
    pSetExp( m,i,0 );
  }

  pDelete( &m );
  pDelete( &one );

  return  TRUE;
}

// ----------------------------------------------------------------------------
// print error message corresponding to spectrumState state:
// ----------------------------------------------------------------------------
static void spectrumPrintError(spectrumState state)
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
// ----------------------------------------------------------------------------
//  this procedure is called from the interpreter
// ----------------------------------------------------------------------------
//  first  = polynomial
//  result = list of spectrum numbers
// ----------------------------------------------------------------------------

BOOLEAN spectrumProc( leftv result,leftv first )
{
  spectrumState state = spectrumOK;

  // -------------------
  //  check consistency
  // -------------------

  //  check for a local ring

  if( !ringIsLocal( ) )
  {
    WerrorS( "only works for local orderings" );
    state = spectrumWrongRing;
  }

  //  no quotient rings are allowed

  else if( currRing->qideal != NULL )
  {
    WerrorS( "does not work in quotient rings" );
    state = spectrumWrongRing;
  }
  else
  {
    lists   L    = (lists)NULL;
    int     flag = 1; // weight corner optimization is safe

    state = spectrumCompute( (poly)first->Data( ),&L,flag );

    if( state==spectrumOK )
    {
      result->rtyp = LIST_CMD;
      result->data = (char*)L;
    }
    else
    {
      spectrumPrintError(state);
    }
  }

  return  (state!=spectrumOK);
}

// ----------------------------------------------------------------------------
//  this procedure is called from the interpreter
// ----------------------------------------------------------------------------
//  first  = polynomial
//  result = list of spectrum numbers
// ----------------------------------------------------------------------------

BOOLEAN spectrumfProc( leftv result,leftv first )
{
  spectrumState state = spectrumOK;

  // -------------------
  //  check consistency
  // -------------------

  //  check for a local polynomial ring

  if( currRing->OrdSgn != -1 )
  // ?? HS: the test above is also true for k[x][[y]], k[[x]][y]
  // or should we use:
  //if( !ringIsLocal( ) )
  {
    WerrorS( "only works for local orderings" );
    state = spectrumWrongRing;
  }
  else if( currRing->qideal != NULL )
  {
    WerrorS( "does not work in quotient rings" );
    state = spectrumWrongRing;
  }
  else
  {
    lists   L    = (lists)NULL;
    int     flag = 2; // symmetric optimization

    state = spectrumCompute( (poly)first->Data( ),&L,flag );

    if( state==spectrumOK )
    {
      result->rtyp = LIST_CMD;
      result->data = (char*)L;
    }
    else
    {
      spectrumPrintError(state);
    }
  }

  return  (state!=spectrumOK);
}

// ----------------------------------------------------------------------------
//  check if a list is a spectrum
//  check for:
//      list has 6 elements
//      1st element is int (mu=Milnor number)
//      2nd element is int (pg=geometrical genus)
//      3rd element is int (n =number of different spectrum numbers)
//      4th element is intvec (num=numerators)
//      5th element is intvec (den=denomiantors)
//      6th element is intvec (mul=multiplicities)
//      exactly n numerators
//      exactly n denominators
//      exactly n multiplicities
//      mu>0
//      pg>=0
//      n>0
//      num>0
//      den>0
//      mul>0
//      symmetriy with respect to numberofvariables/2
//      monotony
//      mu = sum of all multiplicities
//      pg = sum of all multiplicities where num/den<=1
// ----------------------------------------------------------------------------

semicState  list_is_spectrum( lists l )
{
    // -------------------
    //  check list length
    // -------------------

    if( l->nr < 5 )
    {
        return  semicListTooShort;
    }
    else if( l->nr > 5 )
    {
        return  semicListTooLong;
    }

    // -------------
    //  check types
    // -------------

    if( l->m[0].rtyp != INT_CMD )
    {
        return  semicListFirstElementWrongType;
    }
    else if( l->m[1].rtyp != INT_CMD )
    {
        return  semicListSecondElementWrongType;
    }
    else if( l->m[2].rtyp != INT_CMD )
    {
        return  semicListThirdElementWrongType;
    }
    else if( l->m[3].rtyp != INTVEC_CMD )
    {
        return  semicListFourthElementWrongType;
    }
    else if( l->m[4].rtyp != INTVEC_CMD )
    {
        return  semicListFifthElementWrongType;
    }
    else if( l->m[5].rtyp != INTVEC_CMD )
    {
        return  semicListSixthElementWrongType;
    }

    // -------------------------
    //  check number of entries
    // -------------------------

    int     mu = (int)(l->m[0].Data( ));
    int     pg = (int)(l->m[1].Data( ));
    int     n  = (int)(l->m[2].Data( ));

    if( n <= 0 )
    {
        return  semicListNNegative;
    }

    intvec  *num = (intvec*)l->m[3].Data( );
    intvec  *den = (intvec*)l->m[4].Data( );
    intvec  *mul = (intvec*)l->m[5].Data( );

    if( n != num->length( ) )
    {
        return  semicListWrongNumberOfNumerators;
    }
    else if( n != den->length( ) )
    {
        return  semicListWrongNumberOfDenominators;
    }
    else if( n != mul->length( ) )
    {
        return  semicListWrongNumberOfMultiplicities;
    }

    // --------
    //  values
    // --------

    if( mu <= 0 )
    {
        return  semicListMuNegative;
    }
    if( pg < 0 )
    {
        return  semicListPgNegative;
    }

    int i;

    for( i=0; i<n; i++ )
    {
        if( (*num)[i] <= 0 )
        {
            return  semicListNumNegative;
        }
        if( (*den)[i] <= 0 )
        {
            return  semicListDenNegative;
        }
        if( (*mul)[i] <= 0 )
        {
            return  semicListMulNegative;
        }
    }

    // ----------------
    //  check symmetry
    // ----------------

    int     j;

    for( i=0, j=n-1; i<=j; i++,j-- )
    {
        if( (*num)[i] != pVariables*((*den)[i]) - (*num)[j] ||
            (*den)[i] != (*den)[j] ||
            (*mul)[i] != (*mul)[j] )
        {
            return  semicListNotSymmetric;
        }
    }

    // ----------------
    //  check monotony
    // ----------------

    for( i=0, j=1; i<n/2; i++,j++ )
    {
        if( (*num)[i]*(*den)[j] >= (*num)[j]*(*den)[i] )
        {
            return  semicListNotMonotonous;
        }
    }

    // ---------------------
    //  check Milnor number
    // ---------------------

    for( mu=0, i=0; i<n; i++ )
    {
        mu += (*mul)[i];
    }

    if( mu != (int)(l->m[0].Data( )) )
    {
        return  semicListMilnorWrong;
    }

    // -------------------------
    //  check geometrical genus
    // -------------------------

    for( pg=0, i=0; i<n; i++ )
    {
        if( (*num)[i]<=(*den)[i] )
        {
            pg += (*mul)[i];
        }
    }

    if( pg != (int)(l->m[1].Data( )) )
    {
        return  semicListPGWrong;
    }

    return  semicOK;
}

// ----------------------------------------------------------------------------
//  print out an error message for a spectrum list
// ----------------------------------------------------------------------------

void    list_error( semicState state )
{
    switch( state )
    {
        case semicListTooShort:
            WerrorS( "the list is too short" );
            break;
        case semicListTooLong:
            WerrorS( "the list is too long" );
            break;

        case semicListFirstElementWrongType:
            WerrorS( "first element of the list should be int" );
            break;
        case semicListSecondElementWrongType:
            WerrorS( "second element of the list should be int" );
            break;
        case semicListThirdElementWrongType:
            WerrorS( "third element of the list should be int" );
            break;
        case semicListFourthElementWrongType:
            WerrorS( "fourth element of the list should be intvec" );
            break;
        case semicListFifthElementWrongType:
            WerrorS( "fifth element of the list should be intvec" );
            break;
        case semicListSixthElementWrongType:
            WerrorS( "sixth element of the list should be intvec" );
            break;

        case semicListNNegative:
            WerrorS( "first element of the list should be positive" );
            break;
        case semicListWrongNumberOfNumerators:
            WerrorS( "wrong number of numerators" );
            break;
        case semicListWrongNumberOfDenominators:
            WerrorS( "wrong number of denominators" );
            break;
        case semicListWrongNumberOfMultiplicities:
            WerrorS( "wrong number of multiplicities" );
            break;

        case semicListMuNegative:
            WerrorS( "the Milnor number should be positive" );
            break;
        case semicListPgNegative:
            WerrorS( "the geometrical genus should be nonnegative" );
            break;
        case semicListNumNegative:
            WerrorS( "all numerators should be positive" );
            break;
        case semicListDenNegative:
            WerrorS( "all denominators should be positive" );
            break;
        case semicListMulNegative:
            WerrorS( "all multiplicities should be positive" );
            break;

        case semicListNotSymmetric:
            WerrorS( "it is not symmetric" );
            break;
        case semicListNotMonotonous:
            WerrorS( "it is not monotonous" );
            break;

        case semicListMilnorWrong:
            WerrorS( "the Milnor number is wrong" );
            break;
        case semicListPGWrong:
            WerrorS( "the geometrical genus is wrong" );
            break;

        default:
            WerrorS( "unspecific error" );
            break;
    }
}

// ----------------------------------------------------------------------------
//  this procedure is called from the interpreter
// ----------------------------------------------------------------------------
//  first  = list of spectrum numbers
//  second = list of spectrum numbers
//  result = sum of the two lists
// ----------------------------------------------------------------------------

BOOLEAN spaddProc( leftv result,leftv first,leftv second )
{
    semicState  state;

    // -----------------
    //  check arguments
    // -----------------

    lists l1 = (lists)first->Data( );
    lists l2 = (lists)second->Data( );

    if( (state=list_is_spectrum( l1 )) != semicOK )
    {
        WerrorS( "first argument is not a spectrum:" );
        list_error( state );
    }
    else if( (state=list_is_spectrum( l2 )) != semicOK )
    {
        WerrorS( "second argument is not a spectrum:" );
        list_error( state );
    }
    else
    {
        spectrum s1( l1 );
        spectrum s2( l2 );
        spectrum sum( s1+s2 );

        result->rtyp = LIST_CMD;
        result->data = (char*)(sum.thelist( ));
    }

    return  (state!=semicOK);
}

// ----------------------------------------------------------------------------
//  this procedure is called from the interpreter
// ----------------------------------------------------------------------------
//  first  = list of spectrum numbers
//  second = integer
//  result = the multiple of the first list by the second factor
// ----------------------------------------------------------------------------

BOOLEAN spmulProc( leftv result,leftv first,leftv second )
{
    semicState  state;

    // -----------------
    //  check arguments
    // -----------------

    lists   l = (lists)first->Data( );
    int     k = (int)second->Data( );

    if( (state=list_is_spectrum( l ))!=semicOK )
    {
        WerrorS( "first argument is not a spectrum" );
        list_error( state );
    }
    else if( k < 0 )
    {
        WerrorS( "second argument should be positive" );
        state = semicMulNegative;
    }
    else
    {
        spectrum s( l );
        spectrum product( k*s );

        result->rtyp = LIST_CMD;
        result->data = (char*)product.thelist( );
    }

    return  (state!=semicOK);
}

// ----------------------------------------------------------------------------
//  this procedure is called from the interpreter
// ----------------------------------------------------------------------------
//  first  = list of spectrum numbers
//  second = list of spectrum numbers
//  result = semicontinuity index
// ----------------------------------------------------------------------------

BOOLEAN    semicProc3   ( leftv res,leftv u,leftv v,leftv w )
{
  semicState  state;
  BOOLEAN qh=(((int)w->Data())==1);

  // -----------------
  //  check arguments
  // -----------------

  lists l1 = (lists)u->Data( );
  lists l2 = (lists)v->Data( );

  if( (state=list_is_spectrum( l1 ))!=semicOK )
  {
    WerrorS( "first argument is not a spectrum" );
    list_error( state );
  }
  else if( (state=list_is_spectrum( l2 ))!=semicOK )
  {
    WerrorS( "second argument is not a spectrum" );
    list_error( state );
  }
  else
  {
    spectrum s1( l1 );
    spectrum s2( l2 );

    res->rtyp = INT_CMD;
    if (qh)
      res->data = (void*)(s1.mult_spectrumh( s2 ));
    else
      res->data = (void*)(s1.mult_spectrum( s2 ));
  }

  // -----------------
  //  check status
  // -----------------

  return  (state!=semicOK);
}
BOOLEAN    semicProc   ( leftv res,leftv u,leftv v )
{
  sleftv tmp;
  memset(&tmp,0,sizeof(tmp));
  tmp.rtyp=INT_CMD;
  /* tmp.data = (void *)0;  -- done by memset */

  return  semicProc3(res,u,v,&tmp);
}
#endif /* HAVE_SPECTRUM */
// ----------------------------------------------------------------------------
//  spectrum.cc
//  end of file
// ----------------------------------------------------------------------------

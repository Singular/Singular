/****************************************
*  Computer Algebra System SINGULAR     *
****************************************/

/* $Id: mpr_inout.cc,v 1.5 1999-10-14 14:27:23 obachman Exp $ */

/*
* ABSTRACT - multipolynomial resultant
*/

#include "mod2.h"
//#ifdef HAVE_MPR

//-> includes
#include "tok.h"
#include "structs.h"
#include "subexpr.h"
#include "polys.h"
#include "ideals.h"
#include "ring.h"
#include "ipid.h"
#include "ipshell.h"
#include "febase.h"
#include "mmemory.h"
#include "numbers.h"
#include "lists.h"

#include <math.h>

#include "mpr_global.h"
#include "mpr_inout.h"
#include "mpr_base.h"
#include "mpr_numeric.h"

// to get detailed timigs, define MPR_TIMING
#ifdef MPR_TIMING
#define TIMING
#include "../factory/timing.h"
TIMING_DEFINE_PRINT(mpr_overall)
TIMING_DEFINE_PRINT(mpr_check)
TIMING_DEFINE_PRINT(mpr_constr)
TIMING_DEFINE_PRINT(mpr_ures)
TIMING_DEFINE_PRINT(mpr_mures)
TIMING_DEFINE_PRINT(mpr_arrange)
TIMING_DEFINE_PRINT(mpr_solver)
#define TIMING_EPR(t,msg) TIMING_END_AND_PRINT(t,msg);TIMING_RESET(t);
#else
#define TIMING_START(x)
#define TIMING_EPR(x,y)
#endif


enum mprState
{
    mprOk,
    mprWrongRType,
    mprHasOne,
    mprInfNumOfVars,
    mprNotReduced,
    mprNotZeroDim,
    mprNotHomog,
    mprUnSupField
};

extern size_t gmp_output_digits;
//<-

//-> nPrint(number n)
void nPrint(number n)
{
  poly o=pOne();
  pSetCoeff(o, nCopy(n) );
  pWrite0( o );
  pDelete( &o );
}
//<-

//------------------------------------------------------------------------------

//-> void mprPrintError( mprState state )
void mprPrintError( mprState state, const char * name )
{
  switch (state)
  {
  case mprWrongRType:
    WerrorS("Unknown resultant matrix type choosen!");
    break;
  case mprHasOne:
    Werror("One element of the ideal %s is constant!",name);
    break;
  case mprInfNumOfVars:
    Werror("Numer of elements in given ideal %s must be equal to %d!",
           name,pVariables+1);
    break;
  case mprNotZeroDim:
    Werror("The given ideal %s must 0-dimensional!",name);
    break;
  case mprNotHomog:
    Werror("The given ideal %s has to be homogeneous in"
           " the first ring variable!",name);
    break;
  case mprNotReduced:
    Werror("The given ideal %s has to reduced!",name);
    break;
  case mprUnSupField:
    WerrorS("Ground field not implemented!");
    break;
  default:
    break;
  }
}
//<-

//-> mprState mprIdealCheck()
mprState mprIdealCheck( const ideal theIdeal,
                        const char * name,
                        uResultant::resMatType mtype,
                        BOOLEAN rmatrix= false )
{
  mprState state = mprOk;
  int power;
  int k;

  int numOfVars= mtype == uResultant::denseResMat?pVariables-1:pVariables;
  if ( rmatrix ) numOfVars++;

  if ( mtype == uResultant::none )
    state= mprWrongRType;

  if ( IDELEMS(theIdeal) != numOfVars )
    state= mprInfNumOfVars;

  for ( k= IDELEMS(theIdeal) - 1; (state == mprOk) && (k >= 0); k-- )
  {
    poly p = (theIdeal->m)[k];
    if ( pIsConstant(p) ) state= mprHasOne;
    else
    if ( (mtype == uResultant::denseResMat) && !pIsHomogeneous(p) )
      state=mprNotHomog;
  }

  if ( !(rField_is_R()||
         rField_is_Q()||
         rField_is_long_R()||
         rField_is_long_C()||
         (rmatrix && rField_is_Q_a())) )
    state= mprUnSupField;

  if ( state != mprOk ) mprPrintError( state, "" /* name */ );

  return state;
}
//<-

//-> uResultant::resMatType determineMType( int imtype )
uResultant::resMatType determineMType( int imtype )
{
  switch ( imtype )
  {
  case MPR_DENSE:
    return uResultant::denseResMat;
  case 0:
  case MPR_SPARSE:
    return uResultant::sparseResMat;
  default:
    return uResultant::none;
  }
}
//<-

//-> BOOLEAN nuUResSolve( leftv res, leftv args )
BOOLEAN nuUResSolve( leftv res, leftv args )
{
  leftv v= args;

  ideal gls;
  int imtype;
  int howclean;

  // get ideal
  if ( v->Typ() != IDEAL_CMD )
    return TRUE;
  else gls= (ideal)(args->Data());
  v= v->next;

  // get resultant matrix type to use (0,1)
  if ( v->Typ() != INT_CMD )
    return TRUE;
  else imtype= (int)v->Data();
  v= v->next;

  // get and set precision in digits ( > 0 )
  if ( v->Typ() != INT_CMD )
    return TRUE;
  else if ( !(rField_is_R()||rField_is_long_R()||rField_is_long_C()) ) 
  {
    setGMPFloatDigits( (unsigned long int)v->Data() );
  }
  v= v->next;

  // get interpolation steps (0,1,2)
  if ( v->Typ() != INT_CMD )
    return TRUE;
  else howclean= (int)v->Data();

  uResultant::resMatType mtype= determineMType( imtype );
  int i,c,count;
  lists listofroots= NULL;
  lists emptylist;
  number smv= NULL;
  BOOLEAN interpolate_det= (mtype==uResultant::denseResMat)?TRUE:FALSE;

  //emptylist= (lists)AllocSizeOf( slists );
  //emptylist->Init( 0 );

  //res->rtyp = LIST_CMD;
  //res->data= (void *)emptylist;

  TIMING_START(mpr_overall);

  // check input ideal ( = polynomial system )
  if ( mprIdealCheck( gls, args->Name(), mtype ) != mprOk )
  {
    return TRUE;
  }

  uResultant * ures;
  rootContainer ** iproots;
  rootContainer ** muiproots;
  rootArranger * arranger;

  // main task 1: setup of resultant matrix
  TIMING_START(mpr_constr);
  ures= new uResultant( gls, mtype );
  if ( ures->accessResMat()->initState() != resMatrixBase::ready )
  {
    WerrorS("Error occurred during matrix setup!");
    return TRUE;
  }
  TIMING_EPR(mpr_constr, "construction\t\t")

  // if dense resultant, check if minor nonsingular
  TIMING_START(mpr_check);
  if ( mtype == uResultant::denseResMat )
  {
    smv= ures->accessResMat()->getSubDet();
#ifdef mprDEBUG_PROT
    Print("// Determinant of submatrix: ");nPrint(smv);PrintLn();
#endif
    if ( nIsZero(smv) )
    {
      WerrorS("Unsuitable input ideal: Minor of resultant matrix is singular!");
      return TRUE;
    }
  }
  TIMING_EPR(mpr_check,  "input check\t\t")

  // main task 2: Interpolate specialized resultant polynomials
  TIMING_START(mpr_ures);
  if ( interpolate_det )
    iproots= ures->interpolateDenseSP( false, smv );
  else
    iproots= ures->specializeInU( false, smv );
  TIMING_EPR(mpr_ures,   "interpolation ures\t")

  // main task 3: Interpolate specialized resultant polynomials
  TIMING_START(mpr_mures);
  if ( interpolate_det )
    muiproots= ures->interpolateDenseSP( true, smv );
  else
    muiproots= ures->specializeInU( true, smv );
  TIMING_EPR(mpr_mures,  "interpolation mures\t")

#ifdef mprDEBUG_PROT
  c= iproots[0]->getAnzElems();
  for (i=0; i < c; i++) pWrite(iproots[i]->getPoly());
  c= muiproots[0]->getAnzElems();
  for (i=0; i < c; i++) pWrite(muiproots[i]->getPoly());
#endif

  // main task 4: Compute roots of specialized polys and match them up
  arranger= new rootArranger( iproots, muiproots, howclean );
  TIMING_START(mpr_solver);
  arranger->solve_all();
  TIMING_EPR(mpr_solver, "solver time\t\t");

  // get list of roots
  if ( arranger->success() )
  {
    TIMING_START(mpr_arrange);
    arranger->arrange();
    TIMING_EPR(mpr_arrange, "arrange time\t\t");
    listofroots= arranger->listOfRoots( gmp_output_digits );
  }
  else
  {
    WerrorS("Solver was unable to find any roots!");
    return TRUE;
  }

  // free everything
  count= iproots[0]->getAnzElems();
  for (i=0; i < count; i++) delete iproots[i];
  Free( (ADDRESS) iproots, count * sizeof(rootContainer*) );
  count= muiproots[0]->getAnzElems();
  for (i=0; i < count; i++) delete muiproots[i];
  Free( (ADDRESS) muiproots, count * sizeof(rootContainer*) );

  delete ures;
  delete arranger;
  nDelete( &smv );

  res->data= (void *)listofroots;

  //emptylist->Clean();
  //  FreeSizeOf( (ADDRESS) emptylist, slists );

  TIMING_EPR(mpr_overall,"overall time\t\t")

  return FALSE;
}
//<-

//-> BOOLEAN nuMPResMat( leftv res, leftv arg1, leftv arg2 )
BOOLEAN nuMPResMat( leftv res, leftv arg1, leftv arg2 )
{
  ideal gls;
  gls= (ideal)(arg1->Data());
  int imtype= (int)arg2->Data();

  uResultant::resMatType mtype= determineMType( imtype );

  // check input ideal ( = polynomial system )
  if ( mprIdealCheck( gls, arg1->Name(), mtype, true ) != mprOk )
  {
    return TRUE;
  }

  uResultant *resMat= new uResultant( gls, mtype, false );

  res->rtyp = MODUL_CMD;
  res->data= (void*)resMat->accessResMat()->getMatrix();

  delete resMat;

  return FALSE;
}
//<-

//-> BOOLEAN nuLagSolve( leftv res, leftv arg1, leftv arg2, leftv arg3 )
BOOLEAN nuLagSolve( leftv res, leftv arg1, leftv arg2, leftv arg3 )
{

  poly gls;
  gls= (poly)(arg1->Data());
  int howclean= (int)arg3->Data();

  if ( !(rField_is_R()||rField_is_long_R()||rField_is_long_C()) ) 
  {
    setGMPFloatDigits( (unsigned long int)arg2->Data() );
  }

  int deg= pTotaldegree( gls );
  //  int deg= pDeg( gls );
  int len= pLength( gls );
  int i,vpos;
  poly piter;
  lists elist;
  lists rlist;

  elist= (lists)AllocSizeOf( slists );
  elist->Init( 0 );

  if ( !(rField_is_R() ||
         rField_is_Q() ||
         rField_is_long_R() ||
         rField_is_long_C()) )
         {
    WerrorS("Ground field not implemented!");
    return TRUE;
  }

  if ( pVariables > 1 )
  {
    piter= gls;
    for ( i= 1; i <= pVariables; i++ )
      if ( pGetExp( piter, i ) )
      {
        vpos= i;
        break;
      }
    while ( piter )
    {
      for ( i= 1; i <= pVariables; i++ )
        if ( (vpos != i) && (pGetExp( piter, i ) != 0) )
        {
          Werror("The polynomial %s must be univariate!",arg1->Name());
          return TRUE;
        }
      pIter( piter );
    }
  }

  rootContainer * roots= new rootContainer();
  number * pcoeffs= (number *)Alloc( (deg+1) * sizeof( number ) );
  piter= gls;
  for ( i= deg; i >= 0; i-- )
  {
    //if ( piter ) Print("deg %d, pDeg(piter) %d\n",i,pTotaldegree(piter));
    if ( piter && pTotaldegree(piter) == i )
    {
      pcoeffs[i]= nCopy( pGetCoeff( piter ) );
      //nPrint( pcoeffs[i] );Print("  ");
      pIter( piter );
    }
    else
    {
      pcoeffs[i]= nInit(0);
    }
  }

#ifdef mprDEBUG_PROT
  for (i=deg; i >= 0; i--)
  {
    nPrint( pcoeffs[i] );Print("  ");
  }
  PrintLn();
#endif

  TIMING_START(mpr_solver);
  roots->fillContainer( pcoeffs, NULL, 1, deg, rootContainer::onepoly, 1 );
  roots->solver( howclean );
  TIMING_EPR(mpr_solver, "solver time\t\t");

  int elem= roots->getAnzRoots();
  char *out;
  char *dummy;
  int j;

  rlist= (lists)AllocSizeOf( slists );
  rlist->Init( elem );

  if (rField_is_long_C())
  {
    for ( j= 0; j < elem; j++ )
    {
      rlist->m[j].rtyp=NUMBER_CMD;
      rlist->m[j].data=(void *)nCopy((number)(roots->getRoot(j)));
      //rlist->m[j].data=(void *)(number)(roots->getRoot(j));
    }
  }
  else
  {
    for ( j= 0; j < elem; j++ )
    {
      dummy = complexToStr( (*roots)[j], gmp_output_digits );
      rlist->m[j].rtyp=STRING_CMD;
      rlist->m[j].data=(void *)dummy;
    }
  }

  elist->Clean();
  //FreeSizeOf( (ADDRESS) elist, slists );

  for ( i= deg; i >= 0; i-- ) nDelete( &pcoeffs[i] );
  Free( (ADDRESS) pcoeffs, (deg+1) * sizeof( number ) );

  res->rtyp= LIST_CMD;
  res->data= (void*)rlist;

  return FALSE;
}
//<-

//-> BOOLEAN nuVanderSys( leftv res, leftv arg1, leftv arg2 )
BOOLEAN nuVanderSys( leftv res, leftv arg1, leftv arg2, leftv arg3)
{
  int i;
  ideal p,w;
  p= (ideal)arg1->Data();
  w= (ideal)arg2->Data();

  // w[0] = f(p^0)
  // w[1] = f(p^1)
  // ...
  // p can be a vector of numbers (multivariate polynom)
  //   or one number (univariate polynom)
  // tdg = deg(f)

  int n= IDELEMS( p );
  int m= IDELEMS( w );
  int tdg= (int)arg3->Data();

  res->data= (void*)NULL;

  // check the input
  if ( tdg < 1 )
  {
    WerrorS("Last input parameter must be > 0!");
    return TRUE;
  }
  if ( n != pVariables )
  {
    Werror("Size of first input ideal must be equal to %d!",pVariables);
    return TRUE;
  }
  if ( m != (int)pow((double)tdg+1,(int)n) )
  {
    Werror("Size of second input ideal must be equal to %d!",
      (int)pow((double)tdg+1,(int)n));
    return TRUE;
  }
  if ( !(rField_is_Q() /* ||
         rField_is_R() || rField_is_long_R() ||
         rField_is_long_C()*/ ) )
         {
    WerrorS("Ground field not implemented!");
    return TRUE;
  }

  number tmp;
  number *pevpoint= (number *)Alloc( n * sizeof( number ) );
  for ( i= 0; i < n; i++ )
  {
    pevpoint[i]=nInit(0);
    if (  (p->m)[i] )
    {
      tmp = pGetCoeff( (p->m)[i] );
      if ( nIsZero(tmp) || nIsOne(tmp) || nIsMOne(tmp) )
      {
        Free( (ADDRESS)pevpoint, n * sizeof( number ) );
        WerrorS("Elements of first input ideal must not be equal to -1, 0, 1!");
        return TRUE;
      }
    } else tmp= NULL;
    if ( !nIsZero(tmp) )
    {
      if ( !pIsConstant((p->m)[i]))
      {
        Free( (ADDRESS)pevpoint, n * sizeof( number ) );
        WerrorS("Elements of first input ideal must be numbers!");
        return TRUE;
      }
      pevpoint[i]= nCopy( tmp );
    }
  }

  number *wresults= (number *)Alloc( m * sizeof( number ) );
  for ( i= 0; i < m; i++ )
  {
    wresults[i]= nInit(0);
    if ( (w->m)[i] && !nIsZero(pGetCoeff((w->m)[i])) )
    {
      if ( !pIsConstant((w->m)[i]))
      {
        Free( (ADDRESS)pevpoint, n * sizeof( number ) );
        Free( (ADDRESS)wresults, m * sizeof( number ) );
        WerrorS("Elements of second input ideal must be numbers!");
        return TRUE;
      }
      wresults[i]= nCopy(pGetCoeff((w->m)[i]));
    }
  }

  vandermonde vm( m, n, tdg, pevpoint, FALSE );
  number *ncpoly= vm.interpolateDense( wresults );
  // do not free ncpoly[]!!
  poly rpoly= vm.numvec2poly( ncpoly );

  Free( (ADDRESS)pevpoint, n * sizeof( number ) );
  Free( (ADDRESS)wresults, m * sizeof( number ) );

  res->data= (void*)rpoly;
  return FALSE;
}
//<-

//-> function u_resultant_det
poly u_resultant_det( ideal gls, int imtype )
{
  uResultant::resMatType mtype= determineMType( imtype );
  poly resdet;
  poly emptypoly= pInit();
  number smv= NULL;

  TIMING_START(mpr_overall);

  // check input ideal ( = polynomial system )
  if ( mprIdealCheck( gls, "", mtype ) != mprOk )
  {
    return emptypoly;
  }

  uResultant *ures;

  // main task 1: setup of resultant matrix
  TIMING_START(mpr_constr);
  ures= new uResultant( gls, mtype );
  TIMING_EPR(mpr_constr,"construction");

  // if dense resultant, check if minor nonsingular
  if ( mtype == uResultant::denseResMat )
  {
    smv= ures->accessResMat()->getSubDet();
#ifdef mprDEBUG_PROT
    Print("// Determinant of submatrix: ");nPrint(smv); PrintLn();
#endif
    if ( nIsZero(smv) )
    {
      WerrorS("Unsuitable input ideal: Minor of resultant matrix is singular!");
      return emptypoly;
    }
  }

  // main task 2: Interpolate resultant polynomial
  TIMING_START(mpr_ures);
  resdet= ures->interpolateDense( smv );
  TIMING_EPR(mpr_ures,"ures");

  // free mem
  delete ures;
  nDelete( &smv );
  pDelete( &emptypoly );

  TIMING_EPR(mpr_overall,"overall");

  return ( resdet );
}
//<-

//-----------------------------------------------------------------------------

//#endif // HAVE_MPR

// local Variables: ***
// folded-file: t ***
// compile-command-1: "make installg" ***
// compile-command-2: "make install" ***
// End: ***

// in folding: C-c x
// leave fold: C-c y
//   foldmode: F10










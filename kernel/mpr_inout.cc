/****************************************
*  Computer Algebra System SINGULAR     *
****************************************/

/*
* ABSTRACT - multipolynomial resultant
*/

#include <kernel/mod2.h>
//#ifdef HAVE_MPR

//-> includes
#include <kernel/structs.h>
#include <kernel/polys.h>
#include <kernel/ideals.h>
#include <kernel/ring.h>
#include <kernel/febase.h>
#include <omalloc/omalloc.h>
#include <kernel/numbers.h>
#include <kernel/matpol.h>

#include <math.h>

#include <kernel/mpr_global.h>
#include <kernel/mpr_inout.h>
#include <kernel/mpr_base.h>
#include <kernel/mpr_numeric.h>

// to get detailed timigs, define MPR_TIMING
#ifdef MPR_TIMING
#define TIMING
#endif
#include <factory/timing.h>
TIMING_DEFINE_PRINT(mpr_overall)
TIMING_DEFINE_PRINT(mpr_check)
TIMING_DEFINE_PRINT(mpr_constr)
TIMING_DEFINE_PRINT(mpr_ures)
TIMING_DEFINE_PRINT(mpr_mures)
TIMING_DEFINE_PRINT(mpr_arrange)
TIMING_DEFINE_PRINT(mpr_solver)

#define TIMING_EPR(t,msg) TIMING_END_AND_PRINT(t,msg);TIMING_RESET(t);

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
    WerrorS("Unknown resultant matrix type chosen!");
    break;
  case mprHasOne:
    Werror("One element of the ideal %s is constant!",name);
    break;
  case mprInfNumOfVars:
    Werror("Wrong number of elements in given ideal %s, should be %d resp. %d!",
           name,pVariables+1,pVariables);
    break;
  case mprNotZeroDim:
    Werror("The given ideal %s must be 0-dimensional!",name);
    break;
  case mprNotHomog:
    Werror("The given ideal %s has to be homogeneous in the first ring variable!",
           name);
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
                        BOOLEAN rmatrix )
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
    PrintS("// Determinant of submatrix: ");nPrint(smv); PrintLn();
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

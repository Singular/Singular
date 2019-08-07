#ifndef MPR_BASE_H
#define MPR_BASE_H
/****************************************
*  Computer Algebra System SINGULAR     *
****************************************/

/*
* ABSTRACT - multipolynomial resultants - resultant matrices
*            ( sparse, dense, u-resultant solver )
*/

#include "kernel/numeric/mpr_numeric.h"

#define SNONE -1
#define SFREE -2

//%s
//-> class resMatrixBase
/**
 * Base class for sparse and dense u-Resultant computation
 */
class resMatrixBase
{
public:
  /* state of the resultant */
  enum IStateType { none, ready, notInit, fatalError, sparseError };

  resMatrixBase() : istate(notInit), totDeg(0) {}
  virtual ~resMatrixBase() {}

  virtual ideal getMatrix() { return NULL; }
  virtual ideal getSubMatrix() { return NULL; }

  virtual poly getUDet( const number* /*evpoint*/ ) { return NULL; }

  virtual number getDetAt( const number* /*evpoint*/ ) { return NULL; }
  virtual number getSubDet() { return NULL; }

  virtual long getDetDeg() { return totDeg; }

  virtual IStateType initState() const { return istate; }

protected:
  IStateType istate;

  ideal gls;
  int linPolyS;
  ring sourceRing;

  int totDeg;

private:
  /* disables the copy constructor */
  resMatrixBase( const resMatrixBase & );
};
//<-

//-> class uResultant
/**
 * Base class for solving 0-dim poly systems using u-resultant
 */
class uResultant
{
public:
  enum resMatType { none, sparseResMat, denseResMat };

  uResultant( const ideal _gls, const resMatType _rmt= sparseResMat, BOOLEAN extIdeal= true );
  ~uResultant();

  poly interpolateDense( const number subDetVal= NULL );

  /* Interpolates n+1 determinat polys for coeff specializations. */
  rootContainer ** interpolateDenseSP( BOOLEAN matchUp= false, const number subDetVal= NULL );

  /* Uses Bareiss */
  rootContainer ** specializeInU( BOOLEAN matchUp= false, const number subDetVal= NULL );

  resMatrixBase * accessResMat() { return resMat; }

private:
  /* deactivated copy constructor */
  uResultant( const uResultant & );

  ideal extendIdeal( const ideal gls, poly linPoly, const resMatType rmt );
  poly linearPoly( const resMatType rmt );
  int nextPrime( const int p );

  ideal gls;
  int n;

  resMatType rmt;        // sparse or dense resultant matrix ?
  resMatrixBase *resMat; // pointer to base resultant matrix class
};
//<-
uResultant::resMatType determineMType( int imtype );
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

mprState mprIdealCheck( const ideal theIdeal,
                        const char * name,
                        uResultant::resMatType mtype,
                        BOOLEAN rmatrix= false );

ideal loNewtonPolytope( const ideal id );

THREAD_VAR extern size_t gmp_output_digits;
//%e
#endif /*MPR_BASE_H*/

// local Variables: ***
// folded-file: t ***
// compile-command-2: "make install" ***
// compile-command: "make installg" ***
// End: ***

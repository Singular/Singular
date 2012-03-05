/****************************************
*  Computer Algebra System SINGULAR     *
****************************************/

#include <kernel/mod2.h>
#include <kernel/structs.h>
#include <kernel/structs.h>
#include <kernel/polys.h>
#include <kernel/ideals.h>
#include <kernel/ring.h>
#include <kernel/febase.h>
#include <kernel/maps.h>
#include <omalloc/omalloc.h>
#include <kernel/kstd1.h>
#include <kernel/fglm.h>
#include <kernel/walkMain.h>
#include <kernel/walkSupport.h>
#include <kernel/walkProc.h>
#include <kernel/prCopy.h>

///////////////////////////////////////////////////////////////////
//Frame procedures for Groebner Walk and Fractal Walk
///////////////////////////////////////////////////////////////////
//v1.3 2004-11-15
///////////////////////////////////////////////////////////////////
//implemented by Henrik Strohmayer
///////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////
//walkConsistency
///////////////////////////////////////////////////////////////////
//Description:
// Checks if the two rings sringHdl and dringHdl are compatible
// enough to be used for the Walk. This means:
// 1) Same Characteristic
// 2) globalOrderings in both rings,
// 3) Same number of variables
// 4) same number of parameters
// 5) variables in one ring have the same names
//    and order as variables of the other
// 6) parameters in one ring have the same names
//    and order as parameters of the other
// 7) none of the rings are qrings
// vperm must be a vector of length pVariables+1, initialized by 0.
// If both rings are compatible, it stores the permutation of the
// variables if mapped from sringHdl to dringHdl.
// if the rings are compatible, it returns WalkOk.
// Should be called with currRing= IDRING( sringHdl );
///////////////////////////////////////////////////////////////////
//Uses: IDRING,WerrorS,rPar,omAlloc0,maFindPerm,omFreeSize,sizeof
///////////////////////////////////////////////////////////////////

WalkState
walkConsistency( ring sring, ring dring, int * vperm )
{
    int k;
    WalkState state= WalkOk;

    if ( rChar(sring) != rChar(dring) )
    {
        WerrorS( "rings must have same characteristic" );
        state= WalkIncompatibleRings;
    }
    else if ( (rHasLocalOrMixedOrdering(sring))
    || (rHasLocalOrMixedOrdering(dring)) )
    {
        WerrorS( "only works for global orderings" );
        state= WalkIncompatibleRings;
    }
    else if ( sring->N != dring->N )
    {
        WerrorS( "rings must have same number of variables" );
        state= WalkIncompatibleRings;
    }
    else if ( rPar(sring) != rPar(dring) )
    {
        WerrorS( "rings must have same number of parameters" );
        state= WalkIncompatibleRings;
    }

    if ( state != WalkOk ) return state;
    // now the rings have the same number of variables resp. parameters.
    // check if the names of the variables resp. parameters do agree:

    int nvar = rVar(sring);
    int npar = rPar(sring);
    int * pperm;
    if ( npar > 0 )
        pperm= (int *)omAlloc0( (npar+1)*sizeof( int ) );
    else
        pperm= NULL;

    maFindPerm( sring->names, nvar, sring->parameter, npar,
                dring->names, nvar, dring->parameter, npar, vperm, pperm,
                dring->ch);

    for ( k= nvar; (k > 0) && (state == WalkOk); k-- )
        if ( vperm[k] <= 0 )
        {
            WerrorS( "variable names do not agree" );
            state= WalkIncompatibleRings;
        }

    for ( k= npar-1; (k >= 0) && (state == WalkOk); k-- )
        if ( pperm[k] >= 0 )
        {
            WerrorS( "parameter names do not agree" );
            state= WalkIncompatibleRings;
        }

    //remove this to if you want to allow permutations of variables
    for ( k= nvar; (k > 0) && (state == WalkOk); k-- )
      if ( vperm[k] != (k) )
      {
        WerrorS( "orders of variables do not agree" );
        state= WalkIncompatibleRings;
      }

    //remove this to if you want to allow permutations of parameters
    for ( k= npar; (k > 0) && (state == WalkOk); k-- )
      if ( pperm[k-1] != (-k) )
      {
        WerrorS( "orders of parameters do not agree" );
        state= WalkIncompatibleRings;
      }

      if (pperm != NULL)
        omFreeSize( (ADDRESS)pperm, (npar+1)*sizeof( int ) );

    if ( state != WalkOk ) return state;

    // check if any of the rings are qrings or not
    if ( (sring->qideal != NULL) || (dring->qideal != NULL) )
    {
          WerrorS( "rings are not allowed to be qrings");
          return WalkIncompatibleRings;
    }

    int i=0;
    while(dring->order[i]!=0)
    {
      if(
           !(dring->order[i]==ringorder_a) &&
           !(dring->order[i]==ringorder_a64) &&
           !(dring->order[i]==ringorder_lp) &&
           !(dring->order[i]==ringorder_dp) &&
           !(dring->order[i]==ringorder_Dp) &&
           !(dring->order[i]==ringorder_wp) &&
           !(dring->order[i]==ringorder_Wp) &&
           !(dring->order[i]==ringorder_C)  &&
           !(dring->order[i]==ringorder_M)
         )
      {
        state=WalkIncompatibleDestRing;
      }
      i++;
    }

    i=0;
    while(sring->order[i]!=0)
    {
      if(
           !(sring->order[i]==ringorder_a) &&
           !(sring->order[i]==ringorder_a64) &&
           !(sring->order[i]==ringorder_lp) &&
           !(sring->order[i]==ringorder_dp) &&
           !(sring->order[i]==ringorder_Dp) &&
           !(sring->order[i]==ringorder_wp) &&
           !(sring->order[i]==ringorder_Wp) &&
           !(sring->order[i]==ringorder_C)  &&
           !(sring->order[i]==ringorder_M)
         )
      {
       state=WalkIncompatibleSourceRing;
      }
      i++;
    }

    return state;
}

///////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////
//fractalWalkConsistency
///////////////////////////////////////////////////////////////////
//Description:
// Checks if the two rings sringHdl and dringHdl are compatible
// enough to be used for the Walk. This means:
// 1) Same Characteristic
// 2) globalOrderings in both rings,
// 3) Same number of variables
// 4) same number of parameters
// 5) variables in one ring have the same names
//    and order as variables of the other
// 6) parameters in one ring have the same names
//    and order as parameters of the other
// 7) none of the rings are qrings
// vperm must be a vector of length pVariables+1, initialized by 0.
// If both rings are compatible, it stores the permutation of the
// variables if mapped from sringHdl to dringHdl.
// if the rings are compatible, it returns WalkOk.
// Should be called with currRing= IDRING( sringHdl );
///////////////////////////////////////////////////////////////////
//Uses: IDRING,WerrorS,rPar,omAlloc0,maFindPerm,omFreeSize,sizeof
///////////////////////////////////////////////////////////////////

WalkState
fractalWalkConsistency( ring sring, ring dring, int * vperm )
{
    int k;
    WalkState state= WalkOk;

    if ( rChar(sring) != rChar(dring) )
    {
        WerrorS( "rings must have same characteristic" );
        state= WalkIncompatibleRings;
    }

    if ( (rHasLocalOrMixedOrdering(sring))
    || (rHasLocalOrMixedOrdering(dring)) )
    {
        WerrorS( "only works for global orderings" );
        state= WalkIncompatibleRings;
    }

    if ( rVar(sring) != rVar(dring) )
    {
        WerrorS( "rings must have same number of variables" );
        state= WalkIncompatibleRings;
    }

    if ( rPar(sring) != rPar(dring) )
    {
        WerrorS( "rings must have same number of parameters" );
        state= WalkIncompatibleRings;
    }

    if ( state != WalkOk ) return state;

    // now the rings have the same number of variables resp. parameters.
    // check if the names of the variables resp. parameters do agree:
    int nvar = sring->N;
    int npar = rPar(sring);
    int * pperm;

    if ( npar > 0 )
        pperm= (int *)omAlloc0( (npar+1)*sizeof( int ) );
    else
        pperm= NULL;

    maFindPerm( sring->names, nvar, sring->parameter, npar,
                dring->names, nvar, dring->parameter, npar, vperm, pperm,
                dring->ch);

    for ( k= nvar; (k > 0) && (state == WalkOk); k-- )
      if ( vperm[k] <= 0 )
      {
        WerrorS( "variable names do not agree" );
        state= WalkIncompatibleRings;
      }

    for ( k= npar; (k > 0) && (state == WalkOk); k-- )
      if ( pperm[k-1] >= 0 )
      {
        WerrorS( "parameter names do not agree" );
        state= WalkIncompatibleRings;
      }

    //check if order of variables resp. parameters does agree
    //remove this to if you want to allow permutations of variables
    for ( k= nvar; (k > 0) && (state == WalkOk); k-- )
      if ( vperm[k] != (k) )
      {
        WerrorS( "orders of variables do not agree" );
        state= WalkIncompatibleRings;
      }

    //remove this to if you want to allow permutations of parameters
    for ( k= npar; (k > 0) && (state == WalkOk); k-- )
      if ( pperm[k-1] != (-k) )
      {
        WerrorS( "orders of parameters do not agree" );
        state= WalkIncompatibleRings;
      }

    if (pperm != NULL)
      omFreeSize( (ADDRESS)pperm, (npar+1)*sizeof( int ) );

    if ( state != WalkOk ) return state;

    // check if any of the rings are qrings or not
    if ( (sring->qideal != NULL) || (dring->qideal != NULL) )
    {
          WerrorS( "rings are not allowed to be qrings");
          return WalkIncompatibleRings;
    }

    int i=0;
    while(dring->order[i]!=0){
      if(  !(dring->order[i]==ringorder_lp) &&
           !(dring->order[i]==ringorder_dp) &&
           !(dring->order[i]==ringorder_Dp) &&
           !(dring->order[i]==ringorder_wp) &&
           !(dring->order[i]==ringorder_Wp) &&
           !(dring->order[i]==ringorder_C)  &&
           !(dring->order[0]==ringorder_M)
         )
      {
        state=WalkIncompatibleDestRing;
      }
      i++;
    }

    i=0;
    while(sring->order[i]!=0)
    {
      if(  !(sring->order[i]==ringorder_lp) &&
           !(sring->order[i]==ringorder_dp) &&
           !(sring->order[i]==ringorder_Dp) &&
           !(sring->order[i]==ringorder_wp) &&
           !(sring->order[i]==ringorder_Wp) &&
           !(sring->order[i]==ringorder_C)  &&
           !(dring->order[0]==ringorder_M)
         )
      {
        state=WalkIncompatibleSourceRing;
      }
      i++;
    }

    return state;
}

///////////////////////////////////////////////////////////////////



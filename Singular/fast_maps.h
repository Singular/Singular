/****************************************
*  Computer Algebra System SINGULAR     *
****************************************/
/***************************************************************
 *  File:    fast_maps.cc
 *  Purpose: implementation of fast maps
 *  Author:  obachman (Olaf Bachmann), hannes (Hannes Schoenemann),
 *           bricken (Michael Brickenstein) 
 *  Created: 01/02
 *  Version: $Id: fast_maps.h,v 1.4 2002-01-19 10:40:15 Singular Exp $
 *******************************************************************/

/*******************************************************************************
**
*S  mapoly, macoeff . . . . . . . . . . . . definition of structs/classes
*/
class macoeff_s;
class mapoly_s;
class maideal_s;
typedef class mapoly_s*  mapoly;
typedef class macoeff_s* macoeff;
typedef class maideal_s* maideal;

class mapoly_s
{
public:
  mapoly    next;
  poly      src;        // monomial from WeightedRing
  poly      dest;       // poly in CompRing
  mapoly    f1, f2;     // if f1 != NULL && f2 != NULL then dest = f1*f2
  int       ref;        // use to catch last usage to save last copy
  macoeff   coeff;      // list of coeffs to use
};

class macoeff_s 
{
public:
  macoeff       next;
  number        n;
  sBucket_pt    bucket;
};

class maideal_s
{
public:
  int n;
  sBucket_pt* buckets;
};

/*******************************************************************************
**
*S  definition of basic routines
*/
void maMonomial_Out(mapoly monomial, ring src_r, ring dest_r);
void maPoly_Out(mapoly mpoly, ring src_ring, ring dest_r);

// creates a new maMonomial
// if bucket != NULL, a coeff with the bucket is created, as well
mapoly maMonomial_Create(poly p, ring , sBucket_pt bucket);
// unconditionally destroys a maMonomial:
// src: LmFree 
// dest: p_Delete 
// coeffs: delete list
void maMonomial_Destroy(mapoly monomial, ring src_r, ring dest_r);
// decrements ref counter, if 0, calls Destroy
inline mapoly maMonomial_Free(mapoly monomial, ring src_r, ring dest_r)
{
  monomial->ref--;
  if (monomial->ref <= 0) 
  { maMonomial_Destroy(monomial, src_r, dest_r); return NULL;}
  return monomial;
}

// inserts ("adds") monomial what into poly into
mapoly maPoly_InsertMonomial(mapoly &into, mapoly what, ring src_r);

// optimizes mpoly for later evaluation
mapoly maPoly_Optimize(mapoly mpoly, ring src_r);

// evaluates mpoly and destroys it, on the fly
mapoly maPoly_Eval(mapoly mpoly, ring src_r, poly* dest_id, ring dest_r);





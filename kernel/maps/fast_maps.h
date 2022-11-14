/****************************************
*  Computer Algebra System SINGULAR     *
****************************************/
/***************************************************************
 *  File:    fast_maps.cc
 *  Purpose: implementation of fast maps
 *  Author:  obachman (Olaf Bachmann), hannes (Hannes Schoenemann),
 *           bricken (Michael Brickenstein)
 *  Created: 01/02
 *******************************************************************/

/*******************************************************************************
**
*S  mapoly, macoeff . . . . . . . . . . . . definition of structs/classes
*/
#ifndef FAST_MAPS_HEADER
#define FAST_MAPS_HEADER
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
void maMonomial_Out(mapoly monomial, ring src_r, ring dest_r = NULL);
void maPoly_Out(mapoly mpoly, ring src_ring, ring dest_r = NULL);

// creates a new maMonomial
// if bucket != NULL, a coeff with the bucket is created, as well
mapoly maMonomial_Create(poly p, ring , sBucket_pt bucket = NULL);
// unconditionally destroys a maMonomial:
// src: LmFree
// dest: p_Delete
// coeffs: delete list
void maMonomial_Destroy(mapoly monomial, ring src_r, ring dest_r = NULL);
// decrements ref counter, if 0, calls Destroy
inline mapoly maMonomial_Free(mapoly monomial, ring src_r, ring dest_r = NULL)
{
  monomial->ref--;
  if (monomial->ref <= 0)
  { maMonomial_Destroy(monomial, src_r, dest_r); return NULL;}
  return monomial;
}

// inserts ("adds") monomial what into poly into
// returns the maMonomial which was inserted, or, if an equal one was found,
// the monomial which "swallowed" the monomial
// It furthermore might reset into
mapoly maPoly_InsertMonomial(mapoly &into, mapoly what, ring src_r);
mapoly maPoly_InsertMonomial(mapoly &into, poly p, ring src_r, sBucket_pt bucket = NULL);

// optimizes mpoly for later evaluation
void maPoly_Optimize(mapoly mpoly, ring src_r);

// evaluates mpoly and destroys it, on the fly
void maPoly_Eval(mapoly mpoly, ring src_r, ideal dest_id, ring dest_r, int total_cost);

// creates mpoly and  mideal
void maMap_CreatePolyIdeal(ideal map_id, ring map_r,
                           ring src_r, ring dest_r,
                           mapoly &mp, maideal &mideal);
// creates src_r: rings with weights
//         dest_r: where we do our computations
void maMap_CreateRings(ideal map_id, ring map_r,
                       ideal image_id, ring image_r,
                       ring &src_r, ring &dest_r, BOOLEAN &no_sort);

// collects the results into an ideal and destroys maideal
ideal maIdeal_2_Ideal(maideal ideal, ring dest_r);

// main routine: map_id: the ideal to map
//               map_r: the base ring for map_id
//               image_id: the image of the variables
ideal fast_map_common_subexp(const ideal map_id,const ring map_r,const ideal image_id,const ring image_r);

#endif



#include <tropicalStrategy.h>
#include <adjustWeights.h>
#include <ppinitialReduction.h>
#include <ttinitialReduction.h>
#include <tropical.h>
#include <std_wrapper.h>

// for various commands in dim(ideal I, ring r):
#include <kernel/ideals.h>
#include <kernel/GBEngine/stairc.h>
#include <Singular/ipshell.h> // for isPrime(int i)

/***
 * Computes the dimension of an ideal I in ring r
 * Copied from jjDim in iparith.cc
 **/
int dim(ideal I, ring r)
{
  ring origin = currRing;
  if (origin != r)
    rChangeCurrRing(r);
  int d;
  if (rField_is_Ring(currRing))
  {
    int i = idPosConstant(I);
    if ((i != -1) && (n_IsUnit(p_GetCoeff(I->m[i],currRing->cf),currRing->cf)))
      return -1;
    ideal vv = id_Head(I,currRing);
    if (i != -1) pDelete(&vv->m[i]);
    d = scDimInt(vv, currRing->qideal);
    if (rField_is_Ring_Z(currRing) && (i==-1)) d++;
    idDelete(&vv);
    return d;
  }
  else
    d = scDimInt(I,currRing->qideal);
  if (origin != r)
    rChangeCurrRing(origin);
  return d;
}

static bool noExtraReduction(ideal /*I*/, ring /*r*/, number /*p*/)
{
  return false;
}

/**
 * Initializes all relevant structures and information for the trivial valuation case,
 * i.e. computing a tropical variety without any valuation.
 *g
 */
tropicalStrategy::tropicalStrategy(const ideal I, const ring r,
                                   const bool completelyHomogeneous,
                                   const bool completeSpace):
  originalRing(rCopy(r)),
  originalIdeal(id_Copy(I,r)),
  expectedDimension(dim(originalIdeal,originalRing)),
  linealitySpace(homogeneitySpace(originalIdeal,originalRing)),
  startingRing(rCopy(originalRing)),
  startingIdeal(id_Copy(originalIdeal,originalRing)),
  uniformizingParameter(NULL),
  shortcutRing(NULL),
  onlyLowerHalfSpace(false),
  weightAdjustingAlgorithm1(nonvalued_adjustWeightForHomogeneity),
  weightAdjustingAlgorithm2(nonvalued_adjustWeightUnderHomogeneity),
  extraReductionAlgorithm(noExtraReduction)
{
  assume(rField_is_Q(r) || rField_is_Zp(r));
  startingIdeal = gfanlib_kStd_wrapper(startingIdeal,startingRing);
  if (!completelyHomogeneous)
  {
    weightAdjustingAlgorithm1 = valued_adjustWeightForHomogeneity;
    weightAdjustingAlgorithm2 = valued_adjustWeightUnderHomogeneity;
  }
  if (!completeSpace)
    onlyLowerHalfSpace = true;
}

/**
 * Given a polynomial ring r over the rational numbers and a weighted ordering,
 * returns a polynomial ring s over the integers with one extra variable, which is weighted -1.
 */
static ring constructStartingRing(ring r)
{
  assume(rField_is_Q(r));

  ring s = rCopy0(r,FALSE,FALSE);
  nKillChar(s->cf);
  s->cf = nInitChar(n_Z,NULL);

  int n = rVar(s)+1;
  s->N = n;
  char** oldNames = s->names;
  s->names = (char**) omAlloc((n+1)*sizeof(char**));
  s->names[0] = omStrDup("t");
  for (int i=1; i<n; i++)
    s->names[i] = oldNames[i-1];
  omFree(oldNames);

  s->order = (int*) omAlloc0(3*sizeof(int));
  s->block0 = (int*) omAlloc0(3*sizeof(int));
  s->block1 = (int*) omAlloc0(3*sizeof(int));
  s->wvhdl = (int**) omAlloc0(3*sizeof(int**));
  s->order[0] = ringorder_ws;
  s->block0[0] = 1;
  s->block1[0] = n;
  s->wvhdl[0] = (int*) omAlloc(n*sizeof(int));
  s->wvhdl[0][0] = 1;
  for (int i=1; i<n; i++)
    s->wvhdl[0][i] = -(r->wvhdl[0][i-1]);
  s->order[1] = ringorder_C;

  rComplete(s);
  rTest(s);
  return s;
}

static ring writeOrderingAsWP(ring r)
{
  assume(r->order[0]==ringorder_wp || r->order[0]==ringorder_dp);
  if (r->order[0]==ringorder_dp)
  {
    ring s = rCopy0(r,FALSE,TRUE);
    rComplete(s);
    rTest(s);
    return s;
  }
  return rCopy(r);
}

/***
 * Initializes all relevant structures and information for the valued case,
 * i.e. computing a tropical variety over the rational numbers with p-adic valuation
 **/
tropicalStrategy::tropicalStrategy(ideal J, number q, ring s):
  originalRing(rCopy(s)),
  originalIdeal(id_Copy(J,s)),
  expectedDimension(dim(originalIdeal,originalRing)+1),
  linealitySpace(gfan::ZCone()), // to come, see below
  startingRing(NULL),            // to come, see below
  startingIdeal(NULL),           // to come, see below
  uniformizingParameter(NULL),   // to come, see below
  shortcutRing(NULL),            // to come, see below
  onlyLowerHalfSpace(true),
  weightAdjustingAlgorithm1(valued_adjustWeightForHomogeneity),
  weightAdjustingAlgorithm2(valued_adjustWeightUnderHomogeneity),
  extraReductionAlgorithm(ppreduceInitially)
{
  /* assume that the ground field of the originalRing is Q */
  assume(rField_is_Q(s));
  originalRing = rCopy(s);

  /* replace Q with Z for the startingRing
   * and add an extra variable for tracking the uniformizing parameter */
  startingRing = constructStartingRing(originalRing);

  /* map the uniformizing parameter into the new coefficient domain */
  nMapFunc nMap = n_SetMap(originalRing->cf,startingRing->cf);
  uniformizingParameter = nMap(q,originalRing->cf,startingRing->cf);

  /* map the input ideal into the new polynomial ring */
  int k = idSize(J);
  startingIdeal = idInit(k+1);
  poly g = p_One(startingRing);
  p_SetCoeff(g,uniformizingParameter,startingRing);
  pNext(g) = p_One(startingRing);
  p_SetExp(pNext(g),1,1,startingRing);
  p_SetCoeff(pNext(g),n_Init(-1,startingRing->cf),startingRing);
  p_Setm(pNext(g),startingRing);
  startingIdeal->m[0] = g;
  int n = rVar(originalRing);
  int* shiftByOne = (int*) omAlloc((n+1)*sizeof(int));
  for (int i=1; i<=n; i++)
    shiftByOne[i]=i+1;
  for (int i=0; i<k; i++)
    startingIdeal->m[i+1] = p_PermPoly(J->m[i],shiftByOne,originalRing,startingRing,nMap,NULL,0);
  omFreeSize(shiftByOne,(n+1)*sizeof(int));
  startingIdeal = gfanlib_kStd_wrapper(startingIdeal,startingRing);

  linealitySpace = homogeneitySpace(startingIdeal,startingRing);

  /* construct the shorcut ring */
  shortcutRing = rCopy0(startingRing);
  nKillChar(shortcutRing->cf);
  shortcutRing->cf = nInitChar(n_Zp,(void*)(long)IsPrime(n_Int(uniformizingParameter,startingRing->cf)));
  rComplete(shortcutRing);
  rTest(shortcutRing);
}

tropicalStrategy::tropicalStrategy(const tropicalStrategy& currentStrategy):
  originalRing(rCopy(currentStrategy.getOriginalRing())),
  originalIdeal(id_Copy(currentStrategy.getOriginalIdeal(),currentStrategy.getOriginalRing())),
  expectedDimension(currentStrategy.getExpectedDimension()),
  linealitySpace(currentStrategy.getHomogeneitySpace()),
  startingRing(rCopy(currentStrategy.getStartingRing())),
  startingIdeal(id_Copy(currentStrategy.getStartingIdeal(),currentStrategy.getStartingRing())),
  uniformizingParameter(n_Copy(currentStrategy.getUniformizingParameter(),startingRing->cf)),
  shortcutRing(rCopy(currentStrategy.getShortcutRing())),
  onlyLowerHalfSpace(currentStrategy.restrictToLowerHalfSpace()),
  weightAdjustingAlgorithm1(currentStrategy.weightAdjustingAlgorithm1),
  weightAdjustingAlgorithm2(currentStrategy.weightAdjustingAlgorithm2),
  extraReductionAlgorithm(currentStrategy.extraReductionAlgorithm)
{
  if (originalRing) rTest(originalRing);
  if (originalIdeal) id_Test(originalIdeal,originalRing);
  if (startingRing) rTest(startingRing);
  if (startingIdeal) id_Test(startingIdeal,startingRing);
  if (uniformizingParameter) n_Test(uniformizingParameter,startingRing->cf);
  if (shortcutRing) rTest(shortcutRing);
}

tropicalStrategy::~tropicalStrategy()
{
  if (originalRing) rTest(originalRing);
  if (originalIdeal) id_Test(originalIdeal,originalRing);
  if (startingRing) rTest(startingRing);
  if (startingIdeal) id_Test(startingIdeal,startingRing);
  if (uniformizingParameter) n_Test(uniformizingParameter,startingRing->cf);
  if (shortcutRing) rTest(shortcutRing);

  id_Delete(&originalIdeal,originalRing);
  rDelete(originalRing);
  id_Delete(&startingIdeal,startingRing);
  n_Delete(&uniformizingParameter,startingRing->cf);
  rDelete(startingRing);
  rDelete(shortcutRing);
}

tropicalStrategy& tropicalStrategy::operator=(const tropicalStrategy& currentStrategy)
{
  originalRing = rCopy(currentStrategy.getOriginalRing());
  originalIdeal = id_Copy(currentStrategy.getOriginalIdeal(),currentStrategy.getOriginalRing());
  expectedDimension = currentStrategy.getExpectedDimension();
  startingRing = rCopy(currentStrategy.getStartingRing());
  startingIdeal = id_Copy(currentStrategy.getStartingIdeal(),currentStrategy.getStartingRing());
  uniformizingParameter = n_Copy(currentStrategy.getUniformizingParameter(),startingRing->cf);
  shortcutRing = rCopy(currentStrategy.getShortcutRing());
  onlyLowerHalfSpace = currentStrategy.restrictToLowerHalfSpace();
  weightAdjustingAlgorithm1 = currentStrategy.weightAdjustingAlgorithm1;
  weightAdjustingAlgorithm2 = currentStrategy.weightAdjustingAlgorithm2;
  extraReductionAlgorithm = currentStrategy.extraReductionAlgorithm;

  if (originalRing) rTest(originalRing);
  if (originalIdeal) id_Test(originalIdeal,originalRing);
  if (startingRing) rTest(startingRing);
  if (startingIdeal) id_Test(startingIdeal,startingRing);
  if (uniformizingParameter) n_Test(uniformizingParameter,startingRing->cf);
  if (shortcutRing) rTest(shortcutRing);

  return *this;
}

bool tropicalStrategy::reduce(ideal I, const ring r) const
{
  rTest(r);
  id_Test(I,r);
  nMapFunc nMap = n_SetMap(startingRing->cf,r->cf);
  number p = nMap(uniformizingParameter,startingRing->cf,r->cf);
  bool b = this->extraReductionAlgorithm(I,r,p);
  n_Delete(&p,r->cf);
  return b;
}

ring tropicalStrategy::getShortcutRingPrependingWeight(const ring r, const gfan::ZVector v) const
{
  ring rShortcut = rCopy0(r);

  // save old ordering
  int* order = rShortcut->order;
  int* block0 = rShortcut->block0;
  int* block1 = rShortcut->block1;
  int** wvhdl = rShortcut->wvhdl;

  // adjust weight and create new ordering
  gfan::ZVector w = adjustWeightForHomogeneity(v);
  int h = rBlocks(r); int n = rVar(r);
  rShortcut->order = (int*) omAlloc0((h+1)*sizeof(int));
  rShortcut->block0 = (int*) omAlloc0((h+1)*sizeof(int));
  rShortcut->block1 = (int*) omAlloc0((h+1)*sizeof(int));
  rShortcut->wvhdl = (int**) omAlloc0((h+1)*sizeof(int*));
  rShortcut->order[0] = ringorder_a;
  rShortcut->block0[0] = 1;
  rShortcut->block1[0] = n;
  bool overflow;
  rShortcut->wvhdl[0] = ZVectorToIntStar(w,overflow);
  for (int i=1; i<=h; i++)
  {
    rShortcut->order[i] = order[i-1];
    rShortcut->block0[i] = block0[i-1];
    rShortcut->block1[i] = block1[i-1];
    rShortcut->wvhdl[i] = wvhdl[i-1];
  }

  // if valuation non-trivial, change coefficient ring to residue field
  if (valuationIsNonTrivial())
  {
    nKillChar(rShortcut->cf);
    rShortcut->cf = nCopyCoeff(shortcutRing->cf);
  }
  rComplete(rShortcut);
  rTest(rShortcut);

  // delete old ordering
  omFree(order);
  omFree(block0);
  omFree(block1);
  omFree(wvhdl);

  return rShortcut;
}

poly tropicalStrategy::checkInitialIdealForMonomial(const ideal I, const ring r, const gfan::ZVector w) const
{
  // prepend extra weight for homogeneity
  // switch to residue field if valuation is non trivial
  ring rShortcut = getShortcutRingPrependingWeight(r,w);

  // compute the initial ideal and map it into the constructed ring
  // if switched to residue field, remove possibly 0 elements
  ideal inI = initial(I,r,w);
  int k = idSize(inI);
  ideal inIShortcut = idInit(k);
  nMapFunc intoShortcut = n_SetMap(r->cf,rShortcut->cf);
  for (int i=0; i<k; i++)
    inIShortcut->m[i] = p_PermPoly(inI->m[i],NULL,r,rShortcut,intoShortcut,NULL,0);
  if (isValuationNonTrivial())
    idSkipZeroes(inIShortcut);

  // check initial ideal for monomial and
  // if it exsists, return a copy of the monomial in the input ring
  poly p = checkForMonomialViaSuddenSaturation(inIShortcut,rShortcut);
  poly monomial = NULL;
  if (p!=NULL)
  {
    monomial=p_One(r);
    for (int i=1; i<n; i++)
      p_SetExp(monomial,i,p_GetExp(p,i,rShortcut),r);
    p_Delete(&p,rShortcut);
  }
  id_Delete(&inI,r);
  id_Delete(&inIShortcut,rShortcut);
  rDelete(rShortcut);
  return monomial;
}

ring tropicalStrategy::copyAndChangeCoefficientRing(const ring r) const
{
  ring rShortcut = rCopy0(r);
  nKillChar(rShortcut->cf);
  rShortcut->cf = nCopyCoeff(shortcutRing->cf);
  rComplete(rShortcut);
  rTest(rShortcut);
  return rShortcut;
}

ideal getWitness(const ideal inJ, const ideal inI, const ideal I, const ring r) const
{
  // if the valuation is trivial and the ring and ideal have not been extended,
  // then it is sufficient to return the difference between the elements of inJ
  // and their normal forms with respect to I and r
  if (isValuationTrivial())
    return witness(inJ,I,r);
  // if the valuation is non-trivial and the ring and ideal have been extended,
  // then we can make a shortcut through the residue field
  else
  {
    checkFirstGenerator(I,r);
    assume(idSize(inI)==idSize(I));
    /**
     * change ground ring into finite field
     * and map the data into it
     */
    ring rShortcut = copyAndChangeCoefficientRing(r);

    int k = idSize(inI);
    int l = idSize(I);
    ideal inJShortcut = idInit(k);
    ideal inIShortcut = idInit(l);
    nMapFunc takingResidues = n_SetMap(r->cf,rShortcut->cf);
    for (int i=0; i<k; i++)
      inJShortcut->m[i] = p_PermPoly(inJ->m[i],NULL,r,rShortcut,takingResidues,NULL,0);
    for (int j=0; j<l; j++)
      inIShortcut->m[j] = p_PermPoly(inI->m[j],NULL,r,rShortcut,takingResidues,NULL,0);
    id_Test(inJShortcut,rShortcut);
    id_Test(inIShortcut,rShortcut);

    /**
     * Compute a division with remainder over the finite field
     * and map the result back to r
     */
    matrix QShortcut = divisionDiscardingRemainder(inJShortcut,inIShortcut,rShortcut);
    matrix Q = mpNew(k,l);
    nMapFunc takingRepresentatives = n_SetMap(rShortcut->cf,r->cf);
    for (int ij=k*l-1; ij>=0; ij--)
      Q->m[ij] = p_PermPoly(Q->m[ij],NULL,rShortcut,r,takingRepresentatives,NULL,0);

    /**
     * Compute the normal forms
     */
    ideal J = idInit(k);
    for (int j=0; j<l; j++)
    {
      poly q0 = p_Copy(inJ->m[j],r);
      for (int i=1; i<k; i++)
        q0 = p_Add_q(q0,p_Neg(p_Mult_q(p_Copy(MATELEM(Q,i,j),r),p_Copy(inI->m[i],r),r),r),r);
      J->m[j] = p_Mult_q(q0,p_Copy(J->m[0],r),r);
      q0 = NULL;
      for (int i=1; i<k; i++)
        J->m[j] = p_Add_q(I->m[j],p_Mult_q(p_Copy(MATELEM(Q,i,j),r),p_Copy(I->m[i],r),r),r);
    }

    return J;
  }
}

ideal tropicalStrategy::getStdOfInitialIdeal(const ideal inI, const ring r) const
{
  ideal inJ;
  if (isValuationTrivial())
    // if valuation trivial, then compute std as usual
    inJ = gfanlib_kStd_wrapper(inI,r);
  else
  {
    // if valuation non-trivial, then uniformizing parameter is in ideal
    // so switch to residue field first and compute standard basis over the residue field
    ring rShortcut = copyAndChangeCoefficientRing(r);
    nMapFunc takingResidues = n_SetMap(r->cf,rShortcut->cf);
    int k = idSize(inI);
    ideal inIShortcut = idInit(k);
    for (int i=0; i<k; i++)
      inIShortcut->m[i] = p_PermPoly(inI->m[i],NULL,r,rShortcut,takingResidues,NULL,0);
    ideal inJShortcut = gfanlib_kStd_wrapper(inIShortcut,rShortcut);

    // and lift the result back to the ring with valuation
    nMapFunc takingRepresentatives = n_SetMap(rShortcut->cf,r->cf);
    k = idSize(inJShortcut);
    inJ = idInit(k+1);
    inJ->m[0] = p_One(r);
    p_SetCoeff(inJ->m[0],nMap(uniformizingParameter,startingRing->cf,r->cf),r);
    for (int i=0; i<k; i++)
      inJ->m[i+1] = p_PermPoly(inJShortcut->m[i],NULL,rShortcut,r,takingRepresentatives,NULL,0);
  }
  return inJ;
}

ring tropicalStrategy::copyAndChangeOrderingWP(const ring r, const gfan::ZVector w, const gfan::ZVector v)
{
  // copy shortcutRing and change to desired ordering
  bool ok;
  ring s = rCopy0(r);
  int n = rVar(s);
  deleteOrdering(s);
  gfan::ZVector wAdjusted = adjustWeightForHomogeneity(w);
  gfan::ZVector vAdjusted = adjustWeightUnderHomogeneity(wAdjusted,v);
  s->order = (int*) omAlloc0(4*sizeof(int));
  s->block0 = (int*) omAlloc0(4*sizeof(int));
  s->block1 = (int*) omAlloc0(4*sizeof(int));
  s->wvhdl = (int**) omAlloc0(4*sizeof(int**));
  s->order[0] = ringorder_a;
  s->block0[0] = 1;
  s->block1[0] = n;
  s->wvhdl[0] = ZVectorToIntStar(adjustedInteriorPoint,ok);
  s->order[1] = ringorder_wp;
  s->block0[1] = 1;
  s->block1[1] = n;
  s->wvhdl[1] = ZVectorToIntStar(adjustedFacetNormal,ok);
  s->order[2] = ringorder_C;
  rComplete(s);
  rTest(s);

  return s;
}

ring tropicalStrategy::copyAndChangeOrderingDP(const ring r, const gfan::ZVector w, const gfan::ZVector v)
{
  // copy shortcutRing and change to desired ordering
  bool ok;
  ring s = rCopy0(r);
  int n = rVar(s);
  deleteOrdering(s);
  gfan::ZVector wAdjusted = adjustWeightForHomogeneity(w);
  gfan::ZVector vAdjusted = adjustWeightUnderHomogeneity(wAdjusted,v);
  s->order = (int*) omAlloc0(5*sizeof(int));
  s->block0 = (int*) omAlloc0(5*sizeof(int));
  s->block1 = (int*) omAlloc0(5*sizeof(int));
  s->wvhdl = (int**) omAlloc0(5*sizeof(int**));
  s->order[0] = ringorder_a;
  s->block0[0] = 1;
  s->block1[0] = n;
  s->wvhdl[0] = ZVectorToIntStar(adjustedInteriorPoint,ok);
  s->order[1] = ringorder_a;
  s->block0[1] = 1;
  s->block1[1] = n;
  s->wvhdl[1] = ZVectorToIntStar(adjustedFacetNormal,ok);
  s->order[2] = ringorder_dp;
  s->block0[2] = 1;
  s->block1[2] = n;
  s->order[3] = ringorder_C;
  rComplete(s);
  rTest(s);

  return s;
}


std::pair<ideal,ring> tropicalStrategy::getFlip(const ideal I, const ring r,
                                                const gfan::ZVector interiorPoint,
                                                const gfan::ZVector facetNormal) const
{
  assume(isValuationTrivial() || interiorPoint[0].sign()<0);
  assume(checkFirstGenerators(I,r));

  // get a generating system of the initial ideal
  // and compute a standard basis with respect to adjacent ordering
  ideal inIr = initial(I,r,interiorPoint);
  ring sAdjusted = copyAndChangeOrderingWP(r,interiorPoint,facetNormal);
  nMapFunc identity = n_SetMap(r->cf,sAdjusted->cf);
  int k = idSize(I); ideal inIs = idInit(k);
  for (int i=0; i<k; i++)
    inIsAdjusted->m[i] = p_PermPoly(inIr->m[i],NULL,r,sAdjusted,identity,NULL,0);
  ideal inJsAdjusted = getStdOfInitialIdeal(inIsAdjusted,sShortcut);

  // find witnesses of the new standard basis elements of the initial ideal
  // with the help of the old standard basis of the ideal
  k = idSize(inJsAdjusted);
  ideal inJr = idInit(k);
  identity = n_SetMap(sAdjusted->cf,r->cf);
  for (int i=0; i<k; i++)
    inJr->m[i] = p_PermPoly(inJsAdjusted->m[i],NULL,sAdjusted,r,identity,NULL,0);

  ideal Jr = getWitness(inJr,inIr,I,r);
  ring s = copyAndChangeOrderingDP(r,interiorPoint,facetNormal);
  identity = n_SetMap(r->cf,s->cf);
  for (int i=0; i<k; i++)
    Js->m[i] = p_PermPoly(inJsAdjusted->m[i],NULL,r,s,identity,NULL,0);

  return std::make_pair(Js,s);
}

#include <tropicalStrategy.h>
#include <adjustWeights.h>
#include <ppinitialReduction.h>
#include <ttinitialReduction.h>
#include <tropical.h>

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
    if ((i != -1) && (n_IsUnit(pGetCoeff(I->m[i]),currRing->cf)))
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

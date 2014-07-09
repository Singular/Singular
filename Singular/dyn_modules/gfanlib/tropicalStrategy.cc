#include <tropicalStrategy.h>
#include <adjustWeights.h>
#include <ppinitialReduction.h>
#include <ttinitialReduction.h>

// for various commands in dim(ideal I, ring r):
#include <kernel/ideals.h>
#include <kernel/stairc.h>

/***
 * Computes the dimension of an ideal I in ring r
 * Copied from jjDim in iparith.cc
 **/
static int dim(ideal I, ring r)
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
    d = scDimInt(vv, currQuotient);
    if (rField_is_Ring_Z(currRing) && (i==-1)) d++;
    idDelete(&vv);
    return d;
  }
  else
    d = scDimInt(I,currQuotient);
  if (origin != r)
    rChangeCurrRing(origin);
  return d;
}


/***
 * Initializes all relevant structures and information for the valued case,
 * i.e. computing a tropical variety over the rational numbers with p-adic valuation
 **/
tropicalStrategy::tropicalStrategy(ideal J, number q, ring s)
{
  /* assume that the ground field of the originalRing is Q */
  assume(rField_is_Q(s));
  originalRing = rCopy(s);

  /* replace Q with Z for the startingRing */
  startingRing = rCopy0(originalRing);
  nKillChar(startingRing->cf);
  startingRing->cf = nInitChar(n_Z,NULL);
  rComplete(startingRing);

  /* map p into the new coefficient domain */
  nMapFunc nMap = n_SetMap(originalRing->cf,startingRing->cf);
  uniformizingParameter = nMap(q,originalRing->cf,startingRing->cf);

  /* map J into the new polynomial ring */
  int k = idSize(J);
  startingIdeal = idInit(k);
  for (int i=0; i<k; i++)
    startingIdeal->m[i] = p_PermPoly(J->m[i],NULL,originalRing,startingRing,nMap,NULL,0);

  /* compute the dimension of the ideal in the original ring */
  dimensionOfIdeal = dim(J,s);

  /* set the flag that convex computations only occur in the lower half space to true */
  onlyLowerHalfSpace = true;

  /* set the function pointers to the appropiate functions */
  weightAdjustingAlgorithm1 = valued_adjustWeightForHomogeneity;
  weightAdjustingAlgorithm2 = valued_adjustWeightUnderHomogeneity;
  reductionAlgorithm = ppreduceInitially;
}

static bool doNothing(ideal /*I*/, ring /*r*/, number /*p*/)
{
  return false;
}

/***
 * Initializes all relevant structures and information for the valued case,
 * i.e. computing a tropical variety without any valuation.
 **/
tropicalStrategy::tropicalStrategy(ideal I, ring r)
{
  /* assume that the ground field of the originalRing is Q */
  assume(rField_is_Q(r));
  originalRing = rCopy(r);

  /* the starting ring is the originaRing */
  startingRing = rCopy(originalRing);

  /* the uniformizing parameter is non-existant*/
  uniformizingParameter = NULL;

  /* set the startingIdeal */
  startingIdeal = id_Copy(I,startingRing);

  /* compute the dimension of the ideal */
  dimensionOfIdeal = dim(startingIdeal,startingRing);

  /* set the flag that convex computations only occur in the lower half space to false */
  onlyLowerHalfSpace = false;

  /* set the function pointers to the appropiate functions */
  weightAdjustingAlgorithm1 = nonvalued_adjustWeightForHomogeneity;
  weightAdjustingAlgorithm2 = nonvalued_adjustWeightUnderHomogeneity;
  reductionAlgorithm = doNothing;
}

tropicalStrategy::tropicalStrategy(const tropicalStrategy& currentStrategy):
  originalRing(rCopy(currentStrategy.getOriginalRing())),
  startingRing(rCopy(currentStrategy.getStartingRing())),
  uniformizingParameter(n_Copy(currentStrategy.getUniformizingParameter(),startingRing->cf)),
  startingIdeal(id_Copy(currentStrategy.getStartingIdeal(),startingRing)),
  dimensionOfIdeal(currentStrategy.getDimensionOfIdeal()),
  onlyLowerHalfSpace(currentStrategy.restrictToLowerHalfSpace()),
  weightAdjustingAlgorithm1(currentStrategy.getWeightAdjustingAlgorithm1()),
  weightAdjustingAlgorithm2(currentStrategy.getWeightAdjustingAlgorithm2()),
  reductionAlgorithm(currentStrategy.getReductionAlgorithm())
{
  if (startingRing) rTest(startingRing);
  if (startingIdeal) id_Test(startingIdeal,startingRing);
}

tropicalStrategy::~tropicalStrategy()
{
  if (startingRing) rTest(startingRing);
  if (startingIdeal) id_Test(startingIdeal,startingRing);

  id_Delete(&startingIdeal,startingRing);
  n_Delete(&uniformizingParameter,startingRing->cf);
  rDelete(originalRing);
  rDelete(startingRing);
}

tropicalStrategy& tropicalStrategy::operator=(const tropicalStrategy& currentStrategy)
{
  originalRing = rCopy(currentStrategy.getOriginalRing());
  startingRing = rCopy(currentStrategy.getStartingRing());
  uniformizingParameter = n_Copy(currentStrategy.getUniformizingParameter(),startingRing->cf);
  startingIdeal = id_Copy(currentStrategy.getStartingIdeal(),startingRing);
  dimensionOfIdeal = currentStrategy.getDimensionOfIdeal();
  onlyLowerHalfSpace = currentStrategy.restrictToLowerHalfSpace();
  weightAdjustingAlgorithm1 = currentStrategy.getWeightAdjustingAlgorithm1();
  weightAdjustingAlgorithm2 = currentStrategy.getWeightAdjustingAlgorithm2();
  reductionAlgorithm = currentStrategy.getReductionAlgorithm();

  if (startingRing) rTest(startingRing);
  if (startingIdeal) id_Test(startingIdeal,startingRing);

  return *this;
}

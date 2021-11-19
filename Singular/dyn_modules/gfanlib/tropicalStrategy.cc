#include "tropicalStrategy.h"
#include "singularWishlist.h"
#include "adjustWeights.h"
#include "ppinitialReduction.h"
// #include "ttinitialReduction.h"
#include "tropical.h"
#include "std_wrapper.h"
#include "tropicalCurves.h"
#include "tropicalDebug.h"
#include "containsMonomial.h"


// for various commands in dim(ideal I, ring r):
#include "kernel/ideals.h"
#include "kernel/combinatorics/stairc.h"
#include "kernel/GBEngine/kstd1.h"
#include "misc/prime.h" // for isPrime(int i)

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
    if ((i != -1)
    #ifdef HAVE_RINGS
    && (n_IsUnit(p_GetCoeff(I->m[i],currRing->cf),currRing->cf))
    #endif
    )
      return -1;
    ideal vv = id_Head(I,currRing);
    if (i != -1) pDelete(&vv->m[i]);
    d = scDimInt(vv, currRing->qideal);
    if (rField_is_Z(currRing) && (i==-1)) d++;
    idDelete(&vv);
    return d;
  }
  else
    d = scDimInt(I,currRing->qideal);
  if (origin != r)
    rChangeCurrRing(origin);
  return d;
}

static void swapElements(ideal I, ideal J)
{
  assume(IDELEMS(I)==IDELEMS(J));

  for (int i=IDELEMS(I)-1; i>=0; i--)
  {
    poly cache = I->m[i];
    I->m[i] = J->m[i];
    J->m[i] = cache;
  }
}

static bool noExtraReduction(ideal I, ring r, number /*p*/)
{
  int n = rVar(r);
  gfan::ZVector allOnes(n);
  for (int i=0; i<n; i++)
    allOnes[i] = 1;
  ring rShortcut = rCopy0(r);

  rRingOrder_t* order = rShortcut->order;
  int* block0 = rShortcut->block0;
  int* block1 = rShortcut->block1;
  int** wvhdl = rShortcut->wvhdl;

  int h = rBlocks(r);
  rShortcut->order = (rRingOrder_t*) omAlloc0((h+2)*sizeof(rRingOrder_t));
  rShortcut->block0 = (int*) omAlloc0((h+2)*sizeof(int));
  rShortcut->block1 = (int*) omAlloc0((h+2)*sizeof(int));
  rShortcut->wvhdl = (int**) omAlloc0((h+2)*sizeof(int*));
  rShortcut->order[0] = ringorder_a;
  rShortcut->block0[0] = 1;
  rShortcut->block1[0] = n;
  bool overflow;
  rShortcut->wvhdl[0] = ZVectorToIntStar(allOnes,overflow);
  for (int i=1; i<=h; i++)
  {
    rShortcut->order[i] = order[i-1];
    rShortcut->block0[i] = block0[i-1];
    rShortcut->block1[i] = block1[i-1];
    rShortcut->wvhdl[i] = wvhdl[i-1];
  }
  //rShortcut->order[h+1] = (rRingOrder_t)0; -- done by omAlloc0
  //rShortcut->block0[h+1] = 0;
  //rShortcut->block1[h+1] = 0;
  //rShortcut->wvhdl[h+1] = NULL;

  rComplete(rShortcut);
  rTest(rShortcut);

  omFree(order);
  omFree(block0);
  omFree(block1);
  omFree(wvhdl);

  int k = IDELEMS(I);
  ideal IShortcut = idInit(k);
  nMapFunc intoShortcut = n_SetMap(r->cf,rShortcut->cf);
  for (int i=0; i<k; i++)
  {
    if(I->m[i]!=NULL)
    {
      IShortcut->m[i] = p_PermPoly(I->m[i],NULL,r,rShortcut,intoShortcut,NULL,0);
    }
  }

  ideal JShortcut = gfanlib_kStd_wrapper(IShortcut,rShortcut);

  ideal J = idInit(k);
  nMapFunc outofShortcut = n_SetMap(rShortcut->cf,r->cf);
  for (int i=0; i<k; i++)
    J->m[i] = p_PermPoly(JShortcut->m[i],NULL,rShortcut,r,outofShortcut,NULL,0);

  assume(areIdealsEqual(J,r,I,r));
  swapElements(I,J);
  id_Delete(&IShortcut,rShortcut);
  id_Delete(&JShortcut,rShortcut);
  rDelete(rShortcut);
  id_Delete(&J,r);
  return false;
}

/**
 * Initializes all relevant structures and information for the trivial valuation case,
 * i.e. computing a tropical variety without any valuation.
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
  assume(rField_is_Q(r) || rField_is_Zp(r) || rField_is_Z(r));
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

  s->order = (rRingOrder_t*) omAlloc0(3*sizeof(rRingOrder_t));
  s->block0 = (int*) omAlloc0(3*sizeof(int));
  s->block1 = (int*) omAlloc0(3*sizeof(int));
  s->wvhdl = (int**) omAlloc0(3*sizeof(int**));
  s->order[0] = ringorder_ws;
  s->block0[0] = 1;
  s->block1[0] = n;
  s->wvhdl[0] = (int*) omAlloc(n*sizeof(int));
  s->wvhdl[0][0] = 1;
  if (r->order[0] == ringorder_lp)
  {
    s->wvhdl[0][1] = 1;
  }
  else if (r->order[0] == ringorder_ls)
  {
    s->wvhdl[0][1] = -1;
  }
  else if (r->order[0] == ringorder_dp)
  {
    for (int i=1; i<n; i++)
      s->wvhdl[0][i] = -1;
  }
  else if (r->order[0] == ringorder_ds)
  {
    for (int i=1; i<n; i++)
      s->wvhdl[0][i] = 1;
  }
  else if (r->order[0] == ringorder_ws)
  {
    for (int i=1; i<n; i++)
      s->wvhdl[0][i] = r->wvhdl[0][i-1];
  }
  else
  {
    for (int i=1; i<n; i++)
      s->wvhdl[0][i] = -r->wvhdl[0][i-1];
  }
  s->order[1] = ringorder_C;

  rComplete(s);
  rTest(s);
  return s;
}

static ideal constructStartingIdeal(ideal originalIdeal, ring originalRing, number uniformizingParameter, ring startingRing)
{
  // construct p-t
  poly g = p_One(startingRing);
  p_SetCoeff(g,uniformizingParameter,startingRing);
  pNext(g) = p_One(startingRing);
  p_SetExp(pNext(g),1,1,startingRing);
  p_SetCoeff(pNext(g),n_Init(-1,startingRing->cf),startingRing);
  p_Setm(pNext(g),startingRing);
  ideal pt = idInit(1);
  pt->m[0] = g;

  // map originalIdeal from originalRing into startingRing
  int k = IDELEMS(originalIdeal);
  ideal J = idInit(k+1);
  nMapFunc nMap = n_SetMap(originalRing->cf,startingRing->cf);
  int n = rVar(originalRing);
  int* shiftByOne = (int*) omAlloc((n+1)*sizeof(int));
  for (int i=1; i<=n; i++)
    shiftByOne[i]=i+1;
  for (int i=0; i<k; i++)
  {
    if(originalIdeal->m[i]!=NULL)
    {
      J->m[i] = p_PermPoly(originalIdeal->m[i],shiftByOne,originalRing,startingRing,nMap,NULL,0);
    }
  }
  omFreeSize(shiftByOne,(n+1)*sizeof(int));

  ring origin = currRing;
  rChangeCurrRing(startingRing);
  ideal startingIdeal = kNF(pt,startingRing->qideal,J); // mathematically redundant,
  rChangeCurrRing(origin);                              // but helps with upcoming std computation
  // ideal startingIdeal = J; J = NULL;
  assume(startingIdeal->m[k]==NULL);
  startingIdeal->m[k] = pt->m[0];
  startingIdeal = gfanlib_kStd_wrapper(startingIdeal,startingRing);

  id_Delete(&J,startingRing);
  pt->m[0] = NULL;
  id_Delete(&pt,startingRing);
  return startingIdeal;
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

  /* replace Q with Z for the startingRing
   * and add an extra variable for tracking the uniformizing parameter */
  startingRing = constructStartingRing(originalRing);

  /* map the uniformizing parameter into the new coefficient domain */
  nMapFunc nMap = n_SetMap(originalRing->cf,startingRing->cf);
  uniformizingParameter = nMap(q,originalRing->cf,startingRing->cf);

  /* map the input ideal into the new polynomial ring */
  startingIdeal = constructStartingIdeal(J,s,uniformizingParameter,startingRing);
  reduce(startingIdeal,startingRing);

  linealitySpace = homogeneitySpace(startingIdeal,startingRing);

  /* construct the shorcut ring */
  shortcutRing = rCopy0(startingRing,FALSE); // do not copy q-ideal
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
  uniformizingParameter(NULL),
  shortcutRing(NULL),
  onlyLowerHalfSpace(currentStrategy.restrictToLowerHalfSpace()),
  weightAdjustingAlgorithm1(currentStrategy.weightAdjustingAlgorithm1),
  weightAdjustingAlgorithm2(currentStrategy.weightAdjustingAlgorithm2),
  extraReductionAlgorithm(currentStrategy.extraReductionAlgorithm)
{
  if (originalRing) rTest(originalRing);
  if (originalIdeal) id_Test(originalIdeal,originalRing);
  if (startingRing) rTest(startingRing);
  if (startingIdeal) id_Test(startingIdeal,startingRing);
  if (currentStrategy.getUniformizingParameter())
  {
    uniformizingParameter = n_Copy(currentStrategy.getUniformizingParameter(),startingRing->cf);
    n_Test(uniformizingParameter,startingRing->cf);
  }
  if (currentStrategy.getShortcutRing())
  {
    shortcutRing = rCopy(currentStrategy.getShortcutRing());
    rTest(shortcutRing);
  }
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
  if (startingIdeal) id_Delete(&startingIdeal,startingRing);
  if (uniformizingParameter) n_Delete(&uniformizingParameter,startingRing->cf);
  if (startingRing) rDelete(startingRing);
  if (shortcutRing) rDelete(shortcutRing);
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

void tropicalStrategy::putUniformizingBinomialInFront(ideal I, const ring r, const number q) const
{
  poly p = p_One(r);
  p_SetCoeff(p,q,r);
  poly t = p_One(r);
  p_SetExp(t,1,1,r);
  p_Setm(t,r);
  poly pt = p_Add_q(p,p_Neg(t,r),r);

  int k = IDELEMS(I);
  int l;
  for (l=0; l<k; l++)
  {
    if (p_EqualPolys(I->m[l],pt,r))
      break;
  }
  p_Delete(&pt,r);

  if (l>1)
  {
    pt = I->m[l];
    for (int i=l; i>0; i--)
      I->m[l] = I->m[l-1];
    I->m[0] = pt;
    pt = NULL;
  }
  return;
}

bool tropicalStrategy::reduce(ideal I, const ring r) const
{
  rTest(r);
  id_Test(I,r);

  nMapFunc identity = n_SetMap(startingRing->cf,r->cf);
  number p = NULL;
  if (uniformizingParameter!=NULL)
    p = identity(uniformizingParameter,startingRing->cf,r->cf);
  bool b = extraReductionAlgorithm(I,r,p);
  // putUniformizingBinomialInFront(I,r,p);
  if (p!=NULL) n_Delete(&p,r->cf);

  return b;
}

void tropicalStrategy::pReduce(ideal I, const ring r) const
{
  rTest(r);
  id_Test(I,r);

  if (isValuationTrivial())
    return;

  nMapFunc identity = n_SetMap(startingRing->cf,r->cf);
  number p = identity(uniformizingParameter,startingRing->cf,r->cf);
  ::pReduce(I,p,r);
  n_Delete(&p,r->cf);

  return;
}

ring tropicalStrategy::getShortcutRingPrependingWeight(const ring r, const gfan::ZVector &v) const
{
  ring rShortcut = rCopy0(r,FALSE); // do not copy q-ideal

  // save old ordering
  rRingOrder_t* order = rShortcut->order;
  int* block0 = rShortcut->block0;
  int* block1 = rShortcut->block1;
  int** wvhdl = rShortcut->wvhdl;

  // adjust weight and create new ordering
  gfan::ZVector w = adjustWeightForHomogeneity(v);
  int h = rBlocks(r); int n = rVar(r);
  rShortcut->order = (rRingOrder_t*) omAlloc0((h+2)*sizeof(rRingOrder_t));
  rShortcut->block0 = (int*) omAlloc0((h+2)*sizeof(int));
  rShortcut->block1 = (int*) omAlloc0((h+2)*sizeof(int));
  rShortcut->wvhdl = (int**) omAlloc0((h+2)*sizeof(int*));
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
  if (isValuationNonTrivial())
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

std::pair<poly,int> tropicalStrategy::checkInitialIdealForMonomial(const ideal I, const ring r, const gfan::ZVector &w) const
{
  // quick check whether I already contains an ideal
  int k = IDELEMS(I);
  for (int i=0; i<k; i++)
  {
    poly g = I->m[i];
    if (g!=NULL
    && pNext(g)==NULL
    && (isValuationTrivial() || n_IsOne(p_GetCoeff(g,r),r->cf)))
      return std::pair<poly,int>(g,i);
  }

  ring rShortcut;
  ideal inIShortcut;
  if (w.size()>0)
  {
    // if needed, prepend extra weight for homogeneity
    // switch to residue field if valuation is non trivial
    rShortcut = getShortcutRingPrependingWeight(r,w);

    // compute the initial ideal and map it into the constructed ring
    // if switched to residue field, remove possibly 0 elements
    ideal inI = initial(I,r,w);
    inIShortcut = idInit(k);
    nMapFunc intoShortcut = n_SetMap(r->cf,rShortcut->cf);
    for (int i=0; i<k; i++)
      inIShortcut->m[i] = p_PermPoly(inI->m[i],NULL,r,rShortcut,intoShortcut,NULL,0);
    if (isValuationNonTrivial())
      idSkipZeroes(inIShortcut);
    id_Delete(&inI,r);
  }
  else
  {
    rShortcut = r;
    inIShortcut = I;
  }

  gfan::ZCone C0 = homogeneitySpace(inIShortcut,rShortcut);
  gfan::ZCone pos = gfan::ZCone::positiveOrthant(C0.ambientDimension());
  gfan::ZCone C0pos = intersection(C0,pos);
  C0pos.canonicalize();
  gfan::ZVector wpos = C0pos.getRelativeInteriorPoint();
  assume(checkForNonPositiveEntries(wpos));

  // check initial ideal for monomial and
  // if it exsists, return a copy of the monomial in the input ring
  poly p = searchForMonomialViaStepwiseSaturation(inIShortcut,rShortcut,wpos);
  poly monomial = NULL;
  if (p!=NULL)
  {
    monomial=p_One(r);
    for (int i=1; i<=rVar(r); i++)
      p_SetExp(monomial,i,p_GetExp(p,i,rShortcut),r);
    p_Setm(monomial,r);
    p_Delete(&p,rShortcut);
  }


  if (w.size()>0)
  {
    // if needed, cleanup
    id_Delete(&inIShortcut,rShortcut);
    rDelete(rShortcut);
  }
  return std::pair<poly,int>(monomial,-1);
}

ring tropicalStrategy::copyAndChangeCoefficientRing(const ring r) const
{
  ring rShortcut = rCopy0(r,FALSE); // do not copy q-ideal
  nKillChar(rShortcut->cf);
  rShortcut->cf = nCopyCoeff(shortcutRing->cf);
  rComplete(rShortcut);
  rTest(rShortcut);
  return rShortcut;
}

ideal tropicalStrategy::computeWitness(const ideal inJ, const ideal inI, const ideal I, const ring r) const
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
    assume(IDELEMS(inI)==IDELEMS(I));
    int uni = findPositionOfUniformizingBinomial(I,r);
    assume(uni>=0);
    /**
     * change ground ring into finite field
     * and map the data into it
     */
    ring rShortcut = copyAndChangeCoefficientRing(r);

    int k = IDELEMS(inJ);
    int l = IDELEMS(I);
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
    matrix Q = mpNew(l,k);
    nMapFunc takingRepresentatives = n_SetMap(rShortcut->cf,r->cf);
    for (int ij=k*l-1; ij>=0; ij--)
      Q->m[ij] = p_PermPoly(QShortcut->m[ij],NULL,rShortcut,r,takingRepresentatives,NULL,0);

    nMapFunc identity = n_SetMap(startingRing->cf,r->cf);
    number p = identity(uniformizingParameter,startingRing->cf,r->cf);

    /**
     * Compute the normal forms
     */
    ideal J = idInit(k);
    for (int j=0; j<k; j++)
    {
      poly q0 = p_Copy(inJ->m[j],r);
      for (int i=0; i<l; i++)
      {
        poly qij = p_Copy(MATELEM(Q,i+1,j+1),r);
        poly inIi = p_Copy(inI->m[i],r);
        q0 = p_Add_q(q0,p_Neg(p_Mult_q(qij,inIi,r),r),r);
      }
      q0 = p_Div_nn(q0,p,r);
      poly q0g0 = p_Mult_q(q0,p_Copy(I->m[uni],r),r);
      // q0 = NULL;
      poly qigi = NULL;
      for (int i=0; i<l; i++)
      {
        poly qij = p_Copy(MATELEM(Q,i+1,j+1),r);
        // poly inIi = p_Copy(I->m[i],r);
        poly Ii = p_Copy(I->m[i],r);
        qigi = p_Add_q(qigi,p_Mult_q(qij,Ii,r),r);
      }
      J->m[j] = p_Add_q(q0g0,qigi,r);
    }

    id_Delete(&inIShortcut,rShortcut);
    id_Delete(&inJShortcut,rShortcut);
    mp_Delete(&QShortcut,rShortcut);
    rDelete(rShortcut);
    mp_Delete(&Q,r);
    n_Delete(&p,r->cf);
    return J;
  }
}

ideal tropicalStrategy::computeStdOfInitialIdeal(const ideal inI, const ring r) const
{
  // if valuation trivial, then compute std as usual
  if (isValuationTrivial())
    return gfanlib_kStd_wrapper(inI,r);

  // if valuation non-trivial, then uniformizing parameter is in ideal
  // so switch to residue field first and compute standard basis over the residue field
  ring rShortcut = copyAndChangeCoefficientRing(r);
  nMapFunc takingResidues = n_SetMap(r->cf,rShortcut->cf);
  int k = IDELEMS(inI);
  ideal inIShortcut = idInit(k);
  for (int i=0; i<k; i++)
    inIShortcut->m[i] = p_PermPoly(inI->m[i],NULL,r,rShortcut,takingResidues,NULL,0);
  ideal inJShortcut = gfanlib_kStd_wrapper(inIShortcut,rShortcut);

  // and lift the result back to the ring with valuation
  nMapFunc takingRepresentatives = n_SetMap(rShortcut->cf,r->cf);
  k = IDELEMS(inJShortcut);
  ideal inJ = idInit(k+1);
  inJ->m[0] = p_One(r);
  nMapFunc identity = n_SetMap(startingRing->cf,r->cf);
  p_SetCoeff(inJ->m[0],identity(uniformizingParameter,startingRing->cf,r->cf),r);
  for (int i=0; i<k; i++)
    inJ->m[i+1] = p_PermPoly(inJShortcut->m[i],NULL,rShortcut,r,takingRepresentatives,NULL,0);

  id_Delete(&inJShortcut,rShortcut);
  id_Delete(&inIShortcut,rShortcut);
  rDelete(rShortcut);
  return inJ;
}

ideal tropicalStrategy::computeLift(const ideal inJs, const ring s, const ideal inIr, const ideal Ir, const ring r) const
{
  int k = IDELEMS(inJs);
  ideal inJr = idInit(k);
  nMapFunc identitysr = n_SetMap(s->cf,r->cf);
  for (int i=0; i<k; i++)
    inJr->m[i] = p_PermPoly(inJs->m[i],NULL,s,r,identitysr,NULL,0);

  ideal Jr = computeWitness(inJr,inIr,Ir,r);
  nMapFunc identityrs = n_SetMap(r->cf,s->cf);
  ideal Js = idInit(k);
  for (int i=0; i<k; i++)
    Js->m[i] = p_PermPoly(Jr->m[i],NULL,r,s,identityrs,NULL,0);
  return Js;
}

ring tropicalStrategy::copyAndChangeOrderingWP(const ring r, const gfan::ZVector &w, const gfan::ZVector &v) const
{
  // copy shortcutRing and change to desired ordering
  bool ok;
  ring s = rCopy0(r,FALSE,FALSE);
  int n = rVar(s);
  gfan::ZVector wAdjusted = adjustWeightForHomogeneity(w);
  gfan::ZVector vAdjusted = adjustWeightUnderHomogeneity(v,wAdjusted);
  s->order = (rRingOrder_t*) omAlloc0(5*sizeof(rRingOrder_t));
  s->block0 = (int*) omAlloc0(5*sizeof(int));
  s->block1 = (int*) omAlloc0(5*sizeof(int));
  s->wvhdl = (int**) omAlloc0(5*sizeof(int**));
  s->order[0] = ringorder_a;
  s->block0[0] = 1;
  s->block1[0] = n;
  s->wvhdl[0] = ZVectorToIntStar(wAdjusted,ok);
  s->order[1] = ringorder_a;
  s->block0[1] = 1;
  s->block1[1] = n;
  s->wvhdl[1] = ZVectorToIntStar(vAdjusted,ok);
  s->order[2] = ringorder_lp;
  s->block0[2] = 1;
  s->block1[2] = n;
  s->order[3] = ringorder_C;
  rComplete(s);
  rTest(s);

  return s;
}

ring tropicalStrategy::copyAndChangeOrderingLS(const ring r, const gfan::ZVector &w, const gfan::ZVector &v) const
{
  // copy shortcutRing and change to desired ordering
  bool ok;
  ring s = rCopy0(r,FALSE,FALSE);
  int n = rVar(s);
  s->order = (rRingOrder_t*) omAlloc0(5*sizeof(rRingOrder_t));
  s->block0 = (int*) omAlloc0(5*sizeof(int));
  s->block1 = (int*) omAlloc0(5*sizeof(int));
  s->wvhdl = (int**) omAlloc0(5*sizeof(int**));
  s->order[0] = ringorder_a;
  s->block0[0] = 1;
  s->block1[0] = n;
  s->wvhdl[0] = ZVectorToIntStar(w,ok);
  s->order[1] = ringorder_a;
  s->block0[1] = 1;
  s->block1[1] = n;
  s->wvhdl[1] = ZVectorToIntStar(v,ok);
  s->order[2] = ringorder_lp;
  s->block0[2] = 1;
  s->block1[2] = n;
  s->order[3] = ringorder_C;
  rComplete(s);
  rTest(s);

  return s;
}

std::pair<ideal,ring> tropicalStrategy::computeFlip(const ideal Ir, const ring r,
                                                    const gfan::ZVector &interiorPoint,
                                                    const gfan::ZVector &facetNormal) const
{
  assume(isValuationTrivial() || interiorPoint[0].sign()<0);
  assume(checkForUniformizingBinomial(Ir,r));
  assume(checkWeightVector(Ir,r,interiorPoint));

  // get a generating system of the initial ideal
  // and compute a standard basis with respect to adjacent ordering
  ideal inIr = initial(Ir,r,interiorPoint);
  ring sAdjusted = copyAndChangeOrderingWP(r,interiorPoint,facetNormal);
  nMapFunc identity = n_SetMap(r->cf,sAdjusted->cf);
  int k = IDELEMS(Ir);
  ideal inIsAdjusted = idInit(k);
  for (int i=0; i<k; i++)
    inIsAdjusted->m[i] = p_PermPoly(inIr->m[i],NULL,r,sAdjusted,identity,NULL,0);
  ideal inJsAdjusted = computeStdOfInitialIdeal(inIsAdjusted,sAdjusted);

  // find witnesses of the new standard basis elements of the initial ideal
  // with the help of the old standard basis of the ideal
  k = IDELEMS(inJsAdjusted);
  ideal inJr = idInit(k);
  identity = n_SetMap(sAdjusted->cf,r->cf);
  for (int i=0; i<k; i++)
    inJr->m[i] = p_PermPoly(inJsAdjusted->m[i],NULL,sAdjusted,r,identity,NULL,0);

  ideal Jr = computeWitness(inJr,inIr,Ir,r);
  ring s = copyAndChangeOrderingLS(r,interiorPoint,facetNormal);
  identity = n_SetMap(r->cf,s->cf);
  ideal Js = idInit(k);
  for (int i=0; i<k; i++)
    Js->m[i] = p_PermPoly(Jr->m[i],NULL,r,s,identity,NULL,0);

  reduce(Js,s);
  assume(areIdealsEqual(Js,s,Ir,r));
  assume(isValuationTrivial() || isOrderingLocalInT(s));
  assume(checkWeightVector(Js,s,interiorPoint));

  // cleanup
  id_Delete(&inIsAdjusted,sAdjusted);
  id_Delete(&inJsAdjusted,sAdjusted);
  rDelete(sAdjusted);
  id_Delete(&inIr,r);
  id_Delete(&Jr,r);
  id_Delete(&inJr,r);

  assume(isValuationTrivial() || isOrderingLocalInT(s));
  return std::make_pair(Js,s);
}


bool tropicalStrategy::checkForUniformizingBinomial(const ideal I, const ring r) const
{
  // if the valuation is trivial,
  // then there is no special condition the first generator has to fullfill
  if (isValuationTrivial())
    return true;

  // if the valuation is non-trivial then checks if the first generator is p-t
  nMapFunc identity = n_SetMap(startingRing->cf,r->cf);
  poly p = p_One(r);
  p_SetCoeff(p,identity(uniformizingParameter,startingRing->cf,r->cf),r);
  poly t = p_One(r);
  p_SetExp(t,1,1,r);
  p_Setm(t,r);
  poly pt = p_Add_q(p,p_Neg(t,r),r);

  for (int i=0; i<IDELEMS(I); i++)
  {
    if (p_EqualPolys(I->m[i],pt,r))
    {
      p_Delete(&pt,r);
      return true;
    }
  }
  p_Delete(&pt,r);
  return false;
}

int tropicalStrategy::findPositionOfUniformizingBinomial(const ideal I, const ring r) const
{
  assume(isValuationNonTrivial());

  // if the valuation is non-trivial then checks if the first generator is p-t
  nMapFunc identity = n_SetMap(startingRing->cf,r->cf);
  poly p = p_One(r);
  p_SetCoeff(p,identity(uniformizingParameter,startingRing->cf,r->cf),r);
  poly t = p_One(r);
  p_SetExp(t,1,1,r);
  p_Setm(t,r);
  poly pt = p_Add_q(p,p_Neg(t,r),r);

  for (int i=0; i<IDELEMS(I); i++)
  {
    if (p_EqualPolys(I->m[i],pt,r))
    {
      p_Delete(&pt,r);
      return i;
    }
  }
  p_Delete(&pt,r);
  return -1;
}

bool tropicalStrategy::checkForUniformizingParameter(const ideal inI, const ring r) const
{
  // if the valuation is trivial,
  // then there is no special condition the first generator has to fullfill
  if (isValuationTrivial())
    return true;

  // if the valuation is non-trivial then checks if the first generator is p
  if (inI->m[0]==NULL)
    return false;
  nMapFunc identity = n_SetMap(startingRing->cf,r->cf);
  poly p = p_One(r);
  p_SetCoeff(p,identity(uniformizingParameter,startingRing->cf,r->cf),r);

  for (int i=0; i<IDELEMS(inI); i++)
  {
    if (p_EqualPolys(inI->m[i],p,r))
    {
      p_Delete(&p,r);
      return true;
    }
  }
  p_Delete(&p,r);
  return false;
}

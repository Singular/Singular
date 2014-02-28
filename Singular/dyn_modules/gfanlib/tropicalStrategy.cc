#include <tropicalStrategy.h>
#include <tropicalVarietyOfPolynomials.h>
#include <adjustWeights.h>
#include <ppinitialReduction.h>
#include <ttinitialReduction.h>

tropicalStrategy valuedCase, nonValuedCase;

tropicalStrategy initializeValuedCase()
{
  tropicalStrategy currentCase;
  currentCase.tropicalVarietyOfPolynomial = lowerTropicalVarietyOfPolynomial;
  currentCase.adjustWeightForHomogeneity = valued_adjustWeightForHomogeneity;
  currentCase.adjustWeightUnderHomogeneity = valued_adjustWeightUnderHomogeneity;
  currentCase.reduce = ppreduceInitially;
  return currentCase;
}

static bool doNothing(ideal I, ring r)
{
  return false;
}

tropicalStrategy initializeNonValuedCase()
{
  tropicalStrategy currentCase;
  currentCase.tropicalVarietyOfPolynomial = fullTropicalVarietyOfPolynomial;
  currentCase.adjustWeightForHomogeneity = nonvalued_adjustWeightForHomogeneity;
  currentCase.adjustWeightUnderHomogeneity = nonvalued_adjustWeightUnderHomogeneity;
  currentCase.reduce = doNothing;
  return currentCase;
}

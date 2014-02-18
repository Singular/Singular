#include <tropicalStrategy.h>
#include <adjustWeights.h>

tropicalStrategy valuedCase, nonValuedCase;

tropicalStrategy initializeValuedCase()
{
  tropicalStrategy currentCase;
  currentCase.adjustWeightForHomogeneity = valued_adjustWeightForHomogeneity;
  currentCase.adjustWeightUnderHomogeneity = valued_adjustWeightUnderHomogeneity;
  return currentCase;
}

void initializeNonValuedCase()
{
  tropicalStrategy currentCase;
  currentCase.adjustWeightForHomogeneity = nonvalued_adjustWeightForHomogeneity;
  currentCase.adjustWeightUnderHomogeneity = nonvalued_adjustWeightUnderHomogeneity;
  return currentCase;
}

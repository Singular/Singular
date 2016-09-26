#include <vector>

#include <kernel/GBEngine/kstd1.h>
#include <kernel/polys.h>
#include <kernel/ideals.h>

#include <Singular/dyn_modules/customstd/customstd.h>

ideal gfanlib_kStd_wrapper(ideal I, ring r, tHomog h=testHomog)
{
  ring origin = currRing;
  if (origin != r)
    rChangeCurrRing(r);

  intvec* nullVector = NULL;
  ideal stdI = kStd(I,currRing->qideal,h,&nullVector); // there is still a memory leak here!!!
  id_DelDiv(stdI,currRing);
  idSkipZeroes(stdI);
  if (nullVector!=NULL) delete nullVector;

  if (origin != r)
    rChangeCurrRing(origin);

  return stdI;
}

ideal gfanlib_satStd_wrapper(ideal I, ring r, tHomog h=testHomog)
{
  ring origin = currRing;
  if (origin != r)
    rChangeCurrRing(r);

  int n = rVar(currRing);
  satstdSaturatingVariables = std::vector<int>(n);
  for (int i=n-1; i>=0; i--)
    satstdSaturatingVariables[i] = i+1;

  ideal stdI = kStd(I,currRing->qideal,h,NULL,NULL,0,0,NULL,sat_vars_sp);
  id_DelDiv(stdI,currRing);
  idSkipZeroes(stdI);

  if (origin != r)
    rChangeCurrRing(origin);

  return stdI;
}

ideal gfanlib_monomialabortStd_wrapper(ideal I, ring r, tHomog h=testHomog)
{
  ring origin = currRing;
  if (origin != r)
    rChangeCurrRing(r);

  ideal stdI = kStd(I,currRing->qideal,h,NULL,NULL,0,0,NULL,abort_if_monomial_sp);
  id_DelDiv(stdI,currRing);
  idSkipZeroes(stdI);

  if (origin != r)
    rChangeCurrRing(origin);

  return stdI;
}

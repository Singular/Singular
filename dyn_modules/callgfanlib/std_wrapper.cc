#include <kernel/kstd1.h>
#include <kernel/polys.h>

ideal gfanlib_kStd_wrapper(ideal I, ring r, tHomog h=testHomog)
{
  ring origin = currRing;
  if (origin != r)
    rChangeCurrRing(r);

  intvec* nullVector = NULL;
  ideal stdI = kStd(I,currQuotient,h,&nullVector);

  if (origin != r)
    rChangeCurrRing(origin);

  return stdI;
}

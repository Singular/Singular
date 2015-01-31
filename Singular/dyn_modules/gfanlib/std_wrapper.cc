#include <kernel/GBEngine/kstd1.h>
#include <kernel/polys.h>
#include <kernel/ideals.h>

ideal gfanlib_kStd_wrapper(ideal I, ring r, tHomog h=testHomog)
{
  ring origin = currRing;
  if (origin != r)
    rChangeCurrRing(r);

  intvec* nullVector = NULL;
  ideal stdI = kStd(I,currRing->qideal,h,&nullVector); // there is still a memory leak here!!!
  id_DelDiv(stdI,currRing);
  idSkipZeroes(stdI);

  if (origin != r)
    rChangeCurrRing(origin);

  return stdI;
}

#include <libpolys/polys/simpleideals.h>

namespace tropical
{

  extendedIdeal::extendedIdeal():
    I(NULL),
    r(NULL),
    isSB(false),
    isRed(false)
  {
  }

  extendedIdeal::extendedIdeal(const ideal I0, const ring r0, const bool isSB0, const bool isRed0):
    I(id_Copy(I0,r0)),
    r(rCopy(r0)),
    isSB(isSB0),
    isRed(isRed0)
  {
  }

  extendedIdeal::extendedIdeal(const extendedIdeal I0):
    I(id_Copy(I0.I)),
    r(rCopy(I0.r)),
    isSB(I0.isSB),
    isRed(I0.isRed)
  {
  }

  extendedIdeal::~extendedIdeal()
  {
    id_Delete(&I,r);
    rDelete(r);
  }


}

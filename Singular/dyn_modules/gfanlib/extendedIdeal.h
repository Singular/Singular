#ifndef EXTENDED_IDEAL_H
#define EXTENDED_IDEAL_H

#include <libpolys/polys/simpleideals.h>

namespace tropical
{

  class extendedIdeal
  {
    ideal I;
    ring r;
    bool isSB;
    bool isRed;

  public:

    extendedIdeal();
    extendedIdeal(const ideal I0, const ring r0, const bool isSB0=false, const bool isRed0=false);
    extendedIdeal(const extendedIdeal I0);
    ~extendedIdeal();

    flip(const gfan::ZVector w, const gfan::ZVector v);
  }
}

#endif

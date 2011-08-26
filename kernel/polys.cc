#include "mod2.h"

#include <omalloc/omalloc.h>
#include <misc/auxiliary.h>

#include "polys.h"

ring currRing;

void rChangeCurrRing(ring r)
{
  currRing = r;
}

#include "mod2.h"

#include <omalloc/omalloc.h>
#include <misc/auxiliary.h>
#include <misc/options.h>

#include "polys.h"

ring  currRing = NULL;
ideal currQuotient = NULL;

void rChangeCurrRing(ring r)
{
   currRing = r;
   currQuotient = r->qideal;
   test &= ~ TEST_RINGDEP_OPTS;
   test |= r->options;
}

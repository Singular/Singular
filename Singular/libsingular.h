#ifndef LIBSINGULAR__H
#define LIBSINGULAR__H

#include <math.h> // Why this?

#include <misc/auxiliary.h>
#include <kernel/structs.h>
#include <kernel/polys.h>
#include <kernel/longrat.h>
#include <coeffs/numbers.h>
#include <kernel/febase.h>
#include <polys/monomials/ring.h>
#include <omalloc/omalloc.h>
#include <polys/clapsing.h>
#include <polys/monomials/maps.h>
#include <kernel/kutil.h>
#include <kernel/kstd1.h>
#include <kernel/tgb.h>
#include <polys/sparsmat.h>

#include <Singular/subexpr.h>
#include <Singular/tok.h>
#include <Singular/grammar.h>
#include <Singular/ipid.h>
#include <Singular/ipshell.h>
#include <Singular/attrib.h>
#include <misc/options.h>

void siInit(char *);

/* we need this function in Sage*/
number nr2mMapZp(number from); // TODO: replace with something else...


#endif // #ifndef LIBSINGULAR__H


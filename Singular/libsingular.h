#ifndef LIBSINGULAR__H
#define LIBSINGULAR__H

#include <math.h> // Why this?

#include <misc/auxiliary.h>
#include <kernel/structs.h>
#include <kernel/polys.h>
#include <coeffs/numbers.h>
#include <coeffs/longrat.h>
#include <kernel/oswrapper/feread.h>
#include <polys/monomials/ring.h>
#include <omalloc/omalloc.h>
#include <polys/clapsing.h>
#include <polys/monomials/maps.h>
#include <kernel/GBEngine/kutil.h>
#include <kernel/GBEngine/kstd1.h>
#include <kernel/GBEngine/tgb.h>
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


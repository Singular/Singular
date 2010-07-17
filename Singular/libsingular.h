#ifndef SINGULAR__H
#define SINGULAR__H

#include <math.h>
#include <Singular/singular/mod2.h>
#include <kernel/structs.h>
#include <kernel/polys.h>
#include <kernel/longrat.h>
#include <Singular/singular/longalg.h>
#include <kernel/numbers.h>
#include <kernel/febase.h>
#include <kernel/ring.h>
#include <omalloc.h>
#include <Singular/singular/clapsing.h>
#include <kernel/maps.h>
#include <Singular/singular/kutil.h>
#include <kernel/kstd1.h>
#include <Singular/singular/tgb.h>
#include <Singular/singular/sparsmat.h>
#include <Singular/singular/rintegers.h>
#include <Singular/singular/rmodulo2m.h>
#include <Singular/singular/rmodulon.h>

#include <Singular/singular/subexpr.h>
#include <Singular/singular/tok.h>
#include <Singular/singular/grammar.h>
#include <Singular/singular/ipid.h>
#include <Singular/singular/ipshell.h>
#include <Singular/singular/attrib.h>
#include <kernel/options.h>

int siInit(char *);

/* we need this function in Sage*/
number nr2mMapZp(number from);

#endif //SINGULAR__H


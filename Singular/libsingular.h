#ifndef LIBSINGULAR__H
#define LIBSINGULAR__H

#include "singularconfig.h"
#include "kernel/mod2.h"
#include "kernel/structs.h"
#include "kernel/polys.h"
#include "coeffs/numbers.h"
#include "kernel/oswrapper/feread.h"
#include "polys/monomials/ring.h"
#include "omalloc/omalloc.h"
#include "polys/clapsing.h"
#include "polys/monomials/maps.h"
#include "polys/shiftop.h"
#include "kernel/GBEngine/kutil.h"
#include "kernel/GBEngine/kstd1.h"
#include "kernel/GBEngine/tgb.h"
#include "kernel/maps/gen_maps.h"
#include "kernel/preimage.h"
#include "polys/sparsmat.h"

#include "Singular/mod_lib.h"
#include "Singular/subexpr.h"
#include "Singular/tok.h"
#include "Singular/grammar.h"
#include "Singular/ipid.h"
#include "Singular/ipshell.h"
#include "Singular/attrib.h"
#include "Singular/blackbox.h"
#include "misc/options.h"

void siInit(char *);



#endif // #ifndef LIBSINGULAR__H


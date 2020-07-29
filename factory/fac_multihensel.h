#ifndef FAC_MULTIHENSEL_H
#define FAC_MULTIHENSEL_H

#include "config.h"


#include "cf_defs.h"
#include "cf_eval.h"
#include "cf_binom.h"
#include "fac_util.h"
#include "fac_iterfor.h"
#include "fac_multihensel.h"
#include "cf_iter.h"

bool
Hensel ( const CanonicalForm & U, CFArray & G, const CFArray & lcG, const Evaluation & A, const modpk & bound, const Variable & /*x*/ );
#endif

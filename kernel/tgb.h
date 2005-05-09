#ifndef TGB_H
#define TGB_H
//#define OM_CHECK 3
//#define OM_TRACK 5
#include "mod2.h"
#include <omalloc.h>
#include "p_polys.h"

#include "ideals.h"
#include "ring.h"
#include "febase.h"
#include "structs.h"
#include "polys.h"
#include "stdlib.h"


#include "kutil.h"
#include "kInline.cc"
#include "kstd1.h"
#include "kbuckets.h"

ideal t_rep_gb(ring r,ideal arg_I, BOOLEAN F4_mode=FALSE);
#endif

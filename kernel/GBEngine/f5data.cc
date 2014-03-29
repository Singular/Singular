//! \file f5data.h
/****************************************
*  Computer Algebra System SINGULAR     *
****************************************/
/*
* ABSTRACT: lpolynomial definition
*/
#ifdef HAVE_CONFIG_H
#include "singularconfig.h"
#endif /* HAVE_CONFIG_H */
#include <kernel/mod2.h>

#ifdef HAVE_F5
#include <kernel/GBEngine/kutil.h>
#include <kernel/structs.h>
#include <omalloc/omalloc.h>
#include <kernel/polys.h>
#include <polys/monomials/p_polys.h>
#include <kernel/ideals.h>
#include <kernel/febase.h>
#include <kernel/GBEngine/kstd1.h>
#include <kernel/GBEngine/khstd.h>
#include <polys/kbuckets.h>
#include <polys/weight.h>
#include <misc/intvec.h>
#include <kernel/polys.h>
#include <kernel/GBEngine/f5gb.h>
#include <kernel/GBEngine/f5data.h>
#include <kernel/GBEngine/f5lists.h>
/*
=====================
everything is inlined
=====================
*/
#endif

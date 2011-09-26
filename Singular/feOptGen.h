#ifndef FEOPTSGEN_H
#define FEOPTSGEN_H
/****************************************
*  Computer Algebra System SINGULAR     *
****************************************/
/* $Id$ */
/*
* ABSTRACT: Declarations for working with Options
*/

#include <kernel/fegetopt.h>

extern const char SHORT_OPTS_STRING[];
#define LONG_OPTION_RETURN 13

#include <Singular/FeOptTab.h>
typedef enum {FE_OPT_UNDEF}  feOptIndex;

#endif /*  FEOPTSGEN_H */

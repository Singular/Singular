/* emacs edit mode for this file is -*- C++ -*- */
/* $Id: cf_gmp.h,v 1.3 2002-11-04 17:06:24 bricken Exp $ */

#ifndef INCL_CF_GMP_H
#define INCL_CF_GMP_H

#include <config.h>

extern "C" {
#ifdef __cplusplus
#undef __cplusplus
#include <gmp.h>
#define __cplusplus
#else
#include <gmp.h>
#endif
}

#endif /* ! INCL_CF_GMP_H */

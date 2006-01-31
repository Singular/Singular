/* $Id: si_gmp.h,v 1.3 2006-01-31 16:32:33 Singular Exp $ */

#ifndef INCL_CF_GMP_H
#define INCL_CF_GMP_H

#if defined(__cplusplus) && defined(__GNUC__)
extern "C" {
#undef __cplusplus
#include <gmp.h>
#define __cplusplus 1
}
#else
#include <gmp.h>
#endif

#endif /* ! INCL_CF_GMP_H */

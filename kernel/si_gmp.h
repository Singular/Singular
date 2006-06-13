/* $Id: si_gmp.h,v 1.6 2006-06-13 16:57:12 Singular Exp $ */
/* $Id: si_gmp.h,v 1.6 2006-06-13 16:57:12 Singular Exp $ */

#ifndef INCL_CF_GMP_H
#define INCL_CF_GMP_H

#if defined(__cplusplus) && defined(__GNUC__)
extern "C" {
#undef __cplusplus
#include <gmp.h>
}
#define __cplusplus 1
#else
#include <gmp.h>
#endif

#endif /* ! INCL_CF_GMP_H */

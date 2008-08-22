/* $Id: si_gmp.h,v 1.7 2008-08-22 09:03:21 Singular Exp $ */

#ifndef INCL_CF_GMP_H
#define INCL_CF_GMP_H
#ifdef HAVE_FACTORY
#include <cf_gmp.h>
#else
#if defined(__cplusplus) && defined(__GNUC__)
extern "C" {
#undef __cplusplus
#include <gmp.h>
}
#define __cplusplus 1
#else
#include <gmp.h>
#endif

#endif 
#endif /* ! INCL_CF_GMP_H */

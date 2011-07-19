/* $Id$ */

#ifndef INCL_CF_GMP_H
#define INCL_CF_GMP_H
#ifdef HAVE_FACTORY
#  include <factory/cf_gmp.h>
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

/* emacs edit mode for this file is -*- C++ -*- */
/* $Id: si_gmp.h,v 1.4 2006-05-02 16:50:05 Singular Exp $ */

#ifndef INCL_CF_GMP_H
#define INCL_CF_GMP_H

#ifdef __cplusplus
extern "C" {
#undef __cplusplus
#define _WCHAR_T_DECLARED
#include <gmp.h>
#define __cplusplus 1
}
#else
#include <gmp.h>
#endif

#endif /* ! INCL_CF_GMP_H */

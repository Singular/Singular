
#ifndef NTL_IsFinite__H
#define NTL_IsFinite__H

#include <NTL/config.h>

#if (defined(__cplusplus) && !defined(NTL_CXX_ONLY))
extern "C" {
#endif

long _ntl_IsFinite(double *p);
/* This forces a double into memory, and tests if it is "normal";
   that means, not NaN, not +/- infinity, not denormalized, etc.
   Forcing into memory is sometimes necessary on machines 
   with "extended" double precision registers (e.g., Intel x86s)
   to force the standard IEEE format. */

void _ntl_ForceToMem(double *p);
/* This is do-nothing routine that has the effect of forcing
   a double into memory (see comment above). */

#if (defined(__cplusplus) && !defined(NTL_CXX_ONLY))
}
#endif

#endif


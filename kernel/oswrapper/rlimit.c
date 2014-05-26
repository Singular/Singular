/****************************************
 * Computer Algebra System SINGULAR     *
 ****************************************/
/***************************************************************
 * File:    rlimit.c
 * Purpose: set resource limits
 ***************************************************************/

#include "rlimit.h"

#include <stdint.h>
#include <sys/resource.h>

/* raise the maximum number of processes (or threads),
 * return  0 on success,
 *        -1 on error
 */
int raise_rlimit_nproc()
{
#ifdef RLIMIT_NPROC
  struct rlimit nproc;
  getrlimit(RLIMIT_NPROC, &nproc);
  if (nproc.rlim_cur == RLIM_INFINITY
      || (nproc.rlim_max != RLIM_INFINITY && nproc.rlim_cur >= nproc.rlim_max))
  {
    return(-1);
  }
  if (nproc.rlim_cur < 512)
  {
    nproc.rlim_cur = 512;
  }
  if ((nproc.rlim_max == RLIM_INFINITY || 2*nproc.rlim_cur <= nproc.rlim_max)
      && nproc.rlim_cur < 65536)
  {
    nproc.rlim_cur = 2*nproc.rlim_cur;
  }
  else
  {
    nproc.rlim_cur = nproc.rlim_max;
  }
  int res = setrlimit(RLIMIT_NPROC, &nproc);
  return(res);
#else
  return(-1);
#endif
}

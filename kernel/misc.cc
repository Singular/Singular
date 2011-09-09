/****************************************
*  Computer Algebra System SINGULAR     *
****************************************/
/*
* ABSTRACT:
*/

#include <kernel/mod2.h>

#ifdef HAVE_FACTORY
#define SI_DONT_HAVE_GLOBAL_VARS
#  include <factory/factory.h>
/* libfac version strings */
#ifdef HAVE_LIBFAC
  extern const char * libfac_version;
  extern const char * libfac_date;
#endif // #ifdef HAVE_LIBFAC
#endif

#include <coeffs/si_gmp.h>
#include <omalloc/omalloc.h>

#ifdef HAVE_MPSR
#include <MP_Config.h>
#endif

/* init bins */

omBin char_ptr_bin = omGetSpecBin(sizeof(char*));

/*0 implementation*/

/*2
* the renice routine for very large jobs
* works only on unix machines,
* testet on : linux, HP 9.0
*
*#include <sys/times.h>
*#include <sys/resource.h>
*extern "C" int setpriority(int,int,int);
*void very_nice()
*{
*#ifndef NO_SETPRIORITY
*  setpriority(PRIO_PROCESS,0,19);
*#endif
*  sleep(10);
*}
*/

/*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*/
/*
 *
 *  A prime modulus multiplicative linear congruential
 *  generator (PMMLCG), or "Lehmer generator".
 *  Implementation directly derived from the article:
 *
 *        S. K. Park and K. W. Miller
 *        Random Number Generators: Good Ones are Hard to Find
 *        CACM vol 31, #10. Oct. 1988. pp 1192-1201.
 *
 *  Using the following multiplier and modulus, we obtain a
 *  generator which:
 *
 *        1)  Has a full period: 1 to 2^31 - 2.
 *        2)  Is testably "random" (see the article).
 *        3)  Has a known implementation by E. L. Schrage.
 */


#define  A        16807        /*  A "good" multiplier          */
#define  M   2147483647        /*  Modulus: 2^31 - 1          */
#define  Q       127773        /*  M / A                  */
#define  R         2836        /*  M % A                  */


int siSeed = 1;


int siRand()
{
  siSeed = A * (siSeed % Q) - R * (siSeed / Q);

  if ( siSeed < 0 )
    siSeed += M;

  return( siSeed );
}


/* -*-c++-*- */
/*******************************************************************
 *  Computer Algebra System SINGULAR
 *
 *  mod2.h: Main configuration file for Singular
 *
 *******************************************************************/
#ifndef MOD2_H
#define MOD2_H

/* please include singularconfig.h exclusively via <kernel/mod2.h> and before any other header */
# include "singularconfig.h"
# include "misc/auxiliary.h"

#if defined(HAVE_MPFR_H) && defined(HAVE_LIBMPFR)
#define HAVE_SVD
#endif

#define SINGULAR_MAJOR_VERSION 4
#define SINGULAR_MINOR_VERSION 3
#define SINGULAR_SUB_VERSION 2
#define SINGULAR_PATCHLEVEL 10
#define S_ROOT_DIR ""

/*******************************************************************
 * Defines which are not set by configure
 ******************************************************************/

/*defines, which should be set by configure */
#define HAVE_GETTIMEOFDAY 1
#define TIME_WITH_SYS_TIME 1
#define HAVE_SYS_TIME_H 1
/* Default value for timer resolution in ticks per second */
/* set to 10 for resolution of tenth of a second, etc */
#define TIMER_RESOLUTION 1

/* Undefine to disable the quote/eval of expressions */
#define SIQ 1

/* Undefine to disable Gerhard's and Wilfried's fast and dirty std computations */
#define FAST_AND_DIRTY

/* eigenvalues */
#define HAVE_EIGENVAL 1

/* Gauss-Manin system */
#define HAVE_GMS 1

/* include simpleipc/semaphore code, link against librt/libpthread */
#define HAVE_SIMPLEIPC 1


/* linear algebra extensions from pcv.h/pcv.cc */
#define HAVE_PCV 1

/* procedures to compute with units */
#define HAVE_UNITS

/* Define to use scanner when loading libraries */
#define HAVE_LIBPARSER

/* define for parallel processes with shared memory */
#ifndef __CCYGWIN__
#if !defined(__MACH__) || !defined(__APPLE__)
#define HAVE_VSPACE 1
#endif
#endif

/*#define PROFILING*/
#ifdef PROFILING
#define PROFILER ,0,0
#else
#define PROFILER
#endif

/*******************************************************************
 * Evaluate the set defines
 ******************************************************************/
/* Spectrum needs GMP */
#define HAVE_SPECTRUM 1

#if SIZEOF_VOIDP == 8
/* SIZEOF_LONG == SIZEOF_VOIDP is guaranteed by configure */
#define ALIGN_8
#endif

#define SINGULAR_VERSION ((SINGULAR_MAJOR_VERSION*1000 + SINGULAR_MINOR_VERSION*100 + SINGULAR_SUB_VERSION*10)+SINGULAR_PATCHLEVEL)

/*******************************************************************
 * Miscellaneous Defines
 ******************************************************************/
#ifndef HAVE_LIBPARSER
#  undef YYLPDEBUG
#else
#  define YYLPDEBUG 1
#endif

#ifndef FALSE
#define FALSE       0
#endif

#ifndef TRUE
#define TRUE        1
#endif

#ifndef NULL
#define NULL        (0)
#endif

#ifndef SEEK_END
#define SEEK_END 2
#endif

#ifndef SEEK_SET
#define SEEK_SET 0
#endif

#ifdef __cplusplus
extern "C" {
#endif
void  m2_end(int i);
#ifdef __cplusplus
}
#endif

static inline void HALT(void) { m2_end(2);}

/* define OLD_RES for res/sres/mres(i,j,k) */
#undef OLD_RES

/* the maximal ascii length of an int number + 1 = 11 for 32 bit int */
/* #define MAX_INT_LEN 11 */


/* define LINKAGE to "extern C" if compiling for shared libs */
#ifndef LINKAGE
#if defined(PIC)
#define LINKAGE extern "C"
#else
#define LINKAGE
#endif
#endif


/*******************************************************************
 * DEBUG OPTIONS
 * -- only significant for for compiling without -DSING_NDEBUG
 * -- you better know what your are doing, if you touch this
 ******************************************************************/
#ifndef SING_NDEBUG

/* undefine to enable inline */
#define NO_INLINE

/* undef PDEBUG to disable checks of polys

 define PDEBUG to
  0 for enabling pTest
  1 plus tests in Level 1 poly routines (operations on monomials)
  2 plus tests in Level 2 poly routines (operations on single exponents)
 -- see also polys.h for more info

 NOTE: you can set the value of PDEBUG on a per-file basis, before
       including mod2.h, provided ! PDEBUG is defined in mod2.h E.g.:

       #define PDEBUG 2
       #include "mod2.h"
       ...

       makes sure that all poly operations in your file are done with
       PDEBUG == 2
 To break after an error occurred, set a debugger breakpoint on
 dErrorBreak.
*/
#ifndef PDEBUG
#define PDEBUG 0
#endif

/* define MDEBUG to enable memory checks */
#define MDEBUG 0

#ifdef MDEBUG
/* If ! defined(OM_NDEBUG) and (defined(OM_TRACK) or defined(OM_CHECK)
   then omDebug routines are used for memory allocation/free:

   The omDebug routines are controlled by the values of OM_TRACK, OM_CHECK
   and OM_KEEP.  There meaning is roughly as follows:
   OM_TRACK: stored with address                                 : extra space
     0     : no additional info is stored                        : 0
     1     : file:line of location where address was allocated   : 1 word
     2     : plus backtrace of stack where address was allocated : 6 words
     3     : plus size/bin info and front-, and back padding     : 9 words
     4     : plus file:line of location where address was freed  : 10 words
     5     : plus backtrace of stack where address was allocated : 15 words
   OM_CHECK: checks done
     0     : no checks
     1     : constant-time checks: i.e. addr checks only
     2     : plus linear-time checks and constant related bin check
     3     : plus quadratic-time checks and linear-time related bin checks and
             constant time all memory checks
     4     : and so on
     ==> for OM_CHECK >= 3 it gets rather slow
   OM_KEEP:  determines whether addresses are really freed  (
     0     : addresses are really freed
     1     : addresses are only marked as free and not really freed.

   OM_CHECK, OM_TRACK, and OM_KEEP can be set on a per-file basis
   (as can OM_NDEBUG),  e.g.:
     #define OM_CHECK 3
     #define OM_TRACK 5
     #define OM_KEEP  1
     #include "mod2.h"
     #include "omalloc/omalloc.h"
   ensures that all memory allocs/free in this file are done with
   OM_CHECK==3 and OM_TRACK==5, and that all addresses allocated/freed
   in this file are only marked as free and never really freed.

   To set OM_CHECK, OM_TRACK and OM_KEEP under dynamic scope, set
   om_Opts.MinCheck, om_Opts.MinTrack to the respectiv values and
   om_Opts.Keep to the number of addresses which are kept before they are
   actually freed. E.g.:
     int check=om_Opts.MinCheck, track=om_Opts.MinTrack, keep= m_OPts.Keep;
     om_Opts.MinCheck = 3; om_Opts.MinTrack = 5; omOpts.Keep = LONG_MAX;
     ExternalRoutine();
     om_Opts.MinCheck = check; omOpts.MinTrack = track; omOpts.Keep = keep;
   ensures that all calls omDebug routines  occurring during the computation of
   ExternalRoutine() are done with OM_CHECK==3 and OM_TRACK==5, and
   calls to omFree only mark addresses as free and not really free them.

   Furthermore, the value of OM_SING_KEEP (resp. om_Opts.Keep) specifies
   how many addresses are kept before they are actually freed, independently
   of the value of OM_KEEP.

   Some tips on possible values of OM_TRACK, OM_CHECK, OM_KEEP:
   + To find out about an address that has been freed twice, first locate the
     file(s) where the error occurred, and then at the beginning of these files:
       #define OM_CHECK 3
       #define OM_TRACK 5
       #define OM_KEEP  1
       #include "mod2.h"
       #include "omalloc/omalloc.h"
     Under dynamic scope, do (e.g., from within the debugger):
       om_Opts.MinCheck = 3; om_Opts.MinTrack = 5; omOpts.Keep = LONG_MAX;
   + to find out where "memory corruption" occurred, increase value of
     OM_CHECK - the higher this value is, the more consistency checks are
     done (However a value > 3 checks the entire memory each time an omalloc
     routine is used!)

   Some more tips on the usage of omalloc:
   + omAlloc*, omRealloc*, omFree*, omCheck* omDebug* omTest* rotuines
     assume that sizes are > 0 and pointers are != NULL
   + omalloc*, omrealloc*, omfree* omcheck*, omdebug* omtest* routines allow
     NULL pointers and sizes == 0
   + You can safely use any free/realloc routine in combination with any alloc
     routine (including the debug versions): E.g., an address allocated with
     omAllocBin can be freed with omfree, or an address allocated with
     om(Debug)Alloc can be freed with omfree, or omFree, or omFreeSize, etc.
     However, keep in mind that the efficiency decreases from
     Bin over Size to General routines (i.e., omFreeBin is more efficient than
     omFreeSize which is more efficient than omFree, likewise with the alloc
     routines).
   + if OM_CHECK is undefined or 0, then all omCheck routines do nothing
   + if OM_CHECK and OM_TRACK are both undefined (or 0), or if OM_NDEBUG is
     defined, then the "real" alloc/realloc/free macros are used, and all
     omTest, omDebug and omCheck routines are undefined
   + to break after an omError occurred within a debugger,
     set a breakpoint on dErrorBreak
   + to do checks from within the debugger, or to do checks with explicit
     check level, use omTest routines.
*/

/* by default, store alloc info and file/line where addr was freed */
#ifndef OM_TRACK
#define OM_TRACK 4
#endif
/* only do constant-time memory checks */
#ifndef OM_CHECK
#define OM_CHECK 1
#endif
/* Do actually free memory:
   (be careful: if this is set, memory is never really freed,
    but only marked as free) */
#ifndef OM_KEEP
#define OM_KEEP 0
#endif
/* but only after you have freed 1000 more addresses
   (this is actually independent of the value of OM_KEEP and used
   to initialize om_Opts.Keep) */
#ifndef OM_SING_KEEP
#define OM_SING_KEEP 1000
#endif

#endif /* MDEBUG */


/* undef KDEBUG for check of data during std computations
 *
 * define KDEBUG to
 * 0 for basic tests
 * 1 for tests in kSpoly
 * NOTE: You can locally enable tests in kspoly by setting the
 *       define at the beginning of kspoly.cc
 */
#define KDEBUG 0

/* define LDEBUG checking numbers, undefine otherwise */
#define LDEBUG
/* define RDEBUG checking rings (together with TRACE=9) */
#define RDEBUG
/* define TEST for non time critical tests, undefine otherwise */
#define TEST

/* #define PAGE_TEST */

/* define YYDEBUG 1 for debugging bison texts, 0 otherwise */
#define YYDEBUG 1

/* define SPECTRUM_DEBUG and SPECTRUM_PRINT for debugging the spectrum code */
/* define SPECTRUM_IOSTREAM to use C++ iostream for error messages          */

/* #define SPECTRUM_DEBUG */
/* #define SPECTRUM_PRINT */
#undef  SPECTRUM_IOSTREAM

#ifdef  SPECTRUM_DEBUG
#define MULTICNT_DEBUG
#define GMPRAT_DEBUG
#define KMATRIX_DEBUG
#define SPLIST_DEBUG
#define NPOLYGON_DEBUG
#define SEMIC_DEBUG
#endif

#ifdef  SPECTRUM_PRINT
#define MULTICNT_PRINT
#define GMPRAT_PRINT
#define KMATRIX_PRINT
#define SPLIST_PRINT
#define NPOLYGON_PRINT
#define SEMIC_PRINT
#endif

#ifdef  SPECTRUM_IOSTREAM
#define MULTICNT_IOSTREAM
#define GMPRAT_IOSTREAM
#define KMATRIX_IOSTREAM
#define SPLIST_IOSTREAM
#define NPOLYGON_IOSTREAM
#define SEMIC_IOSTREAM
#endif


#else /* not SING_NDEBUG **************************************************** */

#define NO_PDEBUG

/* define YYDEBUG 1 for debugging bison texts, 0 otherwise */
#define YYDEBUG 0

#endif /* not SING_NDEBUG */

/*******************************************************************
 *
 * assume(x) -- a handy macro for assumptions
 *
 ******************************************************************/

#ifdef __cplusplus
extern "C" {
#endif
/* routine which is used to report the error/
* returns 0 */
extern int dReportError(const char* fmt, ...);
/* within a debugger, set a breakpoint on dErrorBreak
* which is called after the error has been reported */
extern void dErrorBreak(void);
#ifdef __cplusplus
}
#endif

#ifdef SING_NDEBUG
#define assume(x) do {} while (0)
#define r_assume(x) do {} while (0)
#else /* SING_NDEBUG */

#define assume_violation(s,f,l) \
  dReportError("assume violation at %s:%d condition: %s", f,l,s)

#define assume(x)   _assume(x, __FILE__, __LINE__)
#define r_assume(x) _r_assume(x, __FILE__, __LINE__)

#define _assume(x, f, l)                        \
do                                              \
{                                               \
  if (! (x))                                    \
  {                                             \
    assume_violation(#x, f, l);                 \
  }                                             \
}                                               \
while (0)

#define _r_assume(x, f, l)                      \
do                                              \
{                                               \
  if (! (x))                                    \
  {                                             \
    assume_violation(#x, f, l);                 \
    return 0;                                   \
  }                                             \
}                                               \
while (0)
#endif /* SING_NDEBUG */

/* do have RDEBUG, unless we are doing the very real thing */
#ifndef SING_NDEBUG
#ifndef RDEBUG
#define RDEBUG
#endif
#endif

#if SIZEOF_VOIDP == 8
#ifndef OM_CHECK
#define OM_CHECK 0
#endif
#endif

/* If we're not using GNU C, elide __attribute__ */
#ifndef __GNUC__
#  define  __attribute__(x)  /*NOTHING*/
#endif

#define STRINGIFY(name) #name
#define EXPANDED_STRINGIFY(name) STRINGIFY(name)

#endif /* MOD2_H  */

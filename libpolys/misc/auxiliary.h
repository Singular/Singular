/*****************************************************************************\
 * Computer Algebra System SINGULAR
\*****************************************************************************/
/** @file auxiliary.h
 *
 * All the auxiliary stuff.
 *
 * ABSTRACT: we shall put here everything that does not have its own place.
 *
 * @author Oleksandr Motsak
 *
 *
 **/
/*****************************************************************************/

#ifndef MISC_AUXILIARY_H
#define MISC_AUXILIARY_H

/* please include libpolysconfig.h exclusively via <misc/auxiliary.h> and before any other header */
#include "libpolysconfig.h"

#include "../factory/globaldefs.h"

/* the following cunstruct is to make it painless to add -DHAVE_NUMSTATS to CPPFLAGS for configure */
#ifndef HAVE_NUMSTATS
/* #define HAVE_NUMSTATS */
#undef HAVE_NUMSTATS
#endif /* HAVE_NUMSTATS */

// ---------------- Singular standard types etc.
/* SI_INTEGER_VARIANT: 1: from longrat.cc
 *                     2: GMP (in rintegers.cc)
 *                     3: CF (in rintegers.cc) */
#define SI_INTEGER_VARIANT 2

/* SI_BIGINT_VARIANT: 1: from longrat.cc
 *                    2: given by SI_INTEGER_VARIANT */
#define SI_BIGINT_VARIANT 1

/* preparation for versio 4.2.0: cpoly, cnumber, cmatrix (4_2) */
#undef SINGULAR_4_2

#ifndef SIZEOF_LONG

#include "misc/mylimits.h"

#ifndef LONG_BIT
#if ULONG_MAX == 0xffffffffUL
#define LONG_BIT 32
#elif ULONG_MAX == 0xffffffffffffffffULL
#define LONG_BIT 64
#else
#error "Unexpected max for unsigned long"
#endif
#endif



#define SIZEOF_LONG (LONG_BIT/CHAR_BIT)
// another option for SIZEOF_LONG: use omConfig included in <omalloc/omalloc.h>...

#endif

#include <sys/types.h>
#if SIZEOF_LONG == 4
typedef long long int64;
#elif SIZEOF_LONG == 8
typedef long int64;
#else
#error "Unexpected SIZEOF_LONG"
#endif


#ifndef CHAR_BIT
#define CHAR_BIT (8)
#endif /*ifndef CHAR_BIT*/


#ifndef BIT_SIZEOF_LONG
#define BIT_SIZEOF_LONG ((CHAR_BIT)*(SIZEOF_LONG))
#endif /*ifndef BIT_SIZEOF_LONG*/




#if (SIZEOF_LONG == 8)
typedef int BOOLEAN;
/* testet on x86_64, gcc 3.4.6: 2 % */
/* testet on IA64, gcc 3.4.6: 1 % */
#else
/* testet on athlon, gcc 2.95.4: 1 % */
typedef short BOOLEAN;
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

#ifndef NULLp
#define NULLp        ((void*)NULL)
#endif

#ifndef ABS
static inline int ABS(int v)
{
  int const mask = v >> (sizeof(int) * CHAR_BIT - 1);
  return ((v + mask) ^ mask);
}
#endif

// stolen from:
// https://graphics.stanford.edu/~seander/bithacks.html#IntegerLog
static inline int SI_LOG2(int v)
{
  const unsigned int b[] = {0x2, 0xC, 0xF0, 0xFF00, 0xFFFF0000};
  const unsigned int S[] = {1, 2, 4, 8, 16};

  unsigned int r = 0; // result of log2(v) will go here
  if (v & b[4]) { v >>= S[4]; r |= S[4]; }
  if (v & b[3]) { v >>= S[3]; r |= S[3]; }
  if (v & b[2]) { v >>= S[2]; r |= S[2]; }
  if (v & b[1]) { v >>= S[1]; r |= S[1]; }
  if (v & b[0]) { v >>= S[0]; r |= S[0]; }
  return (int)r;
}

typedef void* ADDRESS;

#define loop for(;;)

#if defined(__cplusplus)
static inline int si_max(const int a, const int b)  { return (a>b) ? a : b; }
static inline int si_min(const int a, const int b)  { return (a<b) ? a : b; }
static inline long si_max(const long a, const long b)  { return (a>b) ? a : b; }
static inline unsigned long si_max(const unsigned long a, const unsigned long b)  { return (a>b) ? a : b; }
static inline long si_min(const long a, const long b)  { return (a<b) ? a : b; }
static inline unsigned long si_min(const unsigned long a, const unsigned long b)  { return (a<b) ? a : b; }
#else
#define si_max(A,B) ((A) > (B) ? (A) : (B))
#define si_min(A,B) ((A) < (B) ? (A) : (B))
#endif

#define SSI_BASE 16

// ---------------- defines which depend on the settings above

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
//////////////////////////////////////////// #define MDEBUG 0

#ifdef MDEBUG
/* If ! defined(OM_NDEBUG) and (defined(OM_TRACK) or defined(OM_CHECK)
   then omDebug routines are used for memory allocation/free:

   The omDebug routines are controlled by the values of OM_TRACK, OM_CHECK
   and OM_KEEP.  There meaning is roughly as follows:
   OM_TRACK: strored with address                              : extra space
     0     : no additional info is stored                      : 0
     1     : file:line of location where address was allocated : 1 word
     2     : plus backtrace of stack where adress was allocated: 6 words
     3     : plus size/bin info and front-, and back padding   : 9 words
     4     : plus file:line of location where adress was freed : 10 words
     5     : plus backtrace of stack where adress was allocated: 15 words
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
   ensures that all calls omDebug routines  occuring during the computation of
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
       #include "kernel/mod2.h"
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
     omAllocBin can be freed with omfree, or an adress allocated with
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

/* define YYDEBUG 1 for debugging bison texts, 0 otherwise */
#define YYDEBUG 1

#endif
/* end of debugging option (ifndef SING_NDEBUG) */



#ifdef _DEBUG
#      define FORCE_INLINE inline
#else
#ifdef SING_NDEBUG
#if   defined(_MSC_VER)
#      define FORCE_INLINE __forceinline
#elif defined(__GNUC__) && __GNUC__ > 3
#      define FORCE_INLINE inline __attribute__ ((always_inline))
#else
#      define FORCE_INLINE inline
#endif
#else
#      define FORCE_INLINE inline
#endif
/* SING_NDEBUG */
#endif
/* _DEBUG */


#define DO_PRAGMA(x) _Pragma (#x)
#define TODO(who, msg) DO_PRAGMA(message ("TODO [for " #who "]: " #msg))



#if defined(__GNUC__) && defined(__GNUC_MINOR__)
#define _GNUC_PREREQ(maj, min) ((__GNUC__ << 16) + __GNUC_MINOR__ >= ((maj) << 16) + (min))
#else
#define _GNUC_PREREQ(maj, min) 0
#endif

#if _GNUC_PREREQ(3,3) && defined(__ELF__)
#define FORCE_INTERNAL __attribute__ ((visibility ("internal")))
#else
#define FORCE_INTERNAL
#endif

#if _GNUC_PREREQ(3,3)
#define FORCE_DEPRECATED __attribute__ ((deprecated))
#else
#define FORCE_DEPRECATED
#endif

#ifdef __cplusplus
# define  BEGIN_CDECL extern "C" {
# define  END_CDECL   }
#else
# define  BEGIN_CDECL
# define  END_CDECL
#endif

#ifdef __cplusplus
// hack to workaround warnings when casting void pointers
// retrieved from dlsym? to function pointers.
// see: http://trac.osgeo.org/qgis/ticket/234, http://www.trilithium.com/johan/2004/12/problem-with-dlsym/
template<typename A, typename B>
inline B cast_A_to_B( A a )
{
  union
  {
    A a;
    B b;
  } u;

  u.a = a;
  return u.b;
}

template<typename A>
inline void* cast_A_to_vptr( A a )
{
  return cast_A_to_B<A, void*>(a);
}


template<typename A>
inline A cast_vptr_to_A( void * p )
{
  return cast_A_to_B<void*, A>(p);
}
#endif


#ifdef __GNUC__
#define LIKELY(X)   (__builtin_expect(!!(X), 1))
#define UNLIKELY(X) (__builtin_expect(!!(X), 0))
#else
#define LIKELY(X)   (X)
#define UNLIKELY(X) (X)
#endif

#endif
/* MISC_AUXILIARY_H */

